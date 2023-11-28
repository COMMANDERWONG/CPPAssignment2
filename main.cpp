
#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;
bool isValidExit(const json &mapData, const string &currentRoom, const string &exit)
{
    return mapData["rooms"].at(currentRoom).at("exits").find(exit) != mapData["rooms"].at(currentRoom).at("exits").end();
}
void printRoom(const json &mapData, const string &currentRoom)
{
    cout << mapData["rooms"].at("desc") << endl;

    // Print objects in the room
    if (mapData.find("objects") != mapData.end())
    {
        for (const auto &obj : mapData["objects"])
        {
            if (obj.at("initialroom") == currentRoom)
            {
                cout << "You see a/an " << obj.at("id") << " here." << endl;
            }
        }
    }

    // Print enemy in the room
    if (mapData.find("enemies") != mapData.end())
    {
        for (const auto &enemy : mapData["enemies"])
        {
            if (enemy["initialroom"] == currentRoom)
            {
                cout << "A/An " << enemy["id"] << " is in the room." << endl;
            }
        }
    }
}
int main(int argc, char *argv[])
{
    ifstream file(argv[1]);

    json mapData;

    if (file.is_open())
    {
        file >> mapData;
        file.close();
    }
    else
    {
        cerr << "Error opening map file." << endl;
        return 1;
    }

    cout << "Welcome to the Text Adventure Game!" << endl;

    string currentRoom = mapData["player"].at("initialroom");
    while (true)
    {
        printRoom(mapData, currentRoom);

        string command;
        getline(cin, command);
        if (command == "look" || command == "look around")
        {
            printRoom(mapData, currentRoom);
        }
        else if (command.substr(0, 3) == "go ")
        {
            string exit = command.substr(3);
            if (isValidExit(mapData, currentRoom, exit))
            {
                currentRoom = mapData["rooms"].at(currentRoom).at("exits").at(exit);
            }
        }
        else if (command.substr(0, 5) == "take ")
        {
            // Implement object pickup logic here
        }
        else if (command.substr(0, 5) == "kill ")
        {
            // Implement enemy killing logic here
        }
        else if (command == "list items")
        {
            // Implement listing of carried items
        }
        else
        {
            cout << "Nothing Happened" << endl;
        }

        // Check for win condition
        // if (hasPlayerWon(mapData, currentRoom))
        // {
        //     cout << "Congratulations! You have won the game!" << endl;
        //     break;
        // }
    }
    return 0;
}