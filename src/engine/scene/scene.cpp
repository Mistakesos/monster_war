#include "engine/scene/scene.hpp"
#include "engine/render/camera.hpp"
#include "engine/core/context.hpp"
#include "engine/core/game_state.hpp"
#include "engine/scene/scene_manager.hpp"
#include "entt/signal/dispatcher.hpp"   // IWYU pragma: keep
#include <spdlog/spdlog.h>

namespace engine::scene {
Scene::Scene(std::string_view name, engine::core::Context& context)
    : scene_name_{name}
    , context_{context}
    , ui_manager_{std::make_unique<ui::UIManager>(context_.get_game_state().get_logical_size())} {
    registry_.ctx().emplace<engine::resource::ResourceManager*>(&context_.get_resource_manager());
    spdlog::trace("场景 ‘{}’ 初始化完成", scene_name_);
}

Scene::~Scene() = default;

void Scene::update(sf::Time delta) {
    // 更新UI管理器
    ui_manager_->update(delta, context_);
}

void Scene::render() {
    // 渲染UI管理器
    ui_manager_->render(context_);
}

void Scene::request_pop_scene() {
    context_.get_dispatcher().trigger<engine::utils::PopSceneEvent>();
}

void Scene::request_push_scene(std::unique_ptr<engine::scene::Scene>&& scene) {
    context_.get_dispatcher().trigger<engine::utils::PushSceneEvent>(engine::utils::PushSceneEvent{std::move(scene)});
}

void Scene::request_replace_scene(std::unique_ptr<engine::scene::Scene>&& scene) {
    context_.get_dispatcher().trigger<engine::utils::ReplaceSceneEvent>(engine::utils::ReplaceSceneEvent{std::move(scene)});
}

void Scene::quit() {
    context_.get_dispatcher().trigger<engine::utils::QuitEvent>();
}

} // namespace engine::scene