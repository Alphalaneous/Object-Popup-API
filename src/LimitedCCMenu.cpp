#include "LimitedCCMenu.hpp"

LimitedCCMenu* LimitedCCMenu::create() {
    auto ret = new LimitedCCMenu();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool LimitedCCMenu::testLocation(CCPoint point) {
    if (m_scrollLayer) {
        CCPoint touchPoint = m_scrollLayer->convertToNodeSpace({point.x + m_scrollLayer->getPositionX(), point.y + m_scrollLayer->getPositionY()});
        if (m_scrollLayer->boundingBox().containsPoint(touchPoint)) {
            return true;
        }
    }
    return false;
}

bool LimitedCCMenu::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    if (!testLocation(touch->getLocation())) {
        return false;
    }
    return CCMenu::ccTouchBegan(touch, event);
}