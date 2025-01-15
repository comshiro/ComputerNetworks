#include "XMLManager.h"
#include <stdexcept>
#include <sstream>
#include <iostream>

XMLManager::XMLManager(const std::string& filename) {
    std::cout << "Attempting to load file: " << filename << std::endl;
    tinyxml2::XMLError result = doc.LoadFile(filename.c_str());
    if (result != tinyxml2::XML_SUCCESS) {
        std::cout << "Failed to load XML file. Error code: " << result << std::endl;
        throw std::runtime_error("Failed to load XML file");
    }
    std::cout << "XML file loaded successfully" << std::endl;
}

std::string XMLManager::getArrivals(const std::string& trainId, const std::string& stationName) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("trenuri");
    if (!root) {
        std::cout << "Root element not found" << std::endl;
        return "Not found";
    }

    for (tinyxml2::XMLElement* train = root->FirstChildElement("tren"); 
         train; 
         train = train->NextSiblingElement("tren")) {
        
        // Check if this is the train we're looking for
        tinyxml2::XMLElement* numeElement = train->FirstChildElement("nume");
        if (!numeElement) {
            std::cout << "Nume element not found" << std::endl;
            continue;
        }
        
        const char* trainName = numeElement->GetText();
        if (!trainName) {
            std::cout << "Train name is null" << std::endl;
            continue;
        }

        std::cout << "Comparing train ID: '" << trainId << "' with XML train name: '" << trainName << "'" << std::endl;
        
        if (trainId != trainName) {
            continue;
        }
        
        std::cout << "Found matching train: " << trainId << std::endl;

        // Get arrivals for all stations if no station specified
        if (stationName == "") {
            std::stringstream ss;
            tinyxml2::XMLElement* gari = train->FirstChildElement("gari");
            if (!gari) continue;

            for (tinyxml2::XMLElement* gara = gari->FirstChildElement("gara");
                 gara;
                 gara = gara->NextSiblingElement("gara")) {
                
                tinyxml2::XMLElement* numeGara = gara->FirstChildElement("numeGara");
                tinyxml2::XMLElement* sosire = gara->FirstChildElement("sosire");
                if (numeGara && sosire) {
                    ss << numeGara->GetText() << ": " << sosire->GetText() << "\n";
                }
            }
            return ss.str();
        }

        // Search through stations for specific station
        tinyxml2::XMLElement* gari = train->FirstChildElement("gari");
        if (!gari) {
            std::cout << "No gari element found" << std::endl;
            continue;
        }

        for (tinyxml2::XMLElement* gara = gari->FirstChildElement("gara");
             gara;
             gara = gara->NextSiblingElement("gara")) {
            
            tinyxml2::XMLElement* numeGara = gara->FirstChildElement("numeGara");
            if (!numeGara) {
                std::cout << "Station name element not found" << std::endl;
                continue;
            }

            const char* stationNameInXml = numeGara->GetText();
            if (!stationNameInXml) {
                std::cout << "Station name is null" << std::endl;
                continue;
            }

            std::cout << "Comparing station: '" << stationName << "' with XML station: '" << stationNameInXml << "'" << std::endl;
            
            if (stationName != stationNameInXml)
                continue;

            // Found the station, get arrival time
            tinyxml2::XMLElement* sosire = gara->FirstChildElement("sosire");
            if (!sosire || std::string(sosire->GetText()) == "-")
                return "No arrival for this station";

            tinyxml2::XMLElement* estimareSosire = gara->FirstChildElement("estimareSosire");
            if (!estimareSosire)
                return sosire->GetText();
            std::stringstream ss;
            ss << "Scheduled: " << sosire->GetText() << ", Estimated: " << estimareSosire->GetText();
            return ss.str();
        }
    }
    return "Train or station not found";
}

std::string XMLManager::getDepartures(const std::string& trainId, const std::string& stationName) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("trenuri");
    if (!root) return "Not found";

    for (tinyxml2::XMLElement* train = root->FirstChildElement("tren");
         train;
         train = train->NextSiblingElement("tren")) {
        
        tinyxml2::XMLElement* numeElement = train->FirstChildElement("nume");
        if (!numeElement || std::string(numeElement->GetText()) != trainId)
            continue;

        tinyxml2::XMLElement* gari = train->FirstChildElement("gari");
        if (!gari) continue;

        for (tinyxml2::XMLElement* gara = gari->FirstChildElement("gara");
             gara;
             gara = gara->NextSiblingElement("gara")) {
            
            tinyxml2::XMLElement* numeGara = gara->FirstChildElement("numeGara");
            if (!numeGara || std::string(numeGara->GetText()) != stationName)
                continue;

            // Found the station, get departure time
            tinyxml2::XMLElement* plecare = gara->FirstChildElement("plecare");
            if (!plecare || std::string(plecare->GetText()) == "-")
                return "No departure for this station";

            tinyxml2::XMLElement* estimarePlecare = gara->FirstChildElement("estimarePlecare");
            if (!estimarePlecare)
                return plecare->GetText();

            std::stringstream ss;
            ss << "Scheduled: " << plecare->GetText() << ", Estimated: " << estimarePlecare->GetText();
            return ss.str();
        }
    }
    return "Train or station not found";
}

std::string XMLManager::getRoute(const std::string& trainId) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("trenuri");
    if (!root) return "Not found";

    for (tinyxml2::XMLElement* train = root->FirstChildElement("tren");
         train;
         train = train->NextSiblingElement("tren")) {
        
        tinyxml2::XMLElement* numeElement = train->FirstChildElement("nume");
        if (!numeElement || std::string(numeElement->GetText()) != trainId)
            continue;

        // Get the route directly from the ruta element
        tinyxml2::XMLElement* rutaElement = train->FirstChildElement("ruta");
        if (!rutaElement)
            return "Route not found";

        return rutaElement->GetText();
    }
    return "Train not found";
}


void XMLManager::announceDelay(const std::string& trainId, const std::string& stationName, int delayMinutes) {
    tinyxml2::XMLElement* trenuri = doc.FirstChildElement("trenuri");
    
    for (tinyxml2::XMLElement* tren = trenuri->FirstChildElement("tren"); 
         tren != nullptr; 
         tren = tren->NextSiblingElement("tren")) {
        
        if (std::string(tren->FirstChildElement("nume")->GetText()) == trainId) {
            tinyxml2::XMLElement* gari = tren->FirstChildElement("gari");
            
            for (tinyxml2::XMLElement* gara = gari->FirstChildElement("gara"); 
                 gara != nullptr; 
                 gara = gara->NextSiblingElement("gara")) {
                
                if (std::string(gara->FirstChildElement("numeGara")->GetText()) == stationName) {
                    // Update delay
                    tinyxml2::XMLElement* intarziere = gara->FirstChildElement("intarziere");
                    int currentDelay = std::stoi(intarziere->GetText());
                    int totalDelay = currentDelay + delayMinutes;
                    intarziere->SetText(std::to_string(totalDelay).c_str());

                    // Update estimated arrival
                    tinyxml2::XMLElement* sosire = gara->FirstChildElement("sosire");
                    tinyxml2::XMLElement* estimareSosire = gara->FirstChildElement("estimareSosire");
                    if (std::string(sosire->GetText()) != "-") {
                        int hours, minutes;
                        sscanf(sosire->GetText(), "%d:%d", &hours, &minutes);
                        minutes += totalDelay;
                        hours += minutes / 60;
                        minutes %= 60;
                        hours %= 24;
                        char newTime[6];
                        sprintf(newTime, "%02d:%02d", hours, minutes);
                        estimareSosire->SetText(newTime);
                    }

                    // Update estimated departure
                    tinyxml2::XMLElement* plecare = gara->FirstChildElement("plecare");
                    tinyxml2::XMLElement* estimarePlecare = gara->FirstChildElement("estimarePlecare");
                    if (std::string(plecare->GetText()) != "-") {
                        int hours, minutes;
                        sscanf(plecare->GetText(), "%d:%d", &hours, &minutes);
                        minutes += totalDelay;
                        hours += minutes / 60;
                        minutes %= 60;
                        hours %= 24;
                        char newTime[6];
                        sprintf(newTime, "%02d:%02d", hours, minutes);
                        estimarePlecare->SetText(newTime);
                    }
                    break;
                }
            }
            break;
        }
    }
    doc.SaveFile("trains.xml");
}