#include "game/system/place_unit_system.hpp"
#include "game/data/session_data.hpp"
#include "game/data/game_stats.hpp"
#include "game/defs/events.hpp"
#include "game/defs/tags.hpp"
#include "game/component/place_occupied_component.hpp"
#include "game/factory/entity_factory.hpp"
#include "game/component/unit_prep_component.hpp"
#include "engine/core/context.hpp"
#include "engine/audio/audio_player.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/render/camera.hpp"
#include "engine/component/transform_component.hpp"
#include "engine/component/sprite_component.hpp"
#include "engine/component/name_component.hpp"
#include "engine/component/render_component.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::system {

PlaceUnitSystem::PlaceUnitSystem(entt::registry& registry, 
    game::factory::EntityFactory& entity_factory,
    engine::core::Context& context)
    : registry_(registry), entity_factory_(entity_factory), context_(context) {
    // 注册按键
    auto& input_manager = context_.get_input_manager();
    input_manager.on_action(Action::MouseRight).connect<&PlaceUnitSystem::on_cancel_prep_unit>(this);
    input_manager.on_action(Action::MouseLeft).connect<&PlaceUnitSystem::on_place_unit>(this);
    // 注册事件
    auto& dispatcher = context_.get_dispatcher();
    dispatcher.sink<game::defs::PrepUnitEvent>().connect<&PlaceUnitSystem::on_prep_unit_event>(this);
    dispatcher.sink<game::defs::RemovePlayerUnitEvent>().connect<&PlaceUnitSystem::on_remove_unit_event>(this);
}

PlaceUnitSystem::~PlaceUnitSystem() {
    // 断开按键
    auto& input_manager = context_.get_input_manager();
    input_manager.on_action(Action::MouseRight).disconnect<&PlaceUnitSystem::on_cancel_prep_unit>(this);
    input_manager.on_action(Action::MouseLeft).disconnect<&PlaceUnitSystem::on_place_unit>(this);
    // 断开事件
    context_.get_dispatcher().disconnect(this);
}

void PlaceUnitSystem::update(sf::Time) {
    // 目标放置位置先置为null，只有找到了有效位置才会被赋值
    target_place_entity_ = entt::null;

    auto view = registry_.view<game::component::UnitPrepComponent, engine::component::TransformComponent>();
    // 虽然是循环，但拥有UnitPrepComponent的实体最多只有一个
    for (auto entity : view) {
        // 位置同步到到鼠标
        const auto& mouse_pos_screen = context_.get_input_manager().get_mouse_position_window();
        const auto mouse_pos_world = context_.get_camera().screen_to_world(mouse_pos_screen);
        auto& transform = view.get<engine::component::TransformComponent>(entity);
        transform.position_ = mouse_pos_world;

        // 检查放置位置是否有效
        const auto& unit_prep = view.get<game::component::UnitPrepComponent>(entity);
        check_target_place(transform.position_, unit_prep.type_);
        
        // 根据是否有效设置颜色
        auto& render = registry_.get<engine::component::RenderComponent>(entity);
        target_place_entity_ != entt::null ? render.color_ = sf::Color::Green 
                                           : render.color_ = sf::Color::Red;
    }
}

void PlaceUnitSystem::check_target_place(const sf::Vector2f& position, game::defs::PlayerType player_type) {
    // 检查是否处在近战可放置区域（拥有MeleePlaceTag的地点）
    if (player_type == game::defs::PlayerType::Melee) {
        auto melee_place_view = registry_.view<game::defs::MeleePlaceTag, 
            engine::component::TransformComponent, 
            engine::component::SpriteComponent>(entt::exclude<game::component::PlaceOccupiedComponent>);
        for (auto place_entity : melee_place_view) {
            auto& place_transform = melee_place_view.get<engine::component::TransformComponent>(place_entity);
            auto& place_sprite = melee_place_view.get<engine::component::SpriteComponent>(place_entity);
            // 检测与放置区域中心的距离（Tiled中的参照点是左上角）
            auto center_position = place_transform.position_ + place_sprite.sprite_.getLocalBounds().size.componentWiseMul(place_transform.scale_) / 2.f;
            if ((position - center_position).lengthSquared() < game::defs::PLACE_RADIUS * game::defs::PLACE_RADIUS) {
                target_place_entity_ = place_entity;
                return;
            }
        }
    // 检查是否处在远程可放置区域（拥有RangedPlaceTag的地点）
    } else if (player_type == game::defs::PlayerType::Ranged) {
        auto ranged_place_view = registry_.view<game::defs::RangedPlaceTag, 
            engine::component::TransformComponent, 
            engine::component::SpriteComponent>(entt::exclude<game::component::PlaceOccupiedComponent>);
        for (auto place_entity : ranged_place_view) {
            auto& place_transform = ranged_place_view.get<engine::component::TransformComponent>(place_entity);
            auto& place_sprite = ranged_place_view.get<engine::component::SpriteComponent>(place_entity);
            auto center_position = place_transform.position_ + place_sprite.sprite_.getLocalBounds().size.componentWiseMul(place_transform.scale_) / 2.f;
            if ((position - center_position).lengthSquared() < game::defs::PLACE_RADIUS * game::defs::PLACE_RADIUS) {
                target_place_entity_ = place_entity;
                return;
            }
        }
    }
}

void PlaceUnitSystem::on_prep_unit_event(const game::defs::PrepUnitEvent& event) {
    // 如果cost资源不够，直接返回
    auto& game_stats = registry_.ctx().get<game::data::GameStats&>();
    if (game_stats.cost_ < event.cost_) return;

    // 先移除其他单位准备类型实体
    on_cancel_prep_unit();

    // 在鼠标所在的位置创建单位准备类型实体
    auto screen_position = context_.get_input_manager().get_mouse_position_window();
    auto position = context_.get_camera().screen_to_world(screen_position);
    entity_factory_.create_unit_prep(event.name_id_, event.class_id_, event.cost_, position);
    spdlog::info("创建单位准备类型实体: {}, pos: {}, {}", event.name_id_, position.x, position.y);
}

void PlaceUnitSystem::on_remove_unit_event(const game::defs::RemovePlayerUnitEvent& event) {
    // 标记该单位为死亡
    registry_.emplace_or_replace<game::defs::DeadTag>(event.entity_);
    // 检查所有被占用的地点，如果占用者是移除事件中的单位，则移除占用组件
    auto view = registry_.view<game::component::PlaceOccupiedComponent>();
    for (auto entity : view) {
        auto& place_occupied = view.get<game::component::PlaceOccupiedComponent>(entity);
        if (place_occupied.entity_ == event.entity_) {
            registry_.remove<game::component::PlaceOccupiedComponent>(entity);
            spdlog::info("移除地点 {} 的占用组件", entt::to_integral(entity));
            break;
        }
    }
}

bool PlaceUnitSystem::on_place_unit() {
    // 目标放置位置有效才继续
    if (target_place_entity_ == entt::null) return false;

    // 获取目标位置坐标
    const auto& transform = registry_.get<engine::component::TransformComponent>(target_place_entity_);
    const auto& sprite = registry_.get<engine::component::SpriteComponent>(target_place_entity_);
    auto position = transform.position_ + sprite.sprite_.getLocalBounds().size.componentWiseMul(transform.scale_) / 2.f;
    // 获取单位信息
    auto unit_map = registry_.ctx().get<std::shared_ptr<game::data::SessionData>>()->get_unit_map();
    auto& game_stats = registry_.ctx().get<game::data::GameStats&>();
    auto view_prep = registry_.view<game::component::UnitPrepComponent>();
    // 循环只会进行一次，因为拥有UnitPrepComponent的实体最多只有一个
    for (auto entity : view_prep) {
        const auto& unit_prep_component = registry_.get<game::component::UnitPrepComponent>(entity);
        auto& unit_data = unit_map[unit_prep_component.name_id_];
        // 创建单位
        auto unit_entity = entity_factory_.create_player_unit(unit_data.class_id_, position, unit_data.level_, unit_data.rarity_);
        registry_.emplace<engine::component::NameComponent>(unit_entity, unit_data.name_id_, unit_data.name_);
        // 地点实体添加占用组件
        registry_.emplace<game::component::PlaceOccupiedComponent>(target_place_entity_, unit_entity);
        // 扣除费用
        game_stats.cost_ -= unit_prep_component.cost_;
        // 移除单位准备类型实体
        registry_.emplace_or_replace<game::defs::DeadTag>(entity);

        // 通知UI移除对应肖像
        context_.get_dispatcher().enqueue(game::defs::RemoveUIPortraitEvent{unit_data.name_id_});

        // --- 渲染图层修正：确保玩家所在图层大于放置点图标的图层 ---
        const auto& render_place = registry_.get<engine::component::RenderComponent>(target_place_entity_);
        // 正常情况下render_place.layer应该不会超过主图层（10），那么不做处理
        // 如果超过了，就让玩家所在图层 = 放置点图层 + 1
        if (render_place.layer_ > engine::component::RenderComponent::MAIN_LAYER) {
            auto& render_player = registry_.get<engine::component::RenderComponent>(unit_entity);
            render_player.layer_ = render_place.layer_ + 1;
        }
    }
    // 播放放置音效
    context_.get_audio_player().play_sound("unit_placed"_hs);
    return true;
}

bool PlaceUnitSystem::on_cancel_prep_unit() {
    // 移除所有单位准备类型实体
    auto view = registry_.view<game::component::UnitPrepComponent>();
    for (auto entity : view) {
        registry_.emplace_or_replace<game::defs::DeadTag>(entity);
        spdlog::info("移除单位准备类型实体: {}", entt::to_integral(entity));
    }
    return false;   // 让鼠标右键可以穿透
}

}   // namespace game::system