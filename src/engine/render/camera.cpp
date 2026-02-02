#include "engine/render/camera.hpp"
#include "engine/component/transform_component.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

namespace engine::render {
Camera::Camera(sf::RenderWindow* window, std::optional<sf::FloatRect> limit_bounds)
    : window_obs_{window}
    , world_view_{window->getDefaultView()}
    , ui_view_{window->getDefaultView()}
    , limit_bounds_{limit_bounds} {
    spdlog::trace("Camera 初始化成功");
    // world_view_.zoom(0.5f);
    // ui_view_.zoom(0.5f);
    // world_view_.setCenter(world_view_.getSize() / 2.f);
    // ui_view_.setCenter(ui_view_.getSize() / 2.f);
    
    window_obs_->setView(world_view_);
}

void Camera::update(sf::Time delta_time) {
    if (target_obs_ == nullptr) return;
    
    sf::Vector2f target_pos = target_obs_->get_position();
    sf::Vector2f desired_center = target_pos;   // 目标位置就是期望的视图中心
    sf::Vector2f current_center = world_view_.getCenter();
    
    // 计算当前位置与目标位置的距离
    sf::Vector2f diff = desired_center - current_center;
    float distance = diff.length();
    constexpr float SNAP_THRESHOLD = 1.f; // 设置一个距离阈值
    
    sf::Vector2f new_center;
    if (distance < SNAP_THRESHOLD) {
        // 如果距离小于阈值，直接吸附到目标位置
        new_center = desired_center;
    } else {
        // 使用线性插值平滑移动
        float t = smooth_speed_ * delta_time.asSeconds();
        t = std::min(t, 1.f); // 确保插值因子不超过1
        
        new_center = current_center + diff * t;        
    }
    
    world_view_.setCenter(new_center);
    clamp_position();
}

void Camera::set_world_view_center(sf::Vector2f center) {
    world_view_.setCenter(center);
    clamp_position();
}

void Camera::set_ui_view_center(sf::Vector2f center) {
    ui_view_.setCenter(center);
    clamp_position();
}

void Camera::move(const sf::Vector2f& offset) {
    world_view_.move(offset);
    clamp_position();
    window_obs_->setView(world_view_);
}

void Camera::set_limit_bounds(std::optional<sf::FloatRect> limit_bounds) {
    limit_bounds_ = limit_bounds;
    clamp_position();
}

void Camera::clamp_position() {
    if (!limit_bounds_.has_value()) return;
    
    const sf::FloatRect& bounds = limit_bounds_.value();
    
    // 边界检查需要确保相机视图完全在 limit_bounds 内
    if (bounds.size.x > 0 && bounds.size.y > 0) {
        sf::Vector2f view_size = get_world_view_size();
        sf::Vector2f half_view_size = view_size / 2.f;
        
        // 计算允许的相机中心位置范围
        // 最小中心位置：左上角 + 半个视口
        sf::Vector2f min_center = sf::Vector2f(bounds.position.x, bounds.position.y) + half_view_size;
        // 最大中心位置：右下角 - 半个视口
        sf::Vector2f max_center = sf::Vector2f(bounds.position.x + bounds.size.x, 
                                               bounds.position.y + bounds.size.y) - half_view_size;

        // 确保 max_center 不小于 min_center (视口可能比世界还大)
        max_center.x = std::max(min_center.x, max_center.x);
        max_center.y = std::max(min_center.y, max_center.y);

        // 限制视图中心在允许范围内
        sf::Vector2f current_center = world_view_.getCenter();
        current_center.x = std::clamp(current_center.x, min_center.x, max_center.x);
        current_center.y = std::clamp(current_center.y, min_center.y, max_center.y);
        
        world_view_.setCenter(current_center);
    }
}

sf::Vector2f Camera::world_to_screen(const sf::Vector2f& world_pos) const {
    // 将世界坐标减去相机左上角位置
    return window_obs_->mapPixelToCoords(static_cast<sf::Vector2i>(world_pos));
}

sf::Vector2f Camera::world_to_screen_with_parallax(const sf::Vector2f& world_pos, const sf::Vector2f& scroll_factor) const {
    // 1. 计算视差偏移：相机位置乘以滚动因子
    auto position = get_world_view_center();
    sf::Vector2f parallax_offset(position.x * scroll_factor.x, position.y * scroll_factor.y);
    
    // 2. 应用视差效果：世界坐标减去视差偏移
    sf::Vector2f parallax_adjusted_pos = world_pos - parallax_offset;
    
    // 3. 转换为屏幕坐标（与第一个函数保持一致）
    return window_obs_->mapPixelToCoords(static_cast<sf::Vector2i>(parallax_adjusted_pos));
}

sf::Vector2f Camera::screen_to_world(const sf::Vector2f& screen_pos) const {
    // 将屏幕坐标加上相机左上角位置
    return static_cast<sf::Vector2f>(window_obs_->mapCoordsToPixel(screen_pos));
}
} // namespace engine::render