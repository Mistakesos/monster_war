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
    test_resource_manager();
    test_ecs();

    spdlog::info("GameScene 构造完成");
}

GameScene::~GameScene() {
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

void GameScene::test_ecs() {
    auto entity = registry_.create();
    // 变换、速度、精灵组件
    registry_.emplace<engine::component::TransformComponent>(entity, sf::Vector2f(100.f, 100.f));
    registry_.emplace<engine::component::VelocityComponent>(entity, sf::Vector2f(10.f, 10.f));
    if (auto texture = context_.get_resource_manager().get_texture("assets/textures/Units/Archer.png")) {
        registry_.emplace<engine::component::SpriteComponent>(entity, *texture);
    }

    // 动画组件 (单一动画 -> 动画map -> AnimationComponent)
    auto animation = engine::component::Animation(
        {
            engine::component::AnimationFrame(sf::IntRect({0, 0}, {192, 192}), sf::seconds(0.1f)),
            engine::component::AnimationFrame(sf::IntRect({192, 0}, {192, 192}), sf::seconds(0.1f)),
            engine::component::AnimationFrame(sf::IntRect({384, 0}, {192, 192}), sf::seconds(0.1f)),
            engine::component::AnimationFrame(sf::IntRect({576, 0}, {192, 192}), sf::seconds(0.1f)),
            engine::component::AnimationFrame(sf::IntRect({768, 0}, {192, 192}), sf::seconds(0.1f)),
            engine::component::AnimationFrame(sf::IntRect({960, 0}, {192, 192}), sf::seconds(0.1f))
        }
    );
    auto animation_map = std::unordered_map<entt::id_type, engine::component::Animation>{{"idle"_hs, std::move(animation)}};
    registry_.emplace<engine::component::AnimationComponent>(entity, std::move(animation_map), "idle"_hs);
}

} // namespace game::scene
