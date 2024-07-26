#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>
#include <mutex>
#include "tinyxml2.h"

struct Slot {
    std::string type;
    int number;
    bool mock;
};

class Config {
public:
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    static Config& getInstance();
    bool loadConfig();
    const std::vector<Slot>& getSlots() const;
    std::string getIpAddress() const;
    int getPort() const;
    

private:
    Config();

    std::string m_filename;
    std::mutex m_mutex;

    std::vector<Slot> m_slots;
    std::string m_ipAddress;
    int m_port;

    void parseSlots(tinyxml2::XMLElement* root);
    void parseNetwork(tinyxml2::XMLElement* root);
};

#endif // CONFIG_H