#include "geometry.h"

Hexahedra Tree::RemoveHangingNodesMethod2() {
    std::vector<std::array<size_t, 8>> hE;
    std::vector<std::array<double, 3>> hX;

    const std::array<std::array<size_t, 3>, 8> cubeCorners = { {
            { 0, 0, 0 },
            { 3, 0, 0 },
            { 3, 3, 0 },
            { 0, 3, 0 },
            { 0, 0, 3 },
            { 3, 0, 3 },
            { 3, 3, 3 },
            { 0, 3, 3 } } };

    // map coordinate to point index
    std::unordered_map<std::string, size_t> xMap;
    for (const auto& cLayer : c) {
        for (const auto& cLayerCell : cLayer) {
            if (cLayerCell.c[0] != cLayerCell.c[1]) { // ignore cells that have children
                continue;
            }

            for (const auto& cubeCorner : cubeCorners) {
                const std::string hash = HASHIT(double(cLayerCell.p[0][cubeCorner[0]]), double(cLayerCell.p[1][cubeCorner[1]]), double(cLayerCell.p[2][cubeCorner[2]]));
                if (!xMap.count(hash)) {
                    xMap[hash] = hX.size();
                    hX.push_back({ double(cLayerCell.p[0][cubeCorner[0]]), double(cLayerCell.p[1][cubeCorner[1]]), double(cLayerCell.p[2][cubeCorner[2]]) });
                }
            }
        }
    }

    size_t cellIncrease = 0;
    bool cellIncreased = true;

    const std::array<std::array<size_t, 6>, 42> cubeModeratelyBalancedConditionCheckers = { {
        { 8, 1, 1, 0, 1, 0 },
        { 5, 4, 1, 0, 1, 0 },
        { 2, 7, 1, 0, 1, 0 },
        { 5, 4, 1, 0, 5, 4 },
        { 8, 1, 1, 0, 0, 1 },
        { 5, 4, 1, 0, 0, 1 },
        { 2, 7, 1, 0, 0, 1 },
        { 8, 1, 0, 1, 1, 0 },
        { 5, 4, 0, 1, 1, 0 },
        { 2, 7, 0, 1, 1, 0 },
        { 5, 4, 0, 1, 5, 4 },
        { 8, 1, 0, 1, 0, 1 },
        { 5, 4, 0, 1, 0, 1 },
        { 2, 7, 0, 1, 0, 1 },
        { 1, 0, 8, 1, 1, 0 },
        { 1, 0, 5, 4, 1, 0 },
        { 1, 0, 2, 7, 1, 0 },
        { 1, 0, 5, 4, 5, 4 },
        { 1, 0, 8, 1, 0, 1 },
        { 1, 0, 5, 4, 0, 1 },
        { 1, 0, 2, 7, 0, 1 },
        { 0, 1, 8, 1, 1, 0 },
        { 0, 1, 5, 4, 1, 0 },
        { 0, 1, 2, 7, 1, 0 },
        { 0, 1, 5, 4, 5, 4 },
        { 0, 1, 8, 1, 0, 1 },
        { 0, 1, 5, 4, 0, 1 },
        { 0, 1, 2, 7, 0, 1 },
        { 5, 4, 5, 4, 1, 0 },
        { 5, 4, 5, 4, 0, 1 },
        { 1, 0, 1, 0, 8, 1 },
        { 0, 1, 1, 0, 8, 1 },
        { 0, 1, 0, 1, 8, 1 },
        { 1, 0, 0, 1, 8, 1 },
        { 1, 0, 1, 0, 5, 4 },
        { 0, 1, 1, 0, 5, 4 },
        { 0, 1, 0, 1, 5, 4 },
        { 1, 0, 0, 1, 5, 4 },
        { 1, 0, 1, 0, 2, 7 },
        { 0, 1, 1, 0, 2, 7 },
        { 0, 1, 0, 1, 2, 7 },
        { 1, 0, 0, 1, 2, 7 } } };

    // make the grid moderately-balanced and pairing
    // combine with gap filling and decoupling node analysis
    while (cellIncreased) {
        cellIncreased = false;

        // indicate whether a point is level1 point
        std::unordered_set<std::string> isLevel1;
        // indicate whether a point is level0 point
        std::unordered_set<std::string> isLevel0;
        for (size_t level = 0; level < c.size() - 1; ++level) {
            const auto& cLayer = c[level];
            for (const auto& cLayerCell : cLayer) {
                if (cLayerCell.c[0] != cLayerCell.c[1]) { // skip if the current cell has children
                    continue;
                }

                const std::array<size_t, 12> edgeIsSubdivided = {
                    xMap.count(HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][0]))),
                    xMap.count(HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][0]))),
                    xMap.count(HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][0]))),
                    xMap.count(HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][0]))),
                    xMap.count(HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][1]))),
                    xMap.count(HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][1]))),
                    xMap.count(HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][1]))),
                    xMap.count(HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][1]))),
                    xMap.count(HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][3]))),
                    xMap.count(HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][3]))),
                    xMap.count(HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][3]))),
                    xMap.count(HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][3]))) };
                const size_t subdividedEdgeCount = edgeIsSubdivided[0] + edgeIsSubdivided[1] + edgeIsSubdivided[2] + edgeIsSubdivided[3] +
                    edgeIsSubdivided[4] + edgeIsSubdivided[5] + edgeIsSubdivided[6] + edgeIsSubdivided[7] +
                    edgeIsSubdivided[8] + edgeIsSubdivided[9] + edgeIsSubdivided[10] + edgeIsSubdivided[11];
                const std::array<std::string, 8> eightCornerHashes = {
                    HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][0])),
                    HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][0])),
                    HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][0])),
                    HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][0])),
                    HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][3])),
                    HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][3])),
                    HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][3])),
                    HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][3])) };

                if (subdividedEdgeCount == 2) {
                    if (edgeIsSubdivided[0] && edgeIsSubdivided[1]) {
                        isLevel1.insert(eightCornerHashes[3]);
                    }
                    else if (edgeIsSubdivided[1] && edgeIsSubdivided[2]) {
                        isLevel1.insert(eightCornerHashes[0]);
                    }
                    else if (edgeIsSubdivided[2] && edgeIsSubdivided[3]) {
                        isLevel1.insert(eightCornerHashes[1]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[3]) {
                        isLevel1.insert(eightCornerHashes[2]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[5]) {
                        isLevel1.insert(eightCornerHashes[4]);
                    }
                    else if (edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                        isLevel1.insert(eightCornerHashes[0]);
                    }
                    else if (edgeIsSubdivided[4] && edgeIsSubdivided[8]) {
                        isLevel1.insert(eightCornerHashes[1]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[4]) {
                        isLevel1.insert(eightCornerHashes[5]);
                    }
                    else if (edgeIsSubdivided[1] && edgeIsSubdivided[6]) {
                        isLevel1.insert(eightCornerHashes[5]);
                    }
                    else if (edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                        isLevel1.insert(eightCornerHashes[1]);
                    }
                    else if (edgeIsSubdivided[5] && edgeIsSubdivided[9]) {
                        isLevel1.insert(eightCornerHashes[2]);
                    }
                    else if (edgeIsSubdivided[1] && edgeIsSubdivided[5]) {
                        isLevel1.insert(eightCornerHashes[6]);
                    }
                    else if (edgeIsSubdivided[2] && edgeIsSubdivided[6]) {
                        isLevel1.insert(eightCornerHashes[7]);
                    }
                    else if (edgeIsSubdivided[6] && edgeIsSubdivided[10]) {
                        isLevel1.insert(eightCornerHashes[3]);
                    }
                    else if (edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                        isLevel1.insert(eightCornerHashes[2]);
                    }
                    else if (edgeIsSubdivided[2] && edgeIsSubdivided[7]) {
                        isLevel1.insert(eightCornerHashes[6]);
                    }
                    else if (edgeIsSubdivided[3] && edgeIsSubdivided[7]) {
                        isLevel1.insert(eightCornerHashes[4]);
                    }
                    else if (edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                        isLevel1.insert(eightCornerHashes[0]);
                    }
                    else if (edgeIsSubdivided[4] && edgeIsSubdivided[11]) {
                        isLevel1.insert(eightCornerHashes[3]);
                    }
                    else if (edgeIsSubdivided[3] && edgeIsSubdivided[4]) {
                        isLevel1.insert(eightCornerHashes[7]);
                    }
                    else if (edgeIsSubdivided[8] && edgeIsSubdivided[9]) {
                        isLevel1.insert(eightCornerHashes[7]);
                    }
                    else if (edgeIsSubdivided[9] && edgeIsSubdivided[10]) {
                        isLevel1.insert(eightCornerHashes[4]);
                    }
                    else if (edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel1.insert(eightCornerHashes[5]);
                    }
                    else if (edgeIsSubdivided[8] && edgeIsSubdivided[11]) {
                        isLevel1.insert(eightCornerHashes[6]);
                    }
                }
                else if (subdividedEdgeCount == 7) {
                    if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] &&
                        edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                        isLevel1.insert(eightCornerHashes[6]);
                        isLevel1.insert(eightCornerHashes[7]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] &&
                        edgeIsSubdivided[3] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                        isLevel1.insert(eightCornerHashes[4]);
                        isLevel1.insert(eightCornerHashes[7]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] &&
                        edgeIsSubdivided[3] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                        isLevel1.insert(eightCornerHashes[4]);
                        isLevel1.insert(eightCornerHashes[5]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] &&
                        edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                        isLevel1.insert(eightCornerHashes[5]);
                        isLevel1.insert(eightCornerHashes[6]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[3] && edgeIsSubdivided[4] &&
                        edgeIsSubdivided[5] && edgeIsSubdivided[7] && edgeIsSubdivided[8] && edgeIsSubdivided[11]) {
                        isLevel1.insert(eightCornerHashes[2]);
                        isLevel1.insert(eightCornerHashes[6]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[4] &&
                        edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[8] && edgeIsSubdivided[9]) {
                        isLevel1.insert(eightCornerHashes[3]);
                        isLevel1.insert(eightCornerHashes[7]);
                    }
                    else if (edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[5] &&
                        edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[9] && edgeIsSubdivided[10]) {
                        isLevel1.insert(eightCornerHashes[0]);
                        isLevel1.insert(eightCornerHashes[4]);
                    }
                    else if (edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[4] &&
                        edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel1.insert(eightCornerHashes[1]);
                        isLevel1.insert(eightCornerHashes[5]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[5] &&
                        edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel1.insert(eightCornerHashes[2]);
                        isLevel1.insert(eightCornerHashes[3]);
                    }
                    else if (edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[6] &&
                        edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel1.insert(eightCornerHashes[0]);
                        isLevel1.insert(eightCornerHashes[3]);
                    }
                    else if (edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7] &&
                        edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel1.insert(eightCornerHashes[0]);
                        isLevel1.insert(eightCornerHashes[1]);
                    }
                    else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] &&
                        edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel1.insert(eightCornerHashes[1]);
                        isLevel1.insert(eightCornerHashes[2]);
                    }
                }
                else if (subdividedEdgeCount == 3) {
                    if (edgeIsSubdivided[0] && edgeIsSubdivided[3] && edgeIsSubdivided[4]) {
                        isLevel0.insert(eightCornerHashes[6]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[5]) {
                        isLevel0.insert(eightCornerHashes[7]);
                    }
                    else if (edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[6]) {
                        isLevel0.insert(eightCornerHashes[4]);
                    }
                    else if (edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[7]) {
                        isLevel0.insert(eightCornerHashes[5]);
                    }
                    else if (edgeIsSubdivided[4] && edgeIsSubdivided[8] && edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[2]);
                    }
                    else if (edgeIsSubdivided[5] && edgeIsSubdivided[8] && edgeIsSubdivided[9]) {
                        isLevel0.insert(eightCornerHashes[3]);
                    }
                    else if (edgeIsSubdivided[6] && edgeIsSubdivided[9] && edgeIsSubdivided[10]) {
                        isLevel0.insert(eightCornerHashes[0]);
                    }
                    else if (edgeIsSubdivided[7] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[1]);
                    }
                }
                else if (subdividedEdgeCount == 5) {
                    if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[4]) {
                        isLevel0.insert(eightCornerHashes[6]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[5]) {
                        isLevel0.insert(eightCornerHashes[7]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[6]) {
                        isLevel0.insert(eightCornerHashes[4]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[7]) {
                        isLevel0.insert(eightCornerHashes[5]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                        isLevel0.insert(eightCornerHashes[7]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8] && edgeIsSubdivided[9]) {
                        isLevel0.insert(eightCornerHashes[3]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8] && edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[2]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                        isLevel0.insert(eightCornerHashes[6]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                        isLevel0.insert(eightCornerHashes[7]);
                    }
                    else if (edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                        isLevel0.insert(eightCornerHashes[4]);
                    }
                    else if (edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9] && edgeIsSubdivided[10]) {
                        isLevel0.insert(eightCornerHashes[0]);
                    }
                    else if (edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[8] && edgeIsSubdivided[9]) {
                        isLevel0.insert(eightCornerHashes[3]);
                    }
                    else if (edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                        isLevel0.insert(eightCornerHashes[4]);
                    }
                    else if (edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                        isLevel0.insert(eightCornerHashes[5]);
                    }
                    else if (edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[1]);
                    }
                    else if (edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[9] && edgeIsSubdivided[10]) {
                        isLevel0.insert(eightCornerHashes[0]);
                    }
                    else if (edgeIsSubdivided[0] && edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[6]);
                    }
                    else if (edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[5]);
                    }
                    else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[1]);
                    }
                    else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[8] && edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[2]);
                    }
                    else if (edgeIsSubdivided[4] && edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[2]);
                    }
                    else if (edgeIsSubdivided[5] && edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[3]);
                    }
                    else if (edgeIsSubdivided[6] && edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[0]);
                    }
                    else if (edgeIsSubdivided[7] && edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[1]);
                    }
                }
                else if (subdividedEdgeCount == 9) {
                    if (!edgeIsSubdivided[0] && !edgeIsSubdivided[3] && !edgeIsSubdivided[4]) {
                        isLevel0.insert(eightCornerHashes[0]);
                    }
                    else if (!edgeIsSubdivided[0] && !edgeIsSubdivided[1] && !edgeIsSubdivided[5]) {
                        isLevel0.insert(eightCornerHashes[1]);
                    }
                    else if (!edgeIsSubdivided[1] && !edgeIsSubdivided[2] && !edgeIsSubdivided[6]) {
                        isLevel0.insert(eightCornerHashes[2]);
                    }
                    else if (!edgeIsSubdivided[2] && !edgeIsSubdivided[3] && !edgeIsSubdivided[7]) {
                        isLevel0.insert(eightCornerHashes[3]);
                    }
                    else if (!edgeIsSubdivided[4] && !edgeIsSubdivided[8] && !edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[4]);
                    }
                    else if (!edgeIsSubdivided[5] && !edgeIsSubdivided[8] && !edgeIsSubdivided[9]) {
                        isLevel0.insert(eightCornerHashes[5]);
                    }
                    else if (!edgeIsSubdivided[6] && !edgeIsSubdivided[9] && !edgeIsSubdivided[10]) {
                        isLevel0.insert(eightCornerHashes[6]);
                    }
                    else if (!edgeIsSubdivided[7] && !edgeIsSubdivided[10] && !edgeIsSubdivided[11]) {
                        isLevel0.insert(eightCornerHashes[7]);
                    }
                }
            }
        }

        size_t maxRefinedEdgeCount = 0;
        
        for (size_t reversedLevel = 3; reversedLevel <= c.size(); ++reversedLevel) {
            const size_t level = c.size() - reversedLevel;
            std::cout << "refining at tree depth: " << level + 1 << ", cell increase: " << cellIncrease << std::endl;
            if (cellIncrease > 1) {
                cellIncreased = true;
            }
            cellIncrease = 0;
            const auto& cLayer = c[level];

            for (size_t id = 0; id < cLayer.size(); ++id) {
                const auto& cLayerCell = cLayer[id];
                if (cLayerCell.c[0] != cLayerCell.c[1]) {
                    // if the current cell has children

                    bool refineAllChild = false;

                    // check if the center child is illegal
                    const std::array<std::string, 8> eightCornerHashes = {
                        HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][1])),
                        HASHIT(double(cLayerCell.p[0][2]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][1])),
                        HASHIT(double(cLayerCell.p[0][2]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][1])),
                        HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][1])),
                        HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][2])),
                        HASHIT(double(cLayerCell.p[0][2]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][2])),
                        HASHIT(double(cLayerCell.p[0][2]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][2])),
                        HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][2]), double(cLayerCell.p[2][2])) };
                    const std::array<size_t, 8> pointIsLevel1 = {
                        isLevel1.count(eightCornerHashes[0]), isLevel1.count(eightCornerHashes[1]),
                        isLevel1.count(eightCornerHashes[2]), isLevel1.count(eightCornerHashes[3]),
                        isLevel1.count(eightCornerHashes[4]), isLevel1.count(eightCornerHashes[5]),
                        isLevel1.count(eightCornerHashes[6]), isLevel1.count(eightCornerHashes[7]) };
                    const size_t level1PtCount = pointIsLevel1[0] + pointIsLevel1[1] + pointIsLevel1[2] + pointIsLevel1[3] +
                        pointIsLevel1[4] + pointIsLevel1[5] + pointIsLevel1[6] + pointIsLevel1[7];
                    if (level1PtCount == 8) {
                        refineAllChild = true;
                    }
                    else if (level1PtCount == 4) {
                        if (pointIsLevel1[0] && pointIsLevel1[1] && pointIsLevel1[2] && pointIsLevel1[3]) {
                            refineAllChild = true;
                        }
                        else if (pointIsLevel1[0] && pointIsLevel1[1] && pointIsLevel1[4] && pointIsLevel1[5]) {
                            refineAllChild = true;
                        }
                        else if (pointIsLevel1[1] && pointIsLevel1[2] && pointIsLevel1[5] && pointIsLevel1[6]) {
                            refineAllChild = true;
                        }
                        else if (pointIsLevel1[2] && pointIsLevel1[3] && pointIsLevel1[6] && pointIsLevel1[7]) {
                            refineAllChild = true;
                        }
                        else if (pointIsLevel1[0] && pointIsLevel1[3] && pointIsLevel1[4] && pointIsLevel1[7]) {
                            refineAllChild = true;
                        }
                        else if (pointIsLevel1[4] && pointIsLevel1[5] && pointIsLevel1[6] && pointIsLevel1[7]) {
                            refineAllChild = true;
                        }
                    }
                    else if (level1PtCount == 6) {
                        if (!pointIsLevel1[0] && !pointIsLevel1[1]) {
                            refineAllChild = true;
                        }
                        else if (!pointIsLevel1[1] && !pointIsLevel1[2]) {
                            refineAllChild = true;
                        }
                        else if (!pointIsLevel1[2] && !pointIsLevel1[3]) {
                            refineAllChild = true;
                        }
                        else if (!pointIsLevel1[0] && !pointIsLevel1[3]) {
                            refineAllChild = true;
                        }
                        else if (!pointIsLevel1[0] && !pointIsLevel1[4]) {
                            refineAllChild = true;
                        }
                        else if (!pointIsLevel1[1] && !pointIsLevel1[5]) {
                            refineAllChild = true;
                        }
                        else if (!pointIsLevel1[2] && !pointIsLevel1[6]) {
                            refineAllChild = true;
                        }
                        else if (!pointIsLevel1[3] && !pointIsLevel1[7]) {
                            refineAllChild = true;
                        }
                        else if (!pointIsLevel1[4] && !pointIsLevel1[5]) {
                            refineAllChild = true;
                        }
                        else if (!pointIsLevel1[5] && !pointIsLevel1[6]) {
                            refineAllChild = true;
                        }
                        else if (!pointIsLevel1[6] && !pointIsLevel1[7]) {
                            refineAllChild = true;
                        }
                        else if (!pointIsLevel1[4] && !pointIsLevel1[7]) {
                            refineAllChild = true;
                        }
                    }

                    // check all the children of the current cell
                    if (!refineAllChild) {
                        for (const size_t childIdx : cLayerCell.c) {
                            if (c[level + 1][childIdx].c[0] != c[level + 1][childIdx].c[1]) {
                                // if any child has children, then all the other children should have their own children
                                refineAllChild = true;
                                break;
                            }

                            refineAllChild = true;
                            const std::array<std::string, 8> childEightCornerHashes = {
                                HASHIT(double(c[level + 1][childIdx].p[0][0]), double(c[level + 1][childIdx].p[1][0]), double(c[level + 1][childIdx].p[2][0])),
                                HASHIT(double(c[level + 1][childIdx].p[0][3]), double(c[level + 1][childIdx].p[1][0]), double(c[level + 1][childIdx].p[2][0])),
                                HASHIT(double(c[level + 1][childIdx].p[0][3]), double(c[level + 1][childIdx].p[1][3]), double(c[level + 1][childIdx].p[2][0])),
                                HASHIT(double(c[level + 1][childIdx].p[0][0]), double(c[level + 1][childIdx].p[1][3]), double(c[level + 1][childIdx].p[2][0])),
                                HASHIT(double(c[level + 1][childIdx].p[0][0]), double(c[level + 1][childIdx].p[1][0]), double(c[level + 1][childIdx].p[2][3])),
                                HASHIT(double(c[level + 1][childIdx].p[0][3]), double(c[level + 1][childIdx].p[1][0]), double(c[level + 1][childIdx].p[2][3])),
                                HASHIT(double(c[level + 1][childIdx].p[0][3]), double(c[level + 1][childIdx].p[1][3]), double(c[level + 1][childIdx].p[2][3])),
                                HASHIT(double(c[level + 1][childIdx].p[0][0]), double(c[level + 1][childIdx].p[1][3]), double(c[level + 1][childIdx].p[2][3])) };
                            const std::array<bool, 8> pointIsLevel0Or1 = {
                                isLevel0.count(childEightCornerHashes[0]) || isLevel1.count(childEightCornerHashes[0]),
                                isLevel0.count(childEightCornerHashes[1]) || isLevel1.count(childEightCornerHashes[1]),
                                isLevel0.count(childEightCornerHashes[2]) || isLevel1.count(childEightCornerHashes[2]),
                                isLevel0.count(childEightCornerHashes[3]) || isLevel1.count(childEightCornerHashes[3]),
                                isLevel0.count(childEightCornerHashes[4]) || isLevel1.count(childEightCornerHashes[4]),
                                isLevel0.count(childEightCornerHashes[5]) || isLevel1.count(childEightCornerHashes[5]),
                                isLevel0.count(childEightCornerHashes[6]) || isLevel1.count(childEightCornerHashes[6]),
                                isLevel0.count(childEightCornerHashes[7]) || isLevel1.count(childEightCornerHashes[7]) };
                            const size_t level0Or1PtCount = pointIsLevel0Or1[0] + pointIsLevel0Or1[1] + pointIsLevel0Or1[2] +
                                pointIsLevel0Or1[3] + pointIsLevel0Or1[4] + pointIsLevel0Or1[5] + pointIsLevel0Or1[6] + pointIsLevel0Or1[7];
                            if (level0Or1PtCount <= 1) {
                                refineAllChild = false;
                            }
                            else if (level0Or1PtCount == 2) {
                                if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[2]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[3]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[3]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[4]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[5]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[6]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[3] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[4] && pointIsLevel0Or1[5]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[5] && pointIsLevel0Or1[6]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[4] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                            }
                            else if (level0Or1PtCount == 3) {
                                if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1] && pointIsLevel0Or1[2]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[2] && pointIsLevel0Or1[3]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[2] && pointIsLevel0Or1[3]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1] && pointIsLevel0Or1[3]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1] && pointIsLevel0Or1[5]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[4] && pointIsLevel0Or1[5]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[4] && pointIsLevel0Or1[5]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1] && pointIsLevel0Or1[4]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[2] && pointIsLevel0Or1[6]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[5] && pointIsLevel0Or1[6]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[5] && pointIsLevel0Or1[6]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[2] && pointIsLevel0Or1[5]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[3] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[3] && pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[3] && pointIsLevel0Or1[6]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[3] && pointIsLevel0Or1[4]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[4] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[3] && pointIsLevel0Or1[4] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[3] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[4] && pointIsLevel0Or1[5] && pointIsLevel0Or1[6]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[5] && pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[4] && pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[4] && pointIsLevel0Or1[5] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                            }
                            else if (level0Or1PtCount == 4) {
                                if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1] && pointIsLevel0Or1[3] && pointIsLevel0Or1[4]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1] && pointIsLevel0Or1[2] && pointIsLevel0Or1[5]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[2] && pointIsLevel0Or1[3] && pointIsLevel0Or1[6]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[2] && pointIsLevel0Or1[3] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[4] && pointIsLevel0Or1[5] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[4] && pointIsLevel0Or1[5] && pointIsLevel0Or1[6]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[5] && pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                                else if (pointIsLevel0Or1[3] && pointIsLevel0Or1[4] && pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                    refineAllChild = false;
                                }
                            }
                            if (refineAllChild) {
                                maxRefinedEdgeCount = std::max(maxRefinedEdgeCount,
                                    xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[0]].p[0][1]), double(c[level + 1][cLayerCell.c[0]].p[1][0]), double(c[level + 1][cLayerCell.c[0]].p[2][0]))) +
                                    xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[0]].p[0][0]), double(c[level + 1][cLayerCell.c[0]].p[1][1]), double(c[level + 1][cLayerCell.c[0]].p[2][0]))) +
                                    xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[0]].p[0][0]), double(c[level + 1][cLayerCell.c[0]].p[1][0]), double(c[level + 1][cLayerCell.c[0]].p[2][1]))) +
                                    xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[2]].p[0][3]), double(c[level + 1][cLayerCell.c[2]].p[1][1]), double(c[level + 1][cLayerCell.c[2]].p[2][0]))) +
                                    xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[2]].p[0][1]), double(c[level + 1][cLayerCell.c[2]].p[1][3]), double(c[level + 1][cLayerCell.c[2]].p[2][0]))) +
                                    xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[2]].p[0][3]), double(c[level + 1][cLayerCell.c[2]].p[1][3]), double(c[level + 1][cLayerCell.c[2]].p[2][1]))) +
                                    xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[5]].p[0][3]), double(c[level + 1][cLayerCell.c[5]].p[1][0]), double(c[level + 1][cLayerCell.c[5]].p[2][1]))) +
                                    xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[5]].p[0][1]), double(c[level + 1][cLayerCell.c[5]].p[1][0]), double(c[level + 1][cLayerCell.c[5]].p[2][3]))) +
                                    xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[5]].p[0][3]), double(c[level + 1][cLayerCell.c[5]].p[1][1]), double(c[level + 1][cLayerCell.c[5]].p[2][3]))) +
                                    xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[7]].p[0][0]), double(c[level + 1][cLayerCell.c[7]].p[1][3]), double(c[level + 1][cLayerCell.c[7]].p[2][1]))) +
                                    xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[7]].p[0][0]), double(c[level + 1][cLayerCell.c[7]].p[1][1]), double(c[level + 1][cLayerCell.c[7]].p[2][3]))) +
                                    xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[7]].p[0][1]), double(c[level + 1][cLayerCell.c[7]].p[1][3]), double(c[level + 1][cLayerCell.c[7]].p[2][3]))));
                                refineAllChild = false;
                                break;
                            }
                        }
                    }

                    if (!refineAllChild) {
                        continue;
                    }

                    // refine the current cell and update xMap
                    for (const size_t childIdx : cLayerCell.c) {
                        if (c[level + 1][childIdx].c[0] == c[level + 1][childIdx].c[1]) {
                            // the child does not have children
                            // refine the cell
                            RefineCell(level + 1, childIdx);
                            cellIncrease += 26;

                            // for each child, update xMap
                            for (size_t childId = c[level + 2].size() - 27; childId < c[level + 2].size(); ++childId) {
                                const auto& childCell = c[level + 2][childId];

                                for (const auto& cubeCorner : cubeCorners) {
                                    const std::string hash = HASHIT(double(childCell.p[0][cubeCorner[0]]), double(childCell.p[1][cubeCorner[1]]), double(childCell.p[2][cubeCorner[2]]));
                                    if (!xMap.count(hash)) {
                                        xMap[hash] = hX.size();
                                        hX.push_back({ double(childCell.p[0][cubeCorner[0]]), double(childCell.p[1][cubeCorner[1]]), double(childCell.p[2][cubeCorner[2]]) });
                                    }
                                }
                            }
                        }
                    }
                    continue;
                }

                bool refine = false;
                for (const auto& cubeCheckPoint : cubeModeratelyBalancedConditionCheckers) {
                    const std::string hash = HASHIT(
                        double(cLayerCell.p[0][0] / (cubeCheckPoint[0] + cubeCheckPoint[1]) * cubeCheckPoint[0] +
                            cLayerCell.p[0][3] / (cubeCheckPoint[0] + cubeCheckPoint[1]) * cubeCheckPoint[1]),
                        double(cLayerCell.p[1][0] / (cubeCheckPoint[2] + cubeCheckPoint[3]) * cubeCheckPoint[2] +
                            cLayerCell.p[1][3] / (cubeCheckPoint[2] + cubeCheckPoint[3]) * cubeCheckPoint[3]),
                        double(cLayerCell.p[2][0] / (cubeCheckPoint[4] + cubeCheckPoint[5]) * cubeCheckPoint[4] +
                            cLayerCell.p[2][3] / (cubeCheckPoint[4] + cubeCheckPoint[5]) * cubeCheckPoint[5]));
                    if (xMap.count(hash)) {
                        refine = true;
                        break;
                    }
                }
                if (!refine) {
                    continue;
                }

                // refine the cell
                RefineCell(level, id);
                cellIncrease += 26;

                // for each child, update xMap
                for (size_t childId = c[level + 1].size() - 27; childId < c[level + 1].size(); ++childId) {
                    const auto& childCell = c[level + 1][childId];

                    for (const auto& cubeCorner : cubeCorners) {
                        const std::string hash = HASHIT(double(childCell.p[0][cubeCorner[0]]), double(childCell.p[1][cubeCorner[1]]), double(childCell.p[2][cubeCorner[2]]));
                        if (!xMap.count(hash)) {
                            xMap[hash] = hX.size();
                            hX.push_back({ double(childCell.p[0][cubeCorner[0]]), double(childCell.p[1][cubeCorner[1]]), double(childCell.p[2][cubeCorner[2]]) });
                        }
                    }
                }
            }
        }

        std::cout << "-----" << std::endl;

        for (size_t reversedLevel = 3; reversedLevel <= c.size(); ++reversedLevel) {
            const size_t level = c.size() - reversedLevel;
            std::cout << "refining at tree depth: " << level + 1 << ", cell increase: " << cellIncrease << std::endl;
            if (cellIncrease > 1) {
                cellIncreased = true;
            }
            cellIncrease = 0;
            const auto& cLayer = c[level];

            for (size_t id = 0; id < cLayer.size(); ++id) {
                const auto& cLayerCell = cLayer[id];
                if (cLayerCell.c[0] != cLayerCell.c[1]) {
                    // if the current cell has children

                    bool refineAllChild;

                    // check all the children of the current cell
                    for (const size_t childIdx : cLayerCell.c) {

                        refineAllChild = true;
                        const std::array<std::string, 8> childEightCornerHashes = {
                            HASHIT(double(c[level + 1][childIdx].p[0][0]), double(c[level + 1][childIdx].p[1][0]), double(c[level + 1][childIdx].p[2][0])),
                            HASHIT(double(c[level + 1][childIdx].p[0][3]), double(c[level + 1][childIdx].p[1][0]), double(c[level + 1][childIdx].p[2][0])),
                            HASHIT(double(c[level + 1][childIdx].p[0][3]), double(c[level + 1][childIdx].p[1][3]), double(c[level + 1][childIdx].p[2][0])),
                            HASHIT(double(c[level + 1][childIdx].p[0][0]), double(c[level + 1][childIdx].p[1][3]), double(c[level + 1][childIdx].p[2][0])),
                            HASHIT(double(c[level + 1][childIdx].p[0][0]), double(c[level + 1][childIdx].p[1][0]), double(c[level + 1][childIdx].p[2][3])),
                            HASHIT(double(c[level + 1][childIdx].p[0][3]), double(c[level + 1][childIdx].p[1][0]), double(c[level + 1][childIdx].p[2][3])),
                            HASHIT(double(c[level + 1][childIdx].p[0][3]), double(c[level + 1][childIdx].p[1][3]), double(c[level + 1][childIdx].p[2][3])),
                            HASHIT(double(c[level + 1][childIdx].p[0][0]), double(c[level + 1][childIdx].p[1][3]), double(c[level + 1][childIdx].p[2][3])) };
                        const std::array<bool, 8> pointIsLevel0Or1 = {
                            isLevel0.count(childEightCornerHashes[0]) || isLevel1.count(childEightCornerHashes[0]),
                            isLevel0.count(childEightCornerHashes[1]) || isLevel1.count(childEightCornerHashes[1]),
                            isLevel0.count(childEightCornerHashes[2]) || isLevel1.count(childEightCornerHashes[2]),
                            isLevel0.count(childEightCornerHashes[3]) || isLevel1.count(childEightCornerHashes[3]),
                            isLevel0.count(childEightCornerHashes[4]) || isLevel1.count(childEightCornerHashes[4]),
                            isLevel0.count(childEightCornerHashes[5]) || isLevel1.count(childEightCornerHashes[5]),
                            isLevel0.count(childEightCornerHashes[6]) || isLevel1.count(childEightCornerHashes[6]),
                            isLevel0.count(childEightCornerHashes[7]) || isLevel1.count(childEightCornerHashes[7]) };
                        const size_t level0Or1PtCount = pointIsLevel0Or1[0] + pointIsLevel0Or1[1] + pointIsLevel0Or1[2] +
                            pointIsLevel0Or1[3] + pointIsLevel0Or1[4] + pointIsLevel0Or1[5] + pointIsLevel0Or1[6] + pointIsLevel0Or1[7];
                        if (level0Or1PtCount <= 1) {
                            refineAllChild = false;
                        }
                        else if (level0Or1PtCount == 2) {
                            if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[2]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[3]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[3]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[4]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[5]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[6]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[3] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[4] && pointIsLevel0Or1[5]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[5] && pointIsLevel0Or1[6]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[4] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                        }
                        else if (level0Or1PtCount == 3) {
                            if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1] && pointIsLevel0Or1[2]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[2] && pointIsLevel0Or1[3]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[2] && pointIsLevel0Or1[3]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1] && pointIsLevel0Or1[3]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1] && pointIsLevel0Or1[5]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[4] && pointIsLevel0Or1[5]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[4] && pointIsLevel0Or1[5]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1] && pointIsLevel0Or1[4]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[2] && pointIsLevel0Or1[6]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[5] && pointIsLevel0Or1[6]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[5] && pointIsLevel0Or1[6]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[2] && pointIsLevel0Or1[5]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[3] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[3] && pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[3] && pointIsLevel0Or1[6]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[3] && pointIsLevel0Or1[4]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[4] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[3] && pointIsLevel0Or1[4] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[3] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[4] && pointIsLevel0Or1[5] && pointIsLevel0Or1[6]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[5] && pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[4] && pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[4] && pointIsLevel0Or1[5] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                        }
                        else if (level0Or1PtCount == 4) {
                            if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1] && pointIsLevel0Or1[3] && pointIsLevel0Or1[4]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[1] && pointIsLevel0Or1[2] && pointIsLevel0Or1[5]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[2] && pointIsLevel0Or1[3] && pointIsLevel0Or1[6]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[2] && pointIsLevel0Or1[3] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[0] && pointIsLevel0Or1[4] && pointIsLevel0Or1[5] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[1] && pointIsLevel0Or1[4] && pointIsLevel0Or1[5] && pointIsLevel0Or1[6]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[2] && pointIsLevel0Or1[5] && pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                            else if (pointIsLevel0Or1[3] && pointIsLevel0Or1[4] && pointIsLevel0Or1[6] && pointIsLevel0Or1[7]) {
                                refineAllChild = false;
                            }
                        }
                        if (refineAllChild) {
                            const size_t refinedEdgeCount =
                                xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[0]].p[0][1]), double(c[level + 1][cLayerCell.c[0]].p[1][0]), double(c[level + 1][cLayerCell.c[0]].p[2][0]))) +
                                xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[0]].p[0][0]), double(c[level + 1][cLayerCell.c[0]].p[1][1]), double(c[level + 1][cLayerCell.c[0]].p[2][0]))) +
                                xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[0]].p[0][0]), double(c[level + 1][cLayerCell.c[0]].p[1][0]), double(c[level + 1][cLayerCell.c[0]].p[2][1]))) +
                                xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[2]].p[0][3]), double(c[level + 1][cLayerCell.c[2]].p[1][1]), double(c[level + 1][cLayerCell.c[2]].p[2][0]))) +
                                xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[2]].p[0][1]), double(c[level + 1][cLayerCell.c[2]].p[1][3]), double(c[level + 1][cLayerCell.c[2]].p[2][0]))) +
                                xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[2]].p[0][3]), double(c[level + 1][cLayerCell.c[2]].p[1][3]), double(c[level + 1][cLayerCell.c[2]].p[2][1]))) +
                                xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[5]].p[0][3]), double(c[level + 1][cLayerCell.c[5]].p[1][0]), double(c[level + 1][cLayerCell.c[5]].p[2][1]))) +
                                xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[5]].p[0][1]), double(c[level + 1][cLayerCell.c[5]].p[1][0]), double(c[level + 1][cLayerCell.c[5]].p[2][3]))) +
                                xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[5]].p[0][3]), double(c[level + 1][cLayerCell.c[5]].p[1][1]), double(c[level + 1][cLayerCell.c[5]].p[2][3]))) +
                                xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[7]].p[0][0]), double(c[level + 1][cLayerCell.c[7]].p[1][3]), double(c[level + 1][cLayerCell.c[7]].p[2][1]))) +
                                xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[7]].p[0][0]), double(c[level + 1][cLayerCell.c[7]].p[1][1]), double(c[level + 1][cLayerCell.c[7]].p[2][3]))) +
                                xMap.count(HASHIT(double(c[level + 1][cLayerCell.c[7]].p[0][1]), double(c[level + 1][cLayerCell.c[7]].p[1][3]), double(c[level + 1][cLayerCell.c[7]].p[2][3])));
                            if (refinedEdgeCount == maxRefinedEdgeCount) {
                                break;
                            }
                            refineAllChild = false;
                        }
                    }

                    if (!refineAllChild) {
                        continue;
                    }

                    // refine the current cell and update xMap
                    for (const size_t childIdx : cLayerCell.c) {
                        if (c[level + 1][childIdx].c[0] == c[level + 1][childIdx].c[1]) {
                            // the child does not have children
                            // refine the cell
                            RefineCell(level + 1, childIdx);
                            cellIncrease += 26;

                            // for each child, update xMap
                            for (size_t childId = c[level + 2].size() - 27; childId < c[level + 2].size(); ++childId) {
                                const auto& childCell = c[level + 2][childId];

                                for (const auto& cubeCorner : cubeCorners) {
                                    const std::string hash = HASHIT(double(childCell.p[0][cubeCorner[0]]), double(childCell.p[1][cubeCorner[1]]), double(childCell.p[2][cubeCorner[2]]));
                                    if (!xMap.count(hash)) {
                                        xMap[hash] = hX.size();
                                        hX.push_back({ double(childCell.p[0][cubeCorner[0]]), double(childCell.p[1][cubeCorner[1]]), double(childCell.p[2][cubeCorner[2]]) });
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        std::cout << "-----" << std::endl;
    }

    // record the arrange of decoupling nodes
    std::unordered_map<size_t, size_t> level0Decouple;
    std::unordered_map<size_t, size_t> level1Decouple;
    std::unordered_map<std::string, size_t> level1TwinDecouple;
    auto insertSizetKey = [&](std::unordered_map<size_t, size_t>& dictionary, const size_t key) {
        if (!dictionary.count(key)) {
            const size_t dictionarySize = dictionary.size();
            dictionary[key] = dictionarySize;
        }
        };
    auto insertStringKey = [&](std::unordered_map<std::string, size_t>& dictionary, const std::string key) {
        if (!dictionary.count(key)) {
            const size_t dictionarySize = dictionary.size();
            dictionary[key] = dictionarySize;
        }
        };
    
    // level0/level1 node decoupling
    for (size_t level = 0; level < c.size() - 1; ++level) {
        const auto& cLayer = c[level];
        for (size_t cLayerIdx = 0; cLayerIdx < cLayer.size(); ++cLayerIdx) {
            const auto& cLayerCell = cLayer[cLayerIdx];
            if (cLayerCell.c[0] != cLayerCell.c[1]) { // skip if the current cell has children
                continue;
            }

            // eight corner coordinates in double
            std::array<std::array<double, 3>, 8> corners;
            for (size_t i = 0; i < 8; ++i) {
                for (size_t j = 0; j < 3; ++j) {
                    corners[i][j] = double(cLayerCell.p[j][cubeCorners[i][j]]);
                }
            }

            const std::array<size_t, 12> edgeIsSubdivided = {
                xMap.count(HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][0]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][0]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][0]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][0]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][1]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][1]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][1]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][1]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][3]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][3]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][3]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][3]))) };
            const size_t subdividedEdgeCount = edgeIsSubdivided[0] + edgeIsSubdivided[1] + edgeIsSubdivided[2] + edgeIsSubdivided[3] +
                edgeIsSubdivided[4] + edgeIsSubdivided[5] + edgeIsSubdivided[6] + edgeIsSubdivided[7] +
                edgeIsSubdivided[8] + edgeIsSubdivided[9] + edgeIsSubdivided[10] + edgeIsSubdivided[11];
            
            // 3E
            if (subdividedEdgeCount == 3) {
                if (edgeIsSubdivided[0] && edgeIsSubdivided[3] && edgeIsSubdivided[4]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[5]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[6]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[7]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[4] && edgeIsSubdivided[8] && edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[5] && edgeIsSubdivided[8] && edgeIsSubdivided[9]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[6] && edgeIsSubdivided[9] && edgeIsSubdivided[10]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    continue;
                }
                else if (edgeIsSubdivided[7] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    continue;
                }
            }
            // 5E
            else if (subdividedEdgeCount == 5) {
                if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[4]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[5]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[6]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[7]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8] && edgeIsSubdivided[9]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8] && edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    continue;
                }
                else if (edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9] && edgeIsSubdivided[10]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    continue;
                }
                else if (edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[8] && edgeIsSubdivided[9]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    continue;
                }
                else if (edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    continue;
                }
                else if (edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    continue;
                }
                else if (edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[9] && edgeIsSubdivided[10]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    continue;
                }
                else if (edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    continue;
                }
                else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[8] && edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[4] && edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    continue;
                }
                else if (edgeIsSubdivided[5] && edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    continue;
                }
                else if (edgeIsSubdivided[6] && edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    continue;
                }
                else if (edgeIsSubdivided[7] && edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    continue;
                }
            }
            // 9E
            else if (subdividedEdgeCount == 9) {
                if (!edgeIsSubdivided[0] && !edgeIsSubdivided[3] && !edgeIsSubdivided[4]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    continue;
                }
                else if (!edgeIsSubdivided[0] && !edgeIsSubdivided[1] && !edgeIsSubdivided[5]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    continue;
                }
                else if (!edgeIsSubdivided[1] && !edgeIsSubdivided[2] && !edgeIsSubdivided[6]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    continue;
                }
                else if (!edgeIsSubdivided[2] && !edgeIsSubdivided[3] && !edgeIsSubdivided[7]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    continue;
                }
                else if (!edgeIsSubdivided[4] && !edgeIsSubdivided[8] && !edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    continue;
                }
                else if (!edgeIsSubdivided[5] && !edgeIsSubdivided[8] && !edgeIsSubdivided[9]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    continue;
                }
                else if (!edgeIsSubdivided[6] && !edgeIsSubdivided[9] && !edgeIsSubdivided[10]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (!edgeIsSubdivided[7] && !edgeIsSubdivided[10] && !edgeIsSubdivided[11]) {
                    insertSizetKey(level0Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
            }
            // 2E
            else if (subdividedEdgeCount == 2) {
                if (edgeIsSubdivided[0] && edgeIsSubdivided[1]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    continue;
                }
                else if (edgeIsSubdivided[1] && edgeIsSubdivided[2]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    continue;
                }
                else if (edgeIsSubdivided[2] && edgeIsSubdivided[3]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[3]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[5]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    continue;
                }
                else if (edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    continue;
                }
                else if (edgeIsSubdivided[4] && edgeIsSubdivided[8]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[4]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    continue;
                }
                else if (edgeIsSubdivided[1] && edgeIsSubdivided[6]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    continue;
                }
                else if (edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    continue;
                }
                else if (edgeIsSubdivided[5] && edgeIsSubdivided[9]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    continue;
                }
                else if (edgeIsSubdivided[1] && edgeIsSubdivided[5]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[2] && edgeIsSubdivided[6]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[6] && edgeIsSubdivided[10]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    continue;
                }
                else if (edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    continue;
                }
                else if (edgeIsSubdivided[2] && edgeIsSubdivided[7]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[3] && edgeIsSubdivided[7]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    continue;
                }
                else if (edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    continue;
                }
                else if (edgeIsSubdivided[4] && edgeIsSubdivided[11]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    continue;
                }
                else if (edgeIsSubdivided[3] && edgeIsSubdivided[4]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[8] && edgeIsSubdivided[9]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[9] && edgeIsSubdivided[10]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    continue;
                }
                else if (edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    continue;
                }
                else if (edgeIsSubdivided[8] && edgeIsSubdivided[11]) {
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
            }
            // 7E
            else if (subdividedEdgeCount == 7) {
                if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] &&
                    edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                    insertStringKey(level1TwinDecouple, SORTHASHIT2(xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])),
                        xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2]))));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] &&
                    edgeIsSubdivided[3] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                    insertStringKey(level1TwinDecouple, SORTHASHIT2(xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])),
                        xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2]))));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));

                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] &&
                    edgeIsSubdivided[3] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                    insertStringKey(level1TwinDecouple, SORTHASHIT2(xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])),
                        xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2]))));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] &&
                    edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                    insertStringKey(level1TwinDecouple, SORTHASHIT2(xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])),
                        xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2]))));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[3] && edgeIsSubdivided[4] &&
                    edgeIsSubdivided[5] && edgeIsSubdivided[7] && edgeIsSubdivided[8] && edgeIsSubdivided[11]) {
                    insertStringKey(level1TwinDecouple, SORTHASHIT2(xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])),
                        xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2]))));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[6][0], corners[6][1], corners[6][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[4] &&
                    edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[8] && edgeIsSubdivided[9]) {
                    insertStringKey(level1TwinDecouple, SORTHASHIT2(xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])),
                        xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2]))));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[7][0], corners[7][1], corners[7][2])));
                    continue;
                }
                else if (edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[5] &&
                    edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[9] && edgeIsSubdivided[10]) {
                    insertStringKey(level1TwinDecouple, SORTHASHIT2(xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])),
                        xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2]))));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[4][0], corners[4][1], corners[4][2])));
                    continue;
                }
                else if (edgeIsSubdivided[2] && edgeIsSubdivided[3] && edgeIsSubdivided[4] &&
                    edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertStringKey(level1TwinDecouple, SORTHASHIT2(xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])),
                        xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2]))));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[5][0], corners[5][1], corners[5][2])));
                    continue;
                }
                else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[5] &&
                    edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertStringKey(level1TwinDecouple, SORTHASHIT2(xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])),
                        xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2]))));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    continue;
                }
                else if (edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[6] &&
                    edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertStringKey(level1TwinDecouple, SORTHASHIT2(xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])),
                        xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2]))));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[3][0], corners[3][1], corners[3][2])));
                    continue;
                }
                else if (edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7] &&
                    edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertStringKey(level1TwinDecouple, SORTHASHIT2(xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])),
                        xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2]))));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[0][0], corners[0][1], corners[0][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    continue;
                }
                else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] &&
                    edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                    insertStringKey(level1TwinDecouple, SORTHASHIT2(xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])),
                        xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2]))));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[1][0], corners[1][1], corners[1][2])));
                    insertSizetKey(level1Decouple, xMap.at(HASHIT(corners[2][0], corners[2][1], corners[2][2])));
                    continue;
                }
            }
        }
    }

    // constants for constructing oneBlackEdge and fourBlackEdges
    const int bottomCenterThickness = 10;
    const std::array<int, 3> threeWeights = { 150 + bottomCenterThickness, 100, 50 - bottomCenterThickness };
    const std::array<int, 3> fourWeights = { 100 - 2 * bottomCenterThickness, 100, 2 * bottomCenterThickness };

    // add a point to hX and xMap
    auto addPoint = [&](const std::array<double, 3>& point) -> size_t {
        std::string hash = HASHIT(point[0], point[1], point[2]);
        if (!xMap.count(hash)) {
            xMap[hash] = hX.size();
            hX.emplace_back(point);
        }
        return xMap.at(hash);
        };

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

    // assign no-black-edge template
    // 000000000000
    auto noBlackEdge = [&](const std::array<std::array<double, 3>, 8>& positions) {
        hE.push_back({ addPoint(positions[0]), addPoint(positions[1]), addPoint(positions[2]), addPoint(positions[3]),
            addPoint(positions[4]), addPoint(positions[5]), addPoint(positions[6]), addPoint(positions[7]) });
        };

    // assign one-black-edge template
    // 000100000000
    auto oneBlackEdge = [&](const std::array<std::array<double, 3>, 8>& positions) {
        std::array<size_t, 16> ptsIdx;

        // 0-7
        for (size_t j = 0; j < 8; ++j) {
            ptsIdx[j] = addPoint(positions[j]);
        }
        // 8-15
        ptsIdx[8] = addPoint(interpolatePoint(positions[0], 2, positions[3], 1));
        ptsIdx[9] = addPoint(interpolate3Points(positions[1], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
        ptsIdx[10] = addPoint(interpolate3Points(positions[2], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
        ptsIdx[11] = addPoint(interpolatePoint(positions[3], 2, positions[0], 1));
        ptsIdx[12] = addPoint(interpolate3Points(positions[4], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
        ptsIdx[13] = addPoint(interpolate3Points(positions[5], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
        ptsIdx[14] = addPoint(interpolate3Points(positions[6], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
        ptsIdx[15] = addPoint(interpolate3Points(positions[7], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
        // 5 hex elements
        hE.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[9], ptsIdx[8], ptsIdx[4], ptsIdx[5], ptsIdx[13], ptsIdx[12] });
        hE.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
        hE.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[2], ptsIdx[10], ptsIdx[13], ptsIdx[5], ptsIdx[6], ptsIdx[14] });
        hE.push_back({ ptsIdx[11], ptsIdx[10], ptsIdx[2], ptsIdx[3], ptsIdx[15], ptsIdx[14], ptsIdx[6], ptsIdx[7] });
        hE.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
        };

    // assign four-black-edge template
    // 111100000000
    auto fourBlackEdges = [&](const std::array<std::array<double, 3>, 8>& positions) {
        std::array<size_t, 32> ptsIdx;
        std::string hash;

        // 0-7
        for (size_t j = 0; j < 8; ++j) {
            ptsIdx[j] = addPoint(positions[j]);
        }
        // 8-31
        ptsIdx[8] = addPoint(interpolatePoint(positions[0], 2, positions[1], 1));
        ptsIdx[9] = addPoint(interpolatePoint(positions[1], 2, positions[0], 1));
        ptsIdx[10] = addPoint(interpolatePoint(positions[0], 2, positions[3], 1));
        ptsIdx[11] = addPoint(interpolatePoint(positions[0], 2, positions[2], 1));
        ptsIdx[12] = addPoint(interpolatePoint(positions[1], 2, positions[3], 1));
        ptsIdx[13] = addPoint(interpolatePoint(positions[1], 2, positions[2], 1));
        ptsIdx[14] = addPoint(interpolatePoint(positions[3], 2, positions[0], 1));
        ptsIdx[15] = addPoint(interpolatePoint(positions[3], 2, positions[1], 1));
        ptsIdx[16] = addPoint(interpolatePoint(positions[2], 2, positions[0], 1));
        ptsIdx[17] = addPoint(interpolatePoint(positions[2], 2, positions[1], 1));
        ptsIdx[18] = addPoint(interpolatePoint(positions[3], 2, positions[2], 1));
        ptsIdx[19] = addPoint(interpolatePoint(positions[2], 2, positions[3], 1));
        ptsIdx[20] = addPoint(interpolate4Points(positions[0], fourWeights[0], positions[1], fourWeights[1], positions[3], fourWeights[1], positions[4], fourWeights[2]));
        ptsIdx[21] = addPoint(interpolate4Points(positions[1], fourWeights[0], positions[0], fourWeights[1], positions[2], fourWeights[1], positions[5], fourWeights[2]));
        ptsIdx[22] = addPoint(interpolate4Points(positions[3], fourWeights[0], positions[0], fourWeights[1], positions[2], fourWeights[1], positions[7], fourWeights[2]));
        ptsIdx[23] = addPoint(interpolate4Points(positions[2], fourWeights[0], positions[1], fourWeights[1], positions[3], fourWeights[1], positions[6], fourWeights[2]));
        ptsIdx[24] = addPoint(interpolate3Points(positions[4], threeWeights[0], positions[1], threeWeights[1], positions[0], threeWeights[2]));
        ptsIdx[25] = addPoint(interpolate3Points(positions[5], threeWeights[0], positions[0], threeWeights[1], positions[1], threeWeights[2]));
        ptsIdx[26] = addPoint(interpolate3Points(positions[5], threeWeights[0], positions[2], threeWeights[1], positions[1], threeWeights[2]));
        ptsIdx[27] = addPoint(interpolate3Points(positions[6], threeWeights[0], positions[1], threeWeights[1], positions[2], threeWeights[2]));
        ptsIdx[28] = addPoint(interpolate3Points(positions[6], threeWeights[0], positions[3], threeWeights[1], positions[2], threeWeights[2]));
        ptsIdx[29] = addPoint(interpolate3Points(positions[7], threeWeights[0], positions[2], threeWeights[1], positions[3], threeWeights[2]));
        ptsIdx[30] = addPoint(interpolate3Points(positions[7], threeWeights[0], positions[0], threeWeights[1], positions[3], threeWeights[2]));
        ptsIdx[31] = addPoint(interpolate3Points(positions[4], threeWeights[0], positions[3], threeWeights[1], positions[0], threeWeights[2]));
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

    // recursively splite the given hex element
    auto recursiveSplit = [&](auto&& self, const std::array<std::array<double, 3>, 8>& positions, const std::array<size_t, 12>& edgeIsSubdivided) -> void {
        const std::array<size_t, 8> eightCornerIdx = {
            xMap.at(HASHIT(positions[0][0], positions[0][1], positions[0][2])),
            xMap.at(HASHIT(positions[1][0], positions[1][1], positions[1][2])),
            xMap.at(HASHIT(positions[2][0], positions[2][1], positions[2][2])),
            xMap.at(HASHIT(positions[3][0], positions[3][1], positions[3][2])),
            xMap.at(HASHIT(positions[4][0], positions[4][1], positions[4][2])),
            xMap.at(HASHIT(positions[5][0], positions[5][1], positions[5][2])),
            xMap.at(HASHIT(positions[6][0], positions[6][1], positions[6][2])),
            xMap.at(HASHIT(positions[7][0], positions[7][1], positions[7][2])) };
        const std::array<std::string, 12> twelveEdgeHash = {
            SORTHASHIT2(eightCornerIdx[0], eightCornerIdx[1]),
            SORTHASHIT2(eightCornerIdx[1], eightCornerIdx[2]),
            SORTHASHIT2(eightCornerIdx[2], eightCornerIdx[3]),
            SORTHASHIT2(eightCornerIdx[0], eightCornerIdx[3]),
            SORTHASHIT2(eightCornerIdx[0], eightCornerIdx[4]),
            SORTHASHIT2(eightCornerIdx[1], eightCornerIdx[5]),
            SORTHASHIT2(eightCornerIdx[2], eightCornerIdx[6]),
            SORTHASHIT2(eightCornerIdx[3], eightCornerIdx[7]),
            SORTHASHIT2(eightCornerIdx[4], eightCornerIdx[5]),
            SORTHASHIT2(eightCornerIdx[5], eightCornerIdx[6]),
            SORTHASHIT2(eightCornerIdx[6], eightCornerIdx[7]),
            SORTHASHIT2(eightCornerIdx[4], eightCornerIdx[7]) };
        const size_t subdividedEdgeCount = edgeIsSubdivided[0] + edgeIsSubdivided[1] + edgeIsSubdivided[2] + edgeIsSubdivided[3] +
            edgeIsSubdivided[4] + edgeIsSubdivided[5] + edgeIsSubdivided[6] + edgeIsSubdivided[7] +
            edgeIsSubdivided[8] + edgeIsSubdivided[9] + edgeIsSubdivided[10] + edgeIsSubdivided[11];

        std::array<size_t, 8> level0Idx, level1Idx;
        std::array<size_t, 12> level1TwinIdx;
        for (size_t i = 0; i < 8; ++i) {
            level0Idx[i] = (level0Decouple.count(eightCornerIdx[i]) ? level0Decouple.at(eightCornerIdx[i]) : level0Decouple.size());
            level1Idx[i] = (level1Decouple.count(eightCornerIdx[i]) ? level1Decouple.at(eightCornerIdx[i]) : level1Decouple.size());
        }
        for (size_t i = 0; i < 12; ++i) {
            level1TwinIdx[i] = (level1TwinDecouple.count(twelveEdgeHash[i]) ? level1TwinDecouple.at(twelveEdgeHash[i]) : level1TwinDecouple.size());
        }
        // find the level0 point with smallest build index
        size_t minLevel0Idx = level0Decouple.size(), minLevel0IdxPos = 8;
        for (size_t i = 0; i < 8; ++i) {
            if (minLevel0Idx > level0Idx[i]) {
                minLevel0Idx = level0Idx[i];
                minLevel0IdxPos = i;
            }
        }
        // find the level1 point with smallest build index
        size_t minLevel1Idx = level1Decouple.size(), minLevel1IdxPos = 8;
        for (size_t i = 0; i < 8; ++i) {
            if (minLevel1Idx > level1Idx[i]) {
                minLevel1Idx = level1Idx[i];
                minLevel1IdxPos = i;
            }
        }
        // find the level1 edge with smallest build index
        size_t minLevel1TwinIdx = level1TwinDecouple.size(), minLevel1TwinIdxPos = 12;
        for (size_t i = 0; i < 12; ++i) {
            if (minLevel1TwinIdx > level1TwinIdx[i]) {
                minLevel1TwinIdx = level1TwinIdx[i];
                minLevel1TwinIdxPos = i;
            }
        }
        
        if (minLevel0IdxPos < 8) {// split at the level0 point
            std::array<std::array<double, 3>, 8> rotatedPositions;
            std::array<size_t, 12> rotatedEdgeIsSubdivided;
            if (minLevel0IdxPos == 0) {
                rotatedPositions = positions;
                rotatedEdgeIsSubdivided = edgeIsSubdivided;
            }
            else if (minLevel0IdxPos == 1) {
                rotatedPositions = { positions[1], positions[2], positions[3], positions[0], positions[5], positions[6], positions[7], positions[4] };
                rotatedEdgeIsSubdivided = { edgeIsSubdivided[1], edgeIsSubdivided[2], edgeIsSubdivided[3], edgeIsSubdivided[0],
                    edgeIsSubdivided[5], edgeIsSubdivided[6], edgeIsSubdivided[7], edgeIsSubdivided[4],
                    edgeIsSubdivided[9], edgeIsSubdivided[10], edgeIsSubdivided[11], edgeIsSubdivided[8] };
            }
            else if (minLevel0IdxPos == 2) {
                rotatedPositions = { positions[2], positions[3], positions[0], positions[1], positions[6], positions[7], positions[4], positions[5] };
                rotatedEdgeIsSubdivided = { edgeIsSubdivided[2], edgeIsSubdivided[3], edgeIsSubdivided[0], edgeIsSubdivided[1],
                    edgeIsSubdivided[6], edgeIsSubdivided[7], edgeIsSubdivided[4], edgeIsSubdivided[5],
                    edgeIsSubdivided[10], edgeIsSubdivided[11], edgeIsSubdivided[8], edgeIsSubdivided[9] };
            }
            else if (minLevel0IdxPos == 3) {
                rotatedPositions = { positions[3], positions[0], positions[1], positions[2], positions[7], positions[4], positions[5], positions[6] };
                rotatedEdgeIsSubdivided = { edgeIsSubdivided[3], edgeIsSubdivided[0], edgeIsSubdivided[1], edgeIsSubdivided[2],
                    edgeIsSubdivided[7], edgeIsSubdivided[4], edgeIsSubdivided[5], edgeIsSubdivided[6],
                    edgeIsSubdivided[11], edgeIsSubdivided[8], edgeIsSubdivided[9], edgeIsSubdivided[10] };
            }
            else if (minLevel0IdxPos == 4) {
                rotatedPositions = { positions[4], positions[5], positions[1], positions[0], positions[7], positions[6], positions[2], positions[3] };
                rotatedEdgeIsSubdivided = { edgeIsSubdivided[8], edgeIsSubdivided[5], edgeIsSubdivided[0], edgeIsSubdivided[4],
                    edgeIsSubdivided[11], edgeIsSubdivided[9], edgeIsSubdivided[1], edgeIsSubdivided[3],
                    edgeIsSubdivided[10], edgeIsSubdivided[6], edgeIsSubdivided[2], edgeIsSubdivided[7] };
            }
            else if (minLevel0IdxPos == 5) {
                rotatedPositions = { positions[5], positions[6], positions[2], positions[1], positions[4], positions[7], positions[3], positions[0] };
                rotatedEdgeIsSubdivided = { edgeIsSubdivided[9], edgeIsSubdivided[6], edgeIsSubdivided[1], edgeIsSubdivided[5],
                    edgeIsSubdivided[8], edgeIsSubdivided[10], edgeIsSubdivided[2], edgeIsSubdivided[0],
                    edgeIsSubdivided[11], edgeIsSubdivided[7], edgeIsSubdivided[3], edgeIsSubdivided[4] };
            }
            else if (minLevel0IdxPos == 6) {
                rotatedPositions = { positions[6], positions[2], positions[1], positions[5], positions[7], positions[3], positions[0], positions[4] };
                rotatedEdgeIsSubdivided = { edgeIsSubdivided[6], edgeIsSubdivided[1], edgeIsSubdivided[5], edgeIsSubdivided[9],
                    edgeIsSubdivided[10], edgeIsSubdivided[2], edgeIsSubdivided[0], edgeIsSubdivided[8],
                    edgeIsSubdivided[7], edgeIsSubdivided[3], edgeIsSubdivided[4], edgeIsSubdivided[11] };
            }
            else if (minLevel0IdxPos == 7) {
                rotatedPositions = { positions[7], positions[3], positions[2], positions[6], positions[4], positions[0], positions[1], positions[5] };
                rotatedEdgeIsSubdivided = { edgeIsSubdivided[7], edgeIsSubdivided[2], edgeIsSubdivided[6], edgeIsSubdivided[10],
                    edgeIsSubdivided[11], edgeIsSubdivided[3], edgeIsSubdivided[1], edgeIsSubdivided[9],
                    edgeIsSubdivided[4], edgeIsSubdivided[0], edgeIsSubdivided[5], edgeIsSubdivided[8] };
            }

            std::array<size_t, 15> ptsIdx;

            // 0-7
            for (size_t j = 0; j < 8; ++j) {
                ptsIdx[j] = addPoint(rotatedPositions[j]);
            }
            // 8-14
            const std::array<std::array<double, 3>, 7> newPts = {
                interpolatePoint(rotatedPositions[0], 1, rotatedPositions[1], 2),
                interpolatePoint(rotatedPositions[0], 1, rotatedPositions[3], 2),
                interpolatePoint(rotatedPositions[0], 1, rotatedPositions[2], 2),
                interpolatePoint(rotatedPositions[0], 1, rotatedPositions[4], 2),
                interpolatePoint(rotatedPositions[0], 1, rotatedPositions[5], 2),
                interpolatePoint(rotatedPositions[0], 1, rotatedPositions[7], 2),
                interpolatePoint(rotatedPositions[0], 1, rotatedPositions[6], 2) };
            for (size_t j = 8; j < 15; ++j) {
                ptsIdx[j] = addPoint(newPts[j - 8]);
            }
            // 4 hex elements
            hE.push_back({ ptsIdx[0], ptsIdx[8], ptsIdx[10], ptsIdx[9], ptsIdx[11], ptsIdx[12], ptsIdx[14], ptsIdx[13] });
            std::array<std::array<double, 3>, 8> tmpPositions =
            { newPts[0], rotatedPositions[1], rotatedPositions[2], newPts[2], newPts[4], rotatedPositions[5], rotatedPositions[6], newPts[6] };
            std::array<size_t, 12> tmpEdgeIsSubdivided =
            { 0, rotatedEdgeIsSubdivided[1], 0, 0, 0, rotatedEdgeIsSubdivided[5], rotatedEdgeIsSubdivided[6], 0, 0, rotatedEdgeIsSubdivided[9], 0, 0 };
            self(self, tmpPositions, tmpEdgeIsSubdivided);
            tmpPositions = { newPts[1], newPts[2], rotatedPositions[2], rotatedPositions[3], newPts[5], newPts[6], rotatedPositions[6], rotatedPositions[7] };
            tmpEdgeIsSubdivided = { 0, 0, rotatedEdgeIsSubdivided[2], 0, 0, 0, rotatedEdgeIsSubdivided[6], rotatedEdgeIsSubdivided[7], 0, 0, rotatedEdgeIsSubdivided[10], 0 };
            self(self, tmpPositions, tmpEdgeIsSubdivided);
            tmpPositions = { newPts[3], newPts[4], newPts[6], newPts[5], rotatedPositions[4], rotatedPositions[5], rotatedPositions[6], rotatedPositions[7] };
            tmpEdgeIsSubdivided = { 0, 0, 0, 0, 0, 0, 0, 0, rotatedEdgeIsSubdivided[8], rotatedEdgeIsSubdivided[9], rotatedEdgeIsSubdivided[10], rotatedEdgeIsSubdivided[11] };
            self(self, tmpPositions, tmpEdgeIsSubdivided);
        }
        else {// no level0 point exist in the hex
            if (minLevel1TwinIdxPos < 12) {// split at the level1 edge
                std::array<std::array<double, 3>, 8> rotatedPositions;
                std::array<size_t, 12> rotatedEdgeIsSubdivided;
                if (minLevel1TwinIdxPos == 0) {
                    rotatedPositions = positions;
                    rotatedEdgeIsSubdivided = edgeIsSubdivided;
                }
                else if (minLevel1TwinIdxPos == 1) {
                    rotatedPositions = { positions[1], positions[2], positions[3], positions[0], positions[5], positions[6], positions[7], positions[4] };
                    rotatedEdgeIsSubdivided = { edgeIsSubdivided[1], edgeIsSubdivided[2], edgeIsSubdivided[3], edgeIsSubdivided[0],
                        edgeIsSubdivided[5], edgeIsSubdivided[6], edgeIsSubdivided[7], edgeIsSubdivided[4],
                        edgeIsSubdivided[9], edgeIsSubdivided[10], edgeIsSubdivided[11], edgeIsSubdivided[8] };
                }
                else if (minLevel1TwinIdxPos == 2) {
                    rotatedPositions = { positions[2], positions[3], positions[0], positions[1], positions[6], positions[7], positions[4], positions[5] };
                    rotatedEdgeIsSubdivided = { edgeIsSubdivided[2], edgeIsSubdivided[3], edgeIsSubdivided[0], edgeIsSubdivided[1],
                        edgeIsSubdivided[6], edgeIsSubdivided[7], edgeIsSubdivided[4], edgeIsSubdivided[5],
                        edgeIsSubdivided[10], edgeIsSubdivided[11], edgeIsSubdivided[8], edgeIsSubdivided[9] };
                }
                else if (minLevel1TwinIdxPos == 3) {
                    rotatedPositions = { positions[3], positions[0], positions[1], positions[2], positions[7], positions[4], positions[5], positions[6] };
                    rotatedEdgeIsSubdivided = { edgeIsSubdivided[3], edgeIsSubdivided[0], edgeIsSubdivided[1], edgeIsSubdivided[2],
                        edgeIsSubdivided[7], edgeIsSubdivided[4], edgeIsSubdivided[5], edgeIsSubdivided[6],
                        edgeIsSubdivided[11], edgeIsSubdivided[8], edgeIsSubdivided[9], edgeIsSubdivided[10] };
                }
                else if (minLevel1TwinIdxPos == 4) {
                    rotatedPositions = { positions[0], positions[4], positions[5], positions[1], positions[3], positions[7], positions[6], positions[2] };
                    rotatedEdgeIsSubdivided = { edgeIsSubdivided[4], edgeIsSubdivided[8], edgeIsSubdivided[5], edgeIsSubdivided[0],
                        edgeIsSubdivided[3], edgeIsSubdivided[11], edgeIsSubdivided[9], edgeIsSubdivided[1],
                        edgeIsSubdivided[7], edgeIsSubdivided[10], edgeIsSubdivided[6], edgeIsSubdivided[2] };
                }
                else if (minLevel1TwinIdxPos == 5) {
                    rotatedPositions = { positions[5], positions[1], positions[0], positions[4], positions[6], positions[2], positions[3], positions[7] };
                    rotatedEdgeIsSubdivided = { edgeIsSubdivided[5], edgeIsSubdivided[0], edgeIsSubdivided[4], edgeIsSubdivided[8],
                        edgeIsSubdivided[9], edgeIsSubdivided[1], edgeIsSubdivided[3], edgeIsSubdivided[11],
                        edgeIsSubdivided[6], edgeIsSubdivided[2], edgeIsSubdivided[7], edgeIsSubdivided[10] };
                }
                else if (minLevel1TwinIdxPos == 6) {
                    rotatedPositions = { positions[2], positions[6], positions[7], positions[3], positions[1], positions[5], positions[4], positions[0] };
                    rotatedEdgeIsSubdivided = { edgeIsSubdivided[6], edgeIsSubdivided[10], edgeIsSubdivided[7], edgeIsSubdivided[2],
                        edgeIsSubdivided[1], edgeIsSubdivided[9], edgeIsSubdivided[11], edgeIsSubdivided[3],
                        edgeIsSubdivided[5], edgeIsSubdivided[8], edgeIsSubdivided[4], edgeIsSubdivided[0] };
                }
                else if (minLevel1TwinIdxPos == 7) {
                    rotatedPositions = { positions[7], positions[3], positions[2], positions[6], positions[4], positions[0], positions[1], positions[5] };
                    rotatedEdgeIsSubdivided = { edgeIsSubdivided[7], edgeIsSubdivided[2], edgeIsSubdivided[6], edgeIsSubdivided[10],
                        edgeIsSubdivided[11], edgeIsSubdivided[3], edgeIsSubdivided[1], edgeIsSubdivided[9],
                        edgeIsSubdivided[4], edgeIsSubdivided[0], edgeIsSubdivided[5], edgeIsSubdivided[8] };
                }
                else if (minLevel1TwinIdxPos == 8) {
                    rotatedPositions = { positions[4], positions[5], positions[1], positions[0], positions[7], positions[6], positions[2], positions[3] };
                    rotatedEdgeIsSubdivided = { edgeIsSubdivided[8], edgeIsSubdivided[5], edgeIsSubdivided[0], edgeIsSubdivided[4],
                        edgeIsSubdivided[11], edgeIsSubdivided[9], edgeIsSubdivided[1], edgeIsSubdivided[3],
                        edgeIsSubdivided[10], edgeIsSubdivided[6], edgeIsSubdivided[2], edgeIsSubdivided[7] };
                }
                else if (minLevel1TwinIdxPos == 9) {
                    rotatedPositions = { positions[5], positions[6], positions[2], positions[1], positions[4], positions[7], positions[3], positions[0] };
                    rotatedEdgeIsSubdivided = { edgeIsSubdivided[9], edgeIsSubdivided[6], edgeIsSubdivided[1], edgeIsSubdivided[5],
                        edgeIsSubdivided[8], edgeIsSubdivided[10], edgeIsSubdivided[2], edgeIsSubdivided[0],
                        edgeIsSubdivided[11], edgeIsSubdivided[7], edgeIsSubdivided[3], edgeIsSubdivided[4] };
                }
                else if (minLevel1TwinIdxPos == 10) {
                    rotatedPositions = { positions[6], positions[7], positions[3], positions[2], positions[5], positions[4], positions[0], positions[1] };
                    rotatedEdgeIsSubdivided = { edgeIsSubdivided[10], edgeIsSubdivided[7], edgeIsSubdivided[2], edgeIsSubdivided[6],
                        edgeIsSubdivided[9], edgeIsSubdivided[11], edgeIsSubdivided[3], edgeIsSubdivided[1],
                        edgeIsSubdivided[8], edgeIsSubdivided[4], edgeIsSubdivided[0], edgeIsSubdivided[5] };
                }
                else if (minLevel1TwinIdxPos == 11) {
                    rotatedPositions = { positions[7], positions[4], positions[0], positions[3], positions[6], positions[5], positions[1], positions[2] };
                    rotatedEdgeIsSubdivided = { edgeIsSubdivided[11], edgeIsSubdivided[4], edgeIsSubdivided[3], edgeIsSubdivided[7],
                        edgeIsSubdivided[10], edgeIsSubdivided[8], edgeIsSubdivided[0], edgeIsSubdivided[2],
                        edgeIsSubdivided[9], edgeIsSubdivided[5], edgeIsSubdivided[1], edgeIsSubdivided[6] };
                }

                std::array<size_t, 14> ptsIdx;

                // 0-7
                for (size_t j = 0; j < 8; ++j) {
                    ptsIdx[j] = addPoint(rotatedPositions[j]);
                }
                // 8-13
                const std::array<std::array<double, 3>, 6> newPts = {
                    interpolatePoint(rotatedPositions[0], 1, rotatedPositions[3], 2),
                    interpolatePoint(rotatedPositions[1], 1, rotatedPositions[2], 2),
                    interpolatePoint(rotatedPositions[0], 1, rotatedPositions[4], 2),
                    interpolatePoint(rotatedPositions[1], 1, rotatedPositions[5], 2),
                    interpolatePoint(rotatedPositions[0], 1, rotatedPositions[7], 2),
                    interpolatePoint(rotatedPositions[1], 1, rotatedPositions[6], 2) };
                for (size_t j = 8; j < 14; ++j) {
                    ptsIdx[j] = addPoint(newPts[j - 8]);
                }
                // 3 hex elements
                hE.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[9], ptsIdx[8], ptsIdx[10], ptsIdx[11], ptsIdx[13], ptsIdx[12] });
                std::array<std::array<double, 3>, 8> tmpPositions =
                { newPts[0], newPts[1], rotatedPositions[2], rotatedPositions[3], newPts[4], newPts[5], rotatedPositions[6], rotatedPositions[7] };
                std::array<size_t, 12> tmpEdgeIsSubdivided =
                { 0, 0, rotatedEdgeIsSubdivided[2], 0, 0, 0, rotatedEdgeIsSubdivided[6], rotatedEdgeIsSubdivided[7], 0, 0, rotatedEdgeIsSubdivided[10], 0 };
                self(self, tmpPositions, tmpEdgeIsSubdivided);
                tmpPositions = { newPts[2], newPts[3], newPts[5], newPts[4], rotatedPositions[4], rotatedPositions[5], rotatedPositions[6], rotatedPositions[7] };
                tmpEdgeIsSubdivided = { 0, 0, 0, 0, 0, 0, 0, 0, rotatedEdgeIsSubdivided[8], rotatedEdgeIsSubdivided[9], rotatedEdgeIsSubdivided[10], rotatedEdgeIsSubdivided[11] };
                self(self, tmpPositions, tmpEdgeIsSubdivided);
            }
            else {// no level 1 edge exist in the hex
                if (minLevel1IdxPos < 8) {// split at the level1 point
                    std::array<std::array<double, 3>, 8> rotatedPositions;
                    std::array<size_t, 12> rotatedEdgeIsSubdivided;
                    if (minLevel1IdxPos == 0) {
                        rotatedPositions = positions;
                        rotatedEdgeIsSubdivided = edgeIsSubdivided;
                    }
                    else if (minLevel1IdxPos == 1) {
                        rotatedPositions = { positions[1], positions[2], positions[3], positions[0], positions[5], positions[6], positions[7], positions[4] };
                        rotatedEdgeIsSubdivided = { edgeIsSubdivided[1], edgeIsSubdivided[2], edgeIsSubdivided[3], edgeIsSubdivided[0],
                            edgeIsSubdivided[5], edgeIsSubdivided[6], edgeIsSubdivided[7], edgeIsSubdivided[4],
                            edgeIsSubdivided[9], edgeIsSubdivided[10], edgeIsSubdivided[11], edgeIsSubdivided[8] };
                    }
                    else if (minLevel1IdxPos == 2) {
                        rotatedPositions = { positions[2], positions[3], positions[0], positions[1], positions[6], positions[7], positions[4], positions[5] };
                        rotatedEdgeIsSubdivided = { edgeIsSubdivided[2], edgeIsSubdivided[3], edgeIsSubdivided[0], edgeIsSubdivided[1],
                            edgeIsSubdivided[6], edgeIsSubdivided[7], edgeIsSubdivided[4], edgeIsSubdivided[5],
                            edgeIsSubdivided[10], edgeIsSubdivided[11], edgeIsSubdivided[8], edgeIsSubdivided[9] };
                    }
                    else if (minLevel1IdxPos == 3) {
                        rotatedPositions = { positions[3], positions[0], positions[1], positions[2], positions[7], positions[4], positions[5], positions[6] };
                        rotatedEdgeIsSubdivided = { edgeIsSubdivided[3], edgeIsSubdivided[0], edgeIsSubdivided[1], edgeIsSubdivided[2],
                            edgeIsSubdivided[7], edgeIsSubdivided[4], edgeIsSubdivided[5], edgeIsSubdivided[6],
                            edgeIsSubdivided[11], edgeIsSubdivided[8], edgeIsSubdivided[9], edgeIsSubdivided[10] };
                    }
                    else if (minLevel1IdxPos == 4) {
                        rotatedPositions = { positions[4], positions[5], positions[1], positions[0], positions[7], positions[6], positions[2], positions[3] };
                        rotatedEdgeIsSubdivided = { edgeIsSubdivided[8], edgeIsSubdivided[5], edgeIsSubdivided[0], edgeIsSubdivided[4],
                            edgeIsSubdivided[11], edgeIsSubdivided[9], edgeIsSubdivided[1], edgeIsSubdivided[3],
                            edgeIsSubdivided[10], edgeIsSubdivided[6], edgeIsSubdivided[2], edgeIsSubdivided[7] };
                    }
                    else if (minLevel1IdxPos == 5) {
                        rotatedPositions = { positions[5], positions[6], positions[2], positions[1], positions[4], positions[7], positions[3], positions[0] };
                        rotatedEdgeIsSubdivided = { edgeIsSubdivided[9], edgeIsSubdivided[6], edgeIsSubdivided[1], edgeIsSubdivided[5],
                            edgeIsSubdivided[8], edgeIsSubdivided[10], edgeIsSubdivided[2], edgeIsSubdivided[0],
                            edgeIsSubdivided[11], edgeIsSubdivided[7], edgeIsSubdivided[3], edgeIsSubdivided[4] };
                    }
                    else if (minLevel1IdxPos == 6) {
                        rotatedPositions = { positions[6], positions[2], positions[1], positions[5], positions[7], positions[3], positions[0], positions[4] };
                        rotatedEdgeIsSubdivided = { edgeIsSubdivided[6], edgeIsSubdivided[1], edgeIsSubdivided[5], edgeIsSubdivided[9],
                            edgeIsSubdivided[10], edgeIsSubdivided[2], edgeIsSubdivided[0], edgeIsSubdivided[8],
                            edgeIsSubdivided[7], edgeIsSubdivided[3], edgeIsSubdivided[4], edgeIsSubdivided[11] };
                    }
                    else if (minLevel1IdxPos == 7) {
                        rotatedPositions = { positions[7], positions[3], positions[2], positions[6], positions[4], positions[0], positions[1], positions[5] };
                        rotatedEdgeIsSubdivided = { edgeIsSubdivided[7], edgeIsSubdivided[2], edgeIsSubdivided[6], edgeIsSubdivided[10],
                            edgeIsSubdivided[11], edgeIsSubdivided[3], edgeIsSubdivided[1], edgeIsSubdivided[9],
                            edgeIsSubdivided[4], edgeIsSubdivided[0], edgeIsSubdivided[5], edgeIsSubdivided[8] };
                    }

                    std::array<size_t, 15> ptsIdx;

                    // 0-7
                    for (size_t j = 0; j < 8; ++j) {
                        ptsIdx[j] = addPoint(rotatedPositions[j]);
                    }
                    // 8-14
                    const std::array<std::array<double, 3>, 7> newPts = {
                        interpolatePoint(rotatedPositions[0], 1, rotatedPositions[1], 2),
                        interpolatePoint(rotatedPositions[0], 1, rotatedPositions[3], 2),
                        interpolatePoint(rotatedPositions[0], 1, rotatedPositions[2], 2),
                        interpolatePoint(rotatedPositions[0], 1, rotatedPositions[4], 2),
                        interpolatePoint(rotatedPositions[0], 1, rotatedPositions[5], 2),
                        interpolatePoint(rotatedPositions[0], 1, rotatedPositions[7], 2),
                        interpolatePoint(rotatedPositions[0], 1, rotatedPositions[6], 2) };
                    for (size_t j = 8; j < 15; ++j) {
                        ptsIdx[j] = addPoint(newPts[j - 8]);
                    }
                    // 4 hex elements
                    hE.push_back({ ptsIdx[0], ptsIdx[8], ptsIdx[10], ptsIdx[9], ptsIdx[11], ptsIdx[12], ptsIdx[14], ptsIdx[13] });
                    std::array<std::array<double, 3>, 8> tmpPositions =
                    { newPts[0], rotatedPositions[1], rotatedPositions[2], newPts[2], newPts[4], rotatedPositions[5], rotatedPositions[6], newPts[6] };
                    std::array<size_t, 12> tmpEdgeIsSubdivided =
                    { 0, rotatedEdgeIsSubdivided[1], 0, 0, 0, rotatedEdgeIsSubdivided[5], rotatedEdgeIsSubdivided[6], 0, 0, rotatedEdgeIsSubdivided[9], 0, 0 };
                    self(self, tmpPositions, tmpEdgeIsSubdivided);
                    tmpPositions = { newPts[1], newPts[2], rotatedPositions[2], rotatedPositions[3], newPts[5], newPts[6], rotatedPositions[6], rotatedPositions[7] };
                    tmpEdgeIsSubdivided = { 0, 0, rotatedEdgeIsSubdivided[2], 0, 0, 0, rotatedEdgeIsSubdivided[6], rotatedEdgeIsSubdivided[7], 0, 0, rotatedEdgeIsSubdivided[10], 0 };
                    self(self, tmpPositions, tmpEdgeIsSubdivided);
                    tmpPositions = { newPts[3], newPts[4], newPts[6], newPts[5], rotatedPositions[4], rotatedPositions[5], rotatedPositions[6], rotatedPositions[7] };
                    tmpEdgeIsSubdivided = { 0, 0, 0, 0, 0, 0, 0, 0, rotatedEdgeIsSubdivided[8], rotatedEdgeIsSubdivided[9], rotatedEdgeIsSubdivided[10], rotatedEdgeIsSubdivided[11] };
                    self(self, tmpPositions, tmpEdgeIsSubdivided);
                }
                else {// no level1 point exist in the hex
                    // 0E
                    if (subdividedEdgeCount == 0) {
                        noBlackEdge(positions);
                    }
                    // 1E
                    else if (subdividedEdgeCount == 1) {
                        if (edgeIsSubdivided[0]) {
                            oneBlackEdge({ positions[1], positions[2], positions[3], positions[0], positions[5], positions[6], positions[7], positions[4] });
                        }
                        else if (edgeIsSubdivided[1]) {
                            oneBlackEdge({ positions[2], positions[3], positions[0], positions[1], positions[6], positions[7], positions[4], positions[5] });
                        }
                        else if (edgeIsSubdivided[2]) {
                            oneBlackEdge({ positions[3], positions[0], positions[1], positions[2], positions[7], positions[4], positions[5], positions[6] });
                        }
                        else if (edgeIsSubdivided[3]) {
                            oneBlackEdge(positions);
                        }
                        else if (edgeIsSubdivided[4]) {
                            oneBlackEdge({ positions[0], positions[3], positions[7], positions[4], positions[1], positions[2], positions[6], positions[5] });
                        }
                        else if (edgeIsSubdivided[5]) {
                            oneBlackEdge({ positions[1], positions[0], positions[4], positions[5], positions[2], positions[3], positions[7], positions[6] });
                        }
                        else if (edgeIsSubdivided[6]) {
                            oneBlackEdge({ positions[2], positions[1], positions[5], positions[6], positions[3], positions[0], positions[4], positions[7] });
                        }
                        else if (edgeIsSubdivided[7]) {
                            oneBlackEdge({ positions[7], positions[4], positions[0], positions[3], positions[6], positions[5], positions[1], positions[2] });
                        }
                        else if (edgeIsSubdivided[8]) {
                            oneBlackEdge({ positions[5], positions[1], positions[0], positions[4], positions[6], positions[2], positions[3], positions[7] });
                        }
                        else if (edgeIsSubdivided[9]) {
                            oneBlackEdge({ positions[6], positions[2], positions[1], positions[5], positions[7], positions[3], positions[0], positions[4] });
                        }
                        else if (edgeIsSubdivided[10]) {
                            oneBlackEdge({ positions[7], positions[3], positions[2], positions[6], positions[4], positions[0], positions[1], positions[5] });
                        }
                        else if (edgeIsSubdivided[11]) {
                            oneBlackEdge({ positions[7], positions[6], positions[5], positions[4], positions[3], positions[2], positions[1], positions[0] });
                        }
                    }
                    // 4E
                    else if (subdividedEdgeCount == 4) {
                        if (edgeIsSubdivided[0] && edgeIsSubdivided[1] && edgeIsSubdivided[2] && edgeIsSubdivided[3]) {
                            fourBlackEdges({ positions[0], positions[1], positions[2], positions[3], positions[4], positions[5], positions[6], positions[7] });
                        }
                        else if (edgeIsSubdivided[0] && edgeIsSubdivided[4] && edgeIsSubdivided[5] && edgeIsSubdivided[8]) {
                            fourBlackEdges({ positions[4], positions[5], positions[1], positions[0], positions[7], positions[6], positions[2], positions[3] });
                        }
                        else if (edgeIsSubdivided[1] && edgeIsSubdivided[5] && edgeIsSubdivided[6] && edgeIsSubdivided[9]) {
                            fourBlackEdges({ positions[5], positions[6], positions[2], positions[1], positions[4], positions[7], positions[3], positions[0] });
                        }
                        else if (edgeIsSubdivided[2] && edgeIsSubdivided[6] && edgeIsSubdivided[7] && edgeIsSubdivided[10]) {
                            fourBlackEdges({ positions[3], positions[2], positions[6], positions[7], positions[0], positions[1], positions[5], positions[4] });
                        }
                        else if (edgeIsSubdivided[3] && edgeIsSubdivided[4] && edgeIsSubdivided[7] && edgeIsSubdivided[11]) {
                            fourBlackEdges({ positions[4], positions[0], positions[3], positions[7], positions[5], positions[1], positions[2], positions[6] });
                        }
                        else if (edgeIsSubdivided[8] && edgeIsSubdivided[9] && edgeIsSubdivided[10] && edgeIsSubdivided[11]) {
                            fourBlackEdges({ positions[7], positions[6], positions[5], positions[4], positions[3], positions[2], positions[1], positions[0] });
                        }
                    }
                }
            }
        }

        };

    // replace grid cells with templates
    for (size_t level = 0; level < c.size(); ++level) {
        std::cout << "replacing cell with hex templates at tree depth: " << level + 1 << std::endl;
        const auto& cLayer = c[level];
        for (size_t cLayerIdx = 0; cLayerIdx < cLayer.size(); ++cLayerIdx) {
            const auto& cLayerCell = cLayer[cLayerIdx];
            if (cLayerCell.c[0] != cLayerCell.c[1]) { // skip if the current cell has children
                continue;
            }

            // eight corner coordinates in double
            std::array<std::array<double, 3>, 8> corners;
            for (size_t i = 0; i < 8; ++i) {
                for (size_t j = 0; j < 3; ++j) {
                    corners[i][j] = double(cLayerCell.p[j][cubeCorners[i][j]]);
                }
            }

            // 12 edge subdivision situation
            const std::array<size_t, 12> edgeIsSubdivided = {
                xMap.count(HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][0]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][0]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][0]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][0]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][1]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][1]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][1]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][1]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][0]), double(cLayerCell.p[2][3]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][3]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][3]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][1]), double(cLayerCell.p[1][3]), double(cLayerCell.p[2][3]))),
                xMap.count(HASHIT(double(cLayerCell.p[0][0]), double(cLayerCell.p[1][1]), double(cLayerCell.p[2][3]))) };
            
            recursiveSplit(recursiveSplit, corners, edgeIsSubdivided);
        }
    }

    return Hexahedra(tri, hE, hX);
}