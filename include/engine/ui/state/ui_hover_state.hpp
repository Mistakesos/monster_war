#pragma once
#include "ui_state.hpp"

namespace engine::ui::state {
/**
 * @brief 正常状态
 *
 * 正常状态是UI元素的默认状态。
 */
class UIHoverState final: public UIState {
    friend class engine::ui::UIInteractive;
public:
    UIHoverState(engine::ui::UIInteractive* owner);
    ~UIHoverState() override = default;

private:
    void handle_input(engine::core::Context& context) override;
};
} // namespace engine::ui::state