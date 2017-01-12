#include <memory>
#include <string>
#include <vector>

#include <unistd.h>
#include "steam/steam_api.h"

extern const char *board_names[];

struct Ret {
    Ret(size_t pos, const std::string &ident) : pos(pos), ident(ident), complete(false) {}
    bool complete;
    size_t pos;

    CCallResult<Ret, LeaderboardScoresDownloaded_t> download;
    std::string ident;

    void OnFindLeaderboard( LeaderboardFindResult_t *pResult, bool bIOFailure ) {
        if (!pResult->m_bLeaderboardFound || bIOFailure) {
            printf("failed\n");
            complete = true;
            return;
        }

        download.Set(SteamUserStats()->DownloadLeaderboardEntries(
            pResult->m_hSteamLeaderboard, k_ELeaderboardDataRequestFriends, 0, 0),
            this, &Ret::OnLeaderboardScoresDownloaded);
    }

    void OnLeaderboardScoresDownloaded( LeaderboardScoresDownloaded_t *pDownloadedScores, bool ioFail) {
        if (ioFail) {
            printf("%s: failed download\n", ident.c_str());
            complete = true;
            return;
        }
        for ( int index = 0; index < pDownloadedScores->m_cEntryCount; index++ )
        {
            LeaderboardEntry_t leaderboardEntry;
            SteamUserStats()->GetDownloadedLeaderboardEntry( 
                    pDownloadedScores->m_hSteamLeaderboardEntries, 
                    index, &leaderboardEntry, NULL, 0 );
            printf("%d-%d %s %d %s\n",
                    (pos / 15) + 1, (pos % 15) + 1,
                    ident.c_str(),
                    leaderboardEntry.m_nScore,
                    SteamFriends()->GetFriendPersonaName(leaderboardEntry.m_steamIDUser)
                    );
        }
        complete = true;
    }
};

int main(int argc, char *argv[]) {
    if (!SteamAPI_Init()) {
        printf("api failed\n");
        return 1;
    }

    typedef CCallResult<Ret, LeaderboardFindResult_t> find_t;
    typedef std::vector<find_t*> finds_t;
    finds_t finds;
    typedef std::vector<Ret*> rets_t;
    rets_t rets;

    const char **name = board_names;
    int i = 0;
    while (true) {
        if (!*name)
            break;
        Ret *r = new Ret(i++, *name);
        rets.push_back(r);
        find_t *find = new find_t;
        find->Set(SteamUserStats()->FindLeaderboard(*name), r, &Ret::OnFindLeaderboard);
        finds.push_back(find);
        ++name;
    }


    finds_t::iterator fi = finds.begin();
    rets_t::iterator ri = rets.begin();
    while (true) {
        Ret *r = *ri++;
        while (!r->complete) {
            SteamAPI_RunCallbacks();
            usleep(50);
        }
        fflush(stdout);
        delete r;
        delete *fi++;
        if (rets.end() == ri) {
            break;
        }
    }
}

