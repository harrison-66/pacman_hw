#include "Player.h"
#include <string>
#include <iostream>
Player::Player(const std::string name, const bool is_human, Position pos) 
    : name_(name), 
        points_(0), 
        is_human_(is_human), 
        has_Treasure_(false),
        has_Enemy_Treasure_(false),
        isDead_(false),
        lives_(3) {  // Initialize with 3 lives
        pos_ = pos;
}

void Player::ChangePoints(const int x) {
    points_ += x;
}

void Player::SetPosition(Position pos) {
    pos_ = pos;
}


void Player::setIsDead(bool isdead) {
    isDead_ = isdead;
    if (isdead && is_human_) {
        decreaseLives();
    }
}

void Player::respawn() {
    if (lives_ > 0) {
        isDead_ = false;
        has_Treasure_ = false;
        pos_ = Position{5, 5};  // Respawn at (5,5)
        std::cout << "You Died!\tRespawning at position (5,5)..." << std::endl;
    }
}

std::string Player::ToRelativePosition(Position other) {
    // Calculate relative position
    if (other.row < pos_.row) return "up (w)";
    if (other.row > pos_.row) return "down (s)";
    if (other.col < pos_.col) return "left (a)";
    if (other.col > pos_.col) return "right (d)";
    return "same";
}

std::string Player::Stringify() {
    return name_ + " (Points: " + std::to_string(points_) + ")";
}
