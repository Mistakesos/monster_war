#include "game/data/ui_config.hpp"
#include "engine/resource/resource_manager.hpp"
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

namespace game::data {

UIConfig::UIConfig(engine::resource::ResourceManager* resource_manager)
    : resource_manager_obs_{resource_manager} {
}

UIConfig::~UIConfig() = default;

bool UIConfig::load_from_file(std::string_view path) {
    std::filesystem::path file_path(path);
    std::ifstream file(file_path);
    nlohmann::json json;
    file >> json;

    try {
        load_icon(json["icon"]);
        load_portrait(json["portrait"]);
        load_portrait_frame(json["portrait_frame"]);
        load_layout(json["layout"]);
    } catch (const std::exception& e) {
        spdlog::error("载入 UI config 失败: {}", e.what());
        return false;
    }
    return true;
}

void UIConfig::load_icon(nlohmann::json& json) {
    for (auto& [key, value] : json.items()) {
        entt::id_type id = entt::hashed_string(key.c_str());
        auto texture_path = value["sprite_sheet"].get<std::string>();
        sf::IntRect src_rect = {{(value["x"].get<int>()), 
            (value["y"].get<int>())}, 
            {(value["width"].get<int>()), 
            (value["height"].get<int>())}};
        auto* texture = resource_manager_obs_->load_texture(entt::hashed_string(texture_path.c_str()));
        sf::Sprite spr(*texture);
        spr.setTextureRect(src_rect);
        icon_map_.try_emplace(id, std::move(spr));
    }
}

void UIConfig::load_portrait(nlohmann::json& json) {
    for (auto& [key, value] : json.items()) {
        entt::id_type id = entt::hashed_string(key.c_str());
        auto texture_path = value["sprite_sheet"].get<std::string>();
        sf::IntRect src_rect = {{(value["x"].get<int>()), 
            (value["y"].get<int>())}, 
            {(value["width"].get<int>()), 
            (value["height"].get<int>())}};
        auto* texture = resource_manager_obs_->load_texture(entt::hashed_string(texture_path.c_str()));
        sf::Sprite spr(*texture);
        spr.setTextureRect(src_rect);
        portrait_map_.try_emplace(id, std::move(spr));
    }
}

void UIConfig::load_portrait_frame(nlohmann::json& json) {
    for (auto& [key, value] : json.items()) {
        auto texture_path = value["sprite_sheet"].get<std::string>();
        int level = value["level"].get<int>();
        sf::IntRect src_rect = {{(value["x"].get<int>()), 
            (value["y"].get<int>())}, 
            {(value["width"].get<int>()), 
            (value["height"].get<int>())}};
        auto* texture = resource_manager_obs_->load_texture(entt::hashed_string(texture_path.c_str()));
        sf::Sprite spr(*texture);
        spr.setTextureRect(src_rect);
        portrait_frame_map_.try_emplace(level, std::move(spr));
    }
}

void UIConfig::load_layout(nlohmann::json& json) {
    unit_panel_padding_ = json["unit_panel"]["padding"].get<float>();
    unit_panel_frame_size_ = {json["unit_panel"]["frame_size"]["width"].get<float>(), 
                              json["unit_panel"]["frame_size"]["height"].get<float>()};
    unit_panel_font_size_ = json["unit_panel"]["font_size"].get<int>();
    unit_panel_font_path_ = json["unit_panel"]["font_path"].get<std::string>();
    unit_panel_font_offset_ = {json["unit_panel"]["font_offset"]["x"].get<float>(), 
                               json["unit_panel"]["font_offset"]["y"].get<float>()};
}

sf::Sprite& UIConfig::get_icon(entt::id_type id) {
    if (auto it = icon_map_.find(id); it != icon_map_.end()) {
        return it->second;
    } else {
        spdlog::error("Icon 未找到: {}", id);
        return icon_map_.begin()->second;
    }
}

sf::Sprite& UIConfig::get_portrait(entt::id_type id) {
    if (auto it = portrait_map_.find(id); it != portrait_map_.end()) {
        return it->second;
    } else {
        spdlog::error("Portrait 未找到: {}", id);
        return portrait_map_.begin()->second;
    }
}

sf::Sprite& UIConfig::get_portrait_frame(int rarity) {
    if (auto it = portrait_frame_map_.find(rarity); it != portrait_frame_map_.end()) {
        return it->second;
    } else {
        spdlog::error("Portrait Frame 未找到: {}", rarity);
        return portrait_frame_map_.begin()->second;
    }
}

} // namespace game::data