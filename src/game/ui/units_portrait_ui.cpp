#include "game/ui/units_portrait_ui.hpp"
#include "game/data/ui_config.hpp"
#include "game/data/session_data.hpp"
#include "game/data/game_stats.hpp"
#include "game/factory/blueprint_manager.hpp"
#include "engine/core/context.hpp"
#include "engine/core/game_state.hpp"
#include "engine/ui/ui_element.hpp"
#include "engine/ui/ui_panel.hpp"
#include "engine/ui/ui_image.hpp"
#include "engine/ui/ui_button.hpp"
#include "engine/ui/ui_label.hpp"
#include "engine/ui/ui_manager.hpp"
#include "engine/utils/math.hpp"
#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::ui {

UnitsPortraitUI::UnitsPortraitUI(entt::registry& registry, 
    engine::ui::UIManager& ui_manager, 
    engine::core::Context& context)
    : registry_{registry}
    , ui_manager_{ui_manager}
    , context_{context} {
    // 构造函数中直接初始化（创建单位肖像UI），可省去init函数
    create_units_portrait_ui();
    // 注册事件
    context_.get_dispatcher().sink<game::defs::RemoveUIPortraitEvent>().connect<&UnitsPortraitUI::on_remove_ui_portrait_event>(this);
    spdlog::trace("UnitsPortraitUI 构造完成。");
}

UnitsPortraitUI::~UnitsPortraitUI() {
    context_.get_dispatcher().sink<game::defs::RemoveUIPortraitEvent>().disconnect<&UnitsPortraitUI::on_remove_ui_portrait_event>(this);
}

void UnitsPortraitUI::update(sf::Time) {
    update_portrait_cover();
}

void UnitsPortraitUI::update_portrait_cover() {
    // 获取game_stats
    auto& game_stats = registry_.ctx().get<game::data::GameStats&>();
    // 获取anchor_panel中的所有子元素(frame_panel)
    auto& frame_panels = anchor_panel_->get_children();
    for (auto& frame_panel : frame_panels) {
        // 获取frame_panel中的cover_panel
        auto cover_panel = frame_panel->get_child_by_id("cover_panel"_hs);
        // 设置cover_panel的可见性（frame_panel的order_index_已设为出击cost耗费值）
        if (cover_panel) {
            cover_panel->set_visible(game_stats.cost_ < frame_panel->get_order_index());
        }
    }
}

void UnitsPortraitUI::create_units_portrait_ui() {
    // 获取ui_config、session_data、blueprint_manager上下文数据
    auto ui_config = registry_.ctx().get<std::shared_ptr<game::data::UIConfig>>();
    auto session_data = registry_.ctx().get<std::shared_ptr<game::data::SessionData>>();
    auto blueprint_manager = registry_.ctx().get<std::shared_ptr<game::factory::BlueprintManager>>();

    // 获取单位面板的间隔、角色map、角色数量
    auto padding = ui_config->get_unit_panel_padding();
    auto& unit_map = session_data->get_unit_map();
    auto unit_num = unit_map.size();

    // --- 在屏幕下方创建一个panel UI 条，用于显示角色肖像 ---
    // 获取窗口大小和角色肖像框大小
    auto window_size = context_.get_game_state().get_logical_size();
    auto frame_size = ui_config->get_unit_panel_frame_size();
    // 根据角色数量、角色肖像框大小、间隔计算panel的位置和大小
    auto pos = sf::Vector2f(0.f, window_size.y - frame_size.y - 2.f * padding);
    auto size = sf::Vector2f(unit_num * frame_size.x + (unit_num + 1u) * padding, frame_size.y + 2.f * padding);
    auto anchor_panel = std::make_unique<engine::ui::UIPanel>(pos, size);
    // 设置背景色
    anchor_panel->set_background_color(sf::Color(26, 26, 26, 26));
    // 设置ID，以后即可根据ID找到该panel
    anchor_panel->set_id("anchor_panel"_hs);

    // 依次添加角色肖像，每个肖像显示由四部分依次叠加：portrait，frame，icon，cost，可以通过一个frame_panel定位（位于上层anchor_panel之中）
    int index = 0;
    for (auto& [name_id, unit_data] : unit_map) {
        auto portrait = ui_config->get_portrait(name_id);
        auto frame = ui_config->get_portrait_frame(unit_data.rarity_);
        auto icon = ui_config->get_icon(unit_data.class_id_);
        auto cost = blueprint_manager->get_player_class_blueprint(unit_data.class_id_).player_.cost_;
        cost = static_cast<int>(std::round(engine::utils::stat_modify(cost, 1, unit_data.rarity_))); // 只有稀有度对cost有影响

        // 创建每个肖像的 frame_panel
        auto frame_pos = sf::Vector2f(padding + index * (frame_size.x + padding), padding);
        auto frame_panel = std::make_unique<engine::ui::UIPanel>(frame_pos, frame_size);
        frame_panel->set_id(name_id);

        // 依次添加四个元素，为了能够交互，将frame设置为按钮，并绑定点击事件
        frame_panel->add_child(std::make_unique<engine::ui::UIImage>(portrait, sf::Vector2f(0.f, 0.f), frame_size));
        frame_panel->add_child(std::make_unique<engine::ui::UIButton>(context_, 
            frame, 
            frame, 
            frame, 
            sf::Vector2f(0.f, 0.f), 
            frame_size,
            [this, name_id, &unit_data, cost]() {   // 按钮点击回调：发送单位准备事件
                context_.get_dispatcher().enqueue(game::defs::PrepUnitEvent{name_id, unit_data.class_id_, cost}); 
            }
            // TODO: 悬浮进入和悬浮离开回调函数
        ));
        frame_panel->add_child(std::make_unique<engine::ui::UIImage>(icon, sf::Vector2f(0.f, 0.f), frame_size / 2.f));
        frame_panel->add_child(std::make_unique<engine::ui::UILabel>(context_, 
            std::to_string(cost), 
            ui_config->get_unit_panel_font_path(), 
            ui_config->get_unit_panel_font_size(), 
            sf::Color::Yellow, 
            ui_config->get_unit_panel_font_offset()
        ));
        // 最后添加一个灰色的遮盖panel，cost不足以支持该角色出击时显示
        auto cover_panel = std::make_unique<engine::ui::UIPanel>(sf::Vector2f(0.f, 0.f), frame_size);
        cover_panel->set_background_color(sf::Color(0, 0, 0, 100));
        cover_panel->set_id("cover_panel"_hs);
        frame_panel->add_child(std::move(cover_panel));

        // 将frame_panel添加到anchor_panel中，并使用cost作为排序键
        anchor_panel->add_child(std::move(frame_panel), cost);
        index++;
    }
    // 将anchor_panel添加到ui_manager中
    ui_manager_.add_element(std::move(anchor_panel));
    
    // 移动赋值之后需要找到anchor_panel，并将指针赋值给成员变量anchor_panel_
    anchor_panel_ = static_cast<engine::ui::UIPanel*>(ui_manager_.get_root_element()->get_child_by_id("anchor_panel"_hs));
    
    anchor_panel_->sort_children_by_order_index();  // 对anchor_panel中的子元素(frame_panel)进行排序
    arrange_units_portrait_ui();                    // 按顺序排列anchor_panel中的子元素(frame_panel)的位置 
}

void UnitsPortraitUI::arrange_units_portrait_ui() {
    // 获取ui_config
    auto ui_config = registry_.ctx().get<std::shared_ptr<game::data::UIConfig>>();
    // 获取单位面板的间隔、大小
    auto padding = ui_config->get_unit_panel_padding();
    auto frame_size = ui_config->get_unit_panel_frame_size();
    // 遍历panel中的子元素(定位panel)，并依次设定位置
    for (size_t i = 0; i < anchor_panel_->get_children().size(); i++) {
        auto& child = anchor_panel_->get_children()[i];
        child->set_position(sf::Vector2f(padding + i * (frame_size.x + padding), padding));
    }
    // 更新panel的size
    anchor_panel_->set_size(sf::Vector2f(padding + anchor_panel_->get_children().size() * (frame_size.x + padding), 
                                    frame_size.y + 2 * padding));
}

void UnitsPortraitUI::on_remove_ui_portrait_event(const game::defs::RemoveUIPortraitEvent& event) {
    anchor_panel_->remove_child_by_id(event.name_id_);
    arrange_units_portrait_ui();
}

} // namespace game::ui