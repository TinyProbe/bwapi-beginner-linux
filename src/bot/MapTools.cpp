#include "MapTools.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <array>

// constructor for MapTools
MapTools::MapTools() {}

std::string const &MapTools::mapName() const {
    return m_mapName;
}

void MapTools::onStart() {
    m_mapName        = fixMapName(BWAPI::Broodwar->mapName());
    m_width          = BWAPI::Broodwar->mapWidth();
    m_height         = BWAPI::Broodwar->mapHeight();
    m_walkable       = Grid<int>(m_width, m_height, 1);
    m_buildable      = Grid<int>(m_width, m_height, 0);
    m_depotBuildable = Grid<int>(m_width, m_height, 0);
    m_lastSeen       = Grid<int>(m_width, m_height, 0);
    m_tileType       = Grid<char>(m_width, m_height, 0);

    // Set the boolean grid data from the Map
    for (int x = 0; x < m_width; ++x) {
        for (int y = 0; y < m_height; ++y) {
            m_buildable.set(x, y, canBuild(x, y));
            m_depotBuildable.set(x, y, canBuild(x, y));
            m_walkable.set(x, y, m_buildable.get(x,y) || canWalk(x, y));
        }
    }

    // set tiles that static resources are on as unbuildable
    for (auto &resource : BWAPI::Broodwar->getStaticNeutralUnits()) {
        if (!resource->getType().isResourceContainer()) {
            continue;
        }

        int const tileX = resource->getTilePosition().x;
        int const tileY = resource->getTilePosition().y;

        for (int x = tileX; x < tileX + resource->getType().tileWidth(); ++x) {
            for (int y = tileY; y < tileY + resource->getType().tileHeight(); ++y) {
                if (resource->getType().isMineralField()) {
                    m_tileType.set(x, y, 'M');
                } else {
                    m_tileType.set(x, y, 'G');
                }
                m_buildable.set(x, y, false);

                // depots can't be built within 3 tiles of any resource
                for (int rx = -3; rx <= 3; rx++) {
                    for (int ry = -3; ry <= 3; ry++) {
                        if (!BWAPI::TilePosition(x + rx, y + ry).isValid()) {
                            continue;
                        }
                        m_depotBuildable.set(x + rx, y + ry, 0);
                    }
                }
            }
        }
    }

    // set the other tile types
    for (int x = 0; x < m_width; ++x) {
        for (int y = 0; y < m_height; ++y) {
            // if it has a type already it's a mineral or a gas
            if (m_tileType.get(x, y) != 0) {
                continue; }
            if (isBuildable(x, y) && !isDepotBuildableTile(x, y)) {
                m_tileType.set(x, y, 'D'); continue; }
            if (isBuildable(x,y)) {
                m_tileType.set(x, y, 'B'); continue; }
            if (isWalkable(x,y)) {
                m_tileType.set(x, y, 'W'); continue; }
            if (!isWalkable(x,y)) {
                m_tileType.set(x, y, 'U'); continue; }
        }
    }
}

void MapTools::onFrame() {
    for (int x = 0; x < m_width; ++x) {
        for (int y = 0; y < m_height; ++y) {
            if (isVisible(x, y)) {
                m_lastSeen.set(x, y, BWAPI::Broodwar->getFrameCount());
            }
        }
    }
    if (m_drawMap) { draw(); }
}

void MapTools::toggleDraw() {
    m_drawMap = !m_drawMap;
}

bool MapTools::isExplored(BWAPI::TilePosition const &pos) const {
    return isExplored(pos.x, pos.y);
}

bool MapTools::isExplored(BWAPI::Position const &pos) const {
    return isExplored(BWAPI::TilePosition(pos));
}

bool MapTools::isExplored(int tileX, int tileY) const {
    if (!isValidTile(tileX, tileY)) { return false; }
    return BWAPI::Broodwar->isExplored(tileX, tileY);
}

bool MapTools::isVisible(int tileX, int tileY) const {
    if (!isValidTile(tileX, tileY)) { return false; }
    return BWAPI::Broodwar->isVisible(BWAPI::TilePosition(tileX, tileY));
}

bool MapTools::isPowered(int tileX, int tileY) const {
    return BWAPI::Broodwar->hasPower(BWAPI::TilePosition(tileX, tileY));
}

bool MapTools::isValidTile(int tileX, int tileY) const {
    return tileX >= 0 && tileY >= 0 && tileX < m_width && tileY < m_height;
}

bool MapTools::isValidTile(BWAPI::TilePosition const &tile) const {
    return isValidTile(tile.x, tile.y);
}

bool MapTools::isValidPosition(BWAPI::Position const &pos) const {
    return isValidTile(BWAPI::TilePosition(pos));
}

bool MapTools::isBuildable(int tileX, int tileY) const {
    if (!isValidTile(tileX, tileY)) {
        return false;
    }
    return m_buildable.get(tileX, tileY);
}

bool MapTools::isBuildable(BWAPI::TilePosition const &tile) const {
    return isBuildable(tile.x, tile.y);
}

// saves the map to a file in the StarDraft map format
// https://github.com/davechurchill/stardraft/wiki/Map-File-Syntax
void MapTools::saveMapToFile(std::string const &str) const {
    std::string path = str;

    if (path.length() == 0) {
        path = "replaydata/maps/" + m_mapName + ".txt";
    }

    std::ofstream fout(path);

    // print the width and height of the map
    fout << width() << " " << height() << "\n";

    // print all the possible starting locations
    fout << BWAPI::Broodwar->getStartLocations().size();
    for (auto tile : BWAPI::Broodwar->getStartLocations()) {
        fout << " " << tile.x << " " << tile.y;
    }
    fout << "\n";

    // print the character of the type of tile this is
    for (size_t y = 0; y < height(); y++) {
        for (size_t x = 0; x < width(); x++) {
            fout << m_tileType.get(x, y);
        }
        fout << "\n";
    }

    // print out the walk tiles
    for (size_t y = 0; y < height() * 4; y++) {
        for (size_t x = 0; x < width() * 4; x++) {
            fout << BWAPI::Broodwar->isWalkable((int)x, (int)y) ? '0' : '1';
        }
        fout << "\n";
    }
}

void MapTools::printMap() const {
    std::stringstream ss;
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            ss << isWalkable(x, y);
        }
        ss << "\n";
    }

    std::ofstream out("map.txt");
    out << ss.str();
    out.close();
}

bool MapTools::isDepotBuildableTile(int tileX, int tileY) const {
    if (!isValidTile(tileX, tileY)) {
        return false;
    }
    return m_depotBuildable.get(tileX, tileY);
}

bool MapTools::isWalkable(int tileX, int tileY) const {
    if (!isValidTile(tileX, tileY)) {
        return false;
    }
    return m_walkable.get(tileX, tileY);
}

bool MapTools::isWalkable(BWAPI::TilePosition const &tile) const {
    return isWalkable(tile.x, tile.y);
}

int MapTools::width() const {
    return m_width;
}

int MapTools::height() const {
    return m_height;
}

void MapTools::drawTile(int tileX, int tileY, BWAPI::Color const &color) const {
    int const padding = 2;
    int const px      = tileX * 32 + padding;
    int const py      = tileY * 32 + padding;
    int const d       = 32 - 2 * padding;

    BWAPI::Broodwar->drawLineMap(px,     py,     px + d, py,     color);
    BWAPI::Broodwar->drawLineMap(px + d, py,     px + d, py + d, color);
    BWAPI::Broodwar->drawLineMap(px + d, py + d, px,     py + d, color);
    BWAPI::Broodwar->drawLineMap(px,     py + d, px,     py,     color);
}

bool MapTools::canWalk(int tileX, int tileY) const {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (!BWAPI::Broodwar->isWalkable(tileX * 4 + i, tileY * 4 + j)) {
                return false;
            }
        }
    }
    return true;
}

bool MapTools::canBuild(int tileX, int tileY) const {
    return BWAPI::Broodwar->isBuildable(BWAPI::TilePosition(tileX, tileY));
}

void MapTools::draw() const {
    const BWAPI::TilePosition screen(BWAPI::Broodwar->getScreenPosition());
    const int sx = screen.x;
    const int sy = screen.y;
    const int ex = sx + 20;
    const int ey = sy + 15;

    for (int x = sx; x < ex; ++x) {
        for (int y = sy; y < ey; y++) {
            const BWAPI::TilePosition tilePos(x, y);

            if (!tilePos.isValid()) { continue; }

            if (true) {
                BWAPI::Color color = isWalkable(x, y) ? BWAPI::Color(0, 255, 0) : BWAPI::Color(255, 0, 0);
                if (isWalkable(x, y) && !isBuildable(x, y)) { color = BWAPI::Color(255, 255, 0); }
                if (isBuildable(x, y) && !isDepotBuildableTile(x, y)) { color = BWAPI::Color(127, 255, 255); }
                drawTile(x, y, color);
            }
        }
    }

    const char red = '\x08';
    const char green = '\x07';
    const char white = '\x04';
    const char yellow = '\x03';

    BWAPI::Broodwar->drawBoxScreen(0, 0, 200, 100, BWAPI::Colors::Black, true);
    BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Huge);
    BWAPI::Broodwar->drawTextScreen(10, 5, "%cMap Legend", white);
    BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Default);
    BWAPI::Broodwar->drawTextScreen(10, 30, "%cRed: ", red);
    BWAPI::Broodwar->drawTextScreen(60, 30, "%cCan't Walk or Build", white);
    BWAPI::Broodwar->drawTextScreen(10, 45, "%cGreen:", green);
    BWAPI::Broodwar->drawTextScreen(60, 45, "%cCan Walk and Build", white);
    BWAPI::Broodwar->drawTextScreen(10, 60, "%cYellow:", yellow);
    BWAPI::Broodwar->drawTextScreen(60, 60, "%cResource Tile, Can't Build", white);
    BWAPI::Broodwar->drawTextScreen(10, 75, "Teal:");
    BWAPI::Broodwar->drawTextScreen(60, 75, "%cCan't Build Depot", white);
}

std::string MapTools::fixMapName(std::string const &s) const {
    std::stringstream ss;

    for (auto c : s) {
        if (std::isalnum(c) || c == '.') {
            ss << c;
        }
    }
    return ss.str();
}
