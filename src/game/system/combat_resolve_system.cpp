#include "game/system/combat_resolve_system.hpp"
#include "game/component/stats_component.hpp"
#include "game/component/player_component.hpp"
#include "game/component/enemy_component.hpp"
#include "game/component/blocked_by_component.hpp"
#include "game/component/blocker_component.hpp"
#include "game/component/class_name_component.hpp"
#include "engine/component/transform_component.hpp"
#include "engine/component/sprite_component.hpp"
#include "game/defs/tags.hpp"
#include "game/defs/events.hpp"
#include "game/data/game_stats.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::system {

CombatResolveSystem::CombatResolveSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : registry_(registry), dispatcher_(dispatcher) {
    dispatcher_.sink<game::defs::AttackEvent>().connect<&CombatResolveSystem::on_attack_event>(this);
    dispatcher_.sink<game::defs::HealEvent>().connect<&CombatResolveSystem::on_heal_event>(this);
}

CombatResolveSystem::~CombatResolveSystem() {
    dispatcher_.disconnect(this);
}

void CombatResolveSystem::on_attack_event(const game::defs::AttackEvent& event) {
    if (!registry_.valid(event.target_)) return;
    // 已经死亡的目标不再处理
    if (registry_.all_of<game::defs::DeadTag>(event.target_)) return;
    
    // 根据伤害公式，让目标扣血
    auto& target_stats = registry_.get<game::component::StatsComponent>(event.target_);
    float damage = calculate_effective_damage(event.damage_, target_stats.def_);
    target_stats.hp_ -= damage;

    // 如果目标是玩家
    if (registry_.all_of<game::component::PlayerComponent>(event.target_)) {
        spdlog::info("玩家 ID: {} 受到 ID: {} 的伤害, 剩余生命值: {}", 
            entt::to_integral(event.target_), entt::to_integral(event.attacker_), target_stats.hp_);
        // 死亡情况
        if (target_stats.hp_ <= 0) {
            target_stats.hp_ = 0;
            registry_.emplace<game::defs::DeadTag>(event.target_);
            spdlog::info("玩家 ID: {} 死亡", entt::to_integral(event.target_));
            // NOTE: 可添加死亡特效, 统计信息等
        // 受伤情况
        } else if (target_stats.hp_ < target_stats.max_hp_) {
            registry_.emplace_or_replace<game::defs::InjuredTag>(event.target_);
        }
        return;
    }

    // 如果目标是敌人
    if (registry_.all_of<game::component::EnemyComponent>(event.target_)) {
        spdlog::info("敌人 ID: {} 受到 ID: {} 的伤害, 剩余生命值: {}", 
            entt::to_integral(event.target_), entt::to_integral(event.attacker_), target_stats.hp_);
        // 死亡情况
        if (target_stats.hp_ <= 0) {
            target_stats.hp_ = 0;
            registry_.emplace<game::defs::DeadTag>(event.target_);
            spdlog::info("敌人 ID: {} 死亡", entt::to_integral(event.target_));

            // 发送死亡特效事件，需要先获取class_id、位置和是否翻转
            const auto [class_name, transform, sprite] = registry_.get<game::component::ClassNameComponent, 
                engine::component::TransformComponent, 
                engine::component::SpriteComponent>(event.target_);
            bool is_flipped = sprite.sprite_.getScale().x < 0 ? true : false;
            dispatcher_.enqueue(game::defs::EnemyDeadEffectEvent{class_name.class_id_, transform.position_, is_flipped});

            // 更新统计信息
            auto& game_stats = registry_.ctx().get<game::data::GameStats&>();
            game_stats.enemy_killed_count_++;       // 敌人击杀数量+1
            if ((game_stats.enemy_killed_count_ + game_stats.enemy_arrived_count_) >= game_stats.enemy_count_) {
                spdlog::warn("敌人全部死亡");
                // TODO: 切换场景逻辑
            }

            // 如果敌人被阻挡，减少阻挡者的阻挡计数
            if (auto blocked_by = registry_.try_get<game::component::BlockedByComponent>(event.target_); blocked_by) {
                auto blocker_entity = blocked_by->entity_;
                if (registry_.valid(blocker_entity)) {
                    auto& blocker = registry_.get<game::component::BlockerComponent>(blocker_entity);
                    blocker.current_count_ = std::max(0, blocker.current_count_ - 1);
                }
            }
        // 受伤情况
        } else if (target_stats.hp_ < target_stats.max_hp_) {
            registry_.emplace_or_replace<game::defs::InjuredTag>(event.target_);
        }
        return;
    }
}

void CombatResolveSystem::on_heal_event(const game::defs::HealEvent& event) {
    if (!registry_.valid(event.target_)) return;
    if (!registry_.all_of<game::component::PlayerComponent>(event.target_)) return;
    // 根据治疗量，让目标回血
    auto& target_stats = registry_.get<game::component::StatsComponent>(event.target_);
    target_stats.hp_ += event.amount_;
    spdlog::info("治疗者 ID: {}, 治疗目标 ID: {}, 治疗量: {}", 
        entt::to_integral(event.healer_), entt::to_integral(event.target_), event.amount_);
    // 如果治疗后满血，移除受伤标签
    if (target_stats.hp_ >= target_stats.max_hp_) {
        target_stats.hp_ = target_stats.max_hp_;
        registry_.remove<game::defs::InjuredTag>(event.target_);
    }
    // TODO: 添加治疗特效
}

// --- 辅助函数 ---
float CombatResolveSystem::calculate_effective_damage(float attacker_atk, float target_def) {
    // 最终伤害 = 攻击力 - 防御力
    float damage = attacker_atk - target_def;
    // 最小伤害为攻击力的10%
    damage = std::max(damage, 0.1f * attacker_atk);
    return damage;
}

} // namespace game::system