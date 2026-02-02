#include "engine/core/time.hpp"

namespace engine::core {
const sf::Time& Time::get_frame_duration() const {
    return this->time_per_frame_;
}

void Time::set_time_scale(float scale) {
    this->time_scale_ = scale;
    this->time_per_frame_ *= scale;
}

float Time::get_time_scale() {
    return this->time_scale_;
}

void Time::set_target_fps(float fps) {
    this->TAEGET_FPS = fps;
    this->time_per_frame_ = sf::seconds(1.f / TAEGET_FPS);
}

float Time::get_target_fps() const {
    return this->TAEGET_FPS;
}

void Time::accumulate_frame_time() {
    elapsed_time += clock_.restart();
}

bool Time::should_update() const {
    return elapsed_time > time_per_frame_;
}

void Time::consume_update_time() {
    elapsed_time -= time_per_frame_;
}
} // namespace engine::core