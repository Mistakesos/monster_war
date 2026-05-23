#pragma once
#include "engine/scene/scene.hpp"
#include "game/system/fwd.hpp"

namespace game::scene {

class EndScene final: public engine::scene::Scene {
    friend class game::system::DebugUISystem;

public:
    // 构造函数传入是否获胜
    EndScene(engine::core::Context& context, bool is_win = false);
    ~EndScene();

    void render() override;

private:
    // 按钮回调函数
    void on_back_to_title_click();
    void on_quit_click();

    // 目前只需要DebugUI系统
    std::unique_ptr<game::system::DebugUISystem> debug_ui_system_;

    bool is_win_{false};    ///< @brief 是否获胜
};

}   // namespace game::scene