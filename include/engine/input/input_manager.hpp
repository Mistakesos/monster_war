#pragma once
#include "engine/utils/action.hpp"
#include "entt/signal/sigh.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
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
    Pressed,       ///< @brief 动作在本帧刚刚按下
    Held,          ///< @brief 动作被持续按下
    Released,      ///< @brief 动作在本帧被刚刚释放
    Inactive       ///< @brief 动作未激活
};

class InputManager final {
public:
    InputManager(sf::RenderWindow* window, const engine::core::Config* config);
    ~InputManager() = default;
    
    /**
     * @brief 注册一个动作的回调函数
     * @param action_name 动作名称
     * @param action_state 动作状态，默认为按下瞬间
     * @return 一个 sink 对象，用于注册回调函数
     */
    entt::sink<entt::sigh<void()>> on_action(Action action_name, ActionState action_state = ActionState::Pressed);
       
    // 保留动作状态检查，提供不同的使用选择
    bool is_action_held(Action action) const;           ///< @brief 动作当前是否触发（持续按下或本帧按下）
    bool is_action_pressed(Action action) const;        ///< @brief 动作是否在本帧刚刚按下
    bool is_action_released(Action action) const;       ///< @brief 动作是否在本帧刚刚释放

    bool should_quit() const { return should_quit_; }   ///< @brief 查询退出状态
    void set_should_quit(bool should_quit) { should_quit_ = should_quit; }  ///< @brief 设置退出状态
    
    sf::Vector2i get_mouse_position() const;            ///< @brief 获取鼠标位置（屏幕坐标）
    sf::Vector2i get_mouse_position_window() const;     ///< @brief 获取鼠标位置（窗口坐标）
    sf::Vector2i get_mouse_logical_position() const;    ///< @brief 获取鼠标逻辑坐标（窗口坐标，考虑 view 缩放）

    void begin_frame();
    void end_frame();
    void handle_event(const sf::Event& event);

private:
    template <typename Input>
    void update(Input input, bool pressed);

    void toggle_fullscreen();

    sf::RenderWindow* window_obs_ = nullptr;        ///< @brief 用于传入获取鼠标的逻辑位置
    /**
     * @brief 核心数据结构：存储动作名称函数列表的映射
     * 
     * @note 每个动作有3个状态：Pressed、Held、Released，每个状态对应一个回调函数
     * @note 绑定动作时再插入元素（懒加载），初始化时为空
     */
    std::unordered_map<Action, std::array<entt::sigh<void()>, 3>> actions_to_func_;
    using Scancode = sf::Keyboard::Scancode;
    using Button = sf::Mouse::Button;
    std::unordered_map<Action, std::vector<std::variant<Scancode, Button>>> action_to_input_copy_;     ///< @brief 动作到具体输入
    std::unordered_map<Action, ActionState> action_states_;                                            ///< @brief 储存每个动作当前的状态

    bool should_quit_ = false;           ///< @brief 退出标志
    bool is_full_screen_ = false;        ///< @brief 是否全屏
};
} // namespace engine::input

// template func
#include "engine/input/input_manager.tpp"