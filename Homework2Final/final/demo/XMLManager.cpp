#include "XMLManager.h"

XMLManager::XMLManager(const std::string& filename) {
    doc.LoadFile(filename.c_str());
}

std::string XMLManager::getArrivals(const std::string& trainId, const std::string& stationName) {
    tinyxml2::XMLElement* trenuri = doc.FirstChildElement("trenuri");
    std::string response = "Arrivals for train " + trainId + " at " + stationName + ": ";
    for (tinyxml2::XMLElement* tren = trenuri->FirstChildElement("tren"); tren != nullptr; tren = tren->NextSiblingElement("tren")) {
        if (std::string(tren->FirstChildElement("nume")->GetText()) == trainId) {
            tinyxml2::XMLElement* gari = tren->FirstChildElement("gari");
            for (tinyxml2::XMLElement* gara = gari->FirstChildElement("gara"); gara != nullptr; gara = gara->NextSiblingElement("gara")) {
                if (std::string(gara->FirstChildElement("numeGara")->GetText()) == stationName) {
                    response += gara->FirstChildElement("sosire")->GetText();
                    response += ", ";
                }
            }
        }
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