// CBFFPlayerHook: sees every real click, decides pass / hold back / coyote.
// It never touches velocity, gravity, hitboxes or positions.
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "CBFFPlayerState.hpp"
#include "../Core/CBFFSettings.hpp"

using namespace geode::prelude;

class $modify(CBFFGameLayer, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        auto& mgr = cbff::CBFFInputManager::get();

        // only the jump button, only inside a real level, never our own clicks
        if (button != 1 || mgr.synthetic || !PlayLayer::get()) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
            return;
        }

        int slot = isPlayer1 ? 0 : 1;
        auto* player = isPlayer1 ? m_player1 : m_player2;
        if (!player) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
            return;
        }

        if (!down) {
            mgr.onRelease(slot); // releases always go through: real input is preserved
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
            return;
        }

        auto view = cbff::viewOf(player, true);
        switch (mgr.onPress(slot, view)) {
            case cbff::ClickAction::Swallow:
                return; // held back; CBFFGameUpdate sends it when it becomes valid
            case cbff::ClickAction::CoyoteJump: {
                // The player just left the platform: run the click as if still grounded.
                bool wasOnGround = player->m_isOnGround;
                player->m_isOnGround = true;
                GJBaseGameLayer::handleButton(down, button, isPlayer1);
                player->m_isOnGround = wasOnGround;
                return;
            }
            case cbff::ClickAction::PassThrough:
            default:
                GJBaseGameLayer::handleButton(down, button, isPlayer1);
                return;
        }
    }
};
