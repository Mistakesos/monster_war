#pragma once
#include "game/data/entity_blueprint.hpp"
#include <SFML/System/Vector2.hpp>
#include <entt/entity/fwd.hpp>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace game::factory {

class BlueprintManager;
/**
 * @brief 实体工厂，用于创建不同类型的实体
 * 
 * 实体工厂通过蓝图管理器获取蓝图数据，并创建不同类型的实体。
 */
class EntityFactory {

public:
    /// @brief 实体工厂构造函数, 需要传入注册表和蓝图管理器。通过蓝图数据创建不同实体
    EntityFactory(entt::registry& registry, BlueprintManager& blueprint_manager);

    /**
     * @brief 创建玩家单位
     * @param class_id 职业ID
     * @param position 位置
     * @param level 等级
     * @param rarity 稀有度
     * @return 玩家单位实体
     */
    entt::entity create_player_unit(entt::id_type class_id, const sf::Vector2f& position, int level = 1, int rarity = 1);

    /**
     * @brief 创建敌人单位
     * @param class_id 敌人类型ID
     * @param position 位置
     * @param target_waypoint_id 目标路径点ID
     * @param level 等级
     * @param rarity 稀有度
     * @return 敌人单位实体
     */
    entt::entity create_enemy_unit(entt::id_type class_id, const sf::Vector2f& position, int target_waypoint_id, int level = 1, int rarity = 1);

    /**
     * @brief 创建投射物
     * @param id 投射物ID
     * @param start_position 起始位置
     * @param target_position 目标位置
     * @param target 目标实体
     * @param damage 伤害
     * @return 投射物实体
     */
    entt::entity create_projectile(entt::id_type id, const sf::Vector2f& start_position, const sf::Vector2f& target_position, entt::entity target, float damage);
    
    /**
     * @brief 创建单位准备类型实体
     * @param name_id 单位名称ID
     * @param class_id 单位ID
     * @param cost 费用
     * @param position 位置
     * @return 单位准备类型实体
     */
    entt::entity create_unit_prep(entt::id_type name_id, entt::id_type class_id, int cost, const sf::Vector2f& position);

    /**
     * @brief 创建敌人死亡特效
     * @note 敌人死亡特效直接从敌人蓝图中获取，对应的动画名称必须为“damage”。
     * @param class_id 敌人ID
     * @param position 位置
     * @param is_flipped 是否翻转
     * @return 敌人死亡特效实体
     */
    entt::entity create_enemy_dead_effect(entt::id_type class_id, const sf::Vector2f& position, const bool is_flipped = false);

    /**
     * @brief 创建（通用）特效，数据来自特效蓝图
     * @param effect_id 特效ID
     * @param position 位置
     * @param is_flipped 是否翻转
     * @return 特效实体
     */
    entt::entity create_effect(entt::id_type effect_id, const sf::Vector2f& position, const bool is_flipped = false);

    /**
     * @brief 创建技能显示实体
     * @param id 技能ID
     * @param position 位置
     * @return 技能显示实体
     */
    entt::entity create_skill_display(entt::id_type effect_id, const sf::Vector2f& position);
    // TODO: 未来添加其他实体的创建函数

private:
    // --- 组件创建函数 ---
    void add_transform_component(entt::entity entity, const sf::Vector2f& position, const sf::Vector2f& scale = sf::Vector2f(1.f, 1.f), sf::Angle rotation = sf::degrees(0.f));
    void add_sprite_component(entt::entity entity, const data::SpriteBlueprint& sprite, const bool is_flipped = false);
    void add_animation_component(entt::entity entity,       ///< @brief 正常动画组件添加（多个动画）
        const std::unordered_map<entt::id_type, data::AnimationBlueprint>& animation_blueprints, 
        const data::SpriteBlueprint& sprite_blueprint,
        entt::id_type default_animation_id);
    void add_one_animation_component(entt::entity entity,      ///< @brief 单个动画组件添加（组件中只包含一个动画），用于创建特效
        const data::AnimationBlueprint& animation_blueprint, 
        const data::SpriteBlueprint& sprite_blueprint,
        entt::id_type animation_id,
        bool loop = false);
    void add_stats_component(entt::entity entity, const data::StatsBlueprint& stats, int level = 1, int rarity = 1);
    void add_player_component(entt::entity entity, const data::PlayerBlueprint& player, int rarity);
    void add_enemy_component(entt::entity entity, const data::EnemyBlueprint& enemy, int target_waypoint_id);
    void add_audio_component(entt::entity entity, const data::SoundBlueprint& sounds);
    void add_projectile_id_component(entt::entity entity, entt::id_type id);
    void add_skill_component(entt::entity entity, entt::id_type skill_id);
    // TODO: 未来添加其他组件创建函数

    entt::registry& registry_;
    BlueprintManager& blueprint_manager_;
};

}   // namespace game::factory