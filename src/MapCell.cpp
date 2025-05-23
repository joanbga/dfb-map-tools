#include "MapCell.hpp"

#include <iomanip>
#include <unordered_set>

void MapCell::print(int cellIndex) const {
    std::cout << "Cellule " << cellIndex << ":\n";
    std::cout << "  Numero: " << cellNumber << "\n";
    std::cout << "  Vitesse: " << speed << "\n";
    std::cout << "  MapChangeData: " << mapChangeData << "\n";
    std::cout << "  MoveZone: " << moveZone << "\n";
    std::cout << "  LinkedZone: " << linkedZone << "\n";
    std::cout << "  Mov: " << mov << " (" << getWalkableStatus() << ")\n";
    std::cout << "  LOS: " << los << "\n";
    std::cout << "  NonWalkableDuringFight: " << nonWalkableDuringFight << "\n";
    std::cout << "  NonWalkableDuringRP: " << nonWalkableDuringRP << "\n";
    std::cout << "  FarmCell: " << farmCell << (isFarm() ? " (ferme)" : "") << "\n";
    std::cout << "  Visible: " << visible << " (" << (isVisible() ? "visible" : "invisible")
              << ")\n";
    std::cout << "  HavenbagCell: " << havenbagCell << "\n";
    std::cout << "  Floor: " << floor << "\n";
    std::cout << "  Red: " << red << "\n";
    std::cout << "  Blue: " << blue << "\n";
    std::cout << "  Arrow: " << arrow << "\n\n";
}

bool MapCell::operator==(const MapCell& other) const {
    return cellNumber == other.cellNumber && speed == other.speed &&
           mapChangeData == other.mapChangeData && moveZone == other.moveZone &&
           linkedZone == other.linkedZone && mov == other.mov && los == other.los &&
           nonWalkableDuringFight == other.nonWalkableDuringFight &&
           nonWalkableDuringRP == other.nonWalkableDuringRP && farmCell == other.farmCell &&
           visible == other.visible && havenbagCell == other.havenbagCell && floor == other.floor &&
           red == other.red && blue == other.blue && arrow == other.arrow;
}
