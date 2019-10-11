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
    ifstream inputFile("../noCO2-meta/noCO2-core/json_samples/real_sample_car.json");
    if (!inputFile.is_open()) {
        cerr << "Error opening the file!" << endl;
        return FAILURE_EXIT;
    }

    json root = json::parse(inputFile);
#else
    json root = json::parse(cin);
#endif

    json& routes = root["routes"];
    json& legs = routes[0]["legs"];
    json& steps = legs[0]["steps"];

    float baseEmission = 120;
    float emissions = 0;
    for (json& step : steps) {
        string travelMode = step["travel_mode"].get<string>();
        if (travelMode == "DRIVING") {
            float duration = step["duration"]["value"].get<float>();
            float distance = step["distance"]["value"].get<float>();
            float stepEmission = getEmissions(baseEmission, (distance/duration)*3.6f)*(distance/1000.f);
            emissions += stepEmission;
            step["emissions"] = stepEmission;
        }
        /*else if (travelMode == "TRANSIT") {
            json transitDetails = step["transit_details"];
        }*/
    }

    cout << root.dump();
}
