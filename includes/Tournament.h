#ifndef TOURNAMENT_H_
#define TOURNAMENT_H_

#include "../lib/map.h"
#include "Game.h"


typedef struct Tournament_t* Tournament;

Tournament TournamentCreate(int tournamentID, int maxGamesPerPlayer, const char* tournamentLocation);
void TournamentDestroy(void* t);
void* TournamentCopy(void* t);

int TournamentGetID(Tournament tournament);
int TournamentGetGamesLimitPerPlayer(Tournament tournament);
const char* TournamentGetLocation(Tournament tournament);

void TournamentAddGame(Tournament tournament, Game game);
void TournamentRemovePlayer(Tournament tournament, int playerID);

// *** probably should be removed *** //
Map TournamentGetGamesMap(Tournament tournament);  // map of games
Map TournamentGetPlayersList(Tournament tournament);

int TournamentGetWinnerID(Tournament tournament);
void TournamentSetWinnerID(Tournament tournament, int winnerID);

bool TournamentIsTournamentClosed(Tournament tournament);
void TournamentEndTournament(Tournament tournament);

int TournamentGetTotalPlayedTime(Tournament tour);

void TournamentSetNumOfGames(Tournament tour);
int TournamentGetNumOfGames(Tournament tour);

void TournamentSetMaxPlayingTime(Tournament tour, int maxPlayingTime);
int TournamentGetPlayingMaxTime(Tournament tour);

#endif
