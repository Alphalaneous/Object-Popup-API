#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "EditButtonBar.hpp"
#include "../../include/ObjectSelectPopup.hpp"
#include "../../include/ObjectNames.hpp"
#include "CCMenuItemSpriteExtra.hpp"

using namespace geode::prelude;
using namespace std::placeholders;

struct GroupInfo : CCObject {};

class $modify(MyEditorUI, EditorUI) {

	static void onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("EditorUI::init", "hjfod.betteredit");
        (void) self.setHookPriorityAfterPost("EditorUI::init", "alphalaneous.vanilla_pages");
    }

	struct Fields {
		std::map<int, Ref<GameObject>> m_gameObjects{};
		std::unordered_map<int, std::vector<std::pair<int, Ref<GameObject>>>> m_tabObjects{};
	};

    bool init(LevelEditorLayer* editorLayer) {
		if (!EditorUI::init(editorLayer)) return false;
		auto fields = m_fields.self();
		ObjectNames::get();
    	Mod* mod = Mod::get();

		for (MyEditButtonBar* buttonBar : CCArrayExt<MyEditButtonBar*>(m_createButtonBars)) {
			int tab = buttonBar->m_tabIndex;
			if (tab <= -1 || tab >= 13) continue;
			if (!fields->m_tabObjects.contains(tab)) {
				for (int id : buttonBar->m_fields->m_objectIDs) {
					fields->m_tabObjects[tab].push_back({id, createGameObject(id, fields->m_gameObjects)});
				}
			}
		}
		
		return true;
	}

	GameObject* createGameObject(int id, std::map<int, Ref<GameObject>>& gameObjects) {
		if (id == 0) return nullptr;
		if (GameObject* object = gameObjects[id]) {
			return object;
		}
		else {
			CreateMenuItem* cmi = getCreateBtn(id, 0);
			ButtonSprite* buttonSprite = cmi->getChildByType<ButtonSprite*>(0);
			GameObject* obj = buttonSprite->getChildByType<GameObject*>(0);
			gameObjects[id] = obj;
			return obj;
		}
		return nullptr;
	}
};