#include "engine/core/config.hpp"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>

namespace engine::core {
Config::Config(std::string_view filepath) {
    load_from_file(filepath);

    // 初始化动作到输入
    for (const auto& [action, scancodes] : keyboard_input_mappings_) {
        for (const auto& scancode : scancodes) {
            action_to_input_[action].push_back(scancode);
        }
    }
    
    for (const auto& [action, buttons] : mouse_input_mappings_) {
        for (const auto& button : buttons) {
            action_to_input_[action].push_back(button);
        }
    }
}

bool Config::load_from_file(std::string_view filepath) {
    auto path = std::filesystem::path(filepath);
    std::fstream file(path);
    if (!file.is_open()) {
        spdlog::warn("配置文件 ‘{}’ 未找到，将使用默认设置并创建默认配置文件", filepath);
        if (!save_to_file(filepath)) {
            spdlog::error("无法创建默认配置文件 ‘{}’ ", filepath);
            return false;
        }
        return false;
    }

    try {
        nlohmann::json json;
        file >> json;
        from_json(json);
        spdlog::info("成功从 ‘{}’ 加载配置", filepath);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("读取配置文件 ‘{}’ 时出错：{} \n使用默认设置", filepath, e.what());
    }
    return false;
}

bool Config::save_to_file(std::string_view filepath) {
    auto path = std::filesystem::path(filepath);
    std::ofstream file(path);
    if (!file.is_open()) {
        spdlog::error("无法打开配置文件 ‘{}’ 进行写入", filepath);
        return false;
    }

    try {
        nlohmann::ordered_json json = to_json();
        file << json.dump(4);
        spdlog::info("成功将配置保存到 ‘{}’", filepath);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("写入配置文件 ‘{}’ 时出错：{}", filepath, e.what());
    }
    return false;
}

void Config::from_json(const nlohmann::json& json) {
    if (json.contains("window")) {
        const auto& window_config = json["window"];
        window_title_ = window_config.value("title", window_title_);
        window_size_.x = window_config.value("width", window_size_.x);
        window_size_.y = window_config.value("height", window_size_.y);
        window_resizable_ = window_config.value("resizable", window_resizable_);
    }
    if (json.contains("graphics")) {
        const auto& graphics_config = json["graphics"];
        vsync_enabled_ = graphics_config.value("vsync", vsync_enabled_);
    }
    if (json.contains("performance")) {
        const auto& perf_config = json["performance"];
        target_fps_ = perf_config.value("target_fps", target_fps_);
    }
    if (json.contains("audio")) {
        const auto& audio_config = json["audio"];
        music_volume_ = audio_config.value("music_volume", music_volume_);
        sound_volume_ = audio_config.value("sound_volume", sound_volume_);
    }

    // 从 JSON 加载 input_mappings
    if (json.contains("keyboard_input_mappings") && json["keyboard_input_mappings"].is_object()) {
        const auto& mappings_json = json["keyboard_input_mappings"];
        try {
            // 直接尝试从 JSON 对象转换为 Action, std::vector<sf::Keyboard::Scancode>
            auto keyboard_input_mappings = mappings_json.get<std::unordered_map<Action, std::vector<sf::Keyboard::Scancode>>>();
            // 如果成功，则将 keyboard_input_mappings 移动到 keyboard_input_mappings_
            keyboard_input_mappings_ = std::move(keyboard_input_mappings);
            spdlog::trace("成功从配置加载输入映射。");
        } catch (const std::exception& e) {
            spdlog::warn("配置加载警告：解析 'keyboard_input_mappings' 时发生异常。使用默认映射。错误：{}", e.what());
        }
    } else {
        spdlog::trace("配置跟踪：未找到 'keyboard_input_mappings' 部分或不是对象。使用头文件中定义的默认映射。");
    }

    if (json.contains("mouse_input_mappings") && json["mouse_input_mappings"].is_object()) {
    const auto& mappings_json = json["mouse_input_mappings"];
    try {
        // 直接尝试从 JSON 对象转换为 Action, std::vector<sf::Mouse::Button>
        auto mouse_input_mappings = mappings_json.get<std::unordered_map<Action, std::vector<sf::Mouse::Button>>>();
        // 如果成功，则将 mouse_input_mappings 移动到 mouse_input_mappings_
        mouse_input_mappings_ = std::move(mouse_input_mappings);
        spdlog::trace("成功从配置加载输入映射。");
    } catch (const std::exception& e) {
        spdlog::warn("配置加载警告：解析 'mouse_input_mappings' 时发生异常。使用默认映射。错误：{}", e.what());
    }
    } else {
        spdlog::trace("配置跟踪：未找到 'mouse_input_mappings' 部分或不是对象。使用头文件中定义的默认映射。");
    }
}

nlohmann::ordered_json Config::to_json() const {
    return nlohmann::ordered_json{
        {"window", {
            {"title", window_title_},
            {"width", window_size_.x},
            {"height", window_size_.y},
            {"resizable", window_resizable_}
        }},
        {"graphics", {
            {"vsync", vsync_enabled_}
        }},
        {"performance", {
            {"target_fps", target_fps_}
        }},
        {"audio", {
            {"music_volume", music_volume_},
            {"sound_volume", sound_volume_}
        }},
        {"keyboard_input_mappings", keyboard_input_mappings_},
        {"mouse_input_mappings", mouse_input_mappings_}
    };
}
} // namespace engine::core