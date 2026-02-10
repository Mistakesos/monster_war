#include "engine/scene/scene.hpp"
#include "engine/render/camera.hpp"
#include "engine/core/context.hpp"
#include "engine/object/game_object.hpp"
#include "engine/core/game_state.hpp"
#include "engine/scene/scene_manager.hpp"
#include "engine/ui/ui_manager.hpp"
#include "entt/signal/dispatcher.hpp"
#include <spdlog/spdlog.h>

namespace engine::scene {
Scene::Scene(std::string_view name, engine::core::Context& context)
    : scene_name_{name}
    , context_{context}
    , ui_manager_{std::make_unique<ui::UIManager>(context_.get_game_state().get_logical_size())} {
    spdlog::trace("场景 ‘{}’ 初始化完成", scene_name_);
}

Scene::~Scene() = default;

void Scene::update(sf::Time delta) {
    // 更新所有游戏对象，先略过需要移除的对象
    for (auto& obj : game_objects_) {
        if (obj && !obj->is_need_remove()) {
            obj->update(delta, context_);
        } else {
            if (!obj) spdlog::warn("尝试更新一个空的游戏对象指针。");
            std::erase_if(game_objects_, [](const std::unique_ptr<engine::object::GameObject>& obj) {
                return !obj || obj->is_need_remove();
            });
        }
    }

    // 只有游戏进行中，才需要更新物理引擎和相机
    if (context_.get_game_state().is_playing()){
        context_.get_camera().update(delta);
    }

    // 更新UI管理器
    ui_manager_->update(delta, context_);
    
    process_pending_additions();      // 处理待添加（延时添加）的游戏对象
}

void Scene::render() {
    // 渲染所有游戏对象
    for (const auto& obj : game_objects_) {
        if (obj) obj->render(context_);
    }

    // 渲染UI管理器
    ui_manager_->render(context_);
}

void Scene::handle_input() {
    // 处理UI管理器输入
    if (ui_manager_->handle_input(context_)) return;   // 如果输入事件被UI处理则返回，不再处理游戏对象输入

    // 遍历所有游戏对象，略过需要移除的对象
    for (auto& obj : game_objects_) {
        if (obj && !obj->is_need_remove()) {
            obj->handle_input(context_);
        }
    }
}

void Scene::add_game_object(std::unique_ptr<engine::object::GameObject>&& game_object) {
    if (game_object) game_objects_.push_back(std::move(game_object));
    else spdlog::warn("尝试向场景 '{}' 添加空游戏对象。", scene_name_);
}

void Scene::safe_add_game_object(std::unique_ptr<engine::object::GameObject>&& game_object) {
    if (game_object) pending_additions_.push_back(std::move(game_object));
    else spdlog::warn("尝试向场景 '{}' 添加空游戏对象。", scene_name_);
}

void Scene::remove_game_object(engine::object::GameObject* game_object_ptr) {
    if (!game_object_ptr) {
        spdlog::warn("尝试从场景 '{}' 中移除一个空的游戏对象指针。", scene_name_);
        return;
    }

    // erase-remove 移除法不可用，因为智能指针与裸指针无法比较
    // 需要使用 std::remove_if 和 lambda 表达式自定义比较的方式
    auto it = std::remove_if(game_objects_.begin(), game_objects_.end(),
                             [game_object_ptr](const std::unique_ptr<engine::object::GameObject>& p) {
                                 return p.get() == game_object_ptr;    // 比较裸指针是否相等（自定义比较方式）
                             });

    if (it != game_objects_.end()) {
        game_objects_.erase(it, game_objects_.end());   // 删除从it到末尾的元素（最后一个元素）
        spdlog::trace("从场景 '{}' 中移除游戏对象。", scene_name_);
    } else {
        spdlog::warn("游戏对象指针未找到在场景 '{}' 中。", scene_name_);
    }
}

void Scene::safe_remove_game_object(engine::object::GameObject* game_object_ptr) {
    game_object_ptr->set_need_remove(true);
}

const std::vector<std::unique_ptr<engine::object::GameObject>>& Scene::get_game_objects() const {
    return game_objects_;
}

engine::object::GameObject* Scene::find_game_object_by_name(std::string_view name) const {
        // 找到第一个符合条件的游戏对象就返回
    for (const auto& obj : game_objects_) {
        if (obj && obj->get_name() == name) {
            return obj.get();
        }
    }
    return nullptr;
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

void Scene::process_pending_additions() {
    // 处理待添加的游戏对象
    for (auto& game_object : pending_additions_) {
        add_game_object(std::move(game_object));
    }
    pending_additions_.clear();
}
} // namespace engine::scene