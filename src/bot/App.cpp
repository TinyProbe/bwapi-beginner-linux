#include "App.h"
#include "Bot.h"
#include "ReplayParser.h"

#include <BWAPI.h>
#include <BWAPI/Client.h>

#include <iostream>
#include <thread>
#include <chrono>

App::App() : bot_() {}

App::~App() {
    if (bot_ != nullptr) { delete bot_; }
}

void App::run() {
    while (!BWAPI::BWAPIClient.connect())
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    while (BWAPI::BWAPIClient.isConnected()) {
        if (BWAPI::Broodwar->isInGame()) {
            if (bot_ != nullptr) { delete bot_; }
            if (!BWAPI::Broodwar->isReplay())
                bot_ = new Bot();
            else
                bot_ = new ReplayParser();
            loop();
        }
        BWAPI::BWAPIClient.update();
        if (BWAPI::BroodwarPtr == nullptr) { break; }
    }
    std::cout << "Disconnected" << '\n';
}

void App::loop() {
    do {
        for (BWAPI::Event const &e : BWAPI::Broodwar->getEvents()) {
            switch (e.getType()) {
            case BWAPI::EventType::MatchStart:   bot_->onStart(); break;
            case BWAPI::EventType::MatchEnd:     bot_->onEnd(e.isWinner()); break;
            case BWAPI::EventType::MatchFrame:   bot_->onFrame(); break;
            case BWAPI::EventType::SendText:     bot_->onSendText(e.getText()); break;
            case BWAPI::EventType::ReceiveText:  bot_->onReceiveText(e.getPlayer(), e.getText()); break;
            case BWAPI::EventType::PlayerLeft:   bot_->onPlayerLeft(e.getPlayer()); break;
            case BWAPI::EventType::NukeDetect:   bot_->onNukeDetect(e.getPosition()); break;
            case BWAPI::EventType::UnitDiscover: bot_->onUnitDiscover(e.getUnit()); break;
            case BWAPI::EventType::UnitEvade:    bot_->onUnitEvade(e.getUnit()); break;
            case BWAPI::EventType::UnitShow:     bot_->onUnitShow(e.getUnit()); break;
            case BWAPI::EventType::UnitHide:     bot_->onUnitHide(e.getUnit()); break;
            case BWAPI::EventType::UnitCreate:   bot_->onUnitCreate(e.getUnit()); break;
            case BWAPI::EventType::UnitDestroy:  bot_->onUnitDestroy(e.getUnit()); break;
            case BWAPI::EventType::UnitMorph:    bot_->onUnitMorph(e.getUnit()); break;
            case BWAPI::EventType::UnitRenegade: bot_->onUnitRenegade(e.getUnit()); break;
            case BWAPI::EventType::UnitComplete: bot_->onUnitComplete(e.getUnit()); break;
            case BWAPI::EventType::SaveGame:     bot_->onSaveGame(e.getText()); break;
            }
        }
        BWAPI::BWAPIClient.update();
    } while (BWAPI::BWAPIClient.isConnected() && BWAPI::Broodwar->isInGame());
    std::cout << "End Of Game" << '\n';
}
