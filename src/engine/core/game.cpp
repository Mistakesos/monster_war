#include "engine/core/game.hpp"
#include "engine/utils/action.hpp"
#include "engine/core/time.hpp"
#include "engine/core/config.hpp"
#include "engine/resource/resource_manager.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/scene/scene_manager.hpp"
#include "engine/render/render.hpp"
#include "engine/render/camera.hpp"
#include "engine/object/game_object.hpp"
#include "engine/audio/audio_player.hpp"
#include "engine/core/game_state.hpp"
#include "engine/core/context.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <spdlog/spdlog.h>

namespace engine::core {
Game::Game()
    : config_{std::make_unique<Config>("assets/config.json")}
    , window_{std::make_unique<sf::RenderWindow>(sf::VideoMode(config_->window_size_), config_->window_title_)}
    , time_{std::make_unique<Time>()}
    , resource_manager_{std::make_unique<engine::resource::ResourceManager>()}
    , input_manager_{std::make_unique<engine::input::InputManager>(window_.get(), config_.get())}
    , renderer_{std::make_unique<engine::render::Renderer>(window_.get(), resource_manager_.get())}
    , camera_{std::make_unique<engine::render::Camera>(window_.get())}
    , audio_player_{std::make_unique<engine::audio::AudioPlayer>(resource_manager_.get())}
    , game_state_{std::make_unique<engine::core::GameState>(window_.get())}
    , context_{std::make_unique<engine::core::Context>(*input_manager_
                                                     , *renderer_
                                                     , *camera_
                                                     , *resource_manager_
                                                     , *audio_player_
                                                     , *game_state_)}
    , scene_manager_{std::make_unique<engine::scene::SceneManager>(*context_)} {
    // 设置游戏音量（从 assets/config.json 里读取）
    audio_player_->set_music_volume(config_->music_volume_);    // 设置背景音乐音量
    audio_player_->set_sound_volume(config_->sound_volume_);    // 设置音效音量
}

Game::~Game() = default;

void Game::run() {
    // 调用场景设置函数(创建第一个场景并压入栈)
    scene_setup_func_(*scene_manager_);

    time_->set_target_fps(config_->target_fps_);

    while (window_->isOpen()) {
        input_manager_->update();
        handle_event();

        time_->accumulate_frame_time();
        while (time_->should_update()) {
            time_->consume_update_time();

            handle_event();
            update(time_->get_frame_duration());
        }
        render();
    }
}

void Game::register_scene_setup(std::function<void(engine::scene::SceneManager&)> func) {
    scene_setup_func_ = std::move(func);
    spdlog::trace("已注册场景设置函数");
}

void Game::handle_event() {
    input_manager_->process_event();

    if (input_manager_->should_quit()) {
        spdlog::trace("Game 收到来自 InputManager 的退出请求。");
        window_->close();
    }

    scene_manager_->handle_input();
}

void Game::update(sf::Time delta) {
    scene_manager_->update(delta);
}

void Game::render() {
    window_->clear();

    scene_manager_->render();

    window_->display();
}
} // namespace engine::core