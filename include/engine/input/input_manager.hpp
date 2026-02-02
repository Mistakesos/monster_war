#pragma once
#include "engine/utils/action.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/System/Vector2.hpp>
#include <unordered_map>
#include <vector>
#include <variant>

namespace sf {
    class RenderWindow;
} // namespace sf

namespace engine::core {
    class Config;
} // namespace engine::core

namespace engine::input {
enum class ActionState {
    Inactive,               ///< @brief 动作未激活
    PressedThisFrame,       ///< @brief 动作在本帧刚刚按下
    HeldDown,               ///< @brief 动作被持续按下
    ReleasedThisFrame       ///< @brief 动作在本帧被刚刚释放
};

class InputManager final {
public:
    InputManager(sf::RenderWindow* window, const engine::core::Config* config);
    ~InputManager() = default;
    
    void update();                                      ///< @brief 更新输入状态，每轮循环最先调用
    
    bool is_action_held(Action action) const;           ///< @brief 动作当前是否触发（持续按下或本帧按下）
    bool is_action_pressed(Action action) const;        ///< @brief 动作是否在本帧刚刚按下
    bool is_action_released(Action action) const;       ///< @brief 动作是否在本帧刚刚释放

    bool should_quit() const { return should_quit_; }   ///< @brief 查询退出状态
    void set_should_quit(bool should_quit) { should_quit_ = should_quit; }  ///< @brief 设置退出状态
    
    sf::Vector2i get_mouse_position() const;            ///< @brief 获取鼠标位置（屏幕坐标）
    sf::Vector2i get_mouse_position_window() const;     ///< @brief 获取鼠标位置（窗口坐标）
    sf::Vector2i get_mouse_logical_position() const;    ///< @brief 获取鼠标逻辑坐标（窗口坐标，考虑 view 缩放）
    
    void process_event();
    
private:
    sf::RenderWindow* window_obs_ = nullptr;                                                            ///< @brief 用于传入获取鼠标的逻辑位置
    using Scancode = sf::Keyboard::Scancode;
    using Button = sf::Mouse::Button;
    std::unordered_map<Action, std::vector<std::variant<Scancode, Button>>> action_to_input_copy_;     ///< @brief 动作到具体输入
    std::unordered_map<Action, ActionState> action_state_map_;                                         ///< @brief 储存每个动作当前的状态

    bool should_quit_ = false;           ///< @brief 退出标志
    bool is_full_screen_ = false;        ///< @brief 是否全屏
};
} // namespace engine::input