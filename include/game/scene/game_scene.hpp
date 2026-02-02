#pragma once
#include "engine/scene/scene.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>

namespace game::scene {
/**
 * @brief 主要的游戏场景，包含玩家、敌人、关卡元素等
 */
class GameScene final : public engine::scene::Scene {
public:
    GameScene(engine::core::Context& context,
              engine::scene::SceneManager& scene_manager
    );
    
    ~GameScene() = default;
};
}