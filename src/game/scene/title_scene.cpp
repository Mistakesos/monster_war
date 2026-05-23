#include "game/scene/title_scene.hpp"
#include "game/scene/game_scene.hpp"
#include "game/data/ui_config.hpp"
#include "game/data/session_data.hpp"
#include "engine/ui/ui_manager.hpp"
#include "engine/core/context.hpp"
#include "engine/core/time.hpp"
#include "engine/core/game_state.hpp"
#include "engine/audio/audio_player.hpp"
#include "engine/utils/events.hpp"
#include "engine/system/render_system.hpp"
#include "engine/system/ysort_system.hpp"
#include "engine/system/animation_system.hpp"
#include "engine/system/movement_system.hpp"
#include "engine/loader/level_loader.hpp"
#include "engine/loader/basic_entity_builder.hpp"
#include "game/system/debug_ui_system.hpp"
#include <spdlog/spdlog.h>
#include <entt/entity/registry.hpp>

using namespace entt::literals;

namespace game::scene {

TitleScene::TitleScene(engine::core::Context& context,
    std::shared_ptr<game::factory::BlueprintManager> blueprint_manager,
    std::shared_ptr<game::data::SessionData> session_data,
    std::shared_ptr<game::data::UIConfig> ui_config, 
    std::shared_ptr<game::data::LevelConfig> level_config)
    : engine::scene::Scene("TitleScene", context), 
    blueprint_manager_(blueprint_manager),
    session_data_(session_data),
    ui_config_(ui_config), 
    level_config_(level_config) {

    if (!init_session_data())         { spdlog::error("初始化session_data_失败"); return; }
    if (!init_level_config())         { spdlog::error("初始化关卡配置失败"); return; }
    if (!init_blueprint_manager())    { spdlog::error("初始化蓝图管理器失败"); return; }
    if (!init_ui_config())            { spdlog::error("初始化UI配置失败"); return; }
    if (!load_title_level())          { spdlog::error("加载关卡失败"); return; }
    if (!init_systems())             { spdlog::error("初始化系统失败"); return; }
    if (!init_registry_context())     { spdlog::error("初始化注册表上下文失败"); return; }
    if (!init_ui())                  { spdlog::error("初始化UI失败"); return; }

    context_.get_game_state().set_state(engine::core::State::Title);
    context_.get_time().set_time_scale(1.f);      // 重置游戏速度
}

TitleScene::~TitleScene() = default;

void TitleScene::update(sf::Time delta) {
    engine::scene::Scene::update(delta);
    animation_system_->update(delta);
    movement_system_->update(registry_, delta);
    ysort_system_->update(registry_);
}

void TitleScene::render() {
    auto& renderer = context_.get_renderer();
    auto& camera = context_.get_camera();

    render_system_->update(registry_, renderer, camera);

    engine::scene::Scene::render();
    debug_ui_system_->update_title(*this);
}

bool TitleScene::init_session_data() {
    if (!session_data_) {
        session_data_ = std::make_shared<game::data::SessionData>();
        if (!session_data_->load_default_data()) {
            spdlog::error("初始化session_data_失败");
            return false;
        }
    }
    return true;
}

bool TitleScene::init_level_config() {
    if (!level_config_) {
        level_config_ = std::make_shared<game::data::LevelConfig>();
        if (!level_config_->load_from_file("assets/data/level_config.json")) {
            spdlog::error("加载关卡配置失败");
            return false;
        }
    }
    return true;
}

bool TitleScene::init_blueprint_manager() {
    if (!blueprint_manager_) {
        blueprint_manager_ = std::make_shared<game::factory::BlueprintManager>(context_.get_resource_manager());
        if (!blueprint_manager_->load_enemy_class_blueprints("assets/data/enemy_data.json") ||
            !blueprint_manager_->load_player_class_blueprints("assets/data/player_data.json") ||
            !blueprint_manager_->load_projectile_blueprints("assets/data/projectile_data.json") ||
            !blueprint_manager_->load_effect_blueprints("assets/data/effect_data.json") ||
            !blueprint_manager_->load_skill_blueprints("assets/data/skill_data.json")) {
            spdlog::error("加载蓝图失败");
            return false;
        }
    }
    return true;
}

bool TitleScene::init_ui_config() {
    if (!ui_config_) {
        ui_config_ = std::make_shared<game::data::UIConfig>(&context_.get_resource_manager());
        if (!ui_config_->load_from_file("assets/data/ui_config.json")) {
            spdlog::error("加载UI配置失败");
            return false;
        }
    }
    return true;
}


bool TitleScene::load_title_level() {
    engine::loader::LevelLoader level_loader{&context_};
    if (!level_loader.load_level("assets/maps/title.tmj", this)) {
        spdlog::error("加载标题关卡失败");
        return false;
    }
    return true;
}

bool TitleScene::init_systems() {
    // 初始化系统
    auto& dispatcher = context_.get_dispatcher();
    debug_ui_system_ = std::make_unique<game::system::DebugUISystem>(registry_, context_);
    render_system_ = std::make_unique<engine::system::RenderSystem>();
    ysort_system_ = std::make_unique<engine::system::YSortSystem>();
    animation_system_ = std::make_unique<engine::system::AnimationSystem>(registry_, dispatcher);
    movement_system_ = std::make_unique<engine::system::MovementSystem>();
    return true;
}

bool TitleScene::init_registry_context() {
    // 让注册表存储一些数据类型实例作为上下文，方便使用
    registry_.ctx().emplace<std::shared_ptr<game::data::SessionData>>(session_data_);
    registry_.ctx().emplace<std::shared_ptr<game::factory::BlueprintManager>>(blueprint_manager_);
    registry_.ctx().emplace<std::shared_ptr<game::data::UIConfig>>(ui_config_);
    return true;
}

bool TitleScene::init_ui() {
    auto window_size = context_.get_game_state().get_logical_size();
    ui_manager_ = {std::make_unique<engine::ui::UIManager>(window_size)};
    if (!ui_manager_) return false;

    // 设置背景音乐
    context_.get_audio_player().play_music("title_bgm"_hs);

    /* 先用ImGui实现UI，未来再使用游戏内UI */
    return true;
}

void TitleScene::on_start_game_click() {
    // 如果数据是读档载入的，有可能已经通关，此时需要进入下一关
    if (session_data_->is_level_clear()) {
        session_data_->set_level_clear(false);
        session_data_->add_one_level();
    }
    request_replace_scene(std::make_unique<game::scene::GameScene>(
        context_, 
        blueprint_manager_,
        session_data_,
        ui_config_, 
        level_config_
        )
    );
}

void TitleScene::on_confirm_role_click() {
    show_unit_info_ = !show_unit_info_;
    /* 用ImGui快速实现逻辑，未来再完善游戏内UI */
}

void TitleScene::on_load_game_click() {
    show_load_panel_ = !show_load_panel_;
    /* 用ImGui快速实现逻辑，未来再完善游戏内UI */
}

void TitleScene::on_quit_click() {
    quit();
}

}   // namespace game::scene