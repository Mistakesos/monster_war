#pragma once

// 前置声明核心系统
namespace engine::input {
    class InputManager;
} // namespace engine::input

namespace engine::render {
    class Renderer;
    class Camera;
} // namespace engine::render

namespace engine::resource {
    class ResourceManager;
} // namespace engine::resource

namespace engine::audio {
    class AudioPlayer;
} // namespace engine::audio

namespace engine::core {
class GameState;

/**
 * @brief 持有对核心引擎模块引用的上下文对象
 * 
 * 用于简化依赖注入，传递Context即可获取引擎的各个模块
 */
class Context final {
public:
    /**
     * @brief 构造函数。
     * @param input_manager 对 InputManager 实例的引用。
     * @param renderer 对 Renderer 实例的引用。
     * @param camera 对 Camera 实例的引用。
     * @param resource_manager 对 ResourceManager 实例的引用。
     */
    Context(engine::input::InputManager& input_manager
          , engine::render::Renderer& renderer
          , engine::render::Camera& camera
          , engine::resource::ResourceManager& resource_manager
          , engine::audio::AudioPlayer& audio_player
          , engine::core::GameState& game_state);
    ~Context() = default;

    // 禁止拷贝和移动，Context 对象通常是唯一的或按需创建/传递
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(Context&&) = delete;

    // --- Getters ---
    engine::input::InputManager& get_input_manager() const { return input_manager_; }            ///< @brief 获取输入管理器
    engine::render::Renderer& get_renderer() const { return renderer_; }                         ///< @brief 获取渲染器
    engine::render::Camera& get_camera() const { return camera_; }                               ///< @brief 获取相机
    engine::resource::ResourceManager& get_resource_manager() const { return resource_manager_; }///< @brief 获取资源管理器
    engine::audio::AudioPlayer& get_audio_player() const { return audio_player_; }               ///< @brief 获取音频播放器
    engine::core::GameState& get_game_state() const { return game_state_; }                      ///< @brief 获取游戏状态

private:
    engine::input::InputManager& input_manager_;                ///< @brief 输入管理器
    engine::render::Renderer& renderer_;                        ///< @brief 渲染器
    engine::render::Camera& camera_;                            ///< @brief 相机
    engine::resource::ResourceManager& resource_manager_;       ///< @brief 资源管理器
    engine::audio::AudioPlayer& audio_player_;                  ///< @brief 音频播放器
    engine::core::GameState& game_state_;                       ///< @brief 游戏状态
};
} // namespace engine::core