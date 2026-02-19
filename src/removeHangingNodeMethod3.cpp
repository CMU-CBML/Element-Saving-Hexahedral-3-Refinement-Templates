#include "geometry.h"

Hexahedra Tree::RemoveHangingNodesMethod3() {
    std::vector<std::array<size_t, 8>> hE;
    std::vector<std::array<double, 3>> hX;

    // first three values are coordinate, the fourth value (0-7) is the octants that is not affected by the current cell (8 means all octants are affected)
        // the rest values are the octants that are occupied by the current cell
    const std::array<std::vector<size_t>, 56> cubeFaces = { {
        { 0, 0, 0, 0, 6 },
        { 3, 0, 0, 1, 7 },
        { 3, 3, 0, 2, 4 },
        { 0, 3, 0, 3, 5 },
        { 0, 0, 3, 4, 2 },
        { 3, 0, 3, 5, 3 },
        { 3, 3, 3, 6, 0 },
        { 0, 3, 3, 7, 1 },

        { 1, 1, 0, 8, 4, 5, 6, 7 },
        { 2, 1, 0, 8, 4, 5, 6, 7 },
        { 2, 2, 0, 8, 4, 5, 6, 7 },
        { 1, 2, 0, 8, 4, 5, 6, 7 },
        { 1, 1, 3, 8, 0, 1, 2, 3 },
        { 2, 1, 3, 8, 0, 1, 2, 3 },
        { 2, 2, 3, 8, 0, 1, 2, 3 },
        { 1, 2, 3, 8, 0, 1, 2, 3 },
        { 1, 0, 1, 8, 2, 3, 6, 7 },
        { 2, 0, 1, 8, 2, 3, 6, 7 },
        { 2, 0, 2, 8, 2, 3, 6, 7 },
        { 1, 0, 2, 8, 2, 3, 6, 7 },
        { 1, 3, 1, 8, 0, 1, 4, 5 },
        { 2, 3, 1, 8, 0, 1, 4, 5 },
        { 2, 3, 2, 8, 0, 1, 4, 5 },
        { 1, 3, 2, 8, 0, 1, 4, 5 },
        { 0, 1, 1, 8, 1, 2, 5, 6 },
        { 0, 2, 1, 8, 1, 2, 5, 6 },
        { 0, 2, 2, 8, 1, 2, 5, 6 },
        { 0, 1, 2, 8, 1, 2, 5, 6 },
        { 3, 1, 1, 8, 0, 3, 4, 7 },
        { 3, 2, 1, 8, 0, 3, 4, 7 },
        { 3, 2, 2, 8, 0, 3, 4, 7 },
        { 3, 1, 2, 8, 0, 3, 4, 7 },

        { 1, 0, 0, 8, 6, 7 },
        { 2, 0, 0, 8, 6, 7 },
        { 3, 1, 0, 8, 4, 7 },
        { 3, 2, 0, 8, 4, 7 },
        { 2, 3, 0, 8, 4, 5 },
        { 1, 3, 0, 8, 4, 5 },
        { 0, 2, 0, 8, 5, 6 },
        { 0, 1, 0, 8, 5, 6 },
        { 0, 0, 1, 8, 2, 6 },
        { 0, 0, 2, 8, 2, 6 },
        { 3, 0, 1, 8, 3, 7 },
        { 3, 0, 2, 8, 3, 7 },
        { 3, 3, 1, 8, 0, 4 },
        { 3, 3, 2, 8, 0, 4 },
        { 0, 3, 1, 8, 1, 5 },
        { 0, 3, 2, 8, 1, 5 },
        { 1, 0, 3, 8, 2, 3 },
        { 2, 0, 3, 8, 2, 3 },
        { 3, 1, 3, 8, 0, 3 },
        { 3, 2, 3, 8, 0, 3 },
        { 2, 3, 3, 8, 0, 1 },
        { 1, 3, 3, 8, 0, 1 },
        { 0, 2, 3, 8, 1, 2 },
        { 0, 1, 3, 8, 1, 2 } } };

    // give a point, return the minimum side length according to the octant the cell's relative position to the point
    std::unordered_map<std::string, std::array<size_t, 8>> posOctToMinSide;
    for (const auto& cLayer : c) {
        for (const auto& cLayerCell : cLayer) {
            if (cLayerCell.c[0] != cLayerCell.c[1]) { // which means this cell has children
                continue;
            }

            const size_t edgeLength = cLayerCell.p[0][3] - cLayerCell.p[0][0];
            for (size_t cubeCornerIdx = 0; cubeCornerIdx < 8; ++cubeCornerIdx) {
                const auto& cubeCorner = cubeFaces[cubeCornerIdx];
                const std::string hash = HASHIT(cLayerCell.p[0][cubeCorner[0]], cLayerCell.p[1][cubeCorner[1]], cLayerCell.p[2][cubeCorner[2]]);
                for (size_t oct = 0; oct < 8; ++oct) {
                    if (oct != cubeCorner[3]) {
                        if (!posOctToMinSide.count(hash) || posOctToMinSide.at(hash)[oct] == 0) {
                            posOctToMinSide[hash][oct] = edgeLength;
                        }
                        else {
                            posOctToMinSide[hash][oct] = std::min(posOctToMinSide.at(hash)[oct], edgeLength);
                        }
                    }
                }
            }
        }
    }

    size_t cellIncrease = 0;
    bool cellIncreased = true;

    while (cellIncreased) {
        cellIncreased = false;
        for (size_t reversedLevel = 2; reversedLevel <= c.size(); ++reversedLevel) {
            const size_t level = c.size() - reversedLevel;
            std::cout << "refining at tree depth: " << level + 1 << ", cell increase: " << cellIncrease << std::endl;
            if (cellIncrease > 1) {
                cellIncreased = true;
            }
            cellIncrease = 0;
            const auto& cLayer = c[level];

            for (size_t id = 0; id < cLayer.size(); ++id) {
                const auto& cLayerCell = cLayer[id];
                if (cLayerCell.c[0] != cLayerCell.c[1]) { // skip if the current cell has children
                    continue;
                }

                bool refine = false;
                for (const auto& cubeFace : cubeFaces) {
                    const std::string hash = HASHIT(cLayerCell.p[0][cubeFace[0]], cLayerCell.p[1][cubeFace[1]], cLayerCell.p[2][cubeFace[2]]);
                    for (size_t octIdx = 4; octIdx < cubeFace.size(); ++octIdx) {
                        const size_t oct = cubeFace[octIdx];
                        if (posOctToMinSide.count(hash) && posOctToMinSide.at(hash)[oct] < 0.5 * (cLayerCell.p[0][1] - cLayerCell.p[0][0])) {
                            refine = true;
                            break;
                        }
                    }
                }
                if (!refine) {
                    continue;
                }

                // refine the cell
                RefineCell(level, id);
                cellIncrease += 26;

                // for each child, update posToMinSide
                for (size_t childId = c[level + 1].size() - 27; childId < c[level + 1].size(); ++childId) {
                    const auto& childCell = c[level + 1][childId];

                    const size_t edgeLength = childCell.p[0][3] - childCell.p[0][0];
                    for (size_t cubeCornerIdx = 0; cubeCornerIdx < 8; ++cubeCornerIdx) {
                        const auto& cubeCorner = cubeFaces[cubeCornerIdx];
                        const std::string hash = HASHIT(childCell.p[0][cubeCorner[0]], childCell.p[1][cubeCorner[1]], childCell.p[2][cubeCorner[2]]);
                        for (size_t oct = 0; oct < 8; ++oct) {
                            if (oct != cubeCorner[3]) {
                                if (!posOctToMinSide.count(hash) || posOctToMinSide.at(hash)[oct] == 0) {
                                    posOctToMinSide[hash][oct] = edgeLength;
                                }
                                else {
                                    posOctToMinSide[hash][oct] = std::min(posOctToMinSide.at(hash)[oct], edgeLength);
                                }
                            }
                        }
                    }
                }
            }
        }
        std::cout << "-----" << std::endl;
    }

    for (auto& element : posOctToMinSide) {
        size_t minNonZero = 0;
        for (size_t i = 0; i < 8; ++i) {
            if (element.second[i] > 0.5) {
                if (minNonZero == 0 || element.second[i] < minNonZero) {
                    minNonZero = element.second[i];
                }
            }
        }

        element.second[0] = minNonZero;
    }

    const std::unordered_map<uint8_t, std::pair<uint8_t, std::array<uint8_t, 8>>> rotations = {
        // 00000000
        { 0, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        // 10000000
        { 1, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 2, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 4, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 8, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 16, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 32, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 64, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 128, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        // 01010000
        { 5, { 1, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 10, { 1, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 18, { 1, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 24, { 1, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 33, { 1, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 36, { 1, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 66, { 1, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 72, { 1, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 80, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 129, { 1, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 132, { 1, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 160, { 1, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        // 10000010
        { 20, { 1, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 40, { 1, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 65, { 1, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 130, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        // 00011010
        { 26, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 37, { 1, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 74, { 1, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 82, { 1, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 88, { 1, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 133, { 1, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 161, { 1, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 164, { 1, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        // 01011010
        { 90, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 165, { 1, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        // 10010000
        { 3, { 2, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 6, { 2, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 9, { 2, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 12, { 2, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 17, { 2, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 34, { 2, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 48, { 2, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 68, { 2, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 96, { 2, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 136, { 2, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 144, { 2, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 192, { 2, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        // 10010010
        { 21, { 2, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 22, { 2, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 28, { 2, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 41, { 2, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 42, { 2, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 44, { 2, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 52, { 2, { 3, 0, 1, 2, 7, 4, 5, 6 } } },
        { 56, { 2, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 67, { 2, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 69, { 2, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 73, { 2, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 81, { 2, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 84, { 2, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 97, { 2, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 104, { 2, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 131, { 2, { 6, 5, 4, 7, 2, 1, 0, 3 } } },
        { 134, { 2, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 138, { 2, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 146, { 2, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 148, { 2, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 162, { 2, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 168, { 2, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 193, { 2, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 194, { 2, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        // 11110000
        { 15, { 3, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 51, { 3, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 102, { 3, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 153, { 3, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 204, { 3, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 240, { 3, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        // 10110000
        { 7, { 4, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 11, { 4, { 6, 5, 4, 7, 2, 1, 0, 3 } } },
        { 13, { 4, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 14, { 4, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 19, { 4, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 25, { 4, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 35, { 4, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 38, { 4, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 49, { 4, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 50, { 4, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 70, { 4, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 76, { 4, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 98, { 4, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 100, { 4, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 112, { 4, { 3, 0, 1, 2, 7, 4, 5, 6 } } },
        { 137, { 4, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 140, { 4, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 145, { 4, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 152, { 4, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 176, { 4, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 196, { 4, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 200, { 4, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 208, { 4, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 224, { 4, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        // 10110100
        { 30, { 4, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 45, { 4, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 53, { 4, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 58, { 4, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 75, { 4, { 6, 5, 4, 7, 2, 1, 0, 3 } } },
        { 83, { 4, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 86, { 4, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 89, { 4, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 92, { 4, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 101, { 4, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 106, { 4, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 120, { 4, { 3, 0, 1, 2, 7, 4, 5, 6 } } },
        { 135, { 4, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 149, { 4, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 154, { 4, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 163, { 4, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 166, { 4, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 169, { 4, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 172, { 4, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 180, { 4, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 197, { 4, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 202, { 4, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 210, { 4, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 225, { 4, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        // 10111011
        { 63, { 5, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 111, { 5, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 119, { 5, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 159, { 5, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 187, { 5, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 207, { 5, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 221, { 5, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 238, { 5, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 243, { 5, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 246, { 5, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 249, { 5, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 252, { 5, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        // 10110001
        { 27, { 6, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 39, { 6, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 78, { 6, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 114, { 6, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 141, { 6, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 177, { 6, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 216, { 6, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 228, { 6, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        // 10110101
        { 91, { 6, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 94, { 6, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 122, { 6, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 167, { 6, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 173, { 6, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 181, { 6, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 218, { 6, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 229, { 6, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        // 10110010
        { 23, { 7, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 46, { 7, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 57, { 7, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 77, { 7, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 99, { 7, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 116, { 7, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 139, { 7, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 156, { 7, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 178, { 7, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 198, { 7, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 209, { 7, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 232, { 7, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        // 10111000
        { 29, { 8, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 43, { 8, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 54, { 8, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 71, { 8, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 108, { 8, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 113, { 8, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 142, { 8, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 147, { 8, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 184, { 8, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 201, { 8, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 212, { 8, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 226, { 8, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        // 10010110
        { 60, { 9, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 85, { 9, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 105, { 9, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 150, { 9, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 170, { 9, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 195, { 9, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        // 10110011
        { 31, { 10, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 47, { 10, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 55, { 10, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 59, { 10, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 79, { 10, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 103, { 10, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 110, { 10, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 115, { 10, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 118, { 10, { 3, 0, 1, 2, 7, 4, 5, 6 } } },
        { 143, { 10, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 155, { 10, { 6, 5, 4, 7, 2, 1, 0, 3 } } },
        { 157, { 10, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 179, { 10, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 185, { 10, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 205, { 10, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 206, { 10, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 217, { 10, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 220, { 10, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 230, { 10, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 236, { 10, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 241, { 10, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 242, { 10, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 244, { 10, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 248, { 10, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        // 10111010
        { 61, { 11, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 62, { 11, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 87, { 11, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 93, { 11, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 107, { 11, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 109, { 11, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 117, { 11, { 3, 0, 1, 2, 7, 4, 5, 6 } } },
        { 121, { 11, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 124, { 11, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 151, { 11, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 158, { 11, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 171, { 11, { 6, 5, 4, 7, 2, 1, 0, 3 } } },
        { 174, { 11, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 182, { 11, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 186, { 11, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 188, { 11, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 199, { 11, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 203, { 11, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 211, { 11, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 213, { 11, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 214, { 11, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 227, { 11, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 233, { 11, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 234, { 11, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        // 10110111
        { 95, { 12, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 123, { 12, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 126, { 12, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 175, { 12, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 183, { 12, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 189, { 12, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 219, { 12, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 222, { 12, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 231, { 12, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 237, { 12, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 245, { 12, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 250, { 12, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        // 10111110
        { 125, { 13, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 190, { 13, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 215, { 13, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 235, { 13, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        // 10111111
        { 127, { 14, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 191, { 14, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 223, { 14, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 239, { 14, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 247, { 14, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 251, { 14, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 253, { 14, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 254, { 14, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        // 11111111
        { 255, { 15, { 0, 1, 2, 3, 4, 5, 6, 7 } } } };

    std::unordered_map<std::string, size_t> hXMap;

    // remove hanging nodes
    for (size_t level = 0; level < c.size(); ++level) {
        std::cout << "replacing cell with hex templates at tree depth: " << level + 1 << std::endl;
        const auto& cLayer = c[level];
        for (size_t cLayerIdx = 0; cLayerIdx < cLayer.size(); ++cLayerIdx) {
            const auto& cLayerCell = cLayer[cLayerIdx];
            if (cLayerCell.c[0] != cLayerCell.c[1]) { // skip if the current cell has children
                continue;
            }

            uint8_t cubeBlackCode = 0;
            uint8_t digit = 128;

            for (size_t cubeCornerIdx = 0; cubeCornerIdx < 8; ++cubeCornerIdx) {
                const auto& cubeCorner = cubeFaces[cubeCornerIdx];
                const std::string hash = HASHIT(cLayerCell.p[0][cubeCorner[0]], cLayerCell.p[1][cubeCorner[1]], cLayerCell.p[2][cubeCorner[2]]);
                if (posOctToMinSide.at(hash)[0] < 0.4 * (cLayerCell.p[0][3] - cLayerCell.p[0][0])) {
                    cubeBlackCode += digit;
                }
                digit /= 2;
            }

            const auto& rotation = rotations.at(cubeBlackCode);

            std::array<std::array<double, 3>, 8> eightPos;
            for (size_t i = 0; i < 8; ++i) {
                for (size_t j = 0; j < 3; ++j) {
                    eightPos[i][j] = double(cLayerCell.p[j][cubeFaces[rotation.second[i]][j]]);
                }
            }

            auto interpolatePoint = [](const std::array<double, 3>& p1, const int weight1,
                const std::array<double, 3>& p2, const int weight2) -> std::array<double, 3> {
                    std::array<double, 3> result;
                    const int totalWeight = weight1 + weight2;
                    for (size_t i = 0; i < 3; ++i) {
                        result[i] = p1[i] / totalWeight * weight1 + p2[i] / totalWeight * weight2;
                    }
                    return result;
                };

            auto interpolate3Points = [](const std::array<double, 3>& p1, const int weight1,
                const std::array<double, 3>& p2, const int weight2,
                const std::array<double, 3>& p3, const int weight3) -> std::array<double, 3> {
                    std::array<double, 3> result;
                    const int totalWeight = weight1 + weight2 + weight3;
                    for (size_t i = 0; i < 3; ++i) {
                        result[i] = p1[i] / totalWeight * weight1 + p2[i] / totalWeight * weight2 + p3[i] / totalWeight * weight3;
                    }
                    return result;
                };

            auto interpolate4Points = [](const std::array<double, 3>& p1, const int weight1,
                const std::array<double, 3>& p2, const int weight2,
                const std::array<double, 3>& p3, const int weight3,
                const std::array<double, 3>& p4, const int weight4) -> std::array<double, 3> {
                    std::array<double, 3> result;
                    const int totalWeight = weight1 + weight2 + weight3 + weight4;
                    for (size_t i = 0; i < 3; ++i) {
                        result[i] = p1[i] / totalWeight * weight1 + p2[i] / totalWeight * weight2 + p3[i] / totalWeight * weight3 + p4[i] / totalWeight * weight4;
                    }
                    return result;
                };

            // assign no-black-dot template (00000000) to eightPos
            auto noBlackDot = [&](const std::array<std::array<double, 3>, 8>& positions) {
                std::array<size_t, 8> elem;

                for (size_t i = 0; i < 8; ++i) {
                    const std::string hash = HASHIT(positions[i][0], positions[i][1], positions[i][2]);
                    if (!hXMap.count(hash)) {
                        hXMap[hash] = hX.size();
                        hX.push_back(positions[i]);
                    }
                    elem[i] = hXMap.at(hash);
                }
                hE.emplace_back(elem);
                };
            const std::array<int, 3> threeWeights = { 8, 5, 2 };
            const std::array<int, 3> fourWeights = { 4, 5, 1 };

            // assign two-black-dot template (10010000) to eightPos
            auto twoBlackDots = [&](const std::array<std::array<double, 3>, 8>& eightPos) {
                std::array<size_t, 16> ptsIdx;
                std::string hash;

                auto addPoint = [&](const std::array<double, 3>& point) -> size_t {
                    hash = HASHIT(point[0], point[1], point[2]);
                    if (!hXMap.count(hash)) {
                        hXMap[hash] = hX.size();
                        hX.push_back(point);
                    }
                    return hXMap.at(hash);
                    };

                // 0-7
                for (size_t j = 0; j < 8; ++j) {
                    ptsIdx[j] = addPoint(eightPos[j]);
                }

                // 8-15
                ptsIdx[8] = addPoint(interpolatePoint(eightPos[0], 2, eightPos[3], 1));
                ptsIdx[9] = addPoint(interpolate3Points(eightPos[1], threeWeights[0], eightPos[3], threeWeights[1], eightPos[0], threeWeights[2]));
                ptsIdx[10] = addPoint(interpolate3Points(eightPos[2], threeWeights[0], eightPos[0], threeWeights[1], eightPos[3], threeWeights[2]));
                ptsIdx[11] = addPoint(interpolatePoint(eightPos[3], 2, eightPos[0], 1));
                ptsIdx[12] = addPoint(interpolate3Points(eightPos[4], threeWeights[0], eightPos[3], threeWeights[1], eightPos[0], threeWeights[2]));
                ptsIdx[13] = addPoint(interpolate3Points(eightPos[5], threeWeights[0], eightPos[3], threeWeights[1], eightPos[0], threeWeights[2]));
                ptsIdx[14] = addPoint(interpolate3Points(eightPos[6], threeWeights[0], eightPos[0], threeWeights[1], eightPos[3], threeWeights[2]));
                ptsIdx[15] = addPoint(interpolate3Points(eightPos[7], threeWeights[0], eightPos[0], threeWeights[1], eightPos[3], threeWeights[2]));

                // 5 hex elements
                hE.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[9], ptsIdx[8], ptsIdx[4], ptsIdx[5], ptsIdx[13], ptsIdx[12] });
                hE.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
                hE.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[2], ptsIdx[10], ptsIdx[13], ptsIdx[5], ptsIdx[6], ptsIdx[14] });
                hE.push_back({ ptsIdx[11], ptsIdx[10], ptsIdx[2], ptsIdx[3], ptsIdx[15], ptsIdx[14], ptsIdx[6], ptsIdx[7] });
                hE.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
                };
            // specialized template for warped eightPos
            auto twoBlackDots2 = [&](const std::array<std::array<double, 3>, 8>& eightPos) {
                std::array<size_t, 16> ptsIdx;
                std::string hash;

                auto addPoint = [&](const std::array<double, 3>& point) -> size_t {
                    hash = HASHIT(point[0], point[1], point[2]);
                    if (!hXMap.count(hash)) {
                        hXMap[hash] = hX.size();
                        hX.push_back(point);
                    }
                    return hXMap.at(hash);
                    };

                // 0-7
                for (size_t j = 0; j < 8; ++j) {
                    ptsIdx[j] = addPoint(eightPos[j]);
                }

                // 8-15
                ptsIdx[8] = addPoint(interpolatePoint(eightPos[0], 2, eightPos[3], 1));
                ptsIdx[9] = addPoint(interpolate3Points(eightPos[1], threeWeights[0], eightPos[3], threeWeights[1], eightPos[0], threeWeights[2]));
                ptsIdx[10] = addPoint(interpolate3Points(eightPos[2], threeWeights[0], eightPos[0], threeWeights[1], eightPos[3], threeWeights[2]));
                ptsIdx[11] = addPoint(interpolatePoint(eightPos[3], 2, eightPos[0], 1));
                // move point 12, 13, 14, 15 down a little bit
                const std::array<int, 3> threeWeights2 = { 7, 5, 3 };
                const std::array<int, 4> fourWeights2 = { 24, 40, -15, 71 };
                ptsIdx[12] = addPoint(interpolate3Points(eightPos[4], threeWeights2[0], eightPos[3], threeWeights2[1], eightPos[0], threeWeights2[2]));
                ptsIdx[13] = addPoint(interpolate4Points(eightPos[0], fourWeights2[0], eightPos[3], fourWeights2[1], eightPos[4], fourWeights2[2], eightPos[5], fourWeights2[3]));
                ptsIdx[14] = addPoint(interpolate4Points(eightPos[3], fourWeights2[0], eightPos[0], fourWeights2[1], eightPos[7], fourWeights2[2], eightPos[6], fourWeights2[3]));
                ptsIdx[15] = addPoint(interpolate3Points(eightPos[7], threeWeights2[0], eightPos[0], threeWeights2[1], eightPos[3], threeWeights2[2]));

                // 5 hex elements
                hE.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[9], ptsIdx[8], ptsIdx[4], ptsIdx[5], ptsIdx[13], ptsIdx[12] });
                hE.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
                hE.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[2], ptsIdx[10], ptsIdx[13], ptsIdx[5], ptsIdx[6], ptsIdx[14] });
                hE.push_back({ ptsIdx[11], ptsIdx[10], ptsIdx[2], ptsIdx[3], ptsIdx[15], ptsIdx[14], ptsIdx[6], ptsIdx[7] });
                hE.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
                };

            // assign four-black-dot template (11110000) to eightPos
            auto fourBlackDots = [&](const std::array<std::array<double, 3>, 8>& eightPos) {
                std::array<size_t, 32> ptsIdx;
                std::string hash;

                auto addPoint = [&](const std::array<double, 3>& point) -> size_t {
                    hash = HASHIT(point[0], point[1], point[2]);
                    if (!hXMap.count(hash)) {
                        hXMap[hash] = hX.size();
                        hX.push_back(point);
                    }
                    return hXMap.at(hash);
                    };

                // 0-7
                for (size_t j = 0; j < 8; ++j) {
                    ptsIdx[j] = addPoint(eightPos[j]);
                }

                // 8-31
                ptsIdx[8] = addPoint(interpolatePoint(eightPos[0], 2, eightPos[1], 1));
                ptsIdx[9] = addPoint(interpolatePoint(eightPos[1], 2, eightPos[0], 1));
                ptsIdx[10] = addPoint(interpolatePoint(eightPos[0], 2, eightPos[3], 1));
                ptsIdx[11] = addPoint(interpolatePoint(eightPos[0], 2, eightPos[2], 1));
                ptsIdx[12] = addPoint(interpolatePoint(eightPos[1], 2, eightPos[3], 1));
                ptsIdx[13] = addPoint(interpolatePoint(eightPos[1], 2, eightPos[2], 1));
                ptsIdx[14] = addPoint(interpolatePoint(eightPos[3], 2, eightPos[0], 1));
                ptsIdx[15] = addPoint(interpolatePoint(eightPos[3], 2, eightPos[1], 1));
                ptsIdx[16] = addPoint(interpolatePoint(eightPos[2], 2, eightPos[0], 1));
                ptsIdx[17] = addPoint(interpolatePoint(eightPos[2], 2, eightPos[1], 1));
                ptsIdx[18] = addPoint(interpolatePoint(eightPos[3], 2, eightPos[2], 1));
                ptsIdx[19] = addPoint(interpolatePoint(eightPos[2], 2, eightPos[3], 1));
                ptsIdx[20] = addPoint(interpolate4Points(eightPos[0], fourWeights[0], eightPos[1], fourWeights[1], eightPos[3], fourWeights[1], eightPos[4], fourWeights[2]));
                ptsIdx[21] = addPoint(interpolate4Points(eightPos[1], fourWeights[0], eightPos[0], fourWeights[1], eightPos[2], fourWeights[1], eightPos[5], fourWeights[2]));
                ptsIdx[22] = addPoint(interpolate4Points(eightPos[3], fourWeights[0], eightPos[0], fourWeights[1], eightPos[2], fourWeights[1], eightPos[7], fourWeights[2]));
                ptsIdx[23] = addPoint(interpolate4Points(eightPos[2], fourWeights[0], eightPos[1], fourWeights[1], eightPos[3], fourWeights[1], eightPos[6], fourWeights[2]));
                ptsIdx[24] = addPoint(interpolate3Points(eightPos[4], threeWeights[0], eightPos[1], threeWeights[1], eightPos[0], threeWeights[2]));
                ptsIdx[25] = addPoint(interpolate3Points(eightPos[5], threeWeights[0], eightPos[0], threeWeights[1], eightPos[1], threeWeights[2]));
                ptsIdx[26] = addPoint(interpolate3Points(eightPos[5], threeWeights[0], eightPos[2], threeWeights[1], eightPos[1], threeWeights[2]));
                ptsIdx[27] = addPoint(interpolate3Points(eightPos[6], threeWeights[0], eightPos[1], threeWeights[1], eightPos[2], threeWeights[2]));
                ptsIdx[28] = addPoint(interpolate3Points(eightPos[6], threeWeights[0], eightPos[3], threeWeights[1], eightPos[2], threeWeights[2]));
                ptsIdx[29] = addPoint(interpolate3Points(eightPos[7], threeWeights[0], eightPos[2], threeWeights[1], eightPos[3], threeWeights[2]));
                ptsIdx[30] = addPoint(interpolate3Points(eightPos[7], threeWeights[0], eightPos[0], threeWeights[1], eightPos[3], threeWeights[2]));
                ptsIdx[31] = addPoint(interpolate3Points(eightPos[4], threeWeights[0], eightPos[3], threeWeights[1], eightPos[0], threeWeights[2]));

                // 13 hex elements
                hE.push_back({ ptsIdx[0], ptsIdx[8], ptsIdx[11], ptsIdx[10], ptsIdx[4], ptsIdx[24], ptsIdx[20], ptsIdx[31] });
                hE.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[12], ptsIdx[11], ptsIdx[24], ptsIdx[25], ptsIdx[21], ptsIdx[20] });
                hE.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[13], ptsIdx[12], ptsIdx[25], ptsIdx[5], ptsIdx[26], ptsIdx[21] });
                hE.push_back({ ptsIdx[10], ptsIdx[11], ptsIdx[15], ptsIdx[14], ptsIdx[31], ptsIdx[20], ptsIdx[22], ptsIdx[30] });
                hE.push_back({ ptsIdx[11], ptsIdx[12], ptsIdx[16], ptsIdx[15], ptsIdx[20], ptsIdx[21], ptsIdx[23], ptsIdx[22] });
                hE.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[17], ptsIdx[16], ptsIdx[21], ptsIdx[26], ptsIdx[27], ptsIdx[23] });
                hE.push_back({ ptsIdx[14], ptsIdx[15], ptsIdx[18], ptsIdx[3], ptsIdx[30], ptsIdx[22], ptsIdx[29], ptsIdx[7] });
                hE.push_back({ ptsIdx[15], ptsIdx[16], ptsIdx[19], ptsIdx[18], ptsIdx[22], ptsIdx[23], ptsIdx[28], ptsIdx[29] });
                hE.push_back({ ptsIdx[16], ptsIdx[17], ptsIdx[2], ptsIdx[19], ptsIdx[23], ptsIdx[27], ptsIdx[6], ptsIdx[28] });
                hE.push_back({ ptsIdx[20], ptsIdx[21], ptsIdx[23], ptsIdx[22], ptsIdx[24], ptsIdx[25], ptsIdx[28], ptsIdx[29] });
                hE.push_back({ ptsIdx[31], ptsIdx[20], ptsIdx[22], ptsIdx[30], ptsIdx[4], ptsIdx[24], ptsIdx[29], ptsIdx[7] });
                hE.push_back({ ptsIdx[21], ptsIdx[26], ptsIdx[27], ptsIdx[23], ptsIdx[25], ptsIdx[5], ptsIdx[6], ptsIdx[28] });
                hE.push_back({ ptsIdx[24], ptsIdx[25], ptsIdx[28], ptsIdx[29], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
                };
            // assign four-black-dot template (10010110) to eightPos
            auto fourBlackDots2 = [&](const std::array<std::array<double, 3>, 8>& eightPos) {
                std::array<size_t, 16> ptsIdx;
                std::string hash;

                auto addPoint = [&](const std::array<double, 3>& point) -> size_t {
                    hash = HASHIT(point[0], point[1], point[2]);
                    if (!hXMap.count(hash)) {
                        hXMap[hash] = hX.size();
                        hX.push_back(point);
                    }
                    return hXMap.at(hash);
                    };

                std::array<std::array<double, 3>, 16> pts;

                // 0-7
                for (size_t j = 0; j < 8; ++j) {
                    pts[j] = eightPos[j];
                    ptsIdx[j] = addPoint(pts[j]);
                }

                // 8-15
                pts[8] = interpolatePoint(eightPos[0], 2, eightPos[3], 1);
                pts[9] = interpolate3Points(eightPos[1], threeWeights[0], eightPos[3], threeWeights[1], eightPos[0], threeWeights[2]);
                pts[10] = interpolate3Points(eightPos[2], threeWeights[0], eightPos[0], threeWeights[1], eightPos[3], threeWeights[2]);
                pts[11] = interpolatePoint(eightPos[3], 2, eightPos[0], 1);
                pts[12] = interpolate3Points(eightPos[4], threeWeights[0], eightPos[3], threeWeights[1], eightPos[0], threeWeights[2]);
                pts[13] = interpolate3Points(eightPos[5], threeWeights[0], eightPos[3], threeWeights[1], eightPos[0], threeWeights[2]);
                pts[14] = interpolate3Points(eightPos[6], threeWeights[0], eightPos[0], threeWeights[1], eightPos[3], threeWeights[2]);
                pts[15] = interpolate3Points(eightPos[7], threeWeights[0], eightPos[0], threeWeights[1], eightPos[3], threeWeights[2]);
                for (size_t j = 8; j < 16; ++j) {
                    ptsIdx[j] = addPoint(pts[j]);
                }

                // 5 hex elements
                hE.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[9], ptsIdx[8], ptsIdx[4], ptsIdx[5], ptsIdx[13], ptsIdx[12] });
                hE.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
                hE.push_back({ ptsIdx[11], ptsIdx[10], ptsIdx[2], ptsIdx[3], ptsIdx[15], ptsIdx[14], ptsIdx[6], ptsIdx[7] });

                twoBlackDots2({ pts[6], pts[2], pts[1], pts[5], pts[14], pts[10], pts[9], pts[13] });
                twoBlackDots2({ pts[5], pts[4], pts[7], pts[6], pts[13], pts[12], pts[15], pts[14] });
                };

            // assign three-black-dot template (10110000) to eightPos
            auto threeBlackDots = [&](const std::array<std::array<double, 3>, 8>& eightPos) {
                const std::array<std::array<double, 3>, 16> allP = {
                    eightPos[0],
                    eightPos[1],
                    eightPos[2],
                    eightPos[3],
                    eightPos[4],
                    eightPos[5],
                    eightPos[6],
                    eightPos[7],
                    interpolatePoint(eightPos[0], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[1], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[2], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[3], 3, eightPos[1], 1),
                    interpolate3Points(eightPos[4], 3, eightPos[2], 4, eightPos[0], 9),
                    interpolate3Points(eightPos[5], 3, eightPos[3], 4, eightPos[1], 9),
                    interpolate3Points(eightPos[6], 3, eightPos[0], 4, eightPos[2], 9),
                    interpolate3Points(eightPos[7], 3, eightPos[1], 4, eightPos[3], 9) };
                twoBlackDots({ allP[0], allP[8], allP[11], allP[3], allP[4], allP[12], allP[15], allP[7] });
                twoBlackDots({ allP[3], allP[11], allP[10], allP[2], allP[7], allP[15], allP[14], allP[6] });
                noBlackDot({ allP[0], allP[1], allP[9], allP[8], allP[4], allP[5], allP[13], allP[12] });
                noBlackDot({ allP[1], allP[2], allP[10], allP[9], allP[5], allP[6], allP[14], allP[13] });
                noBlackDot({ allP[12], allP[13], allP[14], allP[15], allP[4], allP[5], allP[6], allP[7] });
                noBlackDot({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
                };

            if (rotation.first == 1) {
                noBlackDot(eightPos);
            }
            else if (rotation.first == 2) {
                twoBlackDots(eightPos);
            }
            else if (rotation.first == 3) {
                fourBlackDots(eightPos);
            }
            else if (rotation.first == 4) {
                const std::array<std::array<double, 3>, 16> allP = {
                eightPos[0],
                eightPos[1],
                eightPos[2],
                eightPos[3],
                eightPos[4],
                eightPos[5],
                eightPos[6],
                eightPos[7],
                interpolatePoint(eightPos[0], 3, eightPos[2], 1),
                interpolatePoint(eightPos[1], 3, eightPos[3], 1),
                interpolatePoint(eightPos[2], 3, eightPos[0], 1),
                interpolatePoint(eightPos[3], 3, eightPos[1], 1),
                interpolatePoint(eightPos[4], 3, eightPos[2], 1),
                interpolatePoint(eightPos[5], 3, eightPos[3], 1),
                interpolatePoint(eightPos[6], 3, eightPos[0], 1),
                interpolatePoint(eightPos[7], 3, eightPos[1], 1) };
                twoBlackDots({ allP[0], allP[8], allP[11], allP[3], allP[4], allP[12], allP[15], allP[7] });
                twoBlackDots({ allP[3], allP[11], allP[10], allP[2], allP[7], allP[15], allP[14], allP[6] });
                noBlackDot({ allP[0], allP[1], allP[9], allP[8], allP[4], allP[5], allP[13], allP[12] });
                noBlackDot({ allP[1], allP[2], allP[10], allP[9], allP[5], allP[6], allP[14], allP[13] });
                noBlackDot({ allP[12], allP[13], allP[14], allP[15], allP[4], allP[5], allP[6], allP[7] });
                noBlackDot({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
            }
            else if (rotation.first == 5) {
                const std::array<std::array<double, 3>, 16> allP = {
                    eightPos[0],
                    eightPos[1],
                    eightPos[2],
                    eightPos[3],
                    eightPos[4],
                    eightPos[5],
                    eightPos[6],
                    eightPos[7],
                    interpolatePoint(eightPos[0], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[1], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[2], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[3], 3, eightPos[1], 1),
                    interpolatePoint(eightPos[4], 3, eightPos[6], 1),
                    interpolatePoint(eightPos[5], 3, eightPos[7], 1),
                    interpolatePoint(eightPos[6], 3, eightPos[4], 1),
                    interpolatePoint(eightPos[7], 3, eightPos[5], 1) };
                fourBlackDots({ allP[0], allP[3], allP[7], allP[4], allP[8], allP[11], allP[15], allP[12] });
                fourBlackDots({ allP[3], allP[2], allP[6], allP[7], allP[11], allP[10], allP[14], allP[15] });
                twoBlackDots({ allP[4], allP[5], allP[1], allP[0], allP[12], allP[13], allP[9], allP[8] });
                twoBlackDots({ allP[2], allP[1], allP[5], allP[6], allP[10], allP[9], allP[13], allP[14] });
                noBlackDot({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
            }
            else if (rotation.first == 6) {
                const std::array<std::array<double, 3>, 16> allP = {
                    eightPos[0],
                    eightPos[1],
                    eightPos[2],
                    eightPos[3],
                    eightPos[4],
                    eightPos[5],
                    eightPos[6],
                    eightPos[7],
                    interpolatePoint(eightPos[0], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[1], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[2], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[3], 3, eightPos[1], 1),
                    interpolatePoint(eightPos[4], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[5], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[6], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[7], 3, eightPos[1], 1) };
                threeBlackDots({ allP[7], allP[4], allP[0], allP[3], allP[15], allP[12], allP[8], allP[11] });
                threeBlackDots({ allP[2], allP[6], allP[7], allP[3], allP[10], allP[14], allP[15], allP[11] });
                noBlackDot({ allP[0], allP[1], allP[9], allP[8], allP[4], allP[5], allP[13], allP[12] });
                noBlackDot({ allP[1], allP[2], allP[10], allP[9], allP[5], allP[6], allP[14], allP[13] });
                noBlackDot({ allP[12], allP[13], allP[14], allP[15], allP[4], allP[5], allP[6], allP[7] });
                noBlackDot({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
            }
            else if (rotation.first == 7) {
                const std::array<std::array<double, 3>, 16> allP = {
                    eightPos[0],
                    eightPos[1],
                    eightPos[2],
                    eightPos[3],
                    eightPos[4],
                    eightPos[5],
                    eightPos[6],
                    eightPos[7],
                    interpolatePoint(eightPos[0], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[1], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[2], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[3], 3, eightPos[1], 1),
                    interpolatePoint(eightPos[4], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[5], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[6], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[7], 3, eightPos[1], 1) };
                twoBlackDots({ allP[0], allP[8], allP[11], allP[3], allP[4], allP[12], allP[15], allP[7] });
                threeBlackDots({ allP[6], allP[7], allP[3], allP[2], allP[14], allP[15], allP[11], allP[10] });
                noBlackDot({ allP[0], allP[1], allP[9], allP[8], allP[4], allP[5], allP[13], allP[12] });
                twoBlackDots({ allP[2], allP[1], allP[5], allP[6], allP[10], allP[9], allP[13], allP[14] });
                noBlackDot({ allP[12], allP[13], allP[14], allP[15], allP[4], allP[5], allP[6], allP[7] });
                noBlackDot({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
            }
            else if (rotation.first == 8) {
                const std::array<std::array<double, 3>, 16> allP = {
                    eightPos[0],
                    eightPos[1],
                    eightPos[2],
                    eightPos[3],
                    eightPos[4],
                    eightPos[5],
                    eightPos[6],
                    eightPos[7],
                    interpolatePoint(eightPos[0], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[1], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[2], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[3], 3, eightPos[1], 1),
                    interpolatePoint(eightPos[4], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[5], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[6], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[7], 3, eightPos[1], 1) };
                threeBlackDots({ allP[3], allP[7], allP[4], allP[0], allP[11], allP[15], allP[12], allP[8] });
                twoBlackDots({ allP[3], allP[11], allP[10], allP[2], allP[7], allP[15], allP[14], allP[6] });
                twoBlackDots({ allP[4], allP[5], allP[1], allP[0], allP[12], allP[13], allP[9], allP[8] });
                noBlackDot({ allP[1], allP[2], allP[10], allP[9], allP[5], allP[6], allP[14], allP[13] });
                noBlackDot({ allP[12], allP[13], allP[14], allP[15], allP[4], allP[5], allP[6], allP[7] });
                noBlackDot({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
            }
            else if (rotation.first == 9) {
                fourBlackDots2(eightPos);
            }
            else if (rotation.first == 10) {
                const std::array<std::array<double, 3>, 16> allP = {
                    eightPos[0],
                    eightPos[1],
                    eightPos[2],
                    eightPos[3],
                    eightPos[4],
                    eightPos[5],
                    eightPos[6],
                    eightPos[7],
                    interpolatePoint(eightPos[0], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[1], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[2], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[3], 3, eightPos[1], 1),
                    interpolatePoint(eightPos[4], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[5], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[6], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[7], 3, eightPos[1], 1) };
                threeBlackDots({ allP[7], allP[4], allP[0], allP[3], allP[15], allP[12], allP[8], allP[11] });
                fourBlackDots({ allP[3], allP[2], allP[6], allP[7], allP[11], allP[10], allP[14], allP[15] });
                twoBlackDots({ allP[2], allP[1], allP[5], allP[6], allP[10], allP[9], allP[13], allP[14] });
                noBlackDot({ allP[4], allP[5], allP[1], allP[0], allP[12], allP[13], allP[9], allP[8] });
                noBlackDot({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
                twoBlackDots({ allP[6], allP[5], allP[4], allP[7], allP[14], allP[13], allP[12], allP[15] });
            }
            else if (rotation.first == 11) {
                const std::array<std::array<double, 3>, 16> allP = {
                    eightPos[0],
                    eightPos[1],
                    eightPos[2],
                    eightPos[3],
                    eightPos[4],
                    eightPos[5],
                    eightPos[6],
                    eightPos[7],
                    interpolatePoint(eightPos[0], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[1], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[2], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[3], 3, eightPos[1], 1),
                    interpolatePoint(eightPos[4], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[5], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[6], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[7], 3, eightPos[1], 1) };
                threeBlackDots({ allP[3], allP[7], allP[4], allP[0], allP[11], allP[15], allP[12], allP[8] });
                threeBlackDots({ allP[6], allP[7], allP[3], allP[2], allP[14], allP[15], allP[11], allP[10] });
                twoBlackDots({ allP[2], allP[1], allP[5], allP[6], allP[10], allP[9], allP[13], allP[14] });
                twoBlackDots({ allP[4], allP[5], allP[1], allP[0], allP[12], allP[13], allP[9], allP[8] });
                noBlackDot({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
                noBlackDot({ allP[6], allP[5], allP[4], allP[7], allP[14], allP[13], allP[12], allP[15] });
            }
            else if (rotation.first == 12) {
                const std::array<std::array<double, 3>, 16> allP = {
                    eightPos[0],
                    eightPos[1],
                    eightPos[2],
                    eightPos[3],
                    eightPos[4],
                    eightPos[5],
                    eightPos[6],
                    eightPos[7],
                    interpolatePoint(eightPos[0], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[1], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[2], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[3], 3, eightPos[1], 1),
                    interpolatePoint(eightPos[4], 3, eightPos[6], 1),
                    interpolatePoint(eightPos[5], 3, eightPos[7], 1),
                    interpolatePoint(eightPos[6], 3, eightPos[4], 1),
                    interpolatePoint(eightPos[7], 3, eightPos[5], 1) };
                threeBlackDots({ allP[7], allP[4], allP[0], allP[3], allP[15], allP[12], allP[8], allP[11] });
                fourBlackDots({ allP[3], allP[2], allP[6], allP[7], allP[11], allP[10], allP[14], allP[15] });
                threeBlackDots({ allP[2], allP[1], allP[5], allP[6], allP[10], allP[9], allP[13], allP[14] });
                noBlackDot({ allP[4], allP[5], allP[1], allP[0], allP[12], allP[13], allP[9], allP[8] });
                noBlackDot({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
            }
            else if (rotation.first == 13) {
                const std::array<std::array<double, 3>, 16> allP = {
                    eightPos[0],
                    eightPos[1],
                    eightPos[2],
                    eightPos[3],
                    eightPos[4],
                    eightPos[5],
                    eightPos[6],
                    eightPos[7],
                    interpolatePoint(eightPos[0], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[1], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[2], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[3], 3, eightPos[1], 1),
                    interpolatePoint(eightPos[4], 3, eightPos[6], 1),
                    interpolatePoint(eightPos[5], 3, eightPos[7], 1),
                    interpolatePoint(eightPos[6], 3, eightPos[4], 1),
                    interpolatePoint(eightPos[7], 3, eightPos[5], 1) };
                threeBlackDots({ allP[3], allP[7], allP[4], allP[0], allP[11], allP[15], allP[12], allP[8] });
                threeBlackDots({ allP[6], allP[7], allP[3], allP[2], allP[14], allP[15], allP[11], allP[10] });
                threeBlackDots({ allP[2], allP[1], allP[5], allP[6], allP[10], allP[9], allP[13], allP[14] });
                threeBlackDots({ allP[5], allP[1], allP[0], allP[4], allP[13], allP[9], allP[8], allP[12] });
                noBlackDot({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
            }
            else if (rotation.first == 14) {
                const std::array<std::array<double, 3>, 16> allP = {
                    eightPos[0],
                    eightPos[1],
                    eightPos[2],
                    eightPos[3],
                    eightPos[4],
                    eightPos[5],
                    eightPos[6],
                    eightPos[7],
                    interpolatePoint(eightPos[0], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[1], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[2], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[3], 3, eightPos[1], 1),
                    interpolatePoint(eightPos[4], 3, eightPos[2], 1),
                    interpolatePoint(eightPos[5], 3, eightPos[3], 1),
                    interpolatePoint(eightPos[6], 3, eightPos[0], 1),
                    interpolatePoint(eightPos[7], 3, eightPos[1], 1) };
                fourBlackDots({ allP[3], allP[7], allP[4], allP[0], allP[11], allP[15], allP[12], allP[8] });
                fourBlackDots({ allP[6], allP[7], allP[3], allP[2], allP[14], allP[15], allP[11], allP[10] });
                threeBlackDots({ allP[2], allP[1], allP[5], allP[6], allP[10], allP[9], allP[13], allP[14] });
                threeBlackDots({ allP[5], allP[1], allP[0], allP[4], allP[13], allP[9], allP[8], allP[12] });
                noBlackDot({ allP[8], allP[9], allP[10], allP[11], allP[12], allP[13], allP[14], allP[15] });
                fourBlackDots({ allP[7], allP[6], allP[5], allP[4], allP[15], allP[14], allP[13], allP[12] });
            }
            else if (rotation.first == 15) {
                const std::array<std::array<double, 3>, 64> allP = { {
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][0]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][1]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][2]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][0]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][1]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][2]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][3]) },
                    { double(cLayerCell.p[0][3]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][3]) } } };
                noBlackDot({ allP[0], allP[1], allP[5], allP[4], allP[16], allP[17], allP[21], allP[20] });
                noBlackDot({ allP[1], allP[2], allP[6], allP[5], allP[17], allP[18], allP[22], allP[21] });
                noBlackDot({ allP[2], allP[3], allP[7], allP[6], allP[18], allP[19], allP[23], allP[22] });
                noBlackDot({ allP[4], allP[5], allP[9], allP[8], allP[20], allP[21], allP[25], allP[24] });
                noBlackDot({ allP[5], allP[6], allP[10], allP[9], allP[21], allP[22], allP[26], allP[25] });
                noBlackDot({ allP[6], allP[7], allP[11], allP[10], allP[22], allP[23], allP[27], allP[26] });
                noBlackDot({ allP[8], allP[9], allP[13], allP[12], allP[24], allP[25], allP[29], allP[28] });
                noBlackDot({ allP[9], allP[10], allP[14], allP[13], allP[25], allP[26], allP[30], allP[29] });
                noBlackDot({ allP[10], allP[11], allP[15], allP[14], allP[26], allP[27], allP[31], allP[30] });
                noBlackDot({ allP[16], allP[17], allP[21], allP[20], allP[32], allP[33], allP[37], allP[36] });
                noBlackDot({ allP[17], allP[18], allP[22], allP[21], allP[33], allP[34], allP[38], allP[37] });
                noBlackDot({ allP[18], allP[19], allP[23], allP[22], allP[34], allP[35], allP[39], allP[38] });
                noBlackDot({ allP[20], allP[21], allP[25], allP[24], allP[36], allP[37], allP[41], allP[40] });
                noBlackDot({ allP[21], allP[22], allP[26], allP[25], allP[37], allP[38], allP[42], allP[41] });
                noBlackDot({ allP[22], allP[23], allP[27], allP[26], allP[38], allP[39], allP[43], allP[42] });
                noBlackDot({ allP[24], allP[25], allP[29], allP[28], allP[40], allP[41], allP[45], allP[44] });
                noBlackDot({ allP[25], allP[26], allP[30], allP[29], allP[41], allP[42], allP[46], allP[45] });
                noBlackDot({ allP[26], allP[27], allP[31], allP[30], allP[42], allP[43], allP[47], allP[46] });
                noBlackDot({ allP[32], allP[33], allP[37], allP[36], allP[48], allP[49], allP[53], allP[52] });
                noBlackDot({ allP[33], allP[34], allP[38], allP[37], allP[49], allP[50], allP[54], allP[53] });
                noBlackDot({ allP[34], allP[35], allP[39], allP[38], allP[50], allP[51], allP[55], allP[54] });
                noBlackDot({ allP[36], allP[37], allP[41], allP[40], allP[52], allP[53], allP[57], allP[56] });
                noBlackDot({ allP[37], allP[38], allP[42], allP[41], allP[53], allP[54], allP[58], allP[57] });
                noBlackDot({ allP[38], allP[39], allP[43], allP[42], allP[54], allP[55], allP[59], allP[58] });
                noBlackDot({ allP[40], allP[41], allP[45], allP[44], allP[56], allP[57], allP[61], allP[60] });
                noBlackDot({ allP[41], allP[42], allP[46], allP[45], allP[57], allP[58], allP[62], allP[61] });
                noBlackDot({ allP[42], allP[43], allP[47], allP[46], allP[58], allP[59], allP[63], allP[62] });
            }
        }
    }

    return Hexahedra(tri, hE, hX);
}