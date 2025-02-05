#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>

#include <vector>

#include <sstream>
#include <fstream>

#include <set>
#include <tuple>
#include <utility>

#include <typeinfo> //debugger

#define RESET "\033[0m"
#define BLUE "\033[34m"
#define RED "\033[31m"

using namespace std;

/*---------------------------------------------------------- Game Reader ----------------------------------------------------------*/

class Game
{
private:
    int iterations;
    int width;
    int height;

public:
    Game(ifstream &infile)
    {
        string line;
        string label;

        bool has_iterations = false;
        bool has_width = false;
        bool has_height = false;

        while (getline(infile, line))
        {
            istringstream iss(line);

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
    }

    int get_iterations() const { return iterations; }
    int get_width() const { return width; }
    int get_height() const { return height; }
};

class Team
{
private:
    string team_name;
    int total_ship_type;
    vector<tuple<string, char, int>> ships;

public:
    static int total_team;
    static int total_ships;

    int team_total_ship = 0;

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
                iss >> team_name >> total_ship_type;
                break;
            }
        }

        string shiptype;
        char symbol;
        int amount;

        for (int i = 0; i < total_ship_type; i++)
        {
            getline(infile, line);
            istringstream iss(line);
            iss >> shiptype >> symbol >> amount;
            ships.emplace_back(make_tuple(shiptype, symbol, amount));
        }

        total_team++;
        for (int i = 0; i < ships.size(); i++)
        {
            tuple<string, char, int> *ship = &ships[i];
            total_ships += get<2>(*ship);
        }

        for (int i = 0; i < ships.size(); i++)
        {
            tuple<string, char, int> *ship = &ships[i];
            team_total_ship += get<2>(*ship);
        }
    }

    string get_team_name() const { return team_name; }
    int get_amount() const { return total_ship_type; }

    tuple<string, char, int> get_ship(int ship_type)
    {
        tuple<string, char, int> *ship = &ships[ship_type];
        return *ship;
    }

    vector<tuple<string, char, int>> get_ships()
    {
        return ships;
    }

    void print()
    {
        cout << "Team: " << team_name << " Ship Types: " << total_ship_type << endl;

        for (int i = 0; i < total_ship_type; i++)
        {
            tuple<string, char, int> *ship = &ships[i];
            cout << "Ship " << i + 1 << ": "
                 << "Type = " << get<0>(*ship) << ", "
                 << "Symbol = " << get<1>(*ship) << ", "
                 << "Amount = " << get<2>(*ship) << endl;
        }

        cout << endl;
    }
};

int Team::total_team = 0;
int Team::total_ships = 0;

class Arena
{
private:
    vector<vector<char>> arena_sandbox;

    int width;
    int height;

public:
    Arena(ifstream &infile, Game &g)
    {
        string line;
        int value;

        width = g.get_width();
        height = g.get_height();

        getline(infile, line);

        while (getline(infile, line))
        {
            istringstream iss(line);
            vector<char> row;
            row.reserve(width);

            for (int j = 0; j < width; j++)
            {
                iss >> value;
                row.push_back(value + '0');
            }

            arena_sandbox.push_back(move(row));
        }
    }

    vector<pair<int, int>> get_island()
    {
        vector<pair<int, int>> island_location;
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (arena_sandbox[i][j] == '1')
                {
                    island_location.push_back(make_pair(j, i));
                }
            }
        }

        return island_location;
    }

    void print_arena()
    {
        cout << endl
             << endl;

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                cout << arena_sandbox[i][j] << " ";
            }
            cout << endl;
        }

        cout << endl
             << endl;
    }

    void print_arena(vector<pair<int, int>> team1_pos, vector<pair<int, int>> team2_pos)
    {
        cout << endl
             << endl;

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                // Check if the current position is in team1
                bool is_team1 = false;
                for (const auto &pos : team1_pos)
                {
                    if (pos.first == j && pos.second == i)
                    {
                        is_team1 = true;
                        break;
                    }
                }

                // Check if the current position is in team2
                bool is_team2 = false;
                for (const auto &pos : team2_pos)
                {
                    if (pos.first == j && pos.second == i)
                    {
                        is_team2 = true;
                        break;
                    }
                }

                // Print with color if it's team1 or team2
                if (is_team1)
                {
                    cout << RED << arena_sandbox[i][j] << RESET << " ";
                }
                else if (is_team2)
                {
                    cout << BLUE << arena_sandbox[i][j] << RESET << " ";
                }
                else
                {
                    cout << arena_sandbox[i][j] << " ";
                }
            }
            cout << endl;
        }

        cout << endl
             << endl;
    }
    void put_ship(pair<int, int> position, char symbol, string colour = RESET)
    {
        arena_sandbox[position.second][position.first] = symbol;
    }
};

/*---------------------------------------------------------- Ships ----------------------------------------------------------*/

// Base Class
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

    pair<int, int> position;

public:
    Ship(string team, char sym, pair<int, int> position)
        : team_name(team), symbol(sym), position(position) {}

    // Move constructor
    Ship(Ship &&other) noexcept
        : team_name(move(other.team_name)), ship_type(move(other.ship_type)),
          kill_count(other.kill_count), hit_count(other.hit_count),
          upgradable(other.upgradable), symbol(other.symbol), health(other.health),
          position(move(other.position)) {}

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
        }
        return *this;
    }

    void show_status() const
    {
        cout << "Ship [" << symbol << "] " << "Ship Type [" << ship_type << "]" << " - Team: " << team_name
             << " | Health: " << health
             << " | Position: (" << position.first << "," << position.second << ")" << endl;
    }

    pair<int, int> get_location() { return position; }
    char get_symbol() { return symbol; }
    string get_team_name() { return team_name; }

    virtual void seeing() = 0;
    virtual void moving() = 0;
    virtual void shooting() = 0;
    virtual void destroy() = 0;

    virtual unique_ptr<Ship> upgrade() = 0;

    string get_ship_type() { return ship_type; }
    void set_ship_type(string type) { ship_type = type; }

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
    SeeingShip(string team, char sym, pair<int, int> position) : Ship(team, sym, position) {}
    virtual void seeing() = 0;
};

class MovingShip : virtual public Ship
{
public:
    MovingShip(string team, char sym, pair<int, int> position) : Ship(team, sym, position) {}
    virtual void moving() = 0;
};

class ShootingShip : virtual public Ship
{
public:
    ShootingShip(string team, char sym, pair<int, int> position) : Ship(team, sym, position) {}
    virtual void shooting() = 0;
};

class RammingShip : virtual public Ship
{
public:
    RammingShip(string team, char sym, pair<int, int> position) : Ship(team, sym, position) {}
    virtual void destroy() = 0;
};

// Concrete class for ship types
class SuperShip : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    SuperShip(string team, char sym, pair<int, int> position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "SuperShip";
    }

    void seeing() override {}
    void moving() override {}
    void shooting() override {}
    void destroy() override {}

    unique_ptr<Ship> upgrade() override { return nullptr; }
};

class Destroyer : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Destroyer(string team, char sym, pair<int, int> position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "Destroyer";
    }

    void seeing() override
    {
        cout << "Ship is looking for enemy" << endl;
    }
    void moving() override
    {
        cout << "Ship is moving" << endl;
    }
    void shooting() override {}
    void destroy() override
    {
        cout << "Ship is ramming" << endl;
        kill_count++;
    }

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
    BattleShip(string team, char sym, pair<int, int> position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "Battleship";
    }

    void seeing() override
    {
        cout << "Ship is looking for enemy" << endl;
    }
    void moving() override
    {
        cout << "Ship is moving" << endl;
    }
    void shooting() override
    {
        cout << "Ship is shooting at enemy" << endl;
        hit_count++;
    }
    void destroy() override {}

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
    Cruiser(string team, char sym, pair<int, int> position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "Cruiser";
    }

    void seeing() override
    {
        cout << "Ship is looking for enemy" << endl;
    }
    void moving() override
    {
        cout << "Ship is moving" << endl;
    }
    void shooting() override {}
    void destroy() override
    {
        cout << "Ship is ramming" << endl;
        kill_count++;
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

class Corvette : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Corvette(string team, char sym, pair<int, int> position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "Corvette";
    }

    void seeing() override {}
    void moving() override {}
    void shooting() override {}
    void destroy() override {}

    unique_ptr<Ship> upgrade() override { return nullptr; }
};

class Frigate : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Frigate(string team, char sym, pair<int, int> position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "Frigate";
    }

    void seeing() override {}
    void moving() override {}
    void shooting() override {}
    void destroy() override {}

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

class Amphibious : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Amphibious(string team, char sym, pair<int, int> position)
        : SeeingShip(team, sym, position), MovingShip(team, sym, position),
          ShootingShip(team, sym, position), RammingShip(team, sym, position),
          Ship(team, sym, position)
    {
        ship_type = "Amphibious";
    }

    void seeing() override {}
    void moving() override {}
    void shooting() override {}
    void destroy() override {}

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

/*---------------------------------------------------------- Main ----------------------------------------------------------*/

int choice = 2;

void test_bench1()
{
    //------ Initializer Game ------//
    ifstream infile("game.txt");
    Game g(infile);

    vector<Team> teams;
    for (int i = 0; i < 2; i++)
    {
        teams.push_back(Team(infile));
    }

    for (int i = 0; i < 2; i++)
    {
        teams[i].print();
        cout << teams[i].team_total_ship << endl;
    }

    cout << Team::total_team << endl;
    cout << Team::total_ships << endl;

    Arena a(infile, g);
    a.print_arena();

    vector<pair<int, int>> island_location = a.get_island();

    for (int i = 0; i < island_location.size(); i++)
    {
        cout << "(" << island_location[i].first << ", " << island_location[i].second << ") | ";
    }
    cout << endl
         << endl;

    //------ Initializer Ship ------//

    int total_ships = Team::total_ships;
    int total_team = Team::total_team;

    int pos_used = 0;

    srand(time(0));

    set<pair<int, int>> used_locations(island_location.begin(), island_location.end());
    vector<pair<int, int>> unique_positions;

    while (unique_positions.size() < total_ships)
    {
        int x = rand() % g.get_width();
        int y = rand() % g.get_height();

        pair<int, int> temp = {x, y};

        if (used_locations.find(temp) == used_locations.end())
        {
            used_locations.insert(temp);
            unique_positions.push_back(temp);
        }
    }

    vector<unique_ptr<Ship>> ships;
    for (int i = 0; i < total_team; i++)
    {
        for (int j = 0; j < teams[i].get_amount(); j++)
        {
            tuple<string, char, int> ship = teams[i].get_ship(j);
            for (int k = 0; k < get<2>(ship); k++)
            {
                if (get<0>(ship) == "Battleship")
                {
                    ships.emplace_back(new BattleShip(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                    pos_used++;
                }

                if (get<0>(ship) == "Cruiser")
                {
                    ships.emplace_back(new Cruiser(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                    pos_used++;
                }

                if (get<0>(ship) == "Destroyer")
                {
                    ships.emplace_back(new Destroyer(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                    pos_used++;
                }

                if (get<0>(ship) == "Frigate")
                {
                    ships.emplace_back(new Frigate(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                    pos_used++;
                }

                if (get<0>(ship) == "Corvette")
                {
                    ships.emplace_back(new Corvette(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                    pos_used++;
                }

                if (get<0>(ship) == "Amphibious")
                {
                    ships.emplace_back(new Amphibious(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                    pos_used++;
                }

                if (get<0>(ship) == "SuperShip")
                {
                    ships.emplace_back(new SuperShip(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                    pos_used++;
                }
            }
        }
    }

    for (int i = 0; i < ships.size(); ++i)
    {
        ships[i]->show_status();
    }

    for (int i = 0; i < ships.size(); ++i)
    {
        if (ships[i]->get_ship_type() == "Battleship")
        {
            for (int j = 0; j < 4; ++j)
            {
                ships[i]->shooting();
            }
        }
        else if (ships[i]->get_ship_type() == "Cruiser")
        {
            for (int j = 0; j < 3; ++j)
            {
                ships[i]->destroy();
            }
        }

        unique_ptr<Ship> upgraded_ship = ships[i]->upgrade();
        if (upgraded_ship)
        {
            ships[i] = move(upgraded_ship);
        }
    }

    // cout << ships[0]->get_kill_count();
    ships[0]->destroy();
    ships[0]->destroy();
    ships[0]->destroy();

    unique_ptr<Ship> upgraded_ship = ships[0]->upgrade();
    if (upgraded_ship)
    {
        ships[0] = move(upgraded_ship);
    }

    for (int i = 0; i < ships.size(); ++i)
    {
        ships[i]->show_status();
    }

    //------ Div ------//
}

void test_bench2()
{
    //------ Initializer Game ------//
    ifstream infile("game.txt");
    Game g(infile);

    vector<Team> teams;
    for (int i = 0; i < 2; i++)
    {
        teams.push_back(Team(infile));
    }

    for (int i = 0; i < 2; i++)
    {
        teams[i].print();
        cout << teams[i].team_total_ship << endl;
    }

    cout << Team::total_team << endl;
    cout << Team::total_ships << endl;

    Arena a(infile, g);
    a.print_arena();

    vector<pair<int, int>> island_location = a.get_island();

    for (int i = 0; i < island_location.size(); i++)
    {
        cout << "(" << island_location[i].first << ", " << island_location[i].second << ") | ";
    }
    cout << endl
         << endl;

    //------ Initializer Ships ------//

    int total_ships = Team::total_ships;
    int total_team = Team::total_team;

    int pos_used = 0;

    srand(time(0));

    set<pair<int, int>> used_locations(island_location.begin(), island_location.end());
    vector<pair<int, int>> unique_positions;

    while (unique_positions.size() < total_ships)
    {
        int x = rand() % g.get_width();
        int y = rand() % g.get_height();

        pair<int, int> temp = {x, y};

        if (used_locations.find(temp) == used_locations.end())
        {
            used_locations.insert(temp);
            unique_positions.push_back(temp);
        }
    }

    vector<unique_ptr<Ship>> ships;
    for (int i = 0; i < total_team; i++)
    {
        for (int j = 0; j < teams[i].get_amount(); j++)
        {
            tuple<string, char, int> ship = teams[i].get_ship(j);
            for (int k = 0; k < get<2>(ship); k++)
            {
                if (get<0>(ship) == "Battleship")
                {
                    ships.emplace_back(new BattleShip(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                }

                if (get<0>(ship) == "Cruiser")
                {
                    ships.emplace_back(new Cruiser(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                }

                if (get<0>(ship) == "Destroyer")
                {
                    ships.emplace_back(new Destroyer(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                }

                if (get<0>(ship) == "Frigate")
                {
                    ships.emplace_back(new Frigate(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                }

                if (get<0>(ship) == "Corvette")
                {
                    ships.emplace_back(new Corvette(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                }

                if (get<0>(ship) == "Amphibious")
                {
                    ships.emplace_back(new Amphibious(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                }

                if (get<0>(ship) == "SuperShip")
                {
                    ships.emplace_back(new SuperShip(teams[i].get_team_name(), get<1>(ship), unique_positions[pos_used]));
                }

                pos_used++;
            }
        }
    }

    for (int i = 0; i < ships.size(); ++i)
    {
        ships[i]->show_status();
    }

    string team1 = teams[0].get_team_name();
    string team2 = teams[1].get_team_name();

    vector<pair<int, int>> team1_pos;
    vector<pair<int, int>> team2_pos;

    for (int i = 0; i < ships.size(); i++)
    {
        if (ships[i]->get_team_name() == team1)
        {
            team1_pos.push_back(ships[i]->get_location());
        }
        else if (ships[i]->get_team_name() == team2)
        {
            team2_pos.push_back(ships[i]->get_location());
        }
    }

    for (int i = 0; i < team1_pos.size(); i++)
    {
        cout << "(" << team1_pos[i].first << ", " << team1_pos[i].second << ") |";
    }
    cout << endl;

    for (int i = 0; i < team2_pos.size(); i++)
    {
        cout << "(" << team2_pos[i].first << ", " << team2_pos[i].second << ") |";
    }
    cout << endl;

    for (int i = 0; i < ships.size(); ++i)
    {
        a.put_ship(ships[i]->get_location(), ships[i]->get_symbol());
    }

    a.print_arena(team1_pos, team2_pos);
}

int main()
{
    if (choice == 1)
    {
        test_bench1();
    }
    else if (choice == 2)
    {
        test_bench2();
    }
    return 0;
}