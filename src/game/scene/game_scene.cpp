#include "game/scene/game_scene.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/resource/resource_manager.hpp"
#include "engine/audio/audio_player.hpp"
#include "engine/ui/ui_manager.hpp"
#include "engine/ui/ui_image.hpp"
#include "engine/ui/ui_label.hpp"
#include "engine/component/transform_component.hpp"
#include "engine/component/sprite_component.hpp"
#include "engine/component/velocity_component.hpp"
#include "engine/component/animation_component.hpp"
#include "engine/system/render_system.hpp"
#include "engine/system/movement_system.hpp"
#include "engine/system/animation_system.hpp"
#include "engine/loader/level_loader.hpp"
#include "engine/core/context.hpp"
#include "engine/utils/events.hpp"
#include "entt/signal/sigh.hpp"
#include "entt/signal/dispatcher.hpp"
#include "entt/core/hashed_string.hpp"
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::scene {
GameScene::GameScene(engine::core::Context& context)
    : Scene{"GameScene", context}
    , render_system_{std::make_unique<engine::system::RenderSystem>()}
    , movement_system_{std::make_unique<engine::system::MovementSystem>()}
    , animation_system_{std::make_unique<engine::system::AnimationSystem>()} {

    if (!load_level()) {
        spdlog::error("加载关卡失败！");
    }
    
    spdlog::info("GameScene 构造完成");
}

GameScene::~GameScene() {
}

bool GameScene::load_level() {
    engine::loader::LevelLoader level_loader{&context_};
    // 不调用setEntityBuilder，则使用默认的BasicEntityBuilder
    if (!level_loader.load_level("assets/maps/level1.tmj", this)) {
        spdlog::error("加载关卡失败");
        return false;
    }
    return true;
}

void GameScene::update(sf::Time delta) {
    movement_system_->update(registry_, delta);
    animation_system_->update(registry_, delta);

    Scene::update(delta);
}

void GameScene::render() {
    render_system_->update(registry_, context_.get_renderer(), context_.get_camera());

    Scene::render();
}
} // namespace game::scene
