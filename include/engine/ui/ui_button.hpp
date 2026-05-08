#pragma once
#include "engine/ui/ui_interactive.hpp"
#include <functional>
#include <utility>

namespace engine::ui {

/**
 * @brief 按钮UI元素
 * 
 * 继承自UIInteractive，用于创建可交互的按钮。
 * 支持三种状态：正常、悬停、按下。
 * 支持回调函数，当按钮被点击时调用。
 */
class UIButton final: public UIInteractive {
private:
    std::function<void()> click_callback_;          ///< @brief 点击回调函数
    std::function<void()> hover_enter_callback_;    ///< @brief 悬停进入回调函数
    std::function<void()> hover_leave_callback_;    ///< @brief 悬停离开回调函数

public:
    /**
     * @brief 构造函数
     * @param normal_image 正常状态的图片Image类
     * @param hover_image 悬停状态的图片Image类
     * @param pressed_image 按下状态的图片Image类
     * @param position 位置
     * @param size 大小
     * @param click_callback 点击回调函数
     * @param hover_enter_callback 悬停进入回调函数
     * @param hover_leave_callback 悬停离开回调函数
     */
    UIButton(engine::core::Context& context,
             sf::Sprite normal_image,
             sf::Sprite hover_image,
             sf::Sprite pressed_image,
             sf::Vector2f position = {0.f, 0.f}, 
             sf::Vector2f size = {0.f, 0.f}, 
             std::function<void()> click_callback = nullptr,
             std::function<void()> hover_enter_callback = nullptr,
             std::function<void()> hover_leave_callback = nullptr);
    ~UIButton() override = default;

    // --- 重写基类方法：点击、悬停进入、悬停离开 ---
    void clicked() override { if (click_callback_) click_callback_(); }
    void hover_enter() override { if (hover_enter_callback_) hover_enter_callback_(); }
    void hover_leave() override { if (hover_leave_callback_) hover_leave_callback_(); }

    // --- Setters & Getters ---
    /// @brief 设置点击回调函数
    void set_click_callback(std::function<void()> callback) { click_callback_ = std::move(callback); }   
    /// @brief 设置悬停进入回调函数
    void set_hover_enter_callback(std::function<void()> callback) { hover_enter_callback_ = std::move(callback); }  
    /// @brief 设置悬停离开回调函数
    void set_hover_leave_callback(std::function<void()> callback) { hover_leave_callback_ = std::move(callback); }

    std::function<void()> get_click_callback() const { return click_callback_; }               ///< @brief 获取点击回调函数
    std::function<void()> get_hover_enter_callback() const { return hover_enter_callback_; }   ///< @brief 获取悬停进入回调函数
    std::function<void()> get_hover_leave_callback() const { return hover_leave_callback_; }   ///< @brief 获取悬停离开回调函数

};

} // namespace engine::ui