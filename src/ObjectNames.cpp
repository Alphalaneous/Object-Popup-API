#include <Geode/Geode.hpp>
#include "../include/ObjectNames.hpp"

using namespace geode::prelude;

ObjectNames::ObjectNames() {

    std::filesystem::path objectNames = Mod::get()->getResourcesDir() / "objects.csv";
    if (!std::filesystem::exists(objectNames)) return;

    std::ifstream inputFile(objectNames);

    std::string value;

    if (inputFile.is_open()) {
        while (getline(inputFile, value)) {
            std::vector<std::string> split = utils::string::split(value, ",");
            if (split.size() == 2) {
                auto resID = utils::numFromString<int>(split[0]);
                if (resID.isOk()) {
                    if (!split[1].empty()) m_names[resID.unwrap()] = split[1];
                    else m_names[resID.unwrap()] = "Unknown";
                }
            }
        }
        inputFile.close();
    }
}

std::string ObjectNames::nameForID(int id) {
    if (m_names.contains(id) && !m_names[id].empty()) return m_names[id];
    return "Unknown";
}

std::unordered_map<int, std::string> ObjectNames::getNames() {
    return m_names;
}

ObjectNames* ObjectNames::get() {
    static ObjectNames* instance = nullptr;
    if (!instance) {
        instance = new ObjectNames();
    }
    return instance;
}
