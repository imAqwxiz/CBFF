#pragma once
#include "../Core/CBFFTypes.hpp"

namespace cbff {

class CBFFPresetManager {
public:
    static const char* presetName(PresetId id);
    static const char* presetBlurb(PresetId id);

    // Table for a preset (Custom returns the Balanced table).
    static std::array<ModeConfig, kModeCount> build(PresetId id);

    // Writes the preset into CBFFSettings and saves it.
    static void apply(PresetId id);
};

} // namespace cbff
