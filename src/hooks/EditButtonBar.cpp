#include "EditButtonBar.hpp"
#include "EditorUI.hpp"

void MyEditButtonBar::loadFromItems(CCArray* p0, int p1, int p2, bool p3) {
    auto fields = m_fields.self();
    Mod* mod = Mod::get();
    
    if (fields->m_objectIDs.size() == 0) {
        for (CCNode* node : CCArrayExt<CCNode*>(p0)) {
            if (CreateMenuItem* cmi = typeinfo_cast<CreateMenuItem*>(node)) {
                fields->m_objectIDs.push_back(cmi->m_objectID);
            }
        }
    }
    EditButtonBar::loadFromItems(p0, p1, p2, p3);
}