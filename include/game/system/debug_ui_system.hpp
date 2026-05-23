#pragma once
#include <entt/entity/fwd.hpp>
#include <entt/entity/entity.hpp>
#include "game/defs/events.hpp"

namespace engine::core {
    class Context;
} // namespace engine::core 

namespace game::scene {
    class TitleScene;
    class LevelClearScene;
    class EndScene;
} // namespace game::scene

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

    void update();                                           ///<@brief 针对GameScene的更新
    void update_title(game::scene::TitleScene& title_scene); ///<@brief 针对TitleScene的更新 (直接传入场景引用，提升便捷但增加耦合)
    void update_level_clear(game::scene::LevelClearScene& level_clear_scene);   ///<@brief 针对LevelClearScene的更新
    void update_end(game::scene::EndScene& end_scene);                          ///<@brief 针对EndScene的更新

private:
    // 封装每个UI显示模块
    // --- GameScene ---
    void render_hovered_portrait();
    void render_hovered_unit();
    void render_selected_unit();
    void render_info_ui();
    void render_setting_ui();
    void render_debug_ui();

    // --- TitleScene ---
    void render_title_logo();
    void render_title_buttons(game::scene::TitleScene& title_scene);

    // --- LevelClearScene ---
    void render_level_clear_text();
    void render_level_clear_table(game::scene::LevelClearScene& level_clear_scene);
    void render_level_clear_buttons(game::scene::LevelClearScene& level_clear_scene);

    // --- EndScene ---
    void render_end_text(game::scene::EndScene& end_scene);
    void render_end_buttons(game::scene::EndScene& end_scene);

    // --- Shared ---
    void render_unit_info_ui(bool& show_unit_info);
    void render_save_panel_ui(bool& show_save_panel);
    void render_load_panel_ui(bool& show_load_panel);
    void render_unit_table();

    // 事件回调函数
    void on_ui_portrait_hover_enter_event(const game::defs::UIPortraitHoverEnterEvent& event);
    void on_ui_portrait_hover_leave_event();

    entt::registry& registry_;
    engine::core::Context& context_;

    entt::id_type hovered_portrait_{entt::null};    ///< @brief 悬浮肖像的角色名称ID
    bool show_debug_ui_{true};                      ///< @brief 是否显示调试UI
};

} // namespace game::system