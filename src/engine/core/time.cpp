#include "engine/core/time.hpp"

namespace engine::core {
const sf::Time Time::get_frame_duration() const {
    return this->time_per_frame_;
}

void Time::set_time_scale(float scale) {
    this->time_scale_ = scale;
}

float Time::get_time_scale() {
    return this->time_scale_;
}

void Time::set_target_fps(float fps) {
    if (fps <= 0.f) {
        return;
    }
    this->TARGET_FPS = fps;
    this->time_per_frame_ = sf::seconds(1.f / TARGET_FPS);
}

float Time::get_target_fps() const {
    return this->TARGET_FPS;
}

void Time::accumulate_frame_time(sf::Time delta) {
    elapsed_time += delta * time_scale_;
}

bool Time::should_update() const {
    return elapsed_time >= time_per_frame_;
}

void Time::consume_update_time() {
    elapsed_time -= time_per_frame_;
}
} // namespace engine::core