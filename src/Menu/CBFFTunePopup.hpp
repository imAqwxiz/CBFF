#pragma once
#include <Geode/Geode.hpp>
#include "../Core/CBFFTypes.hpp"

// Shows tracked stats per mode and the tuner's recommendations.
// Nothing here changes by itself: a value only changes when you tap Apply.
class CBFFTunePopup : public geode::Popup {
public:
    static CBFFTunePopup* create();

protected:
    bool init();
    void rebuild();
    void onApply(cocos2d::CCObject*);    // tag = mode index
    void onApplyAll(cocos2d::CCObject*);
    void onResetStats(cocos2d::CCObject*);

    cocos2d::CCNode* m_rows = nullptr;
};
