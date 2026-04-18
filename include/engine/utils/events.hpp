#pragma once
#include <memory>
#include <SFML/System/Vector2.hpp>

// 前向声明
namespace engine::scene {
    class Scene;
} // namespace engine::scene

namespace engine::utils {
struct QuitEvent {};            // 退出事件
struct PopSceneEvent {};        // 弹出场景事件
struct WindowResizedEvent {
    sf::Vector2u window_size;
};                              // 窗口调整大小事件
struct PushSceneEvent {
    std::unique_ptr<engine::scene::Scene> scene;
};
struct ReplaceSceneEvent {
    std::unique_ptr<engine::scene::Scene> scene;
};
} // namespace engine::utils