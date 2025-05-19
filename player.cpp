#include "player.h"
#include "level.h"
#include "enemy.h"

extern Level currentLevel;

// начальные значения игрока
Player::Player() {
    resetStats();
}

// cбрасывает статистику (жизни и очки)
void Player::resetStats() {
    lives = MAX_PLAYER_LIVES;
    for (int i = 0; i < LEVEL_COUNT; i++) {
        levelScores[i] = 0;
    }
}

// увеличивает счёт игрока и проигрывает звук монеты
void Player::incrementScore() {
    PlaySound(coin_sound);
    levelScores[currentLevel.getCurrentLevelIndex()]++;
}

// подсчитывает общий счёт игрока по всем уровням
int Player::getTotalScore() const {
    int sum = 0;
    for (int i = 0; i < LEVEL_COUNT; i++) {
        sum += levelScores[i];
    }
    return sum;
}

// размещает игрока на уровне, ищет символ игрока в данных уровня
void Player::spawn(const Level& level) {
    yVelocity = 0;

    for (size_t row = 0; row < level.getRows(); ++row) {
        for (size_t column = 0; column < level.getColumns(); ++column) {
            char cell = level.getLevelCell(row, column);

            if (cell == PLAYER) {
                position.x = column;
                position.y = row;
                const_cast<Level&>(level).setLevelCell(row, column, AIR);
                return;
            }
        }
    }
}

// обрабатывает смерть игрока, проигрывает звук, уменьшает жизни и обнуляет счёт уровня
void Player::kill() {
    PlaySound(player_death_sound);
    game_state = DEATH_STATE;
    lives--;
    levelScores[currentLevel.getCurrentLevelIndex()] = 0;
}

// горизонтальное движение игрока с проверкой столкновений
void Player::moveHorizontally(float delta) {
    float next_x = position.x + delta;

    if (!currentLevel.isColliding({next_x, position.y}, WALL)) {
        position.x = next_x;
    } else {
        position.x = roundf(position.x);
        return;
    }

    lookingForward = delta > 0;
    moving = delta != 0;
}

// прыжок игрока, если он находится на земле
void Player::jump() {
    if (onGround) {
        yVelocity = -JUMP_STRENGTH;
        onGround = false;
    }
}

// обновляет гравитацию и проверяет столкновения с землёй и потолком
void Player::updateGravity(const Level& level) {
    if (level.isColliding({position.x, position.y - 0.1f}, WALL) && yVelocity < 0) {
        yVelocity = CEILING_BOUNCE_OFF;
    }
    
    position.y += yVelocity;
    yVelocity += GRAVITY_FORCE;
    
    onGround = level.isColliding({position.x, position.y + 0.1f}, WALL);
    
    if (onGround) {
        yVelocity = 0;
        position.y = roundf(position.y);
    }
}

// сбор монет, выход с уровня, столкновения с шипами и врагами
void Player::update(const Level& level) {
    updateGravity(level);
    
    // сбор монет
    if (level.isColliding(position, COIN)) {
        const_cast<Level&>(level).getCollider(position, COIN) = AIR;
        incrementScore();
    }
    
    // выхода с уровня
    if (level.isColliding(position, EXIT)) {
        if (timer > 0) {
            timer -= 25;
            time_to_coin_counter += 5;

            if (time_to_coin_counter / 60 > 1) {
                incrementScore();
                time_to_coin_counter = 0;
            }
        } else {
            const_cast<Level&>(level).loadLevel(1);
            PlaySound(exit_sound);
        }
    } else {
        if (timer >= 0) timer--;
    }
    
    // проверка столкновения с шипами или падения
    if (level.isColliding(position, SPIKE) || position.y > level.getRows()) {
        kill();
    }
    
    // проверка столкновения с врагами
    if (Enemy::isCollidingWithEnemies(position)) {
        if (yVelocity > 0) {
            // игрок падает на врага - убиваем врага
            Enemy::removeCollidingEnemy(position);
            PlaySound(kill_enemy_sound);

            incrementScore();
            yVelocity = -BOUNCE_OFF_ENEMY;
        } else {
            // игрок столкнулся с врагом сбоку - игрок умирает
            kill();
        }
    }
}