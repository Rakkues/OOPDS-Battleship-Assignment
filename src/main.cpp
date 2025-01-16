#include "property.h"

#include <iostream>

using namespace std;

int main()
{
    ifstream infile("game.txt");
    Game g(infile);
    vector<Team> teams;

    for (int i = 0; i < 2; i++)
    {
        teams.push_back(Team(infile));
    }

    g.print();
    for (int i = 0; i < 2; i++)
    {
        teams[i].print();
    }
}