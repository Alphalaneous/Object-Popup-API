#pragma once

#ifdef GEODE_IS_WINDOWS
    #ifdef ALPHALANEOUS_OBJECT_POPUP_API_EXPORTING
        #define OBJECT_POPUP_API_DLL __declspec(dllexport)
    #else
        #define OBJECT_POPUP_API_DLL __declspec(dllimport)
    #endif
    #else
        #define OBJECT_POPUP_API_DLL __attribute__((visibility("default")))
#endif

class OBJECT_POPUP_API_DLL ObjectNames {
private:
    std::unordered_map<int, std::string> m_names{};
    ObjectNames();
public:
    std::string nameForID(int id);
    std::unordered_map<int, std::string> getNames();
    static ObjectNames* get();
};




