#include "engine/system/ysort_system.hpp"
#include "engine/component/render_component.hpp"
#include "engine/component/transform_component.hpp"
#include <entt/entity/registry.hpp>

namespace engine::system {

void YSortSystem::update(entt::registry& registry) {
    // 让RenderComponent的深度depth等于TransformComponent的y坐标
    auto view = registry.view<component::RenderComponent, const component::TransformComponent>();
    for (auto&& [entity, render, transform] : view.each()) {
        render.depth_ = transform.position_.y;
    }
}

}