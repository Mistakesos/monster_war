#include "game/system/render_range_system.hpp"
#include "game/component/unit_prep_component.hpp"
#include "game/defs/tags.hpp"
#include "engine/component/transform_component.hpp"
#include "engine/render/render.hpp"
#include "engine/render/camera.hpp"
#include "engine/utils/math.hpp"
#include <entt/entity/registry.hpp>

namespace game::system {

void RenderRangeSystem::update(entt::registry& registry, engine::render::Renderer& renderer, const engine::render::Camera& camera) {
    // 准备放置类型的单位
    auto view_prep = registry.view<game::defs::ShowRangeTag, engine::component::TransformComponent, game::component::UnitPrepComponent>();
    for (auto&& [entity, transform, prep] : view_prep.each()) {
        // 攻击范围显示为透明绿色圆形
        renderer.draw_filled_circle(camera, prep.range_, transform.position_, game::defs::RANGE_COLOR);
    }
    // TODO: 地图上的远程单位
}

}   // namespace game::system