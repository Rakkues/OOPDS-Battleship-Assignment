/**********|**********|**********|
Program: GameReader.cpp / GameReader.h
Course: Data Structures and Algorithms
Trimester: 2430
Lecture Section: TC1L
Tutorial Section: TT2L
Name: Danish Zafrin bin Mohd Mohzani
ID: 241UA240TD
Email: danish.zafrin.mohd@student.mmu.edu.my
Phone: 018-919 6242
Name: Muhammad Anas bin Khairul Azman
ID: 241UC2401Z
Email: muhammad.anas.khairul@student.mmu.edu.my
Phone: 019-210 3461
Name: Muhammad Qaid Danial bin Jamaludin
ID: 241UC2415L
Email: muhammad.qaid.danial@student.mmu.edu.my
Phone: 012-951 9400
Name: Raja Izzat bin Raja Kamaruzaman
ID: 241UC240KW
Email: raja.izzat.raja@student.mmu.edu.my
Phone: 012-674 8544
**********|**********|**********/

#include "GameReader.h"
#include "Ship.h"
#include <cstdlib>
#include <ctime>
#define RESET "\033[0m"
#define BLUE "\033[34m"
#define RED "\033[31m"

int Team::total_ships = 0;

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

string Team::get_team_title() const { return team_title; }
int Team::get_total_ship_type() const { return total_ship_type; }
int Team::get_teams_total_ship() const { return this_total_ship; }
tuple<string, char, int> Team::get_ship(int choice) { return ships[choice]; }
vector<tuple<string, char, int>> &Team::get_ships() { return ships; }
vector<pair<int, int>> &Team::get_ships_location() { return ships_location; }

GameReader::GameReader(ifstream &document)
{
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

    teams.emplace_back(Team(document));
    teams.emplace_back(Team(document));

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

int GameReader::get_iterations() { return iterations; }

pair<int, int> GameReader::get_border_limit() { return make_pair(width, height); }
vector<pair<int, int>> GameReader::get_island_location() { return island_location; }
vector<Team> &GameReader::get_teams() { return teams; }

void GameReader::generate_ships_locations()
{
    cout << "generating locations for all the ships" << endl;

    int unique_positions_index = 0;
    set<pair<int, int>> used_locations(island_location.begin(), island_location.end());
    vector<pair<int, int>> unique_positions;

    int index = 0;

    while (index < Team::total_ships)
    {
        int x = rand() % width;
        int y = rand() % height;

        pair<int, int> temp = {x, y};

        if (used_locations.find(temp) == used_locations.end())
        {
            used_locations.insert(temp);
            unique_positions.push_back(temp);
            index++;
            continue;
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

void GameReader::instantiate_ships(vector<unique_ptr<Ship>> &ships)
{
    for (int i = 0; i < 2; i++)
    {
        int unique_location_index = 0;
        int id_index = 1;

        for (int j = 0; j < teams[i].get_total_ship_type(); j++)
        {
            vector<pair<int, int>> &locations = teams[i].get_ships_location();
            tuple<string, char, int> ship = teams[i].get_ship(j);

            for (int k = 0; k < get<2>(ship); k++)
            {
                if (get<0>(ship) == "SuperShip")
                {
                    ships.emplace_back(new SuperShip(id_index, teams[i].get_team_title(), get<1>(ship), &locations[unique_location_index]));
                }
                if (get<0>(ship) == "Destroyer")
                {
                    ships.emplace_back(new Destroyer(id_index, teams[i].get_team_title(), get<1>(ship), &locations[unique_location_index]));
                }
                if (get<0>(ship) == "Battleship")
                {
                    ships.emplace_back(new BattleShip(id_index, teams[i].get_team_title(), get<1>(ship), &locations[unique_location_index]));
                }
                if (get<0>(ship) == "Cruiser")
                {
                    ships.emplace_back(new Cruiser(id_index, teams[i].get_team_title(), get<1>(ship), &locations[unique_location_index]));
                }
                if (get<0>(ship) == "Frigate")
                {
                    ships.emplace_back(new Frigate(id_index, teams[i].get_team_title(), get<1>(ship), &locations[unique_location_index]));
                }
                if (get<0>(ship) == "Corvette")
                {
                    ships.emplace_back(new Corvette(id_index, teams[i].get_team_title(), get<1>(ship), &locations[unique_location_index]));
                }
                if (get<0>(ship) == "Amphibious")
                {
                    ships.emplace_back(new Amphibious(id_index, teams[i].get_team_title(), get<1>(ship), &locations[unique_location_index]));
                }
                unique_location_index++;
                id_index++;
            }
        }
    }
}

void GameReader::remove_dead_ships(vector<unique_ptr<Ship>> &ships)
{
    for (int i = 0; i < ships.size(); i++)
    {
        if (ships[i]->is_marked_for_removal())
        {
            ships.erase(ships.begin() + i);
        }
    }
}

void GameReader::print_ships_info(vector<unique_ptr<Ship>> &ships, ofstream &outfile)
{
    cout << "Teams & Their Locations: ";
    for (int i = 0; i < ships.size(); i++)
    {
        pair<int, int> ships_location = ships[i]->get_location();
        cout << endl;
        cout << "Team " << ships[i]->get_team_name() << ": "
             << "[" << ships[i]->get_ship_type() << ": " << ships[i]->get_id() << ships[i]->get_symbol() << "]"
             << "(" << ships_location.first << ", " << ships_location.second << ") "
             << "Health: " << ships[i]->get_health();
    }

    outfile << "Teams & Their Locations: ";
    for (int i = 0; i < ships.size(); i++)
    {
        pair<int, int> ships_location = ships[i]->get_location();
        outfile << endl;
        outfile << "Team " << ships[i]->get_team_name() << ": "
                << "[" << ships[i]->get_ship_type() << ": " << ships[i]->get_id() << ships[i]->get_symbol() << "]"
                << "(" << ships_location.first << ", " << ships_location.second << ") "
                << "Health: " << ships[i]->get_health();
    }
}

void GameReader::print_battlefield(vector<unique_ptr<Ship>> &ships, ofstream &outfile)
{
    arena_battlefield = arena_empty;

    for (int i = 0; i < ships.size(); ++i)
    {
        pair<int, int> loc = ships[i]->get_location();
        arena_battlefield[loc.second][loc.first] = ships[i]->get_symbol();
    }

    cout << endl
         << endl;
    outfile << endl
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

            outfile << arena_battlefield[i][j] << " ";
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
        outfile << endl;
    }

    cout << endl
         << endl;
    outfile << endl
            << endl;
}

void GameReader::print_teams_total_ship()
{
    for (int i = 0; i < 2; i++)
    {
        cout << teams[i].get_team_title() << ": " << teams[i].get_teams_total_ship();
    }
}
