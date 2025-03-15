#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ScrollLayerPro : public geode::ScrollLayer {

protected:
    std::vector<Ref<CCMenuItem>> m_buttons{};
    std::vector<Ref<CCNode>> m_rows{};
    std::function<void(bool)> m_dragCallback;
    float m_rowHeight;
    int m_rowCount;
    bool m_buttonsDisabled;
    float m_positionY;

    ScrollLayerPro(cocos2d::CCRect const& rect, std::function<void(bool)>, bool scrollWheelEnabled, bool vertical);

public:
    void addButtons(std::vector<CCMenuItem*> buttons);
    void addRows(std::vector<CCNode*> nodes, float rowHeight, int visibleRowCount);
    void setButtonsEnabled(bool on);
    void updateRowVisibility();
    void ccTouchMoved(cocos2d::CCTouch* p0, cocos2d::CCEvent* p1) override;
    void ccTouchEnded(cocos2d::CCTouch* p0, cocos2d::CCEvent* p1) override;
    void listenForPosChange(float dt);

    static ScrollLayerPro* create(
            cocos2d::CCRect const& rect, std::function<void(bool)>, bool scrollWheelEnabled = true, bool vertical = true
    );
    static ScrollLayerPro* create(
        cocos2d::CCSize const& size, std::function<void(bool)>, bool scrollWheelEnabled = true, bool vertical = true
    );
};