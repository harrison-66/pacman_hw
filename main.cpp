#include <iostream>
#include <ctime>    // for time()
#include <cstdlib>  // for srand() and rand()
#include "Game.h"
#include "Player.h"

int main() {
    // Seed random number generator
    srand(time(nullptr));
    
    // Create game
    Game game;

    // Create human player at position (5,5)
    Player* human = new Player("Player1", true, Position{5,5});
    
    // Create vector for enemies
    std::vector<Player*> enemies;
    
    // Initialize new game with 2 enemies
    game.NewGame(human, enemies, 2);

    // Print the initial board state
    std::cout << "Initial Board State:\n";
    std::cout << game.GetBoard() << std::endl;

    // Game loop
    while (!game.IsGameOver(human)) {
        // Display current game state
        std::cout << "\nCurrent Score: " << human->get_points() << std::endl;
        
        // Player's turn
        std::cout << "\nYour turn! Enter move (w/a/s/d): " << std::endl;
        game.TakeTurn(human, enemies);
        
        // Check if player died during their turn
        if (game.IsGameOver(human)) {
            break;
        }
        
        // Display board after player's move
        std::cout << game.GetBoard() << std::endl;
        
        // Enemy turns
        for (Player* enemy : enemies) {
            game.TakeTurnEnemy(enemy);
            // Check if enemy killed player
            if (game.IsGameOver(human)) {
                break;
            }
        }
        
        // If player died during enemy turns, break
        if (game.IsGameOver(human)) {
            break;
        }
        
        // Display board after enemies move
        std::cout << "\nAfter enemy moves:" << std::endl;
        std::cout << game.GetBoard() << std::endl;
    }

    // Game over
    std::cout << "\nGame Over!\n";
    std::cout << "Final Score: " << human->get_points() << std::endl;
    std::cout << "You were caught by a ghost!" << std::endl;

    // Clean up
    delete human;
    for (Player* enemy : enemies) {
        delete enemy;
    }

    return 0;
}