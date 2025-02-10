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

#ifndef GAMEREADER_H
#define GAMEREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <utility>
#include <set>
#include <memory>
#include "Ship.h" // Include your Ship header if you have one

using namespace std;

class Team
{
private:
    string team_title;
    int total_ship_type;
    int this_total_ship = 0;
    vector<tuple<string, char, int>> ships;
    vector<pair<int, int>> ships_location;

public:
    Team(ifstream &infile);
    static int total_ships;

    // Getters
    string get_team_title() const;
    int get_total_ship_type() const;
    int get_teams_total_ship() const;

    // Referencer & setters
    tuple<string, char, int> get_ship(int choice);
    vector<tuple<string, char, int>> &get_ships();
    vector<pair<int, int>> &get_ships_location();
};

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
    GameReader(ifstream &document);

    // Getters
    int get_iterations();
    pair<int, int> get_border_limit();
    vector<pair<int, int>> get_island_location();
    vector<Team> &get_teams();

    // Instantiators
    void generate_ships_locations();
    void instantiate_ships(vector<unique_ptr<Ship>> &ships);
    void remove_dead_ships(vector<unique_ptr<Ship>> &ships);

    // Printer
    void print_ships_info(vector<unique_ptr<Ship>> &ships, ofstream &outfile);
    void print_battlefield(vector<unique_ptr<Ship>> &ships, ofstream &outfile);
    void print_teams_total_ship();
};

#endif // GAMEREADER_H
