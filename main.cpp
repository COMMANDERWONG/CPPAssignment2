
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;
bool enemyAttack(const json &mapData, const string &currentRoom, const vector<string> &killList)
{
    for (const auto &enemy : mapData["enemies"])
    {
        auto it = find(killList.begin(), killList.end(), enemy.at("id").get<string>());
        if (enemy.at("initialroom") == currentRoom && it == killList.end())
        {
            int aggressiveness = enemy.at("aggressiveness").get<int>();
            if (aggressiveness == 0)
            {
                cout << "The " << enemy.at("id").get<string>() << " does nothing when you try to leave the room." << endl;
            }
            else
            {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<int> dis(1, 100);
                int randomInt = dis(gen);
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
string checkExit(const json &mapData, const string &currentRoom, const string &input)
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
bool checkItem(const json &mapData, const string &currentRoom, vector<string> &itemList, string &input)
{
    for (const auto &item : mapData["objects"])
    {
        auto it = find(itemList.begin(), itemList.end(), input);
        if (item.at("id") == input && item.at("initialroom") == currentRoom && it == itemList.end())
        {
            itemList.push_back(item.at("id").get<string>());
            cout << "You picked up " << item.at("id").get<string>() << endl;
            return true;
        }
    }
    return false;
}
string checkKill(const json &mapData, const string &currentRoom, vector<string> &killList, vector<string> &itemList, string &input)
{
    string success = "true";
    for (const auto &enemy : mapData["enemies"])
    {
        auto it = find(killList.begin(), killList.end(), input);
        if (enemy.at("id") == input && enemy.at("initialroom") == currentRoom && it == killList.end())
        {
            for (const auto &item : enemy["killedby"])
            {
                auto it = find(itemList.begin(), itemList.end(), item);
                if (it == itemList.end())
                {
                    success = "false";
                    break;
                }
            }

            if (success == "false")
            {
                cout << "You don't have enough items and got killed by the " << enemy.at("id").get<string>() << '.'<< endl;
                return success;
            }
            else
            {
                success = enemy.at("id").get<string>();
                killList.push_back(enemy.at("id").get<string>());
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
bool checkLook(const json &mapData, const string &currentRoom, vector<string> &killList, vector<string> &itemList, string &input)
{
    for (const auto &enemy : mapData["enemies"])
    {
        auto it = find(killList.begin(), killList.end(), input);
        if (enemy.at("id") == input && enemy.at("initialroom") == currentRoom && it == killList.end())
        {
            cout << enemy.at("desc").get<string>() << endl;
            return true;
        }
    }

    for (const auto &item : mapData["objects"])
    {
        auto it = find(itemList.begin(), itemList.end(), input);
        if (item.at("id") == input && item.at("initialroom") == currentRoom || it != itemList.end())
        {
            cout << item.at("desc").get<string>() << endl;
            return true;
        }
    }
    return false;
}
void commandError()
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

    int roomIndex;
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
int main(int argc, char *argv[])
{
    ifstream file(argv[1]);

    json mapData;
    vector<string> killList;
    vector<string> itemList;

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

    cout << "Welcome to the Text Adventure Game! Input a command when you see a '>'" << endl;
    cout << "Input the command 'quit' to exit the game." << endl;

    string currentRoom = mapData["player"].at("initialroom");
    printRoom(mapData, currentRoom, itemList, killList);

    while (true)
    {
        // Check for win condition
        // if (hasPlayerWon(mapData, currentRoom))
        // {
        //     cout << "Congratulations! You have won the game!" << endl;
        //     break;
        // }
        cout << ">" << endl;
        string command;
        getline(cin, command);
        if (command == "look" || command == "look around")
        {
            printRoom(mapData, currentRoom, itemList, killList);
        }
        else if (command.substr(0, 3) == "go ")
        {
            string exit = command.substr(3);
            string check = checkExit(mapData, currentRoom, exit);

            // check if go xxx ,where xxx should be proper command
            // string direction = command.substr(3, command.size());
            // int flag = 0;
            // for (const auto &room : mapData["rooms"])
            // {
            //     if (room.at("id") == direction)
            //     {
            //         flag = 1;
            //     }
            // }

            if (check != "error")
            {
                bool alive = enemyAttack(mapData, currentRoom, killList);
                if (!alive)
                {
                    gameOver();
                    break;
                }
                currentRoom = check;
                printRoom(mapData, currentRoom, itemList, killList);
            }
            else
            {
                commandError();
            }
        }
        else if (command.substr(0, 5) == "take ")
        {
            string item = command.substr(5);
            if (!checkItem(mapData, currentRoom, itemList, item))
            {
                commandError();
            }
        }
        else if (command.substr(0, 5) == "kill ")
        {
            string kill = command.substr(5);
            string check = checkKill(mapData, currentRoom, killList, itemList, kill);
            if (check == "false")
            {
                gameOver();
                break;
            }
            else if (check == "error")
            {
                commandError();
            }
        }
        else if (command.substr(0, 5) == "look ")
        {
            string target = command.substr(5);
            bool check = checkLook(mapData, currentRoom, killList, itemList, target);
            if (!check)
            {
                commandError();
            }
        }
        else if (command == "list items")
        {
            if (itemList.empty())
            {
                cout << "You have no item in possession" << endl;
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
    }
    return 0;
}
