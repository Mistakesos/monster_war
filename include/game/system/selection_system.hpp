#pragma once
#include <entt/entity/fwd.hpp>

namespace engine::core {
    class Context;
} // namespace engine::core

namespace game::system {

/**
 * @brief 选择单位系统
 * @note 处理玩家角色的“选中单位”操作以及“鼠标悬浮单位”的操作。
 */
class SelectionSystem {
public:
    SelectionSystem(entt::registry& registry, engine::core::Context& context);
    ~SelectionSystem();

    void update();  ///< @brief 判断是否有鼠标悬浮单位

private:
    void clear_current_selection();   ///< @brief 清除当前选中单位

    // 输入控制回调函数
    bool on_mouse_left_click();    ///< @brief 鼠标左键点击时，如果有悬浮单位，则选中该单位
    bool on_mouse_right_click();   ///< @brief 鼠标右键点击时，清除当前选中单位

    entt::registry& registry_;
    engine::core::Context& context_;
};

} // namespace game::system