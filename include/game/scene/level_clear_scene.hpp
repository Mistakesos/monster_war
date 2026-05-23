#pragma once
#include "engine/scene/scene.hpp"
#include "game/data/ui_config.hpp"
#include "game/data/session_data.hpp"
#include "game/data/game_stats.hpp"
#include "game/data/level_config.hpp"
#include "game/factory/blueprint_manager.hpp"
#include "game/system/fwd.hpp"

namespace game::scene {

class LevelClearScene : public engine::scene::Scene {
    friend class game::system::DebugUISystem;

public:
    LevelClearScene(engine::core::Context& context, 
        std::shared_ptr<game::factory::BlueprintManager> blueprint_manager,
        std::shared_ptr<game::data::UIConfig> ui_config, 
        std::shared_ptr<game::data::LevelConfig> level_config,
        std::shared_ptr<game::data::SessionData> session_data,
        game::data::GameStats& game_stats);
    ~LevelClearScene();

    void render() override;

private:
    // 按钮回调函数
    void on_next_level_click();
    void on_back_to_title_click();
    void on_save_click();

    // 场景中共享的数据实例
    std::shared_ptr<game::factory::BlueprintManager> blueprint_manager_;
    std::shared_ptr<game::data::UIConfig> ui_config_;
    std::shared_ptr<game::data::LevelConfig> level_config_;
    std::shared_ptr<game::data::SessionData> session_data_;

    game::data::GameStats& game_stats_;     ///< @brief 构造函数传入关卡内游戏统计数据，需要在此场景中显示

    // 目前只需要DebugUI系统
    std::unique_ptr<game::system::DebugUISystem> debug_ui_system_;

    bool show_save_panel_{false};           ///< @brief 是否显示保存面板
};

}   // namespace game::scene