/**
 * Game.cpp
 * Implementation of the Game and Board classes for a Pacman-style game.
 * Includes board initialization, player movement, enemy AI, and game state management.
 */

#include "Game.h"
#include <iostream>
#include <cstdlib>

/**
 * Converts a SquareType enum to its string (emoji) representation.
 * 
 * @param sq The SquareType to convert
 * @return String (emoji) representation of the square type
 */
std::string SquareTypeStringify(SquareType sq) {
    switch (sq) {
        case SquareType::Wall: return "🟩";      // green cross
        case SquareType::Dots: return "🟧";      // orange square
        case SquareType::Pacman: return "😊";    // smiley face
        case SquareType::Treasure: return "🎁";   // gift
        case SquareType::Enemies: return "👻";    // ghost
        case SquareType::Empty: return "⬜";       // empty space
        case SquareType::PowerfulPacman: return "😎"; // cool face for powered up
        case SquareType::SpecialDots: return "🟦";      // blue square (special pellet)
        case SquareType::EnemySpecialTreasure: return "🦹"; // enemy power-up sword
        case SquareType::Trap: return "🪤 ";      // visible trap
        case SquareType::HiddenTrap: return "🟧"; // hidden trap (looks like a normal dot)
        default: return "❓";                     // unknown
    }
}

/**
 * Initializes a new board with the specified number of enemies.
 * Sets up walls, dots, traps, treasures, and enemies in their starting positions.
 * 
 * @param num_enemies Number of enemies to place on the board (default: 2)
 */
Board::Board(int num_enemies) {
    rows_ = get_rows();
    cols_ = get_cols();
    // Initialize board with pellets squares and mark nothing as collected
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            arr_[i][j] = SquareType::Dots;
            collected_[i][j] = false;
        }
    }
    //set pacman to middle, and walls around the edges
    for (int i = 0; i < rows_; i++) {
        arr_[i][0] = SquareType::Wall;
        arr_[i][cols_-1] = SquareType::Wall;
        collected_[i][0] = true;  // Mark walls as collected so they don't respawn as dots
        collected_[i][cols_-1] = true;
    }
    for (int j = 0; j < cols_; j++) {
        arr_[0][j] = SquareType::Wall;
        arr_[rows_-1][j] = SquareType::Wall;
        collected_[0][j] = true;  // Mark walls as collected
        collected_[rows_-1][j] = true;
    }
    arr_[5][5] = SquareType::Pacman;
    collected_[5][5] = true;  // Mark Pacman's starting position as collected

    //add additional walls
    arr_[2][2] = SquareType::Wall;
    arr_[2][3] = SquareType::Wall;
    arr_[2][4] = SquareType::Wall;
    arr_[3][2] = SquareType::Wall;
    collected_[2][2] = true;  // Mark walls as collected
    collected_[2][3] = true;
    collected_[2][4] = true;
    collected_[3][2] = true;

    arr_[6][2] = SquareType::Wall;
    arr_[7][3] = SquareType::Wall;
    arr_[3][6] = SquareType::Wall;
    arr_[3][7] = SquareType::Wall;
    arr_[4][7] = SquareType::Wall;
    arr_[7][7] = SquareType::Wall;
    arr_[4][4] = SquareType::Wall;
    arr_[5][4] = SquareType::Wall;
    collected_[6][2] = true;  // Mark walls as collected
    collected_[7][3] = true;
    collected_[3][6] = true;
    collected_[3][7] = true;
    collected_[4][7] = true;
    collected_[7][7] = true;
    collected_[4][4] = true;
    collected_[5][4] = true;
    
    // Add the two traps (before adding treasures and enemies)
    // First, add the hidden trap (avoiding 3x3 grid around spawn point)
    bool hidden_trap_placed = false;
    while (!hidden_trap_placed) {
        int rand_row = rand() % rows_;
        int rand_col = rand() % cols_;
        // Check if position is valid (not in 3x3 grid around spawn and not a wall)
        if (abs(rand_row - 5) > 1 || abs(rand_col - 5) > 1) { // spawn is at 5,5
            if (arr_[rand_row][rand_col] == SquareType::Dots) {
                arr_[rand_row][rand_col] = SquareType::HiddenTrap;
                hidden_trap_placed = true;
            }
        }
    }

    // Next, add the visible trap (same restrictions)
    bool visible_trap_placed = false;
    while (!visible_trap_placed) {
        int rand_row = rand() % rows_;
        int rand_col = rand() % cols_;
        // Check if position is valid (not in 3x3 grid around spawn and not a wall or hidden trap)
        if (abs(rand_row - 5) > 1 || abs(rand_col - 5) > 1) {
            if (arr_[rand_row][rand_col] == SquareType::Dots) {
                arr_[rand_row][rand_col] = SquareType::Trap;
                visible_trap_placed = true;
            }
        }
    }

    // for all spaces not walls or middle space, first handle treasures, then special dots
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            if (arr_[i][j] == SquareType::Dots) {  // Only consider dot spaces
                // First determine if this should be a treasure space (15% chance)
                if (rand() % 100 < 15) {
                    // 80% chance for regular treasure, 20% for enemy treasure
                    if (rand() % 5 != 0) {
                        arr_[i][j] = SquareType::Treasure;
                    } else {
                        arr_[i][j] = SquareType::EnemySpecialTreasure;
                    }
                } else if (rand() % 100 < 10) {  // If not a treasure, 10% chance for special dot
                    arr_[i][j] = SquareType::SpecialDots;
                }
            }
        }
    }
    //finally, add enemies randomly onto dot spaces
    num_enemies = std::min(8, std::max(1, num_enemies));  // Ensure between 1 and 8 enemies
    for (int i = 0; i < num_enemies; i++) {
        int rand_row = rand() % rows_;
        int rand_col = rand() % cols_;
        while (arr_[rand_row][rand_col] != SquareType::Dots) {
            rand_row = rand() % rows_;
            rand_col = rand() % cols_;
        }
        arr_[rand_row][rand_col] = SquareType::Enemies;
    }
}

/**
 * Gets the value of a square at the specified position.
 * 
 * @param pos Position to check
 * @return SquareType value at the position
 */
SquareType Board::get_square_value(Position pos) const {
    return arr_[pos.row][pos.col];
}

/**
 * Sets the value of a square at the specified position.
 * 
 * @param pos Position to set
 * @param value New SquareType value
 */
void Board::SetSquareValue(Position pos, SquareType value) {
    arr_[pos.row][pos.col] = value;
}

/**
 * Gets all possible moves for a player from their current position.
 * Checks all four directions and excludes walls.
 * 
 * @param p Pointer to the player
 * @return Vector of valid positions the player can move to
 */
std::vector<Position> Board::GetMoves(Player *p) {
    std::vector<Position> moves;
    // Check all 4 directions
    if (arr_[p->get_position().row-1][p->get_position().col] != SquareType::Wall) {
        moves.push_back(Position{p->get_position().row-1, p->get_position().col});
    }
    if (arr_[p->get_position().row+1][p->get_position().col] != SquareType::Wall) {
        moves.push_back(Position{p->get_position().row+1, p->get_position().col});
    }
    if (arr_[p->get_position().row][p->get_position().col-1] != SquareType::Wall) {
        moves.push_back(Position{p->get_position().row, p->get_position().col-1});
    }
    if (arr_[p->get_position().row][p->get_position().col+1] != SquareType::Wall) {
        moves.push_back(Position{p->get_position().row, p->get_position().col+1});
    }
    return moves;
}

/**
 * Moves a player to a new position on the board.
 * Handles interactions with enemies, treasures, traps, and other game elements.
 * 
 * @param p The player to move
 * @param pos The target position
 * @param enemylist List of enemies in the game
 * @return true if move was successful, false otherwise
 */
bool Board::MovePlayer(Player *p, Position pos, std::vector<Player*>& enemylist) {
    // Check if move is valid
    std::vector<Position> valid_moves = GetMoves(p);
    bool valid_move = false;
    for (const Position& move : valid_moves) {
        if (pos == move) {
            valid_move = true;
            break;
        }
    }
    if (!valid_move) return false;

    SquareType target = get_square_value(pos);

    // Handle trap interactions
    if (target == SquareType::Trap) {
        // Visible trap - instant game over
        std::cout << "\n💀 You hit the deadly trap! GAME OVER! 💀\n";
        p->setIsDead(true);
        p->setLives(0);  // Set lives to 0 to trigger game over
        return true;
    }
    if (target == SquareType::HiddenTrap) {
        // Hidden trap - lose a life and clear the trap
        std::cout << "\n⚠️  HIDDEN TRAP TRIGGERED! You lose a life! ⚠️\n";
        p->setIsDead(true);
        // Clear old position and move to new position
        SetSquareValue(p->get_position(), SquareType::Empty);
        SetSquareValue(pos, SquareType::Empty);  // Remove the trap after it's triggered
        collected_[pos.row][pos.col] = true;  // Mark as collected
        p->SetPosition(pos);
        return true;
    }

    // Handle enemy treasure as wall for player
    if (target == SquareType::EnemySpecialTreasure) {
        return false;  // Acts as a wall for players
    }

    // Handle collisions based on treasure status
    if (target == SquareType::Enemies) {
        // Find the enemy at this position
        Player* enemy = nullptr;
        for (Player* e : enemylist) {
            if (e->get_position() == pos) {
                enemy = e;
                break;
            }
        }
        
        if (enemy && enemy->hasEnemyTreasure() && p->hasTreasure()) {
            // Stalemate - both lose treasures
            std::cout << "\n⚔️  STALEMATE! Both you and the enemy lose your power-ups! ⚔️\n";
            enemy->setHasEnemyTreasure(false);
            p->setHasTreasure(false);
            SetSquareValue(p->get_position(), SquareType::Pacman);  // Update player icon to normal
            std::cout << *this << std::endl;
            return false;  // Don't move, just lose treasures
        }
        
        if (enemy && enemy->hasEnemyTreasure()) {
            // Enemy has treasure but player doesn't - player dies
            p->setIsDead(true);
            return false;
        }

        if (p->hasTreasure()) {
            p->setHasTreasure(false);  // Lose power-up
            
            // Find and remove the enemy
            for (auto it = enemylist.begin(); it != enemylist.end(); ++it) {
                if ((*it)->get_position() == pos) {
                    delete *it;
                    enemylist.erase(it);
                    break;
                }
            }
            
            // Move player
            SetSquareValue(p->get_position(), SquareType::Empty);
            SetSquareValue(pos, SquareType::Pacman);
            p->SetPosition(pos);
            collected_[pos.row][pos.col] = true;
            
            SpawnNewEnemy(enemylist);
            return true;
        } else {
            p->setIsDead(true);
            return false;
        }
    }

    // Handle treasure pickup
    if (target == SquareType::Treasure) {
        p->ChangePoints(100);
        p->setHasTreasure(true); 
        SetSquareValue(p->get_position(), SquareType::Empty);
        SetSquareValue(pos, SquareType::PowerfulPacman);
        p->SetPosition(pos);
        collected_[pos.row][pos.col] = true;  // Mark as collected
        return true;
    }

    // Handle other moves
    if (target == SquareType::Dots || target == SquareType::Empty || target == SquareType::SpecialDots) {
        // Update points
        if (target == SquareType::Dots) p->ChangePoints(1);
        if (target == SquareType::SpecialDots) p->ChangePoints(5);
        
        // Clear old position
        SetSquareValue(p->get_position(), SquareType::Empty);
        
        // Update player position
        p->SetPosition(pos);
        collected_[pos.row][pos.col] = true;  // Mark as collected
        
        // Set new position with correct sprite based on power state
        if (p->hasTreasure()) {
            SetSquareValue(pos, SquareType::PowerfulPacman);
        } else {
            SetSquareValue(pos, SquareType::Pacman);
        }
        
        return true;
    }

    return false;
}

/**
 * Spawns a new enemy at a random valid position on the board.
 * Valid positions are empty spaces or dots.
 * 
 * @param enemylist Vector of enemy pointers to add the new enemy to
 */
void Board::SpawnNewEnemy(std::vector<Player*>& enemylist) {
    int attempts = 0;
    int rand_row, rand_col;
    bool found_position = false;
    
    // Try random positions first
    while (!found_position && attempts < 100) {
        rand_row = rand() % rows_;
        rand_col = rand() % cols_;
        
        if (arr_[rand_row][rand_col] == SquareType::Empty || 
            arr_[rand_row][rand_col] == SquareType::Dots) {
            found_position = true;
        }
        attempts++;
    }
    
    // If no random position found, scan systematically
    if (!found_position) {
        for (int i = 0; i < rows_ && !found_position; i++) {
            for (int j = 0; j < cols_; j++) {
                if (arr_[i][j] == SquareType::Empty || 
                    arr_[i][j] == SquareType::Dots) {
                    rand_row = i;
                    rand_col = j;
                    found_position = true;
                    break;
                }
            }
        }
    }
    
    if (found_position) {
        Position new_pos{rand_row, rand_col};
        Player* new_enemy = new Player("Enemy", false, new_pos);
        enemylist.push_back(new_enemy);
        SetSquareValue(new_pos, SquareType::Enemies);
    }
}

/**
 * Moves an enemy to a new position, handling interactions with players and treasures.
 * 
 * @param p Pointer to the enemy to move
 * @param pos Target position to move to
 * @param game Pointer to the current game instance
 * @return true if move was successful, false otherwise
 */
bool Board::MoveEnemy(Player *p, Position pos, Game* game) {
    SquareType target = get_square_value(pos);
    
    // Handle enemy treasure pickup
    if (target == SquareType::EnemySpecialTreasure) {
        p->setHasEnemyTreasure(true);
        SetSquareValue(p->get_position(), SquareType::Empty);
        SetSquareValue(pos, SquareType::Enemies);
        p->SetPosition(pos);
        collected_[pos.row][pos.col] = true;
        return true;
    }

    // Handle collision with player
    if (target == SquareType::Pacman || target == SquareType::PowerfulPacman) {
        Player* human = game->GetHumanPlayer();
        
        if (target == SquareType::PowerfulPacman && p->hasEnemyTreasure()) {
            // Stalemate - both lose treasures
            std::cout << "\n⚔️  STALEMATE! Both you and the enemy lose your power-ups! ⚔️\n";
            p->setHasEnemyTreasure(false);
            human->setHasTreasure(false);
            SetSquareValue(pos, SquareType::Pacman);  // Reset player to normal
            std::cout << *this << std::endl;
            return false;
        }
        
        if (target == SquareType::PowerfulPacman) {
            // Enemy dies when colliding with powered-up player
            return false;
        }
        
        // Enemy collides with regular player
        if (p->hasEnemyTreasure() || target == SquareType::Pacman) {
            Position old_pos = p->get_position();
            // Restore the special dot if we were on one
            if (arr_[old_pos.row][old_pos.col] == SquareType::Enemies && !collected_[old_pos.row][old_pos.col]) {
                SetSquareValue(old_pos, SquareType::SpecialDots);
            } else {
                SetSquareValue(old_pos, collected_[old_pos.row][old_pos.col] ? SquareType::Empty : SquareType::Dots);
            }
            SetSquareValue(pos, SquareType::Enemies);
            p->SetPosition(pos);
            
            if (game && game->GetHumanPlayer()) {
                game->GetHumanPlayer()->setIsDead(true);
            }
            return true;
        }
    }
    
    // Only move if target square is empty, has dots, or has special dots
    if (target == SquareType::Empty || target == SquareType::Dots || target == SquareType::SpecialDots) {
        Position old_pos = p->get_position();
        
        // Remember if we're moving onto a special dot
        bool was_special_dot = (target == SquareType::SpecialDots);
        
        // Restore the special dot if we were on one
        if (arr_[old_pos.row][old_pos.col] == SquareType::Enemies && !collected_[old_pos.row][old_pos.col]) {
            SetSquareValue(old_pos, SquareType::SpecialDots);
        } else {
            SetSquareValue(old_pos, collected_[old_pos.row][old_pos.col] ? SquareType::Empty : SquareType::Dots);
        }
        
        // Move enemy to new position
        SetSquareValue(pos, SquareType::Enemies);
        p->SetPosition(pos);
        
        // If we moved onto a special dot, don't mark it as collected
        if (!was_special_dot) {
            collected_[pos.row][pos.col] = true;
        }
        
        return true;
    }
    
    return false;
}

/**
 * Overloaded output operator for Board class.
 * Prints the current state of the board using emoji representations.
 * 
 * @param os Output stream
 * @param b Board to print
 * @return Modified output stream
 */
std::ostream& operator<<(std::ostream& os, const Board &b) {
    for (int i = 0; i < b.rows_; i++) {
        for (int j = 0; j < b.cols_; j++) {
            os << SquareTypeStringify(b.arr_[i][j]) << " ";
        }
        os << "\n";
    }
    return os;
}

/**
 * Game constructor. Initializes a new game with default values.
 */
Game::Game() : board_(new Board()), turn_count_(0), human_player_(nullptr) {
}

/**
 * Starts a new game with specified players and number of enemies.
 * Initializes the board and places all entities in their starting positions.
 * 
 * @param human Pointer to the human player
 * @param enemylist Vector of enemy pointers
 * @param enemies Number of enemies to create
 */
void Game::NewGame(Player *human, std::vector<Player*>& enemylist, const int enemies) {
    // Delete old board if it exists and create new one
    if (board_) {
        delete board_;
    }
    board_ = new Board(enemies);
    
    SetHumanPlayer(human);
    
    // Clear existing enemies
    for (Player* enemy : enemylist) {
        delete enemy;
    }
    enemylist.clear();
    
    // Set player starting position
    Position pac_pos = {5,5};
    human->SetPosition(pac_pos);
    
    // Create new enemies at their positions
    for (int i = 0; i < board_->get_rows(); i++) {
        for (int j = 0; j < board_->get_cols(); j++) {
            if (board_->get_square_value(Position{i,j}) == SquareType::Enemies) {
                Player* enemy = new Player("Enemy", false, Position{i,j});
                enemylist.push_back(enemy);
            }
        }
    }
}

/**
 * Takes a turn for a player, handling their movement and interactions.
 * 
 * @param p Pointer to the player taking the turn
 * @param enemylist Vector of enemy pointers
 */
void Game::TakeTurn(Player *p, std::vector<Player*>& enemylist) {
    if (p->isDead()) {
        return;  // Don't allow moves if player is dead
    }

    std::cout << "Possible moves: " << std::endl;
    std::vector<Position> valid_moves = board_->GetMoves(p);
    for (const Position& move : valid_moves) {
        std::cout << p->ToRelativePosition(move) << std::endl;
    }
    
    bool valid_move = false;
    while (!valid_move) {
        char move_input;
        std::cin >> move_input;
        switch (move_input) {
            case 'w':
                valid_move = board_->MovePlayer(p, Position{p->get_position().row-1, p->get_position().col}, enemylist);
                if (valid_move) turn_count_++;
                break;
            case 's':
                valid_move = board_->MovePlayer(p, Position{p->get_position().row+1, p->get_position().col}, enemylist);
                if (valid_move) turn_count_++;
                break;
            case 'a':
                valid_move = board_->MovePlayer(p, Position{p->get_position().row, p->get_position().col-1}, enemylist);
                if (valid_move) turn_count_++;
                break;
            case 'd':
                valid_move = board_->MovePlayer(p, Position{p->get_position().row, p->get_position().col+1}, enemylist);
                if (valid_move) turn_count_++;
                break;
            default:
                std::cout << "Invalid move. Please try again: " << std::endl;
                break;
        }
        if (!valid_move && (move_input == 'w' || move_input == 'a' || move_input == 's' || move_input == 'd')) {
            std::cout << "Cannot move there. Please try again: " << std::endl;
        }
        if (p->isDead()) {
            break;  // Exit move loop if player dies
        }
    }
}

void Game::TakeTurnEnemy(Player *p) {
    std::vector<Position> valid_moves = board_->GetMoves(p);
    if (valid_moves.empty()) return;

    // Find Pacman's position
    Position pacman_pos;
    bool found_pacman = false;
    for (int i = 0; i < board_->get_rows(); i++) {
        for (int j = 0; j < board_->get_cols(); j++) {
            SquareType square = board_->get_square_value(Position{i,j});
            if (square == SquareType::Pacman || square == SquareType::PowerfulPacman) {
                pacman_pos = Position{i,j};
                found_pacman = true;
                break;
            }
        }
        if (found_pacman) break;
    }

    // If Pacman not found or no valid moves, return
    if (!found_pacman || valid_moves.empty()) return;

    // Check if enemy is adjacent to player
    Position enemy_pos = p->get_position();
    if (abs(enemy_pos.row - pacman_pos.row) + abs(enemy_pos.col - pacman_pos.col) == 1) {
        // If adjacent to regular Pacman, kill them
        if (board_->get_square_value(pacman_pos) == SquareType::Pacman) {
            // Mark the player's position as empty when they die
            board_->SetSquareValue(pacman_pos, SquareType::Empty);
            board_->SetSquareValue(enemy_pos, SquareType::Dots);
            p->SetPosition(pacman_pos);
            board_->SetSquareValue(pacman_pos, SquareType::Enemies);
            // Mark the human player as dead
            GetHumanPlayer()->setIsDead(true);
            return;
        }
    }

    // Find the move that gets closest to Pacman
    Position best_move = valid_moves[0];
    int min_distance = abs(best_move.row - pacman_pos.row) + abs(best_move.col - pacman_pos.col);

    for (const Position& move : valid_moves) {
        int distance = abs(move.row - pacman_pos.row) + abs(move.col - pacman_pos.col);
        if (distance < min_distance) {
            min_distance = distance;
            best_move = move;
        }
    }

    // Try to move to best position
    if (!board_->MoveEnemy(p, best_move, this)) {
        // If best move fails, try a random valid move
        int random_index = rand() % valid_moves.size();
        board_->MoveEnemy(p, valid_moves[random_index], this);
    }
}

bool Game::IsGameOver(Player *p) {
    return (p->isDead() && p->getLives() <= 0) || CheckifdotsOver();  // Game is over if player is out of lives or all dots are collected
}

bool Game::CheckifdotsOver() {
    // Scan entire board for any remaining dots or special dots
    for (int i = 0; i < board_->get_rows(); i++) {
        for (int j = 0; j < board_->get_cols(); j++) {
            SquareType square = board_->get_square_value(Position{i,j});
            if (square == SquareType::Dots || square == SquareType::SpecialDots) {
                return false;  // Found at least one dot, game continues
            }
        }
    }
    return true;  // No dots found, game is over
}

std::string Game::GenerateReport(Player *p) {
    // TODO: Implement game report generation
    return "";
}

std::ostream& operator<<(std::ostream& os, const Game &m) {
    // TODO: Implement game state display
    return os;
}

void Game::SetHumanPlayer(Player* human) {
    human_player_ = human;
}

Player* Game::GetHumanPlayer() const {
    return human_player_;
}

/**
 * Respawns a player at the default spawn position.
 * Handles enemy displacement if necessary.
 * 
 * @param player Pointer to the player to respawn
 * @return true if respawn was successful, false otherwise
 */
bool Game::RespawnPlayer(Player* player) {
    Position spawn_pos{5, 5};
    
    // Clear the player's old position if they're still on the board
    for (int i = 0; i < board_->get_rows(); i++) {
        for (int j = 0; j < board_->get_cols(); j++) {
            if (board_->get_square_value(Position{i,j}) == SquareType::Pacman ||
                board_->get_square_value(Position{i,j}) == SquareType::PowerfulPacman) {
                board_->SetSquareValue(Position{i,j}, SquareType::Empty);
            }
        }
    }
    
    // Check if there's an enemy at the spawn position
    SquareType spawn_square = board_->get_square_value(spawn_pos);
    if (spawn_square == SquareType::Enemies) {
        // Try to move enemy to an open diagonal
        Position diagonals[] = {
            Position{4, 4}, Position{4, 6},
            Position{6, 4}, Position{6, 6}
        };
        
        // Find first open diagonal
        for (const Position& pos : diagonals) {
            SquareType square = board_->get_square_value(pos);
            if (square != SquareType::Wall && square != SquareType::Enemies) {
                // Move enemy to this position
                for (Player* enemy : players_) {
                    if (!enemy->is_human() && enemy->get_position() == spawn_pos) {
                        board_->SetSquareValue(enemy->get_position(), SquareType::Dots);
                        enemy->SetPosition(pos);
                        board_->SetSquareValue(pos, SquareType::Enemies);
                        break;
                    }
                }
                break;
            }
        }
    }
    
    // Spawn the player and update the board
    player->SetPosition(spawn_pos);
    board_->SetSquareValue(spawn_pos, SquareType::Pacman);
    std::cout << *board_ << std::endl;
    return true;
}
