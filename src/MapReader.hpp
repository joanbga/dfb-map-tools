#include <vector>

#include "Map.hpp"
#include "MapCell.hpp"

/**
 * Classe pour gérer la lecture des cartes
 */
class MapReader
{
public:
    static constexpr size_t CELLS_PER_MAP = 560; // 20x28

    /**
     * Lit une carte depuis un fichier binaire
     * @param filename: nom du fichier binaire
     * @return: vecteur de MapCell, vide en cas d'erreur
     */
    static std::vector<MapCell> readMapFromBinary(const std::string &filename);

    /**
     * Lit une carte depuis un fichier binaire (version avec tableau C-style)
     * @param filename: nom du fichier binaire
     * @param cells: tableau de MapCell à remplir
     * @param maxCells: taille maximale du tableau
     * @return: nombre de cellules lues, -1 en cas d'erreur
     */
    static int readMapFromBinary(const std::string &filename, MapCell *cells, size_t maxCells);

    /**
     * Lit une carte et retourne un objet Map
     */
    static Map readMapObjectFromBinary(const std::string &filename, int width = Map::DEFAULT_WIDTH);
};