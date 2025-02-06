#include "Game.h"
#include <iostream>
#include <cstdlib>


// Convert SquareType to string representation
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
        case SquareType::EnemySpecialTreasure: return "💎"; // special treasure
        case SquareType::Trap: return " ";      // 
        default: return "❓";                     // unknown
    }
}

// Board initialization: sets up the board with pellets, walls, and enemies as per specifications
Board::Board() {
    rows_ = get_rows();
    cols_ = get_cols();
    // Initialize board with pellets squares
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            arr_[i][j] = SquareType::Dots;
        }
    }
    //set pacman to middle, and walls around the edges
    for (int i = 0; i < rows_; i++) {
        arr_[i][0] = SquareType::Wall;
        arr_[i][cols_-1] = SquareType::Wall;
    }
    for (int j = 0; j < cols_; j++) {
        arr_[0][j] = SquareType::Wall;
        arr_[rows_-1][j] = SquareType::Wall;
    }
    arr_[5][5] = SquareType::Pacman;

    //add additional walls
    arr_[2][2] = SquareType::Wall;
    arr_[2][3] = SquareType::Wall;
    arr_[2][4] = SquareType::Wall;
    arr_[3][2] = SquareType::Wall;

    arr_[6][2] = SquareType::Wall;
    arr_[7][3] = SquareType::Wall;
    arr_[3][6] = SquareType::Wall;
    arr_[3][7] = SquareType::Wall;
    arr_[4][7] = SquareType::Wall;
    arr_[7][7] = SquareType::Wall;
    arr_[4][4] = SquareType::Wall;
    arr_[5][4] = SquareType::Wall;
    
    // for all spaces not walls or middle space, 10% chance of a treasure; then 10% chance pellet is special
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            if (arr_[i][j] != SquareType::Wall && arr_[i][j] != SquareType::Pacman) {
                if (rand() % 10 == 0) {
                    arr_[i][j] = SquareType::Treasure;
                }
            }
            if(arr_[i][j] == SquareType::Dots) {
                if (rand() % 10 == 0) {
                    arr_[i][j] = SquareType::SpecialDots;
                }
            }
        }
    }
    //finally, add 2 enemies randomly onto dot spaces
    for (int i = 0; i < 2; i++) {
        int rand_row = rand() % rows_;
        int rand_col = rand() % cols_;
        while (arr_[rand_row][rand_col] != SquareType::Dots) {
            rand_row = rand() % rows_;
            rand_col = rand() % cols_;
        }
        arr_[rand_row][rand_col] = SquareType::Enemies;
    }



}

SquareType Board::get_square_value(Position pos) const {
    return arr_[pos.row][pos.col];
}

void Board::SetSquareValue(Position pos, SquareType value) {
    arr_[pos.row][pos.col] = value;
}

// Get all possible moves for a player
std::vector<Position> Board::GetMoves(Player *p) {
    std::vector<Position> moves;
    //check all 4 directions
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

    // Handle collisions based on treasure status
    if (target == SquareType::Enemies) {
        if (p->hasTreasure()) {
            // Player has treasure - destroy enemy and lose power-up
            p->setHasTreasure(false);  // Explicitly set to false
            
            // Find and remove the enemy at this position
            for (auto it = enemylist.begin(); it != enemylist.end(); ++it) {
                if ((*it)->get_position() == pos) {
                    delete *it;
                    enemylist.erase(it);
                    break;
                }
            }
            
            // Move player to the position
            SetSquareValue(p->get_position(), SquareType::Empty);
            SetSquareValue(pos, SquareType::Pacman);  // Regular Pacman after losing power-up
            p->SetPosition(pos);
            
            // Spawn new enemy at random location
            SpawnNewEnemy(enemylist);
            return true;
        } else {
            // Player dies - set state but don't move player
            p->setIsDead(true);
            // Leave player's current position as is, don't move to enemy position
            return false;
        }
    }

    // Handle treasure pickup
    if (target == SquareType::Treasure) {
        p->ChangePoints(100);
        p->setHasTreasure(true);  // Explicitly set to true
        SetSquareValue(p->get_position(), SquareType::Empty);
        SetSquareValue(pos, SquareType::PowerfulPacman);
        p->SetPosition(pos);
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

// Add this helper function to Board class
void Board::SpawnNewEnemy(std::vector<Player*>& enemylist) {
    int attempts = 0;
    int rand_row, rand_col;
    bool found_position = false;
    
    while (!found_position && attempts < 100) {  // Add maximum attempts to prevent infinite loop
        rand_row = rand() % rows_;
        rand_col = rand() % cols_;
        
        if (arr_[rand_row][rand_col] == SquareType::Empty || 
            arr_[rand_row][rand_col] == SquareType::Dots) {
            found_position = true;
        }
        attempts++;
    }
    
    if (!found_position) {
        // If no position found after max attempts, scan board systematically
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

bool Board::MoveEnemy(Player *p, Position pos, Game* game) {
    //if enemy is on player's square, kill player
    if (get_square_value(pos) == SquareType::Pacman) {
        game->GetHumanPlayer()->setIsDead(true);
        return true;
    }
    SquareType target = get_square_value(pos);
    
    // Handle collision with player
    if (target == SquareType::Pacman || target == SquareType::PowerfulPacman) {
        if (target == SquareType::PowerfulPacman) {
            // Enemy dies when colliding with powered-up player
            return false;
        }
        // Enemy collides with regular player
        SetSquareValue(p->get_position(), SquareType::Dots);  // clear enemy's old square
        SetSquareValue(pos, SquareType::Enemies);             // enemy moves onto player's square
        p->SetPosition(pos);
        
        // Mark the human player as dead
        if (game && game->GetHumanPlayer()) {
            game->GetHumanPlayer()->setIsDead(true);
        }
        if (get_square_value(pos) == SquareType::Pacman) {
            game->GetHumanPlayer()->setIsDead(true);
            return true;
        }
        return true;
    }
    
    // Check adjacent squares for regular player (not powered up)
    std::vector<Position> adjacent = {
        Position{pos.row-1, pos.col},
        Position{pos.row+1, pos.col},
        Position{pos.row, pos.col-1},
        Position{pos.row, pos.col+1}
    };
    
    // First check if we can move onto player
    for (const Position& adj : adjacent) {
        if (adj.row >= 0 && adj.row < get_rows() && adj.col >= 0 && adj.col < get_cols()) {
            if (get_square_value(adj) == SquareType::Pacman) {
                // Move onto player if possible
                if (target == SquareType::Empty || target == SquareType::Dots) {
                    SetSquareValue(p->get_position(), SquareType::Dots);  // Leave dots behind
                    SetSquareValue(pos, SquareType::Enemies);  // Move to position next to player
                    p->SetPosition(pos);
                    return true;  // Player will be killed in Game::TakeTurnEnemy
                }
            }
        }
    }
    
    // Only move if target square is empty or has dots
    if (target != SquareType::Empty && target != SquareType::Dots) {
        return false;
    }
    
    // Move enemy
    SetSquareValue(p->get_position(), SquareType::Dots);  // Leave dots behind
    SetSquareValue(pos, SquareType::Enemies);
    p->SetPosition(pos);
    
    return true;
}

std::ostream& operator<<(std::ostream& os, const Board &b) {
    for (int i = 0; i < b.rows_; i++) {
        for (int j = 0; j < b.cols_; j++) {
            os << SquareTypeStringify(b.arr_[i][j]) << " ";
        }
        os << "\n";
    }
    return os;
}

// Game implementation
Game::Game() : board_(new Board()), turn_count_(0), human_player_(nullptr) {
}

void Game::NewGame(Player *human, std::vector<Player*>& enemylist, const int enemies) {
    SetHumanPlayer(human);  // Store the human player pointer
    // Clear any existing enemies first
    for (Player* enemy : enemylist) {
        delete enemy;
    }
    enemylist.clear();
    
    Position pac_pos = {5,5};
    human->SetPosition(pac_pos);
    
    // Find and create enemies
    for (int i = 0; i < board_->get_rows(); i++) {
        for (int j = 0; j < board_->get_cols(); j++) {
            if (board_->get_square_value(Position{i,j}) == SquareType::Enemies) {
                Player* enemy = new Player("Enemy", false, Position{i,j});
                enemylist.push_back(enemy);
            }
        }
    }
}

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
            board_->SetSquareValue(pacman_pos, SquareType::Enemies);
            board_->SetSquareValue(enemy_pos, SquareType::Dots);
            p->SetPosition(pacman_pos);
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
    return p->isDead();  // Game is over if player is dead
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
