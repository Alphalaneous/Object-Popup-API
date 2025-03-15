# Object Popup API

An API to show an object popup. This can be used whenever you need to have the user choose objects for any purpose.

It contains the following methods:

## ObjectSelectPopup

```cpp
//create the popup, multiSelect allows for more than one object to be selected
static ObjectSelectPopup* create(EditorUI* editorUI, std::string title, bool multiSelect = false);

//get the popup if one exists
static ObjectSelectPopup* get();

//set a callback for when an object is selected
void setSelectCallback(std::function<void(cocos2d::CCMenuItem* button, int objectID, bool isSelected)>);

//set a callback for when the popup is closed
void setCloseCallback(std::function<void()>);

//clear all selected objects
void clearSelection();

//select a set of objects, set doCallback if you wish for your callback to be ran
void selectObjects(std::set<int> objects, bool doCallback = false);

//select an object, set doCallback if you wish for your callback to be ran
void selectObject(int id, bool doCallback = false);

//overrides this mod's settings and lets you force a tooltip scale
void setTooltipScale(float scale);

//overrides this mod's settings and lets you force a selection overlay color
void setOverlayColor(cocos2d::ccColor3B color);

//gets the set of selected IDs
std::set<int> getSelectedIDs();
```

## ObjectNames

```cpp
//get the ObjectNames singleton
static ObjectNames* get();

//get the object name by ID
std::string nameForID(int id);

//get a map of names to ID
std::unordered_map<int, std::string> getNames();
```