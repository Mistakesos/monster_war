#pragma once
#include <entt/entity/fwd.hpp>
#include <unordered_map>

namespace engine::component {

/**
 * @brief 音频组件，用于处理音频播放和管理。
 */
struct AudioComponent {
    std::unordered_map<entt::id_type, entt::id_type> sounds_;     ///< @brief 音效id 到路径的映射表
};

} // namespace engine::component