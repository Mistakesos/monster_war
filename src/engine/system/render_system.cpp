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
        if (transform.rotation_ != sf::Angle::Zero) {
            // 对于旋转的实体（如子弹），使用纹理中心作为旋转原点
            auto center = s.getLocalBounds().getCenter();
            // 计算自定义原点与纹理中心的偏移量，补偿到位置上
            auto offset = center - transform.origin_;
            s.setOrigin(center);
            s.setPosition(transform.position_ + offset);
            s.setRotation(transform.rotation_);
        } else {
            s.setOrigin(transform.origin_);
            s.setPosition(transform.position_);
        }
        s.setScale(transform.scale_);
        render.draw_sprite(camera, s);
    }
}

} // namespace engine::system 