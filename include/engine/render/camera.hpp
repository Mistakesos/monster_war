#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <optional>

namespace sf {
    class View;
    class Time;
    class RenderWindow;
} // namespace sf

namespace engine::component {
    class TransformComponent;
} // namespace engine::component

namespace engine::render {
/**
 * @brief 相机类负责管理相机位置和视口大小，并提供坐标转换功能。
 * 它还包含限制相机移动范围的边界。
 */
class Camera final {
public:
    /**
     * @brief 构造相机对象
     * @param viewport_size 视口大小
     * @param position 相机位置
     * @param limit_bounds 限制相机的移动范围
     */
    Camera(sf::RenderWindow* window, std::optional<sf::FloatRect> limit_bounds = std::nullopt);
    
    void update(sf::Time delta_time);                                       ///< @brief 更新相机位置
    void move(const sf::Vector2f& offset);                                  ///< @brief 移动相机

    sf::Vector2f world_to_screen(const sf::Vector2f& world_pos) const;      ///< @brief 世界坐标转屏幕坐标
    sf::Vector2f world_to_screen_with_parallax(const sf::Vector2f& world_pos, const sf::Vector2f& scroll_factor) const; ///< 世界坐标转屏幕坐标，考虑视差滚动
    sf::Vector2f screen_to_world(const sf::Vector2f& screen_pos) const;     ///< @brief 屏幕坐标转世界坐标

    void set_world_view_center(sf::Vector2f center);                        ///< @brief 设置世界摄像机中心
    void set_ui_view_center(sf::Vector2f center);                           ///< @brief 设置ui摄像机中心
    void set_limit_bounds(std::optional<sf::FloatRect> limit_bounds);       ///< @brief 设置限制相机的移动范围
    void set_target(engine::component::TransformComponent* target) { target_obs_ = target; }         ///< @brief 设置跟随目标变换组件

    const sf::Vector2f get_world_view_center() const { return world_view_.getCenter(); }             ///< @brief 获取世界摄像机中心位置
    const sf::Vector2f get_ui_view_center() const { return ui_view_.getCenter(); }                   ///< @brief 获取ui摄像机中心位置
    
    sf::Vector2f get_world_view_size() const { return world_view_.getSize(); }                       ///< @brief 获取世界视口大小
    sf::Vector2f get_ui_view_size() const { return ui_view_.getSize(); }                             ///< @brief 获取ui视口大小
    std::optional<sf::FloatRect> get_limit_bounds() const { return limit_bounds_; }                  ///< @brief 获取限制相机的移动范围
    engine::component::TransformComponent* get_target() const { return target_obs_; }                ///< @brief 获取跟随目标变换组件

    // 新增：获取SFML视图（用于设置到渲染窗口）
    const sf::View& get_world_view() const { return world_view_; }
    const sf::View& get_ui_view() const { return ui_view_; }

    // 禁用拷贝和移动语义
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(Camera&&) = delete;

private:
    void clamp_position();                                          ///< @brief 限制相机位置在边界内

    sf::RenderWindow* window_obs_;                                  ///< @brief 窗口的观察者指针，不管理其生命周期
    sf::View world_view_;                                           ///< @brief world（世界）摄像机
    sf::View ui_view_;                                              ///< @brief ui（界面）摄像机
    std::optional<sf::FloatRect> limit_bounds_;                     ///< @brief 限制相机的移动范围，空值表示不限制
    float smooth_speed_ = 5.f;                                      ///< @brief 相机移动的平滑速度
    engine::component::TransformComponent* target_obs_ = nullptr;   ///< @brief 跟随目标变换组件，空值表示不跟随
};
} // namespace engine::render