#pragma once
#include "game/data/entity_blueprint.hpp"
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

    entt::entity create_player_unit(entt::id_type class_id, const sf::Vector2f& position, int level = 1, int rarity = 1);
    entt::entity create_enemy_unit(entt::id_type class_id, const sf::Vector2f& position, int target_waypoint_id, int level = 1, int rarity = 1);
    // TODO: 未来添加其他实体的创建函数

private:
    // --- 组件创建函数 ---
    void add_transform_component(entt::entity entity, const sf::Vector2f& position, const sf::Vector2f& scale = sf::Vector2f(1.f, 1.f), sf::Angle rotation = sf::degrees(0.f));
    void add_sprite_component(entt::entity entity, const data::SpriteBlueprint& sprite, const bool is_flipped = false);
    void add_animation_component(entt::entity entity, 
        const std::unordered_map<entt::id_type, data::AnimationBlueprint>& animation_blueprints, 
        const data::SpriteBlueprint& sprite_blueprint,
        entt::id_type default_animation_id);
    void add_stats_component(entt::entity entity, const data::StatsBlueprint& stats, int level = 1, int rarity = 1);
    void add_player_component(entt::entity entity, const data::PlayerBlueprint& player, int rarity);
    void add_enemy_component(entt::entity entity, const data::EnemyBlueprint& enemy, int target_waypoint_id);
    void add_audio_component(entt::entity entity, const data::SoundBlueprint& sounds);
    // TODO: 未来添加其他组件创建函数

    entt::registry& registry_;
    BlueprintManager& blueprint_manager_;
};

}   // namespace game::factory