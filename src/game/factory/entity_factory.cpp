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
#include "game/component/player_component.hpp"
#include "game/component/enemy_component.hpp"
#include "game/component/stats_component.hpp"
#include "game/component/class_name_component.hpp"
#include "game/component/blocker_component.hpp"
#include "game/component/projectile_component.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <entt/entity/registry.hpp>
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::factory {

EntityFactory::EntityFactory(entt::registry& registry, 
    BlueprintManager& blueprint_manager)
    : registry_{registry}
    , blueprint_manager_{blueprint_manager} {
}

entt::entity EntityFactory::create_player_unit(entt::id_type class_id, const sf::Vector2f& position, int level, int rarity) {
    auto entity = registry_.create();
    const auto& blueprint = blueprint_manager_.get_player_class_blueprint(class_id);
    // --- 添加组件 ---
    // 添加Transform组件
    add_transform_component(entity, position);

    // 添加Sprite组件
    add_sprite_component(entity, blueprint.sprite_);

    // 添加Animation组件
    add_animation_component(entity, blueprint.animations_, blueprint.sprite_, "idle"_hs);

    // 添加Audio组件
    add_audio_component(entity, blueprint.sounds_);

    // 添加Stats组件
    add_stats_component(entity, blueprint.stats_, level, rarity);
    
    // 添加Player组件
    add_player_component(entity, blueprint.player_, rarity);

    // 添加ProjectileID组件
    add_projectile_id_component(entity, blueprint.projectile_id_);

    // 补充其他必要组件
    registry_.emplace<game::component::ClassNameComponent>(entity, class_id, blueprint.display_info_.name_);
    registry_.emplace<engine::component::RenderComponent>(entity);
    registry_.emplace<game::defs::HasHealthBarTag>(entity);

    // 未来可添加其它组件

    return entity;
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

    // 添加ProjectileID组件
    add_projectile_id_component(entity, blueprint.projectile_id_);

    // 补充其他必要组件
    registry_.emplace<game::component::ClassNameComponent>(entity, class_id, blueprint.display_info_.name_);
    registry_.emplace<engine::component::RenderComponent>(entity);  // 使用默认主图层
    registry_.emplace<game::defs::HasHealthBarTag>(entity);
    
    // 未来可添加其它组件

    return entity;
}

entt::entity EntityFactory::create_projectile(entt::id_type id, const sf::Vector2f& start_position, const sf::Vector2f& target_position, entt::entity target, float damage) {
    // 创建投射物实体
    auto entity = registry_.create();
    const auto& blueprint = blueprint_manager_.get_projectile_blueprint(id);
    // --- 依次添加必要组件 ---
    // 添加ProjectileComponent
    registry_.emplace<game::component::ProjectileComponent>(entity, 
        target, 
        damage,
        start_position,
        target_position, 
        start_position,
        blueprint.arc_height_, 
        blueprint.total_flight_time_,
        sf::Time::Zero);
    // 先添加TransformComponent（因为add_sprite_component内部依赖TransformComponent）
    add_transform_component(entity, start_position);
    // 添加SpriteComponent
    add_sprite_component(entity, blueprint.sprite_);
    // 添加AudioComponent
    add_audio_component(entity, blueprint.sounds_);
    // 添加RenderComponent(让投射物位于主图层+1，即可以遮住角色)
    registry_.emplace<engine::component::RenderComponent>(entity, engine::component::RenderComponent::MAIN_LAYER + 1);
    return entity;
}
void EntityFactory::add_transform_component(entt::entity entity, const sf::Vector2f& position, const sf::Vector2f& scale, sf::Angle rotation) {
    registry_.emplace<engine::component::TransformComponent>(entity, position, scale, rotation);
}

entt::entity EntityFactory::create_enemy_dead_effect(entt::id_type class_id, const sf::Vector2f& position, const bool is_flipped) {
    auto entity = registry_.create();
    const auto& blueprint = blueprint_manager_.get_enemy_class_blueprint(class_id);
    // 添加Transform组件
    add_transform_component(entity, position);

    // 添加Sprite组件
    add_sprite_component(entity, blueprint.sprite_, is_flipped);

    // 添加Animation组件(死亡动画名称为“damage”)
    add_one_animation_component(entity, blueprint.animations_.at("damage"_hs), blueprint.sprite_, "damage"_hs);

    // 补充其他必要组件
    registry_.emplace<engine::component::RenderComponent>(entity);
    registry_.emplace<game::defs::OneShotRemoveTag>(entity);
    return entity;
}

void EntityFactory::add_sprite_component(entt::entity entity, const data::SpriteBlueprint& sprite, const bool is_flipped) {
    auto* resource_manager = registry_.ctx().get<engine::resource::ResourceManager*>();
    auto texture = resource_manager->load_texture(entt::hashed_string(sprite.path_.c_str()), sprite.path_);
    // 手绘风格的动态单位（玩家、敌人）需要平滑过滤，防止锯齿
    texture->setSmooth(true);
    sf::Sprite spr(*texture, static_cast<sf::IntRect>(sprite.src_rect_));
    registry_.emplace<engine::component::SpriteComponent>(entity, spr);
    
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
        // 将创建好的动画帧容器插入动画map容器 (可直接使用蓝图中的事件信息)
        animations.emplace(anim_id, engine::component::Animation(std::move(frames), anim_blueprint.events_));
    }
    // 通过动画map容器创建动画组件
    registry_.emplace<engine::component::AnimationComponent>(entity, std::move(animations), default_animation_id);

    // 立即将默认动画的第一帧纹理矩形设置到精灵上，防止在动画系统更新前显示整个纹理
    auto& sprite_component = registry_.get<engine::component::SpriteComponent>(entity);
    auto& anim_component = registry_.get<engine::component::AnimationComponent>(entity);
    auto it = anim_component.animations_.find(anim_component.current_animation_id_);
    if (it != anim_component.animations_.end() && !it->second.frames_.empty()) {
        sprite_component.sprite_.setTextureRect(it->second.frames_[0].src_rect_);
    }
}

void EntityFactory::add_one_animation_component(entt::entity entity,
                                                const data::AnimationBlueprint& animation_blueprint, 
                                                const data::SpriteBlueprint& sprite_blueprint,
                                                entt::id_type animation_id,
                                                bool loop ) {
    // 创建动画帧容器
    std::vector<engine::component::AnimationFrame> frames;
    // 依次读取动画蓝图中每一个动画帧，并插入容器
    for (const auto& frame_index : animation_blueprint.frames_) {
        sf::IntRect source_rect = static_cast<sf::IntRect>(sprite_blueprint.src_rect_);
        source_rect.position.x += frame_index * source_rect.size.x;
        source_rect.position.y += animation_blueprint.row_ * source_rect.size.y;
        frames.emplace_back(source_rect, animation_blueprint.frame_duration_);
    }
    // 创建动画map容器
    std::unordered_map<entt::id_type, engine::component::Animation> animations;
    // 将创建好的动画帧容器插入动画map容器 (只有一个动画)
    animations.emplace(animation_id, engine::component::Animation(std::move(frames), animation_blueprint.events_, loop));
    // 通过动画map容器创建动画组件
    registry_.emplace<engine::component::AnimationComponent>(entity, std::move(animations), animation_id);
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
        sf::Time::Zero,
        level,
        rarity);
}

void EntityFactory::add_player_component(entt::entity entity, const data::PlayerBlueprint& player, int rarity) {
    auto cost = static_cast<int>(std::round(player.cost_ * (0.9f + 0.1f * rarity)));
    registry_.emplace<game::component::PlayerComponent>(entity, cost);
    // 添加类型标签(近战、远程、治疗)
    if (player.type_ == game::defs::PlayerType::Melee) {
        registry_.emplace<game::defs::MeleeUnitTag>(entity);    // 近战单位标签
        // 近战类型添加阻挡者组件
        registry_.emplace<game::component::BlockerComponent>(entity, player.block_);
    } else if (player.type_ == game::defs::PlayerType::Ranged) {
        registry_.emplace<game::defs::RangedUnitTag>(entity);    // 远程单位标签
        if (player.healer_) {
            registry_.emplace<game::defs::HealerTag>(entity);    // 治疗单位标签
        }
    }
    // TODO: 未来添加技能组件
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

void EntityFactory::add_projectile_id_component(entt::entity entity, entt::id_type id) {
    if (id == entt::null) return;
    registry_.emplace<game::component::ProjectileIDComponent>(entity, id);
}

}   // namespace game::factory