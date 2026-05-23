#include "game/system/game_rule_system.hpp"
#include "game/data/game_stats.hpp"
#include "game/factory/blueprint_manager.hpp"
#include "game/component/cost_regen_component.hpp"
#include "game/component/stats_component.hpp"
#include "game/component/class_name_component.hpp"
#include "engine/component/transform_component.hpp"
#include "engine/utils/math.hpp"
#include "engine/utils/events.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::system {

GameRuleSystem::GameRuleSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : registry_{registry}
    , dispatcher_{dispatcher} {
    dispatcher_.sink<game::defs::EnemyArriveHomeEvent>().connect<&GameRuleSystem::on_enemy_arrive_home>(this);
    dispatcher_.sink<game::defs::UpgradeUnitEvent>().connect<&GameRuleSystem::on_upgrade_unit_event>(this);
    dispatcher_.sink<game::defs::RetreatEvent>().connect<&GameRuleSystem::on_retreat_event>(this);
    dispatcher_.sink<game::defs::LevelClearDelayedEvent>().connect<&GameRuleSystem::on_level_clear_delayed_event>(this);
}

GameRuleSystem::~GameRuleSystem() {
    dispatcher_.disconnect(this);
}

void GameRuleSystem::update(sf::Time delta) {
    // 更新Cost
    auto& game_stats = registry_.ctx().get<game::data::GameStats&>();
    game_stats.cost_ += game_stats.cost_gen_per_second_ * delta.asSeconds();
    // 更新COST恢复
    auto view_cost_regen = registry_.view<game::component::CostRegenComponent>();
    for (auto&& [entity, cost_regen] : view_cost_regen->each()) {
        game_stats.cost_ += cost_regen.rate_ * delta.asSeconds();
    }
    // 如果已经通关，计时器归零后切换场景
    if (is_level_clear_) {
        level_clear_timer_ -= delta;
        if (level_clear_timer_ <= sf::Time::Zero) {
            dispatcher_.enqueue(game::defs::LevelClearEvent{});
            is_level_clear_ = false;    // 重置关卡通关标志, 避免重复触发
        }
    }
}

void GameRuleSystem::on_enemy_arrive_home(const game::defs::EnemyArriveHomeEvent&) {
    spdlog::info("敌人到达基地");
    auto& game_stats = registry_.ctx().get<game::data::GameStats&>();
    game_stats.enemy_arrived_count_++;      // 敌人到达数量+1
    game_stats.home_hp_ -= 1;               // 基地血量-1
    if (game_stats.home_hp_ <= 0) {
        spdlog::warn("基地被摧毁");
        // 游戏失败
        dispatcher_.enqueue(game::defs::GameEndEvent{false});
    }
    else if ((game_stats.enemy_arrived_count_ + game_stats.enemy_killed_count_) >= game_stats.enemy_count_) {
        // 通关成功，延迟切换场景
        dispatcher_.enqueue(game::defs::LevelClearDelayedEvent{});
    }
}

void GameRuleSystem::on_upgrade_unit_event(const game::defs::UpgradeUnitEvent& event) {
    if (event.entity_ == entt::null || !registry_.valid(event.entity_)) return;
    // 扣除COST
    auto& game_stats = registry_.ctx().get<game::data::GameStats&>();
    game_stats.cost_ -= event.cost_;
    // 获取Stats组件并让其等级 + 1
    auto& stats = registry_.get<game::component::StatsComponent>(event.entity_);
    stats.level_++;
    // 更新属性 (需要从蓝图中获取基础数据，然后根据等级和稀有度修改Stats组件)
    auto& blueprint_mgr = registry_.ctx().get<std::shared_ptr<game::factory::BlueprintManager>>();
    const auto& class_name = registry_.get<game::component::ClassNameComponent>(event.entity_);
    const auto& stats_blueprint = blueprint_mgr->get_player_class_blueprint(class_name.class_id_).stats_;
    stats.hp_ = engine::utils::stat_modify(stats_blueprint.hp_, stats.level_, stats.rarity_);
    stats.max_hp_ = engine::utils::stat_modify(stats_blueprint.hp_, stats.level_, stats.rarity_);
    stats.atk_ = engine::utils::stat_modify(stats_blueprint.atk_, stats.level_, stats.rarity_);
    stats.def_ = engine::utils::stat_modify(stats_blueprint.def_, stats.level_, stats.rarity_);
    // 创建特效
    const auto& transform = registry_.get<engine::component::TransformComponent>(event.entity_);
    dispatcher_.enqueue(game::defs::EffectEvent{"level_up"_hs, transform.position_, false});
    // 播放音效
    dispatcher_.enqueue(engine::utils::PlaySoundEvent{event.entity_, "level_up"_hs});
}

void GameRuleSystem::on_retreat_event(const game::defs::RetreatEvent& event) {
    if (event.entity_ == entt::null || !registry_.valid(event.entity_)) return;
    // 返还COST
    auto& game_stats = registry_.ctx().get<game::data::GameStats&>();
    game_stats.cost_ += event.cost_;
    // 发送移除单位事件
    dispatcher_.enqueue(game::defs::RemovePlayerUnitEvent{event.entity_});
}

void GameRuleSystem::on_level_clear_delayed_event(const game::defs::LevelClearDelayedEvent& event) {
    // 设置关卡通关标志和计时器
    is_level_clear_ = true;
    level_clear_timer_ = event.delay_time_;
}
}   // namespace game::system