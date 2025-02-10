#include "Ship.h"

// Implementations of Ship and derived classes...
Ship::Ship(int id, string team, char sym, pair<int, int> *position)
    : id(id), team_name(team), symbol(sym), position(position) {}

// Move constructor
Ship::Ship(Ship &&other) noexcept
    : id(move(other.id)), team_name(move(other.team_name)), ship_type(move(other.ship_type)),
      kill_count(other.kill_count), hit_count(other.hit_count),
      symbol(other.symbol), health(other.health),
      position(move(other.position)), closest_enemy(move(other.closest_enemy)) {}

// Move assignment operator
Ship &Ship::operator=(Ship &&other) noexcept
{
    if (this != &other)
    {
        id = move(other.id);
        team_name = move(other.team_name);
        ship_type = move(other.ship_type);
        kill_count = other.kill_count;
        hit_count = other.hit_count;
        symbol = other.symbol;
        health = other.health;
        position = move(other.position);
        closest_enemy = move(other.closest_enemy);
    }
    return *this;
}

void Ship::show_status() const
{
    cout << "Ship [" << symbol << "] " << "Ship Type [" << ship_type << "]" << " - Team: " << team_name
         << " | Health: " << health
         << " | Position: (" << position->first << "," << position->second << ")" << endl;
}

pair<int, int> Ship::get_location() { return *position; }
char Ship::get_symbol() const { return symbol; }
string Ship::get_team_name() { return team_name; }
int Ship::get_id() { return id; }

void Ship::mark_for_removal() { marked_for_removal = true; }
bool Ship::is_marked_for_removal() const { return marked_for_removal; }

void Ship::killed()
{
    marked_for_removal = true; // Mark the ship for removal
}

string Ship::get_ship_type() { return ship_type; }
void Ship::set_ship_type(string type) { ship_type = type; }

void Ship::set_health(int hlth) { health = hlth; }
int Ship::get_health() { return health; }

int Ship::get_kill_count() { return kill_count; }
void Ship::update_kill_count() { kill_count++; }
void Ship::reset_kill_count() { kill_count = 0; }

int Ship::get_hit_count() { return hit_count; }
void Ship::update_hit_count() { hit_count++; }
void Ship::reset_hit_count() { hit_count = 0; }

Ship::~Ship() {}

SeeingShip::SeeingShip(int id, string team, char sym, pair<int, int> *position)
    : Ship(id, team, sym, position) {}
void SeeingShip::seeing(const vector<unique_ptr<Ship>> &ships)
{
    // Pure virtual function; implementation should be in derived classes
}
MovingShip::MovingShip(int id, string team, char sym, pair<int, int> *position)
    : Ship(id, team, sym, position) {}
void MovingShip::moving(pair<int, int> borders, const vector<pair<int, int>> &island_location)
{
    // Pure virtual function; implementation should be in derived classes
}
ShootingShip::ShootingShip(int id, string team, char sym, pair<int, int> *position)
    : Ship(id, team, sym, position) {}
void ShootingShip::shooting(vector<unique_ptr<Ship>> &ships)
{
    // Pure virtual function; implementation should be in derived classes
}
RammingShip::RammingShip(int id, string team, char sym, pair<int, int> *position)
    : Ship(id, team, sym, position) {}
void RammingShip::destroy(vector<unique_ptr<Ship>> &ships)
{
    // Pure virtual function; implementation should be in derived classes
}

// Example implementation for SuperShip
SuperShip::SuperShip(int id, string team, char sym, pair<int, int> *position)
    : SeeingShip(id, team, sym, position), MovingShip(id, team, sym, position),
      ShootingShip(id, team, sym, position), RammingShip(id, team, sym, position),
      Ship(id, team, sym, position)
{
    ship_type = "SuperShip";
}
void SuperShip::seeing(const vector<unique_ptr<Ship>> &ships)
{
    closest_enemy.clear(); // Clear previous entries

    for (const auto &ship : ships)
    {
        if (ship->get_team_name() != team_name) // Check if the ship is an enemy
        {
            pair<int, int> enemyPosition = ship->get_location();
            int distance = abs(enemyPosition.first - position->first) + abs(enemyPosition.second - position->second);

            if (distance <= 4) // If the enemy is within 5 blocks
            {
                cout << "[" << ship_type << ": " << id << symbol << "]" << "Enemy Located" << endl;
                closest_enemy.push_back(enemyPosition);
            }
        }
    }
}
void SuperShip::moving(pair<int, int> border, const vector<pair<int, int>> &island_location)
{
    int width = border.first;
    int height = border.second;

    // Determine the direction to move
    int newX = position->second;
    int newY = position->first;

    if (!closest_enemy.empty())
    {
        // Move towards the first enemy in the closest_enemy vector
        pair<int, int> target = closest_enemy[0];

        int deltaY = target.first - position->first;
        int deltaX = target.second - position->second;

        if (abs(deltaX) > abs(deltaY))
        {
            // Move horizontally
            if (deltaX > 0)
                newX++; // Move right
            else
                newX--; // Move left
        }
        else
        {
            // Move vertically
            if (deltaY > 0)
                newY++; // Move down
            else
                newY--; // Move up
        }
    }
    else
    {
        // Randomly select a direction (0: right, 1: left, 2: down, 3: up)
        int direction = rand() % 4;

        switch (direction)
        {
        case 0: // Right
            newX++;
            break;
        case 1: // Left
            newX--;
            break;
        case 2: // Down
            newY++;
            break;
        case 3: // Up
            newY--;
            break;
        }
    }

    // Check if the new position is within the battlefield bounds
    if (newX >= 0 && newX < width && newY >= 0 && newY < height)
    {
        // Check if the new position is an island
        bool isIsland = false;
        for (const auto &island : island_location)
        {
            if (island.second == newX && island.first == newY)
            {
                isIsland = true;
                break;
            }
        }

        // Update the ship's position if it's not an island
        if (!isIsland)
        {
            position->second = newX;
            position->first = newY;
            cout << "[" << ship_type << ": " << id << symbol << "] moved to (" << position->first << ", " << position->second << ")." << endl;
        }
        else
        {
            cout << "[" << ship_type << ": " << id << symbol << "] cannot move to (" << newX << ", " << newY << ") - Will be beached." << endl;
        }
    }
    else
    {
        cout << "[" << ship_type << ": " << id << symbol << "] cannot move to (" << newX << ", " << newY << ") - Will be considered Deserter." << endl;
    }
}
void SuperShip::shooting(vector<unique_ptr<Ship>> &ships)
{
    if (!closest_enemy.empty())
    {
        // Iterate over all detected enemies
        for (const auto &enemy_pos : closest_enemy)
        {
            // Check if the enemy is within shooting range (e.g., 1 block away)
            int distance = abs(enemy_pos.first - position->first) + abs(enemy_pos.second - position->second);
            if (distance <= 2) // Assuming shooting range is 1 block
            {
                // cout << "Ship [" << symbol << "] within distance!" << endl;
                // Find the enemy ship in the ships vector
                for (auto &ship : ships)
                {
                    if (ship->get_location() == enemy_pos && ship->get_team_name() != team_name) // Ensure it's not a friendly ship
                    {
                        // cout << "Ship [" << symbol << "] confirmed ship is not friendly!" << endl;
                        // 1/2 chance to miss (1/2 chance to hit)
                        if ((rand() % 2) == 0) // 1 in 2 chance to hit
                        {
                            // cout << "Ship [" << symbol << "] taking a shot!" << endl;
                            // Reduce enemy health by 1
                            ship->set_health(ship->get_health() - 2);
                            update_hit_count(); // Increase hit count for the shooter

                            cout << "Ship [" << id << symbol << "] hit Ship [" << ship->get_id() << ship->get_symbol() << "]!" << endl;

                            if (ship->get_health() <= 0)
                            {
                                cout << "Ship [" << ship->get_id() << ship->get_symbol() << "] has been destroyed by Ship [" << id << symbol << "]!" << endl;
                                update_kill_count(); // Increase kill count for the shooter
                                ship->killed();      // Mark the ship as destroyed
                            }
                        }

                        else
                        {
                            cout << "Ship [" << id << symbol << "] missed the shot!" << endl;
                        }
                        break; // Exit after attempting to shoot the first enemy in range
                    }
                }
            }
        }
    }
}
void SuperShip::destroy(vector<unique_ptr<Ship>> &ships)
{
    for (int i = 0; i < ships.size(); ++i)
    {
        if (ships[i]->get_location() == *position && ships[i].get() != this && ships[i]->get_team_name() != this->get_team_name()) // Avoid self-collision & team mates
        {
            cout << "Ship [" << symbol << "] rammed and destroyed Ship [" << ships[i]->get_symbol() << "]!" << endl;

            ships[i]->set_health(0);
            ships[i]->killed();

            update_kill_count();

            break; // Exit after the first collision
        }
    }
}
unique_ptr<Ship> SuperShip::upgrade()
{
    return nullptr;
}

Destroyer::Destroyer(int id, string team, char sym, pair<int, int> *position)
    : SeeingShip(id, team, sym, position), MovingShip(id, team, sym, position),
      ShootingShip(id, team, sym, position), RammingShip(id, team, sym, position),
      Ship(id, team, sym, position)
{
    ship_type = "Destroyer";
}
void Destroyer::seeing(const vector<unique_ptr<Ship>> &ships)
{
    closest_enemy.clear(); // Clear previous entries

    for (const auto &ship : ships)
    {
        if (ship->get_team_name() != team_name) // Check if the ship is an enemy
        {
            pair<int, int> enemyPosition = ship->get_location();
            int distance = abs(enemyPosition.first - position->first) + abs(enemyPosition.second - position->second);

            if (distance <= 4) // If the enemy is within 5 blocks
            {
                cout << "[" << ship_type << ": " << id << symbol << "]" << "Enemy Located" << endl;
                closest_enemy.push_back(enemyPosition);
            }
        }
    }
}
void Destroyer::moving(pair<int, int> border, const vector<pair<int, int>> &island_location)
{
    int width = border.first;
    int height = border.second;

    // Determine the direction to move
    int newX = position->second;
    int newY = position->first;

    if (!closest_enemy.empty())
    {
        // Move towards the first enemy in the closest_enemy vector
        pair<int, int> target = closest_enemy[0];

        int deltaY = target.first - position->first;
        int deltaX = target.second - position->second;

        if (abs(deltaX) > abs(deltaY))
        {
            // Move horizontally
            if (deltaX > 0)
                newX++; // Move right
            else
                newX--; // Move left
        }
        else
        {
            // Move vertically
            if (deltaY > 0)
                newY++; // Move down
            else
                newY--; // Move up
        }
    }
    else
    {
        // Randomly select a direction (0: right, 1: left, 2: down, 3: up)
        int direction = rand() % 4;

        switch (direction)
        {
        case 0: // Right
            newX++;
            break;
        case 1: // Left
            newX--;
            break;
        case 2: // Down
            newY++;
            break;
        case 3: // Up
            newY--;
            break;
        }
    }

    // Check if the new position is within the battlefield bounds
    if (newX >= 0 && newX < width && newY >= 0 && newY < height)
    {
        // Check if the new position is an island
        bool isIsland = false;
        for (const auto &island : island_location)
        {
            if (island.second == newX && island.first == newY)
            {
                isIsland = true;
                break;
            }
        }

        // Update the ship's position if it's not an island
        if (!isIsland)
        {
            position->second = newX;
            position->first = newY;
            cout << "[" << ship_type << ": " << id << symbol << "] moved to (" << position->first << ", " << position->second << ")." << endl;
        }
        else
        {
            cout << "[" << ship_type << ": " << id << symbol << "] cannot move to (" << newX << ", " << newY << ") - Will be beached." << endl;
        }
    }
    else
    {
        cout << "[" << ship_type << ": " << id << symbol << "] cannot move to (" << newX << ", " << newY << ") - Will be considered Deserter." << endl;
    }
}
void Destroyer::shooting(vector<unique_ptr<Ship>> &ships)
{
    if (!closest_enemy.empty())
    {
        // Iterate over all detected enemies
        for (const auto &enemy_pos : closest_enemy)
        {
            // Check if the enemy is within shooting range (e.g., 1 block away)
            int distance = abs(enemy_pos.first - position->first) + abs(enemy_pos.second - position->second);
            if (distance <= 2) // Assuming shooting range is 1 block
            {
                // cout << "Ship [" << symbol << "] within distance!" << endl;
                // Find the enemy ship in the ships vector
                for (auto &ship : ships)
                {
                    if (ship->get_location() == enemy_pos && ship->get_team_name() != team_name) // Ensure it's not a friendly ship
                    {
                        // cout << "Ship [" << symbol << "] confirmed ship is not friendly!" << endl;
                        // 1/2 chance to miss (1/2 chance to hit)
                        if ((rand() % 2) == 0) // 1 in 2 chance to hit
                        {
                            // cout << "Ship [" << symbol << "] taking a shot!" << endl;
                            // Reduce enemy health by 1
                            ship->set_health(ship->get_health() - 1);
                            update_hit_count(); // Increase hit count for the shooter

                            cout << "Ship [" << id << symbol << "] hit Ship [" << ship->get_id() << ship->get_symbol() << "]!" << endl;

                            if (ship->get_health() <= 0)
                            {
                                cout << "Ship [" << ship->get_id() << ship->get_symbol() << "] has been destroyed by Ship [" << id << symbol << "]!" << endl;
                                update_kill_count(); // Increase kill count for the shooter
                                ship->killed();      // Mark the ship as destroyed
                            }
                        }

                        else
                        {
                            cout << "Ship [" << id << symbol << "] missed the shot!" << endl;
                        }
                        break; // Exit after attempting to shoot the first enemy in range
                    }
                }
            }
        }
    }
}
void Destroyer::destroy(vector<unique_ptr<Ship>> &ships)
{
    for (int i = 0; i < ships.size(); ++i)
    {
        if (ships[i]->get_location() == *position && ships[i].get() != this && ships[i]->get_team_name() != this->get_team_name()) // Avoid self-collision & team mates
        {
            cout << "Ship [" << symbol << "] rammed and destroyed Ship [" << ships[i]->get_symbol() << "]!" << endl;

            ships[i]->set_health(0);
            ships[i]->killed();

            update_kill_count();

            break; // Exit after the first collision
        }
    }
}
unique_ptr<Ship> Destroyer::upgrade()
{
    if (kill_count >= 3)
    { // Upgrade condition for Cruiser
        cout << "Destroyer is upgrading to SuperShip!" << endl;
        return unique_ptr<Ship>(new SuperShip(id, team_name, symbol, position));
    }
    return nullptr; // No upgrade
}

BattleShip::BattleShip(int id, string team, char sym, pair<int, int> *position)
    : SeeingShip(id, team, sym, position), MovingShip(id, team, sym, position),
      ShootingShip(id, team, sym, position), RammingShip(id, team, sym, position),
      Ship(id, team, sym, position)
{
    ship_type = "Battleship";
}
void BattleShip::seeing(const vector<unique_ptr<Ship>> &ships)
{
    closest_enemy.clear(); // Clear previous entries

    for (const auto &ship : ships)
    {
        if (ship->get_team_name() != team_name) // Check if the ship is an enemy
        {
            pair<int, int> enemyPosition = ship->get_location();
            int distance = abs(enemyPosition.first - position->first) + abs(enemyPosition.second - position->second);

            if (distance <= 5) // If the enemy is within 5 blocks
            {
                cout << "[" << ship_type << ": " << id << symbol << "] Enemy Located" << endl;
                closest_enemy.push_back(enemyPosition);
            }
        }
    }
}
void BattleShip::moving(pair<int, int> border, const vector<pair<int, int>> &island_location)
{
    int width = border.first;
    int height = border.second;

    // Determine the direction to move
    int newX = position->second;
    int newY = position->first;

    if (!closest_enemy.empty())
    {
        // Move towards the first enemy in the closest_enemy vector
        pair<int, int> target = closest_enemy[0];

        int deltaY = target.first - position->first;
        int deltaX = target.second - position->second;

        int distance = abs(deltaX) + abs(deltaY);

        // If the enemy is within 3 blocks, try to maintain that distance
        if (distance <= 3)
        {
            // Move away from the enemy
            if (abs(deltaX) > abs(deltaY))
            {
                // Move horizontally
                if (deltaX > 0)
                    newX--; // Move left
                else
                    newX++; // Move right
            }
            else
            {
                // Move vertically
                if (deltaY > 0)
                    newY--; // Move up
                else
                    newY++; // Move down
            }
        }
        else
        {
            // Move towards the enemy to get within 3 blocks
            if (abs(deltaX) > abs(deltaY))
            {
                // Move horizontally
                if (deltaX > 0)
                    newX++; // Move right
                else
                    newX--; // Move left
            }
            else
            {
                // Move vertically
                if (deltaY > 0)
                    newY++; // Move down
                else
                    newY--; // Move up
            }
        }
    }
    else
    {
        // Randomly select a direction (0: right, 1: left, 2: down, 3: up)
        int direction = rand() % 4;

        switch (direction)
        {
        case 0: // Right
            newX++;
            break;
        case 1: // Left
            newX--;
            break;
        case 2: // Down
            newY++;
            break;
        case 3: // Up
            newY--;
            break;
        }
    }

    // Check if the new position is within the battlefield bounds
    if (newX >= 0 && newX < width && newY >= 0 && newY < height)
    {
        // Check if the new position is an island
        bool isIsland = false;
        for (const auto &island : island_location)
        {
            if (island.second == newX && island.first == newY)
            {
                isIsland = true;
                break;
            }
        }

        // Update the ship's position if it's not an island
        if (!isIsland)
        {
            position->second = newX;
            position->first = newY;
            cout << "[" << ship_type << ": " << id << symbol << "] moved to (" << position->first << ", " << position->second << ")." << endl;
        }
        else
        {
            cout << "[" << ship_type << ": " << id << symbol << "] cannot move to (" << newX << ", " << newY << ") - Will be beached." << endl;
        }
    }
    else
    {
        cout << "[" << ship_type << ": " << id << symbol << "] cannot move to (" << newX << ", " << newY << ") - Will be considered Deserter." << endl;
    }
}
void BattleShip::shooting(vector<unique_ptr<Ship>> &ships)
{
    if (!closest_enemy.empty())
    {
        // Iterate over all detected enemies
        for (const auto &enemy_pos : closest_enemy)
        {
            // Check if the enemy is within shooting range (e.g., 1 block away)
            int distance = abs(enemy_pos.first - position->first) + abs(enemy_pos.second - position->second);
            if (distance <= 2) // Assuming shooting range is 1 block
            {
                // cout << "Ship [" << symbol << "] within distance!" << endl;
                // Find the enemy ship in the ships vector
                for (auto &ship : ships)
                {
                    if (ship->get_location() == enemy_pos && ship->get_team_name() != team_name) // Ensure it's not a friendly ship
                    {
                        // cout << "Ship [" << symbol << "] confirmed ship is not friendly!" << endl;
                        // 1/2 chance to miss (1/2 chance to hit)
                        if ((rand() % 2) == 0) // 1 in 2 chance to hit
                        {
                            // cout << "Ship [" << symbol << "] taking a shot!" << endl;
                            // Reduce enemy health by 1
                            ship->set_health(ship->get_health() - 1);
                            update_hit_count(); // Increase hit count for the shooter

                            cout << "Ship [" << id << symbol << "] hit Ship [" << ship->get_id() << ship->get_symbol() << "]!" << endl;

                            if (ship->get_health() <= 0)
                            {
                                cout << "Ship [" << ship->get_id() << ship->get_symbol() << "] has been destroyed by Ship [" << id << symbol << "]!" << endl;
                                update_kill_count(); // Increase kill count for the shooter
                                ship->killed();      // Mark the ship as destroyed
                            }
                        }

                        else
                        {
                            cout << "Ship [" << id << symbol << "] missed the shot!" << endl;
                        }
                        break; // Exit after attempting to shoot the first enemy in range
                    }
                }
            }
        }
    }
}
void BattleShip::destroy(vector<unique_ptr<Ship>> &ships) {}
unique_ptr<Ship> BattleShip::upgrade()
{
    if (hit_count >= 4)
    { // Upgrade condition for BattleShip
        cout << "BattleShip is upgrading to Destroyer!" << endl;
        return unique_ptr<Ship>(new Destroyer(id, team_name, symbol, position));
    }
    return nullptr; // No upgrade
}

Cruiser::Cruiser(int id, string team, char sym, pair<int, int> *position)
    : SeeingShip(id, team, sym, position), MovingShip(id, team, sym, position),
      ShootingShip(id, team, sym, position), RammingShip(id, team, sym, position),
      Ship(id, team, sym, position)
{
    ship_type = "Cruiser";
}
void Cruiser::seeing(const vector<unique_ptr<Ship>> &ships)
{
    closest_enemy.clear(); // Clear previous entries

    for (const auto &ship : ships)
    {
        if (ship->get_team_name() != team_name)
        { // Check if the ship is an enemy
            pair<int, int> enemyPosition = ship->get_location();
            int distance = abs(enemyPosition.first - position->first) + abs(enemyPosition.second - position->second);

            if (distance <= 3)
            { // If the enemy is within 3 blocks
                cout << "[" << ship_type << ": " << id << symbol << "] Enemy Located" << endl;
                closest_enemy.push_back(enemyPosition);
            }
        }
    }
}
void Cruiser::moving(pair<int, int> border, const vector<pair<int, int>> &island_location)
{
    int width = border.first;
    int height = border.second;

    // Determine the direction to move
    int newX = position->second;
    int newY = position->first;

    if (!closest_enemy.empty())
    {
        // Move towards the first enemy in the closest_enemy vector
        pair<int, int> target = closest_enemy[0];

        int deltaY = target.first - position->first;
        int deltaX = target.second - position->second;

        if (abs(deltaX) > abs(deltaY))
        {
            // Move horizontally
            if (deltaX > 0)
                newX++; // Move right
            else
                newX--; // Move left
        }
        else
        {
            // Move vertically
            if (deltaY > 0)
                newY++; // Move down
            else
                newY--; // Move up
        }
    }
    else
    {
        // Randomly select a direction (0: right, 1: left, 2: down, 3: up)
        int direction = rand() % 4;

        switch (direction)
        {
        case 0: // Right
            newX++;
            break;
        case 1: // Left
            newX--;
            break;
        case 2: // Down
            newY++;
            break;
        case 3: // Up
            newY--;
            break;
        }
    }

    // Check if the new position is within the battlefield bounds
    if (newX >= 0 && newX < width && newY >= 0 && newY < height)
    {
        // Check if the new position is an island
        bool isIsland = false;
        for (const auto &island : island_location)
        {
            if (island.second == newX && island.first == newY)
            {
                isIsland = true;
                break;
            }
        }

        // Update the ship's position if it's not an island
        if (!isIsland)
        {
            position->second = newX;
            position->first = newY;
            cout << "[" << ship_type << ": " << id << symbol << "] moved to (" << position->first << ", " << position->second << ")." << endl;
        }
        else
        {
            cout << "[" << ship_type << ": " << id << symbol << "] cannot move to (" << newX << ", " << newY << ") - Will be beached." << endl;
        }
    }
    else
    {
        cout << "[" << ship_type << ": " << id << symbol << "] cannot move to (" << newX << ", " << newY << ") - Will be considered a Deserter." << endl;
    }
}
void Cruiser::shooting(vector<unique_ptr<Ship>> &ships)
{
    // Implement shooting logic if needed
}
void Cruiser::destroy(vector<unique_ptr<Ship>> &ships)
{
    for (int i = 0; i < ships.size(); ++i)
    {
        if (ships[i]->get_location() == *position && ships[i].get() != this && ships[i]->get_team_name() != this->get_team_name())
        { // Avoid self-collision & teammates
            cout << "Ship [" << symbol << "] rammed and destroyed Ship [" << ships[i]->get_symbol() << "]!" << endl;

            ships[i]->set_health(0);
            ships[i]->killed();

            update_kill_count();

            break; // Exit after the first collision
        }
    }
}
unique_ptr<Ship> Cruiser::upgrade()
{
    if (kill_count >= 3)
    { // Upgrade condition for Cruiser
        cout << "Cruiser is upgrading to Destroyer!" << endl;
        return unique_ptr<Ship>(new Destroyer(id, team_name, symbol, position));
    }
    return nullptr; // No upgrade
}

Corvette::Corvette(int id, string team, char sym, pair<int, int> *position)
    : SeeingShip(id, team, sym, position), MovingShip(id, team, sym, position),
      ShootingShip(id, team, sym, position), RammingShip(id, team, sym, position),
      Ship(id, team, sym, position)
{
    ship_type = "Corvette";
}
void Corvette::seeing(const vector<unique_ptr<Ship>> &ships) {}
void Corvette::moving(pair<int, int> border, const vector<pair<int, int>> &island_location) {}
void Corvette::shooting(vector<unique_ptr<Ship>> &ships)
{
    pair<int, int> current_loc = *position;

    // Find the closest enemy in the current direction
    int closest_distance = INT_MAX;
    Ship *target_ship = nullptr;

    for (auto &ship : ships)
    {
        if (ship->get_team_name() != team_name) // Ensure it's an enemy ship
        {
            pair<int, int> enemy_loc = ship->get_location();
            int distance = abs(enemy_loc.first - current_loc.first) + abs(enemy_loc.second - current_loc.second);

            // Check if the enemy is in the current direction
            bool in_direction = false;
            if (enemy_loc.second < current_loc.second && enemy_loc.first == current_loc.first)
                in_direction = true;
            else if (enemy_loc.first > current_loc.first && enemy_loc.second == current_loc.second)
                in_direction = true;
            else if (enemy_loc.second > current_loc.second && enemy_loc.first == current_loc.first)
                in_direction = true;
            else if (enemy_loc.first < current_loc.first && enemy_loc.second == current_loc.second)
                in_direction = true;

            if (in_direction && distance < closest_distance)
            {
                closest_distance = distance;
                target_ship = ship.get();
            }
        }
    }

    // Shoot the target if found
    if (target_ship)
    {
        cout << "Ship [" << target_ship->get_id() << target_ship->get_symbol() << "] has been destroyed by Ship [" << id << symbol << "]!" << endl;

        target_ship->set_health(target_ship->get_health() - 1);
        update_hit_count(); // Increase hit count for the shooter

        if (target_ship->get_health() <= 0)
        {
            cout << "Ship [" << target_ship->get_id() << target_ship->get_symbol() << "] has been destroyed by Frigate [" << id << symbol << "]!" << endl;
            update_kill_count();   // Increase kill count for the shooter
            target_ship->killed(); // Mark the ship as destroyed
        }
    }
}
void Corvette::destroy(vector<unique_ptr<Ship>> &ships) {}
unique_ptr<Ship> Corvette::upgrade() { return nullptr; }

Frigate::Frigate(int id, string team, char sym, pair<int, int> *position)
    : SeeingShip(id, team, sym, position), MovingShip(id, team, sym, position),
      ShootingShip(id, team, sym, position), RammingShip(id, team, sym, position),
      Ship(id, team, sym, position)
{
    ship_type = "Frigate";
}
void Frigate::seeing(const vector<unique_ptr<Ship>> &ships) {}
void Frigate::moving(pair<int, int> border, const vector<pair<int, int>> &island_location) {}
void Frigate::shooting(vector<unique_ptr<Ship>> &ships)
{
    pair<int, int> current_loc = *position;

    // Determine the direction based on shooting_index
    string direction;
    switch (shooting_index % 4)
    {
    case 0:
        direction = "Up";
        break;
    case 1:
        direction = "Right";
        break;
    case 2:
        direction = "Down";
        break;
    case 3:
        direction = "Left";
        break;
    }

    // Find the closest enemy in the current direction
    int closest_distance = INT_MAX;
    Ship *target_ship = nullptr;

    for (auto &ship : ships)
    {
        if (ship->get_team_name() != team_name) // Ensure it's an enemy ship
        {
            pair<int, int> enemy_loc = ship->get_location();
            int distance = abs(enemy_loc.first - current_loc.first) + abs(enemy_loc.second - current_loc.second);

            // Check if the enemy is in the current direction
            bool in_direction = false;
            if (direction == "Up" && enemy_loc.second < current_loc.second && enemy_loc.first == current_loc.first)
                in_direction = true;
            else if (direction == "Right" && enemy_loc.first > current_loc.first && enemy_loc.second == current_loc.second)
                in_direction = true;
            else if (direction == "Down" && enemy_loc.second > current_loc.second && enemy_loc.first == current_loc.first)
                in_direction = true;
            else if (direction == "Left" && enemy_loc.first < current_loc.first && enemy_loc.second == current_loc.second)
                in_direction = true;

            if (in_direction && distance < closest_distance)
            {
                closest_distance = distance;
                target_ship = ship.get();
            }
        }
    }

    // Shoot the target if found
    if (target_ship)
    {
        cout << "Frigate [" << id << symbol << "] shooting " << direction << " at Ship [" << target_ship->get_id() << target_ship->get_symbol() << "]" << endl;

        target_ship->set_health(target_ship->get_health() - 1);
        update_hit_count(); // Increase hit count for the shooter

        if (target_ship->get_health() <= 0)
        {
            cout << "Ship [" << target_ship->get_id() << target_ship->get_symbol() << "] has been destroyed by Frigate [" << id << symbol << "]!" << endl;
            update_kill_count();   // Increase kill count for the shooter
            target_ship->killed(); // Mark the ship as destroyed
        }

        shooting_index++; // Move to the next direction for the next shot
    }
    else
    {
        cout << "Frigate [" << id << symbol << "] found no target in direction " << direction << endl;
        shooting_index++; // Move to the next direction if no target is found
    }
}
void Frigate::destroy(vector<unique_ptr<Ship>> &ships) {}
unique_ptr<Ship> Frigate::upgrade()
{
    if (hit_count >= 3)
    { // Upgrade condition for Cruiser
        cout << "Frigate is upgrading to Corvette!" << endl;
        return unique_ptr<Ship>(new Corvette(id, team_name, symbol, position));
    }
    return nullptr; // No upgrade
}

Amphibious::Amphibious(int id, string team, char sym, pair<int, int> *position)
    : SeeingShip(id, team, sym, position), MovingShip(id, team, sym, position),
      ShootingShip(id, team, sym, position), RammingShip(id, team, sym, position),
      Ship(id, team, sym, position)
{
    ship_type = "Amphibious";
}
void Amphibious::seeing(const vector<unique_ptr<Ship>> &ships)
{
    closest_enemy.clear(); // Clear previous entries

    for (const auto &ship : ships)
    {
        if (ship->get_team_name() != team_name) // Check if the ship is an enemy
        {
            pair<int, int> enemyPosition = ship->get_location();
            int distance = abs(enemyPosition.first - position->first) + abs(enemyPosition.second - position->second);

            if (distance <= 5) // If the enemy is within 5 blocks
            {
                cout << "[" << ship_type << ": " << id << symbol << "] Enemy Located" << endl;
                closest_enemy.push_back(enemyPosition);
            }
        }
    }
}
void Amphibious::moving(pair<int, int> border, const vector<pair<int, int>> &island_location)
{
    int width = border.first;
    int height = border.second;

    // Determine the direction to move
    int newX = position->second;
    int newY = position->first;

    if (!closest_enemy.empty())
    {
        // Move towards the first enemy in the closest_enemy vector
        pair<int, int> target = closest_enemy[0];

        int deltaY = target.first - position->first;
        int deltaX = target.second - position->second;

        int distance = abs(deltaX) + abs(deltaY);

        // If the enemy is within 3 blocks, try to maintain that distance
        if (distance <= 3)
        {
            // Move away from the enemy
            if (abs(deltaX) > abs(deltaY))
            {
                // Move horizontally
                if (deltaX > 0)
                    newX--; // Move left
                else
                    newX++; // Move right
            }
            else
            {
                // Move vertically
                if (deltaY > 0)
                    newY--; // Move up
                else
                    newY++; // Move down
            }
        }
        else
        {
            // Move towards the enemy
            if (abs(deltaX) > abs(deltaY))
            {
                // Move horizontally
                if (deltaX > 0)
                    newX++; // Move right
                else
                    newX--; // Move left
            }
            else
            {
                // Move vertically
                if (deltaY > 0)
                    newY++; // Move down
                else
                    newY--; // Move up
            }
        }
    }

    // Ensure the new position is within bounds
    newX = max(0, min(newX, width - 1));
    newY = max(0, min(newY, height - 1));

    position->first = newY;  // Update the Y coordinate
    position->second = newX; // Update the X coordinate

    cout << "[" << ship_type << ": " << id << symbol << "] Moving to (" << position->first << ", " << position->second << ")" << endl;
}
void Amphibious::shooting(vector<unique_ptr<Ship>> &ships)
{
    pair<int, int> current_loc = *position;

    // Check if there are enemies nearby
    for (const auto &enemy_loc : closest_enemy)
    {
        if (current_loc.first == enemy_loc.first || current_loc.second == enemy_loc.second) // Check if in line of sight
        {
            for (auto &ship : ships)
            {
                if (ship->get_location() == enemy_loc)
                {
                    cout << "[" << ship_type << ": " << id << symbol << "] Shooting enemy at (" << enemy_loc.first << ", " << enemy_loc.second << ")" << endl;
                    ship->set_health(ship->get_health() - 1);
                    update_hit_count(); // Increase hit count for the shooter

                    if (ship->get_health() <= 0)
                    {
                        cout << "Ship [" << ship->get_id() << ship->get_symbol() << "] has been destroyed by Amphibious [" << id << symbol << "]!" << endl;
                        update_kill_count(); // Increase kill count for the shooter
                        ship->killed();      // Mark the ship as destroyed
                    }
                }
            }
        }
    }
}
void Amphibious::destroy(vector<unique_ptr<Ship>> &ships) {}
unique_ptr<Ship> Amphibious::upgrade() { return nullptr; }
