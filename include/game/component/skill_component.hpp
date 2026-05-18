#pragma once
#include <SFML/System/Time.hpp>
#include <entt/entity/entity.hpp>
#include <string>

namespace game::component {

/**
 * @brief 技能组件
 * @note 用于存储技能信息，包括技能ID、用于显示特效的实体ID、名称、描述、冷却时间、计时器等。
 */
struct SkillComponent {
    entt::id_type skill_id_{entt::null};        ///< 技能ID
    entt::entity display_entity_{entt::null};   ///< 用于显示特效的实体ID
    std::string name_;                          ///< @brief 技能名称
    std::string description_;                   ///< @brief 技能描述
    sf::Time cooldown_{sf::Time::Zero};         ///< @brief 技能冷却时间
    sf::Time duration_{sf::Time::Zero};         ///< @brief 技能持续时间
    sf::Time cooldown_timer_{sf::Time::Zero};   ///< @brief 技能冷却计时器
    sf::Time duration_timer_{sf::Time::Zero};   ///< @brief 技能持续计时器
};

}   // namespace game::component