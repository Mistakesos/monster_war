#include "engine/ui/ui_manager.hpp"
#include "engine/ui/ui_panel.hpp"
#include "engine/ui/ui_element.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui {
UIManager::UIManager(sf::Vector2f window_size) {
    // 创建一个无特定大小和位置的Panel，它的子元素将基于它定位。
    root_element_ = std::make_unique<UIPanel>(sf::Vector2f{0.f, 0.f}, sf::Vector2f{0.f, 0.f});
    root_element_->set_size(std::move(window_size));
    spdlog::trace("UI管理器已初始化根面板。");
    spdlog::trace("UI管理器构造完成。");
}

UIManager::~UIManager() = default;

void UIManager::add_element(std::unique_ptr<UIElement> element) {
    if (root_element_) {
        root_element_->add_child(std::move(element));
    } else {
        spdlog::error("无法添加元素：root_element_ 为空！");
    }
}

void UIManager::clear_elements() {
    if (root_element_) {
        root_element_->remove_all_children();
        spdlog::trace("所有UI元素已从UI管理器中清除。");
    }
}

bool UIManager::handle_input(engine::core::Context& context) {
    if (root_element_ && root_element_->is_visible()) {
        // 从根元素开始向下分发事件
        if (root_element_->handle_input(context)) return true;
    }
    return false;
}

void UIManager::update(sf::Time delta, engine::core::Context& context) {
    if (root_element_ && root_element_->is_visible()) {
        // 从根元素开始向下更新
        root_element_->update(delta, context);
    }
}

void UIManager::render(engine::core::Context& context) {
    if (root_element_ && root_element_->is_visible()) {
        // 从根元素开始向下渲染
        root_element_->render(context);
    }
}

UIPanel* UIManager::get_root_element() const {
    return root_element_.get();
}
} // namespace engine::ui 