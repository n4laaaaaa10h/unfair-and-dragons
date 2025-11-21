//lets just hope gua ngoding ga kayak jason "pirate software" thorhall atau alex "yanderedev" mahan

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME 32
#define MAX_INV 20
#define SAVEFILE "savegame.dat"

typedef struct {
    char name[MAX_NAME];
    int price;
    int healAmount; // potion healing value; 0 alabipa bukan potion
    int attackBonus; // opsional. like, literally.
} Item;

typedef struct {
    char name[MAX_NAME];
    int hp, maxHp;
    int attack;
    int defense;
    int level;
    int exp;
    int gold;
    Item inventory[MAX_INV];
    int invCount;
} Player;

typedef struct {
    char name[MAX_NAME];
    int hp;
    int attack;
    int defense;
    int expReward;
    int goldReward;
} Enemy;

/* ----------the many voids---------- */
void initPlayer(Player *p);
void mainMenu(Player *p);
void startNewGame(Player *p);
void explore(Player *p);
void battle(Player *p, Enemy en);
void showStats(const Player *p);
void showInventory(const Player *p);
void addItem(Player *p, Item it);
void useItem(Player *p);
void shop(Player *p);
void saveGame(const Player *p);
int loadGame(Player *p);
int riggedD20();
void sortInventoryByName(Player *p);
void sortInventoryByPrice(Player *p);
int searchItemByName(const Player *p, const char *name); // return index atau niga 1
int getInt();
void pressEnterToContinue();

//dont know what else should i add. jujur. skip ke shopitems buat sample data aja dulu. berharap ke tuhan segini cukup.

Item shopItems[] = {
    {"Potion Butut", 10, 20, 0},
    {"Potion Yang Begitu Deh", 30, 50, 0},
    {"Potion Maha Obelix", 80, 120, 0},
    {"Pedang Tetanus", 50, 0, 3},
    {"Pedang Gitu Gitu Aja", 150, 0, 7},
    {"Tameng Ga Jelas", 120, 0, 0}
};
int shopItemsCount = sizeof(shopItems)/sizeof(shopItems[0]);

//screw spotify and their stupid changes to their premium plans btw.

Enemy enemyPool[] = {
    {"Slime", 20, 4, 1, 10, 5},
    {"Goblin", 35, 6, 2, 20, 10},
    {"Wolf", 45, 8, 3, 30, 15},
    {"Bandit", 60, 10, 4, 50, 25},
    {"Orc", 90, 14, 6, 80, 50}
};
int enemyPoolCount = sizeof(enemyPool)/sizeof(enemyPool[0]);
//basic emang maklum lah ya. namanya juga baru belajar.

//uitel utility
int getInt() {
    int x;
    while (scanf("%d", &x) != 1) {
        while (getchar() != '\n');
        printf("Input invalid. Try again: ");
    }
    while (getchar() != '\n'); // clear rest
    return x;
}

void pressEnterToContinue() {
    printf("\n[Press Enter to continue]");
    getchar();
}
/* ----------riggedD20---------- */
/* Behavior:
   - static counter totalRolls accumulates across calls
   - before threshold (80) mostly return 1, with tiny noise
   - after threshold: 1% nat20, 99% nat1 (with tiny noise biar ga anomali)
*/
//if i fucked up here, therefore im screwed.
int riggedD20() {
    static int totalRolls = 0;
    totalRolls++;
    // Disguise RNG: produce occasional 2 or 3 early biar keliatan random
    if (totalRolls < 80) {
        int noise = rand() % 100;
        if (noise < 94) return 1;    // 94% -> nat1
        else if (noise < 98) return 2; // 4% -> low roll
        else return 3;                // 2% -> naik dikit lah.
    } else {
        int r = rand() % 100; // 0..99
        if (r == 0) return 20; // 1% -> nat20
        // small chance of 2..5 biar ada variasi
        if (r < 6) return 2 + (r % 4); // 5% small low rolls
        return 1;
    }
}
//that wasn't so bad....i guess lebih mending ini daripada uts bastat tapi kalkulator abis baterai. :crying_emoji:

/* ----------gameFunctions---------- */