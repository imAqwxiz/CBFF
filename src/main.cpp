
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// ==========================================
// CBFF STATE
// ==========================================

static bool g_enabled = true;
static std::string g_preset = "Balanced";
static std::string g_gamemode = "All";

// ==========================================
// CBFF POPUP
// ==========================================

class CBFFLayer : public Popup<> {
protected:
    CCLabelBMFont* m_status = nullptr;
    CCMenu* m_buttonMenu = nullptr;

    // --------------------------------------
    // BUTTON CREATION
    // --------------------------------------

    void addButton(
        const char* text,
        SEL_MenuHandler callback,
        float x,
        float y,
        float width = 115.f
    ) {
        auto sprite = ButtonSprite::create(
            text,
            static_cast<int>(width),
            true,
            "goldFont.fnt",
            "GJ_button_01.png",
            30.f,
            0.65f
        );

        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            callback
        );

        button->setPosition({x, y});
        m_buttonMenu->addChild(button);
    }

    // --------------------------------------
    // STATUS
    // --------------------------------------

    void updateStatus(const std::string& text) {
        if (m_status) {
            m_status->setString(text.c_str());
        }
    }

    void selectPreset(const std::string& preset) {
        g_preset = preset;

        updateStatus(
            ("Active preset: " + preset).c_str()
        );
    }

public:

    static CBFFLayer* create() {
        auto ret = new CBFFLayer();

        if (ret && ret->init(360.f, 260.f)) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    // ======================================
    // POPUP INITIALIZATION
    // ======================================

    bool setup() override {

        this->setTitle("CBFF");

        auto size = m_mainLayer->getContentSize();

        const float centerX = size.width / 2.f;

        // Main button menu
        m_buttonMenu = CCMenu::create();
        m_buttonMenu->setPosition({0.f, 0.f});

        m_mainLayer->addChild(m_buttonMenu);

        // ----------------------------------
        // STATUS
        // ----------------------------------

        m_status = CCLabelBMFont::create(
            "Active preset: Balanced",
            "bigFont.fnt"
        );

        m_status->setScale(0.42f);
        m_status->setPosition({
            centerX,
            size.height - 48.f
        });

        m_mainLayer->addChild(m_status);

        // ----------------------------------
        // ENABLE / DISABLE
        // ----------------------------------

        addButton(
            g_enabled ? "CBFF: ON" : "CBFF: OFF",
            menu_selector(CBFFLayer::onToggle),
            centerX,
            size.height - 82.f,
            110.f
        );

        // ----------------------------------
        // PRESETS HEADER
        // ----------------------------------

        auto presetLabel = CCLabelBMFont::create(
            "PRESETS",
            "goldFont.fnt"
        );

        presetLabel->setScale(0.55f);
        presetLabel->setPosition({
            centerX,
            size.height - 113.f
        });

        m_mainLayer->addChild(presetLabel);

        // ----------------------------------
        // PRESET BUTTONS
        // ----------------------------------

        const float leftX = centerX - 82.f;
        const float rightX = centerX + 82.f;

        addButton(
            "AGGRESSIVE",
            menu_selector(CBFFLayer::onAggressive),
            leftX,
            size.height - 145.f
        );

        addButton(
            "BALANCED",
            menu_selector(CBFFLayer::onBalanced),
            rightX,
            size.height - 145.f
        );

        addButton(
            "SAFE",
            menu_selector(CBFFLayer::onSafe),
            leftX,
            size.height - 180.f
        );

        addButton(
            "GD GOD",
            menu_selector(CBFFLayer::onGod),
            rightX,
            size.height - 180.f
        );

        addButton(
            "OBVIOUS CHEATING",
            menu_selector(CBFFLayer::onCheating),
            centerX,
            size.height - 215.f,
            165.f
        );

        // ----------------------------------
        // CONFIGURATION
        // ----------------------------------

        addButton(
            "CUSTOMIZE ALL",
            menu_selector(CBFFLayer::onCustomizeAll),
            centerX - 82.f,
            20.f,
            130.f
        );

        addButton(
            "GAMEMODES",
            menu_selector(CBFFLayer::onGamemodes),
            centerX + 82.f,
            20.f,
            115.f
        );

        return true;
    }

    // ======================================
    // CALLBACKS
    // ======================================

    void onToggle(CCObject*) {
        g_enabled = !g_enabled;

        updateStatus(
            g_enabled ? "CBFF enabled" : "CBFF disabled"
        );
    }

    void onAggressive(CCObject*) {
        selectPreset("Aggressive");
    }

    void onBalanced(CCObject*) {
        selectPreset("Balanced");
    }

    void onSafe(CCObject*) {
        selectPreset("Safe");
    }

    void onGod(CCObject*) {
        selectPreset("Genuine GD God");
    }

    void onCheating(CCObject*) {
        selectPreset("Obvious Cheating");
    }

    void onCustomizeAll(CCObject*) {
        g_gamemode = "All";
        updateStatus("Editing all gamemodes");
    }

    void onGamemodes(CCObject*) {
        updateStatus("Gamemode settings");
    }
};

// ==========================================
// MAIN MENU INTEGRATION
// ==========================================

class $modify(CBFFMenuLayer, MenuLayer) {

    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto bottomMenu = this->getChildByID(
            "bottom-menu"
        );

        if (bottomMenu) {

            auto sprite = ButtonSprite::create(
                "CBFF",
                40,
                true,
                "goldFont.fnt",
                "GJ_button_01.png",
                25.f,
                0.55f
            );

            auto button = CCMenuItemSpriteExtra::create(
                sprite,
                this,
                menu_selector(CBFFMenuLayer::onOpen)
            );

            button->setID("cbff-open-button");

            bottomMenu->addChild(button);
            bottomMenu->updateLayout();
        }

        log::info("CBFF initialized!");

        return true;
    }

    void onOpen(CCObject*) {
        CBFFLayer::create()->show();
    }
};

