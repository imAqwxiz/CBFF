#pragma once
#include <Geode/Geode.hpp>
#include "../Core/CBFFInputManager.hpp"

namespace cbff {

inline Mode modeOf(PlayerObject* p) {
    if (p->m_isShip) return Mode::Ship;
    if (p->m_isBird) return Mode::UFO;
    if (p->m_isBall) return Mode::Ball;
    if (p->m_isDart) return Mode::Wave;
    if (p->m_isRobot) return Mode::Robot;
    if (p->m_isSpider) return Mode::Spider;
    if (p->m_isSwing) return Mode::Swing;
    return Mode::Cube;
}

inline PlayerView viewOf(PlayerObject* p, bool active) {
    PlayerView v;
    if (!p || !active) return v;
    v.active = true;
    v.dead = p->m_isDead;
    v.grounded = p->m_isOnGround;
    v.ringTouched = p->m_touchingRings && p->m_touchingRings->count() > 0;
    v.mode = modeOf(p);
    return v;
}

// While alive, CBFF's own clicks pass straight through the handleButton hook.
struct SyntheticScope {
    SyntheticScope() { CBFFInputManager::get().synthetic = true; }
    ~SyntheticScope() { CBFFInputManager::get().synthetic = false; }
};

} // namespace cbff
