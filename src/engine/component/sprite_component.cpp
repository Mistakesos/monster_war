#include "engine/component/sprite_component.hpp"
#include "engine/component/transform_component.hpp"
#include "engine/object/game_object.hpp"
#include "engine/render/render.hpp"
#include "engine/render/camera.hpp"
#include "engine/core/context.hpp"
#include <spdlog/spdlog.h>

namespace engine::component {
SpriteComponent::SpriteComponent(engine::object::GameObject* owner, const sf::Texture& texture)
    : Component{owner}
    , sprite_{texture} {
    ///< @attention transform_obs_ 在渲染函数调用时初始化，并确保了只初始化一次
}

SpriteComponent::SpriteComponent(engine::object::GameObject* owner, sf::Sprite&& sprite) 
    : Component{owner}
    , sprite_{sprite} {
}

void SpriteComponent::render(engine::core::Context& context) {
    if (!transform_obs_) {
        transform_obs_ = owner_->get_component<TransformComponent>();
        if (!transform_obs_) {
            spdlog::warn(
                "GameObject '{}' 上的 SpriteComponent 需要一个 TransformComponent，但未找到。",
                owner_->get_name()
            );
        }
    }
    if (is_hidden_) {
        return;
    }

    // 获取变换信息（考虑偏移量）
    const sf::Vector2f& origin = transform_obs_->get_origin();
    const sf::Vector2f& position = transform_obs_->get_position();
    const sf::Vector2f& scale = transform_obs_->get_scale();
    sf::Angle rotation = transform_obs_->get_rotation();

    sprite_.setOrigin(origin);
    sprite_.setPosition(position);
    sprite_.setScale(scale);
    sprite_.setRotation(rotation);
    
    // 执行绘制
    context.get_renderer().draw_sprite(context.get_camera(), sprite_);
}
} // namespace engine::core