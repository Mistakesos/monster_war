#include "engine/system/render_system.hpp"
#include "engine/render/render.hpp"
#include "engine/render/camera.hpp"
#include "engine/component/transform_component.hpp"
#include "engine/component/sprite_component.hpp"
#include "engine/component/render_component.hpp"
#include <spdlog/spdlog.h>

namespace engine::system {

void RenderSystem::update(entt::registry& registry, render::Renderer& render, const render::Camera& camera) {
    // 对RenderComponent进行排序 (需要自定义RenderComponent的比较运算符)
    registry.sort<component::RenderComponent>([](const auto& lhs, const auto& rhs) {
        return lhs < rhs;
    });

    auto view = registry.view<component::RenderComponent, component::TransformComponent, component::SpriteComponent>();
    view.use<component::RenderComponent>();     // 重要！这里要使用 RenderComponent 的顺序
    for (auto&& [entity, _, transform, sprite] : view.each()) {
        auto& s = sprite.sprite_;
        s.setOrigin(transform.origin_);
        s.setRotation(transform.angle_);
        s.setPosition(transform.position_);
        s.setScale(transform.scale_);
        render.draw_sprite(camera, s);
    }
}

} // namespace engine::system 