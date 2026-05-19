#include "engine/core/game.hpp"
#include "engine/core/time.hpp"
#include "engine/core/config.hpp"
#include "engine/resource/resource_manager.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/scene/scene_manager.hpp"
#include "engine/render/render.hpp"
#include "engine/render/camera.hpp"
#include "engine/audio/audio_player.hpp"
#include "engine/core/game_state.hpp"
#include "engine/core/context.hpp"
#include "engine/utils/events.hpp"
#include "entt/signal/dispatcher.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <spdlog/spdlog.h>

namespace engine::core {
Game::Game()
    : config_{std::make_unique<Config>("assets/config.json")}
    , window_{std::make_unique<sf::RenderWindow>(sf::VideoMode(config_->window_size_), config_->window_title_)}
    , dispatcher_{std::make_unique<entt::dispatcher>()}
    , time_{std::make_unique<Time>()}
    , resource_manager_{std::make_unique<engine::resource::ResourceManager>()}
    , camera_{std::make_unique<engine::render::Camera>(window_.get(), dispatcher_.get())}
    , input_manager_{std::make_unique<engine::input::InputManager>(window_.get(), config_.get(), dispatcher_.get(), camera_.get())}
    , renderer_{std::make_unique<engine::render::Renderer>(window_.get(), resource_manager_.get())}
    , audio_player_{std::make_unique<engine::audio::AudioPlayer>(resource_manager_.get())}
    , game_state_{std::make_unique<engine::core::GameState>(window_.get())}
    , context_{std::make_unique<engine::core::Context>(*dispatcher_
                                                     , *input_manager_
                                                     , *renderer_
                                                     , *camera_
                                                     , *resource_manager_
                                                     , *audio_player_
                                                     , *game_state_
                                                     , *time_)}
    , scene_manager_{std::make_unique<engine::scene::SceneManager>(*context_)} {
    // 根据缩放调整窗口大小
    sf::Vector2u window_size = static_cast<sf::Vector2u>(static_cast<sf::Vector2f>(config_->window_size_) * config_->window_scale_);
    window_->create(sf::VideoMode(window_size), config_->window_title_);

    // 调整 view 到缩放后大小
    camera_->set_world_view_size(static_cast<sf::Vector2f>(config_->window_size_));
    camera_->set_ui_view_sizs(static_cast<sf::Vector2f>(config_->window_size_));
    camera_->set_world_view_center(camera_->get_world_view_size() / 2.f);
    camera_->set_ui_view_center(camera_->get_ui_view_size() / 2.f);

    // 设置逻辑尺寸（与 ui_view 大小一致，供 UI 定位使用）
    game_state_->set_logical_size(static_cast<sf::Vector2f>(config_->window_size_));
    
    // 加载初始资源（载入默认资源映射文件）
    resource_manager_->load_resource("assets/data/resource_mapping.json");
        
    // 设置游戏音量（从 assets/config.json 里读取）
    audio_player_->set_music_volume(config_->music_volume_);    // 设置背景音乐音量
    audio_player_->set_sound_volume(config_->sound_volume_);    // 设置音效音量

    if (!init_imgui()) return;
    
    // 注册退出事件（回调函数可以无参数，代表不使用事件结构体中的数据）
    dispatcher_->sink<utils::QuitEvent>().connect<&Game::on_quit_event>(this);

    spdlog::trace("Game 初始化成功。");
}

Game::~Game() {
    spdlog::trace("关闭 Game ...");

    // --- ImGui 步骤4 清理 ---
    ImGui::SFML::Shutdown();
        
    // 断开事件处理函数
    dispatcher_->sink<utils::QuitEvent>().disconnect<&Game::on_quit_event>(this);
}

void Game::run() {
    // 调用场景设置函数(创建第一个场景并压入栈)
    scene_setup_func_(*context_);

    time_->set_target_fps(config_->target_fps_);

    while (window_->isOpen()) {
        const sf::Time delta = clock_.restart();

        // --- 输入帧开始 ---
        input_manager_->begin_frame();

        handle_event();

        ImGui::SFML::Update(*window_, delta);

        // --- 固定步长更新 ---
        time_->accumulate_frame_time(delta);
        while (time_->should_update()) {
            time_->consume_update_time();
            update(time_->get_frame_duration());
        }

        // --- 输入帧结束 ---
        input_manager_->end_frame();

        render();

        // 分发事件
        dispatcher_->update();
    }
}

void Game::register_scene_setup(std::function<void(engine::core::Context&)> func) {
    scene_setup_func_ = std::move(func);
    spdlog::trace("已注册场景设置函数");
}

void Game::handle_event() {
    while (std::optional event = window_->pollEvent()) {
        input_manager_->handle_event(*event);
    }
}

void Game::update(sf::Time delta) {
    // 游戏逻辑更新
    scene_manager_->update(delta);
}

void Game::render() {
    renderer_->clear_frame();

    scene_manager_->render();

    // 在场景渲染完成后、但显示之前，提交所有 ImGui 绘制数据
    ImGui::SFML::Render(*window_);

    renderer_->display_frame();
}

bool Game::init_imgui() {
    // 3. 初始化 imgui-sfml 后端
    // 这个调用会创建 ImGui 上下文并关联到 SFML 窗口。
    if (!ImGui::SFML::Init(*window_)) {
        spdlog::error("ImGui-SFML 初始化失败！");
        return false;
    }

    /* --- 可选配置开始 --- */
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();

    // 设置缩放
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(1.f); // 暂设为 1.0

    // 设置透明度
    float window_alpha = 0.5f;

    // 修改各个UI元素的透明度
    style.Colors[ImGuiCol_WindowBg].w = window_alpha;
    style.Colors[ImGuiCol_PopupBg].w = window_alpha;

    // 4. 正确的字体加载流程
    // a. 调用 AddFontFromFileTTF 加载你的中文字体
    io.Fonts->Clear();
    auto* font = io.Fonts->AddFontFromFileTTF(
        "assets/fonts/VonwaonBitmap-16px.ttf", 16.f, nullptr,
        io.Fonts->GetGlyphRangesChineseFull());
    // b. 如果加载失败，可以回退到默认字体(ImGui::SFML::Init 会创建一个默认字体)
    if (!font) {
        spdlog::warn("警告：无法加载中文字体，中文字符将无法正确显示。");
    }
    // c. 关键！必须调用此函数，它会根据你新加载的字体重新生成 ImGui 内部使用的纹理。
    if (!ImGui::SFML::UpdateFontTexture()) {
        spdlog::error("更新字体失败！");
    }
    /* --- 可选配置结束 --- */

    spdlog::trace("ImGui 初始化成功。");
    return true;
}

void engine::core::Game::on_quit_event() {
    spdlog::trace("Game 收到来自事件分发器的退出请求");
    window_->close();
}
} // namespace engine::core