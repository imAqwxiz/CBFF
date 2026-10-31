#include "CBFFInputManager.hpp"
#include "CBFFSettings.hpp"
#include <algorithm>

namespace cbff {

static constexpr int kNearMissWindow = 4; // frames after expiry we still watch for a landing

CBFFInputManager& CBFFInputManager::get() {
    static CBFFInputManager inst;
    return inst;
}

void CBFFInputManager::reset() {
    for (auto& s : m_slots) s = Slot{};
}

void CBFFInputManager::resetStats() {
    for (auto& st : m_stats) st = ModeStats{};
}

// Coyote only helps if the player WALKED off the ground. If they jumped (or the
// game auto-jumped because the button was held) it must not give a second jump.
bool CBFFInputManager::coyoteAllowed(Slot const& s, ModeConfig const& mc) const {
    if (mc.coyoteFrames <= 0) return false;
    if (s.coyoteUsed || s.heldWhileGrounded) return false;
    if (s.framesSinceGrounded > 900) return false; // never been grounded
    // a jump we know about left the ground at the same moment -> not a walk-off
    if (s.framesSinceJump <= s.framesSinceGrounded + 1) return false;
    return true;
}

ClickAction CBFFInputManager::onPress(int slot, PlayerView const& v) {
    auto& cfg = CBFFSettings::get();
    auto& s = m_slots[slot];
    s.physHeld = true;

    if (!cfg.masterEnabled()) return ClickAction::PassThrough;
    auto const& mc = cfg.mode(v.mode);
    if (!mc.enabled) return ClickAction::PassThrough;

    auto& st = m_stats[idx(v.mode)];
    bool rec = cfg.recordStats();
    if (rec) st.clicks++;

    // 1) valid right now: normal click, drop anything buffered
    if (v.grounded || v.ringTouched) {
        s.pending = false;
        s.framesSinceJump = 0;
        return ClickAction::PassThrough;
    }

    // 2) coyote: late click just after walking off a platform
    if (isGroundJumpMode(v.mode) && !v.grounded && s.framesSinceGrounded <= mc.coyoteFrames &&
        coyoteAllowed(s, mc)) {
        s.coyoteUsed = true;
        s.pending = false;
        s.framesSinceJump = 0;
        if (rec) st.coyoteSaves++;
        return ClickAction::CoyoteJump;
    }

    // coyote near-miss: would a slightly longer window have saved it?
    if (rec && isGroundJumpMode(v.mode) && mc.coyoteFrames > 0 && s.framesSinceGrounded > mc.coyoteFrames &&
        s.framesSinceGrounded <= mc.coyoteFrames + kNearMissWindow) {
        Slot probe = s;
        ModeConfig wide = mc;
        wide.coyoteFrames = mc.coyoteFrames + kNearMissWindow;
        if (coyoteAllowed(probe, wide)) {
            st.nearMissCoyote++;
            if (st.coyoteMissCount < 64) st.coyoteMissGaps[st.coyoteMissCount++] = s.framesSinceGrounded - mc.coyoteFrames;
        }
    }

    // 3) too early: buffer it
    if (mc.bufferFrames <= 0) return ClickAction::PassThrough;
    s.pending = true;
    s.pendingAge = 0;
    s.pendingReleased = false;
    s.validFrames = 0;
    s.pendingMode = v.mode;
    s.pendingBlocked = isGroundJumpMode(v.mode);
    if (rec) st.buffered++;
    return s.pendingBlocked ? ClickAction::Swallow : ClickAction::PassThrough;
}

void CBFFInputManager::onRelease(int slot) {
    auto& s = m_slots[slot];
    s.physHeld = false;
    if (s.pending) s.pendingReleased = true; // remember the real tap length
}

void CBFFInputManager::tick(PlayerView const (&views)[2], std::vector<FireEvent>& out) {
    auto& cfg = CBFFSettings::get();
    bool rec = cfg.recordStats();

    for (int slot = 0; slot < 2; ++slot) {
        auto& s = m_slots[slot];
        auto const& v = views[slot];
        if (s.framesSinceJump < 9999) s.framesSinceJump++;

        if (!v.active || v.dead) {
            s.pending = false;
            s.watch = -1;
            continue;
        }

        // ground tracking
        if (v.grounded) {
            s.framesSinceGrounded = 0;
            s.coyoteUsed = false;
            s.heldWhileGrounded = s.physHeld;
        } else if (s.framesSinceGrounded < 9999) {
            s.framesSinceGrounded++;
        }

        // delayed release for a buffered tap
        if (s.releaseCountdown > 0 && --s.releaseCountdown == 0) {
            out.push_back({slot, false});
            s.releaseCountdown = -1;
        }

        // near-miss watch after an expiry
        if (s.watch >= 0) {
            s.watch++;
            bool validNow = isGroundJumpMode(s.watchMode) ? (v.grounded || v.ringTouched) : v.ringTouched;
            if (validNow && v.mode == s.watchMode) {
                auto& st = m_stats[idx(s.watchMode)];
                if (rec) {
                    st.nearMissBuffer++;
                    if (st.bufferMissCount < 64) st.bufferMissGaps[st.bufferMissCount++] = s.watch;
                }
                s.watch = -1;
            } else if (s.watch > kNearMissWindow) {
                s.watch = -1;
            }
        }

        if (!s.pending) continue;
        auto const& mc = cfg.mode(s.pendingMode);
        auto& st = m_stats[idx(s.pendingMode)];

        // mode changed (portal): give the real input back if we were holding it
        if (v.mode != s.pendingMode) {
            if (s.pendingBlocked && s.physHeld) out.push_back({slot, true});
            s.pending = false;
            continue;
        }

        s.pendingAge++;
        bool valid = s.pendingBlocked ? (v.grounded || v.ringTouched) : v.ringTouched;

        if (valid) {
            s.validFrames++;
            if (s.validFrames > mc.delayFrames) {
                out.push_back({slot, true});
                if (s.pendingReleased) s.releaseCountdown = 2; // send the release a frame later
                if (rec) {
                    st.fired++;
                    st.fireAgeHist[std::min(s.pendingAge, 15)]++;
                }
                s.framesSinceJump = 0;
                s.pending = false;
            }
        } else {
            s.validFrames = 0;
            if (s.pendingAge >= mc.bufferFrames) {
                // ran out of time. If the player is STILL holding, hand the real
                // input back so the game behaves exactly like vanilla.
                if (s.pendingBlocked && s.physHeld) out.push_back({slot, true});
                if (rec) st.expired++;
                s.pending = false;
                s.watch = 0;
                s.watchMode = s.pendingMode;
            }
        }
    }
}

} // namespace cbff
