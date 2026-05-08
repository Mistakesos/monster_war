#pragma once
#include "engine/scene/scene.hpp"
#include "engine/system/fwd.hpp"
#include "game/system/fwd.hpp"
#include "game/data/waypoint_node.hpp"
#include "game/data/session_data.hpp"
#include "game/data/ui_config.hpp"
#include "game/defs/events.hpp"
#include <memory>

namespace engine::ui {
    class UIElement;
} // namespace engine::ui

namespace game::factory {
    class EntityFactory;
    class BlueprintManager;
} // namespace game::factory

namespace game::scene {
/**
 * @brief 主要的游戏场景，包含玩家、敌人、关卡元素等
 */
class GameScene final : public engine::scene::Scene {
public:
    GameScene(engine::core::Context& context);
    ~GameScene();

    void update(sf::Time delta) override;
    void render() override;

private:
    [[nodiscard]] bool init_session_data();
    [[nodiscard]] bool init_ui_config();
    [[nodiscard]] bool load_level();
    [[nodiscard]] bool init_event_connections();
    [[nodiscard]] bool init_input_connections();
    [[nodiscard]] bool init_entity_factory();
    [[nodiscard]] bool init_systems();

    void create_units_portrait_ui();       ///< @brief 创建画面下方的单位肖像UI

    ///< @brief 排列画面下方的单位肖像UI (肖像增/减时调用)
    void arrange_units_portrait_ui(engine::ui::UIElement* anchor_panel, const sf::Vector2f& frame_size, float padding);

    // 事件回调函数
    void on_enemy_arrive_home(const game::defs::EnemyArriveHomeEvent& event);

    // 测试函数
    void test_session_data();
    void create_test_enemy();
    bool on_create_test_player_melee();
    bool on_create_test_player_ranged();
    bool on_create_test_player_healer();
    bool on_clear_all_players();

    std::unique_ptr<engine::system::RenderSystem> render_system_;
    std::unique_ptr<engine::system::MovementSystem> movement_system_;
    std::unique_ptr<engine::system::AnimationSystem> animation_system_;
    std::unique_ptr<engine::system::YSortSystem> ysort_system_;
    std::unique_ptr<engine::system::AudioSystem> audio_system_;

    std::unique_ptr<game::system::FollowPathSystem> follow_path_system_;
    std::unique_ptr<game::system::RemoveDeadSystem> remove_dead_system_;
    std::unique_ptr<game::system::BlockSystem> block_system_;
    std::unique_ptr<game::system::SetTargetSystem> set_target_system_;
    std::unique_ptr<game::system::AttackStarterSystem> attack_starter_system_;
    std::unique_ptr<game::system::TimerSystem> timer_system_;
    std::unique_ptr<game::system::OrientationSystem> orientation_system_;
    std::unique_ptr<game::system::AnimationStateSystem> animation_state_system_;
    std::unique_ptr<game::system::AnimationEventSystem> animation_event_system_;
    std::unique_ptr<game::system::CombatResolveSystem> combat_resolve_system_;
    std::unique_ptr<game::system::ProjectileSystem> projectile_system_;
    std::unique_ptr<game::system::EffectSystem> effect_system_;
    std::unique_ptr<game::system::HealthBarSystem> health_bar_system_;

    std::unordered_map<int, game::data::WaypointNode> waypoint_nodes_;  // 路径节点ID到节点数据的映射
    std::vector<int> start_points_;                                     // 起点ID列表

    std::unique_ptr<game::factory::EntityFactory> entity_factory_;      // 实体工厂，负责创建和管理实体

    // 管理数据的实例很可能同时被多个场景使用，因此使用共享指针
    std::shared_ptr<game::factory::BlueprintManager> blueprint_manager_;// 蓝图管理器，负责管理蓝图数据
    std::shared_ptr<game::data::SessionData> session_data_;             // 会话数据，关卡切换时需要传递的数据
    std::shared_ptr<game::data::UIConfig> ui_config_;                   // UI配置，负责管理UI数据

    // --- 其他场景数据 ---
    int level_number_{1};
};

}