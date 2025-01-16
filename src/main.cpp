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

    cout << endl;
    cout << endl;

    tuple<string, char, int> ship1 = teams[0].getShip(1);
    cout << "Type = " << get<0>(ship1) << ", "
         << "Symbol = " << get<1>(ship1) << ", "
         << "Amount = " << get<2>(ship1) << endl;
}