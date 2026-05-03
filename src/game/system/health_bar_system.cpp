#include "game/system/health_bar_system.hpp"
#include "game/component/stats_component.hpp"
#include "engine/component/transform_component.hpp"
#include "game/defs/tags.hpp"
#include "game/defs/constants.hpp"
#include "engine/render/render.hpp"
#include "engine/render/camera.hpp"
#include "engine/utils/math.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <entt/entity/registry.hpp>

namespace game::system {

void HealthBarSystem::update(entt::registry& registry, engine::render::Renderer& renderer, engine::render::Camera& camera) {
    // 只有受伤的实体才显示血量标签
    auto view = registry.view<engine::component::TransformComponent,
        game::component::StatsComponent,
        game::defs::HasHealthBarTag,
        game::defs::InjuredTag>();

    for (auto entity : view) {
        const auto [transform, stats] = view.get<engine::component::TransformComponent, game::component::StatsComponent>(entity);

        auto size = game::defs::HEALTH_BAR_SIZE;
        // 血量条位置 = 角色位置 + 偏移量
        auto position = transform.position_ + sf::Vector2f(-size.x / 2.f, game::defs::HEALTH_BAR_OFFSET_Y);
        
        // 根据血量百分比确定颜色
        auto health_percent = static_cast<float>(stats.hp_) / static_cast<float>(stats.max_hp_);
        sf::Color color;
        if (health_percent > 0.7f) {
            color = sf::Color::Green;
        } else if (health_percent > 0.3f) {
            color = sf::Color(255, 166, 0, 255);    // Orange
        } else {
            color = sf::Color::Red;
        }

        // 执行绘制(先画边框，再画血量)
        renderer.draw_ui_rect(camera, {position, size}, color);
        size.x = size.x * health_percent;
        renderer.draw_ui_filled_rect(camera, {position, size}, color);
    }
}

} // namespace game::system