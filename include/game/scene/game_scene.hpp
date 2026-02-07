#pragma once
#include "engine/scene/scene.hpp"

namespace game::scene {
/**
 * @brief 主要的游戏场景，包含玩家、敌人、关卡元素等
 */
class GameScene final : public engine::scene::Scene {
public:
    GameScene(engine::core::Context& context,
              engine::scene::SceneManager& scene_manager
    );
    
    ~GameScene();

private:
    // --- 测试输入回调事件 ---
    void on_attack();
    void on_jump();
};
}