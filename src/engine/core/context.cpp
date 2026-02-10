#include "engine/core/context.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/render/render.hpp"
#include "engine/render/camera.hpp"
#include "engine/resource/resource_manager.hpp"
#include "engine/audio/audio_player.hpp"
#include "engine/core/game_state.hpp"
#include "entt/signal/dispatcher.hpp"
#include <spdlog/spdlog.h>

namespace engine::core {
Context::Context(entt::dispatcher& dispatcher
               , engine::input::InputManager& input_manager
               , engine::render::Renderer& renderer
               , engine::render::Camera& camera
               , engine::resource::ResourceManager& resource_manager
               , engine::audio::AudioPlayer& audio_player
               , engine::core::GameState& game_state)
    : dispatcher_{dispatcher}
    , input_manager_{input_manager}
    , renderer_{renderer}
    , camera_{camera}
    , resource_manager_{resource_manager}
    , audio_player_{audio_player}
    , game_state_{game_state} {
    spdlog::trace("上下文已创建并初始化");
}
} // namespace engine::core