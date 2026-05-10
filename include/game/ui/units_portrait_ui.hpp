#pragma once
#include "game/defs/events.hpp"
#include <SFML/System/Time.hpp>
#include <entt/entity/fwd.hpp>

namespace engine::core {
    class Context;
} // namespace engine::core

namespace engine::ui {
    class UIPanel;
    class UIManager;
} // namespace engine::ui

namespace game::ui {

/**
 * @brief 单位肖像UI
 * 
 * 负责管理单位肖像UI的创建、更新和排列。
 */
class UnitsPortraitUI {
public:
    /**
     * @brief 构造函数
     * @param registry 注册表
     * @param ui_manager UI管理器
     * @param context 引擎上下文
     */
    UnitsPortraitUI(entt::registry& registry, 
        engine::ui::UIManager& ui_manager, 
        engine::core::Context& context);
    ~UnitsPortraitUI();

    void update(sf::Time delta);

    engine::ui::UIPanel* get_anchor_panel() const { return anchor_panel_; }

private:
    void update_portrait_cover();           ///< @brief 更新肖像遮盖
    void create_units_portrait_ui();        ///< @brief 创建单位肖像UI
    void arrange_units_portrait_ui();       ///< @brief 排列单位肖像UI（肖像增/减时调用）

    // 事件回调函数
    void on_remove_ui_portrait_event(const game::defs::RemoveUIPortraitEvent& event);

    // --- 构造函数传入的外部组件引用 ---
    entt::registry& registry_;
    engine::ui::UIManager& ui_manager_;
    engine::core::Context& context_;

    engine::ui::UIPanel* anchor_panel_;     ///< @brief 保存单位肖像UI的根面板(非拥有指针)，方便使用
};

} // namespace game::ui