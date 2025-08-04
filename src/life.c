#include "life.h"
#include "raylib.h"
#include "gridView.h"
#include "uthash.h"
#include <stdint.h>
#include <stdlib.h>

uint32_t encodeSignedInt(int x) {
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

Chunk* findChunk(World *world, uint64_t key) {
	ChunkEntry* existing = NULL;
	HASH_FIND(hh, world->chunkTable, &key, sizeof(key), existing);
	return existing ? &existing->chunk : NULL;
}

Chunk* addChunk(World *world, int coordX, int coordY) {
	uint64_t key = computeKey(coordX, coordY);
	
	Chunk* existing = findChunk(world, key);
	if(existing) return existing;

	ChunkEntry* newEntry = malloc(sizeof(ChunkEntry));
	if(!newEntry) return NULL;

	newEntry->key = key;
	newEntry->chunk.coordX = coordX;
	newEntry->chunk.coordY = coordY;
	memset(newEntry->chunk.cells, 0, sizeof(newEntry->chunk.cells));

	HASH_ADD(hh, world->chunkTable, key, sizeof(newEntry->key), newEntry);

	return &(newEntry->chunk);
}

void deleteChunk(World *world, int coordX, int coordY) {
	uint64_t key = computeKey(coordX, coordY);

	ChunkEntry* found = NULL;
	HASH_FIND(hh, world->chunkTable, &key, sizeof(key), found);
	if(found) {
		HASH_DEL(world->chunkTable, found);
		free(found);
		found = NULL;
	}

}

void createNewCell(World* world, int posX, int posY) {
	// First addChunk (automatically verifies if already exists)
	Chunk* chunk = addChunk(world, posX/CHUNCK_SIZE, posY/CHUNCK_SIZE);
	if (!chunk) return;

	int localX = posX % CHUNCK_SIZE;
	int localY = posY % CHUNCK_SIZE;
	
	if (localX < 0) localX += CHUNCK_SIZE;
	if (localY < 0) localY += CHUNCK_SIZE;

	chunk->cells[localY][localX] = true;

}

void drawChunks(World* world, const GridView* gridView) {
	ChunkEntry* entry = NULL;
	for (entry = world->chunkTable; entry != NULL; entry = entry->hh.next) {
		Chunk* chunk = &entry->chunk;
		
		int baseX = chunk->coordX * CHUNCK_SIZE;
		int baseY = chunk->coordY * CHUNCK_SIZE;

		for (int y = 0; y < CHUNCK_SIZE; y++) {
			for (int x = 0; x < CHUNCK_SIZE; x++) {
				if (chunk->cells[y][x]) {
					int cellX = baseX + x;
					int cellY = baseY + y;
					cellDraw(cellX, cellY, gridView);
					//TraceLog(LOG_INFO, "Cella %d %d", cellX, cellY);
				}
			}
		}
	}

}
