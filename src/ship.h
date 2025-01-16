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
    int positionX;
    int positionY;
    // int killCount;
};

class MovingShip : public Ship
{
  public:
    void move(int x, int y);
};

class ShootingShip : public Ship
{
  public:
    void shoot(int x, int y);
};

class SeeingShip : public Ship
{
  public:
    void look(int x, int y);
};

class RamShip : public Ship
{
  public:
    void destroy(int x, int y);
};

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

class Destroyer : public SeeingShip, public MovingShip, public ShootingShip, public RamShip
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

class Corvette : public ShootingShip
{
  public:
    Corvette(Frigate &&frigate);
};

class Amphibious : public MovingShip, public ShootingShip
{
};

class SuperShip : public SeeingShip, public MovingShip, public ShootingShip, public RamShip {};