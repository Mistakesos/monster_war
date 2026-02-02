#pragma once
#include "component.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>

namespace engine::render {
    class Animation;
} // namespace engine::render

namespace engine::component {
    class SpriteComponent;
} // namespace engine::component

namespace engine::component {
/**
 * @brief GameObject的动画组件。
 *
 * 持有一组Animation对象并控制其播放，
 * 根据当前帧更新关联的SpriteComponent。
 */
class AnimationComponent : public Component {
    friend class engine::object::GameObject;
public:
    AnimationComponent(engine::object::GameObject* owner);
    ~AnimationComponent() override;

    // 删除复制/移动操作
    AnimationComponent(const AnimationComponent&) = delete;
    AnimationComponent& operator=(const AnimationComponent&) = delete;
    AnimationComponent(AnimationComponent&&) = delete;
    AnimationComponent& operator=(AnimationComponent&&) = delete;

    void add_animation(std::unique_ptr<engine::render::Animation> animation);   ///< @brief 向 animations_ map容器中添加一个动画。
    void play_animation(std::string_view name);                                 ///< @brief 播放指定名称的动画。
    void stop_animation() { is_playing_ = false; }                              ///< @brief 停止当前动画播放。
    void resume_animation() {is_playing_ = true; }                              ///< @brief 恢复当前动画播放。

    // --- Getters and Setters ---
    std::string_view get_current_animation_name() const;
    bool is_playing() const { return is_playing_; }
    bool is_animation_finished() const;
    bool is_one_shot_removal() const { return is_one_shot_removal_; }
    void set_one_shot_removal(bool is_one_shot_removal) { is_one_shot_removal_ = is_one_shot_removal; }

protected:
    // 核心循环方法
    void update(sf::Time delta, engine::core::Context& context) override;

private:
    /// @brief 动画名称到Animation对象的映射。
    std::unordered_map<std::string, std::unique_ptr<engine::render::Animation>> animations_;
    SpriteComponent* sprite_component_obs_ = nullptr;               ///< @brief 指向必需的SpriteComponent的指针
    engine::render::Animation* current_animation_obs_ = nullptr;    ///< @brief 指向当前播放动画的原始指针

    sf::Time animation_timer_ = sf::Time::Zero;         ///< @brief 动画播放中的计时器
    bool is_playing_ = false;                           ///< @brief 当前是否有动画正在播放
    bool is_one_shot_removal_ = false;                  ///< @brief 是否在动画结束后删除整个GameObject
};
} // namespace engine::component