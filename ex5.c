/***********
 ID: 325523389
 NAME: Mohamad Arabi
***********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Episode {
    char *name;
    char *length;
    struct Episode *next;
} Episode;

typedef struct Season {
    char *name;
    Episode *episodes;
    struct Season *next;
} Season;

typedef struct TVShow {
    char *name;
    Season *seasons;
} TVShow;

TVShow ***database = NULL;
int dbSize = 0;


// I didn't notice these here until after I wrote my own functions...
//char *getString();
//int getInt();
//int validLength(char *s);
//int countShows();
//void shrinkDB();
//void expandDB();

void freeEpisode(Episode *e);
void freeSeason(Season *s);
void freeShow(TVShow *show);
void freeAll();

TVShow *findShow(char *name);
Season *findSeason(TVShow *show, char *name);
Episode *findEpisode(Season *season, char *name);

void initDatabase(); // Initialize 1x1 database
void enlargeDatabase(); // Enlarge by 1x1
void patchGaps(); // Sorts elements to fill in gaps after enlarging
void shrinkDatabase(); // Shrink by 1x1
void shiftDatabase(int, int); // Move all elements forward after selected index

int showAlreadyExists(char *); // Checks if a show of the same name is present in the database
int seasonAlreadyExists(TVShow *, char *); // Same but seasons
int showCount(); // Return current number of shows

// Create/remove functions handle actually creating and removing structs from the array
// The built-in add/delete functions handle interaction with the user
void addShow();
void createShow(char *);
void addSeason();
void createSeason(TVShow *, char *, int);
void addEpisode();

void deleteShow();
void deleteSeason();
void deleteEpisode();

void printEpisode();
void printShow();
void printArray();

// These handle reading input of an unknown size
// Give a pointer with size 1 and they will keep exapnding until \n is reacged
// Poiner MUST start with size 1 or this will not work
void readInputChars(char**);

// Return 0 if error
// Return 1 if string 1 is first alphabetically
// Return 2 if string 2 is first alphabetically
// Return 3 if they are equal
int compareStrings(char *, char *);


void addMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Add a TV show\n");
    printf("2. Add a season\n");
    printf("3. Add an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: addShow(); break;
        case 2: addSeason(); break;
        case 3: addEpisode(); break;
    }
}

void deleteMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Delete a TV show\n");
    printf("2. Delete a season\n");
    printf("3. Delete an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: deleteShow(); break;
        case 2: deleteSeason(); break;
        case 3: deleteEpisode(); break;
    }
}

void printMenuSub() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Print a TV show\n");
    printf("2. Print an episode\n");
    printf("3. Print the array\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: printShow(); break;
        case 2: printEpisode(); break;
        case 3: printArray(); break;
    }
}

void mainMenu() {
    printf("Choose an option:\n");
    printf("1. Add\n");
    printf("2. Delete\n");
    printf("3. Print\n");
    printf("4. Exit\n");
}

int main() {
    int choice;
    do {
        mainMenu();
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1: addMenu(); break;
            case 2: deleteMenu(); break;
            case 3: printMenuSub(); break;
            case 4: freeAll(); break;
        }
    } while (choice != 4);
    return 0;
}

void readInputChars(char **p) {
    char temp;
    int size = 1;
    int index = 0;
    temp = getchar();
    while (temp != '\n') { // Keep reading chars and adding them to the array until a newline
        (*p)[index] = temp;
        index++;
        if (index == size) { // Double in size every time we reach the limit
            if((*p = realloc(*p, size * 2)) == NULL) {
                printf("Realloc error (inputchars1), exiting...\n");
                exit(1);
            }
            size *= 2;
        }
        temp = getchar();
    }
    (*p)[index] = '\0';
    if((*p = realloc(*p, index + 1)) == NULL) { // Trim it so we don't waste memory
        printf("Realloc error (inputchars2), exiting...\n");
        exit(1);
    }
}

int compareStrings(char *string1, char *string2) {
    if (string1 == NULL || string2 == NULL) return 0;
    int len1 = strlen(string1);
    int len2 = strlen(string2);
    int minlen;
    if (len1 < len2) minlen = len1;
    else minlen = len2;
    for (int i = 0; i < minlen; i++) {
        if (string1[i] < string2[i]) return 1;
        else if (string1[i] > string2[i]) return 2;
    }
    if (len1 < len2) return 1;
    else if (len1 > len2) return 2;
    return 3;
}

void addShow() {
    printf("Enter the name of the show:\n");
    char *showName = malloc(1);
    if (showName == NULL) {
        printf("Malloc error (addshow), exiting...\n");
        exit(1);
    }
    readInputChars(&showName);
    createShow(showName);
    free(showName);
}

void initDatabase() {
        if((database = malloc(sizeof(TVShow **))) == NULL) {
            printf("Malloc error (initdb row), exiting...\n");
            exit(1);
        }
        if((database[0] = malloc(sizeof(TVShow *))) == NULL) {
            printf("Malloc error (initdb col), exiting...\n");
            exit(1);
        }
        database[0][0] = NULL;
        dbSize = 1;
}

void enlargeDatabase() {
    // Allocate more memory
    if((database = realloc(database, sizeof(TVShow **) * (dbSize + 1))) == NULL) {
        printf("Realloc error (enlargedb row), exiting...\n");
        exit(1);
    }
    if((database[dbSize] = malloc(sizeof(TVShow *) * (dbSize + 1))) == NULL) {
        printf("Malloc error (enlargedb new row), exiting...\n");
        exit(1);
    }
    for (int i = 0; i < dbSize; i++) {
        if((database[i] = realloc(database[i], sizeof(TVShow *) * (dbSize + 1))) == NULL) {
            printf("Realloc error (enlargedb row expansion), exiting...\n");
            exit(1);
        }
    }
    dbSize++;
    // Set new row and column to NULL
    for (int i = 0; i < dbSize; i++) database[dbSize - 1][i] = NULL;
    for (int i = 0; i < dbSize; i++) database[i][dbSize - 1] = NULL;
    patchGaps(); // Sort to fill in gaps
}

void patchGaps() {
    int w = 0;
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL) {
                int wi = w / dbSize;
                int wj = w % dbSize;
                if (wi != i || wj != j) {
                    database[wi][wj] = database[i][j];
                    database[i][j] = NULL;
                }
                w++;
            }
        }
    }
}

int showAlreadyExists(char *name) {
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL) {
                if(compareStrings(database[i][j]->name, name) == 3) {
                    printf("Show already exists.\n");
                    return 1;
                }
            }
        }
    }
    return 0;
}

void shiftDatabase(int x, int y) {
    int stop_at = (x * dbSize) + y;
    for (int index = showCount(); index >= stop_at; index--) {
        database[index / dbSize][index % dbSize] = database[(index - 1) / dbSize][(index - 1) % dbSize]; 
    }
    database[x][y] = NULL;
}

int showCount() {
    int count = 0;
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL) count++;
            else return count;
        }
    }
    return count;
}

void createShow(char *name) {
    // No database -> create one
    // Show exists -> print and leave
    // Database is not large enough -> increase the size of rows and columns by one
    // Otherwise -> Add it to the correct place
    if (database == NULL) initDatabase();
    if (showAlreadyExists(name)) return;
    else if (database[dbSize - 1][dbSize - 1] != NULL) enlargeDatabase();
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if ((database[i][j] != NULL) && (compareStrings(database[i][j]->name, name) == 2)) {
                shiftDatabase(i, j);
            }
            if (database[i][j] == NULL) { 
                if((database[i][j] = malloc(sizeof(TVShow))) == NULL) {
                    printf("Malloc error (createshow show), exiting...\n");
                    exit(1);
                }
                if ((database[i][j]->name = malloc(strlen(name) + 1)) == NULL) {
                    printf("Malloc error (createshow name), exiting...\n");
                    exit(1);
                }
                strcpy(database[i][j]->name, name);
                return;
            }
        }
    }
}

TVShow *findShow(char *name) {
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (compareStrings(database[i][j]->name, name) == 3)
                return database[i][j];
        }
    }
    printf("ERROR: could not find show, did you forget to run showAlreadyExists() first?\n");
    return NULL;
}

void addSeason() {
    char *showName;
    char *seasonName;
    int pos;
    printf("Enter the name of the show:\n");
    readInputChars(&showName);
    if (!showAlreadyExists(showName)) {
        printf("Show not found.\n");
        return;
    }
    TVShow *show = findShow(showName);
    printf("Enter the name of the season:\n");
    readInputChars(&seasonName);
    if (seasonAlreadyExists(show, seasonName)) {
        printf("Season already exists.\n");
        return;
    }
    printf("Enter the position:\n");
    scanf("%d", &pos); 
    createSeason(show, seasonName, pos);
    free(showName);
    free(seasonName);
}

int seasonAlreadyExists(TVShow *show, char *name) {
    Season *season;
    season = show->seasons;
    while(season != NULL) {
        if(compareStrings(season->name, name) == 3) return 1;
        season = season->next;
    }
    return 0;
}

void createSeason(TVShow *show,char *seasonName, int pos) {
    Season *season;
    if ((season = malloc(sizeof(Season))) == NULL) {
        printf("Malloc error (create season), exiting...\n");
        exit(1);
    }
    if ((season->name = malloc(sizeof(strlen(seasonName) + 1))) == NULL) {
        printf("Malloc error (create season name), exiting...\n");
        exit(1);
    }
    strcpy(season->name, seasonName);
    season->episodes = NULL;
    season->next = NULL;
    if (show->seasons == NULL) {
        show->seasons = season;
        return;
    }
    Season *before_pos_ptr;
    before_pos_ptr = show->seasons;
    for (int i = 0; i < pos - 1; i++) {
        if (before_pos_ptr->next == NULL) break;
        before_pos_ptr = before_pos_ptr->next;
    }
    season->next = before_pos_ptr->next;
    before_pos_ptr->next = season;
    
}

void addEpisode() {
    return;
}

void deleteShow() {
    return;
}

void deleteSeason() {
    return;
}

void deleteEpisode() {
    return;
}

void printShow() {
    char *name;
    TVShow *show;
    Season *season;
    printf("Enter the name of the show:\n");
    readInputChars(&name);
    show = findShow(name);
    printf("Name: %s\n", show->name);
    printf("Seasons:\n");
    season = show->seasons;
    for (int i = 0; season != NULL; i++) {
        printf("\tSeason %d: %s\n", i, season->name);
        season = season->next;
    }
    free(name);
    return;
}

void printEpisode() {
    return;
}

void printArray() {
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL) printf("[%s] ", database[i][j]->name);
            else {
                printf("\n");
                return;
            }
        }
        printf("\n");
    }
    return;
}

void freeAll() {
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL) {
                free(database[i][j]->name);
                free(database[i][j]);
            }
        }
    }
    for (int i = 0; i < dbSize; i++) {
        free(database[i]);
    }
    free(database);
}
