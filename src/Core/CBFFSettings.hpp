#pragma once
#include "CBFFTypes.hpp"

namespace cbff {

class CBFFSettings {
public:
    static CBFFSettings& get();

    void load();
    void save() const;

    bool masterEnabled() const { return m_master; }
    void setMasterEnabled(bool v) { m_master = v; save(); }

    bool recordStats() const { return m_recordStats; }

    PresetId activePreset() const { return m_preset; }
    void setActivePreset(PresetId p) { m_preset = p; save(); }
    void markCustom() { m_preset = PresetId::Custom; save(); }

    ModeConfig& mode(Mode m) { return m_modes[idx(m)]; }
    const ModeConfig& mode(Mode m) const { return m_modes[idx(m)]; }

    void clamp();

private:
    bool m_master = true;
    bool m_recordStats = true;
    PresetId m_preset = PresetId::Balanced;
    std::array<ModeConfig, kModeCount> m_modes{};
};

} // namespace cbff
