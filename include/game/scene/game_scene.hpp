#pragma once
#include "engine/scene/scene.hpp"
#include "engine/system/fwd.hpp"
#include "game/system/fwd.hpp"
#include "game/data/waypoint_node.hpp"
#include "game/defs/events.hpp"
#include <memory>

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
    [[nodiscard]] bool load_level();
    [[nodiscard]] bool init_event_connections();

    // 事件回调函数
    void on_enemy_arrive_home(const game::defs::EnemyArriveHomeEvent& event);

    // 测试函数
    void create_test_enemy();

    std::unique_ptr<engine::system::RenderSystem> render_system_;
    std::unique_ptr<engine::system::MovementSystem> movement_system_;
    std::unique_ptr<engine::system::AnimationSystem> animation_system_;
    std::unique_ptr<engine::system::YSortSystem> ysort_system_;

    std::unique_ptr<game::system::FollowPathSystem> follow_path_system_;
    std::unique_ptr<game::system::RemoveDeadSystem> remove_dead_system_;

    std::unordered_map<int, game::data::WaypointNode> waypoint_nodes_;  // 路径节点ID到节点数据的映射
    std::vector<int> start_points_;                                     // 起点ID列表
};
}