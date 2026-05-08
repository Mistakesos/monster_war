#pragma once
#include <entt/entity/fwd.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>
#include <string_view>

namespace engine::resource {
    class ResourceManager;
} // namespace engine::resource

namespace game::data {

/**
 * @brief 管理UI配置数据。
 * 
 * 包含icon、portrait、portrait_frame、unit_panel的配置数据。
 */
class UIConfig {

public:
    UIConfig(engine::resource::ResourceManager* resource_manager);
    ~UIConfig();

    [[nodiscard]] bool load_from_file(std::string_view path = "assets/data/ui_config.json");   ///< @brief 从json配置文件加载数据

    // --- Getters ---
    [[nodiscard]] sf::Sprite& get_icon(entt::id_type id);
    [[nodiscard]] sf::Sprite& get_portrait(entt::id_type id);
    [[nodiscard]] sf::Sprite& get_portrait_frame(int rarity);
    [[nodiscard]] float get_unit_panel_padding() const { return unit_panel_padding_; }
    [[nodiscard]] sf::Vector2f get_unit_panel_frame_size() const { return unit_panel_frame_size_; }
    [[nodiscard]] int get_unit_panel_font_size() const { return unit_panel_font_size_; }
    [[nodiscard]] std::string get_unit_panel_font_path() const { return unit_panel_font_path_; }
    [[nodiscard]] sf::Vector2f get_unit_panel_font_offset() const { return unit_panel_font_offset_; }

    private:
    // --- 分步骤的数据加载函数 ---
    void load_icon(nlohmann::json& json);
    void load_portrait(nlohmann::json& json);
    void load_portrait_frame(nlohmann::json& json);
    void load_layout(nlohmann::json& json);

    engine::resource::ResourceManager* resource_manager_obs_{nullptr};
    
    /// @brief 储存职业类型icon的map
    std::unordered_map<entt::id_type, sf::Sprite> icon_map_;
    /// @brief 储存角色肖像的map
    std::unordered_map<entt::id_type, sf::Sprite> portrait_map_;
    /// @brief 储存角色肖像框的map（稀有度作为key）
    std::unordered_map<int, sf::Sprite> portrait_frame_map_;
    
    // --- 单位面板的配置数据（从json配置文件读取） ---
    float unit_panel_padding_{10.f};                    ///< @brief 单位面板间隔
    sf::Vector2f unit_panel_frame_size_{128.f, 128.f};  ///< @brief 单位面板大小
    int unit_panel_font_size_{40};                      ///< @brief 单位面板字体大小
    std::string unit_panel_font_path_;                  ///< @brief 单位面板字体路径
    sf::Vector2f unit_panel_font_offset_{16.f, 72.f};   ///< @brief 单位面板字体偏移
};

} // namespace game::data