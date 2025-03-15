#pragma once

#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/cocos.hpp>

#ifdef GEODE_IS_WINDOWS
    #ifdef ALPHALANEOUS_OBJECT_POPUP_API_EXPORTING
        #define OBJECT_POPUP_API_DLL __declspec(dllexport)
    #else
        #define OBJECT_POPUP_API_DLL __declspec(dllimport)
    #endif
    #else
        #define OBJECT_POPUP_API_DLL __attribute__((visibility("default")))
#endif

struct OBJECT_POPUP_API_DLL NameData {
    int id;
    std::string name;
    int score;
};

class OBJECT_POPUP_API_DLL ObjectSelectPopup : public geode::Popup<> {

protected:
    cocos2d::CCMenu* m_tabsMenu;
    bool setup() override;
    bool init(EditorUI* editorUI, std::string title, bool multiSelect);
private:
    EditorUI* m_editorUI;
    geode::ScrollLayer* m_scrollLayer;
    cocos2d::CCNode* m_tooltip;
    cocos2d::extension::CCScale9Sprite* m_tooltipBG;
    cocos2d::CCLabelBMFont* m_tooltipText;
    cocos2d::CCLabelBMFont* m_tooltipObjID;
    cocos2d::CCNode* m_searchBar;
    std::unordered_map<int, geode::Ref<cocos2d::CCMenuItem>> m_searchButtons{};
    geode::TextInput* m_searchInput;
    std::vector<cocos2d::CCMenuItem*> m_buttons;
    std::function<void(cocos2d::CCMenuItem* btn, int, bool)> m_selectCallback;
    std::function<void()> m_closeCallback;
    std::set<int> m_selectedIDs;
    geode::Ref<cocos2d::CCSprite> m_highlightSprite;
    geode::Ref<cocos2d::CCSprite> m_backgroundSprite;
    geode::Ref<cocos2d::CCSprite> m_overlaySprite;
    bool m_multiSelect;
    bool m_overrideColor;
    bool m_overrideScale;
    float m_tooltipScale;
    cocos2d::ccColor3B m_overlayColor;

    int m_tab = -1;
    bool m_isDraggingScroll = false;
    bool m_queueVisible = false;
    void onTab(cocos2d::CCObject* obj);
    void onClear(cocos2d::CCObject* obj);
    void setScroll(float scroll);
    void setTooltipText(std::string text, int id);
    void setTooltipPosition(cocos2d::CCPoint point);
    void setTooltipVisible(bool visible);
    void clearSearch(cocos2d::CCObject* sender);
    bool isDraggingScroll();
    float getScroll();
    CCMenuItemSpriteExtra* createTabButton(std::string spriteName, cocos2d::SEL_MenuHandler callback, int tag, float scale = 1);
    CCMenuItemSpriteExtra* createObjectButton(int id, std::map<int, geode::Ref<GameObject>>& gameObjects);
    void onObjectButtonHover(cocos2d::CCObject* sender, cocos2d::CCPoint point, bool hovering, bool isStart);
    void onObjectButton(cocos2d::CCObject* sender);
    void generateList(int tab, std::string query = "", bool reset = false);
    ~ObjectSelectPopup();
    static ObjectSelectPopup* s_instance;
    static constexpr float s_scaleMult = 0.8f;

public:
    static ObjectSelectPopup* create(EditorUI* editorUI, std::string title, bool multiSelect = false);
    static ObjectSelectPopup* get();
    void onClose(cocos2d::CCObject*) override;
    void setSelectCallback(std::function<void(cocos2d::CCMenuItem* btn, int, bool)>);
    void setCloseCallback(std::function<void()>);
    void clearSelection();
    void selectObjects(std::set<int> objects, bool doCallback = false);
    void selectObject(int id, bool doCallback = false);
    void setTooltipScale(float scale);
    void setOverlayColor(cocos2d::ccColor3B color);
    std::set<int> getSelectedIDs();
};