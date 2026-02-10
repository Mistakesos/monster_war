#include "game/scene/game_scene.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/core/context.hpp"
#include "engine/utils/events.hpp"
#include "entt/signal/sigh.hpp"
#include "entt/signal/dispatcher.hpp"
#include <spdlog/spdlog.h>

namespace game::scene {
GameScene::GameScene(engine::core::Context& context)
    : Scene{"GameScene", context} {
    static int count = 0;
    scene_index_ = count++;
    spdlog::info("场景编号：{}", scene_index_);
    
    // 注册输入回调事件 (J,K 键)
    auto& input_manager = context_.get_input_manager();
    input_manager.on_action(Action::Jump).connect<&GameScene::on_replace>(this);     // J 键
    input_manager.on_action(Action::MouseLeft).connect<&GameScene::on_push>(this);   // 鼠标左键
    input_manager.on_action(Action::MouseRight).connect<&GameScene::on_pop>(this);   // 鼠标右键
    input_manager.on_action(Action::Pause).connect<&GameScene::on_quit>(this);        // P 键
}

GameScene::~GameScene() {
    // 断开输入回调事件 (谁连接，谁负责断开)
    auto& input_manager = context_.get_input_manager();
    input_manager.on_action(Action::Jump).disconnect<&GameScene::on_replace>(this);    // J 键
    input_manager.on_action(Action::MouseLeft).disconnect<&GameScene::on_push>(this);  // 鼠标左键
    input_manager.on_action(Action::MouseRight).disconnect<&GameScene::on_pop>(this);  // 鼠标右键
    input_manager.on_action(Action::Pause).disconnect<&GameScene::on_quit>(this);       // P 键
}

void GameScene::on_replace() {
    spdlog::info("on_replace, 切换场景");
    request_replace_scene(std::make_unique<game::scene::GameScene>(context_));
}

void GameScene::on_push() {
    spdlog::info("on_push, 压入场景");
    request_push_scene(std::make_unique<game::scene::GameScene>(context_));
}

void GameScene::on_pop() {
    spdlog::info("on_pop, 弹出编号为 {} 的场景", scene_index_);
    request_pop_scene();
}

void GameScene::on_quit() {
    spdlog::info("on_quit, 退出游戏");
    quit();
}
} // namespace game::scene