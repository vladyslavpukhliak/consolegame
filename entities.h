#pragma once  // Додаємо це, щоб запобігти повторному включенню файлу

// Визначення констант (якщо вони ще не визначені)
#ifndef CANNON_CONSTANTS_DEFINED
#define CANNON_CONSTANTS_DEFINED
const int CANNON_TILE_COUNT = 2;
const int CANNON_PROJECTILE_COUNT = 4;
#endif

// Визначення структур
#ifndef ENEMY_INFO_DEFINED
#define ENEMY_INFO_DEFINED
struct EnemyInfo {
    char tile;
    std::string name;
    std::string color;
    std::string description;
    int level;
    int attack;
    int health;
    int experience;
};
#endif

#ifndef CANNON_DEFINED
#define CANNON_DEFINED
struct Cannon {
    char tile[CANNON_TILE_COUNT];
    char projectile[CANNON_PROJECTILE_COUNT];
    std::string name;
    std::string color;
    std::string description;
    std::string playerDeathLine;
};
#endif