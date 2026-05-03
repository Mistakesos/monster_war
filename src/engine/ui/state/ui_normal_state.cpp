#include "engine/ui/state/ui_normal_state.hpp"
#include "engine/ui/state/ui_hover_state.hpp"
#include "engine/ui/ui_interactive.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/core/context.hpp"
#include <spdlog/spdlog.h>
#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace engine::ui::state {
UINormalState::UINormalState(engine::ui::UIInteractive* owner) 
    : UIState{owner} {
    owner_->set_sprite("normal"_hs);
    spdlog::debug("切换到正常状态");
}

void UINormalState::handle_input(engine::core::Context& context) {
    auto& input_manager = context.get_input_manager();
    auto mouse_pos = input_manager.get_mouse_logical_position();
    if (owner_->is_point_inside(static_cast<sf::Vector2f>(mouse_pos))) {         // 如果鼠标在UI元素内，则切换到悬停状态
        owner_->play_sound("hover"_hs);
        transition<engine::ui::state::UIHoverState>();
    }
}
} // namespace engine::ui::state