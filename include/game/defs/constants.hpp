#pragma once

namespace game::defs {

constexpr float BLOCK_RADIUS = 40.f;   ///< @brief 阻挡半径
constexpr float UNIT_RADIUS = 20.f;    ///< @brief 角色自身半径（相当于碰撞盒，用于计算攻击范围）

/// @brief 玩家类型枚举
enum class PlayerType {
    Unknown,
    Melee,      ///< @brief 近战型，只能放在近战区域
    Ranged,     ///< @brief 远程型，只能放在远程区域
    Mixed       ///< @brief 混合型，可以放在任意区域（暂不实现，未来可拓展）
};

}   // namespace game::defs