#include "config.h"
#include <iostream>

Config::Config():m_filename("config.xml") {}

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

bool Config::loadConfig() {
    std::lock_guard<std::mutex> lock(m_mutex);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError result = doc.LoadFile(m_filename.c_str());
    if (result != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading XML file: " << doc.ErrorIDToName(result) << std::endl;
        return false;
    }

    tinyxml2::XMLElement* root = doc.RootElement();
    if (!root) {
        std::cerr << "No root element in XML file." << std::endl;
        return false;
    }

    parseSlots(root);
    parseNetwork(root);

    return true;
}

void Config::parseSlots(tinyxml2::XMLElement* root) {
    tinyxml2::XMLElement* slotsElement = root->FirstChildElement("slots");
    if (!slotsElement) {
        std::cerr << "No slots element in XML file." << std::endl;
        return;
        /* Throw exception */ 
    }

    m_slots.clear();

    for (tinyxml2::XMLElement* slot = slotsElement->FirstChildElement(); slot != nullptr; slot = slot->NextSiblingElement()) {
        const char* type = slot->Name();
        int number = slot->IntAttribute("number");
        bool mock = slot->BoolAttribute("mock");

        if (type) {
            m_slots.emplace_back(Slot{type, number, mock});
        }
    }
}
void Config::parseNetwork(tinyxml2::XMLElement* root) {
    tinyxml2::XMLElement* networkElement = root->FirstChildElement("network");
    if (!networkElement) {
        std::cerr << "No network element in XML file." << std::endl;
        return;
    }

    tinyxml2::XMLElement* ipElement = networkElement->FirstChildElement("ip");
    tinyxml2::XMLElement* portElement = networkElement->FirstChildElement("port");

    if (ipElement && portElement) {
        m_ipAddress = ipElement->GetText();
        m_port = portElement->IntText();
    } else {
        std::cerr << "No IP or port element in XML file." << std::endl;
    }
}

const std::vector<Slot>& Config::getSlots() const {
    return m_slots;
}
std::string Config::getIpAddress() const {
    return m_ipAddress;
}

int Config::getPort() const {
    return m_port;
}