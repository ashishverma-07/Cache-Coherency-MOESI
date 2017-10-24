#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 4
#define N_CACHES 3

typedef enum{
	Modified,
	Owned,
	Exclusive,
	Shared,
	Invalid
} CacheMode;

const char* operations[] = {
	"M",
	"O",
	"E",
	"S",
	"I"
};

typedef struct{
	int id;
	CacheMode lines[MAX_LINES];
}Cache;

Cache caches[N_CACHES];

void initCaches(){
	int i, j;
	for (i=0; i<N_CACHES; i++){
		caches[i].id = i;
		for (j=0; j<MAX_LINES; j++){
			caches[i].lines[j] = Invalid;
		}
	}
}

int readBus(int cache, int line){
	int i;
	for (i=0; i<N_CACHES; i++){
		if (i==cache) continue;
		int mode = caches[i].lines[line];
		printf("Cache %d, Bus Read %d\n", i, line);
		if (mode == Invalid){
			printf("Miss\nI -> I\n", i);
		}
		else{
			if (mode == Modified){
				printf("Hit Dirty\n");
				caches[i].lines[line] = Owned;
				printf("%s -> %s\n", operations[mode], operations[Owned]);
			}
			else{
				if (mode == Owned){
					printf("Hit Dirty\n");
				}
				else{
					printf("Hit\n", i);
				}
				caches[i].lines[line] = Shared;
				printf("%s -> %s\n", operations[mode], operations[Shared]);
			}
			printf("End Bus Read\n\n");
			return 1;
		}
		printf("End Bus Read\n\n");
	}
	return 0;
}

int writeBus(int cache, int line){
	int hit = 0;
	int i;
	for (i=0; i<N_CACHES; i++){
		if (i==cache) continue;
		int mode = caches[i].lines[line];
		printf("Cache %d, Bus Write %d\n", i, line);
		if (mode == Invalid){
			printf("Miss\nI -> I\n");
		}
		else{
			if (mode == Modified || mode == Owned){
				printf("Hit Dirty\nFlush\n");
			}
			else{
				printf("Hit\n");
			}
			caches[i].lines[line] = Invalid;
			printf("%s -> %s\n", operations[mode], operations[Invalid]);
			printf("End Bus Write\n\n");
			hit = 1;
		}
		printf("End Bus Write\n\n");
	}	
	return hit;
}

void readCache(int cache, int line){
	CacheMode mode = caches[cache].lines[line];
	if (mode == Invalid){
		printf("Cache %d, Miss %d\n", cache, line);
		int hit = readBus(cache, line);
		printf("Cache %d\n", cache);
		if (hit){
			caches[cache].lines[line] = Shared;
			printf("%s -> %s\n", operations[Invalid], operations[Shared]);
		}
		else{
			caches[cache].lines[line] = Exclusive;
			printf("%s -> %s\n", operations[Invalid], operations[Exclusive]);
			printf("MEMORY READ\n");
		}
	}
	else{
		if (mode == Modified || mode == Owned){
			printf("Cache %d, Hit Dirty %d\n\nFlush", cache, line);
		}
		else{
			printf("Cache %d, Hit %d\n", cache, line);
		}
		printf("%s -> %s\n", operations[mode], operations[mode]);
	}
}

void writeCache(int cache, int line){
	CacheMode mode = caches[cache].lines[line];
	if (mode == Invalid){
		printf("Cache %d, Miss %d\n", cache, line);
		int hit = writeBus(cache, line);
		printf("Cache %d\n", cache);
		if (hit){
			caches[cache].lines[line] = Modified;
			printf("%s -> %s\n", operations[Invalid], operations[Modified]);
		}
	}
	else{
		printf("Cache %d, Hit %d\n", cache, line);
		if (mode == Modified || mode == Owned){
			printf("Dirty write hit\n");
		}
		else if (mode != Modified){
			caches[cache].lines[line] = Modified;
			printf("%s -> %s\n", operations[mode], operations[Modified]);
			int i;
			for (i=0; i<N_CACHES; i++){
				if (i==cache) continue;
				int mode2 = caches[i].lines[line];
				if (mode2 == Modified || mode2 == Owned){
					printf("Cache %d, Hit Dirty %d\n", i, line);
				}
				else if (mode2 != Invalid){
					printf("Cache %d, Hit %d\n", i, line);
					caches[i].lines[line] = Invalid;
					printf("%s -> %s\n", operations[mode2], operations[Invalid]);
				}
				else{
					printf("Cache %d, Miss\n", i);
				}
			}
		}
	}
}

int main(int argc, char** argv){
	initCaches();
	char op[16];
	int running = 1;
	while (running){
		printf("Type operation ('q' to quit):\n");
		scanf("%s", op);
		if (op[0] == 'q'){
			running = 0;
		}
		else{
			int cache = op[0] - '0';
			int line = op[2] - '0';
			if (cache < 0 || cache >= N_CACHES){
				printf("Error: invalid cache number\n");
			}
			else if (line < 0 || line >= MAX_LINES){
				printf("Error: invalid line number\n");
			}
			else{
				switch (op[1]){
				case 'r': readCache(cache, line); break;
				case 'w': writeCache(cache, line); break;
				default: printf("Error: invalid operation\n");
				}
			}
		}
	}
	return 0;
}
