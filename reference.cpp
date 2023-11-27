#include <iostream>
#include <fstream>
#include "json.hpp" // Include the JSON parser

using json = nlohmann::json;

// Function to load the map data from a JSON file
json loadMapData(const std::string& filename) {
    std::ifstream file(filename);
    json mapData;
    file >> mapData;
    file.close();
    return mapData;
}

// Function to print the current room description, objects, and enemy
void printRoom(const json& mapData, const std::string& currentRoom) {
    std::cout << mapData["rooms"][currentRoom]["desc"] << std::endl;

    // Print objects in the room
    if (mapData.find("objects") != mapData.end()) {
        for (const auto& obj : mapData["objects"]) {
            if (obj["initialroom"] == currentRoom) {
                std::cout << "You see a " << obj["desc"] << " here." << std::endl;
            }
        }
    }

    // Print enemy in the room
    if (mapData.find("enemies") != mapData.end()) {
        for (const auto& enemy : mapData["enemies"]) {
            if (enemy["initialroom"] == currentRoom) {
                std::cout << "An enemy, " << enemy["desc"] << ", is in the room." << std::endl;
            }
        }
    }
}

// Function to check if an exit is valid
bool isValidExit(const json& mapData, const std::string& currentRoom, const std::string& exit) {
    return mapData["rooms"][currentRoom]["exits"].find(exit) != mapData["rooms"][currentRoom]["exits"].end();
}

// Function to check if the player has won
bool hasPlayerWon(const json& mapData, const std::string& currentRoom) {
    const json& objective = mapData["objective"];
    const std::string& objectiveType = objective["type"];

    if (objectiveType == "room") {
        return currentRoom == objective["what"][0];
    }

    // Implement other objective types if needed

    return false;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <map_data_file.json>" << std::endl;
        return 1;
    }

    const std::string mapFilename = argv[1];
    json mapData = loadMapData(mapFilename);

    std::string currentRoom = mapData["player"]["initialroom"];

    std::cout << "Welcome to the Text Adventure Game!" << std::endl;

    while (true) {
        printRoom(mapData, currentRoom);

        std::string command;
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "look" || command == "look around") {
            printRoom(mapData, currentRoom);
        } else if (command.substr(0, 3) == "go ") {
            std::string exit = command.substr(3);
            if (isValidExit(mapData, currentRoom, exit)) {
                currentRoom = mapData["rooms"][currentRoom]["exits"][exit];
            } else {
                std::cout << "Invalid exit." << std::endl;
            }
        } else if (command.substr(0, 5) == "take ") {
            // Implement object pickup logic here
        } else if (command.substr(0, 5) == "kill ") {
            // Implement enemy killing logic here
        } else if (command == "list items") {
            // Implement listing of carried items
        } else {
            std::cout << "Invalid command." << std::endl;
        }

        // Check for win condition
        if (hasPlayerWon(mapData, currentRoom)) {
            std::cout << "Congratulations! You have won the game!" << std::endl;
            break;
        }
    }

    return 0;
}
