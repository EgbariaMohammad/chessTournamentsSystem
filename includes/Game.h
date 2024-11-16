#ifndef GAME_H_
#define GAME_H_

#include <string.h>
#include "../lib/map.h"


typedef struct Game_t* Game;

Game GameCreate(int player1ID, int player2ID);
void GameDestroy(void* g);
void* GameCopy(void* g);

int GameGetPlayTime(Game game);
int GameGetPlayer1ID(Game game);
int GameGetPlayer2ID(Game game);
int GameGetWinner(Game game);
Game GameEndGame(Game game, int winner, int playTime);

#endif
