#include "CBFFSettings.hpp"
#include "../Preset/CBFFPresetManager.hpp"
#include <Geode/Geode.hpp>
#include <algorithm>
#include <string>

using namespace geode::prelude;

namespace cbff {

CBFFSettings& CBFFSettings::get() {
    static CBFFSettings inst;
    return inst;
}

static std::string key(Mode m, char const* field) {
    return std::string(modeKey(m)) + "-" + field;
}

void CBFFSettings::load() {
    auto* mod = Mod::get();
    m_master = mod->getSavedValue<bool>("master", true);
    m_recordStats = mod->getSavedValue<bool>("record-stats", true);
    m_preset = static_cast<PresetId>(mod->getSavedValue<int>("preset", static_cast<int>(PresetId::Balanced)));

    // defaults = Balanced preset
    auto defaults = CBFFPresetManager::build(PresetId::Balanced);
    for (std::size_t i = 0; i < kModeCount; ++i) {
        auto m = static_cast<Mode>(i);
        auto const& d = defaults[i];
        m_modes[i].enabled = mod->getSavedValue<bool>(key(m, "enabled"), d.enabled);
        m_modes[i].bufferFrames = mod->getSavedValue<int>(key(m, "buffer"), d.bufferFrames);
        m_modes[i].coyoteFrames = mod->getSavedValue<int>(key(m, "coyote"), d.coyoteFrames);
        m_modes[i].delayFrames = mod->getSavedValue<int>(key(m, "delay"), d.delayFrames);
    }
    clamp();
}

void CBFFSettings::save() const {
    auto* mod = Mod::get();
    mod->setSavedValue("master", m_master);
    mod->setSavedValue("record-stats", m_recordStats);
    mod->setSavedValue("preset", static_cast<int>(m_preset));
    for (std::size_t i = 0; i < kModeCount; ++i) {
        auto m = static_cast<Mode>(i);
        mod->setSavedValue(key(m, "enabled"), m_modes[i].enabled);
        mod->setSavedValue(key(m, "buffer"), m_modes[i].bufferFrames);
        mod->setSavedValue(key(m, "coyote"), m_modes[i].coyoteFrames);
        mod->setSavedValue(key(m, "delay"), m_modes[i].delayFrames);
    }
}

void CBFFSettings::clamp() {
    int p = static_cast<int>(m_preset);
    if (p < 0 || p >= static_cast<int>(PresetId::Count)) m_preset = PresetId::Custom;
    for (auto& c : m_modes) {
        c.bufferFrames = std::clamp(c.bufferFrames, 0, limits::kMaxBuffer);
        c.coyoteFrames = std::clamp(c.coyoteFrames, 0, limits::kMaxCoyote);
        c.delayFrames = std::clamp(c.delayFrames, 0, limits::kMaxDelay);
    }
}

} // namespace cbff
