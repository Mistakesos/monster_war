#pragma once
#include "engine/ui/ui_element.hpp"
#include "engine/ui/state/ui_state.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <entt/core/fwd.hpp>
#include <entt/entity/fwd.hpp>
#include <memory>
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
    virtual void hover_enter() {}   ///< @brief 如果有悬停进入事件，则重写该方法
    virtual void hover_leave() {}   ///< @brief 如果有悬停离开事件，则重写该方法

    void add_sprite(entt::id_type name_id, sf::Sprite sprite);                     ///< @brief 添加/替换图片
    void set_current_sprite(entt::id_type name_id);                                 ///< @brief 设置当前显示的图片

    void set_hover_sound(entt::id_type id, std::string_view path = "");             ///< @brief 设置悬浮音效
    void set_click_sound(entt::id_type id, std::string_view path = "");             ///< @brief 设置点击音效
    void play_sound(entt::id_type name_id);                                         ///< @brief 播放音效

    // --- Getters and Setters ---
    engine::core::Context& get_context() const { return context_; }
    void set_current_state(std::unique_ptr<engine::ui::state::UIState> state);      ///< @brief 设置当前状态
    void set_next_state(std::unique_ptr<engine::ui::state::UIState> state);         ///< @brief 设置下一个状态
    engine::ui::state::UIState* get_state() const { return current_state_.get(); }  ///< @brief 获取当前状态

    void set_interactive(bool interactive) { interactive_ = interactive; }          ///< @brief 设置是否可交互
    bool is_interactive() const { return interactive_; }                            ///< @brief 获取是否可交互

    // --- 核心方法 ---
    void update(sf::Time delta, engine::core::Context& context) override;
    void render(engine::core::Context& context) override;

protected:
    engine::core::Context& context_;                                            ///< @brief 可交互元素很可能需要其他引擎组件
    std::unique_ptr<engine::ui::state::UIState> current_state_;                 ///< @brief 当前状态
    std::unique_ptr<engine::ui::state::UIState> next_state_;                    ///< @brief 下一个状态，用于处理状态切换
    std::unordered_map<entt::id_type, sf::Sprite> sprites_;                     ///< @brief 精灵集合，键为名称ID
    std::unordered_map<entt::id_type, entt::id_type> sounds_;                   ///< @brief 音效集合，key为音效名称ID，value为音效ID（路径哈希）
    entt::id_type current_sprite_id_;                                           ///< @brief 当前显示的精灵ID
    bool interactive_ = true;                                                   ///< @brief 是否可交互
};
} // namespace engine::ui