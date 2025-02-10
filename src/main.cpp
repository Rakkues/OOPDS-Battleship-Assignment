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