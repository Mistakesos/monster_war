#include "engine/component/component.hpp"

namespace engine::component {
Component::Component(engine::object::GameObject* owner)
    : owner_{owner} {
}

void Component::set_owner(engine::object::GameObject* owner) {
    this->owner_ = owner;
}

engine::object::GameObject* Component::get_owner() const {
    return this->owner_;
}
} // namespace engine::component