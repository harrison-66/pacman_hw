#include <iostream>
#include <ctime>    // for time()
#include <cstdlib>  // for srand() and rand()
#include "Game.h"
#include "Player.h"

/*
Harrison Getches | Intensive Programming Workshop | 02/06/2025
Additional Features added:
- Traps: Visible and Hidden traps that kill the player if touched
- Enemy Routing: Enemies move towards player if possible
- Custom Enemy Count: User can input the number of enemies in terminal or CL Argument
- Enemy Treasure: Enemies can pick up special treasure that gives them a power boost (overrules 20% of normal treasure spawn)
*/


int main(int argc, char* argv[]) {
    // Seed random number generator
    srand(time(nullptr));
    
    // Create game
    Game game;

    // Create human player at position (5,5)
    Player* human = new Player("Player1", true, Position{5,5});
    
    // Create vector for enemies
    std::vector<Player*> enemies;
    
    // Determine number of enemies (from command line or user input)
    int num_enemies = 2;  // default value
    
    if (argc > 1) {
        // Try to get number from command line
        num_enemies = std::atoi(argv[1]);
        if (num_enemies < 1 || num_enemies > 8) {
            std::cout << "Invalid number of enemies specified. Must be between 1 and 8.\n";
            std::cout << "Using default value of 2 enemies.\n";
            num_enemies = 2;
        }
    } else {
        // Ask user for number of enemies
        std::cout << "Enter number of enemies (1-8, default is 2): ";
        std::string input;
        std::getline(std::cin, input);
        
        if (!input.empty()) {
            try {
                num_enemies = std::stoi(input);
                if (num_enemies < 1 || num_enemies > 8) {
                    std::cout << "Invalid number. Using default value of 2 enemies.\n";
                    num_enemies = 2;
                }
            } catch (...) {
                std::cout << "Invalid input. Using default value of 2 enemies.\n";
                num_enemies = 2;
            }
        }
    }
    
    // Initialize new game with specified number of enemies
    game.NewGame(human, enemies, num_enemies);

    // Print the initial board state
    std::cout << "Initial Board State:\n";
    std::cout << game.GetBoard() << std::endl;

    // Game loop
    while (!game.IsGameOver(human)) {
        // Display current game state
        std::cout << "\nLives Remaining: " << human->getLives() << std::endl;
        std::cout << "Current Score: " << human->get_points() << std::endl;
        
        // Player's turn
        std::cout << "\nYour turn! Enter move (w/a/s/d): " << std::endl;
        game.TakeTurn(human, enemies);
        
        // Check if player died during their turn
        if (human->isDead()) {
            if (human->getLives() > 0) {
                std::cout << "\nYou died! Lives remaining: " << human->getLives() << std::endl;
                human->respawn();
                game.RespawnPlayer(human);
                std::cout << "Respawning at position (5,5)..." << std::endl;
                continue;
            } else {
                break;
            }
        }
        
        // Enemy turns
        for (Player* enemy : enemies) {
            game.TakeTurnEnemy(enemy);
            // Check if enemy killed player
            if (human->isDead()) {
                if (human->getLives() > 0) {
                    std::cout << "\nYou died! Lives remaining: " << human->getLives() << std::endl;
                    human->respawn();
                    game.RespawnPlayer(human);
                    std::cout << "Respawning at position (5,5)..." << std::endl;
                    break;
                } else {
                    break;
                }
            }
        }
        
        // If player is out of lives, end game
        if (human->getLives() <= 0) {
            break;
        }
        
        // Display board after enemies move
        std::cout << "\nAfter enemy moves:" << std::endl;
        std::cout << game.GetBoard() << std::endl;
    }

    // Game over
    std::cout << "\nGame Over!\n";
    std::cout << "Final Score: " << human->get_points() << std::endl;
    if (human->getLives() <= 0) {
        std::cout << "You ran out of lives!" << std::endl;
    } else if (game.CheckifdotsOver()) {
        std::cout << "Congratulations! You collected all the dots!" << std::endl;
    }

    // Clean up
    delete human;
    for (Player* enemy : enemies) {
        delete enemy;
    }

    return 0;
}