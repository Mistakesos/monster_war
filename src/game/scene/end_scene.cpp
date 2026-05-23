#include "game/scene/end_scene.hpp"
#include "game/scene/title_scene.hpp"
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

EndScene::EndScene(engine::core::Context& context, bool is_win)
    : engine::scene::Scene{"EndScene", context}
    , is_win_{is_win} {
    debug_ui_system_ = std::make_unique<game::system::DebugUISystem>(registry_, context);

    if (is_win_) {
        context_.get_audio_player().play_music("win"_hs, 0);
    } else {
        context_.get_audio_player().play_music("lose"_hs, 0);
    }
    context_.get_game_state().set_state(engine::core::State::GameOver);
}

EndScene::~EndScene() = default;

void EndScene::render() {
    engine::scene::Scene::render();
    debug_ui_system_->update_end(*this);
}

void EndScene::on_back_to_title_click() {
    request_replace_scene(std::make_unique<game::scene::TitleScene>(context_));
}

void EndScene::on_quit_click() {
    quit();
}

}   // namespace game::scene
