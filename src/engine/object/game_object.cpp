#include "engine/object/game_object.hpp"
#include <SFML/System/Time.hpp>
#include <ranges>

namespace engine::object {
GameObject::GameObject(std::string_view name, std::string_view tag)
    : name_{name}
    , tag_{tag} {
}

void GameObject::handle_input(engine::core::Context& context) {
    for (auto& component : components_ | std::views::values) {
        component->handle_input(context);
    }
}

void GameObject::update(sf::Time delta, engine::core::Context& context) {
    for (auto& component : components_ | std::views::values) {
        component->update(delta, context);
    }
}

void GameObject::render(engine::core::Context& context) {
    for (auto& component : components_ | std::views::values) {
        component->render(context);
    }
}
} // namespace engine::object