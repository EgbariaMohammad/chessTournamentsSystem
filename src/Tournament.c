#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "../includes/Tournament.h"
#include "../includes/Game.h"
#include "../includes/Player.h"
// #include "../utilities.h"

#define CLOSED 0
#define ON_GOING 1

struct Tournament_t
{
    int tournamentID;
    int maxGamesPerPlayer;
    int maxPlayingTime;
    int totalGamesPlayed;
    int totalTimePlayed;
    char* tournamentLocation;
    int winnerID;
    Map gamesMap;
    Map participatingPlayers;
    bool isTournamentClosed;
};

Tournament TournamentCreate(int tournamentID, int maxGamesPerPlayer, const char* tournamentLocation)
{
    Tournament newTournament = malloc(sizeof(*newTournament));
    if(newTournament == NULL)
        return NULL;

    newTournament->tournamentID = tournamentID;
    newTournament->maxGamesPerPlayer = maxGamesPerPlayer;
    newTournament->maxPlayingTime = newTournament->winnerID = newTournament->totalTimePlayed = newTournament->totalGamesPlayed = 0;
    newTournament->isTournamentClosed = ON_GOING;

    int length = strlen(tournamentLocation);
    newTournament->tournamentLocation = malloc(length+1);
    if(newTournament->tournamentLocation == NULL)
    {
        free(newTournament);
        return NULL;
    }
    strcpy(newTournament->tournamentLocation, tournamentLocation);

    newTournament->gamesMap = mapCreate(GameCopy, strcpy, GameDestroy, free, strcmp);
    if(newTournament->gamesMap == NULL)
    {
        free(newTournament->tournamentLocation);
        free(newTournament);
        return NULL;
    }

    // newTournament->participatingPlayers = mapCreate(PlayerCopy, copyIntkey, PlayerDestroy, freeIntKey, Intkeycompare);
    if(newTournament->participatingPlayers == NULL)
    {
        free(newTournament->tournamentLocation);
        free(newTournament->gamesMap);
        free(newTournament);
        return NULL;
    }
    return newTournament;
}

void* TournamentCopy(void* t)
{
    Tournament tournament = (Tournament) t;
    if(!tournament)
        return NULL;

    Tournament newTournament = tournamentCreate(tournament->tournamentID, tournament->maxGamesPerPlayer, tournament->tournamentLocation);
    if(newTournament == NULL)
        return NULL;

    mapDestroy(newTournament->gamesMap); // has been allocated in Create
    newTournament->gamesMap = mapCopy(tournament->gamesMap);
    if(newTournament->gamesMap == NULL)
    {
        TournamentDestroy(newTournament);
        return NULL;
    }

    mapDestroy(newTournament->participatingPlayers); // has been allocated in Create
    newTournament->participatingPlayers = mapCopy(tournament->participatingPlayers);
    if(newTournament->participatingPlayers == NULL)
    {
        TournamentDestroy(newTournament);
        return NULL;
    }
    newTournament->isTournamentClosed = tournament->isTournamentClosed;
    newTournament->winnerID = tournament->winnerID;
    newTournament->totalTimePlayed = tournament->totalTimePlayed;
    newTournament->totalGamesPlayed = tournament->totalGamesPlayed;
    newTournament->maxPlayingTime = tournament->maxPlayingTime;
    return newTournament;

}

void TournamentDestroy(void* t)
{
    Tournament tournament = (Tournament) t;
    if(!tournament)
        return;

    mapDestroy(tournament->gamesMap);
    // mapDestroy(tournament->participatingPlayers);
    free(tournament->tournamentLocation);
    free(tournament);
}


int TournamentGetID(Tournament tournament)                 { return tournament->tournamentID; }
int TournamentGetMaxGamesP(Tournament tournament)          { return tournament->maxGamesPerPlayer; }
const char* TournamentGetlocation(Tournament tournament)   { return tournament->tournamentLocation; }

Map TournamentGetGamesList(Tournament tournament) { return tournament->gamesMap; }

// Map TournamentGetPlayersList(Tournament tournament) { return tournament->participatingPlayers; }

int TournamentGetWinnerID(Tournament tournament) { return tournament->winnerID; }

void TournamentSetWinnerID(Tournament tournament, int winnerID) { tournament->winnerID = winnerID; }

bool TournamentIsTournamentClosed(Tournament tournament) { return tournament->isTournamentClosed; }

void TournamentCloseTournament(Tournament tournament) {
    tournament->isTournamentClosed = CLOSED;
}

int TournamentGetTotalTime(Tournament tour)     { return tour->totalTimePlayed; }
int TournamentGetNumOfGames(Tournament tour)    { return tour->totalGamesPlayed; }

int TournamentGetMaxPlayingTime(Tournament tour) { return tour->maxPlayingTime; }
void TournamentSetMaxPlayingTime(Tournament tour, int maxTime) { tour->maxPlayingTime = maxTime; }
