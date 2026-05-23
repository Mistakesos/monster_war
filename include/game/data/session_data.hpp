#pragma once
#include <entt/entity/entity.hpp>
#include <string_view>
#include <string>
#include <vector>
#include <unordered_map>

namespace game::data {

/**
 * @brief 角色数据
 * 
 * 包含角色名称、职业、等级、稀有度。
 */
struct UnitData {
    entt::id_type name_id_{entt::null};
    entt::id_type class_id_{entt::null};
    std::string name_;
    std::string class_;
    int level_{1};
    int rarity_{1};
};

/**
 * @brief 场景间（例如通关时）传递的关卡数据
 * 
 * 包含角色列表、积分、是否通关等关卡数据。
 */
class SessionData {
public:
    SessionData() = default;
    ~SessionData() = default;

    bool load_default_data(std::string_view path = "assets/data/default_session_data.json");    ///< @brief 加载默认数据
    bool load_from_file(std::string_view path);                                                 ///< @brief 加载文件数据(读档)
    bool save_to_file(std::string_view path);                                                   ///< @brief 保存文件数据(存档)

    void map_unit_data_list();     ///< @brief 将unit_map_中的数据映射到unit_data_list_中

    void add_unit(std::string_view name, std::string_view class_str, int level, int rarity);    ///< @brief 添加角色
    void remove_unit(entt::id_type name_id);                                                    ///< @brief 删除角色
    void add_unit_level(entt::id_type name_id, int add_level = 1);                              ///< @brief 增加角色等级
    void add_unit_rarity(entt::id_type name_id, int add_rarity = 1);                            ///< @brief 增加角色稀有度
    void clear_units();                                                                         ///< @brief 清空角色列表
    void clear();                                                                               ///< @brief 清空所有数据

    void add_point(int add_point) { point_ += add_point; }                                      ///< @brief 增加积分
    int add_one_level() { return ++level_number_; }                                             ///< @brief 增加关卡号(进入下一关)
    void set_level_clear(bool clear) { level_clear_ = clear; }                                  ///< @brief 设置是否通关

    // --- getters ---
    [[nodiscard]] std::unordered_map<entt::id_type, UnitData>& get_unit_map() { return unit_map_; }
    [[nodiscard]] std::vector<UnitData*>& get_unit_data_list() { return unit_data_list_; }
    [[nodiscard]] UnitData& get_unit_data(entt::id_type name_id) { return unit_map_[name_id]; }
    [[nodiscard]] int get_level_number() const { return level_number_; }
    [[nodiscard]] int get_point() const { return point_; }
    [[nodiscard]] bool is_level_clear() const { return level_clear_; }

private:
    int level_number_{1};       ///< @brief 当前关卡
    int point_{0};              ///< @brief 积分
    bool level_clear_{false};   ///< @brief 是否通关

    /// @brief 储存玩家拥有的角色 (角色名id ：角色数据)
    std::unordered_map<entt::id_type, UnitData> unit_map_;

    /// @brief 储存角色数据的指针，与unit_map_同步更新，用于排序
    std::vector<UnitData*> unit_data_list_;
};
}