#pragma once
#include "engine/utils/action.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <nlohmann/json_fwd.hpp>        // 官方提供的前向声明头文件

namespace engine::core {
/**
 * @brief 管理应用程序的配置设置
 * 提供配置项的默认值，且支持从 JSON 文件加载/保存配置
 * 如果加载失败或文件不存在，将使用默认值
 */
class Config final {
public:
    explicit Config(std::string_view filepath);   ///< @brief 构造函数，制定配置文件路径
    ~Config() = default;

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&) = delete;
    Config& operator=(Config&&) = delete;

    bool load_from_file(std::string_view filepath);               ///< @brief 从指定的json文件加载配置，成功返回true
    [[nodiscard]] bool save_to_file(std::string_view filepath);   ///< @brief 将当前配置保存到指定的json文件，成功返回true（不应忽略返回值）

private:
    void from_json(const nlohmann::json& json);
    nlohmann::ordered_json to_json() const;

public:
    // --- 默认配置 ---
    std::string window_title_ = "Sunny Land";
    sf::Vector2u window_size_ = {1280u, 720u};
    bool window_resizable_ = true;

    // 图形设置
    bool vsync_enabled_ = false;                    ///< @brief 垂直同步（默认关闭）

    // 性能设置
    unsigned int target_fps_ = 60;                  ///< @brief 目标FPS，0表示无限制

    // 音频设置
    float music_volume_ = 100.f;
    float sound_volume_ = 100.f;

    // 存储动作名称到 sfml scancode/button 的名称列表映射
    using Scancode = sf::Keyboard::Scancode;
    using Button = sf::Mouse::Button;
    std::unordered_map<Action, std::vector<Scancode>> keyboard_input_mappings_ = {
        {Action::MoveLeft, {Scancode::A, Scancode::Left}},
        {Action::MoveRight, {Scancode::D, Scancode::Right}},
        {Action::MoveUp, {Scancode::W, Scancode::Up}},
        {Action::MoveDown, {Scancode::S, Scancode::Down}},
        {Action::Jump, {Scancode::J, Scancode::Space}},
        {Action::Attack, {Scancode::K}},
        {Action::Pause, {Scancode::P, Scancode::Escape}}
    };
    std::unordered_map<Action, std::vector<Button>> mouse_input_mappings_ = {
        {Action::MouseLeftClick, {Button::Left}},
        {Action::MouseRightClick, {Button::Right}},
        {Action::Attack, {Button::Left}}
    };
    std::unordered_map<Action, std::vector<std::variant<Scancode, Button>>> action_to_input_;
};
} // namespace engine::core