#pragma once
#include "engine/ui/ui_element.hpp"
#include "engine/ui/state/ui_state.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <entt/core/fwd.hpp>
#include <entt/core/hashed_string.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace engine::core {
    class Context;
} // namespace engine::core

namespace engine::ui {
/**
 * @brief 可交互UI元素的基类,继承自UIElement
 *
 * 定义了可交互UI元素的通用属性和行为。
 * 管理UI状态的切换和交互逻辑。
 * 提供事件处理、更新和渲染的虚方法。
 */
class UIInteractive : public UIElement {
public:
    UIInteractive(engine::core::Context& context, sf::Vector2f position = {0.f, 0.f}, sf::Vector2f size = {0.f, 0.f});
    ~UIInteractive() override;

    virtual void clicked() {}       ///< @brief 如果有点击事件，则重写该方法

    void add_sprite(entt::id_type name_id, std::unique_ptr<sf::Sprite> sprite);     ///< @brief 添加精灵
    void set_sprite(entt::id_type name_id);                                         ///< @brief 设置当前显示的精灵
    void add_sound(entt::id_type name_id, entt::hashed_string hashed_path);         ///< @brief 添加音效（预加载）
    void play_sound(entt::id_type name_id);                                         ///< @brief 播放音效
    // --- Getters and Setters ---
    void set_state(std::unique_ptr<engine::ui::state::UIState> state);              ///< @brief 设置当前状态
    engine::ui::state::UIState* get_state() const { return current_state_.get(); }  ///< @brief 获取当前状态

    void set_interactive(bool interactive) { interactive_ = interactive; }          ///< @brief 设置是否可交互
    bool is_interactive() const { return interactive_; }                            ///< @brief 获取是否可交互

    // --- 核心方法 ---
    bool handle_input(engine::core::Context& context) override;
    void render(engine::core::Context& context) override;

protected:
    engine::core::Context& context_;                                ///< @brief 可交互元素很可能需要其他引擎组件
    std::unique_ptr<engine::ui::state::UIState> current_state_;     ///< @brief 当前状态
    std::unordered_map<entt::id_type, std::unique_ptr<sf::Sprite>> sprites_; ///< @brief 精灵集合，键为名称ID
    std::unordered_map<entt::id_type, entt::id_type> sounds_;       ///< @brief 音效集合，key为音效名称ID，value为音效ID（路径哈希）
    entt::id_type current_sprite_id_;                  ///< @brief 当前显示的精灵ID
    bool interactive_ = true;                                       ///< @brief 是否可交互
};
} // namespace engine::ui