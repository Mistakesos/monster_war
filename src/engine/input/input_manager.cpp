#include "engine/input/input_manager.hpp"
#include "engine/core/config.hpp"
#include "engine/utils/events.hpp"
#include "entt/signal/dispatcher.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <spdlog/spdlog.h>
#include <optional>
#include <ranges>

namespace engine::input {

InputManager::InputManager(sf::RenderWindow* window, const engine::core::Config* config, entt::dispatcher* dispatcher)
    : window_obs_{window}
    , config_obs_{config}
    , action_to_input_copy_{config->action_to_input_}
    , dispatcher_{dispatcher} {

    for (const auto& [action, _] : action_to_input_copy_) {
        action_states_.emplace(action, ActionState::Inactive);
    }
}

entt::sink<entt::sigh<bool()>> InputManager::on_action(Action action_name, ActionState action_state) {
    // 如果 action_name 不存在，自动创建一个 std::array<...>
    // .at() 会进行边界检查，更安全
    return actions_to_func_[action_name].at(static_cast<size_t>(action_state));
}

// === 事件驱动核心 ===

void InputManager::begin_frame() {
    // 帧开始：清理瞬时状态
    for (auto& state : action_states_ | std::views::values) {
        if (state == ActionState::Pressed) {
            state = ActionState::Held;
        } else if (state == ActionState::Released) {
            state = ActionState::Inactive;
        }
    }
}

void InputManager::handle_event(const sf::Event& event) {
    // --- 窗口关闭 ---
    if (event.is<sf::Event::Closed>()) {
        window_obs_->close();
        return;
    }

    // --- 全屏切换 ---
    if (auto key = event.getIf<sf::Event::KeyPressed>(); key) {
        if (key->scancode == sf::Keyboard::Scan::F11) {
            toggle_fullscreen();
        }
    }

    // --- 按键按下 ---
    if (auto key = event.getIf<sf::Event::KeyPressed>(); key) {
        update(key->scancode, true);
    }

    // --- 按键释放 ---
    if (auto key = event.getIf<sf::Event::KeyReleased>(); key) {
        update(key->scancode, false);
    }

    // --- 鼠标按下 ---
    if (auto mouse = event.getIf<sf::Event::MouseButtonPressed>(); mouse) {
        update(mouse->button, true);
    }

    // --- 鼠标释放 ---
    if (auto mouse = event.getIf<sf::Event::MouseButtonReleased>(); mouse) {
        update(mouse->button, false);
    }
}

void InputManager::quit() {
    dispatcher_->trigger<engine::utils::QuitEvent>();
}

void InputManager::end_frame() {
    // 触发回调
    for (const auto& [action, state] : action_states_) {
        if (state != ActionState::Inactive) {
            if (auto it = actions_to_func_.find(action); it != actions_to_func_.end()) {
                // collect方法可以获取回调函数返回值，放入lambda函数的参数中。
                // 而lambda函数的返回值为真时，停止分发信号。
                // 分发信号的顺序为“后绑定先调用”
                it->second.at(static_cast<size_t>(state)).collect([](bool result) {
                    return result;
                });
            }
        }
    }
}

// === 内部工具 ===

void InputManager::toggle_fullscreen() {
    
    if (is_full_screen_) {
        // 根据缩放调整窗口大小
        sf::Vector2u window_size = static_cast<sf::Vector2u>(static_cast<sf::Vector2f>(config_obs_->window_size_) * config_obs_->window_scale_);
        window_obs_->create(sf::VideoMode(window_size), config_obs_->window_title_, sf::State::Windowed);
    } else {
        window_obs_->create(sf::VideoMode::getDesktopMode(), config_obs_->window_title_, sf::State::Fullscreen);
    }

    is_full_screen_ = !is_full_screen_;

    dispatcher_->trigger<engine::utils::WindowResizedEvent>(engine::utils::WindowResizedEvent(window_obs_->getSize()));
}

// === 查询接口 ===

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

// === 鼠标位置 ===

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
} // namespace engine::input