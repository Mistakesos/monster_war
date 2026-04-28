#include "game/system/followpath_system.hpp"
#include "game/data/waypoint_node.hpp"
#include "game/component/enemy_component.hpp"
#include "game/component/blocked_by_component.hpp"
#include "engine/component/velocity_component.hpp"
#include "engine/component/transform_component.hpp"
#include "game/defs/tags.hpp"
#include "game/defs/events.hpp"
#include "engine/utils/math.hpp"
#include <entt/signal/dispatcher.hpp>
#include <entt/entity/registry.hpp>
#include <SFML/System/Vector2.hpp>
#include <spdlog/spdlog.h>

namespace game::system {

void FollowPathSystem::update(entt::registry& registry, 
                              entt::dispatcher& dispatcher, 
                              std::unordered_map<int, game::data::WaypointNode>& waypoint_nodes,
                              sf::Time delta) {
    spdlog::trace("FollowPathSystem::update");
    // 筛选依据：速度组件、变换组件、敌人组件，排除“被阻挡的敌人”和“动作锁定敌人”
    auto view = registry.view<engine::component::VelocityComponent, 
                              engine::component::TransformComponent, 
                              game::component::EnemyComponent>(
                              entt::exclude<game::component::BlockedByComponent, game::defs::ActionLockTag>);
    for (auto&& [entity, velocity, transform, enemy] : view.each()) {
        // 如果速度为0，直接跳过移动逻辑
        if (enemy.speed_ == 0.f) {
            velocity.velocity_ = sf::Vector2f(0.f, 0.f);
            continue;
        }
        
        // 获取目标节点
        auto target_node = waypoint_nodes.at(enemy.target_waypoint_id_);        

        // 计算当前位置到目标位置的向量
        sf::Vector2f direction = target_node.position_ - transform.position_;

        // 如果距离小于阈值，则切换到下一个节点（阈值不要太小，不然敌人速度快的话可能造成震荡）
        float frame_step = std::abs(enemy.speed_) * delta.asSeconds();
        float threshold = std::max(frame_step * 1.2f, 1.f);
        if (direction.lengthSquared() < threshold * threshold) {
            // 如果下一个节点ID列表为空，代表到达终点。则发送信号并添加删除标记
            auto size = target_node.next_node_ids_.size();
            if (size == 0) {
                spdlog::info("到达终点");
                // 发送信号并添加删除标记
                dispatcher.enqueue<game::defs::EnemyArriveHomeEvent>(); // 具体做什么，由回调函数决定
                registry.emplace<game::defs::DeadTag>(entity);          // 用于延迟删除
                continue;
            }
            // 随机选择下一个节点
            auto target_index = engine::utils::range_random(0, size - 1);
            enemy.target_waypoint_id_ = target_node.next_node_ids_[target_index];
            // 更新目标节点与方向矢量
            target_node = waypoint_nodes.at(enemy.target_waypoint_id_);
            direction = target_node.position_ - transform.position_;
        }

        // 更新速度组件：velocity = 方向矢量 * speed
        velocity.velocity_ = direction.normalized() * enemy.speed_;
    }
}

}