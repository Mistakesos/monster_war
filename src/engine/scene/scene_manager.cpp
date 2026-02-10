#include "engine/scene/scene_manager.hpp"
#include "engine/core/context.hpp"
#include "engine/scene/scene.hpp"
#include "entt/signal/dispatcher.hpp"
#include <spdlog/spdlog.h>

namespace engine::scene {
SceneManager::SceneManager(engine::core::Context& context)
    : context_{context} {
    context_.get_dispatcher().sink<engine::utils::PopSceneEvent>().connect<&SceneManager::on_pop_scene>(this);
    context_.get_dispatcher().sink<engine::utils::PushSceneEvent>().connect<&SceneManager::on_push_scene>(this);
    context_.get_dispatcher().sink<engine::utils::ReplaceSceneEvent>().connect<&SceneManager::on_replace_scene>(this);
    spdlog::trace("场景管理器已创建");
}

SceneManager::~SceneManager() {
    context_.get_dispatcher().disconnect(this);
    spdlog::trace("场景管理器已销毁");
}

Scene* SceneManager::get_current_scene() const {
    if (scene_stack_.empty()) {
        return nullptr;
    }
    return scene_stack_.back().get(); // 返回栈顶场景的裸指针
}

engine::core::Context& SceneManager::get_context() const {
    return this->context_;
}

void SceneManager::update(sf::Time delta_time) {
    // 只更新栈顶（当前）场景
    Scene* current_scene = get_current_scene();
    if (current_scene) {
        current_scene->update(delta_time);
    }
    // 执行可能的切换场景操作
    process_pending_actions();
}

void SceneManager::render() {
    // 渲染时需要叠加渲染所有场景，而不只是栈顶
    for (const auto& scene : scene_stack_) {
        if (scene) {
            scene->render();
        }
    }
}

void SceneManager::handle_input() {
    // 只考虑栈顶场景
    Scene* current_scene = get_current_scene();
    if (current_scene) {
        current_scene->handle_input();
    }
}

void SceneManager::on_pop_scene() {
    pending_action_ = PendingAction::Pop;
}

void SceneManager::on_push_scene(engine::utils::PushSceneEvent& event) {
    pending_action_ = PendingAction::Push;
    pending_scene_ = std::move(event.scene);
}

void SceneManager::on_replace_scene(engine::utils::ReplaceSceneEvent& event) {
    pending_action_ = PendingAction::Replace;
    pending_scene_ = std::move(event.scene);
}

void SceneManager::process_pending_actions() {
    if (pending_action_ == PendingAction::None) {
        return;
    }

    switch (pending_action_) {
        case PendingAction::Pop:
            pop_scene();
            break;
        case PendingAction::Replace:
            replace_scene(std::move(pending_scene_));
            break;
        case PendingAction::Push:
            push_scene(std::move(pending_scene_));
            break;
        default:
            break;
    }

    pending_action_ = PendingAction::None;
}

void SceneManager::push_scene(std::unique_ptr<Scene>&& scene) {
    if (!scene) {
        spdlog::warn("尝试将空场景压入栈。");
        return;
    }
    spdlog::debug("正在将场景 '{}' 压入栈。", scene->get_name());

    // 将新场景移入栈顶
    scene_stack_.push_back(std::move(scene));
}

void SceneManager::pop_scene() {
    if (scene_stack_.empty()) {
        spdlog::warn("尝试从空场景栈中弹出。");
        return;
    }
    spdlog::debug("正在从栈中弹出场景 '{}' 。", scene_stack_.back()->get_name());

    // 清理并移除栈顶场景
    scene_stack_.pop_back();

    // 如果弹出最后一个场景，退出游戏
    if (scene_stack_.empty()) {
        spdlog::warn("弹出最后一个场景，退出游戏");
        context_.get_dispatcher().enqueue<engine::utils::QuitEvent>();
    }
}

void SceneManager::replace_scene(std::unique_ptr<Scene>&& scene) {
    if (!scene) {
        spdlog::warn("尝试用空场景替换。");
        return;
    }
    spdlog::debug("正在用场景 '{}' 替换场景 '{}' 。", scene->get_name(), scene_stack_.back()->get_name());

    // 清理并移除场景栈中所有场景
    while (!scene_stack_.empty()) {
        scene_stack_.pop_back();
    }

    // 将新场景压入栈顶
    scene_stack_.push_back(std::move(scene));
}
} // namespace engine::scene