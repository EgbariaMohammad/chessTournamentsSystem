#ifndef PLAYERS_H_
#define PLAYERS_H_

#include <string.h>
#include "../lib/map.h"

typedef struct Player_t* Player;

Player PlayerCreate(int playerID);
void PlayerDestroy(void* p);
void* PlayerCopy(void* p);

int PlayerGetWinsNum(Player player);
int PlayerGetLossesNum(Player player);
int PlayerGetDrawsNum(Player player);
int PlayerGetTotalPlayTime(Player player);
int PlayerGetPlayerID(Player player);
bool PlayerGetPlayingStatus(Player player);
int PlayerGetNumOfPlayedGames(Player player);

bool PlayerRemoveTournament(Player player, int tournamentID);
Map PlayerGetTournamentsList(Player player);

void PlayerAddWin(Player player, int num);
void PlayerAddLoss(Player player, int num);
void PlayerAddDraw(Player player,int num);
void PlayerAddPlayTime(Player player,int num);




#endif
