#include "engine/input/input_manager.hpp"
#include "engine/core/config.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <spdlog/spdlog.h>
#include <optional>

namespace engine::input {
InputManager::InputManager(sf::RenderWindow* window, const engine::core::Config* config) 
    : window_obs_{window}
    , action_to_input_copy_{config->action_to_input_} {
    for (const auto& [action, inputs] : action_to_input_copy_) {
        action_states_.emplace(action, ActionState::Inactive);
    }
}

entt::sink<entt::sigh<void()>> InputManager::on_action(Action action_name, ActionState action_state) {
    // 如果 action_name 不存在，自动创建一个 std::array<...>
    // .at() 会进行边界检查，更安全
    return actions_to_func_[action_name].at(static_cast<size_t>(action_state));
}

void InputManager::update() {
    // 根据上一帧的状态更新状态
    for (auto& [action, state] : action_states_) {
        if (state == ActionState::Pressed) {
            state = ActionState::Held;
        } else if (state == ActionState::Released) {
            state = ActionState::Inactive;
        }
    }

    // 处理按键是否按下，并决定是否更改状态
    for (auto& [action, inputs] : action_to_input_copy_) {
        bool pressed = false;

        for (const auto& input : inputs) {
            std::visit([&pressed](auto&& input_value) {
                using T = std::decay_t<decltype(input_value)>;

                if constexpr (std::is_same_v<T, sf::Keyboard::Scancode>) {
                    if (sf::Keyboard::isKeyPressed(input_value)) {
                        pressed = true;
                    }
                } else if constexpr (std::is_same_v<T, sf::Mouse::Button>) {
                    if (sf::Mouse::isButtonPressed(input_value)) {
                        pressed = true;
                    }
                }
            }, input);

            if (pressed) break;
        }

        ActionState prev = action_states_[action];
        if (pressed) {
            if (prev == ActionState::Inactive || prev == ActionState::Released)
                action_states_[action] = ActionState::Pressed;
            else
                action_states_[action] = ActionState::Held;
        } else {
            if (prev == ActionState::Held || prev == ActionState::Pressed)
                action_states_[action] = ActionState::Released;
            else
                action_states_[action] = ActionState::Inactive;
        }
    }

    // 触发回调
    for (const auto& [action_name, state] : action_states_) {
        if (state != ActionState::Inactive) {   // 如果动作状态不是 Inactive
            // 且有绑定回调函数
            if (auto it = actions_to_func_.find(action_name); it != actions_to_func_.end()) {
                it->second.at(static_cast<size_t>(state)).publish();    // 触发回调
            }
        }
    }
}

bool InputManager::is_action_held(Action action) const {
    if (auto it = action_states_.find(action); it != action_states_.end()) {
        return it->second == ActionState::Pressed || it->second == ActionState::Held;
    }
    return false;
}

bool InputManager::is_action_pressed(Action action) const {
    if (auto it = action_states_.find(action); it != action_states_.end()) {
        return it->second == ActionState::Pressed;
    }
    return false;
}

bool InputManager::is_action_released(Action action) const {
    if (auto it = action_states_.find(action); it != action_states_.end()) {
        return it->second == ActionState::Released;
    }
    return false;
}

sf::Vector2i InputManager::get_mouse_position() const {
    return sf::Mouse::getPosition();
}

sf::Vector2i InputManager::get_mouse_position_window() const {
    return sf::Mouse::getPosition(*window_obs_);
}

sf::Vector2i InputManager::get_mouse_logical_position() const {
    const sf::View& view = window_obs_->getView();
    sf::Vector2i mouse_position = get_mouse_position_window();
    sf::Vector2f scale = view.getSize().componentWiseDiv(static_cast<sf::Vector2f>(window_obs_->getSize()));
    sf::Vector2f logical_position = static_cast<sf::Vector2f>(mouse_position).componentWiseMul(scale);
    return static_cast<sf::Vector2i>(logical_position);
}

void InputManager::process_event() {
    while (std::optional event = window_obs_->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_obs_->close();
        } else if (auto key_pressed = event->getIf<sf::Event::KeyPressed>(); key_pressed) {
            if (key_pressed->scancode == sf::Keyboard::Scan::F11) {
                window_obs_->close();

                if (is_full_screen_) {
                    window_obs_->create(sf::VideoMode(sf::Vector2u(1280u, 720u)), "Sunny Land", sf::State::Windowed);
                } else {
                    window_obs_->create(sf::VideoMode::getDesktopMode(), "Sunny Land", sf::State::Fullscreen);
                }
                
                is_full_screen_ = !is_full_screen_;
            }
        }
    }
}
} // namespace engine::input