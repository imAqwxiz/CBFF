#include "CBFFPresetManager.hpp"
#include "PresetData.hpp"
#include "../Core/CBFFSettings.hpp"

namespace cbff {

const char* CBFFPresetManager::presetName(PresetId id) {
    switch (id) {
        case PresetId::Safe: return "Safe";
        case PresetId::Balanced: return "Balanced";
        case PresetId::Aggressive: return "Aggressive";
        case PresetId::GDTop1: return "GD Top 1";
        case PresetId::Custom: return "Custom";
        default: return "?";
    }
}

const char* CBFFPresetManager::presetBlurb(PresetId id) {
    switch (id) {
        case PresetId::Safe: return "Tiny windows, closest to vanilla";
        case PresetId::Balanced: return "Everyday default";
        case PresetId::Aggressive: return "Big windows, very forgiving";
        case PresetId::GDTop1: return "Tight windows for on-frame players";
        case PresetId::Custom: return "Your own values";
        default: return "";
    }
}

std::array<ModeConfig, kModeCount> CBFFPresetManager::build(PresetId id) {
    switch (id) {
        case PresetId::Safe: return presets::safe();
        case PresetId::Aggressive: return presets::aggressive();
        case PresetId::GDTop1: return presets::gdTop1();
        case PresetId::Balanced:
        case PresetId::Custom:
        default: return presets::balanced();
    }
}

void CBFFPresetManager::apply(PresetId id) {
    if (id == PresetId::Custom) return;
    auto table = build(id);
    auto& s = CBFFSettings::get();
    for (std::size_t i = 0; i < kModeCount; ++i) {
        s.mode(static_cast<Mode>(i)) = table[i];
    }
    s.setActivePreset(id); // also saves
}

} // namespace cbff
