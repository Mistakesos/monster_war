#include "engine/render/camera.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>

namespace engine::render {
Camera::Camera(sf::RenderWindow* window, entt::dispatcher* dispatcher, std::optional<sf::FloatRect> limit_bounds)
    : window_obs_{window}
    , dispatcher_obs_{dispatcher}
    , world_view_{window->getDefaultView()}
    , ui_view_{window->getDefaultView()}
    , limit_bounds_{limit_bounds} {
    spdlog::trace("Camera 初始化成功");

    dispatcher->sink<engine::utils::WindowResizedEvent>().connect<&Camera::on_resize>(this);
    
    window_obs_->setView(world_view_);
}

Camera::~Camera() {
    dispatcher_obs_->disconnect(this);
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

void Camera::on_resize(const engine::utils::WindowResizedEvent& event) {
    apply_letterbox(event.window_size);
}

void Camera::apply_letterbox(sf::Vector2u window_size) {
    auto fix_view = [&](sf::View& view) {

        float window_ratio = static_cast<float>(window_size.x) / static_cast<float>(window_size.y);
        float view_ratio = view.getSize().x / view.getSize().y;

        float size_x = 1.f, size_y = 1.f;
        float pos_x = 0.f, pos_y = 0.f;

        if (window_ratio > view_ratio) {
            // 左右黑边
            size_x = view_ratio / window_ratio;
            pos_x = (1.f - size_x) / 2.f;
        } else {
            // 上下黑边
            size_y = window_ratio / view_ratio;
            pos_y = (1.f - size_y) / 2.f;
        }

        view.setViewport({{pos_x, pos_y}, {size_x, size_y}});
    };

    fix_view(world_view_);
    fix_view(ui_view_);
}

sf::Vector2f Camera::world_to_screen(const sf::Vector2f& world_pos) const {
    return static_cast<sf::Vector2f>(window_obs_->mapCoordsToPixel(world_pos));
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
    // 先设置 world_view，确保 mapPixelToCoords 使用正确的 view 进行转换
    // （因为渲染后窗口的 view 可能被设置为 ui_view）
    window_obs_->setView(world_view_);
    return window_obs_->mapPixelToCoords(static_cast<sf::Vector2i>(screen_pos));
}
} // namespace engine::render