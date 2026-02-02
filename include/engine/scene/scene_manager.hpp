#pragma once
#include <SFML/System/Time.hpp>
#include <memory>
#include <string>
#include <vector>

// 前置声明
namespace engine::core {
    class Context;
} // namespace engine::core

namespace engine::scene {
    class Scene;
} // namespace engine::scene

namespace engine::scene {
/**
 * @brief 管理游戏中的场景栈，处理场景切换和生命周期。
 */
class SceneManager final {
public:
    explicit SceneManager(engine::core::Context& context);
    ~SceneManager();
    
    // 禁止拷贝和移动
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager(SceneManager&&) = delete;
    SceneManager& operator=(SceneManager&&) = delete;

    // 延迟切换场景
    void request_push_scene(std::unique_ptr<Scene>&& scene);      ///< @brief 请求压入一个新场景。
    void request_pop_scene();                                     ///< @brief 请求弹出当前场景。
    void request_replace_scene(std::unique_ptr<Scene>&& scene);   ///< @brief 请求替换当前场景。

    // getters
    Scene* get_current_scene() const;                                 ///< @brief 获取当前活动场景（栈顶场景）的指针。
    engine::core::Context& get_context() const;                       ///< @brief 获取引擎上下文引用。

    // 核心循环函数
    void update(sf::Time delta_time);
    void render();
    void handle_input();

private:
    void process_pending_actions();                         ///< @brief 处理挂起的场景操作（每轮更新最后调用）。
    // 直接切换场景
    void push_scene(std::unique_ptr<Scene>&& scene);        ///< @brief 将一个新场景压入栈顶，使其成为活动场景。
    void pop_scene();                                       ///< @brief 移除栈顶场景。
    void replace_scene(std::unique_ptr<Scene>&& scene);     ///< @brief 清理场景栈所有场景，将此场景设为栈顶场景。

    engine::core::Context& context_;                        ///< @brief 引擎上下文引用
    std::vector<std::unique_ptr<Scene>> scene_stack_;        ///< @brief 场景栈

    enum class PendingAction {None, Push, Pop, Replace};    ///< @brief 待处理的动作枚举
    PendingAction pending_action_ = PendingAction::None;    ///< @brief 待处理的动作
    std::unique_ptr<Scene> pending_scene_;                  ///< @brief 待处理场景
};
} // namespace engine::scene