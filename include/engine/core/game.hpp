#pragma once
#include <memory>
#include <functional>

// 前向声明，减少头文件依赖，增加编译速度
namespace sf {
    class RenderWindow;
    class Time;
    class Event;
} // namespace sf

namespace engine::resource {
    class ResourceManager;
} // namespace engine::resource

namespace engine::render {
    class Renderer;
    class Camera;
} // namespace engine::render

namespace engine::input {
    class InputManager;
} // namespace engine::input

namespace engine::scene {
    class SceneManager;
} // namespace engine::scene

namespace engine::audio {
    class AudioPlayer;
} // namespace engine::audio

namespace engine::core {
class Time;
class Config;
class Context;
class GameState;
/**
 * @brief 主游戏类，初始化资源，管理游戏循环
 */
class Game final {
public:
    Game();
    ~Game();

    /**
     * @brief 运行游戏，构造，析构，不需要手动管理他
     */
    void run();

    /**
     * @brief 注册用于设置初始游戏场景的函数。
     *        这个函数将在 SceneManager 初始化后被调用。
     * @param func 一个接收 SceneManager 引用的函数对象。
     */
    void register_scene_setup(std::function<void(engine::scene::SceneManager&)> func);

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;

private:
    void handle_event();
    void update(sf::Time delta);
    void render();

    // 配置组件，优先加载，优先级最高
    std::unique_ptr<engine::core::Config> config_;

    // 游戏主窗口
    std::unique_ptr<sf::RenderWindow> window_;

    /// @brief 游戏场景设置函数，用于在运行游戏前设置初始场景 (GameApp不再决定初始场景是什么)
    std::function<void(engine::scene::SceneManager&)> scene_setup_func_;

    // 引擎组件
    std::unique_ptr<engine::core::Time> time_;                                  ///< @brief 时间组件
    std::unique_ptr<engine::resource::ResourceManager> resource_manager_;       ///< @brief 资源管理器组件
    std::unique_ptr<engine::input::InputManager> input_manager_;                ///< @brief 输入管理器组件
    std::unique_ptr<engine::render::Renderer> renderer_;                        ///< @brief 渲染器组件
    std::unique_ptr<engine::render::Camera> camera_;                            ///< @brief 摄像机组件
    std::unique_ptr<engine::audio::AudioPlayer> audio_player_;                  ///< @brief 音频播放组件
    std::unique_ptr<engine::core::GameState> game_state_;                       ///< @brief 游戏状态组件
    std::unique_ptr<engine::core::Context> context_;                            ///< @brief ！上下文组件，最后初始化的组件
    std::unique_ptr<engine::scene::SceneManager> scene_manager_;                ///< @brief 场景管理器,依赖上下文，最后初始化
};
} // namespace engine::core