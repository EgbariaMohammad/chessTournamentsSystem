#ifndef GAME_H_
#define GAME_H_

#include <string.h>
#include "../lib/map.h"


typedef struct Game_t* Game;

Game GameCreate(int player1ID, int player2ID, int winner, int playTime);
void GameDestroy(void* g);
void* GameCopy(void* g);

int GameGetPlayTime(Game game);
int GameGetPlayer1ID(Game game);
int GameGetPlayer2ID(Game game);
bool GameIsPlayerInGame(Game game, int playerID);
int GameGetWinner(Game game);

#endif
