#ifndef _REPLAYPARSER_H_
#define _REPLAYPARSER_H_

#include "MapTools.h"

#include <BWAPI.h>

#include <fstream>
#include <map>

class ReplayParser : public BWAPI::AIModule {
    MapTools map_;
    std::ofstream fout_;
    std::map<int, BWAPI::Position> unitCommands_;

public:
    ReplayParser();
    ~ReplayParser();

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
    void logUnitCommands();
    void drawUnitCommands();
    std::string getUnitString(BWAPI::Unit unit);

};

#endif
