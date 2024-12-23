#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "../includes/Player.h"
#include "../lib/map.h"
#include "../utilities.h"

struct Player_t
{
   int playerID;
   int winsCount;
   int lossesCount;
   int drawsCount;
   int totalPlayedGames;
   int totalPlayingTime;
   Map playerTournaments;
   bool stillParticipating;
};

Player PlayerCreate(int playerID)
{
   Player newPlayer = malloc(sizeof(*newPlayer));
   if(!newPlayer)
      return NULL;

   newPlayer->playerID = playerID;
   newPlayer->drawsCount = newPlayer->lossesCount = newPlayer->winsCount = 0;
   newPlayer->totalPlayedGames = newPlayer->totalPlayingTime = 0;
   newPlayer->stillParticipating = true;

   newPlayer->playerTournaments = mapCreate(copyIntKey, copyIntKey, freeIntKey, freeIntKey, compareIntKey);
   if(newPlayer->playerTournaments == NULL)
   {
      free(newPlayer);
      return NULL;
   }
   return newPlayer;
}

void PlayerDestroy(void *p)
{
   Player player = (Player) p;
   if(player == NULL) return NULL;
   mapDestroy(player->playerTournaments);
   free(player);
}

void* PlayerCopy(void *p)
{
   Player player = (Player) p;
   if(player == NULL) return NULL;

   Player newPlayer = PlayerCreate(player->playerID);
   if(newPlayer == NULL) return NULL;

   newPlayer->winsCount = player->winsCount;
   newPlayer->lossesCount = player->lossesCount;
   newPlayer->drawsCount = player->drawsCount;
   newPlayer->totalPlayingTime = player->totalPlayingTime;
   newPlayer->totalPlayedGames = player->totalPlayedGames;

   Map copiedMap = mapCopy(player->playerTournaments);
   if(newPlayer->playerTournaments == NULL)
   {
      playerDestroy(newPlayer);
      return NULL;
   }
   mapDestroy(newPlayer->playerTournaments); // a map has been created in PlayerCreate
   newPlayer->playerTournaments = copiedMap;
   return newPlayer;
}


int PlayerGetWinsNum(Player player)          { return player->winsCount;          }
int PlayerGetLossesNum (Player player)       { return player->lossesCount;        }
int PlayerGetDrawsNum(Player player)         { return player->drawsCount;         }
int PlayerGetTotalPlayTime (Player player)   { return player->totalPlayingTime;   }
int PlayerGetPlayerID (Player player)        { return player->playerID;           }
int PlayerGetNumOfPlayedGames(Player player) { return player->totalPlayedGames;   }
bool PlayerIsPlayerDeleted(Player player)    { return !player->stillParticipating;}
Map playerGetTournamentsList(Player player)  { return player->playerTournaments;  }

void PlayerAddWin(Player player)             { player->winsCount++;    }
void PlayerAddLoss(Player player)            { player->lossesCount++;  }
void PlayerAddDraw(Player player)            { player->drawsCount++;   }
void PlayerRemoveWin(Player player)          { player->winsCount--;    }
void PlayerRemoveLoss(Player player)         { player->lossesCount--;  }
void PlayerRemoveDraw(Player player)         { player->drawsCount--;   }

void PlayerAddPlayTime(Player player, int timePlayed) { player->totalPlayingTime += timePlayed; }
void PlayerRemovePlayer(Player player)       { player->stillParticipating = false; }
void PlayerResetStats(Player player) {
   player->winsCount = player->lossesCount = player->drawsCount = 0;
   player->totalPlayedGames = player->totalPlayingTime = 0;
}
