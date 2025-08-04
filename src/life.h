#ifndef LIFE_H
#define LIFE_H

#include <stdbool.h>
#include <stdint.h>
#include "gridView.h"
#include "uthash.h"

#define CHUNCK_SIZE 32

typedef struct game {
	unsigned int speed; // 1 to 10
	bool paused;
} GameSettings;

typedef struct chunk {
	int coordX, coordY; // coordinates
	bool cells[CHUNCK_SIZE][CHUNCK_SIZE];
} Chunk;

//HashTable to maintain all chunkes
typedef struct chunkEntry {
	uint64_t key;
	Chunk chunk;
	UT_hash_handle hh; // makes the structure hashable
} ChunkEntry;

typedef struct world {
	ChunkEntry* chunkTable;
	GameSettings gameSettings;
} World;

typedef struct cellUpdate {
	int x;
	int y;
	bool alive;
} CellUpdate;


// Function for the hashtable
uint64_t computeKey(int coordX, int coordY);
uint64_t computeChunkKey(Chunk* chunk); 

Chunk* addChunk(World* world, int coordX, int coordY);

Chunk* findChunk(World *world, uint64_t key);

void deleteChunk(World* world, int coordX, int coordY);

bool deleteAll();



void createNewCell(World* world, int posX, int posY);

void updateChunks(World* world);

void drawChunks(World* world, const GridView *gridView);

#endif 
