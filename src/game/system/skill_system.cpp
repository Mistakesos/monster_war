#include "game/system/skill_system.hpp"
#include "game/defs/events.hpp"
#include "game/defs/tags.hpp"
#include "game/factory/entity_factory.hpp"
#include "game/factory/blueprint_manager.hpp"
#include "game/component/skill_component.hpp"
#include "game/component/cost_regen_component.hpp"
#include "game/component/stats_component.hpp"
#include "engine/component/transform_component.hpp"
#include "engine/utils/events.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace game::system {

SkillSystem::SkillSystem(entt::registry& registry, entt::dispatcher& dispatcher, game::factory::EntityFactory& entity_factory)
    : registry_{registry}
    , dispatcher_{dispatcher}
    , entity_factory_{entity_factory} {
    dispatcher_.sink<game::defs::SkillReadyEvent>().connect<&SkillSystem::on_skill_ready_event>(this);
    dispatcher_.sink<game::defs::SkillActiveEvent>().connect<&SkillSystem::on_skill_active_event>(this);
    dispatcher_.sink<game::defs::SkillDurationEndEvent>().connect<&SkillSystem::on_skill_duration_end_event>(this);
    dispatcher_.sink<game::defs::RemovePlayerUnitEvent>().connect<&SkillSystem::on_remove_unit_event>(this);
}

SkillSystem::~SkillSystem() {
    dispatcher_.disconnect(this);
}

// 事件回调函数
void SkillSystem::on_skill_ready_event(const game::defs::SkillReadyEvent& event) {
    if (event.entity_ == entt::null || !registry_.valid(event.entity_)) return;
    // 获取技能和位置组件
    auto& skill = registry_.get<game::component::SkillComponent>(event.entity_);
    const auto& transform = registry_.get<engine::component::TransformComponent>(event.entity_);
    // 先删除可能存在的显示实体
    if (skill.display_entity_ != entt::null && registry_.valid(skill.display_entity_)) {
        registry_.emplace_or_replace<game::defs::DeadTag>(skill.display_entity_);
    }
    // 创建新的显示实体 (技能准备就绪)
    skill.display_entity_ = entity_factory_.create_skill_display("skill_ready"_hs, transform.position_ + game::defs::SKILL_DISPLAY_OFFSET);
}

void SkillSystem::on_skill_active_event(const game::defs::SkillActiveEvent& event) {
    if (event.entity_ == entt::null || !registry_.valid(event.entity_)) return;
    // 如果技能未就绪，则返回
    if (!registry_.any_of<game::defs::SkillReadyTag>(event.entity_)) return;

    // 获取技能和位置组件
    auto& skill = registry_.get<game::component::SkillComponent>(event.entity_);
    const auto& transform = registry_.get<engine::component::TransformComponent>(event.entity_);
    // 删除可能存在的显示实体
    if (skill.display_entity_ != entt::null && registry_.valid(skill.display_entity_)) {
        registry_.emplace_or_replace<game::defs::DeadTag>(skill.display_entity_);
    }
    // 创建新的显示实体 (技能激活)
    skill.display_entity_ = entity_factory_.create_skill_display("skill_active"_hs, transform.position_ + game::defs::SKILL_DISPLAY_OFFSET);

    // 移除技能准备标签,添加技能激活标签
    registry_.remove<game::defs::SkillReadyTag>(event.entity_);
    registry_.emplace<game::defs::SkillActiveTag>(event.entity_);

    // 如果技能是盾御，且动作未锁定，则播放guard动画
    if (skill.skill_id_ == "shield"_hs && !registry_.any_of<game::defs::ActionLockTag>(event.entity_)) {
        dispatcher_.enqueue(engine::utils::PlayAnimationEvent{event.entity_, "guard"_hs, true});
    }

    // 添加Buff
    add_buff(event.entity_, skill.skill_id_);
}


void SkillSystem::on_skill_duration_end_event(const game::defs::SkillDurationEndEvent& event) {
    if (event.entity_ == entt::null || !registry_.valid(event.entity_)) return;
    // 获取技能组件
    auto& skill = registry_.get<game::component::SkillComponent>(event.entity_);
    // 删除技能显示实体
    if (skill.display_entity_ != entt::null && registry_.valid(skill.display_entity_)) {
        registry_.emplace_or_replace<game::defs::DeadTag>(skill.display_entity_);
    }

    // 移除技能激活标签
    registry_.remove<game::defs::SkillActiveTag>(event.entity_);

    // 如果技能是盾御，且动作未锁定，则播放idle动画
    if (skill.skill_id_ == "shield"_hs && !registry_.any_of<game::defs::ActionLockTag>(event.entity_)) {
        dispatcher_.enqueue(engine::utils::PlayAnimationEvent{event.entity_, "idle"_hs, true});
    }

    // 移除Buff
    remove_buff(event.entity_, skill.skill_id_);
}

void SkillSystem::on_remove_unit_event(const game::defs::RemovePlayerUnitEvent& event) {
    if (event.entity_ == entt::null || !registry_.valid(event.entity_)) return;
    // 移除技能显示实体
    if (auto skill = registry_.try_get<game::component::SkillComponent>(event.entity_); skill) {
        if (skill->display_entity_ != entt::null && registry_.valid(skill->display_entity_)) {
            registry_.emplace_or_replace<game::defs::DeadTag>(skill->display_entity_);
        }
    }
}

// Buff增删函数
void SkillSystem::add_buff(entt::entity entity, entt::id_type skill_id) {
    if (entity == entt::null || !registry_.valid(entity)) return;

    // 获取Buff信息
    auto blueprint_mgr = registry_.ctx().get<std::shared_ptr<game::factory::BlueprintManager>>();
    const auto& skill_blueprint = blueprint_mgr->get_skill_blueprint(skill_id);
    const auto& buff_blueprint = skill_blueprint.buff_;

    // 将Buff应用到角色的Stats中
    auto& stats = registry_.get<game::component::StatsComponent>(entity);
    stats.hp_ *= buff_blueprint.hp_multiplier_;
    stats.atk_ *= buff_blueprint.atk_multiplier_;
    stats.def_ *= buff_blueprint.def_multiplier_;
    stats.range_ *= buff_blueprint.range_multiplier_;
    stats.atk_interval_ *= buff_blueprint.atk_interval_multiplier_;
    
    // 若存在Cost相关Buff，则添加COST恢复组件
    if (buff_blueprint.cost_regen_ > 0.f) {
        registry_.emplace_or_replace<game::component::CostRegenComponent>(entity, buff_blueprint.cost_regen_);
    }
}

void SkillSystem::remove_buff(entt::entity entity, entt::id_type skill_id) {
    if (entity == entt::null || !registry_.valid(entity)) return;
    // 获取Buff信息
    auto blueprint_mgr = registry_.ctx().get<std::shared_ptr<game::factory::BlueprintManager>>();
    const auto& skill_blueprint = blueprint_mgr->get_skill_blueprint(skill_id);
    const auto& buff_blueprint = skill_blueprint.buff_;

    // 从角色的Stats中移除Buff
    auto& stats = registry_.get<game::component::StatsComponent>(entity);
    stats.hp_ /= buff_blueprint.hp_multiplier_;
    stats.atk_ /= buff_blueprint.atk_multiplier_;
    stats.def_ /= buff_blueprint.def_multiplier_;
    stats.range_ /= buff_blueprint.range_multiplier_;
    stats.atk_interval_ /= buff_blueprint.atk_interval_multiplier_;

    // 若存在Cost相关Buff，则移除COST恢复组件
    if (buff_blueprint.cost_regen_ > 0.f) {
        registry_.remove<game::component::CostRegenComponent>(entity);
    }
}

}   // namespace game::system