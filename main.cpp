
#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

int main(int argc,char *argv[])
{
    ifstream file(argv[1]);

    json jObj;

    if (file.is_open())
    {
        file >> jObj;
        file.close();
        cout << jObj;
    }
    else
    {
        std::cerr << "Error opening map file." << std::endl;
        return 1;
    }

    return 0;
}