// CBFFGameUpdate: once per frame, advances the timers and sends buffered clicks.
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <vector>
#include "CBFFPlayerState.hpp"
#include "../Core/CBFFSettings.hpp"

using namespace geode::prelude;

class $modify(CBFFPlayLayer, PlayLayer) {
    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        auto& cfg = cbff::CBFFSettings::get();
        if (!cfg.masterEnabled()) return;

        bool dual = m_gameState.m_isDualMode;
        cbff::PlayerView views[2] = {
            cbff::viewOf(m_player1, true),
            cbff::viewOf(m_player2, dual),
        };

        std::vector<cbff::FireEvent> fires;
        cbff::CBFFInputManager::get().tick(views, fires);

        for (auto const& f : fires) {
            cbff::SyntheticScope scope;
            this->handleButton(f.press, 1, f.slot == 0);
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        cbff::CBFFInputManager::get().reset();
    }
};
