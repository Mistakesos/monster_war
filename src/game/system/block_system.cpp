#include "game/system/block_system.hpp"
#include "game/component/blocker_component.hpp"
#include "game/component/enemy_component.hpp"
#include "game/component/blocked_by_component.hpp"
#include "game/defs/tags.hpp"
#include "game/defs/constants.hpp"
#include "engine/component/transform_component.hpp"
#include "engine/component/velocity_component.hpp"
#include "engine/utils/events.hpp"
#include "engine/utils/math.hpp"
#include <entt/entity/view.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::system {

void BlockSystem::update(entt::registry& registry, entt::dispatcher& dispatcher) {
    spdlog::trace("BlockSystem::update");

    // 1. 清理失效的阻挡关系
    auto view_blocked_by = registry.view<game::component::BlockedByComponent>();
    for (auto&& [blocked_entity, blocked_by] : view_blocked_by.each()) {
        // 如果阻挡者实体已经无效（死亡/销毁），移除被阻挡组件，并让敌人恢复行走动画
        if (!registry.valid(blocked_by.entity_)) {
            registry.remove<game::component::BlockedByComponent>(blocked_entity);
            dispatcher.enqueue(engine::utils::PlayAnimationEvent{blocked_entity, "walk"_hs, true});
            spdlog::info("敌人: ID: {}, 的阻挡者已失效，移除阻挡状态", entt::to_integral(blocked_entity));
        }
    }

    // 2. 尝试建立新的阻挡关系
    auto view_blocker = registry.view<game::component::BlockerComponent,
                                      engine::component::TransformComponent>();
    auto view_enemy = registry.view<game::component::EnemyComponent,
                                    engine::component::TransformComponent,
                                    engine::component::VelocityComponent>(
                                    entt::exclude<game::component::BlockedByComponent>);
    // 遍历每一个尚未被阻挡的敌人
    for (auto&& [enemy_entity, _, enemy_transform, enemy_velocity] : view_enemy.each()) {
        // 遍历所有阻挡者，检查是否进入了阻挡半径
        for (auto&& [blocker_entity, blocker, blocker_transform] : view_blocker.each()) {
            // 利用 SFML 内置方法计算距离的平方，避免开方
            if ((enemy_transform.position_ - blocker_transform.position_).lengthSquared() <
                game::defs::BLOCK_RADIUS * game::defs::BLOCK_RADIUS) {

                // 阻挡者已达最大阻挡数，跳过
                if (blocker.current_count_ >= blocker.max_count_) {
                    continue;
                }

                blocker.current_count_++;                             // 增加阻挡计数
                enemy_velocity.velocity_ = sf::Vector2f(0.f, 0.f);    // 敌人停下
                registry.emplace<game::component::BlockedByComponent>(enemy_entity, blocker_entity);
                spdlog::info("敌人: ID: {}, 被阻挡, 阻挡者: ID: {}",
                             entt::to_integral(enemy_entity), entt::to_integral(blocker_entity));
                dispatcher.enqueue(engine::utils::PlayAnimationEvent{enemy_entity, "attack"_hs, true});
                break; // 一个敌人只需要被一个阻挡者阻挡即可，跳出内层循环
            }
        }
    }
}

}   // namespace game::system