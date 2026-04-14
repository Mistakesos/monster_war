#pragma once
#include "engine/scene/scene.hpp"
#include "engine/system/fwd.hpp"
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

    std::unique_ptr<engine::system::RenderSystem> render_system_;
    std::unique_ptr<engine::system::MovementSystem> movement_system_;
    std::unique_ptr<engine::system::AnimationSystem> animation_system_;
    std::unique_ptr<engine::system::YSortSystem> ysort_system_;
};
}