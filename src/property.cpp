#include "property.h" // Include the header file

// Game class implementation
Game::Game(ifstream &infile)
{
    string line;
    string label;

    bool hasIterations = false;
    bool hasWidth = false;
    bool hasHeight = false;

    while (getline(infile, line))
    {
        istringstream iss(line);

        iss >> label;
        if (label == "iterations")
        {
            iss >> iterations;
            hasIterations = true;
        }
        else if (label == "width")
        {
            iss >> width;
            hasWidth = true;
        }
        else if (label == "height")
        {
            iss >> height;
            hasHeight = true;
        }

        if (hasIterations && hasWidth && hasHeight)
        {
            break;
        }
    }

    arena = vector<vector<int>>(height, vector<int>(width));
}

int Game::getIterations() { return iterations; }
int Game::getWidth() { return width; }
int Game::getHeight() { return height; }

void Game::print()
{
    cout << "Number of Iterations: " << this->iterations << endl;
    cout << "Width of the Arena: " << this->width << endl;
    cout << "Height of the Arena: " << this->height << endl;
}

// Team class implementation
Team::Team(ifstream &infile)
{
    string line;
    string label;

    while (getline(infile, line))
    {
        istringstream iss(line);

        iss >> label;
        if (label == "Team")
        {
            iss >> title >> typeAmount;
            break;
        }
    }

    for (int i = 0; i < typeAmount; i++)
    {
        if (getline(infile, line))
        {
            istringstream iss(line);
            string shiptype;
            char symbol;
            int amount;
            iss >> shiptype >> symbol >> amount;
            ships.push_back(make_tuple(shiptype, symbol, amount));
        }
        else
        {
            cerr << "Error: Unexpected end of file while reading ships for team " << title << endl;
            break;
        }
    }
}

int Team::getAmount() { return typeAmount; }

void Team::setShips(tuple<string, char, int> x)
{
    ships.push_back(x);
}

tuple<string, char, int> Team::getShip(int val)
{
    tuple<string, char, int> *ship = &ships[val];
    return *ship;
}

void Team::print()
{
    cout << "Team: " << title << " Ship Types: " << typeAmount << endl;
    for (int i = 0; i < ships.size(); i++)
    {
        tuple<string, char, int> &ship = ships[i];
        cout << "Ship " << i + 1 << ": "
             << "Type = " << get<0>(ship) << ", "
             << "Symbol = " << get<1>(ship) << ", "
             << "Amount = " << get<2>(ship) << endl;
    }
}