#include "engine/system/movement_system.hpp"
#include "engine/component/velocity_component.hpp"
#include "engine/component/transform_component.hpp"
#include <spdlog/spdlog.h>

namespace engine::system {

void MovementSystem::update(entt::registry& registry, sf::Time delta) {
    spdlog::trace("MovementSystem::update");
    // 获取感兴趣的实体 view
    auto view = registry.view<engine::component::VelocityComponent, engine::component::TransformComponent>();

    // 遍历获取的实体，获取组件并执行相关逻辑
    for (auto&& [entity, velocity, transform] : view.each()) {
        transform.position_ += velocity.velocity_ * delta.asSeconds();
    }
}

}   // namespace engine::system