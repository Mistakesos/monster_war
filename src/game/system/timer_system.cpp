#include "game/system/timer_system.hpp"
#include "game/component/stats_component.hpp"
#include "game/defs/tags.hpp"
#include <entt/entity/registry.hpp>

namespace game::system {

void TimerSystem::update(entt::registry& registry, sf::Time delta_time) {
    update_attack_timer(registry, delta_time);
}

void TimerSystem::update_attack_timer(entt::registry& registry, sf::Time delta_time) {
    // 筛选条件：有StatsComponent组件，但没有AttackReadyTag标签（即攻击正在冷却）
    auto view_unit = registry.view<game::component::StatsComponent>(entt::exclude<game::defs::AttackReadyTag>);
    for (auto&& [entity, stats] : view_unit.each()) {
        stats.atk_timer_ += delta_time;     // 推进计时器
        // 如果攻击计时器大于等于攻击间隔，代表冷却结束。添加“可攻击”标签，并重置攻击计时器
        if (stats.atk_timer_ >= stats.atk_interval_) {
            registry.emplace_or_replace<game::defs::AttackReadyTag>(entity);
            stats.atk_timer_ = sf::Time::Zero;
        }
    }
}

}   // namespace game::system