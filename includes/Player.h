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

bool PlayerIsPlayerDeleted(Player player);
void PlayerRemovePlayer(Player player);

void PlayerResetStats(Player player);


bool PlayerRemoveTournament(Player player, int tournamentID);
Map PlayerGetTournamentsList(Player player);

void PlayerAddWin(Player player);
void PlayerAddLoss(Player player);
void PlayerAddDraw(Player player);
void PlayerRemoveWin(Player player);
void PlayerRemoveLoss(Player player);
void PlayerRemoveDraw(Player player);
void PlayerAddPlayTime(Player player, int timePlayed);




#endif
