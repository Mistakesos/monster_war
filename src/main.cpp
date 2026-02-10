#include "engine/core/game.hpp"
#include "engine/core/context.hpp"
#include "engine/utils/events.hpp"
#include "game/scene/game_scene.hpp"
#include "entt/signal/dispatcher.hpp"
#include <spdlog/spdlog.h>

int main(int, char*[]) {
    spdlog::set_level(spdlog::level::info);
    
    engine::core::Game game;
    game.register_scene_setup([](engine::core::Context& context) {
        // GameApp在调用run方法之前，先创建并设置初始场景
        auto game_scene = std::make_unique<game::scene::GameScene>(context);
        context.get_dispatcher().trigger<engine::utils::PushSceneEvent>(engine::utils::PushSceneEvent{std::move(game_scene)});
    });
    game.run();
}