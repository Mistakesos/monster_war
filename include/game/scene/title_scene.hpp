#pragma once
#include "engine/scene/scene.hpp"
#include "engine/system/fwd.hpp"
#include "game/data/ui_config.hpp"
#include "game/data/session_data.hpp"
#include "game/data/level_config.hpp"
#include "game/factory/blueprint_manager.hpp"
#include "game/system/fwd.hpp"

namespace game::scene {

class TitleScene final: public engine::scene::Scene {
    friend class game::system::DebugUISystem;   // 允许DebugUISystem访问私有成员变量及方法

public:
    TitleScene(engine::core::Context& context,
        std::shared_ptr<game::factory::BlueprintManager> blueprint_manager = nullptr,
        std::shared_ptr<game::data::SessionData> session_data = nullptr,
        std::shared_ptr<game::data::UIConfig> ui_config = nullptr,
        std::shared_ptr<game::data::LevelConfig> level_config = nullptr);
    ~TitleScene();

    void update(sf::Time delta) override;
    void render() override;

private:
    // 初始化函数(init函数中调用)
    [[nodiscard]] bool init_session_data();
    [[nodiscard]] bool init_level_config();
    [[nodiscard]] bool init_blueprint_manager();
    [[nodiscard]] bool init_ui_config();
    [[nodiscard]] bool load_title_level();
    [[nodiscard]] bool init_systems();
    [[nodiscard]] bool init_registry_context();
    [[nodiscard]] bool init_ui();

    // 按钮回调函数 (未来通过游戏UI调用)
    void on_start_game_click();
    void on_confirm_role_click();
    void on_load_game_click();
    void on_quit_click();

    // 数据相关实例
    std::shared_ptr<game::factory::BlueprintManager> blueprint_manager_;
    std::shared_ptr<game::data::SessionData> session_data_;
    std::shared_ptr<game::data::UIConfig> ui_config_;
    std::shared_ptr<game::data::LevelConfig> level_config_;

    // 系统相关实例
    std::unique_ptr<engine::system::RenderSystem> render_system_;
    std::unique_ptr<engine::system::YSortSystem> ysort_system_;
    std::unique_ptr<engine::system::AnimationSystem> animation_system_;
    std::unique_ptr<engine::system::MovementSystem> movement_system_;
    std::unique_ptr<game::system::DebugUISystem> debug_ui_system_;

    bool show_unit_info_{false};        ///< @brief 是否显示角色列表UI
    bool show_load_panel_{false};       ///< @brief 是否显示加载面板UI
};

}