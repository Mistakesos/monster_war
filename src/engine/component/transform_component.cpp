#include "engine/component/transform_component.hpp"

namespace engine::component {
TransformComponent::TransformComponent(engine::object::GameObject* owner
                                     , sf::Vector2f position
                                     , sf::Vector2f scale
                                     , sf::Angle angle
                                     , sf::Vector2f origin)
    : Component{owner}
    , position_{std::move(position)}
    , scale_{std::move(scale)}
    , angle_{std::move(angle)}
    , origin_{std::move(origin)} {
}
} // namespace engine::component