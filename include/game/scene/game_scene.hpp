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
    // --- 测试回调事件 ---
    int scene_index_ = 0;
    void on_replace();
    void on_push();
    void on_pop();
    void on_quit();
};
}