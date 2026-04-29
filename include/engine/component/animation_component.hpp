#pragma once
#include <entt/core/hashed_string.hpp>
#include <entt/entity/entity.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>
#include <unordered_map>
#include <vector>

namespace engine::component {

/**
 * @brief 动画帧数据结构
 * 
 * 包含帧源矩形和帧间隔（毫秒）。
 */
struct AnimationFrame {
    AnimationFrame(sf::IntRect src_rect, sf::Time duration = sf::seconds(0.1f))
        : src_rect_{std::move(src_rect)}
        , duration_{duration} {
    }

    sf::IntRect src_rect_{};                            ///< @brief 帧源矩形
    sf::Time duration_{sf::seconds(0.1f)};                ///< @brief 帧间隔（毫秒）
};

/**
 * @brief 动画数据结构
 * 
 * 包含动画名称、帧列表、总时长、当前播放时间、是否循环等属性。
 */
struct Animation {
    /**
     * @brief 构造函数
     * @param name 动画名称
     * @param frames 动画帧
     * @param loop 是否循环，默认true
     */
    Animation(std::vector<AnimationFrame> frames, 
              std::unordered_map<int, entt::id_type> events = {},
              bool loop = true) 
        : frames_{std::move(frames)}
        , events_{events}
        , loop_{loop} {
        // 计算动画总时长 (总时长 = 所有帧时长之和)
        total_duration_ = sf::Time::Zero;
        for (const auto& frame : frames_) {
            total_duration_ += frame.duration_;
        }
    }

    std::vector<AnimationFrame> frames_;            ///< @brief 动画帧
    std::unordered_map<int, entt::id_type> events_; ///< @brief 动画事件，键为帧索引，值为事件ID
    sf::Time total_duration_{sf::Time::Zero};       ///< @brief 动画总时长（毫秒）
    bool loop_{true};                               ///< @brief 是否循环
};

/**
 * @brief 动画组件
 * 
 * 包含动画名称、帧列表、总时长、当前播放时间、是否循环等属性。
 */
struct AnimationComponent {
    /**
     * @brief 构造函数
     * @param animations 动画集合
     * @param current_animation_name 当前播放的动画名称
     * @param current_frame_index 当前播放的帧索引
     * @param current_time 当前播放时间（毫秒）
     * @param speed 播放速度
     */
    AnimationComponent(std::unordered_map<entt::id_type, Animation> animations,
                       entt::id_type current_animation_id,
                       size_t current_frame_index = 0,
                       sf::Time current_time = sf::Time::Zero,
                       float speed = 1.f)
        : animations_{std::move(animations)}
        , current_animation_id_{current_animation_id}
        , current_frame_index_{current_frame_index}
        , current_time_{current_time}
        , speed_{speed} {
    }

    std::unordered_map<entt::id_type, Animation> animations_;   ///< @brief 动画集合
    entt::id_type current_animation_id_{entt::null};            ///< @brief 当前播放的动画名称
    size_t current_frame_index_{};                              ///< @brief 当前播放的帧索引
    sf::Time current_time_{sf::Time::Zero};                     ///< @brief 当前播放时间（毫秒）
    float speed_{1.f};                                          ///< @brief 播放速度
};

} // namespace engine::component