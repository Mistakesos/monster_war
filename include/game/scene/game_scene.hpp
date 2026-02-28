#pragma once
#include "engine/scene/scene.hpp"

namespace game::scene {
/**
 * @brief 主要的游戏场景，包含玩家、敌人、关卡元素等
 */
class GameScene final : public engine::scene::Scene {
public:
    GameScene(engine::core::Context& context);
    
    ~GameScene();

private:
    // --- 资源管理器 ---
    void test_resource_manager();
};
}