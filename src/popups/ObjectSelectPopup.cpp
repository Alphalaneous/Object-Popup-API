#include "../../include/ObjectSelectPopup.hpp"
#include "../hooks/EditorUI.hpp"
#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include <Geode/external/fts/fts_fuzzy_match.h>
#include "../LimitedCCMenu.hpp"
#include "../hooks/CCMenuItemSpriteExtra.hpp"
#include "../ScrollLayerPro.hpp"

ObjectSelectPopup* ObjectSelectPopup::s_instance = nullptr;

bool ObjectSelectPopup::init(EditorUI* editorUI, std::string title, bool multiSelect){
  
    m_editorUI = editorUI;
    m_multiSelect = multiSelect;

    if (!Popup<>::initAnchored(300.f, 240.f, "geode.loader/GE_square02.png")) return false;

    m_backgroundSprite = CCSprite::create("rounded-slot.png"_spr);
    m_overlaySprite = CCSprite::create("rounded-slot-overlay.png"_spr);
    m_highlightSprite = CCSprite::create("rounded-slot-highlight.png"_spr);

    setTitle(title);

    m_tabsMenu = CCMenu::create();
    m_tabsMenu->setID("tabs-menu");
    m_tabsMenu->setContentSize({90, 330});
    m_tabsMenu->setLayout(
        ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setCrossAxisOverflow(false)
            ->setGrowCrossAxis(true)
            ->setGap(5.f)
    );
    m_tabsMenu->setScale(0.5f);
    m_tabsMenu->setAnchorPoint({1, 1});

    m_mainLayer->addChildAtPosition(
        m_tabsMenu,
        Anchor::TopLeft,
        { -2.f, -30.f }
    );

    m_tooltip = CCNode::create();
    m_tooltip->setID("tooltip");
    m_tooltipBG = CCScale9Sprite::create("square02_001.png");
    m_tooltipBG->setOpacity(225);
    m_tooltipBG->setAnchorPoint({0, 0});
    m_tooltipBG->setZOrder(-11);
    m_tooltipBG->setID("background");
    m_tooltip->setVisible(false);
    m_tooltip->setAnchorPoint({0, 0});
    m_tooltipText = CCLabelBMFont::create("", "chatFont.fnt");
    m_tooltipText->setAnchorPoint({0.f, 1.f});
    m_tooltipText->setID("tooltip-label");
    m_tooltipText->setScale(0.75);
    m_tooltipText->setPositionX(5);
    m_tooltipObjID = CCLabelBMFont::create("", "chatFont.fnt");
    m_tooltipObjID->setAnchorPoint({0.f, 0.f});
    m_tooltipObjID->setID("object-id-label");
    m_tooltipObjID->setColor({0, 255, 75});
    m_tooltipObjID->setScale(0.5);
    m_tooltipObjID->setPositionX(5);

    m_tooltip->addChild(m_tooltipText);
    m_tooltip->addChild(m_tooltipBG);
    m_tooltip->addChild(m_tooltipObjID);
    m_tooltip->setScale(Mod::get()->getSettingValue<float>("tooltip-scale"));

    addChild(m_tooltip);

    m_searchBar = CCNode::create();
    m_searchBar->setID("search-bar");
    m_searchBar->setAnchorPoint({0.5f, 0.5f});
    m_searchBar->setContentSize({290, 30});
    m_searchBar->ignoreAnchorPointForPosition(false);
    float scale = 0.70f;

    m_searchInput = geode::TextInput::create((m_searchBar->getContentWidth() - 50) / scale, "Search", "bigFont.fnt");
    m_searchInput->setTextAlign(TextInputAlign::Left);
    m_searchInput->setScale(scale);
    m_searchInput->setID("search-input");
    m_searchInput->setPosition(m_searchBar->getContentSize()/2);
    m_searchInput->setPositionX(m_searchInput->getPositionX() - 18);
    m_searchInput->setCallback([this] (std::string str) {
        Mod::get()->setSavedValue("search-query", str);
        setTooltipVisible(false);
        generateList(-2, str, true);
    });
    m_mainLayer->addChildAtPosition(
        m_searchBar,
        Anchor::Top,
        { 0.f, -45.f }
    );
    m_searchBar->addChild(m_searchInput);

    CCMenuItemSpriteExtra* clearSearchBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_longBtn07_001.png"), this, menu_selector(ObjectSelectPopup::clearSearch));
    clearSearchBtn->setPositionX(m_searchInput->getPositionX() + m_searchInput->getScaledContentWidth()/2 + 21);
    clearSearchBtn->setPositionY(m_searchInput->getPositionY());
    clearSearchBtn->setScale(0.7f);
    clearSearchBtn->m_baseScale = 0.7f;
    clearSearchBtn->setID("clear-search-btn");

    CCMenu* clearBtnMenu = CCMenu::create();
    clearBtnMenu->setContentSize(clearSearchBtn->getScaledContentSize());
    clearBtnMenu->setPositionX(m_searchInput->getPositionX() + m_searchInput->getScaledContentWidth()/2 + 21);
    clearBtnMenu->setPositionY(m_searchInput->getPositionY());
    clearBtnMenu->ignoreAnchorPointForPosition(false);
    clearBtnMenu->setID("clear-btn-menu");

    clearSearchBtn->setPosition(clearBtnMenu->getContentSize()/2);

    clearBtnMenu->addChild(clearSearchBtn);
    m_searchBar->addChild(clearBtnMenu);

    CCMenuItemSpriteExtra* tab1Btn = createTabButton("square_01_001.png", menu_selector(ObjectSelectPopup::onTab), 0);
    m_tabsMenu->addChild(tab1Btn);
    CCMenuItemSpriteExtra* tab2Btn = createTabButton("blockOutline_01_001.png", menu_selector(ObjectSelectPopup::onTab), 1);
    m_tabsMenu->addChild(tab2Btn);
    CCMenuItemSpriteExtra* tab3Btn = createTabButton("triangle_a_02_001.png", menu_selector(ObjectSelectPopup::onTab), 2);
    m_tabsMenu->addChild(tab3Btn);
    CCMenuItemSpriteExtra* tab4Btn = createTabButton("spike_01_001.png", menu_selector(ObjectSelectPopup::onTab), 3);
    m_tabsMenu->addChild(tab4Btn);
    CCMenuItemSpriteExtra* tab5Btn = createTabButton("persp_outline_01_001.png", menu_selector(ObjectSelectPopup::onTab), 4);
    m_tabsMenu->addChild(tab5Btn);
    CCMenuItemSpriteExtra* tab6Btn = createTabButton("ring_01_001.png", menu_selector(ObjectSelectPopup::onTab), 5);
    m_tabsMenu->addChild(tab6Btn);
    CCMenuItemSpriteExtra* tab7Btn = createTabButton("GJBeast01_01_001.png", menu_selector(ObjectSelectPopup::onTab), 6);
    m_tabsMenu->addChild(tab7Btn);
    CCMenuItemSpriteExtra* tab8Btn = createTabButton("pixelb_03_01_001.png", menu_selector(ObjectSelectPopup::onTab), 7);
    m_tabsMenu->addChild(tab8Btn);
    CCMenuItemSpriteExtra* tab9Btn = createTabButton("pixelitem_001_001.png", menu_selector(ObjectSelectPopup::onTab), 8);
    m_tabsMenu->addChild(tab9Btn);
    CCMenuItemSpriteExtra* tab10Btn = createTabButton("particle_01_001.png", menu_selector(ObjectSelectPopup::onTab), 9);
    m_tabsMenu->addChild(tab10Btn);
    CCMenuItemSpriteExtra* tab11Btn = createTabButton("d_spikes_01_001.png", menu_selector(ObjectSelectPopup::onTab), 10);
    m_tabsMenu->addChild(tab11Btn);
    CCMenuItemSpriteExtra* tab12Btn = createTabButton("sawblade_02_001.png", menu_selector(ObjectSelectPopup::onTab), 11);
    m_tabsMenu->addChild(tab12Btn);
    CCMenuItemSpriteExtra* tab13Btn = createTabButton("edit_eTintCol01Btn_001.png", menu_selector(ObjectSelectPopup::onTab), 12);
    m_tabsMenu->addChild(tab13Btn);
    CCMenuItemSpriteExtra* tab14Btn = createTabButton("search.png"_spr, menu_selector(ObjectSelectPopup::onTab), -2);
    m_tabsMenu->addChild(tab14Btn);

    m_tabsMenu->updateLayout();

    int tabIndex = Mod::get()->getSavedValue<int>("tab-index", 0);
    std::string searchQuery = Mod::get()->getSavedValue<std::string>("search-query");
    m_searchInput->setString(searchQuery, false);
    generateList(tabIndex, searchQuery, true);
    static_cast<CCMenuItemSpriteExtra*>(m_tabsMenu->getChildByTag(tabIndex))->setOpacity(255);

    return true;
}

void ObjectSelectPopup::setTooltipText(std::string text, int id) {
    if (m_tooltipText) {
        if (id == 0) {
            m_tooltipText->setString("");
            return;
        }
        m_tooltipText->setString(text.c_str());
        id = std::abs(id);
        m_tooltipObjID->setString(fmt::format("(#{})", id).c_str());

        float scaleFactor = 4.f;

        m_tooltip->setContentSize({m_tooltipText->getScaledContentSize().width + 10, m_tooltipText->getScaledContentSize().height + m_tooltipObjID->getScaledContentSize().height + 5});
        m_tooltipBG->setContentSize(m_tooltip->getContentSize() * scaleFactor);
        m_tooltipBG->setScale(1 / scaleFactor);

        m_tooltipText->setPositionY(m_tooltip->getContentSize().height - 2);
        m_tooltipObjID->setPositionY(3);
    }
}

void ObjectSelectPopup::setTooltipPosition(CCPoint point) {

	CCSize winSize = CCDirector::get()->getWinSize();

    if (m_tooltip->getScaledContentSize().width + point.x > winSize.width) {
        point.x = winSize.width - m_tooltip->getScaledContentSize().width;
    }

    if (m_tooltip) m_tooltip->setPosition(point);
}


void ObjectSelectPopup::setTooltipVisible(bool visible) {
    if (std::string_view(m_tooltipText->getString()).empty()) return;
    if (!m_queueVisible && visible) {
        m_queueVisible = true;
        if (m_tooltip) {
            m_tooltip->retain();
            queueInMainThread([this] {
                if (!m_isDraggingScroll) m_tooltip->setVisible(true);
                m_tooltip->release();
                m_queueVisible = false;
            });
        }
    }
    
    if (m_tooltip) m_tooltip->setVisible(visible);
}

CCMenuItemSpriteExtra* ObjectSelectPopup::createTabButton(std::string spriteName, cocos2d::SEL_MenuHandler callback, int tag, float scale) {
    CCSprite* spr = CCSprite::createWithSpriteFrameName(spriteName.c_str());
    if (!spr || spr->getUserObject("geode.texture-loader/fallback")) {
        spr = CCSprite::create(spriteName.c_str());
    }
    CCSprite* buttonSpr = CCSprite::create("GJ_button_04.png");

    spr->setPosition(buttonSpr->getContentSize() / 2);

    float sprMax = std::max(spr->getContentSize().width, spr->getContentSize().height);

    spr->setScale(((40 / sprMax) * 0.7f) * scale);

    buttonSpr->setCascadeColorEnabled(true);
    buttonSpr->setCascadeOpacityEnabled(true);

    buttonSpr->addChild(spr);
    
    CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(buttonSpr, this, callback);
    btn->setTag(tag);
    btn->setCascadeColorEnabled(true);
    btn->setCascadeOpacityEnabled(true);
    btn->setOpacity(127);

    return btn;
}

void ObjectSelectPopup::onTab(CCObject* obj) {
    if (obj->getTag() == m_tab) return;
    generateList(obj->getTag());
    selectObjects(m_selectedIDs);
    for (CCMenuItemSpriteExtra* btn : CCArrayExt<CCMenuItemSpriteExtra*>(m_tabsMenu->getChildren())) {
        btn->setOpacity(127);
    }
    static_cast<CCMenuItemSpriteExtra*>(obj)->setOpacity(255);
}

float ObjectSelectPopup::getScroll() {
    return m_scrollLayer->m_contentLayer->getPositionY();
}

void ObjectSelectPopup::setScroll(float scroll) {
    float difference = -(m_scrollLayer->m_contentLayer->getContentHeight() - m_scrollLayer->getContentHeight());
    if (scroll < difference) {
        scroll = difference;
    }
    m_scrollLayer->m_contentLayer->setPositionY(scroll);
}

void ObjectSelectPopup::clearSearch(CCObject* sender) {
	if (m_searchInput) m_searchInput->setString("", true);
}

void ObjectSelectPopup::generateList(int tab, std::string query, bool reset){
    m_isDraggingScroll = false;

    for (auto& [k, v] : m_searchButtons) {
        v->removeFromParentAndCleanup(false);
    }

    if (tab == m_tab && !reset) return;
    if (!reset) if (m_searchInput) m_searchInput->setString("", false);
    m_tab = tab;
    Mod::get()->setSavedValue("tab-index", m_tab);

    MyEditorUI* myEditorUI = static_cast<MyEditorUI*>(m_editorUI);

    auto fields = myEditorUI->m_fields.self();
    m_buttons.clear();

    CCSize contentSize = { m_mainLayer->getContentSize().width - 35.f, m_mainLayer->getContentSize().height - 50.f };
    float heightOffset = 0;
    int rowOffset = 0;

    if (tab == -2) {
        if (m_searchButtons.empty()) {
            for (auto& [k, v] : fields->m_gameObjects) {
                CCMenuItemSpriteExtra* btn = createObjectButton(k, fields->m_gameObjects);
                m_searchButtons[k] = btn;
            }
        }
        m_searchBar->setVisible(true);
        if (!query.empty()) {

		    std::vector<NameData> nameScores{};

            for (auto& [k, v] : ObjectNames::get()->getNames()) {
			    int score = 0;
                if (!query.empty() && !fts::fuzzy_match(query.c_str(), v.c_str(), score)) continue;
                std::string lowerV = utils::string::toLower(v);
                std::string lowerQuery = utils::string::toLower(query);
                //we still want the results to make some sense
                if (utils::string::contains(lowerV, lowerQuery)) { 
			        nameScores.push_back({k, v, score});
                }
            }

            if (!nameScores.empty()) {
                std::sort(nameScores.begin(), nameScores.end(), [&](const auto& a, const auto& b) {
                    return a.score > b.score;
                });

                for (auto& [k, v] : fields->m_gameObjects) {
                    for (const auto& nameData : nameScores) {
                        if (k == nameData.id) {
                            m_buttons.push_back(m_searchButtons[k]);
                        }
                    }
                }
            }
        }
        else {
            query = Mod::get()->getSavedValue<std::string>("search-query");
            if (query.empty()) {
                for (auto& [k, v] : fields->m_gameObjects) {
                    m_buttons.push_back(m_searchButtons[k]);
                }
            }
            else {
                m_searchInput->setString(query, true);
            }
        }
        heightOffset = 30 * ObjectSelectPopup::s_scaleMult;
        rowOffset = 1;
        m_searchInput->focus();
        m_searchInput->getInputNode()->m_textField->m_uCursorPos = m_searchInput->getString().size();
        m_searchInput->getInputNode()->updateBlinkLabelToChar(m_searchInput->getInputNode()->m_textField->m_uCursorPos);
    }
    else {
        m_searchButtons.clear();
        m_searchBar->setVisible(false);
        std::vector<std::pair<int, GameObject*>> objects(fields->m_tabObjects[tab].begin(), fields->m_tabObjects[tab].end());
        for (auto& [k, v] : objects) {
            CCMenuItemSpriteExtra* btn = createObjectButton(k, fields->m_gameObjects);
            m_buttons.push_back(btn);
        }
    }

    m_mainLayer->removeChildByID("object-list-scrollbar");

    if (m_scrollLayer) {
        m_scrollLayer->removeFromParent();
    }
    float gap = 2.5f;

    CCNode* container = CCNode::create();
    container->setContentSize(contentSize);
    container->setPosition({0, 0});
    container->setLayout(
        ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(contentSize.height)
            ->setCrossAxisOverflow(false)
            ->setGrowCrossAxis(true)
            ->setGap(gap)
    );

    m_scrollLayer = ScrollLayerPro::create({contentSize.width, contentSize.height - heightOffset}, [this] (bool dragging) {
        m_isDraggingScroll = dragging;
        if (dragging) setTooltipVisible(false);
    });

    std::vector<CCNode*> rows;

    int itemsPerRow = 10;
    int i = 0;
    float center = (30 * ObjectSelectPopup::s_scaleMult) / 2.f;
    while (true) {
        if (m_buttons.size() == 0) break;
        LimitedCCMenu* row = LimitedCCMenu::create();
        row->m_scrollLayer = m_scrollLayer;
        row->setContentSize({contentSize.width, 30 * ObjectSelectPopup::s_scaleMult});
        bool shouldBreak = false;
        float posX = 0;
        for (int j = 0; j < itemsPerRow; j++) {
            int pos = i * itemsPerRow + j;
            if (pos >= m_buttons.size()) {
                shouldBreak = true;
                break;
            }
            posX += 30 * ObjectSelectPopup::s_scaleMult;
            HoverEnabledCCMenuItemSpriteExtra* btn = static_cast<HoverEnabledCCMenuItemSpriteExtra*>(m_buttons[pos]);
            btn->setScale(0.935f);
            btn->m_baseScale = 0.935f;
            btn->setPosition({posX - center, center});
            row->addChild(btn);
            posX += gap;
        }
        if (row->getChildrenCount() != 0) {
            rows.push_back(row);
            container->addChild(row);
        }
        i++;
        if (shouldBreak) break;
    }

    container->ignoreAnchorPointForPosition(true);
    container->updateLayout();
    
    ScrollLayerPro* scrollLayerPro = static_cast<ScrollLayerPro*>(m_scrollLayer);

    scrollLayerPro->addButtons(m_buttons);
    scrollLayerPro->setAnchorPoint({0, 0});
    scrollLayerPro->addRows(rows, 30 * ObjectSelectPopup::s_scaleMult, 8 - rowOffset);
    scrollLayerPro->setID("object-list");
    scrollLayerPro->ignoreAnchorPointForPosition(false);
    scrollLayerPro->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(contentSize.height)
            ->setGap(5.0f)
    );
    scrollLayerPro->m_contentLayer->setContentSize(container->getContentSize());
    scrollLayerPro->m_contentLayer->addChild(container);
    scrollLayerPro->scrollToTop();

    class ScrollbarProMax : public Scrollbar {
    public:
        void drawProMax() {
            draw();
        }
    };

    Scrollbar* scrollbar = Scrollbar::create(scrollLayerPro);
    static_cast<ScrollbarProMax*>(scrollbar)->drawProMax();
    scrollbar->setID("object-list-scrollbar");
    scrollbar->setAnchorPoint({1, 0});

    m_mainLayer->addChildAtPosition(
        scrollLayerPro,
        Anchor::BottomLeft,
        { 12.0f, 12.0f }
    );

    m_mainLayer->addChildAtPosition(
        scrollbar,
        Anchor::BottomRight,
        { -10.f, 12.f }
    );

    handleTouchPriority(this);

    queueInMainThread([this] {
        if (auto delegate = typeinfo_cast<CCTouchDelegate*>(m_scrollLayer)) {
            if (auto handler = CCTouchDispatcher::get()->findHandler(delegate)) {
                CCTouchDispatcher::get()->setPriority(handler->getPriority() - 1, handler->getDelegate());
            }
        }
    });
}

void ObjectSelectPopup::onObjectButtonHover(CCObject* sender, CCPoint point, bool hovering, bool isStart) {
    HoverEnabledCCMenuItemSpriteExtra* btn = static_cast<HoverEnabledCCMenuItemSpriteExtra*>(sender);

    if (isStart && hovering) {
        std::string name;
        if (btn->getTag() < 0) {
            name = "Custom Object";
        }
        else if (btn->getTag() > 0) {
            name = ObjectNames::get()->nameForID(btn->getTag());
        }
        setTooltipText(name, btn->getTag());
        setTooltipVisible(true);
        btn->getChildByID("highlight")->setVisible(true);
    }
    if (isStart && !hovering) {
        setTooltipVisible(false);
        btn->getChildByID("highlight")->setVisible(false);
    }

    if (hovering) {
        setTooltipPosition(point);
    }
}

void ObjectSelectPopup::setSelectCallback(std::function<void(CCMenuItem* btn, int, bool)> selectCallback) {
    m_selectCallback = selectCallback;
}

void ObjectSelectPopup::setCloseCallback(std::function<void()> closeCallback) {
    m_closeCallback = closeCallback;
}

void ObjectSelectPopup::clearSelection() {
    m_selectedIDs.clear();
    for (CCMenuItem* btn : m_buttons) {
        CCSprite* overlay = static_cast<CCSprite*>(btn->getChildByID("slot-overlay"));
        overlay->setVisible(false);
    }
}

void ObjectSelectPopup::selectObject(int id, bool doCallback) {

    if (!m_multiSelect) {
        m_selectedIDs.clear();
        m_selectedIDs.insert(id);
        for (CCMenuItem* btn : m_buttons) {
            CCSprite* overlay = static_cast<CCSprite*>(btn->getChildByID("slot-overlay"));
            overlay->setVisible(btn->getTag() == id);
            if (doCallback) m_selectCallback(btn, id, btn->getTag() == id);
        }
    }
    else {
        m_selectedIDs.insert(id);
        for (CCMenuItem* btn : m_buttons) {
            CCSprite* overlay = static_cast<CCSprite*>(btn->getChildByID("slot-overlay"));
            if (id == btn->getTag()) {
                overlay->setVisible(true);
                if (doCallback) m_selectCallback(btn, id, true);
            }
        }
    }
}

void ObjectSelectPopup::selectObjects(std::set<int> objects, bool doCallback) {
    for (int id : objects) {
        selectObject(id, doCallback);
    }
}

void ObjectSelectPopup::onObjectButton(CCObject* sender) {
    int id = sender->getTag();

    MyEditorUI* myEditorUI = static_cast<MyEditorUI*>(EditorUI::get());

    HoverEnabledCCMenuItemSpriteExtra* btn = static_cast<HoverEnabledCCMenuItemSpriteExtra*>(sender);
    CCSprite* overlay = static_cast<CCSprite*>(btn->getChildByID("slot-overlay"));

    if (!m_multiSelect) {
        m_selectedIDs.clear();
        for (CCMenuItem* btn : m_buttons) {
            CCSprite* overlay2 = static_cast<CCSprite*>(btn->getChildByID("slot-overlay"));
            overlay2->setVisible(false);
        }
        m_selectedIDs.insert(id);
        overlay->setVisible(true);
    }
    else {
        if (overlay->isVisible()) {
            m_selectedIDs.erase(id);
            overlay->setVisible(false);
        }
        else {
            m_selectedIDs.insert(id);
            overlay->setVisible(true);
        }
    }

    if (m_selectCallback) m_selectCallback(btn, id, overlay->isVisible());
}

std::set<int> ObjectSelectPopup::getSelectedIDs() {
    return m_selectedIDs;
}

CCMenuItemSpriteExtra* ObjectSelectPopup::createObjectButton(int id, std::map<int, Ref<GameObject>>& gameObjects) {
    Mod* mod = Mod::get();
    MyEditorUI* myEditorUI = static_cast<MyEditorUI*>(EditorUI::get());

    CCNode* objectContainer = CCNode::create();
    CCSprite* slotSprite = CCSprite::createWithTexture(m_backgroundSprite->getTexture());
    CCSprite* slotOverlay = CCSprite::createWithTexture(m_overlaySprite->getTexture());
    CCSprite* highlight = CCSprite::createWithTexture(m_highlightSprite->getTexture());

    slotSprite->setZOrder(-10);
    slotOverlay->setZOrder(10);
    slotOverlay->setVisible(false);
    slotOverlay->setID("slot-overlay");
    
    ccColor3B overlayColor = mod->getSettingValue<ccColor3B>("selection-color");
    if (m_overrideColor) overlayColor = m_overlayColor;
    slotOverlay->setColor(overlayColor);

    highlight->setVisible(false);
    highlight->setID("highlight");
    highlight->setZOrder(-11);
    highlight->setOpacity(127);

    objectContainer->setAnchorPoint({1, 0.5});
    objectContainer->setScale(ObjectSelectPopup::s_scaleMult * 0.7);
    if (GameObject* obj = myEditorUI->createGameObject(id, gameObjects)) {
        objectContainer->addChild(obj);
    }

    objectContainer->setContentSize({40, 40});
    CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(objectContainer, this, menu_selector(ObjectSelectPopup::onObjectButton));
    HoverEnabledCCMenuItemSpriteExtra* hoverBtn = static_cast<HoverEnabledCCMenuItemSpriteExtra*>(btn);
    hoverBtn->enableHover(std::bind(&ObjectSelectPopup::onObjectButtonHover, this, _1, _2, _3, _4));
    objectContainer->setPosition({objectContainer->getPositionX() + 2.75f * objectContainer->getScale(), objectContainer->getPositionY() + 1 * objectContainer->getScale()});
    btn->m_scaleMultiplier = 1;
    btn->addChild(slotSprite);
    btn->addChild(slotOverlay);
    btn->addChild(highlight);
    btn->setContentSize({25.6, 25.6});

    btn->setTag(id);
    slotSprite->setPosition(btn->getContentSize()/2);
    slotSprite->setScale(btn->getContentSize().width / slotSprite->getContentSize().width);
    slotOverlay->setPosition(btn->getContentSize()/2);
    slotOverlay->setScale(btn->getContentSize().width / slotSprite->getContentSize().width);
    highlight->setPosition(btn->getContentSize()/2);
    highlight->setScale(btn->getContentSize().width / slotSprite->getContentSize().width);

    return hoverBtn;
}

void ObjectSelectPopup::setTooltipScale(float scale) {
    m_overrideScale = true;
    m_tooltipScale = scale;
    m_tooltip->setScale(scale);
}

void ObjectSelectPopup::setOverlayColor(ccColor3B color) {
    m_overrideColor = true;
    m_overlayColor = color;
    for (CCMenuItem* btn : m_buttons) {
        CCSprite* overlay = static_cast<CCSprite*>(btn->getChildByID("slot-overlay"));
        overlay->setColor(color);
    }
}

bool ObjectSelectPopup::isDraggingScroll() {
    return m_isDraggingScroll;
}

bool ObjectSelectPopup::setup() {
    return true;
}

void ObjectSelectPopup::onClose(cocos2d::CCObject* sender){
    if(m_closeCallback) m_closeCallback();
    Popup::onClose(sender);
}

ObjectSelectPopup* ObjectSelectPopup::get() {
    return s_instance;
}

ObjectSelectPopup::~ObjectSelectPopup() {
    s_instance = nullptr;
}

ObjectSelectPopup* ObjectSelectPopup::create(EditorUI* editorUI, std::string title, bool multiSelect) {

    auto ret = new ObjectSelectPopup();
    if (ret->init(editorUI, title, multiSelect)) {
        ret->autorelease();
        s_instance = ret;
        return ret;
    }

    delete ret;
    return nullptr;
}