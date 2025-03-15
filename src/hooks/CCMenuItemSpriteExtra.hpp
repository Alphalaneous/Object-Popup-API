#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
#include "../Hover.hpp"

using namespace geode::prelude;

class $modify(HoverEnabledCCMenuItemSpriteExtra, CCMenuItemSpriteExtra) {

	struct Fields {
        std::function<void(CCObject* sender, CCPoint, bool, bool)> m_hoverFunction;
        bool m_inHover;
        CCMenuItemSpriteExtra* m_self;
        ~Fields() {
            if (m_self) {
                Hover::get()->removeHoverEnabledItem(m_self);
            }
        }
	};

    #ifdef GEODE_IS_ANDROID

    void selected() {
        CCMenuItemSpriteExtra::selected();
        CCPoint world = convertToWorldSpace(getContentSize());
        hover(this, world, true);
    }

    void unselected() {
        CCMenuItemSpriteExtra::unselected();
        CCPoint world = convertToWorldSpace(getContentSize());
        hover(this, world, false);
    }

    #endif

    void enableHover(std::function<void(CCObject* sender, CCPoint, bool, bool)> hoverCallback) {
        auto fields = m_fields.self();
        fields->m_self = static_cast<CCMenuItemSpriteExtra*>(this);
        fields->m_hoverFunction = hoverCallback;
        Hover::get()->addHoverEnabledItem(static_cast<CCMenuItemSpriteExtra*>(this));
    }

	void hover(CCObject* sender, CCPoint point, bool hovering) {
        auto fields = m_fields.self();
        if (fields->m_hoverFunction) {
            fields->m_hoverFunction(sender, point, hovering, (hovering && !fields->m_inHover) || (!hovering && fields->m_inHover));
        }
        fields->m_inHover = hovering;
    }
};
