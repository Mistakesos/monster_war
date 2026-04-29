#include "game/factory/blueprint_manager.hpp"
#include "engine/resource/resource_manager.hpp"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <entt/core/hashed_string.hpp>
#include <filesystem>
#include <fstream>

namespace game::factory {

BlueprintManager::BlueprintManager(engine::resource::ResourceManager& resource_manager)
    : resource_manager_{resource_manager} {
}

bool BlueprintManager::load_player_class_blueprints(std::string_view player_json_path) {
    auto path = std::filesystem::path(player_json_path);
    std::ifstream file(path);
    nlohmann::json json;
    file >> json;
    file.close();
    // --- 解析蓝图 ---
    try {
        for (auto& [class_name, data_json] : json.items()) {
            entt::id_type class_id = entt::hashed_string(class_name.c_str());
            // 解析 Stats
            data::StatsBlueprint stats = parse_stats(data_json);
            // 解析 Sprite
            data::SpriteBlueprint sprite = parse_sprite(data_json);
            // 解析 Animation
            std::unordered_map<entt::id_type, data::AnimationBlueprint> animations = parse_animations_map(data_json);
            // 解析Sound
            data::SoundBlueprint sounds = parse_sound(data_json);
            // 解析Player数据
            data::PlayerBlueprint player = parse_player(data_json);
            // 解析DisplayInfo
            data::DisplayInfoBlueprint display_info = parse_display_info(data_json);
            // 解析完毕，组合蓝图并插入容器
            player_class_blueprints_.emplace(class_id, data::PlayerClassBlueprint{class_id, 
                class_name, 
                std::move(stats),
                std::move(player),
                std::move(sounds),
                std::move(sprite),
                std::move(display_info),
                std::move(animations)}
            );
        }
    } catch (const std::exception& e) {
        spdlog::error("加载玩家单位数据时出错: {}", e.what());
        return false;
    }
    return true;
}

bool BlueprintManager::load_enemy_class_blueprints(std::string_view enemy_json_path) {
    auto path = std::filesystem::path(enemy_json_path);
    std::ifstream file(path);
    nlohmann::json json;
    file >> json;
    file.close();
    // --- 解析蓝图 ---
    try {
        for (auto& [class_name, data_json] : json.items()) {
            entt::id_type class_id = entt::hashed_string(class_name.c_str());
            // 解析 Stats
            data::StatsBlueprint stats = parse_stats(data_json);
            // 解析 Sprite
            data::SpriteBlueprint sprite = parse_sprite(data_json);
            // 解析 Animation
            std::unordered_map<entt::id_type, data::AnimationBlueprint> animations = parse_animations_map(data_json);
            // 解析Sound
            data::SoundBlueprint sounds = parse_sound(data_json);
            // 解析Enemy数据
            data::EnemyBlueprint enemy = parse_enemy(data_json);
            // 解析DisplayInfo
            data::DisplayInfoBlueprint display_info = parse_display_info(data_json);
            // 解析完毕，组合蓝图并插入容器
            enemy_class_blueprints_.emplace(class_id, data::EnemyClassBlueprint{class_id, 
                class_name, 
                std::move(stats),
                std::move(enemy),
                std::move(sounds),
                std::move(sprite),
                std::move(display_info),
                std::move(animations)});
        }
    } catch (const std::exception& e) {
        spdlog::error("加载敌人单位数据时出错: {}", e.what());
        return false;
    }
    return true;
}

const data::PlayerClassBlueprint& BlueprintManager::get_player_class_blueprint(entt::id_type id) const {
    if (auto it = player_class_blueprints_.find(id); it != player_class_blueprints_.end()) {
        return it->second;
    }
    spdlog::error("未找到对应 id 的 PlayerClassBlueprint: {}", id);
    return player_class_blueprints_.begin()->second;
}

const data::EnemyClassBlueprint& BlueprintManager::get_enemy_class_blueprint(entt::id_type id) const {
    if (auto it = enemy_class_blueprints_.find(id); it != enemy_class_blueprints_.end()) {
        return it->second;
    }
    spdlog::error("未找到对应 id 的 EnemyClassBlueprint: {}", id);
    return enemy_class_blueprints_.begin()->second;
}

// --- 拆分步骤的私有解析函数 ---

data::StatsBlueprint BlueprintManager::parse_stats(const nlohmann::json& json) {
    return data::StatsBlueprint{json["hp"].get<float>(), 
        json["atk"].get<float>(), 
        json["def"].get<float>(), 
        json["range"].get<float>(),
        sf::seconds(json["atk_interval"].get<float>())};
}

data::SpriteBlueprint BlueprintManager::parse_sprite(const nlohmann::json& json) {
    auto width = json["width"].get<float>();
    auto height = json["height"].get<float>();
    auto path_str = json["sprite_sheet"].get<std::string>();
    auto path_id = entt::hashed_string(path_str.c_str());
    // 可选部分：源矩形的起点默认值为 0,0，渲染目标大小默认值为 width,height
    // （如果指定，起点为 x,y，渲染目标大小为 size_x,size_y）
    return data::SpriteBlueprint{path_id, 
        path_str, 
        sf::FloatRect{sf::Vector2f(json.value("x", 0.f), json.value("y", 0.f)), sf::Vector2f(width, height)}, 
        sf::Vector2f(json.value("size_x", width), json.value("size_y", height)),
        sf::Vector2f(json.value("origin_x", 0.f), json.value("origin_y", 0.f)),
        json.value("face_right", true)
    };
}

std::unordered_map<entt::id_type, data::AnimationBlueprint> BlueprintManager::parse_animations_map(const nlohmann::json& json) {
    std::unordered_map<entt::id_type, data::AnimationBlueprint> animations; // 先准备好容器
    for (auto& [anim_name, anim_data] : json["animation"].items()) {
        // 每轮循环解析一个具体的动画 （动画名，动画数据）
        auto anim_name_id = entt::hashed_string(anim_name.c_str());
        std::vector<int> frames = anim_data["frames"].get<std::vector<int>>();  // 动画帧信息数组可直接获取
        // 处理可能存在的事件信息
        std::unordered_map<int, entt::id_type> events;
        if (anim_data.contains("events")) {
            for (auto& [event_name, event_frame] : anim_data["events"].items()) {
                events.emplace(event_frame.get<int>(), entt::hashed_string(event_name.c_str()));
            }
        }
        // 创建单个动画蓝图，并插入容器
        data::AnimationBlueprint animation{sf::milliseconds(anim_data.value("duration", 100.f)), 
            anim_data.value("row", 0), 
            std::move(frames),
            std::move(events)
        };
        animations.emplace(anim_name_id, animation);
    }
    return animations;
}

data::SoundBlueprint BlueprintManager::parse_sound(const nlohmann::json& json) {
    data::SoundBlueprint sounds;
    if (json.contains("sounds")) {  // 如果包含音效
        // 遍历所有音效键值对
        for (auto& [sound_key, sound_value] : json["sounds"].items()) {
            // 先把 sound_value 看成是音效路径并通过资源管理器加载
            std::string sound_path = sound_value.get<std::string>();
            entt::id_type sound_id = entt::hashed_string(sound_path.c_str());
            resource_manager_.load_sound(sound_id, sound_path);
            // 将音效键值对转换为音效ID并插入到声音蓝图中
            sounds.sounds_.emplace(entt::hashed_string(sound_key.c_str()), sound_id);
        }
    }
    return sounds;
}

data::PlayerBlueprint BlueprintManager::parse_player(const nlohmann::json& json) {
    // 解析类型
    auto type_str = json["type"].get<std::string>();
    auto type = type_str == "melee" ? game::defs::PlayerType::Melee :   // 三目运算符嵌套
        type_str == "ranged" ? game::defs::PlayerType::Ranged : 
        type_str == "mixed" ? game::defs::PlayerType::Mixed : 
        game::defs::PlayerType::Unknown;
    // 解析技能
    entt::id_type skill_id = entt::null;
    if (json.contains("skill")) {
        skill_id = entt::hashed_string(json["skill"].get<std::string>().c_str());
    }
    // 解析其他数据并返回
    data::PlayerBlueprint player{type,
        skill_id,
        json["healer"].get<bool>(),
        json["block"].get<int>(),
        json["cost"].get<int>()};
    return player;
}

data::EnemyBlueprint BlueprintManager::parse_enemy(const nlohmann::json& json) {
    // 敌人组件蓝图只包含“是否远程”和“移动速度”
    return data::EnemyBlueprint{json["ranged"].get<bool>(), json["speed"].get<float>()};
}

data::DisplayInfoBlueprint BlueprintManager::parse_display_info(const nlohmann::json& json) {
    // 显示信息蓝图只包含“名称”和“描述”
    return data::DisplayInfoBlueprint{json.value("name", ""), json.value("description", "")};
}

}   // namespace game::factory