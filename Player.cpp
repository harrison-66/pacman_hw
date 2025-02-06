#include "Player.h"
#include <string>

Player::Player(const std::string name, const bool is_human, Position pos) 
    : name_(name), 
        
        points_(0), 
        is_human_(is_human), 
        has_Treasure_(false),
        isDead_(false) {
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
