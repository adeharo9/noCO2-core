#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
using namespace std;

using json = nlohmann::json;

#define FAILURE_EXIT 1

//Emissions: g/km
//Speed: km/h
inline float getEmissions(float baseEmission, float x) {
    float a = baseEmission/1200.f - 11.f/120.f;
    float b = (110.f - baseEmission)/5.f + 3.f/2.f;
    float c = (35.f*(baseEmission - 110.f))/6.f - 10.f;

    return a*x*x + b*x + c;
}

int main() {
#ifdef PARSE_FROM_FILE
    ifstream inputFile("../noCO2-meta/noCO2-core/json_samples/sample_transit_3.json");
    if (!inputFile.is_open()) {
        cerr << "Error opening the file!" << endl;
        return FAILURE_EXIT;
    }

    json root = json::parse(inputFile);
#else
    json root = json::parse(cin);
#endif

    json& emission_index = root["emission_index"];

    float baseEmission;

    string emissionIndex = "med";
    if (!emission_index.is_null()) {
        emissionIndex = emission_index.get<string>();

        if (emissionIndex == "low") {
            baseEmission = 90;
        }
        else if (emissionIndex == "med") {
            baseEmission = 120;
        }
        else if (emissionIndex == "high") {
            baseEmission = 180;
        }
        else if (emissionIndex == "hybrid") {
            baseEmission = 40;
        }
        else if (emissionIndex == "electric") {
            baseEmission = 5;
        }
        else
            baseEmission = 120;
    }
    else
        baseEmission = 120;

    json& routes = root["routes"];

    for (json& route : routes) {
        json& legs = route["legs"];

        float totalEmissions = 0;
        for (json& leg : legs) {
            json& steps = leg["steps"];

            float legEmissions = 0;
            for (json& step : steps) {
                string travelMode = step["travel_mode"].get<string>();
                float duration = step["duration"]["value"].get<float>();
                float distance = step["distance"]["value"].get<float>();
                float speed = ((distance/duration)*3.6f);
                float stepEmission = 0;
                if (travelMode == "DRIVING") {
                    if (emissionIndex == "electric")
                        stepEmission = baseEmission;
                    else
                        stepEmission = getEmissions(baseEmission, speed);
                }
                else if (travelMode == "TRANSIT") {
                    json& transitDetails = step["transit_details"];
                    string vehicleType = transitDetails["line"]["vehicle"]["type"].get<string>();
                    if (vehicleType == "RAIL") {
                        stepEmission = (65.f/300.f);
                    }
                    else if (vehicleType == "METRO_RAIL") {
                        stepEmission = (65.f/300.f);
                    }
                    else if (vehicleType == "SUBWAY") {
                        stepEmission = (65.f/300.f);
                    }
                    else if (vehicleType == "TRAM") {
                        stepEmission = (40.f/50.f);
                    }
                    else if (vehicleType == "MONORAIL") {
                        stepEmission = (40.f/50.f);
                    }
                    else if (vehicleType == "HEAVY_RAIL") {
                        stepEmission = (65.f/300.f);
                    }
                    else if (vehicleType == "COMMUTER_TRAIN") {
                        stepEmission = (65.f/300.f);
                    }
                    else if (vehicleType == "HIGH_SPEED_TRAIN") {
                        stepEmission = (65.f/300.f);
                    }
                    else if (vehicleType == "LONG_DISTANCE_TRAIN") {
                        stepEmission = (65.f/300.f);
                    }
                    else if (vehicleType == "BUS") {
                        stepEmission = getEmissions(70/30, speed);
                    }
                    else if (vehicleType == "TROLLEYBUS") {
                        stepEmission = (40.f/50.f);
                    }
                    else if (vehicleType == "INTERCITY_BUS") {
                        stepEmission = getEmissions(70/40, speed);
                    }
                    else if (vehicleType == "SHARE_TAXI") {
                        stepEmission = getEmissions(120, speed);
                    }
                    else {
                        route["warnings"].push_back("Found unsupported vehicle type" + vehicleType);
                    }
                }
                stepEmission *= distance/1000.f;
                legEmissions += stepEmission;
                json& emissionsObj = (step["emissions"] = json::object());
                emissionsObj["co2"] = stepEmission;
            }
            totalEmissions += legEmissions;
            json& emissionsObj = (leg["emissions"] = json::object());
            emissionsObj["co2"] = legEmissions;
        }
        json& emissionsObjet = (route["emissions"] = json::object());
        emissionsObjet["co2"] = totalEmissions;
    }
    cout << root.dump();
    //cout << totalEmissions << endl;
}
