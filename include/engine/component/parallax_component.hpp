#pragma once
#include "component.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <string>

namespace engine::component {
class TransformComponent;
/**
 * @brief 在背景中渲染可滚动纹理的组件，以创建视差效果
 * 
 * 该组件根据相机的位置和滚动因子来移动纹理
 */
class ParallaxComponent final : public Component {
    friend class engine::object::GameObject;
public:
    ParallaxComponent(engine::object::GameObject* owner
                    , const sf::Texture& texture
                    , sf::Vector2f scroll_factor
                    , sf::Vector2<bool> repeat = {true, false});
    ~ParallaxComponent();

    // --- setter ---
    void set_sprite(sf::Sprite& sprite) { sprite_ = sprite; }                           ///< @brief 设置精灵对象
    void set_scroll_factor(sf::Vector2f factor) { scroll_factor_ = std::move(factor); } ///< @brief 设置滚动速度因子
    void set_repeat(sf::Vector2<bool> repeat) { repeat_ = std::move(repeat); }          ///< @brief 设置是否重复
    void set_hidden(bool hidden) { is_hidden_ = hidden; }                               ///< @brief 设置是否隐藏（不渲染）

    // --- getter ---
    const sf::Sprite& get_sprite() const { return sprite_; }                            ///< @brief 获取精灵对象
    const sf::Vector2f& get_scroll_factor() const { return scroll_factor_; }            ///< @brief 获取滚动速度因子
    const sf::Vector2<bool>& get_repeat() const { return repeat_; }                     ///< @brief 获取是否重复
    bool is_hidden() const { return is_hidden_; }                                       ///< @brief 获取是否隐藏（不渲染）
    
protected:
    // 核心循环函数覆盖
    void update(sf::Time, engine::core::Context&) override {}     // 必须实现纯虚函数，留空
    void render(engine::core::Context& context) override;      

private:
    TransformComponent* transform_obs_ = nullptr;           ///< @brief 缓存变换组件指针

    sf::Sprite sprite_;                 ///< @brief 内部维护的精灵
    sf::Vector2f scroll_factor_;        ///< @brief 滚动速度因子 (0=静止, 1=随相机移动, <1=比相机慢)
    sf::Vector2<bool> repeat_;          ///< @brief 是否沿着X和Y轴周期性重复
    bool is_hidden_ = false;            ///< @brief 是否隐藏（不渲染）
};
} // namespace engine::component