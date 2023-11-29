#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

int getRandomNumber()
{
    srand(time(0));          // Seed the random number generator with the current time
    return rand() % 100 + 1; // Generate a random number between 1 and 100
}
bool enemyAttack(const json &mapData, const string &currentRoom, const vector<string> &killList)
{
    for (const auto &enemy : mapData["enemies"])
    {
        auto it = find(killList.begin(), killList.end(), enemy.at("id").get<string>()); // find the enemy which the user has already killed
        if (enemy.at("initialroom") == currentRoom && it == killList.end())             // if user has not killed the enemy
        {
            int aggressiveness = enemy.at("aggressiveness").get<int>();
            if (aggressiveness == 0)
            {
                cout << "The " << enemy.at("id").get<string>() << " does nothing when you try to leave the room." << endl;
            }
            else
            {
                int randomInt = getRandomNumber();
                cout << "The " << enemy.at("id").get<string>() << " attacks you when you try to leave the room!" << endl;
                if (randomInt <= aggressiveness)
                {
                    cout << "The " << enemy.at("id").get<string>() << " kills you!" << endl;
                    return false;
                }
                else
                {
                    cout << "You escaped the " << enemy.at("id").get<string>() << " unscathed!" << endl;
                }
            }
        }
    }
    return true;
}
string checkExit(const json &mapData, const string &currentRoom, const string &input) // check that the user can get into another rooms
{
    string temp;
    for (const auto &room : mapData["rooms"])
    {
        if (room.at("id") == currentRoom)
        {

            if (room.at("exits").find(input) != room.at("exits").end())
            {
                return room.at("exits").at(input).get<string>();
            }
        }
    }
    return "error";
}
bool checkItem(const json &mapData, const string &currentRoom, vector<string> &itemList, string &input) // check if the user has already picked the object
{
    for (const auto &item : mapData["objects"])
    {
        auto it = find(itemList.begin(), itemList.end(), input);
        if (item.at("id") == input && item.at("initialroom") == currentRoom && it == itemList.end()) // if the user has not picked the object
        {
            itemList.push_back(item.at("id").get<string>()); // add the object to the itemList (means user has picked the object)
            cout << "You picked up " << item.at("id").get<string>() << '.' << endl;
            return true;
        }
    }
    return false;
}
string checkKill(const json &mapData, const string &currentRoom, vector<string> &killList, vector<string> &itemList, string &input) // check if the user can kill the enemy using the object picked. if user can killed the enemy then name of enemy will  be added to the killList
{
    string success = "true";
    for (const auto &enemy : mapData["enemies"])
    {
        auto it = find(killList.begin(), killList.end(), input);
        if (enemy.at("id") == input && enemy.at("initialroom") == currentRoom && it == killList.end()) // check if the user has not already killed the enemy
        {
            for (const auto &item : enemy["killedby"])
            {
                auto it = find(itemList.begin(), itemList.end(), item);
                if (it == itemList.end()) // check for the object which require to kill enemy does not exits in the itemList
                {
                    success = "false";
                    break;
                }
            }

            if (success == "false")
            {
                cout << "You don't have enough items and got killed by the " << enemy.at("id").get<string>() << '.' << endl;
                return success;
            }
            else
            {
                success = enemy.at("id").get<string>();
                killList.push_back(success); // add the name of enemy into killList which means user has killed the enemy
                cout << "You killed " << enemy.at("id").get<string>() << endl;
                return success;
            }
        }
        else
        {
            success = "error";
        }
    }

    return success;
}
bool checkLook(const json &mapData, const string &currentRoom, vector<string> &killList, vector<string> &itemList, string &input) // check if object exist inside the currentRoom or picked by user.
{
    for (const auto &item : mapData["objects"])
    {
        auto it = find(itemList.begin(), itemList.end(), input);
        if ((item.at("id") == input && item.at("initialroom") == currentRoom) || (item.at("id") == input && it != itemList.end())) // check if the object is in the room or picked by user
        {
            cout << item.at("desc").get<string>() << endl;
            return true;
        }
    }
    return false;
}
void commandError() // if user input some invalid text
{
    cout << "Nothing Happened." << endl;
}
void gameOver()
{
    cout << "GAME OVER" << endl;
    cout << "Good luck next time." << endl;
}
void printRoom(const json &mapData, const string &currentRoom, vector<string> &itemList, vector<string> &killList)
{
    for (const auto &room : mapData["rooms"])
    {
        if (room.at("id") == currentRoom)
        {
            cout << room.at("desc").get<string>() << endl;
        }
    }

    // Print objects in the room
    if (mapData.find("objects") != mapData.end())
    {
        for (const auto &obj : mapData["objects"])
        {
            auto it = find(itemList.begin(), itemList.end(), obj.at("id").get<string>());
            if (obj.at("initialroom") == currentRoom && it == itemList.end())
            {
                cout << "You see a/an " << obj.at("id").get<string>() << " here." << endl;
            }
        }

        // Print enemy in the room
        if (mapData.find("enemies") != mapData.end())
        {
            for (const auto &enemy : mapData["enemies"])
            {
                auto it = find(killList.begin(), killList.end(), enemy.at("id").get<string>());
                if (enemy["initialroom"] == currentRoom && it == killList.end())
                {
                    cout << "A/An " << enemy["id"].get<string>() << " is in the room." << endl;
                }
            }
        }
    }
}
bool checkWin(const json &mapData, const string &currentRoom, const vector<string> &itemList, const vector<string> &killList) // check if the user has win the game
{
    string type = mapData["objective"]["type"].get<string>();
    if (type == "kill")
    {
        for (const auto &enemy : mapData["objective"]["what"])
        {
            auto it = find(killList.begin(), killList.end(), enemy.get<string>());
            if (it == killList.end())
            {
                return false;
            }
        }
        cout << "You have killed all required enemies!" << endl;
    }
    else if (type == "room")
    {
        for (const auto &room : mapData["objective"]["what"])
        {
            if (currentRoom == room)
            {
                cout << "You have reached the designated room." << endl;
                return true;
            }
        }
        return false;
    }
    else if (type == "collect")
    {
        for (const auto &item : mapData["objective"]["what"])
        {
            auto it = find(itemList.begin(), itemList.end(), item.get<string>());
            if (it == itemList.end())
            {
                return false;
            }
        }
        cout << "You have collected all required items!" << endl;
    }
    return true;
}
int main(int argc, char *argv[])
{
    ifstream file(argv[1]);

    json mapData;            // store json data
    vector<string> killList; // store list of enemies killed by user
    vector<string> itemList; // store list of item(object) picked by user

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

    cout << "Welcome to the Text Adventure Game! Input a command when you see '>'" << endl;
    cout << "Input the command 'quit' to exit the game." << endl;

    string currentRoom = mapData["player"].at("initialroom");
    printRoom(mapData, currentRoom, itemList, killList);

    while (true)
    {
        cout << ">" << endl;
        string command; // stores input given by user
        getline(cin, command);
        if (command == "look" || command == "look around")
        {
            printRoom(mapData, currentRoom, itemList, killList);
        }
        else if (command.substr(0, 3) == "go ")
        {
            string exit = command.substr(3);
            string check = checkExit(mapData, currentRoom, exit); // check that the user can get into another rooms
            if (check != "error")
            {
                bool alive = enemyAttack(mapData, currentRoom, killList); // check if the user can exit the room without killing the enemy(if there is enemy in the room)
                if (!alive)
                {
                    gameOver();
                    break;
                }
                currentRoom = check; // user enters into the another room
                printRoom(mapData, currentRoom, itemList, killList);
            }
            else
            {
                commandError(); // if user input some invalid text
            }
        }
        else if (command.substr(0, 5) == "take ")
        {
            string item = command.substr(5);                      // stores the name of item(object)
            if (!checkItem(mapData, currentRoom, itemList, item)) // check if the user has already picked the object
            {
                commandError();
            }
        }
        else if (command.substr(0, 5) == "kill ")
        {
            string kill = command.substr(5);                                          // stores the name of enemy
            string check = checkKill(mapData, currentRoom, killList, itemList, kill); // check if the user can kill the enemy using the object picked. if user can killed the enemy then name of enemy will  be added to the killList
            if (check == "false")
            {
                gameOver();
                break;
            }
            else if (check == "error") // if user input some invalid objects name
            {
                commandError();
            }
        }
        else if (command.substr(0, 5) == "look ")
        {
            string target = command.substr(5);                                        // stores name of object
            bool check = checkLook(mapData, currentRoom, killList, itemList, target); // check if object exist inside the currentRoom or picked by user.
            if (!check)
            {
                commandError();
            }
        }
        else if (command == "list items")
        {
            if (itemList.empty())
            {
                cout << "You have no item in possession." << endl;
            }
            else
            {
                cout << "Your item(s):" << endl;
                for (string s : itemList)
                {
                    cout << s << endl;
                }
            }
        }
        else if (command == "quit")
        {
            break;
        }
        else
        {
            commandError();
        }
        // Check for win condition
        if (checkWin(mapData, currentRoom, itemList, killList))
        {
            cout << "Congratulations! You have won the game!" << endl;
            break;
        }
    }
    return 0;
}