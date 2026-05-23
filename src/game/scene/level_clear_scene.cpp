#include "game/scene/level_clear_scene.hpp"
#include "game/scene/game_scene.hpp"
#include "game/scene/title_scene.hpp"
#include "game/data/ui_config.hpp"
#include "game/data/session_data.hpp"
#include "engine/ui/ui_manager.hpp"
#include "engine/core/context.hpp"
#include "engine/core/game_state.hpp"
#include "engine/audio/audio_player.hpp"
#include "engine/utils/events.hpp"
#include "engine/loader/level_loader.hpp"
#include "engine/loader/basic_entity_builder.hpp"
#include "game/system/debug_ui_system.hpp"
#include <spdlog/spdlog.h>
#include <entt/entity/registry.hpp>

using namespace entt::literals;

namespace game::scene {

LevelClearScene::LevelClearScene(engine::core::Context& context, 
    std::shared_ptr<game::factory::BlueprintManager> blueprint_manager,
    std::shared_ptr<game::data::UIConfig> ui_config, 
    std::shared_ptr<game::data::LevelConfig> level_config,
    std::shared_ptr<game::data::SessionData> session_data,
    game::data::GameStats& game_stats)
    : engine::scene::Scene{"LevelClearScene", context}
    , blueprint_manager_{std::move(blueprint_manager)}
    , ui_config_{std::move(ui_config)}
    , level_config_{std::move(level_config)}
    , session_data_{std::move(session_data)}
    , game_stats_{game_stats} {
    // 直接在构造函数中初始化DebugUI系统
    debug_ui_system_ = std::make_unique<game::system::DebugUISystem>(registry_, context);

    if (!ui_config_ || !level_config_ || !session_data_ || !blueprint_manager_) {
        spdlog::error("LevelClearScene: ui_config_, level_config_, session_data_ or blueprint_manager_ 必须有值");
        return;
    }
    context_.get_game_state().set_state(engine::core::State::LevelClear);

    registry_.ctx().emplace<std::shared_ptr<game::data::SessionData>>(session_data_);
    registry_.ctx().emplace<std::shared_ptr<game::factory::BlueprintManager>>(blueprint_manager_);
    registry_.ctx().emplace<std::shared_ptr<game::data::UIConfig>>(ui_config_);
    context_.get_audio_player().play_music("win"_hs, 0);
}

LevelClearScene::~LevelClearScene() = default;

void LevelClearScene::render() {
    engine::scene::Scene::render();
    debug_ui_system_->update_level_clear(*this);
}

void LevelClearScene::on_next_level_click() {
    session_data_->add_one_level();
    session_data_->set_level_clear(false);
    request_replace_scene(std::make_unique<game::scene::GameScene>(
        context_, 
        blueprint_manager_,
        session_data_,
        ui_config_, 
        level_config_)
    );
}

void LevelClearScene::on_back_to_title_click() {
    request_replace_scene(std::make_unique<game::scene::TitleScene>(context_));
}

void LevelClearScene::on_save_click() {
    show_save_panel_ = !show_save_panel_;
}

}   // namespace game::scene
