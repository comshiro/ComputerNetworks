#ifndef XMLMANAGER_H
#define XMLMANAGER_H

#include <string>
#include <tinyxml2.h>

class XMLManager {
public:
    XMLManager(const std::string& filename);
    std::string getArrivals(const std::string& trainId, const std::string& stationName);
    std::string getDepartures(const std::string& trainId, const std::string& stationName);
    std::string getRoute(const std::string& trainId);
    void announceDelay(const std::string& trainId, const std::string& stationName, int delayMinutes);

private:
    tinyxml2::XMLDocument doc;
};

#endif // XMLMANAGER_H