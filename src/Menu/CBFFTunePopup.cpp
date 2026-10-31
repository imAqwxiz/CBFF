#include "CBFFTunePopup.hpp"
#include "../Core/CBFFInputManager.hpp"
#include "../Core/CBFFSettings.hpp"
#include "../Tuning/CBFFTuner.hpp"

using namespace geode::prelude;
using namespace cbff;

CBFFTunePopup* CBFFTunePopup::create() {
    auto ret = new CBFFTunePopup();
    if (ret->initAnchored(440.f, 290.f, "GJ_square02.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool CBFFTunePopup::setup() {
    this->setTitle("Stats & Tuning", "goldFont.fnt", 0.8f, 22.f);

    auto hint = CCLabelBMFont::create("Suggestions only - nothing changes unless you tap Apply", "chatFont.fnt");
    hint->setScale(0.5f);
    m_mainLayer->addChildAtPosition(hint, Anchor::Center, {0.f, 100.f});

    m_rows = CCNode::create();
    m_mainLayer->addChildAtPosition(m_rows, Anchor::Center);

    auto applyAll = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Apply all", 90, true, "goldFont.fnt", "GJ_button_01.png", 28.f, 0.7f), this,
        menu_selector(CBFFTunePopup::onApplyAll));
    m_buttonMenu->addChildAtPosition(applyAll, Anchor::Center, {-70.f, -122.f});

    auto reset = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Reset stats", 100, true, "goldFont.fnt", "GJ_button_06.png", 28.f, 0.7f), this,
        menu_selector(CBFFTunePopup::onResetStats));
    m_buttonMenu->addChildAtPosition(reset, Anchor::Center, {70.f, -122.f});

    this->rebuild();
    return true;
}

void CBFFTunePopup::rebuild() {
    // clear old rows (labels + apply buttons that were added to m_rows / button menu)
    m_rows->removeAllChildren();
    for (int i = 0; i < static_cast<int>(kModeCount); ++i) {
        if (auto old = m_buttonMenu->getChildByTag(1000 + i)) old->removeFromParent();
    }

    auto& cfg = CBFFSettings::get();
    for (int i = 0; i < static_cast<int>(kModeCount); ++i) {
        auto m = static_cast<Mode>(i);
        auto& st = CBFFInputManager::get().stats(m);
        auto rec = CBFFTuner::recommend(m);
        auto const& c = cfg.mode(m);
        float y = 78.f - i * 22.f;

        auto name = CCLabelBMFont::create(modeName(m), "goldFont.fnt");
        name->setScale(0.45f);
        name->setAnchorPoint({0.f, 0.5f});
        name->setPosition({-205.f, y});
        m_rows->addChild(name);

        auto line1 = fmt::format("clk {}  buf {}  fire {}  exp {}  coy {}", st.clicks, st.buffered, st.fired,
                                 st.expired, st.coyoteSaves);
        auto stats = CCLabelBMFont::create(line1.c_str(), "chatFont.fnt");
        stats->setScale(0.42f);
        stats->setAnchorPoint({0.f, 0.5f});
        stats->setPosition({-150.f, y + 5.f});
        m_rows->addChild(stats);

        std::string line2 = rec.enoughData && rec.differs
            ? fmt::format("{}  ->  buf {}f, coy {}f  (now {}f/{}f)", rec.note, rec.bufferFrames, rec.coyoteFrames,
                          c.bufferFrames, c.coyoteFrames)
            : rec.note;
        auto note = CCLabelBMFont::create(line2.c_str(), "chatFont.fnt");
        note->setScale(0.38f);
        note->setColor(rec.differs ? ccColor3B{120, 255, 120} : ccColor3B{190, 190, 190});
        note->setAnchorPoint({0.f, 0.5f});
        note->setPosition({-150.f, y - 5.f});
        m_rows->addChild(note);

        if (rec.enoughData && rec.differs) {
            auto b = CCMenuItemSpriteExtra::create(
                ButtonSprite::create("Apply", 50, true, "goldFont.fnt", "GJ_button_01.png", 22.f, 0.5f), this,
                menu_selector(CBFFTunePopup::onApply));
            b->setTag(1000 + i);
            m_buttonMenu->addChildAtPosition(b, Anchor::Center, {185.f, y});
        }
    }
}

void CBFFTunePopup::onApply(CCObject* sender) {
    int i = static_cast<CCNode*>(sender)->getTag() - 1000;
    if (i < 0 || i >= static_cast<int>(kModeCount)) return;
    auto m = static_cast<Mode>(i);
    CBFFTuner::apply(m, CBFFTuner::recommend(m));
    this->rebuild();
}

void CBFFTunePopup::onApplyAll(CCObject*) {
    for (int i = 0; i < static_cast<int>(kModeCount); ++i) {
        auto m = static_cast<Mode>(i);
        auto rec = CBFFTuner::recommend(m);
        if (rec.enoughData && rec.differs) CBFFTuner::apply(m, rec);
    }
    this->rebuild();
}

void CBFFTunePopup::onResetStats(CCObject*) {
    CBFFInputManager::get().resetStats();
    this->rebuild();
}
