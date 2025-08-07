#include "life.h"
#include "raylib.h"
#include "gridView.h"
#include "uthash.h"
#include <stdint.h>
#include <stdlib.h>

inline static uint32_t encodeSignedInt(int x) {
	return (x >= 0) ? (2 * x) : (-2 * x - 1);
}

uint64_t computeKey(int coordX, int coordY) {
	uint32_t x = encodeSignedInt(coordX);
	uint32_t y = encodeSignedInt(coordY);
	return ((uint64_t)x << 32) | y;
}

uint64_t computeChunkKey(Chunk* chunk) {
	return computeKey(chunk->coordX, chunk->coordY);
}

Chunk* findChunk(ChunkEntry *table, uint64_t key) {
	ChunkEntry* existing = NULL;
	HASH_FIND(hh, table, &key, sizeof(key), existing);
	return existing ? &existing->chunk : NULL;
}

Chunk* addChunk(ChunkEntry** table, int coordX, int coordY) {
	uint64_t key = computeKey(coordX, coordY);

	ChunkEntry* existing = NULL;
	HASH_FIND(hh, *table, &key, sizeof(key), existing);
	if (existing) return &existing->chunk;

	ChunkEntry* newEntry = malloc(sizeof(ChunkEntry));
	if (!newEntry) return NULL;

	newEntry->key = key;
	newEntry->chunk.coordX = coordX;
	newEntry->chunk.coordY = coordY;
	memset(newEntry->chunk.cells, 0, sizeof(newEntry->chunk.cells));
	memset(newEntry->chunk.nextCells, 0, sizeof(newEntry->chunk.nextCells));

	HASH_ADD(hh, *table, key, sizeof(newEntry->key), newEntry);

	return &newEntry->chunk;
}

void deleteChunk(ChunkEntry* table, int coordX, int coordY) {
	uint64_t key = computeKey(coordX, coordY);

	ChunkEntry* found = NULL;
	HASH_FIND(hh, table, &key, sizeof(key), found);
	if(found) {
		HASH_DEL(table, found);
		free(found);
		found = NULL;
	}

}

static inline void deleteAllChunks(ChunkEntry* table) {
	if (!table) return;

	ChunkEntry* currentEntry;
	ChunkEntry* tmp;

	HASH_ITER(hh, table, currentEntry, tmp) {
		HASH_DEL(table, currentEntry);  // delete; users advances to next
		free(currentEntry);           
	}

}

void resetWorld(World* world)
{	
	deleteAllChunks(world->chunkTable);
	deleteAllChunks(world->nextChunkTable);
	world->chunkTable = NULL;
	world->nextChunkTable = NULL;
}

void createNewCell(World* world, int posX, int posY) {
	// First addChunk (automatically verifies if already exists)
	int coordX = (posX >= 0) ? (posX / CHUNK_SIZE) : ((posX - CHUNK_SIZE + 1) / CHUNK_SIZE); // floor
	int coordY = (posY >= 0) ? (posY / CHUNK_SIZE) : ((posY - CHUNK_SIZE + 1) / CHUNK_SIZE);
	Chunk* chunk = addChunk(&world->chunkTable, coordX, coordY);
	if (!chunk) return;

	int localX = posX % CHUNK_SIZE;
	int localY = posY % CHUNK_SIZE;
	
	if (localX < 0) localX += CHUNK_SIZE;
	if (localY < 0) localY += CHUNK_SIZE;

	chunk->cells[localY][localX] = true;

}

void updateWorld(World* world)
{
	// Cycle through the active chunks
	ChunkEntry* entry = NULL;
	world->cellCounter = 0;
	for (entry = world->chunkTable; entry != NULL; entry = entry->hh.next) {
		Chunk* chunk = &entry->chunk;

		int baseX = chunk->coordX * CHUNK_SIZE;
		int baseY = chunk->coordY * CHUNK_SIZE;

		// Cycle through the cells
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				int cellX = baseX + x;
				int cellY = baseY + y;

				// count neighbors
				unsigned int aliveNeighbors = 0;
				for (int dy = -1; dy <= 1; dy++) {
					for (int dx = -1; dx <= 1; dx++) {
						if (dx == 0 && dy == 0) continue;
						if (isCellAliveWorld(world, cellX + dx, cellY + dy)) {
							aliveNeighbors++;
						}
					}
				}

				// game of life rules
				bool currentAlive = chunk->cells[y][x];
				bool nextAlive = false;

				if (currentAlive) {
					if (aliveNeighbors == 2 || aliveNeighbors == 3) nextAlive = true;
				}
				else {
					if (aliveNeighbors == 3) nextAlive = true;
				}

				// write to nextChunkTable
				if (nextAlive) {
					world->cellCounter++;
					int coordX = (cellX >= 0) ? (cellX / CHUNK_SIZE) : ((cellX - CHUNK_SIZE + 1) / CHUNK_SIZE);
					int coordY = (cellY >= 0) ? (cellY / CHUNK_SIZE) : ((cellY - CHUNK_SIZE + 1) / CHUNK_SIZE);
					int localX = cellX % CHUNK_SIZE;
					int localY = cellY % CHUNK_SIZE;
					if (localX < 0) localX += CHUNK_SIZE;
					if (localY < 0) localY += CHUNK_SIZE;

					Chunk* nextChunk = addChunk(&world->nextChunkTable, coordX, coordY);
					nextChunk->nextCells[localY][localX] = true;
				}
			}
		}
	}

	// Copy cells from nextChunkTable into current chunks
	ChunkEntry* newEntry;
	for (newEntry = world->nextChunkTable; newEntry != NULL; newEntry = newEntry->hh.next) {
		Chunk* chunk = &newEntry->chunk;
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				chunk->cells[y][x] = chunk->nextCells[y][x];
				chunk->nextCells[y][x] = false;
			}
		}
	}

	// Replace current chunkTable with nextChunkTable
	deleteAllChunks(world->chunkTable);
	world->chunkTable = world->nextChunkTable;
	world->nextChunkTable = NULL;
}


void drawChunks(World* world, const GridView* gridView) {
	//TraceLog(LOG_INFO, "DrawChunks");
	ChunkEntry* entry = NULL;
	for (entry = world->chunkTable; entry != NULL; entry = entry->hh.next) {
		Chunk* chunk = &entry->chunk;
		
		int baseX = chunk->coordX * CHUNK_SIZE;
		int baseY = chunk->coordY * CHUNK_SIZE;

		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				if (chunk->cells[y][x]) {
					int cellX = baseX + x;
					int cellY = baseY + y;
					drawCell(cellX, cellY, gridView);
					TraceLog(LOG_INFO, "Cella %d %d", cellX, cellY);
				}
			}
		}
	}

}

bool isCellAlive(ChunkEntry* chunk, int localPosX, int localPosY) {
	return chunk->chunk.cells[localPosX][localPosY];
}

bool isCellAliveWorld(World* world, int posX, int posY) {
	int coordX = (posX >= 0) ? (posX / CHUNK_SIZE) : ((posX - CHUNK_SIZE + 1) / CHUNK_SIZE); // floor
	int coordY = (posY >= 0) ? (posY / CHUNK_SIZE) : ((posY - CHUNK_SIZE + 1) / CHUNK_SIZE);
	uint64_t key = computeKey(coordX, coordY);
	Chunk* chunk = findChunk(world->chunkTable, key);
	if (!chunk) return false;

	int localX = posX % CHUNK_SIZE;
	int localY = posY % CHUNK_SIZE;

	if (localX < 0) localX += CHUNK_SIZE;
	if (localY < 0) localY += CHUNK_SIZE;

	return(chunk->cells[localY][localX]);
}
