/**********|**********|**********|
Program: main.cpp / ship.h 
Course: Data Structures and Algorithms
Trimester: 2430
Name: Muhammad Anas bin Khairul Azman
ID: 241UC2401Z
Lecture Section: TC1L
Tutorial Section: TT1L
Email: muhammad.anas.khairul@student.mmu.edu.my
Phone: 019-2103461
**********|**********|**********/

#include <iostream>
using namespace std;

class Ship
{
  protected:
    char symbol;
    int livesCount = 3;
    // int killCount;
};

class MovingShip : public Ship
{
};

class ShootingShip : public Ship
{
};

class SeeingShip : public Ship
{
};

class RamShip : public Ship {};

class Battleship : public MovingShip, public ShootingShip
{
  private:
    int killCount;
};

class Cruiser : public SeeingShip, public MovingShip, public RamShip
{
  private:
    int killCount;
};

class Destroyer : public Battleship, public Cruiser
{
  private:
    int killCount;

  public:
    Destroyer(Battleship &&battleship);
    Destroyer(Cruiser &&cruiser);
};

class Frigate : public ShootingShip
{
  private:
    int killCount;
};

class Corvette : public Frigate
{
  public:
    Corvette(Frigate &&frigate);
};

class Amphibious : public Battleship
{
};

class SuperShip : public Destroyer {};