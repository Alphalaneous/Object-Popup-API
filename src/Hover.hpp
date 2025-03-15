#pragma once

#include <Geode/Geode.hpp>
#include "LimitedCCMenu.hpp"

using namespace geode::prelude;

class Hover : public CCObject {
public:
    std::vector<CCMenuItemSpriteExtra*> m_hoverEnabledItems;
    CCPoint m_lastMousePos;

    void addHoverEnabledItem(CCMenuItemSpriteExtra* item);
    bool isNodeVisible(CCNode* node);
    void removeHoverEnabledItem(CCMenuItemSpriteExtra* item);
    void updateHover(CCPoint point);
    void onMouseMove(CCPoint point);
    void update(float dt);
    static Hover* create();
    static Hover* get();
};