#include "engine/core/game.hpp"
#include "engine/scene/scene_manager.hpp"
#include "game/scene/game_scene.hpp"
#include <spdlog/spdlog.h>

int main(int, char*[]) {
    spdlog::set_level(spdlog::level::off);
    
    engine::core::Game game;
    game.register_scene_setup([](engine::scene::SceneManager& scene_manager) {
        // GameApp在调用run方法之前，先创建并设置初始场景
        auto game_scene = std::make_unique<game::scene::GameScene>(scene_manager.get_context(), scene_manager);
        scene_manager.request_push_scene(std::move(game_scene));
    });
    game.run();
}