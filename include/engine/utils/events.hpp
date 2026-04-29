#pragma once
#include <memory>
#include <SFML/System/Vector2.hpp>
#include <entt/entity/entity.hpp>

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

/// @brief 播放动画事件
struct PlayAnimationEvent {
    entt::entity entity_{entt::null};           ///< @brief 目标实体
    entt::id_type animation_id_{entt::null};    ///< @brief 动画ID
    bool loop_{true};                           ///< @brief 是否循环
};

/// @brief 动画播放完成事件
struct AnimationFinishedEvent {
    entt::entity entity_{entt::null};           ///< @brief 目标实体
    entt::id_type animation_id_{entt::null};    ///< @brief 动画ID
};

/// @brief 动画事件
struct AnimationEvent {
    entt::entity entity_{entt::null};           ///< @brief 目标实体
    entt::id_type event_name_id_{entt::null};   ///< @brief 事件名称ID
    entt::id_type animation_name_id_{entt::null};   ///< @brief 动画名称ID
};

/// @brief 播放音效事件
struct PlaySoundEvent {
    entt::entity entity_{entt::null};           ///< @brief 目标实体（可以为空，即播放全局音效）
    entt::id_type sound_id_{entt::null};        ///< @brief 音效ID
};

} // namespace engine::utils