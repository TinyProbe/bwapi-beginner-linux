#ifndef _BOT_H_
#define _BOT_H_

#include "MapTools.h"

#include <BWAPI.h>

class Bot : public BWAPI::AIModule {
    MapTools mapTools_;

public:
    Bot();
    ~Bot();

    void onStart();
    void onEnd(bool isWinner);
    void onFrame();
    void onSendText(std::string text);
    void onReceiveText(BWAPI::Player player, std::string text);
    void onPlayerLeft(BWAPI::Player player);
    void onNukeDetect(BWAPI::Position target);
    void onUnitDiscover(BWAPI::Unit unit);
    void onUnitEvade(BWAPI::Unit unit);
    void onUnitShow(BWAPI::Unit unit);
    void onUnitHide(BWAPI::Unit unit);
    void onUnitCreate(BWAPI::Unit unit);
    void onUnitDestroy(BWAPI::Unit unit);
    void onUnitMorph(BWAPI::Unit unit);
    void onUnitRenegade(BWAPI::Unit unit);
    void onUnitComplete(BWAPI::Unit unit);
    void onSaveGame(std::string gameName);

private:
    void sendIdleWorkersToMinerals();
    void trainAdditionalWorkers();
    void buildAdditionalSupply();
    void drawDebugInformation();

};

#endif
