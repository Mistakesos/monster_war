#pragma once
#include <memory>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>

namespace engine::core {
    class Context;
} // namespace engine::core

namespace engine::ui {
    class UIElement;
    class UIPanel; // UIPanel 将作为根元素
} // namespace engine::ui

namespace engine::ui {
/**
 * @brief 管理特定场景中的UI元素集合。
 *
 * 负责UI元素的生命周期管理（通过根元素）、渲染调用和输入事件分发。
 * 每个需要UI的场景（如菜单、游戏HUD）应该拥有一个UIManager实例。
 */
class UIManager final {
public:
    UIManager(sf::Vector2f window_size);
    ~UIManager();

    // 禁止拷贝和移动构造/赋值
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    UIManager(UIManager&&) = delete;
    UIManager& operator=(UIManager&&) = delete;

    void add_element(std::unique_ptr<UIElement> element);    ///< @brief 添加一个UI元素到根节点的child_容器中。
    UIPanel* get_root_element() const;                       ///< @brief 获取根UIPanel元素的指针。
    void clear_elements();                                   ///< @brief 清除所有UI元素，通常用于重置UI状态。

    // --- 核心循环方法 ---
    bool handle_input(engine::core::Context&);               ///< @brief 处理输入事件，如果事件被处理则返回true。
    void update(sf::Time delta, engine::core::Context&);
    void render(engine::core::Context&);

private:
    std::unique_ptr<UIPanel> root_element_;     ///< @brief 一个UIPanel作为根节点(UI元素)
};
} // namespace engine::ui