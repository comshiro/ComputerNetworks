#include "XMLManager.h"

XMLManager::XMLManager(const std::string& filename) {
    doc.LoadFile(filename.c_str());
}

std::string XMLManager::getArrivals(const std::string& trainId, const std::string& stationName) {
    tinyxml2::XMLElement* trenuri = doc.FirstChildElement("trenuri");
    if (!trenuri) {
        return "Error: Could not find trains data";
    }

    std::string response = "Arrivals for train " + trainId;
    bool trainFound = false;
    bool stationFound = false;

    for (tinyxml2::XMLElement* tren = trenuri->FirstChildElement("tren"); tren != nullptr; tren = tren->NextSiblingElement("tren")) {
        const char* nume = tren->FirstChildElement("nume")->GetText();
        if (std::string(nume) == trainId) {
            trainFound = true;
            tinyxml2::XMLElement* gari = tren->FirstChildElement("gari");
            for (tinyxml2::XMLElement* gara = gari->FirstChildElement("gara"); gara != nullptr; gara = gara->NextSiblingElement("gara")) {
                const char* numeGara = gara->FirstChildElement("numeGara")->GetText();
                const char* sosire = gara->FirstChildElement("sosire")->GetText();
                const char* intarziere = gara->FirstChildElement("intarziere")->GetText();
                if (std::string(numeGara) == stationName) {
                    stationFound = true;
                    response += " at station " + std::string(numeGara) + ":\n";
                    response += "Arrival time: " + std::string(sosire) + "\n";
                    response += "Delay: " + std::string(intarziere) + " minutes";
                    break;
                }
            }
            break;
        }
    }

    if (!trainFound) {
        return "Error: Train " + trainId + " not found";
    }
    if (!stationFound) {
        return "Error: Station " + stationName + " not found for train " + trainId;
    }

    return response;
}

std::string XMLManager::getDepartures(const std::string& trainId, const std::string& stationName) {
    tinyxml2::XMLElement* trenuri = doc.FirstChildElement("trenuri");
    std::string response = "Departures for train " + trainId + " at " + stationName + ": ";
    for (tinyxml2::XMLElement* tren = trenuri->FirstChildElement("tren"); tren != nullptr; tren = tren->NextSiblingElement("tren")) {
        if (std::string(tren->FirstChildElement("nume")->GetText()) == trainId) {
            tinyxml2::XMLElement* gari = tren->FirstChildElement("gari");
            for (tinyxml2::XMLElement* gara = gari->FirstChildElement("gara"); gara != nullptr; gara = gara->NextSiblingElement("gara")) {
                if (std::string(gara->FirstChildElement("numeGara")->GetText()) == stationName) {
                    response += gara->FirstChildElement("plecare")->GetText();
                    response += ", ";
                }
            }
        }
    }
    return response;
}

std::string XMLManager::getRoute(const std::string& trainId) {
    tinyxml2::XMLElement* trenuri = doc.FirstChildElement("trenuri");
    std::string response = "Route for train " + trainId + ": ";
    for (tinyxml2::XMLElement* tren = trenuri->FirstChildElement("tren"); tren != nullptr; tren = tren->NextSiblingElement("tren")) {
        if (std::string(tren->FirstChildElement("nume")->GetText()) == trainId) {
            response += tren->FirstChildElement("ruta")->GetText();
        }
    }
    return response;
}

void XMLManager::announceDelay(const std::string& trainId, const std::string& stationName, int delayMinutes) {
    tinyxml2::XMLElement* trenuri = doc.FirstChildElement("trenuri");
    for (tinyxml2::XMLElement* tren = trenuri->FirstChildElement("tren"); tren != nullptr; tren = tren->NextSiblingElement("tren")) {
        if (std::string(tren->FirstChildElement("nume")->GetText()) == trainId) {
            tinyxml2::XMLElement* gari = tren->FirstChildElement("gari");
            for (tinyxml2::XMLElement* gara = gari->FirstChildElement("gara"); gara != nullptr; gara = gara->NextSiblingElement("gara")) {
                if (std::string(gara->FirstChildElement("numeGara")->GetText()) == stationName) {
                    int currentDelay = std::stoi(gara->FirstChildElement("intarziere")->GetText());
                    currentDelay += delayMinutes;
                    gara->FirstChildElement("intarziere")->SetText(std::to_string(currentDelay).c_str());
                }
            }
        }
    }
    doc.SaveFile("trains.xml");
}