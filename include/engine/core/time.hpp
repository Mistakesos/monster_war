#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>

namespace engine::core {
/**
 * @brief 管理游戏循环中的时间
 * 
 * 使用 SFML 的高精度时钟，确保准确性
 * 提供获取缩放和未缩放 DeltaTime 的方法，以及设置时间缩放因子的能力
 */
class Time final {
public:
    Time() = default;
    ~Time() = default;

    Time(const Time&) = delete;
    Time& operator=(const Time&) = delete;
    Time(Time&&) = delete;
    Time& operator=(Time&&) = delete;

    /**
     * @brief 获取帧间隔
     */
    const sf::Time& get_frame_duration() const;

    /**
     * @brief 设置时间缩放因子
     */
    void set_time_scale(float scale);

    /**
     * @brief 获取时间缩放因子
     */
    float get_time_scale();

    /**
     * @brief 设置目标帧率
     */
    void set_target_fps(float fps);

    /**
     * @brief 获取目标帧率
     */
    float get_target_fps() const;

    /**
     * @brief 累加时间
     */
    void accumulate_frame_time();

    /**
     * @brief 是否要更新游戏逻辑
     * @return 如果time_since_last_update > time_per_frame返回真
     */
    bool should_update() const;

    /**
     * @brief 减少一单位帧间隔的时间
     */
    void consume_update_time();
    
private:
    float TAEGET_FPS = 60.f;                                    // 目标帧率

    sf::Clock clock_;                                           // 内部维护的时钟
    sf::Time elapsed_time = sf::Time::Zero;                     // 自上次更新以来的时间
    sf::Time time_per_frame_ = sf::Time::Zero;                  // 目标帧间隔
    float time_scale_ = 1.f;                                    // 时间缩放因子
};
} // namespace engine::core