#pragma once
#include <SFML/System/Time.hpp>

namespace engine::object {
    class GameObject;
} // namespace engine::object

namespace engine::core {
    class Context;
} // namespace engine::core

namespace engine::component {
/**
 * @brief 组件的抽象基类
 * 
 * 所有具体组件都应该从此基类继承
 * 定义了组件生命周期中可能调用的同样方法
 */
class Component {
    friend class engine::object::GameObject;                ///< @brief 他需要调用Component的protected函数
public:
    Component(engine::object::GameObject* owner);
    virtual ~Component() = default;

    // 禁止拷贝和移动，组件通常不应被拷贝或移动（更改owner_就相当于移动）
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    Component(Component&&) = delete;
    Component& operator=(Component&&) = delete;

    void set_owner(engine::object::GameObject* owner);      ///< @brief 设置拥有此组件的 GameObject
    engine::object::GameObject* get_owner() const;          ///< @brief 获取拥有此组件的 GameObject

protected:
    // 关键循环函数（未来将其中一个改为 = 0 以实现纯虚函数
    virtual void handle_input(engine::core::Context&) {}                ///< @brief 处理输入
    virtual void update(sf::Time, engine::core::Context&) = 0;          ///< @brief 更新
    virtual void render(engine::core::Context&) {}                      ///< @brief 渲染

    engine::object::GameObject* owner_ = nullptr;           ///< @brief 指向该组件的 GameObject
};
} // namespace engine::component