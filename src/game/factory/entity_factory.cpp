#include "game/factory/entity_factory.hpp"
#include "engine/utils/math.hpp"
#include "engine/resource/resource_manager.hpp"
#include "engine/component/transform_component.hpp"
#include "engine/component/sprite_component.hpp"
#include "engine/component/animation_component.hpp"
#include "engine/component/velocity_component.hpp"
#include "engine/component/render_component.hpp"
#include "engine/component/audio_component.hpp"
#include "game/factory/blueprint_manager.hpp"
#include "game/data/entity_blueprint.hpp"
#include "game/defs/tags.hpp"
#include "game/component/stats_component.hpp"
#include "game/component/enemy_component.hpp"
#include "game/component/class_name_component.hpp"
#include <entt/entity/registry.hpp>
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

using namespace entt::literals;

namespace game::factory {

EntityFactory::EntityFactory(entt::registry& registry, 
    BlueprintManager& blueprint_manager)
    : registry_{registry}
    , blueprint_manager_{blueprint_manager} {
}

entt::entity EntityFactory::create_enemy_unit(entt::id_type class_id, const sf::Vector2f& position, int target_waypoint_id, int level, int rarity) {
    auto entity = registry_.create();
    const auto& blueprint = blueprint_manager_.get_enemy_class_blueprint(class_id);
    // --- 添加组件 ---
    // 添加Transform组件
    add_transform_component(entity, position);

    // 添加Sprite组件
    add_sprite_component(entity, blueprint.sprite_);

    // 添加Animation组件 (默认动画为“walk”)
    add_animation_component(entity, blueprint.animations_, blueprint.sprite_, "walk"_hs);

    // 添加Audio组件
    add_audio_component(entity, blueprint.sounds_);

    // 添加Stats组件
    add_stats_component(entity, blueprint.stats_, level, rarity);
    
    // 添加Enemy组件
    add_enemy_component(entity, blueprint.enemy_, target_waypoint_id);

    // 补充其他必要组件
    registry_.emplace<game::component::ClassNameComponent>(entity, class_id, blueprint.display_info_.name_);
    registry_.emplace<engine::component::RenderComponent>(entity);  // 使用默认主图层
    
    // 未来可添加其它组件

    return entity;
}

void EntityFactory::add_transform_component(entt::entity entity, const sf::Vector2f& position, const sf::Vector2f& scale, sf::Angle rotation) {
    registry_.emplace<engine::component::TransformComponent>(entity, position, scale, rotation);
}

void EntityFactory::add_sprite_component(entt::entity entity, const data::SpriteBlueprint& sprite, const bool is_flipped) {
    auto* resource_manager = registry_.ctx().get<engine::resource::ResourceManager*>();
    auto texture = resource_manager->load_texture(entt::hashed_string(sprite.path_.c_str()), sprite.path_);
    // 手绘风格的动态单位（玩家、敌人）需要平滑过滤，防止锯齿
    texture->setSmooth(true);
    registry_.emplace<engine::component::SpriteComponent>(entity, *texture);
    
    auto& transform = registry_.get<engine::component::TransformComponent>(entity);
    transform.origin_ = sprite.origin_;
    // 如果翻转，设置 x 负缩放
    if (is_flipped) {
        transform.scale_ = {-transform.scale_.x, transform.scale_.y};
    }
        
    // 如果图片朝左就添加FaceLeftTag
    if (!sprite.face_right_) {
        registry_.emplace<game::defs::FaceLeftTag>(entity);
    }
}

void EntityFactory::add_animation_component(entt::entity entity, 
        const std::unordered_map<entt::id_type, data::AnimationBlueprint>& animation_blueprints,
        const data::SpriteBlueprint& sprite_blueprint,
        entt::id_type default_animation_id) {
    // 先创建map容器  
    std::unordered_map<entt::id_type, engine::component::Animation> animations;
    // 针对每一个动画，
    for (const auto& [anim_id, anim_blueprint] : animation_blueprints) {
        // 创建动画帧容器
        std::vector<engine::component::AnimationFrame> frames;
        // 依次读取蓝图中的每一个帧索引
        for (const auto& frame_index : anim_blueprint.frames_) {
            sf::IntRect source_rect = static_cast<sf::IntRect>(sprite_blueprint.src_rect_);
            // 通过索引计算每一帧的源矩形区域
            source_rect.position.x += frame_index * source_rect.size.x;
            source_rect.position.y += anim_blueprint.row_ * source_rect.size.y;
            // 创建动画帧并插入动画帧容器
            frames.emplace_back(source_rect, anim_blueprint.frame_duration_);
        }
        // 将创建好的动画帧容器插入动画map容器
        animations.emplace(anim_id, engine::component::Animation(std::move(frames)));
    }
    // 通过动画map容器创建动画组件
    registry_.emplace<engine::component::AnimationComponent>(entity, std::move(animations), default_animation_id);
}

void EntityFactory::add_stats_component(entt::entity entity, const data::StatsBlueprint& stats, int level, int rarity) {
    // 计算等级和稀有度对属性的影响 (未来可改成数据驱动方便调整)
    auto hp = engine::utils::stat_modify(stats.hp_, level, rarity);
    auto atk = engine::utils::stat_modify(stats.atk_, level, rarity);
    auto def = engine::utils::stat_modify(stats.def_, level, rarity);
    
    registry_.emplace_or_replace<game::component::StatsComponent>(entity, 
        hp, 
        hp, 
        atk, 
        def, 
        stats.range_,
        stats.atk_interval_,
        sf::seconds(0.f),
        level,
        rarity);
}

void EntityFactory::add_enemy_component(entt::entity entity, const data::EnemyBlueprint& enemy, int target_waypoint_id) {
    registry_.emplace<game::component::EnemyComponent>(entity, target_waypoint_id, enemy.speed_);
    registry_.emplace<engine::component::VelocityComponent>(entity, sf::Vector2f(0.f, 0.f));
    if (enemy.ranged_) {    // 添加远程或近战标签备用
        registry_.emplace<game::defs::RangedUnitTag>(entity);
    } else {
        registry_.emplace<game::defs::MeleeUnitTag>(entity);
    }
}

void EntityFactory::add_audio_component(entt::entity entity, const data::SoundBlueprint& sounds) {
    if (sounds.sounds_.empty()) return;
    // 将sounds_中的键值对转换为audio_map中的键值对
    std::unordered_map<entt::id_type, entt::id_type> audio_map;
    for (const auto& [sound_key, sound_id] : sounds.sounds_) {
        audio_map.emplace(sound_key, sound_id);
    }
    registry_.emplace<engine::component::AudioComponent>(entity, std::move(audio_map));
}

}   // namespace game::factory