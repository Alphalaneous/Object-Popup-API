#include "Hover.hpp"
#include "hooks/CCMenuItemSpriteExtra.hpp"
#include "../include/ObjectSelectPopup.hpp"

void Hover::addHoverEnabledItem(CCMenuItemSpriteExtra* item) {
    if(std::find(m_hoverEnabledItems.begin(), m_hoverEnabledItems.end(), item) == m_hoverEnabledItems.end()) {
        m_hoverEnabledItems.push_back(item);
    }
}

bool Hover::isNodeVisible(CCNode* node) {
    if (!node) {
        return false;
    }

    CCNode* currentNode = node;
    while (currentNode) {
        if (!currentNode->isVisible()) {
            return false;
        }
        if (!currentNode->getParent()) {
            return currentNode != node;
        }
        currentNode = currentNode->getParent();
    }

    return false; 
}

void Hover::removeHoverEnabledItem(CCMenuItemSpriteExtra* item) {
    auto it = std::find(m_hoverEnabledItems.begin(), m_hoverEnabledItems.end(), item);
    if (it != m_hoverEnabledItems.end()) m_hoverEnabledItems.erase(it);
}

void Hover::updateHover(CCPoint point) {
    CCPoint local;
    CCMenu* currentParent = nullptr;
    LimitedCCMenu* currentLimitedCCMenu = nullptr;
    bool currentParentVisible  = false;
  
    for (CCMenuItemSpriteExtra* item : m_hoverEnabledItems) {
        HoverEnabledCCMenuItemSpriteExtra* hoverItem = static_cast<HoverEnabledCCMenuItemSpriteExtra*>(item);
        CCMenu* parent = static_cast<CCMenu*>(item->getParent());
        if (parent != currentParent) {
            currentParent = parent;
            currentParentVisible = parent && parent->isVisible();
            if (currentParentVisible) {
                local = parent->convertToNodeSpace(point);
            }
            currentLimitedCCMenu = typeinfo_cast<LimitedCCMenu*>(currentParent);
        }
        if (currentParentVisible && isNodeVisible(hoverItem)) {
            if (currentLimitedCCMenu) {
                hoverItem->hover(item, point, item->boundingBox().containsPoint(local) && currentLimitedCCMenu->testLocation(point));
            }
            else {
                hoverItem->hover(item, point, item->boundingBox().containsPoint(local));
            }
        }
    }
}

void Hover::onMouseMove(CCPoint point) {
    updateHover(point);
}

void Hover::update(float dt) {
    CCPoint mousePos = getMousePos();
    if (mousePos != m_lastMousePos) {
        m_lastMousePos = mousePos;
        onMouseMove(mousePos);
    }
}

Hover* Hover::create() {
    auto ret = new Hover();
    ret->autorelease();
    return ret;
}

Hover* Hover::get() {
    static Hover* instance = nullptr;
    if (!instance) {
        instance = Hover::create();
        instance->retain();
    }
    return instance;
}

#ifdef GEODE_IS_DESKTOP

$execute {
    Loader::get()->queueInMainThread([]{
        CCScheduler::get()->scheduleUpdateForTarget(Hover::get(), INT_MAX, false);
    });
}

#endif