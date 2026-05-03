#include "game/system/effect_system.hpp"
#include "game/defs/events.hpp"
#include "game/factory/entity_factory.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

namespace game::system {

EffectSystem::~EffectSystem() {
    dispatcher_.disconnect(this);
}

EffectSystem::EffectSystem(entt::registry& registry, entt::dispatcher& dispatcher, game::factory::EntityFactory& entity_factory)
    : registry_{registry}
    , dispatcher_{dispatcher}
    , entity_factory_{entity_factory} {
    dispatcher_.sink<game::defs::EnemyDeadEffectEvent>().connect<&EffectSystem::on_enemy_dead_effect_event>(this);
}

void EffectSystem::on_enemy_dead_effect_event(const game::defs::EnemyDeadEffectEvent& event) {
    entity_factory_.create_enemy_dead_effect(event.class_id_, event.position_, event.is_flipped_);
}

} // namespace game::system