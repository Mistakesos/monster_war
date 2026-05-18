#pragma once
#include "game/defs/events.hpp"
#include <entt/signal/fwd.hpp>
#include <entt/entity/fwd.hpp>

namespace game::factory {
    class EntityFactory;
} // namespace game::factory

namespace game::system {

/**
 * @brief 技能系统
 * @note 用于管理技能的施放与显示、Buff增删等操作
 */
class SkillSystem {
public:
    SkillSystem(entt::registry& registry, entt::dispatcher& dispatcher, game::factory::EntityFactory& entity_factory);
    ~SkillSystem();

private:
    // 事件回调函数
    void on_skill_ready_event(const game::defs::SkillReadyEvent& event);
    void on_skill_active_event(const game::defs::SkillActiveEvent& event);
    void on_skill_duration_end_event(const game::defs::SkillDurationEndEvent& event);
    void on_remove_unit_event(const game::defs::RemovePlayerUnitEvent& event);

    // Buff增删函数
    void add_buff(entt::entity entity, entt::id_type skill_id);
    void remove_buff(entt::entity entity, entt::id_type skill_id);

    entt::registry& registry_;
    entt::dispatcher& dispatcher_;
    game::factory::EntityFactory& entity_factory_;
};

}   // namespace game::system