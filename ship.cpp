#include <iostream>
using namespace std;

class Ship
{
  protected:
    char symbol;
    int shipCount;
};

class Battleship : public Ship
{
  public:
    void Shoot();
};

class Cruiser : public Ship
{
};

class Destroyer : public Battleship
{
  private:
    int killCount;

  public:
    void upgrade();
};

class Frigate : public Ship
{
  private:
    int killCount;
};

class Corvette : public Ship
{
};

class Amphibious : public Ship
{
};

class Supership : public Ship {};