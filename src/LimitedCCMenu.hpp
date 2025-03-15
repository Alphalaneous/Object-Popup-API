#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class LimitedCCMenu : public CCMenu {

public:
	geode::ScrollLayer* m_scrollLayer;

	static LimitedCCMenu* create();
    bool testLocation(CCPoint point);
	bool ccTouchBegan(CCTouch* touch, CCEvent* event);
};