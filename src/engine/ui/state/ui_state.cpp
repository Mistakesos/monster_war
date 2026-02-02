#include "engine/ui/state/ui_state.hpp"

namespace engine::ui::state {
UIState::UIState(engine::ui::UIInteractive* owner)
    : owner_{owner} {
}
}