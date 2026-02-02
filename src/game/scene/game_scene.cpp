#include "game/scene/game_scene.hpp"

namespace game::scene {
GameScene::GameScene(engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : Scene{"GameScene", context, scene_manager} {
}
} // namespace game::scene