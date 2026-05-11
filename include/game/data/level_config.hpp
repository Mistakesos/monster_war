#pragma once
#include "game/data/level_data.hpp"
#include <string_view>

namespace game::data {

/**
 * @brief 关卡配置类
 * @note 负责载入json配置文件，并从中获取各类关卡数据
 */
class LevelConfig {
public:
    bool load_from_file(std::string_view level_json_path = "assets/data/level_config.json");  ///< @brief 加载关卡配置文件

    // --- getters （获取指定关卡编号的对应数据） --- （关卡编号从1开始，数组角标从0开始，因此每次获取时需要减1）
    [[nodiscard]] game::data::LevelData& get_level_data(int level_number) { return level_data_[level_number - 1]; }

    [[nodiscard]] game::data::Waves& get_waves_data(int level_number) { return level_data_[level_number - 1].waves_data_; }
    [[nodiscard]] int get_level_count() const { return level_data_.size(); }
    [[nodiscard]] std::string_view get_map_path(int level_number) const { return level_data_[level_number - 1].map_path_; }
    [[nodiscard]] int get_total_enemy_count(int level_number) const { return level_data_[level_number - 1].total_enemy_count_; }
    [[nodiscard]] bool is_final_level(int level_number) const { return level_number == get_level_count(); }
    [[nodiscard]] int get_enemy_level(int level_number) const { return level_data_[level_number - 1].enemy_level_; }
    [[nodiscard]] int get_enemy_rarity(int level_number) const { return level_data_[level_number - 1].enemy_rarity_; }

private:
    std::vector<game::data::LevelData> level_data_;     ///< @brief 关卡数据（每关对应一个LevelData）
};

}   // namespace game::data