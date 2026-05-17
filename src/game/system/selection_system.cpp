#include "game/system/selection_system.hpp"
#include "game/component/player_component.hpp"
#include "game/component/enemy_component.hpp"
#include "engine/core/context.hpp"
#include "engine/render/camera.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/component/transform_component.hpp"
#include "game/defs/constants.hpp"
#include "game/defs/tags.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/sigh.hpp>
#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace game::system {

SelectionSystem::SelectionSystem(entt::registry& registry, engine::core::Context& context)
    : registry_{registry}
    , context_{context} {
    context_.get_input_manager().on_action(Action::MouseLeft).connect<&SelectionSystem::on_mouse_left_click>(this);
    context_.get_input_manager().on_action(Action::MouseRight).connect<&SelectionSystem::on_mouse_right_click>(this);
}

SelectionSystem::~SelectionSystem() {
    context_.get_input_manager().on_action(Action::MouseLeft).disconnect<&SelectionSystem::on_mouse_left_click>(this);
    context_.get_input_manager().on_action(Action::MouseRight).disconnect<&SelectionSystem::on_mouse_right_click>(this);
}

void SelectionSystem::update() {
    auto screen_position = context_.get_input_manager().get_mouse_position_window();
    auto mouse_pos = context_.get_camera().screen_to_world(screen_position);
    // 优先判断玩家单位
    auto view_player = registry_.view<engine::component::TransformComponent, game::component::PlayerComponent>();
    for (auto&& [entity, transform, _] : view_player.each()) {
        // 判断是否在鼠标悬浮检测范围内
        if ((transform.position_ - mouse_pos).lengthSquared() <= defs::HOVER_RADIUS * defs::HOVER_RADIUS) {
            registry_.ctx().get<entt::entity&>("hovered_unit"_hs) = entity;
            return;   // 找到悬浮单位，直接返回
        }
    }
    // 如果玩家单位没有被选中，再判断敌方单位
    auto view_enemy = registry_.view<engine::component::TransformComponent, game::component::EnemyComponent>();
    for (auto&& [entity, transform, _] : view_enemy.each()) {
        if ((transform.position_ - mouse_pos).lengthSquared() <= defs::HOVER_RADIUS * defs::HOVER_RADIUS) {
            registry_.ctx().get<entt::entity&>("hovered_unit"_hs) = entity;
            return;
        }
    }
    // 如果都没有被悬浮，则不悬浮任何单位
    registry_.ctx().get<entt::entity&>("hovered_unit"_hs) = entt::null;
}

void SelectionSystem::clear_current_selection() {
    auto current_selected_unit = registry_.ctx().get<entt::entity&>("selected_unit"_hs);
    // 移除之前选中的单位，并移除范围显示标签
    if (current_selected_unit != entt::null && registry_.valid(current_selected_unit)) {
        registry_.remove<game::defs::ShowRangeTag>(current_selected_unit);
    }
    registry_.ctx().get<entt::entity&>("selected_unit"_hs) = entt::null;
}

// --- 输入控制回调函数 ---
bool SelectionSystem::on_mouse_left_click() {
    auto hovered_unit = registry_.ctx().get<entt::entity&>("hovered_unit"_hs);
    if (hovered_unit == entt::null || !registry_.valid(hovered_unit)) return false;
    // 如果鼠标悬浮单位是玩家，则选中单位，并清除之前选中的单位
    if (auto player = registry_.try_get<game::component::PlayerComponent>(hovered_unit); player) {
        clear_current_selection();
        registry_.ctx().get<entt::entity&>("selected_unit"_hs) = hovered_unit;
        // 添加范围显示标签
        registry_.emplace_or_replace<game::defs::ShowRangeTag>(hovered_unit);
        return true;
    }
    return false;
}

bool SelectionSystem::on_mouse_right_click() {
    clear_current_selection();
    return false;   // 让鼠标右键可以穿透
}

} // namespace game::system