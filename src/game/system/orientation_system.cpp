#include "game/system/orientation_system.hpp"
#include "game/component/enemy_component.hpp"
#include "game/component/target_component.hpp"
#include "game/component/blocked_by_component.hpp"
#include "game/defs/tags.hpp"
#include "engine/component/velocity_component.hpp"
#include "engine/component/sprite_component.hpp"
#include "engine/component/transform_component.hpp"
#include <entt/entity/registry.hpp>
#include <cmath>

namespace game::system {

void OrientationSystem::update(entt::registry& registry) {
    update_has_target(registry);
    update_blocked(registry);
    update_moving(registry);     // 移动处理最后调用，确保优先级最高
}

void OrientationSystem::update_has_target(entt::registry& registry) {
    // 有目标的角色（包括玩家和敌方），面朝目标
    auto view_has_target = registry.view<game::component::TargetComponent, 
        engine::component::TransformComponent, 
        engine::component::SpriteComponent>();
    for (auto&& [entity, target, transform, sprite] : view_has_target.each()) {
        const auto& target_transform = registry.get<engine::component::TransformComponent>(target.entity_);
        // 根据目标位置和自身位置的 x 差值符号，判断朝向
        bool face_right = target_transform.position_.x > transform.position_.x;
        // 通过修改 transform.scale_.x 的符号来控制翻转（渲染时 setScale 会应用此值）
        float abs_scale_x = std::abs(transform.scale_.x);
        if (registry.all_of<game::defs::FaceLeftTag>(entity)) {
            // 默认朝左的角色：目标在右边时翻转(scale.x为负)，目标在左边时不翻转(scale.x为正)
            transform.scale_.x = face_right ? -abs_scale_x : abs_scale_x;
        } else {
            // 默认朝右的角色：目标在右边时不翻转(scale.x为正)，目标在左边时翻转(scale.x为负)
            transform.scale_.x = face_right ? abs_scale_x : -abs_scale_x;
        }
    }
}

void OrientationSystem::update_blocked(entt::registry& registry) {
    // 被阻挡的敌人角色，面朝阻挡者
    auto view_blocked = registry.view<game::component::BlockedByComponent, 
        engine::component::TransformComponent, 
        engine::component::SpriteComponent>();
    for (auto&& [entity, blocked_by, transform, sprite] : view_blocked.each()) {
        const auto& blocked_by_transform = registry.get<engine::component::TransformComponent>(blocked_by.entity_);
        bool face_right = blocked_by_transform.position_.x > transform.position_.x;
        float abs_scale_x = std::abs(transform.scale_.x);
        if (registry.all_of<game::defs::FaceLeftTag>(entity)) {
            transform.scale_.x = face_right ? -abs_scale_x : abs_scale_x;
        } else {
            transform.scale_.x = face_right ? abs_scale_x : -abs_scale_x;
        }
    }
}

void OrientationSystem::update_moving(entt::registry& registry) {
    // 移动中的敌人角色，面朝移动方向
    auto view_moving = registry.view<engine::component::VelocityComponent, 
        game::component::EnemyComponent,
        engine::component::SpriteComponent>(entt::exclude<game::component::BlockedByComponent, game::defs::ActionLockTag>);
    for (auto&& [entity, velocity, _, sprite] : view_moving.each()) {
        // 根据速度的 x 分量符号判断朝向
        bool face_right = velocity.velocity_.x > 0.f;
        auto& transform = registry.get<engine::component::TransformComponent>(entity);
        float abs_scale_x = std::abs(transform.scale_.x);
        if (registry.all_of<game::defs::FaceLeftTag>(entity)) {
            transform.scale_.x = face_right ? -abs_scale_x : abs_scale_x;
        } else {
            transform.scale_.x = face_right ? abs_scale_x : -abs_scale_x;
        }
    }
}

}   // namespace game::system