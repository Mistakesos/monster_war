#include "engine/core/game.hpp"
#include "engine/core/context.hpp"
#include "engine/utils/events.hpp"
#include "game/scene/title_scene.hpp"
#include "entt/signal/dispatcher.hpp"   // IWYU pragma: keep
#include <spdlog/spdlog.h>

int main(int, char*[]) {
    spdlog::set_level(spdlog::level::info);
    
    engine::core::Game game;
    game.register_scene_setup([](engine::core::Context& context) {
        // GameApp在调用run方法之前，先创建并设置初始场景
        auto title_scene = std::make_unique<game::scene::TitleScene>(context);
        context.get_dispatcher().trigger<engine::utils::PushSceneEvent>(engine::utils::PushSceneEvent{std::move(title_scene)});
    });
    game.run();
}