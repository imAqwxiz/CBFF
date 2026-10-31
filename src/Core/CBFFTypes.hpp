#pragma once
#include <array>
#include <cstddef>

namespace cbff {

enum class Mode : int { Cube, Ship, Ball, UFO, Wave, Robot, Spider, Swing, Count };
constexpr std::size_t kModeCount = static_cast<std::size_t>(Mode::Count);

inline std::size_t idx(Mode m) { return static_cast<std::size_t>(m); }

inline const char* modeName(Mode m) {
    switch (m) {
        case Mode::Cube: return "Cube";
        case Mode::Ship: return "Ship";
        case Mode::Ball: return "Ball";
        case Mode::UFO: return "UFO";
        case Mode::Wave: return "Wave";
        case Mode::Robot: return "Robot";
        case Mode::Spider: return "Spider";
        case Mode::Swing: return "Swing";
        default: return "?";
    }
}

// used for saved-value keys ("cube-buffer", ...)
inline const char* modeKey(Mode m) {
    switch (m) {
        case Mode::Cube: return "cube";
        case Mode::Ship: return "ship";
        case Mode::Ball: return "ball";
        case Mode::UFO: return "ufo";
        case Mode::Wave: return "wave";
        case Mode::Robot: return "robot";
        case Mode::Spider: return "spider";
        case Mode::Swing: return "swing";
        default: return "unknown";
    }
}

// Cube/Ball/Robot/Spider only act on the ground (or on an orb), so an early click
// is held back (buffered) and a late click can be forgiven (coyote).
// Ship/UFO/Wave/Swing work anywhere, so their clicks are NEVER held back (that
// would change how they fly) - they only get "orb buffering": a click made just
// before touching an orb is re-sent when the orb is touched.
inline bool isGroundJumpMode(Mode m) {
    return m == Mode::Cube || m == Mode::Ball || m == Mode::Robot || m == Mode::Spider;
}

struct ModeConfig {
    bool enabled = true;
    int bufferFrames = 4; // early clicks: how long a click is remembered
    int coyoteFrames = 2; // late clicks: grace after leaving the ground (ground modes only)
    int delayFrames = 0;  // extra frames a valid buffered click waits before firing
};

namespace limits {
constexpr int kMaxBuffer = 15;
constexpr int kMaxCoyote = 8;
constexpr int kMaxDelay = 4;
} // namespace limits

enum class PresetId : int { Safe, Balanced, Aggressive, GDTop1, Custom, Count };

struct ModeStats {
    int clicks = 0;
    int buffered = 0;      // clicks that arrived too early
    int fired = 0;         // buffered clicks that executed
    int expired = 0;       // buffered clicks that ran out of time
    int coyoteSaves = 0;   // late clicks that were forgiven
    int nearMissBuffer = 0;
    int nearMissCoyote = 0;
    std::array<int, 16> fireAgeHist{}; // age (frames) when buffered clicks fired
    int bufferMissGaps[64]{};          // frames AFTER expiry that the action became valid
    int bufferMissCount = 0;
    int coyoteMissGaps[64]{};          // frames beyond the coyote window
    int coyoteMissCount = 0;
};

} // namespace cbff
