#include "game/scene/game_scene.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/render/camera.hpp"

#include "game/component/player_component.hpp"

#include "engine/system/render_system.hpp"
#include "engine/system/movement_system.hpp"
#include "engine/system/animation_system.hpp"
#include "engine/system/ysort_system.hpp"
#include "engine/system/audio_system.hpp"
#include "game/system/followpath_system.hpp"
#include "game/system/remove_dead_system.hpp"
#include "game/system/block_system.hpp"
#include "game/system/timer_system.hpp"
#include "game/system/set_target_system.hpp"
#include "game/system/orientation_system.hpp"
#include "game/system/attack_starter_system.hpp"
#include "game/system/animation_state_system.hpp"
#include "game/system/animation_event_system.hpp"
#include "game/system/combat_resolve_system.hpp"
#include "game/system/projectile_system.hpp"
#include "game/system/effect_system.hpp"
#include "game/system/health_bar_system.hpp"
#include "game/system/game_rule_system.hpp"
#include "game/system/place_unit_system.hpp"
#include "game/system/render_range_system.hpp"
#include "game/system/debug_ui_system.hpp"

#include "engine/loader/level_loader.hpp"
#include "game/loader/entity_builder_mw.hpp"
#include "game/spawner/enemy_spawner.hpp"
#include "game/factory/blueprint_manager.hpp"
#include "game/factory/entity_factory.hpp"
#include "engine/core/context.hpp"
#include "engine/utils/events.hpp"
#include "engine/utils/math.hpp"

#include "engine/ui/ui_manager.hpp"
#include "game/ui/units_portrait_ui.hpp"

#include <entt/signal/sigh.hpp>
#include <entt/signal/dispatcher.hpp>
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::scene {
GameScene::GameScene(engine::core::Context& context)
    : Scene{"GameScene", context} {
    if (!init_session_data())           { spdlog::error("初始化session_data_失败"); return; }
    if (!init_level_config())           { spdlog::error("初始化关卡配置失败"); return; }
    if (!init_ui_config())              { spdlog::error("初始化UI配置失败"); return; }
    if (!load_level())                  { spdlog::error("加载关卡失败！"); }
    if (!init_event_connections())      { spdlog::error("初始化事件连接失败"); return; }
    if (!init_input_connections())      { spdlog::error("初始化输入连接失败"); return; }
    if (!init_entity_factory())         { spdlog::error("初始化实体工厂失败"); return; }
    if (!init_registry_context())       { spdlog::error("初始化注册表上下文失败"); return; }
    if (!init_systems())                { spdlog::error("初始化系统失败"); return; }
    if (!init_enemy_spawner())          { spdlog::error("初始化敌人生成器失败"); return; }
    if (!init_units_portrait_ui())      { spdlog::error("初始化单位肖像UI失败"); return; }

    spdlog::info("GameScene 构造完成");
}

GameScene::~GameScene() {
    auto& dispatcher = context_.get_dispatcher();
    auto& input_manager = context_.get_input_manager();
    // 断开所有事件连接
    dispatcher.disconnect(this);
    // 断开输入信号连接
    input_manager.on_action(Action::Pause).disconnect<&GameScene::on_clear_all_players>(this);
}

bool GameScene::init_session_data() {
    if (!session_data_) {
        session_data_ = std::make_shared<game::data::SessionData>();
        if (!session_data_->load_default_data()) {
            spdlog::error("初始化session_data_失败");
            return false;
        }
    }
    level_number_ = session_data_->get_level_number();
    return true;
}

bool GameScene::init_level_config() {
    if (!level_config_) {
        level_config_ = std::make_shared<game::data::LevelConfig>();
        if (!level_config_->load_from_file("assets/data/level_config.json")) {
            spdlog::error("加载关卡配置失败");
            return false;
        }
    }
    waves_ = level_config_->get_waves_data(level_number_);
    game_stats_.enemy_count_ = level_config_->get_total_enemy_count(level_number_);
    return true;
}

bool GameScene::init_ui_config() {
    if (!ui_config_) {
        ui_config_ = std::make_shared<game::data::UIConfig>(&context_.get_resource_manager());
        if (!ui_config_->load_from_file("assets/data/ui_config.json")) {
            spdlog::error("加载UI配置失败");
            return false;
        }
    }
    return true;
}

bool GameScene::load_level() {
    engine::loader::LevelLoader level_loader{&context_};
    // 设置拓展的构建器EntityBuilderMW
    level_loader.set_entity_builder(std::make_unique<game::loader::EntityBuilderMW>(
        level_loader, 
        context_, 
        registry_, 
        waypoint_nodes_, 
        start_points_)
    );

    // 获取关卡地图路径
    auto map_path = level_config_->get_map_path(level_number_);
    if (!level_loader.load_level(map_path, this)) {
        spdlog::error("加载关卡失败");
        return false;
    }
    return true;
}

bool GameScene::init_event_connections() {
    // auto& dispatcher = context_.get_dispatcher();
    return true;
}

bool GameScene::init_input_connections() {
    auto& input_manager = context_.get_input_manager();
    input_manager.on_action(Action::Pause).connect<&GameScene::on_clear_all_players>(this);
    return true;
}

bool GameScene::init_entity_factory() {
    // 如果蓝图管理器为空，则创建一个（将来可能由构造函数传入）
    if (!blueprint_manager_) {  
        blueprint_manager_ = std::make_shared<game::factory::BlueprintManager>(context_.get_resource_manager());
        if (!blueprint_manager_->load_enemy_class_blueprints("assets/data/enemy_data.json") ||
            !blueprint_manager_->load_player_class_blueprints("assets/data/player_data.json") ||
            !blueprint_manager_->load_projectile_blueprints("assets/data/projectile_data.json")) {
            spdlog::error("加载蓝图失败");
            return false;
        }
    }
    entity_factory_ = std::make_unique<game::factory::EntityFactory>(registry_, *blueprint_manager_);
    spdlog::info("entity_factory_ 加载完成");
    return true;
}

bool GameScene::init_registry_context() {
    // 让注册表存储一些数据类型实例作为上下文，方便使用
    registry_.ctx().emplace<std::shared_ptr<game::factory::BlueprintManager>>(blueprint_manager_);
    registry_.ctx().emplace<std::shared_ptr<game::data::SessionData>>(session_data_);
    registry_.ctx().emplace<std::shared_ptr<game::data::UIConfig>>(ui_config_);
    registry_.ctx().emplace<std::shared_ptr<game::data::LevelConfig>>(level_config_);
    registry_.ctx().emplace<std::unordered_map<int, game::data::WaypointNode>&>(waypoint_nodes_);
    registry_.ctx().emplace<std::vector<int>&>(start_points_);
    registry_.ctx().emplace<game::data::GameStats&>(game_stats_);
    registry_.ctx().emplace<game::data::Waves&>(waves_);
    registry_.ctx().emplace<int&>(level_number_);

    spdlog::info("registry_ 上下文初始化完成");
    return true;
}

bool GameScene::init_units_portrait_ui() {
    try {
        units_portrait_ui_ = std::make_unique<game::ui::UnitsPortraitUI>(registry_, *ui_manager_, context_);
    } catch (const std::exception& e) {
        spdlog::error("初始化单位肖像UI失败: {}", e.what());
        return false;
    }
    return true;
}

bool GameScene::init_systems() {
    auto& dispatcher = context_.get_dispatcher();
    // 系统初始化需要在可能的依赖模块(如实体工厂)初始化之后
    render_system_ = std::make_unique<engine::system::RenderSystem>();
    movement_system_ = std::make_unique<engine::system::MovementSystem>();
    animation_system_ = std::make_unique<engine::system::AnimationSystem>(registry_, dispatcher);
    ysort_system_ = std::make_unique<engine::system::YSortSystem>();
    audio_system_ = std::make_unique<engine::system::AudioSystem>(registry_, context_);
    
    follow_path_system_ = std::make_unique<game::system::FollowPathSystem>();
    remove_dead_system_ = std::make_unique<game::system::RemoveDeadSystem>();
    block_system_ = std::make_unique<game::system::BlockSystem>();
    set_target_system_ = std::make_unique<game::system::SetTargetSystem>();
    attack_starter_system_ = std::make_unique<game::system::AttackStarterSystem>();
    timer_system_ = std::make_unique<game::system::TimerSystem>();
    orientation_system_ = std::make_unique<game::system::OrientationSystem>();
    animation_state_system_ = std::make_unique<game::system::AnimationStateSystem>(registry_, dispatcher);
    animation_event_system_ = std::make_unique<game::system::AnimationEventSystem>(registry_, dispatcher);
    combat_resolve_system_ = std::make_unique<game::system::CombatResolveSystem>(registry_, dispatcher);
    projectile_system_ = std::make_unique<game::system::ProjectileSystem>(registry_, dispatcher, *entity_factory_);
    effect_system_ = std::make_unique<game::system::EffectSystem>(registry_, dispatcher, *entity_factory_);
    health_bar_system_ = std::make_unique<game::system::HealthBarSystem>();
    game_rule_system_ = std::make_unique<game::system::GameRuleSystem>(registry_, dispatcher);
    place_unit_system_ = std::make_unique<game::system::PlaceUnitSystem>(registry_, *entity_factory_, context_);
    render_range_system_ = std::make_unique<game::system::RenderRangeSystem>();
    debug_ui_system_ = std::make_unique<game::system::DebugUISystem>(registry_, context_);
    spdlog::info("systems 初始化完成");
    return true;
}

bool GameScene::init_enemy_spawner() {
    enemy_spawner_ = std::make_unique<game::spawner::EnemySpawner>(registry_, *entity_factory_);
    spdlog::info("敌人生成器初始化完成");
    return true;
}

bool GameScene::on_clear_all_players() {
    auto view = registry_.view<game::component::PlayerComponent>();
    for (auto entity : view) {
        context_.get_dispatcher().enqueue(game::defs::RemovePlayerUnitEvent{entity});
    }
    return true;
}

void GameScene::update(sf::Time delta) {
    auto& dispatcher = context_.get_dispatcher();

    // 每一帧最先清理死亡实体(要在dispatcher处理完事件后再清理，因此放在下一帧开头)
    remove_dead_system_->update(registry_);
    
    // 注意系统更新的顺序
    timer_system_->update(registry_, delta);
    game_rule_system_->update(delta);
    block_system_->update(registry_, dispatcher);
    set_target_system_->update(registry_);
    follow_path_system_->update(registry_, dispatcher, waypoint_nodes_, delta);
    orientation_system_->update(registry_);     // 调用顺序要在Block、SetTarget、FollowPath之后
    attack_starter_system_->update(registry_, dispatcher);
    projectile_system_->update(delta);
    movement_system_->update(registry_, delta);
    animation_system_->update(delta);
    place_unit_system_->update(delta);
    ysort_system_->update(registry_);   // 调用顺序要在MovementSystem之后

    // 场景中其他更新函数
    enemy_spawner_->update(delta);
    units_portrait_ui_->update(delta);
    Scene::update(delta);
}

void GameScene::render() {
    auto& renderer = context_.get_renderer();
    auto& camera = context_.get_camera();

    // 注意渲染顺序，保证正确的遮盖关系
    render_system_->update(registry_, context_.get_renderer(), context_.get_camera());
    health_bar_system_->update(registry_, renderer, camera);
    render_range_system_->update(registry_, renderer, camera);

    Scene::render();

    debug_ui_system_->update();     // 调试UI的显示优先级最高，最后渲染
}
} // namespace game::scene