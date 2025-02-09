#include <iostream>
#include <memory>
#include <algorithm>

#include <sstream>
#include <fstream>

#include <set>
#include <vector>
#include <tuple>
#include <utility>

#include <cstdlib>
#include <ctime>
#include <windows.h>

#define RESET "\033[0m"
#define BLUE "\033[34m"
#define RED "\033[31m"

using namespace std;
/*-----------------------------------------*/
class Ship
{
protected:
    string team_name;
    string ship_type;

    int kill_count = 0;
    int hit_count = 0;

    bool upgradable = false;

    char symbol;

    int health = 3;

    pair<int, int> *position;
    vector<pair<int, int>> closest_enemy;

public:
    Ship(string team, char sym, pair<int, int> *position)
        : team_name(team), symbol(sym), position(position) {}

    // Move constructor
    Ship(Ship &&other) noexcept
        : team_name(move(other.team_name)), ship_type(move(other.ship_type)),
          kill_count(other.kill_count), hit_count(other.hit_count),
          upgradable(other.upgradable), symbol(other.symbol), health(other.health),
          position(move(other.position)), closest_enemy(move(other.closest_enemy)) {}

    // Move assignment operator
    Ship &operator=(Ship &&other) noexcept
    {
        if (this != &other)
        {
            team_name = move(other.team_name);
            ship_type = move(other.ship_type);
            kill_count = other.kill_count;
            hit_count = other.hit_count;
            upgradable = other.upgradable;
            symbol = other.symbol;
            health = other.health;
            position = move(other.position);
            closest_enemy = move(other.closest_enemy);
        }
        return *this;
    }

    void show_status() const
    {
        cout << "Ship [" << symbol << "] " << "Ship Type [" << ship_type << "]" << " - Team: " << team_name
             << " | Health: " << health
             << " | Position: (" << position->first << "," << position->second << ")" << endl;
    }

    pair<int, int> get_location() { return *position; }
    char get_symbol() const { return symbol; }
    string get_team_name() { return team_name; }

    virtual void seeing(const vector<unique_ptr<Ship>> &ships) = 0;
    virtual void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) = 0;
    virtual void shooting(vector<unique_ptr<Ship>> &ships) = 0;
    virtual void destroy(vector<unique_ptr<Ship>> &ships) = 0;

    virtual unique_ptr<Ship> upgrade() = 0;

    void killed(vector<unique_ptr<Ship>> &ships)
    {
        for (int i = 0; i < ships.size(); i++)
        {
            if (*position == ships[i]->get_location() && symbol == ships[i]->get_symbol())
            {
                ships.erase(ships.begin() + i);
            }
        }
    }

    string get_ship_type() { return ship_type; }
    void set_ship_type(string type) { ship_type = type; }

    void set_health(int hlth) { health = hlth; }
    int get_health() { return health; }

    int get_kill_count() { return kill_count; }
    void update_kill_count() { kill_count++; }
    void reset_kill_count() { kill_count = 0; }

    int get_hit_count() { return hit_count; }
    void update_hit_count() { hit_count++; }
    void reset_hit_count() { hit_count = 0; }

    virtual ~Ship() {}
};

// Abstract classes for seeing, moving, shooting, and ramming
class SeeingShip : virtual public Ship
{
public:
    SeeingShip(string team, char sym, pair<int, int> *position) : Ship(team, sym, position) {}
    virtual void seeing(const vector<unique_ptr<Ship>> &ships) = 0;
};
class MovingShip : virtual public Ship
{
public:
    MovingShip(string team, char sym, pair<int, int> *position) : Ship(team, sym, position) {}
    virtual void moving(pair<int, int> borders, const vector<pair<int, int>> &island_location) = 0;
};
class ShootingShip : virtual public Ship
{
public:
    ShootingShip(string team, char sym, pair<int, int> *position) : Ship(team, sym, position) {}
    virtual void shooting(vector<unique_ptr<Ship>> &ships) = 0;
};
class RammingShip : virtual public Ship
{
public:
    RammingShip(string team, char sym, pair<int, int> *position) : Ship(team, sym, position) {}
    virtual void destroy(vector<unique_ptr<Ship>> &ships) = 0;
};

// Concrete class for ship types

/* Super Ship TODO:

*/
class SuperShip : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    SuperShip(string team, char sym, pair<int, int> *position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "SuperShip";
    }

    void seeing(const vector<unique_ptr<Ship>> &ships) override
    {
        cout << "Seeing" << endl;
        closest_enemy.clear(); // Clear previous entries

        for (const auto &ship : ships)
        {
            if (ship->get_team_name() != team_name) // Check if the ship is an enemy
            {
                pair<int, int> enemyPosition = ship->get_location();
                int distance = abs(enemyPosition.first - position->first) + abs(enemyPosition.second - position->second);

                if (distance <= 5) // If the enemy is within 5 blocks
                {
                    cout << "Enemy Located" << endl;
                    closest_enemy.push_back(enemyPosition);
                }
            }
        }
    }
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override
    {
        cout << "Moving" << endl;

        int width = border.first;
        int height = border.second;

        // Determine the direction to move
        int newX = position->second;
        int newY = position->first;

        if (!closest_enemy.empty())
        {
            // Move towards the first enemy in the closest_enemy vector
            pair<int, int> target = closest_enemy[0];

            int deltaY = target.first - position->first;
            int deltaX = target.second - position->second;

            if (abs(deltaX) > abs(deltaY))
            {
                // Move horizontally
                if (deltaX > 0)
                    newX++; // Move right
                else
                    newX--; // Move left
            }
            else
            {
                // Move vertically
                if (deltaY > 0)
                    newY++; // Move down
                else
                    newY--; // Move up
            }
        }
        else
        {
            // Randomly select a direction (0: right, 1: left, 2: down, 3: up)
            int direction = rand() % 4;

            switch (direction)
            {
            case 0: // Right
                newX++;
                break;
            case 1: // Left
                newX--;
                break;
            case 2: // Down
                newY++;
                break;
            case 3: // Up
                newY--;
                break;
            }
        }

        // Check if the new position is within the battlefield bounds
        if (newX >= 0 && newX < width && newY >= 0 && newY < height)
        {
            // Check if the new position is an island
            bool isIsland = false;
            for (const auto &island : island_location)
            {
                if (island.second == newX && island.first == newY)
                {
                    isIsland = true;
                    break;
                }
            }

            // Update the ship's position if it's not an island
            if (!isIsland)
            {
                position->second = newX;
                position->first = newY;
                cout << "Ship [" << symbol << "] moved to (" << position->first << ", " << position->second << ")." << endl;
            }
            else
            {
                cout << "Ship [" << symbol << "] cannot move to (" << newX << ", " << newY << ") - island detected." << endl;
            }
        }
        else
        {
            cout << "Ship [" << symbol << "] cannot move to (" << newX << ", " << newY << ") - out of bounds." << endl;
        }
    }
    void shooting(vector<unique_ptr<Ship>> &ships) override
    {
        cout << "Shooting" << endl;
        if (!closest_enemy.empty())
        {
            // Iterate over all detected enemies
            for (const auto &enemy_pos : closest_enemy)
            {
                // Check if the enemy is within shooting range (e.g., 1 block away)
                int distance = abs(enemy_pos.first - position->first) + abs(enemy_pos.second - position->second);
                if (distance <= 1) // Assuming shooting range is 1 block
                {
                    // 1/3 chance of hitting the enemy
                    if ((rand() % 3) == 0) // 1 in 3 chance
                    {
                        // Find the enemy ship in the ships vector
                        for (auto &ship : ships)
                        {
                            if (ship->get_location() == enemy_pos && ship->get_team_name() != team_name)
                            {
                                // Reduce enemy health
                                ship->set_health(ship->get_health() - 1);
                                update_hit_count(); // Increase hit count for the shooter

                                cout << "Ship [" << symbol << "] hit Ship [" << ship->get_symbol() << "]!" << endl;

                                // Check if the enemy ship is destroyed
                                if (ship->get_health() <= 0)
                                {
                                    cout << "Ship [" << ship->get_symbol() << "] has been destroyed by Ship [" << symbol << "]!" << endl;
                                    update_kill_count(); // Increase kill count for the shooter
                                    ship->killed(ships); // Mark the ship as destroyed
                                }
                                break;
                            }
                        }
                    }
                    else
                    {
                        cout << "Ship [" << symbol << "] missed the shot!" << endl;
                    }
                }
            }
        }
    }
    void destroy(vector<unique_ptr<Ship>> &ships) override
    {
        cout << "Destroy" << endl;

        for (size_t i = 0; i < ships.size(); ++i)
        {
            if (ships[i]->get_location() == *position && ships[i].get() != this && ships[i]->get_team_name() != this->get_team_name()) // Avoid self-collision & team mates
            {
                cout << "Collision detected with ship [" << ships[i]->get_symbol() << "]" << endl;
                cout << "Ship [" << symbol << "] rammed and destroyed Ship [" << ships[i]->get_symbol() << "]!" << endl;
                ships[i]->set_health(0);
                ships[i]->killed(ships);

                break; // Exit after the first collision
            }
        }
    }

    unique_ptr<Ship> upgrade() override { return nullptr; }
};

/* Destroyer TODO:

*/
class Destroyer : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Destroyer(string team, char sym, pair<int, int> *position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "Destroyer";
    }

    void seeing(const vector<unique_ptr<Ship>> &ships) override {}
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override {}
    void shooting(vector<unique_ptr<Ship>> &ships) override {}
    void destroy(vector<unique_ptr<Ship>> &ships) override {}

    unique_ptr<Ship> upgrade() override
    {
        if (kill_count >= 3)
        { // Upgrade condition for Cruiser
            cout << "Destroyer is upgrading to SuperShip!" << endl;
            return unique_ptr<Ship>(new SuperShip(team_name, symbol, position));
        }
        return nullptr; // No upgrade
    }
};

class BattleShip : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    BattleShip(string team, char sym, pair<int, int> *position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "Battleship";
    }

    void seeing(const vector<unique_ptr<Ship>> &ships) override
    {
        cout << "Seeing" << endl;
        closest_enemy.clear(); // Clear previous entries

        for (const auto &ship : ships)
        {
            if (ship->get_team_name() != team_name) // Check if the ship is an enemy
            {
                pair<int, int> enemyPosition = ship->get_location();
                int distance = abs(enemyPosition.first - position->first) + abs(enemyPosition.second - position->second);

                if (distance <= 5) // If the enemy is within 5 blocks
                {
                    cout << "Enemy Located" << endl;
                    closest_enemy.push_back(enemyPosition);
                }
            }
        }
    }
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override
    {
        cout << "Moving" << endl;

        int width = border.first;
        int height = border.second;

        // Determine the direction to move
        int newX = position->second;
        int newY = position->first;

        if (!closest_enemy.empty())
        {
            // Move towards the first enemy in the closest_enemy vector
            pair<int, int> target = closest_enemy[0];

            int deltaY = target.first - position->first;
            int deltaX = target.second - position->second;

            if (abs(deltaX) > abs(deltaY))
            {
                // Move horizontally
                if (deltaX > 0)
                    newX++; // Move right
                else
                    newX--; // Move left
            }
            else
            {
                // Move vertically
                if (deltaY > 0)
                    newY++; // Move down
                else
                    newY--; // Move up
            }
        }
        else
        {
            // Randomly select a direction (0: right, 1: left, 2: down, 3: up)
            int direction = rand() % 4;

            switch (direction)
            {
            case 0: // Right
                newX++;
                break;
            case 1: // Left
                newX--;
                break;
            case 2: // Down
                newY++;
                break;
            case 3: // Up
                newY--;
                break;
            }
        }

        // Check if the new position is within the battlefield bounds
        if (newX >= 0 && newX < width && newY >= 0 && newY < height)
        {
            // Check if the new position is an island
            bool isIsland = false;
            for (const auto &island : island_location)
            {
                if (island.second == newX && island.first == newY)
                {
                    isIsland = true;
                    break;
                }
            }

            // Update the ship's position if it's not an island
            if (!isIsland)
            {
                position->second = newX;
                position->first = newY;
                cout << "Ship [" << symbol << "] moved to (" << position->first << ", " << position->second << ")." << endl;
            }
            else
            {
                cout << "Ship [" << symbol << "] cannot move to (" << newX << ", " << newY << ") - island detected." << endl;
            }
        }
        else
        {
            cout << "Ship [" << symbol << "] cannot move to (" << newX << ", " << newY << ") - out of bounds." << endl;
        }
    }
    void shooting(vector<unique_ptr<Ship>> &ships) override {}
    void destroy(vector<unique_ptr<Ship>> &ships) override {}

    unique_ptr<Ship> upgrade() override
    {
        if (hit_count >= 4)
        { // Upgrade condition for BattleShip
            cout << "BattleShip is upgrading to Destroyer!" << endl;
            return unique_ptr<Ship>(new Destroyer(team_name, symbol, position));
        }
        return nullptr; // No upgrade
    }
};

class Cruiser : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Cruiser(string team, char sym, pair<int, int> *position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "Cruiser";
    }

    void seeing(const vector<unique_ptr<Ship>> &ships) override
    {
        closest_enemy.clear(); // Clear previous entries

        for (const auto &ship : ships)
        {
            if (ship->get_team_name() != team_name) // Check if the ship is an enemy
            {
                pair<int, int> enemyPosition = ship->get_location();
                int distance = abs(enemyPosition.first - position->first) + abs(enemyPosition.second - position->second);

                if (distance <= 2) // If the enemy is within 5 blocks
                {
                    cout << "Enemy Located" << endl;
                    closest_enemy.push_back(enemyPosition);
                }
            }
        }
    }
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override
    {
        cout << "Moving" << endl;

        int width = border.first;
        int height = border.second;

        // Determine the direction to move
        int newX = position->second;
        int newY = position->first;

        if (!closest_enemy.empty())
        {
            // Move towards the first enemy in the closest_enemy vector
            pair<int, int> target = closest_enemy[0];

            int deltaY = target.first - position->first;
            int deltaX = target.second - position->second;

            if (abs(deltaX) > abs(deltaY))
            {
                // Move horizontally
                if (deltaX > 0)
                    newX++; // Move right
                else
                    newX--; // Move left
            }
            else
            {
                // Move vertically
                if (deltaY > 0)
                    newY++; // Move down
                else
                    newY--; // Move up
            }
        }
        else
        {
            // Randomly select a direction (0: right, 1: left, 2: down, 3: up)
            int direction = rand() % 4;

            switch (direction)
            {
            case 0: // Right
                newX++;
                break;
            case 1: // Left
                newX--;
                break;
            case 2: // Down
                newY++;
                break;
            case 3: // Up
                newY--;
                break;
            }
        }

        // Check if the new position is within the battlefield bounds
        if (newX >= 0 && newX < width && newY >= 0 && newY < height)
        {
            // Check if the new position is an island
            bool isIsland = false;
            for (const auto &island : island_location)
            {
                if (island.second == newX && island.first == newY)
                {
                    isIsland = true;
                    break;
                }
            }

            // Update the ship's position if it's not an island
            if (!isIsland)
            {
                position->second = newX;
                position->first = newY;
                cout << "Ship [" << symbol << "] moved to (" << position->first << ", " << position->second << ")." << endl;
            }
            else
            {
                cout << "Ship [" << symbol << "] cannot move to (" << newX << ", " << newY << ") - island detected." << endl;
            }
        }
        else
        {
            cout << "Ship [" << symbol << "] cannot move to (" << newX << ", " << newY << ") - out of bounds." << endl;
        }
    }
    void shooting(vector<unique_ptr<Ship>> &ships) override {}
    void destroy(vector<unique_ptr<Ship>> &ships) override
    {
        cout << "Destroy" << endl;

        for (size_t i = 0; i < ships.size(); ++i)
        {
            if (ships[i]->get_location() == *position && ships[i].get() != this && ships[i]->get_team_name() != this->get_team_name()) // Avoid self-collision & team mates
            {
                cout << "Collision detected with ship [" << ships[i]->get_symbol() << "]" << endl;
                cout << "Ship [" << symbol << "] rammed and destroyed Ship [" << ships[i]->get_symbol() << "]!" << endl;
                ships[i]->set_health(0);
                ships[i]->killed(ships);

                break; // Exit after the first collision
            }
        }
    }

    unique_ptr<Ship> upgrade() override
    {
        if (kill_count >= 3)
        { // Upgrade condition for Cruiser
            cout << "Cruiser is upgrading to Destroyer!" << endl;
            return unique_ptr<Ship>(new Destroyer(team_name, symbol, position));
        }
        return nullptr; // No upgrade
    }
};

/* Corvette TODO:

*/
class Corvette : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Corvette(string team, char sym, pair<int, int> *position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "Corvette";
    }

    void seeing(const vector<unique_ptr<Ship>> &ships) override {}
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override {}
    void shooting(vector<unique_ptr<Ship>> &ships) override {}
    void destroy(vector<unique_ptr<Ship>> &ships) override {}

    unique_ptr<Ship> upgrade() override { return nullptr; }
};

/* Frigate TODO:

*/
class Frigate : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Frigate(string team, char sym, pair<int, int> *position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "Frigate";
    }

    void seeing(const vector<unique_ptr<Ship>> &ships) override {}
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override {}
    void shooting(vector<unique_ptr<Ship>> &ships) override {}
    void destroy(vector<unique_ptr<Ship>> &ships) override {}

    unique_ptr<Ship> upgrade() override
    {
        if (kill_count >= 3)
        { // Upgrade condition for Cruiser
            cout << "Frigate is upgrading to Corvette!" << endl;
            return unique_ptr<Ship>(new Corvette(team_name, symbol, position));
        }
        return nullptr; // No upgrade
    }
};

/* Amphibious TODO:

*/
class Amphibious : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Amphibious(string team, char sym, pair<int, int> *position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "Amphibious";
    }

    void seeing(const vector<unique_ptr<Ship>> &ships) override {}
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override {}
    void shooting(vector<unique_ptr<Ship>> &ships) override {}
    void destroy(vector<unique_ptr<Ship>> &ships) override {}

    unique_ptr<Ship> upgrade() override
    {
        if (kill_count >= 3)
        { // Upgrade condition for Cruiser
            cout << "Amphibious is upgrading to SuperShip!" << endl;
            return unique_ptr<Ship>(new SuperShip(team_name, symbol, position));
        }
        return nullptr; // No upgrade
    }
};

//---------------------------------------------------------------------------------

// creating team data type
class Team
{
private:
    string team_title;

    int total_ship_type;
    int this_total_ship = 0;

    vector<tuple<string, char, int>> ships;
    vector<pair<int, int>> ships_location;

public:
    Team(ifstream &infile)
    {
        string line;
        string label;

        while (getline(infile, line))
        {
            istringstream iss(line);

            iss >> label;
            if (label == "Team")
            {
                iss >> team_title >> total_ship_type;
                break;
            }
        }

        string ship_type;
        char symbol;
        int amount;

        for (int i = 0; i < total_ship_type; i++)
        {
            getline(infile, line);
            istringstream iss(line);
            iss >> ship_type >> symbol >> amount;
            ships.emplace_back(make_tuple(ship_type, symbol, amount));
        }

        for (int i = 0; i < ships.size(); i++)
        {
            tuple<string, char, int> *ship = &ships[i];
            total_ships += get<2>(*ship);
            this_total_ship += get<2>(*ship);
        }
    }

    // static variables
    static int total_ships;

    // getters
    string get_team_title() const { return team_title; }

    int get_total_ship_type() const { return total_ship_type; }
    int get_teams_total_ship() const { return this_total_ship; }

    // referencer & setters
    tuple<string, char, int> get_ship(int choice) { return ships[choice]; }

    vector<tuple<string, char, int>> &get_ships() { return ships; }
    vector<pair<int, int>> &get_ships_location() { return ships_location; }
};

int Team::total_ships = 0;

// game reader class
class GameReader
{
private:
    int iterations;

    int width;
    int height;

    vector<Team> teams;

    vector<vector<char>> arena_empty;
    vector<vector<char>> arena_battlefield;

    vector<pair<int, int>> island_location;

public:
    GameReader(ifstream &document)
    {
        // getting the game data
        string line0;
        string label;

        bool has_iterations = false;
        bool has_width = false;
        bool has_height = false;

        while (getline(document, line0))
        {
            istringstream iss(line0);

            iss >> label;
            if (label == "iterations")
            {
                iss >> iterations;
                has_iterations = true;
            }
            else if (label == "width")
            {
                iss >> width;
                has_width = true;
            }
            else if (label == "height")
            {
                iss >> height;
                has_height = true;
            }

            if (has_iterations && has_width && has_height)
            {
                break;
            }
        }

        // inserting teams into their container
        teams.push_back(Team(document));
        teams.push_back(Team(document));

        // making the arena 2d vector
        string line1;
        int value;

        getline(document, line1);

        while (getline(document, line1))
        {
            istringstream iss(line1);
            vector<char> row;
            row.reserve(width);

            for (int j = 0; j < width; j++)
            {
                iss >> value;
                row.push_back(value + '0');
            }

            arena_empty.push_back(move(row));
        }
        arena_battlefield = arena_empty;

        // getting the island locations
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (arena_empty[i][j] == '1')
                {
                    island_location.push_back(make_pair(j, i));
                }
            }
        }
    }

    // getters
    pair<int, int> get_border_limit() { return make_pair(width, height); }
    vector<pair<int, int>> get_island_location() { return island_location; }

    vector<Team> &get_teams() { return teams; }

    // instantiators
    void generate_ships_locations()
    {
        cout << "generating locations for all the ships" << endl;

        int unique_positions_index = 0;

        set<pair<int, int>> used_locations(island_location.begin(), island_location.end());
        vector<pair<int, int>> unique_positions;

        while (unique_positions.size() < Team::total_ships)
        {
            int x = rand() % width;
            int y = rand() % height;

            pair<int, int> temp = {x, y};

            if (used_locations.find(temp) == used_locations.end())
            {
                used_locations.insert(temp);
                unique_positions.push_back(temp);
            }
        }

        for (int i = 0; i < 2; i++)
        {
            vector<pair<int, int>> &locations = teams[i].get_ships_location();
            for (int j = 0; j < teams[i].get_teams_total_ship(); j++)
            {
                locations.push_back(unique_positions[unique_positions_index]);
                unique_positions_index++;
            }
        }
    }
    void instantiate_ships(vector<unique_ptr<Ship>> &ships)
    {
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < teams[i].get_total_ship_type(); j++)
            {
                int unique_location_index = 0;
                vector<pair<int, int>> &locations = teams[i].get_ships_location();

                tuple<string, char, int> ship = teams[i].get_ship(j);
                for (int k = 0; k < get<2>(ship); k++)
                {

                    if (get<0>(ship) == "SuperShip")
                    {
                        ships.emplace_back(new SuperShip(teams[i].get_team_title(), get<1>(ship), &locations[unique_location_index]));
                    }

                    unique_location_index++;
                }
            }
        }
    }

    // printer
    void print_ships_location(vector<unique_ptr<Ship>> &ships)
    {
        cout << "printing all the ships locations" << endl;
        for (int i = 0; i < ships.size(); i++)
        {
            pair<int, int> ships_location = ships[i]->get_location();
            cout << endl;
            cout << "Team " << ships[i]->get_team_name() << ": "
                 << "[" << ships[i]->get_symbol() << "] : "
                 << "(" << ships_location.first << ", " << ships_location.second << ")|";
        }
    }
    void print_battlefield(vector<unique_ptr<Ship>> &ships)
    {
        arena_battlefield = arena_empty;

        for (int i = 0; i < ships.size(); ++i)
        {
            pair<int, int> loc = ships[i]->get_location();
            arena_battlefield[loc.second][loc.first] = ships[i]->get_symbol();
        }

        // puts locations of the ships onto the battlefield vector

        cout << endl
             << endl;

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {

                bool is_team1 = false;
                bool is_team2 = false;

                for (int index = 0; index < ships.size(); index++)
                {
                    pair<int, int> loc = ships[index]->get_location();
                    if (loc.first == j && loc.second == i)
                    {
                        if (ships[index]->get_team_name() == teams[0].get_team_title())
                        {
                            is_team1 = true;
                        }
                        else
                        {
                            is_team2 = true;
                        }
                    }
                }

                // Print with color if it's team1 or team2
                if (is_team1)
                {
                    cout << RED << arena_battlefield[i][j] << RESET << " ";
                }
                else if (is_team2)
                {
                    cout << BLUE << arena_battlefield[i][j] << RESET << " ";
                }
                else
                {
                    cout << arena_battlefield[i][j] << " ";
                }
            }
            cout << endl;
        }

        cout << endl
             << endl;
    }
};
//---------------------------------------------------------------------------------

int main()
{
    srand(time(0));

    ifstream document("main.txt");
    GameReader gr(document);

    vector<unique_ptr<Ship>> ships;

    gr.generate_ships_locations();
    gr.instantiate_ships(ships);

    gr.print_ships_location(ships);
    gr.print_battlefield(ships);

    int iterations = 0;
    while (ships.size() != 1)
    {
        Sleep(500);
        for (int i = 0; i < ships.size(); i++)
        {
            ships[i]->moving(gr.get_border_limit(), gr.get_island_location());
            ships[i]->seeing(ships);
            ships[i]->shooting(ships);
            ships[i]->destroy(ships);
        }
        gr.print_ships_location(ships);
        gr.print_battlefield(ships);

        // insert into .txt

        iterations++;
    }

    cout << "finished" << endl;

    gr.print_ships_location(ships);
    gr.print_battlefield(ships);

    // trackers

    return 0;
}