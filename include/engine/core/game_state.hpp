#pragma once
#include <optional>
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

namespace engine::core {
/**
 * @enum State
 * @brief 定义游戏可能处于的宏观状态。
 */
enum class State {
    Title,          ///< @brief 标题界面
    Playing,        ///< @brief 正常游戏进行中
    Paused,         ///< @brief 游戏暂停（通常覆盖菜单界面）
    GameOver,       ///< @brief 游戏结束界面
    // 可以根据需要添加更多状态，如 Cutscene, SettingsMenu 等
};

/**
 * @brief 管理和查询游戏的全局宏观状态。
 *
 * 提供一个中心点来确定游戏当前处于哪个主要模式，
 * 以便其他系统（输入、渲染、更新等）可以相应地调整其行为。
 */
class GameState final {
public:
    /**
     * @brief 构造函数，初始化游戏状态。
     * @param window sfml窗口，必须传入有效值。
     * @param initial_state 游戏的初始状态，默认为 Title
     */
    explicit GameState(sf::RenderWindow* window, State initial_state = State::Title);
    ~GameState() = default;

    void set_window_size(sf::Vector2u new_size);
    void set_state(State new_state);
    void set_logical_size(sf::Vector2f new_size);
    sf::Vector2u get_window_size() const;
    State get_current_state() const { return current_state_; }
    sf::Vector2f get_logical_size() const;

    // --- 便捷查询方法 ---

    bool is_in_title() const { return current_state_ == State::Title; }
    bool is_playing() const { return current_state_ == State::Playing; }
    bool is_paused() const { return current_state_ == State::Paused; }
    bool is_game_over() const { return current_state_ == State::GameOver; }

private:    
    sf::RenderWindow* window_obs_ = nullptr;              ///< @brief SDL窗口，用于获取窗口大小
    State current_state_ = State::Title;        ///< @brief 当前游戏状态
};
} // namespace engine::core