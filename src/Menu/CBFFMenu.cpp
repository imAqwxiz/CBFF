#include "CBFFMenu.hpp"
#include "CBFFTunePopup.hpp"
#include "../Core/CBFFSettings.hpp"
#include "../Preset/CBFFPresetManager.hpp"
#include <algorithm>

using namespace geode::prelude;
using namespace cbff;

static constexpr PresetId kPresetOrder[4] = {PresetId::Safe, PresetId::Balanced, PresetId::Aggressive, PresetId::GDTop1};

CBFFMenu* CBFFMenu::create() {
    auto ret = new CBFFMenu();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool CBFFMenu::init() {
    if (!Popup::init(400.f, 300.f, "GJ_square01.png")) return false;

    this->setTitle("CBFF Input Buffer", "goldFont.fnt", 0.8f, 22.f);

    auto addLabel = [&](char const* text, float x, float y, float scale, bool leftAlign = false) {
        auto l = CCLabelBMFont::create(text, "bigFont.fnt");
        l->setScale(scale);
        if (leftAlign) l->setAnchorPoint({0.f, 0.5f});
        m_mainLayer->addChildAtPosition(l, Anchor::Center, {x, y});
        return l;
    };
    auto addButton = [&](CCNode* spr, SEL_MenuHandler cb, float x, float y, int tag = 0) {
        auto b = CCMenuItemSpriteExtra::create(spr, this, cb);
        b->setTag(tag);
        m_buttonMenu->addChildAtPosition(b, Anchor::Center, {x, y});
        return b;
    };
    auto smallBtn = [](char const* text) {
        return ButtonSprite::create(text, 26, true, "goldFont.fnt", "GJ_button_01.png", 26.f, 0.7f);
    };

    // --- presets row ---
    for (int i = 0; i < 4; ++i) {
        auto spr = ButtonSprite::create(CBFFPresetManager::presetName(kPresetOrder[i]), 82, true, "bigFont.fnt",
                                        "GJ_button_04.png", 30.f, 0.5f);
        m_presetSprites[i] = spr;
        addButton(spr, menu_selector(CBFFMenu::onPreset), -135.f + i * 90.f, 98.f, static_cast<int>(kPresetOrder[i]));
    }
    m_blurb = addLabel("", 0.f, 72.f, 0.32f);

    // --- master switch + tune ---
    m_masterToggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(CBFFMenu::onMaster), 0.6f);
    m_buttonMenu->addChildAtPosition(m_masterToggle, Anchor::Center, {-160.f, 44.f});
    addLabel("CBFF On", -138.f, 44.f, 0.5f, true);
    addButton(ButtonSprite::create("Stats & Tune", 110, true, "goldFont.fnt", "GJ_button_02.png", 28.f, 0.7f),
              menu_selector(CBFFMenu::onTune), 110.f, 44.f);

    // --- mode selector ---
    auto left = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    auto right = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    right->setFlipX(true);
    left->setScale(0.7f);
    right->setScale(0.7f);
    addButton(left, menu_selector(CBFFMenu::onModeStep), -110.f, 8.f, -1);
    addButton(right, menu_selector(CBFFMenu::onModeStep), 110.f, 8.f, 1);
    m_modeLabel = addLabel("Cube", 0.f, 8.f, 0.9f);

    // --- per-mode settings ---
    addLabel("Mode enabled", -165.f, -26.f, 0.5f, true);
    m_modeToggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(CBFFMenu::onModeEnabled), 0.6f);
    m_buttonMenu->addChildAtPosition(m_modeToggle, Anchor::Center, {140.f, -26.f});

    addLabel("Early click buffer", -165.f, -58.f, 0.5f, true);
    addButton(smallBtn("-"), menu_selector(CBFFMenu::onAdjust), 60.f, -58.f, 1);
    m_bufferValue = addLabel("0", 100.f, -58.f, 0.6f);
    addButton(smallBtn("+"), menu_selector(CBFFMenu::onAdjust), 140.f, -58.f, 2);

    m_coyoteLabel = addLabel("Late click coyote", -165.f, -90.f, 0.5f, true);
    addButton(smallBtn("-"), menu_selector(CBFFMenu::onAdjust), 60.f, -90.f, 3);
    m_coyoteValue = addLabel("0", 100.f, -90.f, 0.6f);
    addButton(smallBtn("+"), menu_selector(CBFFMenu::onAdjust), 140.f, -90.f, 4);

    addLabel("Click delay", -165.f, -122.f, 0.5f, true);
    addButton(smallBtn("-"), menu_selector(CBFFMenu::onAdjust), 60.f, -122.f, 5);
    m_delayValue = addLabel("0", 100.f, -122.f, 0.6f);
    addButton(smallBtn("+"), menu_selector(CBFFMenu::onAdjust), 140.f, -122.f, 6);

    this->refresh();
    return true;
}

void CBFFMenu::refresh() {
    auto& s = CBFFSettings::get();
    auto const& c = s.mode(m_mode);

    m_modeLabel->setString(modeName(m_mode));
    m_bufferValue->setString((std::to_string(c.bufferFrames) + "f").c_str());
    m_coyoteValue->setString((std::to_string(c.coyoteFrames) + "f").c_str());
    m_delayValue->setString((std::to_string(c.delayFrames) + "f").c_str());

    // coyote only does something for Cube / Ball / Robot / Spider
    bool coyoteUsed = isGroundJumpMode(m_mode);
    GLubyte op = coyoteUsed ? 255 : 90;
    m_coyoteLabel->setOpacity(op);
    m_coyoteValue->setOpacity(op);

    m_masterToggle->toggle(s.masterEnabled());
    m_modeToggle->toggle(c.enabled);

    for (int i = 0; i < 4; ++i) {
        bool active = s.activePreset() == kPresetOrder[i];
        m_presetSprites[i]->updateBGImage(active ? "GJ_button_01.png" : "GJ_button_04.png");
    }
    m_blurb->setString(CBFFPresetManager::presetBlurb(s.activePreset()));
}

void CBFFMenu::onPreset(CCObject* sender) {
    auto id = static_cast<PresetId>(static_cast<CCNode*>(sender)->getTag());
    CBFFPresetManager::apply(id);
    this->refresh();
}

void CBFFMenu::onMaster(CCObject* sender) {
    // GD togglers report the OLD state inside the callback
    CBFFSettings::get().setMasterEnabled(!static_cast<CCMenuItemToggler*>(sender)->isToggled());
}

void CBFFMenu::onModeStep(CCObject* sender) {
    int step = static_cast<CCNode*>(sender)->getTag();
    int n = static_cast<int>(kModeCount);
    int next = (static_cast<int>(m_mode) + step + n) % n;
    m_mode = static_cast<Mode>(next);
    this->refresh();
}

void CBFFMenu::onModeEnabled(CCObject* sender) {
    auto& s = CBFFSettings::get();
    s.mode(m_mode).enabled = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
    s.markCustom();
    this->refresh();
}

// tags: 1/2 buffer -/+, 3/4 coyote -/+, 5/6 delay -/+
void CBFFMenu::onAdjust(CCObject* sender) {
    int tag = static_cast<CCNode*>(sender)->getTag();
    auto& s = CBFFSettings::get();
    auto& c = s.mode(m_mode);
    int dir = (tag % 2 == 0) ? 1 : -1;

    switch ((tag + 1) / 2) {
        case 1: c.bufferFrames = std::clamp(c.bufferFrames + dir, 0, limits::kMaxBuffer); break;
        case 2:
            if (!isGroundJumpMode(m_mode)) return; // coyote unused for this mode
            c.coyoteFrames = std::clamp(c.coyoteFrames + dir, 0, limits::kMaxCoyote);
            break;
        case 3: c.delayFrames = std::clamp(c.delayFrames + dir, 0, limits::kMaxDelay); break;
        default: return;
    }
    s.markCustom();
    this->refresh();
}

void CBFFMenu::onTune(CCObject*) {
    CBFFTunePopup::create()->show();
}
