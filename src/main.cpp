#include "entt/entt.hpp"
#include <spdlog/spdlog.h>

// 组件是一些简单的结构体或类，他们只包含数据
// 定义两个组件：位置和速度
struct Position {
    float x, y;
};

struct Velocity {
    float dx, dy;
};

struct Tag {
    entt::hashed_string value;
};

int main() {
    using namespace entt::literals;
    
    entt::registry registry;

    entt::entity player = registry.create();
    registry.emplace<Position>(player, 10.f, 20.f);
    registry.emplace<Velocity>(player, 1.f, 0.5f);
    
    entt::entity enemy = registry.create();
    registry.emplace<Position>(enemy, 100.f, 50.f);
    registry.emplace<Velocity>(enemy, -0.5f, -1.f);

    registry.emplace<Tag>(player, "player"_hs);
    registry.emplace<Tag>(enemy, "enemy"_hs);

    spdlog::info("=== 使用哈希字符串标签 ===");

    auto view = registry.view<const Tag, const Position>();

    view.each([](const auto& entity_tag, const auto& pos) {
        if (entity_tag.value == "player"_hs) {
            spdlog::info("找到玩家，位置：({}, {})", pos.x, pos.y);
        }

        if (entity_tag.value == "enemy"_hs) {
            spdlog::info("找到敌人，位置：({}, {})", pos.x, pos.y);
        }
    });
    
    auto player_tag = registry.get<Tag>(player);
    spdlog::info("玩家标签的哈希值：{}", player_tag.value.value());
    spdlog::info("玩家标签的原始文本：{}", player_tag.value.data());
    
    return 0;
}