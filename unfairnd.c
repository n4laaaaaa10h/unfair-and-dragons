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
    {"Lite Potion", 10, 20, 0},
    {"Standard Potion", 30, 50, 0},
    {"Premium Potion", 80, 120, 0},
    {"Mid Sword", 50, 0, 3},
    {"Normal Ahh Sword", 150, 0, 7},
    {"Normal Ahh Shield", 120, 0, 0}
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
void initPlayer(Player *p) {
    strcpy(p->name, "Hero");
    p->hp = p->maxHp = 100;
    p->attack = 8;
    p->defense = 2;
    p->level = 1;
    p->exp = 0;
    p->gold = 50;
    p->invCount = 0;

    addItem(p, (Item){"Potion Butut", 10, 20, 0});
}

void startNewGame(Player *p) {
    char tmp[MAX_NAME];
    printf("Enter your Character's name: ");
    if (fgets(tmp, sizeof(tmp), stdin)) {
        tmp[strcspn(tmp, "\n")] = 0;
        if (strlen(tmp) > 0) strncpy(p->name, tmp, MAX_NAME-1);
    }
    initPlayer(p);
    printf("Good Morning, %s! Let the suffering begin...\n", p->name);
    pressEnterToContinue();
}

void mainMenu(Player *p) {
    int choice;
    while (1) {
        system("cls"); //set jadi "clear" kalo misalnya pake linux. or imac. idgaf.
        printf("==== Unfair & Dragons (Text & C Edition) ====\n");
        printf("1. New Game\n");
        printf("2. Load Game\n");
        printf("3. Exit\n");
        printf("Choose: ");
        choice = getInt();
        if (choice == 1) {
            startNewGame(p);
            break;
        } else if (choice == 2) {
            if (loadGame(p)) {
                printf("Save file found! Loading.......\n");
                pressEnterToContinue();
                break;
            } else {
                printf("There's no save. Perhaps it was deleted? or you haven't start your journey?\n");
                pressEnterToContinue();
            }
        } else if (choice == 3) {
            printf("Good Night.\n");
            exit(0);
        } else {
            printf("Please Choose between 1 and 3.\n");
            pressEnterToContinue();
        }
    }

//game loop
    int running = 1;
    while (running) {
        system("cls");
        printf("=== Main Menu ===\n");
        printf("Player: %s  Lv:%d  HP:%d/%d  Gold:%d  EXP:%d\n",
               p->name, p->level, p->hp, p->maxHp, p->gold, p->exp);
        printf("1. Explore\n");
        printf("2. Shop\n");
        printf("3. Inventory\n");
        printf("4. Stats\n");
        printf("5. Save Game\n");
        printf("6. Exit to Desktop\n");
        printf("Choose: ");
        choice = getInt();
        switch (choice) {
            case 1: explore(p); break;
            case 2: shop(p); break;
            case 3: showInventory(p); break;
            case 4: showStats(p); break;
            case 5: saveGame(p); break;
            case 6: running = 0; break;
            default: printf("Invalid.\n"); pressEnterToContinue();
        }
        if (p->hp <= 0) {
            printf("Death is something unavoidable, afterall... Game Over.\n");
            running = 0;
        }
    }
}

void explore(Player *p) {
    printf("You wander through a dark area...\n");
    int encounter = rand() % 100;
    if (encounter < 65) { // pick a random enemy based on level
        int idx = rand() % enemyPoolCount;
        Enemy en = enemyPool[idx]; // now we shall scale it a bit with player level. muehehehe.
        en.hp += p->level * 5;
        en.attack += p->level * 1;
        en.defense += (p->level / 2);
        printf("A wild %s appears!\n", en.name);
        battle(p, en);
    } else { // find gold or a random item. idk.
        int find = rand() % 100;
        if (find < 60) {
            int g = 5 + rand() % 31;
            p->gold += g;
            printf("You found %d gold!\n", g);
        } else { // find magi- i mean potion, potion, yeah.
            Item it = shopItems[rand() % shopItemsCount];
            printf("You found a %s!\n", it.name);
            addItem(p, it);
        }
        pressEnterToContinue();
    }
}

void battle(Player *p, Enemy en) {
    printf("Battle start: %s (HP:%d) vs %s (HP:%d)\n", p->name, p->hp, en.name, en.hp);
    while (p->hp > 0 && en.hp > 0) {
        printf("\nYour HP: %d/%d | Enemy HP: %d\n", p->hp, p->maxHp, en.hp);
        printf("1. Attack  2. Use Item  3. Run\nChoose: ");
        int c = getInt();
        if (c == 1) {
            int roll = riggedD20();
            printf("You rolled the D20... => %d\n", roll);
            int base = p->attack;
            int dmg;
            if (roll == 20) {
                printf("CRITICAL HIT!\n");
                dmg = (base * 2) - en.defense;
            } else if (roll == 1) {
                printf("Oh no... NAT 1! Your attack sucks lmao.\n");
                dmg = (base / 5) - en.defense; // tiny damage or negative
            } else {
                dmg = base - en.defense;
            }
            if (dmg < 0) dmg = 0;
            en.hp -= dmg;
            printf("You deal %d damage.\n", dmg);
        } else if (c == 2) {
            useItem(p);
        } else if (c == 3) {
            int chance = riggedD20();
            printf("You attempt to run... roll %d\n", chance);
            if (chance >= 15) { printf("You escaped!\n"); pressEnterToContinue(); return; }
            else { printf("Failed to escape.\n"); }
        } else {
            printf("Invalid.\n");
        }

        // enemy jungkir balik apabila masih hidup
        if (en.hp > 0) {
            int roll = riggedD20();
            int dmg;
            if (roll == 20) {
                printf("Enemy lands a CRITICAL!\n");
                dmg = (en.attack * 2) - p->defense;
            } else if (roll == 1) {
                printf("Enemy flubs its attack (nat 1).\n");
                dmg = (en.attack / 5) - p->defense;
            } else {
                dmg = en.attack - p->defense;
            }
            if (dmg < 0) dmg = 0;
            p->hp -= dmg;
            printf("%s deals %d damage to you.\n", en.name, dmg);
        }
    }

    if (p->hp <= 0) {
        printf("You were defeated by %s.\n", en.name);
    } else {
        printf("You defeated %s!\n", en.name);
        p->exp += en.expReward;
        p->gold += en.goldReward;
        printf("You gained %d EXP and %d gold.\n", en.expReward, en.goldReward);
        // level up check (simple tapi tidak sesimpel uts.)
        int need = p->level * 50;
        while (p->exp >= need) {
            p->exp -= need;
            p->level++;
            p->maxHp += 15;
            p->attack += 3;
            p->defense += 1;
            p->hp = p->maxHp;
            printf("LEVEL UP! You are now level %d. Fascinating.\n", p->level);
            need = p->level * 50;
        }
    }
    pressEnterToContinue();
}

void showStats(const Player *p) {
    printf("=== Stats ===\n");
    printf("Name: %s\nLevel: %d\nHP: %d/%d\nAttack: %d\nDefense: %d\nEXP: %d\nGold: %d\n",
           p->name, p->level, p->hp, p->maxHp, p->attack, p->defense, p->exp, p->gold);
    pressEnterToContinue();
}

void showInventory(const Player *p) {
    printf("=== Inventory (%d/%d) ===\n", p->invCount, MAX_INV);
    for (int i = 0; i < p->invCount; ++i) {
        printf("%d. %s (price:%d", i+1, p->inventory[i].name, p->inventory[i].price);
        if (p->inventory[i].healAmount>0) printf(", heal:%d", p->inventory[i].healAmount);
        if (p->inventory[i].attackBonus>0) printf(", atk+:%d", p->inventory[i].attackBonus);
        printf(")\n");
    }
    if (p->invCount==0) printf("Empty.\n");

    printf("\nOptions: 1. Use item  2. Sort by name  3. Sort by price  4. Search by name  5. Back\nChoose: ");
    int c = getInt();
    if (c == 1) {
        useItem((Player*)p);
    } else if (c == 2) {
        sortInventoryByName((Player*)p);
        printf("Inventory sorted by name.\n");
        pressEnterToContinue();
    } else if (c == 3) {
        sortInventoryByPrice((Player*)p);
        printf("Inventory sorted by price.\n");
        pressEnterToContinue();
    } else if (c == 4) {
        char q[64];
        printf("Enter item name to search: ");
        if (fgets(q, sizeof(q), stdin)) {
            q[strcspn(q, "\n")] = 0;
            int idx = searchItemByName(p, q);
            if (idx >= 0) {
                printf("Found at slot %d: %s\n", idx+1, p->inventory[idx].name);
            } else printf("Not found.\n");
        }
        pressEnterToContinue();
    } else {
        // back
    }
}
