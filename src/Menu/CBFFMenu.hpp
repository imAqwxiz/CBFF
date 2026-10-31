#pragma once
#include <Geode/Geode.hpp>
#include "../Core/CBFFTypes.hpp"

// Main CBFF menu: presets, master switch, per-mode frame windows.
class CBFFMenu : public geode::Popup<> {
public:
    static CBFFMenu* create();

protected:
    bool setup() override;
    void refresh();

    void onPreset(cocos2d::CCObject*);
    void onMaster(cocos2d::CCObject*);
    void onModeStep(cocos2d::CCObject*);   // tag = +1 / -1
    void onModeEnabled(cocos2d::CCObject*);
    void onAdjust(cocos2d::CCObject*);     // tag = 1..6 (see .cpp)
    void onTune(cocos2d::CCObject*);

    cbff::Mode m_mode = cbff::Mode::Cube;

    cocos2d::CCLabelBMFont* m_modeLabel = nullptr;
    cocos2d::CCLabelBMFont* m_bufferValue = nullptr;
    cocos2d::CCLabelBMFont* m_coyoteValue = nullptr;
    cocos2d::CCLabelBMFont* m_delayValue = nullptr;
    cocos2d::CCLabelBMFont* m_coyoteLabel = nullptr;
    cocos2d::CCLabelBMFont* m_blurb = nullptr;
    CCMenuItemToggler* m_masterToggle = nullptr;
    CCMenuItemToggler* m_modeToggle = nullptr;
    ButtonSprite* m_presetSprites[4] = {nullptr, nullptr, nullptr, nullptr};
};
