#ifndef PROPERTY_H // Include guards to prevent multiple inclusions
#define PROPERTY_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>

using namespace std;

class Game
{
private:
    int iterations;
    int width;
    int height;
    vector<vector<int>> arena;

public:
    Game(ifstream &infile);
    int getIterations();
    int getWidth();
    int getHeight();
    void print();
};

class Team
{
private:
    string title;
    int typeAmount;
    vector<tuple<string, char, int>> ships;

public:
    Team(ifstream &infile);
    int getAmount();
    void setShips(tuple<string, char, int> x);
    tuple<string, char, int> getShip(int val);
    void print();
};

#endif // GAME_H