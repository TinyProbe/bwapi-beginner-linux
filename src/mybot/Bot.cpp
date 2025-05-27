#include "Bot.h"

#include "Tools.h"

Bot::Bot() {}

Bot::~Bot() {}

void Bot::onStart() {
    BWAPI::Broodwar->setLocalSpeed(42);
    BWAPI::Broodwar->setFrameSkip(0);
    // BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
    mapTools_.onStart();
}

void Bot::onEnd(bool isWinner) {
    std::cout << "We " << (isWinner ? "won!" : "lost!") << '\n';
}

// Called on each frame of the game
void Bot::onFrame() {
    mapTools_.onFrame();
    sendIdleWorkersToMinerals();
    trainAdditionalWorkers();
    buildAdditionalSupply();
    Tools::DrawUnitHealthBars();
    drawDebugInformation();
}

void Bot::onSendText(std::string text) {
    if (text == "/map")
        mapTools_.toggleDraw();
}

void Bot::onReceiveText(BWAPI::Player player, std::string text) {}

void Bot::onPlayerLeft(BWAPI::Player player) {}

void Bot::onNukeDetect(BWAPI::Position target) {}

void Bot::onUnitDiscover(BWAPI::Unit unit) {}

void Bot::onUnitEvade(BWAPI::Unit unit) {}

void Bot::onUnitShow(BWAPI::Unit unit) {}

void Bot::onUnitHide(BWAPI::Unit unit) {}

void Bot::onUnitCreate(BWAPI::Unit unit) {}

void Bot::onUnitDestroy(BWAPI::Unit unit) {}

void Bot::onUnitMorph(BWAPI::Unit unit) {}

void Bot::onUnitRenegade(BWAPI::Unit unit) {}

void Bot::onUnitComplete(BWAPI::Unit unit) {}

void Bot::onSaveGame(std::string gameName) {}



void Bot::sendIdleWorkersToMinerals() {
    BWAPI::Unitset const &myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto &unit : myUnits) {
        if (unit->getType().isWorker() && unit->isIdle()) {
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());
            if (closestMineral) { unit->rightClick(closestMineral); }
        }
    }
}

void Bot::trainAdditionalWorkers() {
    BWAPI::UnitType const workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    int const workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    int const workersWanted = 20;
    if (workersOwned < workersWanted) {
        BWAPI::Unit const myDepot = Tools::GetDepot();
        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }
    }
}

void Bot::buildAdditionalSupply() {
    int const unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();
    int const margin = Tools::GetTotalSupply(true) * 0.2;

    if (unusedSupply >= margin) { return; }

    BWAPI::UnitType const supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();
    bool const startedBuilding = Tools::BuildBuilding(supplyProviderType);

    if (startedBuilding) {
        BWAPI::Broodwar->printf("Started Building %s", supplyProviderType.getName().c_str());
    }
}

void Bot::drawDebugInformation() {
    BWAPI::UnitType const workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    int const workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    std::string worker_debug = "Worker: " + std::to_string(workersOwned);
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), worker_debug.c_str());
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}
