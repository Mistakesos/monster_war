#include "engine/render/animation.hpp"
#include <spdlog/spdlog.h>

namespace engine::render{
Animation::Animation(std::string_view name, bool loop)
    : name_{name}
    , loop_{loop} {
}

void Animation::add_frame(const sf::IntRect& source_rect, sf::Time duration) {
    if (duration <= sf::Time::Zero) {
        spdlog::warn("尝试向动画 '{}' 添加无效持续时间的帧", name_);
        return;
    }
    frames_.push_back({source_rect, duration});
    total_duration_ += duration;
}

const AnimationFrame& Animation::get_frame(sf::Time time) const {
    if (frames_.empty()) {
        spdlog::error("动画 '{}' 没有帧，无法获取帧", name_);
        return frames_.back();      // 返回最后一帧（空的）
    }

    sf::Time current_time = time;

    if (loop_ && total_duration_ > sf::Time::Zero) {
        // 对循环动画使用模运算获取有效时间
        float mod_time = std::fmod(time.asSeconds(), total_duration_.asSeconds());
        current_time = sf::seconds(mod_time);
    } else {
        // 对于非循环动画，如果时间超过总时长，则停留在最后一帧
        if (current_time >= total_duration_) {
            return frames_.back();
        }
    }

    // 遍历帧以找到正确的帧
    sf::Time accumulated_time = sf::Time::Zero;
    for (const auto& frame : frames_) {
        accumulated_time += frame.duration;
        if (current_time < accumulated_time) {
            return frame;
        }
    }

    // 理论上在不应到达这里，但为了安全起见，返回最后一帧
    spdlog::warn("动画 '{}' 在获取帧信息时出现错误。", name_);
    return frames_.back();
}
} // namespace engine::render