// Adds the CBFF button to the main menu and the pause menu.
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "CBFFMenu.hpp"

using namespace geode::prelude;

static CCMenuItemSpriteExtra* makeCBFFButton(CCObject* target, SEL_MenuHandler cb) {
    auto label = CCLabelBMFont::create("CBFF", "bigFont.fnt");
    label->setScale(0.55f);
    auto spr = CircleButtonSprite::create(label, CircleBaseColor::Green, CircleBaseSize::Medium);
    auto btn = CCMenuItemSpriteExtra::create(spr, target, cb);
    btn->setID("cbff-button"_spr);
    return btn;
}

class $modify(CBFFMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        auto btn = makeCBFFButton(this, menu_selector(CBFFMenuLayer::onCBFF));
        if (auto menu = this->getChildByID("bottom-menu")) {
            menu->addChild(btn);
            menu->updateLayout();
        }
        return true;
    }
    void onCBFF(CCObject*) { CBFFMenu::create()->show(); }
};

class $modify(CBFFPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        auto btn = makeCBFFButton(this, menu_selector(CBFFPauseLayer::onCBFF));
        if (auto menu = this->getChildByID("right-button-menu")) {
            menu->addChild(btn);
            menu->updateLayout();
        } else {
            auto win = CCDirector::get()->getWinSize();
            auto menu = CCMenu::create();
            menu->addChild(btn);
            menu->setPosition({win.width - 30.f, 30.f});
            this->addChild(menu);
        }
    }
    void onCBFF(CCObject*) { CBFFMenu::create()->show(); }
};
