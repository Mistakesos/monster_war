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
    ~UIHoverState() override;

private:
    void update(sf::Time delta, engine::core::Context& context) override;

    bool on_mouse_pressed();  ///< @brief 鼠标按下回调函数 (不再使用轮询“isActionPressed”)
};
} // namespace engine::ui::state