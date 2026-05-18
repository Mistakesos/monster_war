#pragma once
#include "game/data/entity_blueprint.hpp"
#include <entt/entity/fwd.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string_view>
#include <unordered_map>

namespace engine::resource {
    class ResourceManager;
} // namespace engine::resource

namespace game::factory {

/**
 * @brief 蓝图管理器，用于存储、管理所有蓝图
 * 
 * 它从json数据中加载蓝图并保存到容器，并和获取蓝图的功能。蓝图信息将由实体工厂使用。
 */
class BlueprintManager {
    friend class EntityFactory;
public:
    BlueprintManager(engine::resource::ResourceManager& resource_manager);

    [[nodiscard]] bool load_player_class_blueprints(std::string_view player_json_path);     ///< @brief 加载玩家职业蓝图, 返回是否成功
    [[nodiscard]] bool load_enemy_class_blueprints(std::string_view enemy_json_path);       ///< @brief 加载敌人类型蓝图, 返回是否成功
    [[nodiscard]] bool load_projectile_blueprints(std::string_view projectile_json_path);   ///< @brief 加载投射物蓝图, 返回是否成功
    [[nodiscard]] bool load_effect_blueprints(std::string_view effect_json_path);           ///< @brief 加载特效蓝图, 返回是否成功
    // TODO: 未来添加其他蓝图加载函数

    const data::PlayerClassBlueprint& get_player_class_blueprint(entt::id_type id) const;   ///< @brief 获取指定ID的玩家职业蓝图
    const data::EnemyClassBlueprint& get_enemy_class_blueprint(entt::id_type id) const;     ///< @brief 获取指定ID的敌人类型蓝图
    const data::ProjectileBlueprint& get_projectile_blueprint(entt::id_type id) const;      ///< @brief 获取指定ID的投射物蓝图
    const data::EffectBlueprint& get_effect_blueprint(entt::id_type id) const;              ///< @brief 获取指定ID的特效蓝图
    // TODO: 未来添加其他蓝图获取函数

private:
    // --- 分别针对各个子蓝图进行json解析，并创建(返回)对应的蓝图结构体 ---
    entt::id_type parse_projectile_id(const nlohmann::json& json);
    data::StatsBlueprint parse_stats(const nlohmann::json& json);
    data::SpriteBlueprint parse_sprite(const nlohmann::json& json);
    std::unordered_map<entt::id_type, data::AnimationBlueprint> parse_animations_map(const nlohmann::json& json);
    data::AnimationBlueprint parse_one_animation(const nlohmann::json& json);
    data::SoundBlueprint parse_sound(const nlohmann::json& json);
    data::PlayerBlueprint parse_player(const nlohmann::json& json);
    data::EnemyBlueprint parse_enemy(const nlohmann::json& json);
    data::DisplayInfoBlueprint parse_display_info(const nlohmann::json& json);

    engine::resource::ResourceManager& resource_manager_;

    std::unordered_map<entt::id_type, data::PlayerClassBlueprint> player_class_blueprints_; ///< @brief 玩家职业蓝图
    std::unordered_map<entt::id_type, data::EnemyClassBlueprint> enemy_class_blueprints_;   ///< @brief 敌人类型蓝图
    std::unordered_map<entt::id_type, data::ProjectileBlueprint> projectile_blueprints_;    ///< @brief 投射物蓝图
    std::unordered_map<entt::id_type, data::EffectBlueprint> effect_blueprints_;            ///< @brief 特效蓝图
    // TODO: 未来添加其他蓝图容器
};

}   // namespace game::factory