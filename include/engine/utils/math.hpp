#include <SFML/Graphics/Color.hpp>
#include <string_view>
#include <cstdint>
#include <charconv>
#include <spdlog/spdlog.h>

namespace engine::utils {
inline sf::Color parse_hex_color(std::string_view hex_str) {
    // 去掉可选的 '#' 前缀 (C++20 starts_with 自带空串安全检查)
    if (hex_str.starts_with('#')) {
        hex_str.remove_prefix(1);
    }

    // 长度必须为6位(RGB)或8位(RGBA)
    if (hex_str.size() != 6 && hex_str.size() != 8) {
        spdlog::warn("无效的十六进制颜色格式 '{}'，期望 #RRGGBB 或 #RRGGBBAA", hex_str);
        return sf::Color::Transparent;
    }

    // 使用 std::from_chars 零拷贝解析十六进制数
    std::uint32_t value = 0;
    auto [ptr, ec] = std::from_chars(hex_str.data(), hex_str.data() + hex_str.size(), value, 16);
    if (ec != std::errc{}) {
        spdlog::warn("解析十六进制颜色 '{}' 失败：包含非法字符", hex_str);
        return sf::Color::Transparent;
    }

    // 仅RGB时补充不透明Alpha通道 (0xFF)
    if (hex_str.size() == 6) {
        value = (value << 8) | 0xFF;
    }

    return sf::Color(value);
}

} // namespace engine::utils