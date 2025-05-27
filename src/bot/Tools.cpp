#include "Tools.h"

namespace Tools {

BWAPI::Unit GetClosestUnitTo(BWAPI::Position p, BWAPI::Unitset const &units) {
    BWAPI::Unit closestUnit = nullptr;

    for (auto &u : units) {
        if (!closestUnit || u->getDistance(p) < closestUnit->getDistance(p)) {
            closestUnit = u;
        }
    }
    return closestUnit;
}

BWAPI::Unit GetClosestUnitTo(BWAPI::Unit unit, BWAPI::Unitset const &units) {
    if (!unit) { return nullptr; }
    return GetClosestUnitTo(unit->getPosition(), units);
}

int CountUnitsOfType(BWAPI::UnitType type, BWAPI::Unitset const &units) {
    int sum = 0;

    for (auto &unit : units) {
        if (unit->getType() == type) {
            sum++;
        }
    }
    return sum;
}

BWAPI::Unit GetUnitOfType(BWAPI::UnitType type) {
    for (auto &unit : BWAPI::Broodwar->self()->getUnits()) {
        if (unit->getType() == type && unit->isCompleted()) {
            return unit;
        }
    }
    return nullptr;
}

BWAPI::Unit GetDepot() {
    BWAPI::UnitType const depot = BWAPI::Broodwar->self()->getRace().getResourceDepot();
    return GetUnitOfType(depot);
}

bool BuildBuilding(BWAPI::UnitType type) {
    BWAPI::UnitType builderType = type.whatBuilds().first;

    BWAPI::Unit builder = Tools::GetUnitOfType(builderType);
    if (!builder) { return false; }

    BWAPI::TilePosition desiredPos = BWAPI::Broodwar->self()->getStartLocation();

    int maxBuildRange = 64;
    bool buildingOnCreep = type.requiresCreep();
    BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(type, desiredPos, maxBuildRange, buildingOnCreep);
    return builder->build(type, buildPos);
}

void DrawUnitCommands() {
    for (auto &unit : BWAPI::Broodwar->self()->getUnits()) {
        BWAPI::UnitCommand const &command = unit->getLastCommand();

        if (command.getTargetPosition() != BWAPI::Positions::None) {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), command.getTargetPosition(), BWAPI::Colors::Red);
        }

        if (command.getTargetTilePosition() != BWAPI::TilePositions::None) {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), BWAPI::Position(command.getTargetTilePosition()), BWAPI::Colors::Green);
        }

        if (command.getTarget() != nullptr) {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), command.getTarget()->getPosition(), BWAPI::Colors::White);
        }
    }
}

void DrawUnitBoundingBoxes() {
    for (auto &unit : BWAPI::Broodwar->getAllUnits()) {
        BWAPI::Position topLeft(unit->getLeft(), unit->getTop());
        BWAPI::Position bottomRight(unit->getRight(), unit->getBottom());
        BWAPI::Broodwar->drawBoxMap(topLeft, bottomRight, BWAPI::Colors::White);
    }
}

void SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target) {
    if (!unit || !target) { return; }

    if (unit->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount()) { return; }

    if (unit->getLastCommand().getTarget() == target) { return; }

    unit->rightClick(target);
}

int GetTotalSupply(bool inProgress) {
    int totalSupply = BWAPI::Broodwar->self()->supplyTotal();

    if (!inProgress) { return totalSupply; }

    for (auto &unit : BWAPI::Broodwar->self()->getUnits()) {
        if (unit->isCompleted()) { continue; }

        totalSupply += unit->getType().supplyProvided();
    }

    for (auto &unit : BWAPI::Broodwar->self()->getUnits()) {
        BWAPI::UnitCommand const &command = unit->getLastCommand();

        if (command.getType() != BWAPI::UnitCommandTypes::Build) { continue; }

        totalSupply += command.getUnitType().supplyProvided();
    }
    return totalSupply;
}

void DrawUnitHealthBars() {
    int verticalOffset = -10;

    for (auto &unit : BWAPI::Broodwar->getAllUnits()) {
        BWAPI::Position const &pos = unit->getPosition();
        int left = pos.x - unit->getType().dimensionLeft();
        int right = pos.x + unit->getType().dimensionRight();
        int top = pos.y - unit->getType().dimensionUp();
        int bottom = pos.y + unit->getType().dimensionDown();

        if (unit->getType().isResourceContainer() && unit->getInitialResources() > 0) {
            double mineralRatio = (double)unit->getResources() / (double)unit->getInitialResources();
            DrawHealthBar(unit, mineralRatio, BWAPI::Colors::Cyan, 0);
        } else if (unit->getType().maxHitPoints() > 0) {
            double hpRatio = (double)unit->getHitPoints() / (double)unit->getType().maxHitPoints();
            BWAPI::Color hpColor = BWAPI::Colors::Green;
            if (hpRatio < 0.66) hpColor = BWAPI::Colors::Orange;
            if (hpRatio < 0.33) hpColor = BWAPI::Colors::Red;
            DrawHealthBar(unit, hpRatio, hpColor, 0);

            if (unit->getType().maxShields() > 0) {
                double shieldRatio = (double)unit->getShields() / (double)unit->getType().maxShields();
                DrawHealthBar(unit, shieldRatio, BWAPI::Colors::Blue, -3);
            }
        }
    }
}

void DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset) {
    int verticalOffset = -10;
    BWAPI::Position const &pos = unit->getPosition();

    int left = pos.x - unit->getType().dimensionLeft();
    int right = pos.x + unit->getType().dimensionRight();
    int top = pos.y - unit->getType().dimensionUp();
    int bottom = pos.y + unit->getType().dimensionDown();

    int ratioRight = left + (int)((right - left) * ratio);
    int hpTop = top + yOffset + verticalOffset;
    int hpBottom = top + 4 + yOffset + verticalOffset;

    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), color, true);
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

    int ticWidth = 3;

    for (int i = left; i < right - 1; i += ticWidth) {
        BWAPI::Broodwar->drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
    }
}

}
