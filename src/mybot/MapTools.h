#pragma once
#ifndef _MAPTOOLS_H_
#define _MAPTOOLS_H_

#include "Grid.hpp"

#include <BWAPI.h>

#include <vector>

class MapTools {
    std::string m_mapName;
    Grid<int>   m_walkable;       // whether a tile is buildable (includes static resources)          
    Grid<int>   m_buildable;      // whether a tile is buildable (includes static resources)
    Grid<int>   m_depotBuildable; // whether a depot is buildable on a tile (illegal within 3 tiles of static resource)
    Grid<int>   m_lastSeen;       // the last time any of our units has seen this position on the map
    Grid<char>  m_tileType;       // StarDraft tile type
    int  m_width   = 0;
    int  m_height  = 0;
    int  m_frame   = 0;
    bool m_drawMap = false;

    bool canBuild(int tileX, int tileY) const;
    bool canWalk(int tileX, int tileY) const;
    void printMap() const;
    std::string fixMapName(std::string const &s) const;

public:
    MapTools();

    void onStart();
    void onFrame();
    void draw() const;
    void toggleDraw();
    void saveMapToFile(std::string const &str = "") const;
    std::string const &mapName() const;

    int  width() const;
    int  height() const;

    bool isValidTile(int tileX, int tileY) const;
    bool isValidTile(BWAPI::TilePosition const &tile) const;
    bool isValidPosition(BWAPI::Position const &pos) const;
    bool isPowered(int tileX, int tileY) const;
    bool isExplored(int tileX, int tileY) const;
    bool isExplored(BWAPI::Position const &pos) const;
    bool isExplored(BWAPI::TilePosition const &pos) const;
    bool isVisible(int tileX, int tileY) const;
    bool isWalkable(int tileX, int tileY) const;
    bool isWalkable(BWAPI::TilePosition const &tile) const;
    bool isBuildable(int tileX, int tileY) const;
    bool isBuildable(BWAPI::TilePosition const &tile) const;
    bool isDepotBuildableTile(int tileX, int tileY) const;
    void drawTile(int tileX, int tileY, BWAPI::Color const &color) const;
};

#endif
