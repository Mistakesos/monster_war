#include "game/scene/game_scene.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/resource/resource_manager.hpp"
#include "engine/audio/audio_player.hpp"
#include "engine/ui/ui_image.hpp"
#include "engine/ui/ui_label.hpp"
#include "engine/core/context.hpp"
#include "engine/utils/events.hpp"
#include "entt/signal/sigh.hpp"
#include "entt/signal/dispatcher.hpp"
#include "entt/core/hashed_string.hpp"
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::scene {
GameScene::GameScene(engine::core::Context& context)
    : Scene{"GameScene", context} {
    test_resource_manager();
}

GameScene::~GameScene() {
}

void GameScene::test_resource_manager() {
    // 载入资源
    context_.get_resource_manager().load_texture("assets/textures/Buildings/Castle.png"_hs);
    // 播放音乐
    context_.get_audio_player().play_music("battle_bgm"_hs);
    
    // 测试UI元素（使用载入的资源）
    ui_manager_->add_element(std::make_unique<engine::ui::UIImage>(context_, "assets/textures/Buildings/Castle.png"_hs));
    ui_manager_->add_element(std::make_unique<engine::ui::UILabel>(
        context_,
        "Hello, World!", 
        "assets/fonts/VonwaonBitmap-16px.ttf"
    ));
}
} // namespace game::scene
