#ifndef PLAYER_H
#define PLAYER_H

#include "globals.h"
#include "level.h"

class Player {
private:
    Vector2 position;
    float yVelocity;
    int lives;
    int levelScores[LEVEL_COUNT];
    bool lookingForward;
    bool moving;
    bool onGround;

public:
    Player();
    
    // Core functionality
    void resetStats();
    void incrementScore();
    int getTotalScore() const;
    void spawn(const Level& level);
    void kill();
    
    // Movement and physics
    void moveHorizontally(float delta);
    void jump();
    void updateGravity(const Level& level);
    void update(const Level& level);
    
    // Getters
    Vector2 getPosition() const { return position; }
    int getLives() const { return lives; }
    bool isLookingRight() const { return lookingForward; }
    bool isMoving() const { return moving; }
    bool isOnGround() const { return onGround; }
    
    // Setters
    void setPosition(const Vector2& pos) { position = pos; }
};

#endif // PLAYER_H
