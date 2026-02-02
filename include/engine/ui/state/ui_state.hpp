#pragma once
#include <memory>

namespace engine::core {
    class Context;
} // namespace engine::core

namespace engine::ui {
    class UIInteractive;
} // namespace engine::ui

namespace engine::ui::state {
/**
 * @brief 可交互UI元素在特定状态下的行为接口。
 *
 * 该接口定义了所有具体UI状态必须实现的通用操作，
 * 例如处理输入事件、更新状态逻辑以及确定视觉表现。
 */
class UIState {
    friend class engine::ui::UIInteractive;
public:
    /**
     * @brief 构造函数传入父节点指针
     */
    UIState(engine::ui::UIInteractive* owner);
    virtual ~UIState() = default;

    // 删除拷贝和移动构造函数/赋值运算符
    UIState(const UIState&) = delete;
    UIState& operator=(const UIState&) = delete;
    UIState(UIState&&) = delete;
    UIState& operator=(UIState&&) = delete;

    /**
     * @brief 过渡到下一个状态，设置初始状态后无需重复传入 UIInteractive 指针
     * @param Next 下一个状态类名
     * @param Args 下一个状态的构造参数
     */
    template<typename Next, typename... Args>
    void transition(Args&&... args) {
        next_state_ = std::make_unique<Next>(owner_, std::forward<Args>(args)...);
    }

protected:
    // --- 核心方法 --- 
    virtual void handle_input(engine::core::Context& context) = 0;

    engine::ui::UIInteractive* owner_ = nullptr;    ///< @brief 指向父节点
    std::unique_ptr<UIState> next_state_ = nullptr; ///< @brief 指向下一个节点的指针
};
} // namespace engine::ui::state