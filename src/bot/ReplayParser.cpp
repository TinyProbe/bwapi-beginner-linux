#include "ReplayParser.h"
#include "Tools.h"

#include <sstream>

ReplayParser::ReplayParser() {}

ReplayParser::~ReplayParser() {}

void ReplayParser::onStart() {
    map_.onStart();
    map_.onFrame();
    map_.saveMapToFile();

    BWAPI::Broodwar->setLocalSpeed(21);
    BWAPI::Broodwar->setFrameSkip(0);

    fout_ = std::ofstream("replaydata/" + BWAPI::Broodwar->mapFileName() + ".txt");
    fout_ << "map maps/" << (map_.mapName() + ".txt") << '\n';
}

void ReplayParser::onEnd(bool isWinner) {}

void ReplayParser::onFrame() {
    map_.onFrame();

    drawUnitCommands();
    logUnitCommands();
    Tools::DrawUnitHealthBars();
}

void ReplayParser::onSendText(std::string text) {}

void ReplayParser::onReceiveText(BWAPI::Player player, std::string text) {}

void ReplayParser::onPlayerLeft(BWAPI::Player player) {}

void ReplayParser::onNukeDetect(BWAPI::Position target) {}

void ReplayParser::onUnitDiscover(BWAPI::Unit unit) {}

void ReplayParser::onUnitEvade(BWAPI::Unit unit) {}

void ReplayParser::onUnitShow(BWAPI::Unit unit) {}

void ReplayParser::onUnitHide(BWAPI::Unit unit) {}

void ReplayParser::onUnitCreate(BWAPI::Unit unit) {
    BWAPI::UnitType const type = unit->getType();
    if (type.isBuilding() && !unit->getPlayer()->isNeutral()) {
        fout_ << BWAPI::Broodwar->getFrameCount() << " create " << getUnitString(unit) << '\n';
    }
}

void ReplayParser::onUnitDestroy(BWAPI::Unit unit) {
    BWAPI::UnitType const type = unit->getType();
    if (type.isBuilding() && !unit->getPlayer()->isNeutral()) {
        fout_ << BWAPI::Broodwar->getFrameCount() << " destroy " << getUnitString(unit) << '\n';
    }
}

void ReplayParser::onUnitMorph(BWAPI::Unit unit) {
    BWAPI::UnitType const type = unit->getType();
    if (type.isBuilding() && !unit->getPlayer()->isNeutral()) {
        fout_ << BWAPI::Broodwar->getFrameCount() << " create " << getUnitString(unit) << '\n';
    }
}

void ReplayParser::onUnitRenegade(BWAPI::Unit unit) {}

void ReplayParser::onUnitComplete(BWAPI::Unit unit) {}

void ReplayParser::onSaveGame(std::string gameName) {}



std::string ReplayParser::getUnitString(BWAPI::Unit unit) {
    BWAPI::UnitType const type = unit->getType();
    BWAPI::Position tl(unit->getPosition().x - type.dimensionLeft(), unit->getPosition().y - type.dimensionUp());
    BWAPI::Position br(unit->getPosition().x + type.dimensionRight(), unit->getPosition().y + type.dimensionDown());

    std::stringstream ss;
    ss << unit->getPlayer()->getID() << ' ' << unit->getID() << ' ' << type.getName();
    ss << ' ' << tl.x << ' ' << tl.y << ' ' << br.x << ' ' << br.y;
    return ss.str();
}

void ReplayParser::drawUnitCommands() {
    for (auto unit : BWAPI::Broodwar->getAllUnits()) {
        if (!unit->getPlayer()->isNeutral()) {
            BWAPI::Position p1 = unit->getPosition();
            BWAPI::Position p2 = unit->getOrderTargetPosition();

            if (p2 != BWAPI::Position(0, 0)) {
                BWAPI::Broodwar->drawLineMap(p1, p2, BWAPI::Colors::White);
            }
            BWAPI::Broodwar->drawTextMap(p1, unit->getOrder().getName().c_str());
        }
    }
}

void ReplayParser::logUnitCommands() {
    for (auto unit : BWAPI::Broodwar->getAllUnits()) {
        if (unit->getPlayer()->isNeutral()) { continue; }
        if (unit->isFlying()) { continue; }
        if (unit->getType().isBuilding()) { continue; }

        auto order = unit->getOrder().getID();
        if (order != BWAPI::Orders::Move && order != BWAPI::Orders::AttackMove) {
            continue;
        }

        bool newCommand = false;
        BWAPI::Position upos = unit->getPosition();
        BWAPI::Position tpos = unit->getOrderTargetPosition();
        auto it = unitCommands_.find(unit->getID());

        if (it == unitCommands_.end()) {
            newCommand = true;
            unitCommands_[unit->getID()] = tpos;
        } else {
            if (it->second != tpos) {
                newCommand = true;
                unitCommands_[unit->getID()] = tpos;
            }
        }

        if (newCommand) {
            fout_ << BWAPI::Broodwar->getFrameCount() << " query ";
            fout_ << unit->getPlayer()->getID() << ' ' << unit->getID() << ' ' << unit->getType().getName() << ' ';
            fout_ << unit->getOrder().getName() << ' ';
            fout_ << upos.x << ' ' << upos.y << ' ' << tpos.x << ' ' << tpos.y << '\n';
        }
    }
}
