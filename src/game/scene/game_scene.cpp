#include "game/scene/game_scene.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/render/camera.hpp"
#include "engine/resource/resource_manager.hpp"
#include "engine/audio/audio_player.hpp"

#include "engine/ui/ui_manager.hpp"
#include "engine/ui/ui_image.hpp"
#include "engine/ui/ui_label.hpp"

#include "engine/component/transform_component.hpp"
#include "engine/component/sprite_component.hpp"
#include "engine/component/velocity_component.hpp"
#include "engine/component/animation_component.hpp"
#include "engine/component/render_component.hpp"
#include "game/component/enemy_component.hpp"
#include "game/component/player_component.hpp"

#include "engine/system/render_system.hpp"
#include "engine/system/movement_system.hpp"
#include "engine/system/animation_system.hpp"
#include "engine/system/ysort_system.hpp"
#include "game/system/followpath_system.hpp"
#include "game/system/remove_dead_system.hpp"
#include "game/system/block_system.hpp"

#include "engine/loader/level_loader.hpp"
#include "game/loader/entity_builder_mw.hpp"
#include "game/factory/blueprint_manager.hpp"
#include "game/factory/entity_factory.hpp"
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
    , animation_system_{std::make_unique<engine::system::AnimationSystem>(registry_, context.get_dispatcher())}
    , ysort_system_{std::make_unique<engine::system::YSortSystem>()}
    , follow_path_system_{std::make_unique<game::system::FollowPathSystem>()}
    , remove_dead_system_{std::make_unique<game::system::RemoveDeadSystem>()}
    , block_system_{std::make_unique<game::system::BlockSystem>()} {

    if (!load_level()) {
        spdlog::error("加载关卡失败！");
    }
    
    if (!init_event_connections()) {
        spdlog::error("初始化事件连接失败");
        return;
    }

    if (!init_input_connections()) {
        spdlog::error("初始化输入连接失败");
        return;
    }

    if (!init_entity_factory()) {
        spdlog::error("初始化实体工厂失败");
        return;
    }

    create_test_enemy();

    spdlog::info("GameScene 构造完成");
}

GameScene::~GameScene() {
    auto& dispatcher = context_.get_dispatcher();
    auto& input_manager = context_.get_input_manager();
    // 断开所有事件连接
    dispatcher.disconnect(this);
    // 断开输入信号连接
    input_manager.on_action(Action::MouseRight).disconnect<&GameScene::on_create_test_player_melee>(this);
    input_manager.on_action(Action::MouseLeft).disconnect<&GameScene::on_create_test_player_ranged>(this);
    input_manager.on_action(Action::Pause).disconnect<&GameScene::on_clear_all_players>(this);
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

    // 不调用setEntityBuilder，则使用默认的BasicEntityBuilder
    if (!level_loader.load_level("assets/maps/level1.tmj", this)) {
        spdlog::error("加载关卡失败");
        return false;
    }
    return true;
}

bool GameScene::init_event_connections() {
    auto& dispatcher = context_.get_dispatcher();
    dispatcher.sink<game::defs::EnemyArriveHomeEvent>().connect<&GameScene::on_enemy_arrive_home>(this);
    return true;
}

bool GameScene::init_input_connections() {
    auto& input_manager = context_.get_input_manager();
    input_manager.on_action(Action::MouseRight).connect<&GameScene::on_create_test_player_melee>(this);
    input_manager.on_action(Action::MouseLeft).connect<&GameScene::on_create_test_player_ranged>(this);
    input_manager.on_action(Action::Pause).connect<&GameScene::on_clear_all_players>(this);
    return true;
}

bool GameScene::init_entity_factory() {
    // 如果蓝图管理器为空，则创建一个（将来可能由构造函数传入）
    if (!blueprint_manager_) {  
        blueprint_manager_ = std::make_shared<game::factory::BlueprintManager>(context_.get_resource_manager());
        if (!blueprint_manager_->load_enemy_class_blueprints("assets/data/enemy_data.json") ||
            !blueprint_manager_->load_player_class_blueprints("assets/data/player_data.json")) {
            spdlog::error("加载蓝图失败");
            return false;
        }
    }
    entity_factory_ = std::make_unique<game::factory::EntityFactory>(registry_, *blueprint_manager_);
    spdlog::info("entity_factory_ 加载完成");
    return true;
}

void GameScene::on_enemy_arrive_home(const game::defs::EnemyArriveHomeEvent&) {
    spdlog::info("敌人到达基地");
    // TODO: 添加敌人到达基地的逻辑
}

void GameScene::create_test_enemy() {
    // 每个起点创建一批敌人
    for (auto start_index : start_points_) {
        auto position = waypoint_nodes_[start_index].position_;

        entity_factory_->create_enemy_unit("wolf"_hs, position, start_index);
        entity_factory_->create_enemy_unit("slime"_hs, position, start_index);
        entity_factory_->create_enemy_unit("goblin"_hs, position, start_index);
        entity_factory_->create_enemy_unit("dark_witch"_hs, position, start_index);
    }
}

bool GameScene::on_create_test_player_melee() {
    // 获取鼠标在窗口中的像素位置
    auto mouse_pos = context_.get_input_manager().get_mouse_position_window();
    // 通过 camera 将屏幕坐标转换为世界坐标（自动处理 viewport/letterbox 偏移）
    auto world_pos = context_.get_camera().screen_to_world(static_cast<sf::Vector2f>(mouse_pos));
    entity_factory_->create_player_unit("warrior"_hs, world_pos);
    spdlog::info("创建战士: 位置: {}, {}", world_pos.x, world_pos.y);
    return true;
}

bool GameScene::on_create_test_player_ranged() {
    // 获取鼠标在窗口中的像素位置
    auto mouse_pos = context_.get_input_manager().get_mouse_position_window();
    // 通过 camera 将屏幕坐标转换为世界坐标（自动处理 viewport/letterbox 偏移）
    auto world_pos = context_.get_camera().screen_to_world(static_cast<sf::Vector2f>(mouse_pos));
    entity_factory_->create_player_unit("archer"_hs, world_pos);
    spdlog::info("创建弓箭手: 位置: {}, {}", world_pos.x, world_pos.y);
    return true;
}

bool GameScene::on_clear_all_players() {
    auto view = registry_.view<game::component::PlayerComponent>();
    for (auto entity : view) {
        registry_.destroy(entity);
    }
    return true;
}

void GameScene::update(sf::Time delta) {
    auto& dispatcher = context_.get_dispatcher();

    // 每一帧最先清理死亡实体(要在dispatcher处理完事件后再清理，因此放在下一帧开头)
    remove_dead_system_->update(registry_);
    
    // 注意系统更新的顺序
    follow_path_system_->update(registry_, dispatcher, waypoint_nodes_, delta);
    block_system_->update(registry_, dispatcher);
    movement_system_->update(registry_, delta);
    animation_system_->update(delta);
    ysort_system_->update(registry_);       // 确保在 MovementSystem 之后

    Scene::update(delta);
}

void GameScene::render() {
    render_system_->update(registry_, context_.get_renderer(), context_.get_camera());

    Scene::render();
}
} // namespace game::scene
