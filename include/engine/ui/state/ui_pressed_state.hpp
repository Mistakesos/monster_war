#pragma once
#include "ui_state.hpp"

namespace engine::ui::state {
/**
 * @brief 正常状态
 *
 * 正常状态是UI元素的默认状态。
 */
class UIPressedState final: public UIState {
    friend class engine::ui::UIInteractive;
public:
    UIPressedState(engine::ui::UIInteractive* owner);
    ~UIPressedState() override;

private:
    bool on_mouse_released();  ///< @brief 鼠标按下回调函数 (不再使用轮询“isActionPressed”)
};
} // namespace engine::ui::state