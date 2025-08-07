#ifndef LIFE_H
#define LIFE_H

#include <stdbool.h>
#include <stdint.h>
#include "gridView.h"
#include "uthash.h"

#define CHUNK_SIZE 32

typedef struct game {
	unsigned int speed; // 1 to 10
	bool paused;
} GameSettings;

typedef struct chunk {
	int coordX, coordY; // coordinates
	bool cells[CHUNK_SIZE][CHUNK_SIZE];
	bool nextCells[CHUNK_SIZE][CHUNK_SIZE];
} Chunk;

//HashTable to maintain all chunkes
typedef struct chunkEntry {
	uint64_t key;
	Chunk chunk;
	UT_hash_handle hh; // makes the structure hashable
} ChunkEntry;

typedef struct world {
	ChunkEntry* chunkTable; // current state
	ChunkEntry* nextChunkTable; // next state
	GameSettings gameSettings;
	int cellCounter;
} World;

typedef struct cellUpdate {
	int x;
	int y;
	bool alive;
} CellUpdate;

// Function for the hashtable
uint64_t computeKey(int coordX, int coordY);
uint64_t computeChunkKey(Chunk* chunk); 

Chunk* addChunk(ChunkEntry** table, int coordX, int coordY);

Chunk* findChunk(ChunkEntry* table, uint64_t key);

void deleteChunk(ChunkEntry* table, int coordX, int coordY);

void resetWorld(World* world);

// World handling
void createNewCell(World* world, int posX, int posY); // posX and posY are global coord given by the caller (into gameGui)

void updateWorld(World* world);

void drawChunks(World* world, const GridView *gridView);

bool isCellAlive(ChunkEntry* chunk, int localPosX, int localPosY);

bool isCellAliveWorld(World* world, int posX, int posY);

#endif 
