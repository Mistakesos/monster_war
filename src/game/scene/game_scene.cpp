#include "game/scene/game_scene.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/render/camera.hpp"

#include "game/component/player_component.hpp"
#include "game/component/stats_component.hpp"

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

#include "engine/loader/level_loader.hpp"
#include "game/loader/entity_builder_mw.hpp"
#include "game/factory/blueprint_manager.hpp"
#include "game/factory/entity_factory.hpp"
#include "game/defs/tags.hpp"
#include "engine/core/context.hpp"
#include "engine/core/game_state.hpp"
#include "engine/utils/events.hpp"
#include "engine/utils/math.hpp"

#include "engine/ui/ui_manager.hpp"
#include "engine/ui/ui_panel.hpp"
#include "engine/ui/ui_image.hpp"
#include "engine/ui/ui_button.hpp"
#include "engine/ui/ui_label.hpp"

#include <entt/signal/sigh.hpp>
#include <entt/signal/dispatcher.hpp>
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::scene {
GameScene::GameScene(engine::core::Context& context)
    : Scene{"GameScene", context} {
    if (!init_session_data()) {
        spdlog::error("初始化session_data_失败");
        return;
    }

    if (!init_ui_config()) {
        spdlog::error("初始化UI配置失败");
        return;
    }

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

    if (!init_systems()) {
        spdlog::error("初始化系统失败");
        return;
    }

    test_session_data();
    create_test_enemy();
    create_units_portrait_ui();

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
    input_manager.on_action(Action::MoveLeft).connect<&GameScene::on_create_test_player_healer>(this);
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
    spdlog::info("systems 初始化完成");
    return true;
}

void GameScene::create_units_portrait_ui() {
    if (!ui_manager_) return;

    auto padding = ui_config_->get_unit_panel_padding();
    auto& unit_map = session_data_->get_unit_map();
    auto unit_num = unit_map.size();

    // --- 在屏幕下方创建一个panel UI 条，用于显示角色肖像 ---
    // 获取窗口大小和角色肖像框大小
    auto window_size = context_.get_game_state().get_logical_size();
    auto frame_size = ui_config_->get_unit_panel_frame_size();
    // 根据角色数量、角色肖像框大小、间隔计算panel的位置和大小
    auto pos = sf::Vector2f(0.f, window_size.y - frame_size.y - 2 * padding);
    auto size = sf::Vector2f(unit_num*frame_size.x + (unit_num + 1)*padding, frame_size.y + 2 * padding);
    auto anchor_panel = std::make_unique<engine::ui::UIPanel>(pos, size);
    // 设置背景色
    anchor_panel->set_background_color(sf::Color(26, 26, 26, 26));
    // 设置ID，以后即可根据ID找到该panel
    anchor_panel->set_id("unit_panel"_hs);

    // 依次添加角色肖像，每个肖像显示由四部分依次叠加：portrait，frame，icon，cost，可以通过一个frame_panel定位（位于上层anchor_panel之中）
    int index = 0;
    for (auto& [name_id, unit_data] : unit_map) {
        auto portrait = ui_config_->get_portrait(name_id);
        auto frame = ui_config_->get_portrait_frame(unit_data.rarity_);
        auto icon = ui_config_->get_icon(unit_data.class_id_);
        auto cost = blueprint_manager_->get_player_class_blueprint(unit_data.class_id_).player_.cost_;
        cost = static_cast<int>(std::round(engine::utils::stat_modify(cost, 1, unit_data.rarity_))); // 只有稀有度对cost有影响

        // 创建每个肖像的 frame_panel
        auto frame_pos = sf::Vector2f(padding + index * (frame_size.x + padding), padding);
        auto frame_panel = std::make_unique<engine::ui::UIPanel>(frame_pos, frame_size);
        frame_panel->set_id(name_id);

        // 依次添加四个元素，为了能够交互，将frame设置为按钮，并绑定点击事件
        frame_panel->add_child(std::make_unique<engine::ui::UIImage>(portrait, sf::Vector2f(0.f, 0.f), frame_size));
        frame_panel->add_child(std::make_unique<engine::ui::UIButton>(context_, 
            frame, 
            frame, 
            frame, 
            sf::Vector2f(0.f, 0.f), 
            frame_size
            // TODO: 添加点击事件回调函数
        ));
        frame_panel->add_child(std::make_unique<engine::ui::UIImage>(icon, sf::Vector2f(0.f, 0.f), frame_size / 2.f));
        frame_panel->add_child(std::make_unique<engine::ui::UILabel>(context_, 
            std::to_string(cost), 
            ui_config_->get_unit_panel_font_path(), 
            ui_config_->get_unit_panel_font_size(), 
            sf::Color::Yellow, 
            ui_config_->get_unit_panel_font_offset()
        ));
        // 最后添加一个灰色的遮盖panel，cost不足以支持该角色出击时显示
        auto cover_panel = std::make_unique<engine::ui::UIPanel>(sf::Vector2f(0.f, 0.f), frame_size);
        cover_panel->set_background_color(sf::Color(0, 0, 0, 51));
        cover_panel->set_id("cover_panel"_hs);
        frame_panel->add_child(std::move(cover_panel));

        // 将frame_panel添加到anchor_panel中，并使用cost作为排序键
        anchor_panel->add_child(std::move(frame_panel), cost);
        index++;
    }
    
    // 对anchor_panel中的子元素(frame_panel)进行排序
    anchor_panel->sort_children_by_order_index();
    // 按顺序排列anchor_panel中的子元素(frame_panel)的位置
    arrange_units_portrait_ui(anchor_panel.get(), frame_size, padding);

    ui_manager_->add_element(std::move(anchor_panel));
}

void GameScene::arrange_units_portrait_ui(engine::ui::UIElement* anchor_panel, const sf::Vector2f& frame_size, float padding) {
    // 遍历panel中的子元素(定位panel)，并依次设定位置
    for (size_t i = 0; i < anchor_panel->get_children().size(); i++) {
        auto& child = anchor_panel->get_children()[i];
        child->set_position(sf::Vector2f(padding + i * (frame_size.x + padding), padding));
    }
    // 更新panel的size
    anchor_panel->set_size(sf::Vector2f(padding + anchor_panel->get_children().size() * (frame_size.x + padding), 
                                    frame_size.y + 2 * padding));
}

void GameScene::on_enemy_arrive_home(const game::defs::EnemyArriveHomeEvent&) {
    spdlog::info("敌人到达基地");
    // TODO: 添加敌人到达基地的逻辑
}

void GameScene::test_session_data() {
    spdlog::info("关卡号: {}", level_number_);
    spdlog::info("积分: {}", session_data_->get_point());
    spdlog::info("是否通关: {}", session_data_->is_level_clear());
    for (auto& unit : session_data_->get_unit_map()) {
        spdlog::info("角色名: {}, 职业: {}, 等级: {}, 稀有度: {}", unit.second.name_, unit.second.class_, unit.second.level_, unit.second.rarity_);
    }
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
    auto world_pos = context_.get_camera().screen_to_world(mouse_pos);

    auto entity = entity_factory_->create_player_unit("warrior"_hs, world_pos);
    // 让玩家处于受伤状态（治疗师不会锁定满血目标）
    registry_.emplace<game::defs::InjuredTag>(entity);
    auto& stats = registry_.get<game::component::StatsComponent>(entity);
    stats.hp_ = stats.max_hp_ / 2.f;
    spdlog::info("创建战士: 位置: {}, {}", world_pos.x, world_pos.y);
    return true;
}

bool GameScene::on_create_test_player_ranged() {
    // 获取鼠标在窗口中的像素位置
    auto mouse_pos = context_.get_input_manager().get_mouse_position_window();
    // 通过 camera 将屏幕坐标转换为世界坐标（自动处理 viewport/letterbox 偏移）
    auto world_pos = context_.get_camera().screen_to_world(mouse_pos);

    auto entity = entity_factory_->create_player_unit("archer"_hs, world_pos);
    // 让玩家处于受伤状态（治疗师不会锁定满血目标）
    registry_.emplace<game::defs::InjuredTag>(entity);
    auto& stats = registry_.get<game::component::StatsComponent>(entity);
    stats.hp_ = stats.max_hp_ / 2.f;
    spdlog::info("创建弓箭手: 位置: {}, {}", world_pos.x, world_pos.y);
    return true;
}

bool GameScene::on_create_test_player_healer() {
    // 获取鼠标在窗口中的像素位置
    auto mouse_pos = context_.get_input_manager().get_mouse_position_window();
    // 通过 camera 将屏幕坐标转换为世界坐标（自动处理 viewport/letterbox 偏移）
    auto world_pos = context_.get_camera().screen_to_world(mouse_pos);

    entity_factory_->create_player_unit("witch"_hs, world_pos);
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
    timer_system_->update(registry_, delta);
    block_system_->update(registry_, dispatcher);
    set_target_system_->update(registry_);
    follow_path_system_->update(registry_, dispatcher, waypoint_nodes_, delta);
    orientation_system_->update(registry_);     // 调用顺序要在Block、SetTarget、FollowPath之后
    attack_starter_system_->update(registry_, dispatcher);
    projectile_system_->update(delta);
    movement_system_->update(registry_, delta);
    animation_system_->update(delta);
    ysort_system_->update(registry_);   // 调用顺序要在MovementSystem之后
    Scene::update(delta);
}

void GameScene::render() {
    auto& renderer = context_.get_renderer();
    auto& camera = context_.get_camera();

    // 注意渲染顺序，保证正确的遮盖关系
    render_system_->update(registry_, context_.get_renderer(), context_.get_camera());
    health_bar_system_->update(registry_, renderer, camera);

    Scene::render();
}
} // namespace game::scene
