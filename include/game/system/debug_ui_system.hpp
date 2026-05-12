#pragma once
#include <entt/entity/fwd.hpp>

namespace engine::core {
    class Context;
} // namespace engine::core 

namespace game::system {

/**
 * @brief 调试 UI 系统，负责显示调试 UI。
 * 
 * @note 调试UI的主要目的是方便debug，并快速开发UI原型。
 * @note 游戏正式发布时往往会删除，因此不需要过度设计。
 */
class DebugUISystem {
public:
    DebugUISystem(entt::registry& registry, engine::core::Context& context);

    void update();

private:
    // 封装每个UI显示模块
    void render_demo_ui();

    entt::registry& registry_;
    engine::core::Context& context_;
};

} // namespace game::system