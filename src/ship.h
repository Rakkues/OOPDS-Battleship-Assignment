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

#ifndef SHIP_H
#define SHIP_H

#include <iostream>
#include <memory>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

class Ship
{
protected:
    int id;
    string team_name;
    string ship_type;
    int kill_count = 0;
    int hit_count = 0;
    char symbol;
    int health = 3;
    pair<int, int> *position;
    vector<pair<int, int>> closest_enemy;
    bool marked_for_removal = false;

public:
    Ship(int id, string team, char sym, pair<int, int> *position);
    Ship(Ship &&other) noexcept;
    Ship &operator=(Ship &&other) noexcept;
    virtual ~Ship();

    void show_status() const;
    pair<int, int> get_location();
    char get_symbol() const;
    string get_team_name();
    int get_id();
    virtual void seeing(const vector<unique_ptr<Ship>> &ships) = 0;
    virtual void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) = 0;
    virtual void shooting(vector<unique_ptr<Ship>> &ships) = 0;
    virtual void destroy(vector<unique_ptr<Ship>> &ships) = 0;
    virtual unique_ptr<Ship> upgrade() = 0;

    void mark_for_removal();
    bool is_marked_for_removal() const;
    void killed();
    string get_ship_type();
    void set_ship_type(string type);
    void set_health(int hlth);
    int get_health();
    int get_kill_count();
    void update_kill_count();
    void reset_kill_count();
    int get_hit_count();
    void update_hit_count();
    void reset_hit_count();
};

class SeeingShip : virtual public Ship
{
public:
    SeeingShip(int id, string team, char sym, pair<int, int> *position);
    virtual void seeing(const vector<unique_ptr<Ship>> &ships) = 0;
};

class MovingShip : virtual public Ship
{
public:
    MovingShip(int id, string team, char sym, pair<int, int> *position);
    virtual void moving(pair<int, int> borders, const vector<pair<int, int>> &island_location) = 0;
};

class ShootingShip : virtual public Ship
{
public:
    ShootingShip(int id, string team, char sym, pair<int, int> *position);
    virtual void shooting(vector<unique_ptr<Ship>> &ships) = 0;
};

class RammingShip : virtual public Ship
{
public:
    RammingShip(int id, string team, char sym, pair<int, int> *position);
    virtual void destroy(vector<unique_ptr<Ship>> &ships) = 0;
};

class SuperShip : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    SuperShip(int id, string team, char sym, pair<int, int> *position);
    void seeing(const vector<unique_ptr<Ship>> &ships) override;
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override;
    void shooting(vector<unique_ptr<Ship>> &ships) override;
    void destroy(vector<unique_ptr<Ship>> &ships) override;
    unique_ptr<Ship> upgrade() override;
};

class Destroyer : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Destroyer(int id, string team, char sym, pair<int, int> *position);
    void seeing(const vector<unique_ptr<Ship>> &ships) override;
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override;
    void shooting(vector<unique_ptr<Ship>> &ships) override;
    void destroy(vector<unique_ptr<Ship>> &ships) override;
    unique_ptr<Ship> upgrade() override;
};

class BattleShip : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    BattleShip(int id, string team, char sym, pair<int, int> *position);
    void seeing(const vector<unique_ptr<Ship>> &ships) override;
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override;
    void shooting(vector<unique_ptr<Ship>> &ships) override;
    void destroy(vector<unique_ptr<Ship>> &ships) override;
    unique_ptr<Ship> upgrade() override;
};

class Cruiser : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Cruiser(int id, string team, char sym, pair<int, int> *position);
    void seeing(const vector<unique_ptr<Ship>> &ships) override;
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override;
    void shooting(vector<unique_ptr<Ship>> &ships) override;
    void destroy(vector<unique_ptr<Ship>> &ships) override;
    unique_ptr<Ship> upgrade() override;
};

class Corvette : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Corvette(int id, string team, char sym, pair<int, int> *position);
    void seeing(const vector<unique_ptr<Ship>> &ships) override;
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override;
    void shooting(vector<unique_ptr<Ship>> &ships) override;
    void destroy(vector<unique_ptr<Ship>> &ships) override;
    unique_ptr<Ship> upgrade() override;
};

class Frigate : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
private:
    int shooting_index = 0;

public:
    Frigate(int id, string team, char sym, pair<int, int> *position);
    void seeing(const vector<unique_ptr<Ship>> &ships) override;
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override;
    void shooting(vector<unique_ptr<Ship>> &ships) override;
    void destroy(vector<unique_ptr<Ship>> &ships) override;
    unique_ptr<Ship> upgrade() override;
};

class Amphibious : public SeeingShip, MovingShip, ShootingShip, RammingShip
{
public:
    Amphibious(int id, string team, char sym, pair<int, int> *position);
    void seeing(const vector<unique_ptr<Ship>> &ships) override;
    void moving(pair<int, int> border, const vector<pair<int, int>> &island_location) override;
    void shooting(vector<unique_ptr<Ship>> &ships) override;
    void destroy(vector<unique_ptr<Ship>> &ships) override;
    unique_ptr<Ship> upgrade() override;
};

#endif // SHIP_H