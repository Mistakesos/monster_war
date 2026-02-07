#include "game/scene/game_scene.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/core/context.hpp"
#include "entt/signal/sigh.hpp"
#include <spdlog/spdlog.h>

namespace game::scene {
GameScene::GameScene(engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : Scene{"GameScene", context, scene_manager} {
    // 注册输入回调事件 (J,K 键)
    auto& input_manager = context_.get_input_manager();
    input_manager.on_action(Action::Attack).connect<&GameScene::on_attack>(this);
    input_manager.on_action(Action::Jump, engine::input::ActionState::Released).connect<&GameScene::on_jump>(this);
}

GameScene::~GameScene() {
    // 断开输入回调事件 (谁连接，谁负责断开)
    auto& input_manager = context_.get_input_manager();
    input_manager.on_action(Action::Attack).disconnect<&GameScene::on_attack>(this);
    input_manager.on_action(Action::Jump, engine::input::ActionState::Released).disconnect<&GameScene::on_jump>(this);
}

void GameScene::on_attack() {
    spdlog::info("On attack");
}

void GameScene::on_jump() {
    spdlog::info("On jump");
}
} // namespace game::scene