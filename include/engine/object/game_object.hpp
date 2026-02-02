#pragma once
#include "engine/component/component.hpp"
#include <spdlog/spdlog.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <typeindex>            // 用于索引类型
#include <utility>              // 用于完美转发

namespace sf {
    class Time;
} // namespace sf

namespace engine::object {
/**
 * @brief 游戏对象类，负责管理游戏对象的组件
 * 
 * 该类管理游戏对象的组件，并提供添加、获取、检查和移除组件的功能
 * 他还提供更新和渲染游戏对象的方法
 */
class GameObject final {
public:
    GameObject(std::string_view name = "", std::string_view tag = "");  ///< @brief 构造函数，默认名称为空，标签为空
    ~GameObject() = default;

    // 禁止拷贝和移动，确保唯一性 (通常游戏对象不应随意拷贝)
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    GameObject(GameObject&&) = delete;
    GameObject& operator=(GameObject&&) = delete;

    // setters and getters
    void set_name(std::string_view name) { name_ = name; }                    ///< @brief 设置名称
    void set_tag(std::string_view tag) { tag_ = tag; }                        ///< @brief 设置标签
    void set_need_remove(bool need_remove) { need_remove_ = need_remove; }    ///< @brief 设置是否需要删除
    std::string_view get_name() const { return name_; }                       ///< @brief 获取名称
    std::string_view get_tag() const { return tag_; }                         ///< @brief 获取标签
    bool is_need_remove() const { return need_remove_; }                      ///< @brief 获取是否需要删除
    
    // 关键循环函数
    void handle_input(engine::core::Context& context);                        ///< @brief 处理输入
    void update(sf::Time delta, engine::core::Context& context);              ///< @brief 更新所有组件
    void render(engine::core::Context& context);                              ///< @brief 渲染所有组件

    /**
     * @brief 添加组件
     * 
     * @tparam T 组件类型
     * @tparam Args 组件构造函数参数类型
     * @param args 组件构造函数参数
     * @return 组件指针
     */
    template <typename T, typename... Args>
    T* add_component(Args&&... args) {
        static_assert(std::is_base_of<engine::component::Component, T>::value, "T 必须继承自 Component");
        auto type_index = std::type_index(typeid(T));

        // 如果组件存在，直接返回组件指针
        if (has_component<T>()) {
            return get_component<T>();
        }

        // 如果不存在就创建组件 记得把this传入！
        auto new_component = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* ptr = new_component.get();
        components_[type_index] = std::move(new_component);
        spdlog::debug("GameObject::add_component: {} added component {}", name_, typeid(T).name());
        return ptr;
    }

    /**
     * @brief 获取组件
     * 
     * @tparam T 组件类型
     * @return 组件指针
     */
    template <typename T>
    T* get_component() const {
        static_assert(std::is_base_of<engine::component::Component, T>::value, "T 必须继承自 Component");
        auto type_index = std::type_index(typeid(T));
        if (auto it = components_.find(type_index); it != components_.end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }
    
    /**
     * @brief 检查是否存在组件
     * 
     * @tparam T 组件类型
     * @return 是否存在组件
     */
    template <typename T>
    bool has_component() const {
        static_assert(std::is_base_of<engine::component::Component, T>::value, "T 必须继承自 Component");
        return components_.contains(std::type_index(typeid(T)));
    }

    /**
     * @brief 移除组件
     * 
     * @tparam T 组件类型
     */
    template<typename T>
    void remove_component() {
        static_assert(std::is_base_of<engine::component::Component, T>::value, "T 必须继承自 Component");
        auto type_index = std::type_index(typeid(T));
        if (auto it = components_.find(type_index); it != components_.end()) {
            components_.erase(it);
        }
    }

private:
    std::string name_;          ///< @brief 名称
    std::string tag_;           ///< @brief 标签
    std::unordered_map<std::type_index, std::unique_ptr<engine::component::Component>> components_;     ///< @brief 组件列表
    bool need_remove_ = false;  ///< @brief 延迟删除的标识，将由场景类负责管理
};
} // namespace engine::object