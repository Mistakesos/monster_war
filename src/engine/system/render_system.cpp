#include "engine/system/render_system.hpp"
#include "engine/render/render.hpp"
#include "engine/render/camera.hpp"
#include "engine/component/transform_component.hpp"
#include "engine/component/sprite_component.hpp"
#include <spdlog/spdlog.h>

namespace engine::system {

void RenderSystem::update(entt::registry& registry, render::Renderer& render, const render::Camera& camera) {
    auto view = registry.view<component::TransformComponent, component::SpriteComponent>();

    for (auto&& [entity, transform, sprite] : view.each()) {
        auto& s = sprite.sprite_;
        s.setOrigin(transform.origin_);
        s.setRotation(transform.angle_);
        s.setPosition(transform.position_);
        s.setScale(transform.scale_);
        render.draw_sprite(camera, s);
    }
}

} // namespace engine::system 