#include "MyBot.h"
#include "Tools.h"
#include "MapTools.h"

MyBot::MyBot() {}

// Called when the bot starts!
void MyBot::onStart() {
    // Set our BWAPI options here
    BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(0);

    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();
}

// Called on each frame of the game
void MyBot::onFrame() {
    // Update our MapTools information
    m_mapTools.onFrame();

    // Send our idle workers to mine minerals so they don't just stand there
    sendIdleWorkersToMinerals();

    // Train more workers so we can gather more income
    trainAdditionalWorkers();

    // Build more supply if we are going to run out soon
    buildAdditionalSupply();

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
}

// Send our idle workers to mine minerals so they don't just stand there
void MyBot::sendIdleWorkersToMinerals() {
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    BWAPI::Unitset const &myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto &unit : myUnits) {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle()) {
            // Get the closest mineral to this worker unit
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
            if (closestMineral) { unit->rightClick(closestMineral); }
        }
    }
}

// Train more workers so we can gather more income
void MyBot::trainAdditionalWorkers() {
    BWAPI::UnitType const workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    int const workersWanted = 20;
    int const workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    if (workersOwned < workersWanted) {
        // get the unit pointer to my depot
        BWAPI::Unit const myDepot = Tools::GetDepot();

        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }
    }
}

// Build more supply if we are going to run out soon
void MyBot::buildAdditionalSupply() {
    // Get the amount of supply supply we currently have unused
    int const unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply >= 2) { return; }

    // Otherwise, we are going to build a supply provider
    BWAPI::UnitType const supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    bool const startedBuilding = Tools::BuildBuilding(supplyProviderType);
    if (startedBuilding) {
        BWAPI::Broodwar->printf("Started Building %s", supplyProviderType.getName().c_str());
    }
}

// Draw some relevent information to the screen to help us debug the bot
void MyBot::drawDebugInformation() {
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Hello, World!\n");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

// Called whenever the game ends and tells you if you won or not
void MyBot::onEnd(bool isWinner) {
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

// Called whenever a unit is destroyed, with a pointer to the unit
void MyBot::onUnitDestroy(BWAPI::Unit unit) {}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void MyBot::onUnitMorph(BWAPI::Unit unit) {}

// Called whenever a text is sent to the game by a user
void MyBot::onSendText(std::string text) {
    if (text == "/map") {
        m_mapTools.toggleDraw();
    }
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void MyBot::onUnitCreate(BWAPI::Unit unit) {}

// Called whenever a unit finished construction, with a pointer to the unit
void MyBot::onUnitComplete(BWAPI::Unit unit) {}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void MyBot::onUnitShow(BWAPI::Unit unit) {}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void MyBot::onUnitHide(BWAPI::Unit unit) {}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void MyBot::onUnitRenegade(BWAPI::Unit unit) {}
