#pragma once
#include <SFML/System/Time.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <deque>    // 双端队列：两端都可以入队或出队

namespace game::factory {
    class EntityFactory;
}

namespace game::spawner {

/**
 * @brief 敌人生成器，根据波次数据生成敌人
 */
class EnemySpawner {
public:
    /**
     * @brief 构造函数
     * @param registry entt注册表
     * @param entity_factory 实体工厂
     */
    EnemySpawner(entt::registry& registry, game::factory::EntityFactory& entity_factory);
    ~EnemySpawner();

    void update(sf::Time delta);

private:
    void spawn_enemy();

    entt::registry& registry_;
    game::factory::EntityFactory& entity_factory_;

    sf::Time spawn_timer_{sf::Time::Zero};      ///< @brief 波次内生成计时器 (单位：秒)
    sf::Time spawn_interval_{sf::Time::Zero};   ///< @brief 波次内生成间隔 (单位：秒)
    std::deque<entt::id_type> enemy_types_;     ///< @brief 波次内敌人队列 (使用双端队列是为了支持随机打乱顺序)
};

}   // namespace game::spawner