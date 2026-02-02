#pragma once
#include "component.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Angle.hpp>

namespace engine::core {
    class Context;
} // namespace engine::core

namespace engine::resource {
    class ResourceManager;
} // namespace engine::resource

namespace engine::component {
/**
 * @class TransformComponent
 * @brief 管理 GameObject 的位置、旋转和缩放。
 */
class TransformComponent final : public Component {
    friend class engine::object::GameObject;   // 友元不能继承，必须每个子类单独添加
public:
    /**
     * @brief 构造函数
     * @param position 位置
     * @param scale 缩放
     * @param angle 旋转角度（兼容角度和弧度）
     */
    TransformComponent(engine::object::GameObject* owner
                     , sf::Vector2f position = {0.f, 0.f}
                     , sf::Vector2f scale = {1.f, 1.f}
                     , sf::Angle angle = sf::degrees(0.f)
                     , sf::Vector2f origin = {0.f, 0.f}
    );
    ~TransformComponent() override = default;

    // 禁止拷贝和移动
    TransformComponent(const TransformComponent&) = delete;
    TransformComponent& operator=(const TransformComponent&) = delete;
    TransformComponent(TransformComponent&&) = delete;
    TransformComponent& operator=(TransformComponent&&) = delete;

    // Getters and setters 
    const sf::Vector2f& get_position() const { return position_; }                ///< @brief 获取位置
    sf::Angle get_rotation() const { return angle_; }                             ///< @brief 获取旋转
    const sf::Vector2f& get_scale() const { return scale_; }                      ///< @brief 获取缩放
    sf::Vector2f get_origin() const { return origin_; }                           ///< @brief 获取原点
    void set_origin(sf::Vector2f origin) { origin_ = std::move(origin); }         ///< @brief 设置原点
    void set_position(sf::Vector2f position) { position_ = std::move(position); } ///< @brief 设置位置
    void set_rotation(sf::Angle angle) { angle_ = angle; }                        ///< @brief 设置旋转角度
    void set_scale(sf::Vector2f scale) { scale_ = std::move(scale); }             ///< @brief 设置缩放，应用缩放时应同步更新Sprite偏移量
    void translate(sf::Vector2f offset) { position_ += offset; }                  ///< @brief 移动（sf::Sprite::move)    

private:
    void update(sf::Time, engine::core::Context&) override {} ///< @brief 覆盖纯虚函数，这里不需要实现
    
    sf::Vector2f position_ = {0.f, 0.f};        ///< @brief 位置
    sf::Vector2f scale_ = {1.f, 1.f};           ///< @brief 缩放
    sf::Angle angle_ = sf::degrees(0.f);        ///< @brief 角度制，单位：度（约定，实际上也支持弧度）
    sf::Vector2f origin_ = {0.f, 0.f};          ///< @brief 原点
};
} // namespace engine::component