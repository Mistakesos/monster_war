#include "engine/core/game_state.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::core {
GameState::GameState(sf::RenderWindow* window, State initial_state)
    : window_obs_{window}
    , current_state_{initial_state} {
    if (window_obs_ == nullptr) {
        spdlog::error("窗口为空");
        throw std::runtime_error("窗口不能为空");
    }
    spdlog::trace("游戏状态初始化完成");
}

void GameState::set_state(State new_state) {
    if (current_state_ != new_state) {
        spdlog::debug("游戏状态改变");
        current_state_ = new_state;
    } else {
        spdlog::debug("尝试设置相同的游戏状态，跳过");
    }
}

sf::Vector2u GameState::get_window_size() const {
    return window_obs_->getSize();
}

void GameState::set_window_size(sf::Vector2u new_size) {
    window_obs_->setSize(new_size);
}

sf::Vector2f GameState::get_logical_size() const {
    return window_obs_->getView().getSize();
}

void GameState::set_logical_size(sf::Vector2f new_size) {
    const auto& view = window_obs_->getView();
    window_obs_->setView({view.getCenter(), new_size});
    spdlog::trace("逻辑分辨率设置为: {}x{}", new_size.x, new_size.y);
}
} // namespace engine::core