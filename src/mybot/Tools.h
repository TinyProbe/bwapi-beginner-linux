#pragma once
#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <BWAPI.h>

namespace Tools {
    BWAPI::Unit GetClosestUnitTo(BWAPI::Position p, BWAPI::Unitset const &units);
    BWAPI::Unit GetClosestUnitTo(BWAPI::Unit unit, BWAPI::Unitset const &units);

    int CountUnitsOfType(BWAPI::UnitType type, BWAPI::Unitset const &units);

    BWAPI::Unit GetUnitOfType(BWAPI::UnitType type);
    BWAPI::Unit GetDepot();

    bool BuildBuilding(BWAPI::UnitType type);

    void DrawUnitBoundingBoxes();
    void DrawUnitCommands();

    void SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target);

    int GetTotalSupply(bool inProgress = false);

    void DrawUnitHealthBars();
    void DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset);
}

#endif
