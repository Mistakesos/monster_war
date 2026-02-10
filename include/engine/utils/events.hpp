#pragma once
#include <memory>

// 前向声明
namespace engine::scene {
    class Scene;
} // namespace engine::scene

namespace engine::utils {
struct QuitEvent {};        // 退出事件
struct PopSceneEvent {};    // 弹出场景事件
struct PushSceneEvent {
    std::unique_ptr<engine::scene::Scene> scene;
};
struct ReplaceSceneEvent {
    std::unique_ptr<engine::scene::Scene> scene;
};
} // namespace engine::utils