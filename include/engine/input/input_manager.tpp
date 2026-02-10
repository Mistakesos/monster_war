namespace engine::input {
template <typename Input>
void InputManager::update(Input input, bool pressed) {
    for (auto& [action, inputs] : action_to_input_copy_) {
        bool matched = false;

        for (const auto& variant_input : inputs) {
            std::visit([&](auto&& value) {
                    using InputType = std::decay_t<decltype(value)>;
                    if constexpr (std::is_same_v<InputType, Input>) {
                        if (value == input) matched = true;
                    }
            }, variant_input);

            if (matched) break;
        }

        if (!matched) continue;

        ActionState& state = action_states_[action];

        if (pressed) {
            if (state == ActionState::Inactive || state == ActionState::Released)
                state = ActionState::Pressed;
            else
                state = ActionState::Held;
        } else {
            if (state == ActionState::Pressed || state == ActionState::Held)
                state = ActionState::Released;
            else
                state = ActionState::Inactive;
        }
    }
}
} // namespace engine::input