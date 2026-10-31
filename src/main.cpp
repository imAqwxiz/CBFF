
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
// CBFF MENU
// ==========================================

class CBFFLayer : public CCLayerColor {
protected:
    CCLabelBMFont* m_status = nullptr;

    void addTextButton(
        const char* text,
        SEL_MenuHandler callback,
        float x,
        float y,
        float scale = 0.42f
    ) {
        auto label = CCLabelBMFont::create(
            text,
            "bigFont.fnt"
        );

        label->setScale(scale);

        auto item = CCMenuItemLabel::create(
            label,
            this,
            callback
        );

        auto menu = CCMenu::create(item, nullptr);
        menu->setPosition({x, y});

        this->addChild(menu);
    }

    void updateStatus(const std::string& text) {
        if (m_status) {
            m_status->setString(text.c_str());
        }
    }

    void selectPreset(const std::string& preset) {
        g_preset = preset;

        updateStatus(
            ("Selected preset: " + preset).c_str()
        );
    }

public:

    static CBFFLayer* create() {
        auto ret = new CBFFLayer();

        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init() override {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        if (!CCLayerColor::initWithColor(
            ccc4(0, 0, 0, 175)
        )) {
            return false;
        }

        this->setContentSize(winSize);

        // Main panel
        float panelW = std::min(440.f, winSize.width - 24.f);
        float panelH = std::min(310.f, winSize.height - 24.f);

        auto panel = CCLayerColor::create(
            ccc4(20, 35, 59, 250),
            panelW,
            panelH
        );

        panel->setPosition({
            (winSize.width - panelW) / 2.f,
            (winSize.height - panelH) / 2.f
        });

        this->addChild(panel);

        float center = panelW / 2.f;

        // Header
        auto title = CCLabelBMFont::create(
            "CBFF",
            "goldFont.fnt"
        );

        title->setScale(0.9f);
        title->setPosition({center, panelH - 30.f});
        panel->addChild(title);

        auto subtitle = CCLabelBMFont::create(
            "Click beFore FRAMES",
            "bigFont.fnt"
        );

        subtitle->setScale(0.35f);
        subtitle->setPosition({center, panelH - 52.f});
        panel->addChild(subtitle);

        // Status
        m_status = CCLabelBMFont::create(
            "Selected preset: Balanced",
            "bigFont.fnt"
        );

        m_status->setScale(0.38f);
        m_status->setPosition({center, panelH - 82.f});
        panel->addChild(m_status);

        // Enable / Disable
        addTextButton(
            g_enabled ? "CBFF: ON" : "CBFF: OFF",
            menu_selector(CBFFLayer::onToggle),
            center,
            panelH - 112.f
        );

        // Presets
        auto presetTitle = CCLabelBMFont::create(
            "PRESETS",
            "bigFont.fnt"
        );

        presetTitle->setScale(0.45f);
        presetTitle->setPosition({center, panelH - 145.f});
        panel->addChild(presetTitle);

        float left = panelW * 0.28f;
        float right = panelW * 0.72f;

        addTextButton(
            "AGGRESSIVE",
            menu_selector(CBFFLayer::onAggressive),
            left,
            panelH - 178.f
        );

        addTextButton(
            "BALANCED",
            menu_selector(CBFFLayer::onBalanced),
            right,
            panelH - 178.f
        );

        addTextButton(
            "SAFE",
            menu_selector(CBFFLayer::onSafe),
            left,
            panelH - 208.f
        );

        addTextButton(
            "GD GOD",
            menu_selector(CBFFLayer::onGod),
            right,
            panelH - 208.f
        );

        addTextButton(
            "OBVIOUS CHEATING",
            menu_selector(CBFFLayer::onCheating),
            center,
            panelH - 238.f,
            0.36f
        );

        // Configuration buttons
        addTextButton(
            "CUSTOMIZE ALL",
            menu_selector(CBFFLayer::onCustomizeAll),
            left,
            32.f,
            0.36f
        );

        addTextButton(
            "GAMEMODES",
            menu_selector(CBFFLayer::onGamemodes),
            right,
            32.f,
            0.36f
        );

        // Close button
        addTextButton(
            "CLOSE",
            menu_selector(CBFFLayer::onClose),
            center,
            10.f,
            0.38f
        );

        return true;
    }

    // ======================================
    // BUTTON CALLBACKS
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
        updateStatus("Editing: All gamemodes");
    }

    void onGamemodes(CCObject*) {
        updateStatus("Gamemode configuration selected");
    }

    void onClose(CCObject*) {
        this->removeFromParentAndCleanup(true);
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
            auto label = CCLabelBMFont::create(
                "CBFF",
                "goldFont.fnt"
            );

            label->setScale(0.45f);

            auto button = CCMenuItemLabel::create(
                label,
                this,
                menu_selector(CBFFMenuLayer::onOpen)
            );

            button->setID("cbff-open-button"_spr);

            bottomMenu->addChild(button);
            bottomMenu->updateLayout();
        }

        log::info("CBFF initialized.");

        return true;
    }

    void onOpen(CCObject*) {
        auto scene = CCDirector::sharedDirector()
            ->getRunningScene();

        if (!scene) return;

        auto menu = CBFFLayer::create();

        if (menu) {
            scene->addChild(menu, 999);
        }
    }
};

