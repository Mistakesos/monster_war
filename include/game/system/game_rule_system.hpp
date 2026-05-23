#pragma once
#include "game/defs/events.hpp"
#include <SFML/System/Time.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace game::system {

/**
 * @brief 游戏规则系统
 * 
 * 负责处理游戏规则，如COST更新、敌人到达基地等。
 */
class GameRuleSystem {
public:
    GameRuleSystem(entt::registry& registry, entt::dispatcher& dispatcher);
    ~GameRuleSystem();

    void update(sf::Time delta);

private:
    // 事件回调函数
    void on_enemy_arrive_home(const game::defs::EnemyArriveHomeEvent& event);
    void on_upgrade_unit_event(const game::defs::UpgradeUnitEvent& event);
    void on_retreat_event(const game::defs::RetreatEvent& event);
    void on_level_clear_delayed_event(const game::defs::LevelClearDelayedEvent& event);

    entt::registry& registry_;
    entt::dispatcher& dispatcher_;

    bool is_level_clear_{false};                    ///< @brief 是否关卡通关
    sf::Time level_clear_timer_{sf::Time::Zero};    ///< @brief 关卡通关计时器(实现延迟切换场景)
};

}