#include "game/system/debug_ui_system.hpp"
#include "game/component/stats_component.hpp"
#include "game/component/class_name_component.hpp"
#include "game/component/blocker_component.hpp"
#include "game/component/skill_component.hpp"
#include "game/component/player_component.hpp"
#include "game/defs/tags.hpp"
#include "game/defs/events.hpp"
#include "game/data/game_stats.hpp"
#include "game/data/level_data.hpp"
#include "game/data/session_data.hpp"
#include "game/factory/blueprint_manager.hpp"
#include "game/scene/title_scene.hpp"
#include "engine/core/game_state.hpp"
#include "engine/audio/audio_player.hpp"
#include "engine/component/name_component.hpp"
#include "engine/core/context.hpp"
#include "engine/core/time.hpp"
#include "engine/resource/resource_manager.hpp"
#include "engine/utils/math.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::system {

DebugUISystem::DebugUISystem(entt::registry& registry, engine::core::Context& context)
    : registry_{registry}
    , context_{context} {
    context_.get_dispatcher().sink<game::defs::UIPortraitHoverEnterEvent>().connect<&DebugUISystem::on_ui_portrait_hover_enter_event>(this);
    context_.get_dispatcher().sink<game::defs::UIPortraitHoverLeaveEvent>().connect<&DebugUISystem::on_ui_portrait_hover_leave_event>(this);
}

DebugUISystem::~DebugUISystem() {
    context_.get_dispatcher().disconnect(this);
}

void DebugUISystem::update() {
    render_hovered_portrait();
    render_hovered_unit();
    render_selected_unit();
    render_info_ui();
    render_setting_ui();
    render_debug_ui();

    // 渲染可能激活的保存面板
    auto& show_save_panel = registry_.ctx().get<bool&>("show_save_panel"_hs);
    render_save_panel_ui(show_save_panel);
}
void DebugUISystem::update_title(game::scene::TitleScene& title_scene) {
    render_title_logo();
    render_title_buttons(title_scene);
    // 渲染可能激活的角色信息和载入面板
    render_unit_info_ui(title_scene.show_unit_info_);      // 可以直接获取TitleScene的私有成员变量
    render_load_panel_ui(title_scene.show_load_panel_);
}

// ----------------------------- GameScene -----------------------------
void DebugUISystem::render_hovered_portrait() {
    // 确定鼠标悬浮的单位肖像存在
    if (hovered_portrait_ == entt::null) return;

    // 角色名称不是一个实体，需要从蓝图中获取数据
    const auto& session_data = registry_.ctx().get<std::shared_ptr<game::data::SessionData>>();
    const auto& blueprint_mgr = registry_.ctx().get<std::shared_ptr<game::factory::BlueprintManager>>();
    const auto& unit_data = session_data->get_unit_data(hovered_portrait_);
    const auto& class_blueprint = blueprint_mgr->get_player_class_blueprint(unit_data.class_id_);
    const auto& stats = class_blueprint.stats_;
    // 计算等级和稀有度对属性的影响
    const auto hp = engine::utils::stat_modify(stats.hp_, unit_data.level_, unit_data.rarity_);
    const auto atk = engine::utils::stat_modify(stats.atk_, unit_data.level_, unit_data.rarity_);
    const auto def = engine::utils::stat_modify(stats.def_, unit_data.level_, unit_data.rarity_);
    const auto range = stats.range_;
    std::string_view name = class_blueprint.display_info_.name_;
    // std::string_view description = class_blueprint.display_info_.description_;

    // 显示Tooltip信息
    if (!ImGui::BeginTooltip()) {
        ImGui::EndTooltip();
        spdlog::error("鼠标悬浮单位肖像窗口打开失败");
        return;
    }
    ImGui::Text("%s", unit_data.name_.c_str());
    ImGui::SameLine();
    ImGui::Text("职业: %s", name.data());
    ImGui::Text("等级: %d", unit_data.level_);
    ImGui::SameLine();
    ImGui::Text("稀有度: %d", unit_data.rarity_);
    ImGui::Text("生命值: %d", static_cast<int>(std::round(hp)));
    ImGui::SameLine();
    ImGui::Text("攻击力: %d", static_cast<int>(std::round(atk)));
    ImGui::Text("防御力: %d", static_cast<int>(std::round(def)));
    ImGui::SameLine();
    ImGui::Text("攻击范围: %d", static_cast<int>(std::round(range)));
    ImGui::EndTooltip();
}

void DebugUISystem::render_hovered_unit() {
    // 确定鼠标悬浮的单位存在
    auto& entity = registry_.ctx().get<entt::entity&>("hovered_unit"_hs);
    if (entity == entt::null || !registry_.valid(entity)) return;

    // Tooltip 是悬浮在鼠标上的小窗口，可以显示单位信息
    if (!ImGui::BeginTooltip()) {
        ImGui::EndTooltip();
        spdlog::error("鼠标悬浮单位窗口打开失败");
        return;
    }
    // 获取必要信息并显示
    const auto& stats = registry_.get<game::component::StatsComponent>(entity);
    const auto& class_name = registry_.get<game::component::ClassNameComponent>(entity);
    // 只有玩家单位才有姓名，所以需要尝试获取
    if (auto name = registry_.try_get<engine::component::NameComponent>(entity); name) {
        ImGui::Text("%s  ", name->name_.c_str());
        ImGui::SameLine();
    }
    ImGui::Text("%s", class_name.class_name_.c_str());
    ImGui::Text("等级: %d", stats.level_);
    ImGui::SameLine();
    ImGui::Text("稀有度: %d", stats.rarity_);
    ImGui::Text("生命值: %d/%d", static_cast<int>(std::round(stats.hp_)), static_cast<int>(std::round(stats.max_hp_)));
    ImGui::Text("攻击力: %d", static_cast<int>(std::round(stats.atk_)));
    ImGui::Text("防御力: %d", static_cast<int>(std::round(stats.def_)));
    ImGui::Text("攻击范围: %d", static_cast<int>(std::round(stats.range_)));
    ImGui::Text("攻击间隔: %.2f", stats.atk_interval_.asSeconds());
    ImGui::EndTooltip();
}

void DebugUISystem::render_selected_unit() {
    // 确定选中的单位存在
    auto& entity = registry_.ctx().get<entt::entity&>("selected_unit"_hs);
    if (entity == entt::null || !registry_.valid(entity)) return;

    // 设置窗口位置在左上角
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);

    if (!ImGui::Begin("角色状态", nullptr, ImGuiWindowFlags_NoTitleBar)) {
        ImGui::End();
        spdlog::error("角色状态窗口打开失败");
        return;
    }
    // 获取必要信息并显示
    const auto& stats = registry_.get<game::component::StatsComponent>(entity);
    const auto& class_name = registry_.get<game::component::ClassNameComponent>(entity);
    const auto blocker = registry_.try_get<game::component::BlockerComponent>(entity);
    if (auto name = registry_.try_get<engine::component::NameComponent>(entity); name) {
        ImGui::Text("%s  ", name->name_.c_str());
        ImGui::SameLine();
    }
    ImGui::Text("%s", class_name.class_name_.c_str());
    ImGui::Text("等级: %d", stats.level_);
    ImGui::SameLine();
    ImGui::Text("稀有度: %d", stats.rarity_);
    ImGui::Text("生命值: %d/%d", static_cast<int>(std::round(stats.hp_)), static_cast<int>(std::round(stats.max_hp_)));
    ImGui::Text("攻击力: %d", static_cast<int>(std::round(stats.atk_)));
    ImGui::SameLine();
    ImGui::Text("防御力: %d", static_cast<int>(std::round(stats.def_)));
    ImGui::Text("攻击范围: %d", static_cast<int>(std::round(stats.range_)));
    ImGui::SameLine();
    ImGui::Text("攻击间隔: %.2f", stats.atk_interval_.asSeconds());
    if (blocker) {
        ImGui::Text("阻挡数量: %d/%d", blocker->current_count_, blocker->max_count_);
    }

    // 升级，消耗COST与出击COST相同
    const auto& player = registry_.get<game::component::PlayerComponent>(entity);
    auto available_cost = registry_.ctx().get<game::data::GameStats&>().cost_;
    bool button_available = available_cost >= player.cost_;
    // COST资源充足时升级按钮才可用
    ImGui::BeginDisabled(!button_available);
    // 设置快捷键 U 升级
    ImGui::SetNextItemShortcut(ImGuiKey_U, ImGuiInputFlags_RouteAlways | ImGuiInputFlags_Tooltip);
    if (ImGui::Button("升级")) {
        context_.get_dispatcher().enqueue<game::defs::UpgradeUnitEvent>(entity, player.cost_);
    }
    ImGui::SameLine();
    ImGui::Text("快捷键 U: COST消费: %d", player.cost_);
    ImGui::EndDisabled();

    // 撤退，返回 50% 的COST
    auto return_cost = static_cast<int>(player.cost_ * 0.5f);
    // 设置快捷键 R 撤退
    ImGui::SetNextItemShortcut(ImGuiKey_R, ImGuiInputFlags_RouteAlways | ImGuiInputFlags_Tooltip);
    if (ImGui::Button("撤退")) {
        context_.get_dispatcher().enqueue<game::defs::RetreatEvent>(entity, return_cost);
    }
    ImGui::SameLine();
    ImGui::Text("快捷键 R: COST返还: %d", return_cost);

    // 技能显示与交互
    if (auto skill = registry_.try_get<game::component::SkillComponent>(entity); skill) {
        // 如果技能准备就绪，则按钮可用（激活技能），否则按钮不可用
        auto ready = registry_.all_of<game::defs::SkillReadyTag>(entity);
        ImGui::BeginDisabled(!ready);
        // 设置快捷键 S 激活技能
        ImGui::SetNextItemShortcut(ImGuiKey_S, ImGuiInputFlags_RouteAlways | ImGuiInputFlags_Tooltip);
        if (ImGui::Button(skill->name_.c_str())) {
            // 激活技能
            context_.get_dispatcher().enqueue<game::defs::SkillActiveEvent>(entity);
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        // 如果技能激活中，则显示"剩余时间"或"被动技能激活中"
        if (registry_.all_of<game::defs::SkillActiveTag>(entity)) {
            if (registry_.all_of<game::defs::PassiveSkillTag>(entity)) {
                ImGui::Text("被动技能激活中");
            } else {
                ImGui::Text("激活中，剩余时间: %.1f 秒", (skill->duration_ - skill->duration_timer_).asSeconds());
            }
        // 否则显示冷却时间
        } else {
            ImGui::Text("快捷键 S: ");
            ImGui::SameLine();
            if (registry_.all_of<game::defs::SkillReadyTag>(entity)) {
                ImGui::Text("技能准备就绪");
            } else {
                // 用进度条显示冷却时间百分比
                ImGui::ProgressBar(skill->cooldown_timer_ / skill->cooldown_);
            }
        }
        // 显示技能描述
        ImGui::TextWrapped("%s", skill->description_.c_str());
    }
    ImGui::End();
}

void DebugUISystem::render_info_ui() {
    if (!ImGui::Begin("关卡信息", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();
        spdlog::error("关卡信息窗口打开失败");
        return;
    }
    // 获取关卡相关数据
    const auto& game_stats = registry_.ctx().get<game::data::GameStats&>();
    const auto& waves = registry_.ctx().get<game::data::Waves&>();
    const auto& session_data = registry_.ctx().get<std::shared_ptr<game::data::SessionData>>();
    // 显示
    ImGui::Text("基地血量: %d / 5", game_stats.home_hp_);
    ImGui::SameLine();
    ImGui::Text("COST: %d", static_cast<int>(game_stats.cost_));
    ImGui::SameLine();
    ImGui::Text("剩余波次: %ld", waves.waves_.size());
    ImGui::SameLine();
    if (waves.waves_.size() > 0) {
        ImGui::Text("下一波时间: %d", static_cast<int>(waves.next_wave_count_down_.asSeconds()));
    }
    ImGui::SameLine();
    ImGui::Text("击杀数量: %d / %d", game_stats.enemy_killed_count_, game_stats.enemy_count_);
    ImGui::SameLine();
    ImGui::Text("当前关卡: %d", session_data->get_level_number());
    ImGui::End();
}

void DebugUISystem::render_setting_ui() {
    if (!ImGui::Begin("设置工具", nullptr, ImGuiWindowFlags_NoTitleBar)) {
        ImGui::End();
        spdlog::error("设置工具窗口打开失败");
        return;
    }
    // 场景控制
    auto& game_state = context_.get_game_state();
    ImGui::SetNextItemShortcut(ImGuiKey_P, ImGuiInputFlags_RouteAlways | ImGuiInputFlags_Tooltip);
    if (game_state.is_paused()) {    // 如果游戏暂停，则显示“继续游戏”按钮，快捷键 P
        if (ImGui::Button("继续游戏")) {
            game_state.set_state(engine::core::State::Playing);
        }
    } else {        // 如果游戏运行中，则显示“暂停游戏”按钮，快捷键也是 P
        if (ImGui::Button("暂停游戏")) {
            game_state.set_state(engine::core::State::Paused);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("重新开始")) {
        context_.get_dispatcher().enqueue<game::defs::RestartEvent>();
    }
    if (ImGui::Button("返回标题")) {
        context_.get_dispatcher().enqueue<game::defs::BackToTitleEvent>();
    }
    ImGui::SameLine();
    if (ImGui::Button("保存")) {
        context_.get_dispatcher().enqueue<game::defs::SaveEvent>();
    }
    ImGui::Separator();

    // 游戏速度调节
    auto& time = context_.get_time();
    float time_scale = time.get_time_scale();
    if (ImGui::Button("0.5倍速")) {
        time_scale = 0.5f;
        time.set_time_scale(time_scale);
    }
    ImGui::SameLine();
    if (ImGui::Button("1倍速")) {
        time_scale = 1.f;
        time.set_time_scale(time_scale);
    }
    ImGui::SameLine();
    if (ImGui::Button("2倍速")) {
        time_scale = 2.f;
        time.set_time_scale(time_scale);
    }
    ImGui::SliderFloat("游戏速度", &time_scale, 0.5f, 2.f);
    time.set_time_scale(time_scale);

    // 音乐音量调节
    int music_volume = context_.get_audio_player().get_music_volume();
    ImGui::SliderInt("音乐音量", &music_volume, 0u, 100u);
    context_.get_audio_player().set_music_volume(music_volume);
    int sound_volume = context_.get_audio_player().get_sound_volume();
    ImGui::SliderInt("音效音量", &sound_volume, 0u, 100u);
    context_.get_audio_player().set_sound_volume(sound_volume);

    // 切换调试工具显示 （勾选结果保存在show_debug_ui_中）
    ImGui::Checkbox("显示调试工具", &show_debug_ui_);
    ImGui::End();
}

void DebugUISystem::render_debug_ui() {
    if (!show_debug_ui_) return;
    if (!ImGui::Begin("调试工具", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();
        spdlog::error("调试工具窗口打开失败");
        return;
    }
    auto& game_stats = registry_.ctx().get<game::data::GameStats&>();
    if (ImGui::Button("COST + 10")) {
        game_stats.cost_ += 10;
    }
    if (ImGui::Button("COST + 100")) {
        game_stats.cost_ += 100;
    }
    if (ImGui::Button("通关")) {
        context_.get_dispatcher().enqueue<game::defs::LevelClearEvent>();
    }
    // TODO: 未来可按需添加其他调试工具
    ImGui::End();
}

// ----------------------------- TitleScene -----------------------------
void DebugUISystem::render_title_logo() {
    if (!ImGui::Begin("TitleLogo", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground)) {
        ImGui::End();
        spdlog::error("TitleLogo窗口打开失败");
        return;
    }
    // 获取LOGO图片信息
    auto& resource_manager = context_.get_resource_manager();
    auto logo_texture = resource_manager.get_texture("assets/textures/UI/title.png"_hs);
    ImGui::Image(*logo_texture);
    ImGui::End();
}

void DebugUISystem::render_title_buttons(game::scene::TitleScene& title_scene) {
    if (!ImGui::Begin("TitleUI", nullptr, ImGuiWindowFlags_NoTitleBar)) {
        ImGui::End();
        spdlog::error("TitleUI窗口打开失败");
        return;
    }
    // 设置按钮字体更大
    ImGui::SetWindowFontScale(2.f);
    if (ImGui::Button("开始游戏", ImVec2(200, 60))) {
        title_scene.on_start_game_click();     // 直接调用TitleScene的私有函数，不需要通过dispatcher发信号
    }
    ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);
    if (ImGui::Button("确认角色", ImVec2(200, 60))) {
        title_scene.on_confirm_role_click();
    }
    ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);
    if (ImGui::Button("载入游戏", ImVec2(200, 60))) {
        title_scene.on_load_game_click();
    }
    ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);
    if (ImGui::Button("退出游戏", ImVec2(200, 60))) {
        title_scene.on_quit_click();
    }
    ImGui::SetWindowFontScale(1.f);    // 恢复默认字体大小
    ImGui::End();
}

// ----------------------------- Shared -----------------------------
void DebugUISystem::render_unit_info_ui(bool& show_unit_info) {
    if (!show_unit_info) return;
    // 关闭窗口时，第二个参数(show_unit_info)会被设置为false，因此需要传入引用
    if (!ImGui::Begin("角色信息", &show_unit_info, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        spdlog::error("UnitInfoUI窗口打开失败");
        return;
    }
    render_unit_table();
    ImGui::Separator();
    const auto session_data = registry_.ctx().get<std::shared_ptr<game::data::SessionData>>();
    ImGui::Text("剩余点数: %d", session_data->get_point());
    ImGui::End();
}

void DebugUISystem::render_load_panel_ui(bool& show_load_panel) {
    if (!show_load_panel) return;
    if (!ImGui::Begin("读档选择", &show_load_panel, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        spdlog::error("读档选择窗口打开失败");
        return;
    }
    const auto& session_data = registry_.ctx().get<std::shared_ptr<game::data::SessionData>>();
    if (ImGui::Button("SLOT 1")) {
        session_data->load_from_file("assets/save/SLOT_1.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("SLOT 2")) {
        session_data->load_from_file("assets/save/SLOT_2.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("SLOT 3")) {
        session_data->load_from_file("assets/save/SLOT_3.json");
    }
    // 如果已经通关了，则提示将进入下一关，否则显示“当前关卡”
    if (session_data->is_level_clear()) {
        ImGui::Text("下一关: %d", session_data->get_level_number() + 1);
    } else {
        ImGui::Text("当前关卡: %d", session_data->get_level_number());
    }
    ImGui::End();
}

void DebugUISystem::render_save_panel_ui(bool& show_save_panel) {
    if (!show_save_panel) return;
    if (!ImGui::Begin("存档选择", &show_save_panel, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        spdlog::error("存档选择窗口打开失败");
        return;
    }
    const auto& session_data = registry_.ctx().get<std::shared_ptr<game::data::SessionData>>();
    if (ImGui::Button("SLOT 1")) {        
        session_data->save_to_file("assets/save/SLOT_1.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("SLOT 2")) {
        session_data->save_to_file("assets/save/SLOT_2.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("SLOT 3")) {
        session_data->save_to_file("assets/save/SLOT_3.json");
    }
    // 根据是否已经通关，切换显示提示信息
    if (session_data->is_level_clear()) {
        ImGui::Text("下一关: %d", session_data->get_level_number() + 1);
    } else {
        ImGui::Text("当前关卡: %d", session_data->get_level_number());
    }
    ImGui::End();
}

void DebugUISystem::render_unit_table() {
    // 显示表格，需指定列数(14)，标志位使用ImGuiTableFlags_Sortable，可以让表格支持排序
    if (!ImGui::BeginTable("角色信息", 14, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Sortable)) {
        ImGui::End();
        spdlog::error("角色信息表格打开失败");
        return;
    }
    // 定义标题列
    ImGui::TableSetupColumn("姓名");
    ImGui::TableSetupColumn("职业");
    ImGui::TableSetupColumn("类型");
    ImGui::TableSetupColumn("等级");
    ImGui::TableSetupColumn("稀有度");
    ImGui::TableSetupColumn("COST");
    ImGui::TableSetupColumn("生命值");
    ImGui::TableSetupColumn("攻击力");
    ImGui::TableSetupColumn("防御力");
    ImGui::TableSetupColumn("攻击范围");
    ImGui::TableSetupColumn("攻击间隔");
    ImGui::TableSetupColumn("阻挡数量");
    ImGui::TableSetupColumn("技能");
    ImGui::TableSetupColumn("升级");
    // 渲染标题行
    ImGui::TableHeadersRow();
    // 获取数据
    const auto session_data = registry_.ctx().get<std::shared_ptr<game::data::SessionData>>();
    auto& unit_data_list = session_data->get_unit_data_list();
    const auto blueprint_manager = registry_.ctx().get<std::shared_ptr<game::factory::BlueprintManager>>();
    const auto ui_config = registry_.ctx().get<std::shared_ptr<game::data::UIConfig>>();

    // --- 点击标题列，就按照该列排序 ---
    // 获取排序规格参数 (当sort_specs->SpecsDirty为true时，表示点击了某一列标题，需要重新排序)
    if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs()) {
        if (sort_specs->SpecsDirty && !unit_data_list.empty()) {
            // 获取第一个（也是唯一的）排序规格参数 (多列排序会有更多规格参数)
            const ImGuiTableColumnSortSpecs& spec = sort_specs->Specs[0];
            const int col = spec.ColumnIndex;       // 获取列的索引，对应鼠标点击的列
            const bool ascending = (spec.SortDirection == ImGuiSortDirection_Ascending);
            
            // 创建字符串比较函数 (如果左<右，返回-1；如果左>右，返回1；如果左==右，返回0)
            const auto compareStrings = [](const std::string& a, const std::string& b) {
                return (a < b) ? -1 : (a > b ? 1 : 0);
            };
            
            // 执行排序
            std::stable_sort(unit_data_list.begin(), unit_data_list.end(), 
                             [&](const game::data::UnitData* lhs, const game::data::UnitData* rhs) {
                // 获取职业蓝图（用于获取属性）
                const auto& pcb_l = blueprint_manager->get_player_class_blueprint(lhs->class_id_);
                const auto& pcb_r = blueprint_manager->get_player_class_blueprint(rhs->class_id_);
                
                // delta用于记录比较结果（-1表示小于，0表示等于，1表示大于）
                int delta = 0;

                switch (col) {
                    case 0: {   // 姓名
                        delta = compareStrings(lhs->name_, rhs->name_);
                        break;
                    }
                    case 1: {   // 职业
                        delta = compareStrings(lhs->class_, rhs->class_);
                        break;
                    }
                    case 2: {   // 类型（近战、远程、混合）
                        const int type_l = static_cast<int>(pcb_l.player_.type_);
                        const int type_r = static_cast<int>(pcb_r.player_.type_);
                        delta = (type_l < type_r) ? -1 : (type_l > type_r ? 1 : 0);
                        break;
                    }
                    case 3: {   // 等级
                        delta = (lhs->level_ < rhs->level_) ? -1 : (lhs->level_ > rhs->level_ ? 1 : 0);
                        break;
                    }
                    case 4: {   // 稀有度
                        delta = (lhs->rarity_ < rhs->rarity_) ? -1 : (lhs->rarity_ > rhs->rarity_ ? 1 : 0);
                        break;
                    }
                    case 5: {   // COST
                        // 要考虑相等的情况，因此使用int排序而非float
                        const int cost_l = static_cast<int>(std::round(engine::utils::stat_modify(pcb_l.player_.cost_, 1, lhs->rarity_)));
                        const int cost_r = static_cast<int>(std::round(engine::utils::stat_modify(pcb_r.player_.cost_, 1, rhs->rarity_)));
                        delta = (cost_l < cost_r) ? -1 : (cost_l > cost_r ? 1 : 0);
                        break;
                    }
                    case 6: {   // 生命值
                        const int hp_l = static_cast<int>(std::round(engine::utils::stat_modify(pcb_l.stats_.hp_, lhs->level_, lhs->rarity_)));
                        const int hp_r = static_cast<int>(std::round(engine::utils::stat_modify(pcb_r.stats_.hp_, rhs->level_, rhs->rarity_)));
                        delta = (hp_l < hp_r) ? -1 : (hp_l > hp_r ? 1 : 0);
                        break;
                    }
                    case 7: {   // 攻击力
                        const int atk_l = static_cast<int>(std::round(engine::utils::stat_modify(pcb_l.stats_.atk_, lhs->level_, lhs->rarity_)));
                        const int atk_r = static_cast<int>(std::round(engine::utils::stat_modify(pcb_r.stats_.atk_, rhs->level_, rhs->rarity_)));
                        delta = (atk_l < atk_r) ? -1 : (atk_l > atk_r ? 1 : 0);
                        break;
                    }
                    case 8: {   // 防御力
                        const int def_l = static_cast<int>(std::round(engine::utils::stat_modify(pcb_l.stats_.def_, lhs->level_, lhs->rarity_)));
                        const int def_r = static_cast<int>(std::round(engine::utils::stat_modify(pcb_r.stats_.def_, rhs->level_, rhs->rarity_)));
                        delta = (def_l < def_r) ? -1 : (def_l > def_r ? 1 : 0);
                        break;
                    }
                    case 9: {   // 攻击范围
                        const int range_l = static_cast<int>(std::round(pcb_l.stats_.range_));
                        const int range_r = static_cast<int>(std::round(pcb_r.stats_.range_));
                        delta = (range_l < range_r) ? -1 : (range_l > range_r ? 1 : 0);
                        break;
                    }
                    case 10: {  // 攻击间隔
                        const float ai_l = pcb_l.stats_.atk_interval_.asSeconds();
                        const float ai_r = pcb_r.stats_.atk_interval_.asSeconds();
                        delta = (ai_l < ai_r) ? -1 : (ai_l > ai_r ? 1 : 0);
                        break;
                    }
                    case 11: {  // 阻挡数量
                        delta = (pcb_l.player_.block_ < pcb_r.player_.block_) ? -1 : (pcb_l.player_.block_ > pcb_r.player_.block_ ? 1 : 0);
                        break;
                    }
                    case 12: {  // 技能
                        const auto& sk_l = blueprint_manager->get_skill_blueprint(pcb_l.player_.skill_id_);
                        const auto& sk_r = blueprint_manager->get_skill_blueprint(pcb_r.player_.skill_id_);
                        delta = compareStrings(sk_l.name_, sk_r.name_);
                        break;
                    }
                    case 13: {  // 升级按钮 (和COST排序一致)
                        const int cost_l = static_cast<int>(std::round(engine::utils::stat_modify(pcb_l.player_.cost_, 1, lhs->rarity_)));
                        const int cost_r = static_cast<int>(std::round(engine::utils::stat_modify(pcb_r.player_.cost_, 1, rhs->rarity_)));
                        delta = (cost_l < cost_r) ? -1 : (cost_l > cost_r ? 1 : 0);
                        break;
                    }
                    default: break;
                }

                // 根据升序还是降序返回比较结果
                return ascending ? (delta < 0) : (delta > 0);
            });
            
            // 完成排序后，将SpecsDirty设置为false，下轮更新会跳过排序操作（即脏标识模式）
            sort_specs->SpecsDirty = false;
        }
    }

    // 渲染数据行
    for (const auto& unit : unit_data_list) {
        // 获取并计算属性数据信息
        const auto& player_class_blueprint = blueprint_manager->get_player_class_blueprint(unit->class_id_);
        const auto& skill_blueprint = blueprint_manager->get_skill_blueprint(player_class_blueprint.player_.skill_id_);
        const auto& stats = player_class_blueprint.stats_;
        const auto hp = engine::utils::stat_modify(stats.hp_, unit->level_, unit->rarity_);
        const auto atk = engine::utils::stat_modify(stats.atk_, unit->level_, unit->rarity_);
        const auto def = engine::utils::stat_modify(stats.def_, unit->level_, unit->rarity_);
        const auto cost = engine::utils::stat_modify(player_class_blueprint.player_.cost_, 1, unit->rarity_);
        std::string type = player_class_blueprint.player_.type_ == game::defs::PlayerType::Melee ? "近战" : 
            player_class_blueprint.player_.type_ == game::defs::PlayerType::Ranged ? "远程" : 
            player_class_blueprint.player_.type_ == game::defs::PlayerType::Mixed ? "混合" : "未知";

        // 获取头像信息
        const auto& portrait_sprite = ui_config->get_portrait(unit->name_id_);

        // 设置显示尺寸
        constexpr sf::Vector2f DISPLAY_SIZE = sf::Vector2f(128.f, 128.f);

        // 新建一行
        ImGui::TableNextRow();
        // 每一行依次填充对应列的信息
        ImGui::TableNextColumn();       // 第一列：姓名
        ImGui::Text("%s", unit->name_.c_str());
        // 如果鼠标悬浮在该UI组件上，显示复杂信息（支持各种UI组件及其组合，例如下面的Tooltip组件）
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            // sf::Sprite 已通过 setTextureRect 设置了裁剪区域，直接显示即可
            ImGui::Image(portrait_sprite, ImVec2(DISPLAY_SIZE.x, DISPLAY_SIZE.y));
            ImGui::EndTooltip();
        }
        ImGui::TableNextColumn();       // 第二列：职业
        ImGui::Text("%s", player_class_blueprint.display_info_.name_.c_str());
        // 如果鼠标悬浮在该UI组件上，显示描述信息（只支持文本）
        ImGui::SetItemTooltip("%s", player_class_blueprint.display_info_.description_.c_str());
        ImGui::TableNextColumn();       // 第三列：类型
        ImGui::Text("%s", type.c_str());
        ImGui::TableNextColumn();       // 第四列：等级
        ImGui::Text("%d", unit->level_);
        ImGui::TableNextColumn();       // 第五列：稀有度
        ImGui::Text("%d", unit->rarity_);
        ImGui::TableNextColumn();       // 第六列：COST
        ImGui::Text("%d", static_cast<int>(std::round(cost)));
        ImGui::TableNextColumn();       // 第七列：生命值
        ImGui::Text("%d", static_cast<int>(std::round(hp)));
        ImGui::TableNextColumn();        // 第八列：攻击力
        ImGui::Text("%d", static_cast<int>(std::round(atk)));
        ImGui::TableNextColumn();        // 第九列：防御力
        ImGui::Text("%d", static_cast<int>(std::round(def)));
        ImGui::TableNextColumn();        // 第十列：攻击范围
        ImGui::Text("%d", static_cast<int>(std::round(player_class_blueprint.stats_.range_)));
        ImGui::TableNextColumn();        // 第十一列：攻击间隔
        ImGui::Text("%.2f", player_class_blueprint.stats_.atk_interval_.asSeconds());
        ImGui::TableNextColumn();        // 第十二列：阻挡数量
        ImGui::Text("%d", player_class_blueprint.player_.block_);
        ImGui::TableNextColumn();        // 第十三列：技能
        ImGui::Text("%s", skill_blueprint.name_.c_str());
        ImGui::SetItemTooltip("%s", skill_blueprint.description_.c_str());
        ImGui::TableNextColumn();        // 第十四列：升级按钮

        // 使用 name_ 作为下一个UI组件(即Button)的 ID，确保唯一性，否则同名Button会冲突
        ImGui::PushID(unit->name_.c_str());
            // 根据积分点数，判断是否可以升级，并决定升级按钮是否可用
        bool can_upgrade = session_data->get_point() >= static_cast<int>(std::round(cost));
        ImGui::BeginDisabled(!can_upgrade);
        std::string button_text = "- " + std::to_string(static_cast<int>(std::round(cost)));
        if (ImGui::Button(button_text.c_str())) {   // 如果没有PushID，默认会以Button中显示参数作为ID，那会出现重复ID
            session_data->add_point(-static_cast<int>(std::round(cost)));
            unit->level_ += 1;
        }
        ImGui::EndDisabled();
        ImGui::PopID();     // 与前面的PushID对应使用，用于结束ID范围
        ImGui::SetItemTooltip("升级耗费的点数：%d", static_cast<int>(std::round(cost)));
    }
    ImGui::EndTable();
}

// 事件回调函数
void DebugUISystem::on_ui_portrait_hover_enter_event(const game::defs::UIPortraitHoverEnterEvent& event) {
    hovered_portrait_ = event.name_id_;
}
void DebugUISystem::on_ui_portrait_hover_leave_event() {
    hovered_portrait_ = entt::null;
}

} // namespace game::system