#pragma once
#include <entt/entity/fwd.hpp>
#include <entt/entity/entity.hpp>
#include "game/defs/events.hpp"

namespace engine::core {
    class Context;
} // namespace engine::core 

namespace game::system {

/**
 * @brief 调试 UI 系统，负责显示调试 UI。
 * 
 * @note 调试UI的主要目的是方便debug，并快速开发UI原型。
 * @note 游戏正式发布时往往会删除，因此不需要过度设计。
 */
class DebugUISystem {
public:
    DebugUISystem(entt::registry& registry, engine::core::Context& context);
    ~DebugUISystem();

    void update();

private:
    // 封装每个UI显示模块
    void render_hovered_portrait();
    void render_hovered_unit();
    void render_selected_unit();
    void render_info_ui();
    void render_setting_ui();
    void render_debug_ui();

    // 事件回调函数
    void on_ui_portrait_hover_enter_event(const game::defs::UIPortraitHoverEnterEvent& event);
    void on_ui_portrait_hover_leave_event();

    entt::registry& registry_;
    engine::core::Context& context_;

    entt::id_type hovered_portrait_{entt::null};    ///< @brief 悬浮肖像的角色名称ID
    bool show_debug_ui_{true};                      ///< @brief 是否显示调试UI
};

} // namespace game::system