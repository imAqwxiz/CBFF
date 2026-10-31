#pragma once
#include "../Core/CBFFTypes.hpp"

// Frame tables for each preset. Order: Cube, Ship, Ball, UFO, Wave, Robot, Spider, Swing.
// mk(buffer, coyote, delay, enabled)
// Frames are game frames. Coyote only matters for Cube/Ball/Robot/Spider.
// For Ship/UFO/Wave/Swing the buffer is "orb buffering" only (clicks are never held back).
namespace cbff::presets {

inline ModeConfig mk(int buffer, int coyote, int delay = 0, bool enabled = true) {
    ModeConfig c;
    c.enabled = enabled;
    c.bufferFrames = buffer;
    c.coyoteFrames = coyote;
    c.delayFrames = delay;
    return c;
}

// Smallest windows, closest to vanilla timing. Wave off.
inline std::array<ModeConfig, kModeCount> safe() {
    return {mk(2, 1), mk(1, 0), mk(2, 1), mk(2, 0), mk(0, 0, 0, false), mk(2, 1), mk(2, 1), mk(1, 0)};
}

// Everyday default.
inline std::array<ModeConfig, kModeCount> balanced() {
    return {mk(6, 2), mk(2, 1), mk(4, 2), mk(3, 1), mk(0, 0, 0, false), mk(5, 2), mk(4, 2), mk(3, 1)};
}

// Big windows. Forgives a lot, deviates most from your real timing.
inline std::array<ModeConfig, kModeCount> aggressive() {
    return {mk(10, 4), mk(4, 2), mk(8, 3), mk(6, 2), mk(2, 1), mk(9, 4), mk(8, 3), mk(5, 2)};
}

// Tight windows for players who already click on frame: just catches the 1-3 frame slips.
inline std::array<ModeConfig, kModeCount> gdTop1() {
    return {mk(3, 2), mk(1, 1), mk(3, 1), mk(3, 1), mk(0, 0, 0, false), mk(3, 2), mk(2, 1), mk(2, 1)};
}

} // namespace cbff::presets
