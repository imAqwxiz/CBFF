#pragma once
#include "CBFFTypes.hpp"
#include <vector>

namespace cbff {

// What the game looks like for one player right now. The hooks fill this in,
// so the manager itself has no Geode/GD dependency.
struct PlayerView {
    bool active = false;
    bool dead = false;
    bool grounded = false;
    bool ringTouched = false;
    Mode mode = Mode::Cube;
};

enum class ClickAction {
    PassThrough, // let the game handle the click untouched
    Swallow,     // too early: hold it back (buffered)
    CoyoteJump,  // late but forgiven: hook runs the jump as if still grounded
};

struct FireEvent {
    int slot = 0;      // 0 = player 1, 1 = player 2
    bool press = true; // false = release
};

class CBFFInputManager {
public:
    static CBFFInputManager& get();

    // Called from the hook for every real jump press / release.
    ClickAction onPress(int slot, PlayerView const& view);
    void onRelease(int slot);

    // Called once per game frame (after physics). Fills `out` with clicks to send.
    void tick(PlayerView const (&views)[2], std::vector<FireEvent>& out);

    // New attempt / level reset.
    void reset();

    ModeStats& stats(Mode m) { return m_stats[idx(m)]; }
    void resetStats();

    // true while CBFF itself is sending a click, so the hook doesn't process it again
    bool synthetic = false;

private:
    struct Slot {
        // ground tracking
        int framesSinceGrounded = 999;
        int framesSinceJump = 999;
        bool heldWhileGrounded = false;
        bool coyoteUsed = false;
        // real button state
        bool physHeld = false;
        // buffered click
        bool pending = false;
        bool pendingBlocked = false;
        bool pendingReleased = false;
        Mode pendingMode = Mode::Cube;
        int pendingAge = 0;
        int validFrames = 0;
        // delayed release after a buffered fire
        int releaseCountdown = -1;
        // near-miss watch after an expiry
        int watch = -1;
        Mode watchMode = Mode::Cube;
    };

    bool coyoteAllowed(Slot const& s, ModeConfig const& mc) const;

    Slot m_slots[2];
    ModeStats m_stats[kModeCount];
};

} // namespace cbff
