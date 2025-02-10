#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <windows.h>

#include "GameReader.h"
#include "Ship.h"

using namespace std;

int main()
{
    srand(time(0));

    ofstream outfile("output.txt");
    ifstream document("main.txt");

    GameReader gr(document);

    vector<unique_ptr<Ship>> ships;

    gr.generate_ships_locations();
    gr.instantiate_ships(ships);

    gr.print_ships_info(ships, outfile);
    gr.print_battlefield(ships, outfile);

    int iterations = 0;
    bool on_going = true;

    while (iterations <= gr.get_iterations() && on_going)
    {
        Sleep(500);
        for (int i = 0; i < ships.size(); i++)
        {
            ships[i]->moving(gr.get_border_limit(), gr.get_island_location());
            ships[i]->seeing(ships);
            ships[i]->shooting(ships);
            ships[i]->destroy(ships);
        }

        gr.remove_dead_ships(ships);

        cout << endl;
        for (int i = 0; i < ships.size(); i++)
        {
            auto upgraded_ship = ships[i]->upgrade();
            if (upgraded_ship)
            {
                ships[i] = move(upgraded_ship);
            }
        }

        cout << ships.size() << endl;

        for (int i = 0; i < ships.size(); i++)
        {
            if (ships[i]->get_team_name() != ships[ships.size() - 1]->get_team_name())
            {
                break;
            }
            on_going = false;
        }

        gr.print_ships_info(ships, outfile);
        gr.print_battlefield(ships, outfile);

        iterations++;
    }

    return 0;
}