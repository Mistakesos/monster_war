#pragma once
#include "engine/ui/ui_manager.hpp"
#include <entt/entity/registry.hpp>
#include <SFML/System/Time.hpp>
#include <memory>
#include <string>
#include <string_view>

namespace engine::core {
    class Context;
} // namespace engine::core

namespace engine::ui {
    class UIManager;
} // namespace engine::ui

namespace engine::scene {
/**
 * @brief 场景基类，负责管理场景中的游戏对象和场景生命周期。
 *
 * 包含一组游戏对象，并提供更新、渲染、处理输入和清理的接口。
 * 派生类应实现具体的场景逻辑。
 */
class Scene {
public:
    /**
     * @brief 构造函数。
     *
     * @param name 场景的名称。
     * @param context 场景上下文。
     */
    Scene(std::string_view name, engine::core::Context& context);

    virtual ~Scene();           // 1. 基类必须声明虚析构函数才能让派生类析构函数被正确调用。
                                // 2. 析构函数定义必须写在cpp中，不然需要引入GameObject头文件
    // 禁止拷贝和移动构造
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;

    // 核心循环方法
    virtual void update(sf::Time delta);        ///< @brief 更新场景。
    virtual void render();                      ///< @brief 渲染场景。
    virtual void handle_input();                ///< @brief 处理输入。

    /// @brief 请求弹出当前场景
    void request_pop_scene();

    /// @brief 请求压入一个新场景
    void request_push_scene(std::unique_ptr<engine::scene::Scene>&& scene);

    /// @brief 请求替换当前场景
    void request_replace_scene(std::unique_ptr<engine::scene::Scene>&& scene);

    /// @brief 退出游戏
    void quit();
    
    // getters and setters
    void set_name(std::string_view name) { scene_name_ = name; }                ///< @brief 设置场景名称
    std::string_view get_name() const { return scene_name_; }                   ///< @brief 获取场景名称
    entt::registry& get_registry() { return registry_; }                        ///< @brief 获取注册表引用

    engine::core::Context& get_context() const { return context_; }                                         ///< @brief 获取上下文引用
    
protected:
    std::string scene_name_;                                        ///< @brief 场景名称
    engine::core::Context& context_;                                ///< @brief 上下文引用（显式，构造时传入）
    std::unique_ptr<engine::ui::UIManager> ui_manager_ = nullptr;   ///< @brief UI管理器(初始化时自动创建)
    entt::registry registry_;                                       ///< @brief ECS 注册表
};
} // namespace engine::scene