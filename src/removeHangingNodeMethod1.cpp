#include "geometry.h"

Hexahedra Tree::RemoveHangingNodesMethod1() {
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

    std::unordered_map<std::string, std::pair<size_t, std::unordered_map<std::string, std::pair<size_t, size_t>>>> posToMinSideAndAdjCells;
    for (size_t cIdx = 0; cIdx < c.size(); ++cIdx) {
        const auto& cLayer = c[cIdx];
        for (size_t cLayerIdx = 0; cLayerIdx < cLayer.size(); ++cLayerIdx) {
            const auto& cLayerCell = cLayer[cLayerIdx];
            if (cLayerCell.c[0] != cLayerCell.c[1]) {
                continue;
            }

            for (const auto& cubeCorner : cubeCorners) {
                const std::string hash = HASHIT(cLayerCell.p[0][cubeCorner[0]], cLayerCell.p[1][cubeCorner[1]], cLayerCell.p[2][cubeCorner[2]]);
                if (!posToMinSideAndAdjCells.count(hash)) {
                    posToMinSideAndAdjCells[hash].first = cLayerCell.p[0][3] - cLayerCell.p[0][0];
                }
                else {
                    posToMinSideAndAdjCells[hash].first = std::min(posToMinSideAndAdjCells.at(hash).first,
                        cLayerCell.p[0][3] - cLayerCell.p[0][0]);
                }
                posToMinSideAndAdjCells[hash].second[HASHIT2(cIdx, cLayerIdx)] = std::make_pair(cIdx, cLayerIdx);
            }
        }
    }

    const std::unordered_map<uint8_t, std::vector<std::vector<uint8_t>>> cubeBlackCodeTable = {
        // 2B
        { 5, { { 4 }, { 6 } } },
        { 10, { { 5 }, { 7 } } },
        { 18, { { 2 }, { 7 } } },
        { 24, { { 0 }, { 7 } } },
        { 33, { { 3 }, { 6 } } },
        { 36, { { 1 }, { 6 } } },
        { 66, { { 2 }, { 5 } } },
        { 72, { { 0 }, { 5 } } },
        { 80, { { 0 }, { 2 } } },
        { 129, { { 3 }, { 4 } } },
        { 132, { { 1 }, { 4 } } },
        { 160, { { 1 }, { 3 } } },

        // 2C
        { 20, { { 0, 1, 2, 4 }, { 0, 1, 2, 6 }, { 0, 1, 4, 7 }, { 0, 4, 6, 7 }, { 1, 2, 6, 7 }, { 2, 4, 6, 7 } } },
        { 40, { { 0, 1, 3, 5 }, { 0, 1, 3, 7 }, { 0, 1, 5, 6 }, { 0, 3, 6, 7 }, { 1, 5, 6, 7 }, { 3, 5, 6, 7 } } },
        { 65, { { 0, 2, 3, 4 }, { 0, 2, 3, 6 }, { 0, 3, 4, 5 }, { 0, 4, 5, 6 }, { 2, 3, 5, 6 }, { 2, 4, 5, 6 } } },
        { 130, { { 1, 2, 3, 5 }, { 1, 2, 3, 7 }, { 1, 2, 4, 5 }, { 1, 4, 5, 7 }, { 2, 3, 4, 7 }, { 3, 4, 5, 7 } } },

        // 3B
        { 21, { { 0, 1, 4 }, { 0, 4, 6 }, { 1, 2, 6 }, { 2, 4, 6 } } },
        { 22, { { 0, 1, 2 }, { 0, 4, 7 }, { 1, 2, 7 }, { 2, 4, 7 } } },
        { 28, { { 0, 1, 2 }, { 0, 1, 7 }, { 0, 6, 7 }, { 2, 6, 7 } } },
        { 41, { { 0, 1, 3 }, { 0, 3, 6 }, { 1, 5, 6 }, { 3, 5, 6 } } },
        { 42, { { 0, 1, 5 }, { 0, 3, 7 }, { 1, 5, 7 }, { 3, 5, 7 } } },
        { 44, { { 0, 1, 3 }, { 0, 1, 6 }, { 1, 6, 7 }, { 3, 6, 7 } } },
        { 52, { { 0, 1, 4 }, { 0, 1, 6 }, { 1, 6, 7 }, { 4, 6, 7 } } },
        { 56, { { 0, 1, 5 }, { 0, 1, 7 }, { 0, 6, 7 }, { 5, 6, 7 } } },
        { 67, { { 0, 2, 3 }, { 0, 4, 5 }, { 2, 3, 5 }, { 2, 4, 5 } } },
        { 69, { { 0, 3, 4 }, { 0, 4, 6 }, { 2, 3, 6 }, { 2, 4, 6 } } },
        { 73, { { 0, 2, 3 }, { 0, 3, 5 }, { 0, 5, 6 }, { 2, 5, 6 } } },
        { 81, { { 0, 2, 4 }, { 0, 2, 6 }, { 0, 4, 5 }, { 2, 5, 6 } } },
        { 84, { { 0, 2, 4 }, { 0, 2, 6 }, { 0, 4, 7 }, { 2, 6, 7 } } },
        { 97, { { 0, 3, 4 }, { 0, 3, 6 }, { 3, 5, 6 }, { 4, 5, 6 } } },
        { 104, { { 0, 3, 5 }, { 0, 3, 7 }, { 0, 5, 6 }, { 5, 6, 7 } } },
        { 131, { { 1, 2, 3 }, { 1, 4, 5 }, { 2, 3, 4 }, { 3, 4, 5 } } },
        { 134, { { 1, 2, 3 }, { 1, 2, 4 }, { 1, 4, 7 }, { 3, 4, 7 } } },
        { 138, { { 1, 2, 5 }, { 1, 5, 7 }, { 2, 3, 7 }, { 3, 5, 7 } } },
        { 146, { { 1, 2, 5 }, { 1, 2, 7 }, { 2, 4, 7 }, { 4, 5, 7 } } },
        { 148, { { 1, 2, 4 }, { 1, 2, 6 }, { 1, 4, 7 }, { 4, 6, 7 } } },
        { 162, { { 1, 3, 5 }, { 1, 3, 7 }, { 1, 4, 5 }, { 3, 4, 7 } } },
        { 168, { { 1, 3, 5 }, { 1, 3, 7 }, { 1, 5, 6 }, { 3, 6, 7 } } },
        { 193, { { 2, 3, 4 }, { 2, 3, 6 }, { 3, 4, 5 }, { 4, 5, 6 } } },
        { 194, { { 2, 3, 5 }, { 2, 3, 7 }, { 2, 4, 5 }, { 4, 5, 7 } } },

        // 3C
        { 26, { { 0, 2, 7 }, { 0, 5, 7 }, { 2, 5, 7 } } },
        { 37, { { 1, 3, 6 }, { 1, 4, 6 }, { 3, 4, 6 } } },
        { 74, { { 0, 2, 5 }, { 0, 5, 7 }, { 2, 5, 7 } } },
        { 82, { { 0, 2, 5 }, { 0, 2, 7 }, { 2, 5, 7 } } },
        { 88, { { 0, 2, 5 }, { 0, 2, 7 }, { 0, 5, 7 } } },
        { 133, { { 1, 3, 4 }, { 1, 4, 6 }, { 3, 4, 6 } } },
        { 161, { { 1, 3, 4 }, { 1, 3, 6 }, { 3, 4, 6 } } },
        { 164, { { 1, 3, 4 }, { 1, 3, 6 }, { 1, 4, 6 } } },

        // 4B
        { 27, { { 0, 2 }, { 0, 5 }, { 2, 5 } } },
        { 39, { { 1, 3 }, { 1, 4 }, { 3, 4 } } },
        { 78, { { 0, 2 }, { 0, 7 }, { 2, 7 } } },
        { 114, { { 0, 5 }, { 0, 7 }, { 5, 7 } } },
        { 141, { { 1, 3 }, { 1, 6 }, { 3, 6 } } },
        { 177, { { 1, 4 }, { 1, 6 }, { 4, 6 } } },
        { 216, { { 2, 5 }, { 2, 7 }, { 5, 7 } } },
        { 228, { { 3, 4 }, { 3, 6 }, { 4, 6 } } },

        // 4C
        { 23, { { 0, 4 }, { 1, 2 }, { 2, 4 } } },
        { 46, { { 0, 1 }, { 1, 7 }, { 3, 7 } } },
        { 57, { { 0, 1 }, { 0, 6 }, { 5, 6 } } },
        { 77, { { 0, 3 }, { 0, 6 }, { 2, 6 } } },
        { 99, { { 0, 3 }, { 3, 5 }, { 4, 5 } } },
        { 116, { { 0, 4 }, { 0, 6 }, { 6, 7 } } },
        { 139, { { 1, 5 }, { 2, 3 }, { 3, 5 } } },
        { 156, { { 1, 2 }, { 1, 7 }, { 6, 7 } } },
        { 178, { { 1, 5 }, { 1, 7 }, { 4, 7 } } },
        { 198, { { 2, 3 }, { 2, 4 }, { 4, 7 } } },
        { 209, { { 2, 4 }, { 2, 6 }, { 4, 5 } } },
        { 232, { { 3, 5 }, { 3, 7 }, { 5, 6 } } },

        // 4C mirror
        { 29, { { 0, 1 }, { 0, 6 }, { 2, 6 } } },
        { 43, { { 0, 3 }, { 1, 5 }, { 3, 5 } } },
        { 54, { { 0, 1 }, { 1, 7 }, { 4, 7 } } },
        { 71, { { 0, 4 }, { 2, 3 }, { 2, 4 } } },
        { 108, { { 0, 3 }, { 0, 6 }, { 6, 7 } } },
        { 113, { { 0, 4 }, { 0, 6 }, { 5, 6 } } },
        { 142, { { 1, 2 }, { 1, 7 }, { 3, 7 } } },
        { 147, { { 1, 2 }, { 2, 4 }, { 4, 5 } } },
        { 184, { { 1, 5 }, { 1, 7 }, { 6, 7 } } },
        { 201, { { 2, 3 }, { 3, 5 }, { 5, 6 } } },
        { 212, { { 2, 4 }, { 2, 6 }, { 4, 7 } } },
        { 226, { { 3, 5 }, { 3, 7 }, { 4, 5 } } },

        // 4D
        { 30, { { 0, 7 }, { 2, 7 } } },
        { 45, { { 1, 6 }, { 3, 6 } } },
        { 53, { { 1, 6 }, { 4, 6 } } },
        { 58, { { 0, 7 }, { 5, 7 } } },
        { 75, { { 0, 5 }, { 2, 5 } } },
        { 83, { { 0, 2 }, { 2, 5 } } },
        { 86, { { 0, 2 }, { 2, 7 } } },
        { 89, { { 0, 2 }, { 0, 5 } } },
        { 92, { { 0, 2 }, { 0, 7 } } },
        { 101, { { 3, 6 }, { 4, 6 } } },
        { 106, { { 0, 5 }, { 5, 7 } } },
        { 120, { { 0, 5 }, { 0, 7 } } },
        { 135, { { 1, 4 }, { 3, 4 } } },
        { 149, { { 1, 4 }, { 4, 6 } } },
        { 154, { { 2, 7 }, { 5, 7 } } },
        { 163, { { 1, 3 }, { 3, 4 } } },
        { 166, { { 1, 3 }, { 1, 4 } } },
        { 169, { { 1, 3 }, { 3, 6 } } },
        { 172, { { 1, 3 }, { 1, 6 } } },
        { 180, { { 1, 4 }, { 1, 6 } } },
        { 197, { { 3, 4 }, { 4, 6 } } },
        { 202, { { 2, 5 }, { 5, 7 } } },
        { 210, { { 2, 5 }, { 2, 7 } } },
        { 225, { { 3, 4 }, { 3, 6 } } },

        // 4E
        { 60, { { 0, 1 }, { 6, 7 } } },
        { 85, { { 0, 4 }, { 2, 6 } } },
        { 105, { { 0, 3 }, { 5, 6 } } },
        { 150, { { 1, 2 }, { 4, 7 } } },
        { 170, { { 1, 5 }, { 3, 7 } } },
        { 195, { { 2, 3 }, { 4, 5 } } },

        // 5A
        { 31, { { 0 }, { 2 } } },
        { 47, { { 1 }, { 3 } } },
        { 55, { { 1 }, { 4 } } },
        { 59, { { 0 }, { 5 } } },
        { 79, { { 0 }, { 2 } } },
        { 103, { { 3 }, { 4 } } },
        { 110, { { 0 }, { 7 } } },
        { 115, { { 0 }, { 5 } } },
        { 118, { { 0 }, { 7 } } },
        { 143, { { 1 }, { 3 } } },
        { 155, { { 2 }, { 5 } } },
        { 157, { { 1 }, { 6 } } },
        { 179, { { 1 }, { 4 } } },
        { 185, { { 1 }, { 6 } } },
        { 205, { { 3 }, { 6 } } },
        { 206, { { 2 }, { 7 } } },
        { 217, { { 2 }, { 5 } } },
        { 220, { { 2 }, { 7 } } },
        { 230, { { 3 }, { 4 } } },
        { 236, { { 3 }, { 6 } } },
        { 241, { { 4 }, { 6 } } },
        { 242, { { 5 }, { 7 } } },
        { 244, { { 4 }, { 6 } } },
        { 248, { { 5 }, { 7 } } },

        // 5B
        { 61, { { 6 } } },
        { 62, { { 7 } } },
        { 87, { { 2 } } },
        { 93, { { 0 } } },
        { 107, { { 5 } } },
        { 109, { { 6 } } },
        { 117, { { 6 } } },
        { 121, { { 0 } } },
        { 124, { { 0 } } },
        { 151, { { 4 } } },
        { 158, { { 7 } } },
        { 171, { { 3 } } },
        { 174, { { 1 } } },
        { 182, { { 1 } } },
        { 186, { { 7 } } },
        { 188, { { 1 } } },
        { 199, { { 4 } } },
        { 203, { { 5 } } },
        { 211, { { 2 } } },
        { 213, { { 4 } } },
        { 214, { { 2 } } },
        { 227, { { 3 } } },
        { 233, { { 3 } } },
        { 234, { { 5 } } },

        // 4F
        { 90, { { 0, 2, 5, 7 } } },
        { 165, { { 1, 3, 4, 6 } } },

        // 5C
        { 91, { { 0, 2, 5 } } },
        { 94, { { 0, 2, 7 } } },
        { 122, { { 0, 5, 7 } } },
        { 167, { { 1, 3, 4 } } },
        { 173, { { 1, 3, 6 } } },
        { 181, { { 1, 4, 6 } } },
        { 218, { { 2, 5, 7 } } },
        { 229, { { 3, 4, 6 } } },

        // 6B
        { 95, { { 0, 2 } } },
        { 123, { { 0, 5 } } },
        { 126, { { 0, 7 } } },
        { 175, { { 1, 3 } } },
        { 183, { { 1, 4 } } },
        { 189, { { 1, 6 } } },
        { 219, { { 2, 5 } } },
        { 222, { { 2, 7 } } },
        { 231, { { 3, 4 } } },
        { 237, { { 3, 6 } } },
        { 245, { { 4, 6 } } },
        { 250, { { 5, 7 } } },

        // 6C
        { 125, { { 0, 6 } } },
        { 190, { { 1, 7 } } },
        { 215, { { 2, 4 } } },
        { 235, { { 3, 5 } } },

        // 7
        { 127, { { 0 } } },
        { 191, { { 1 } } },
        { 223, { { 2 } } },
        { 239, { { 3 } } },
        { 247, { { 4 } } },
        { 251, { { 5 } } },
        { 253, { { 6 } } },
        { 254, { { 7 } } } };

    const std::unordered_map<uint8_t, std::pair<uint8_t, std::array<uint8_t, 8>>> transitionTemplates = {
        { 0, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 32, { 1, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 4, { 1, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 1, { 1, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 128, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 2, { 1, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 64, { 1, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 8, { 1, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 16, { 1, { 3, 0, 1, 2, 7, 4, 5, 6 } } },

        { 12, { 2, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 34, { 2, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 17, { 2, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 3, { 2, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 6, { 2, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 136, { 2, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 96, { 2, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 9, { 2, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 68, { 2, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 144, { 2, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 192, { 2, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 48, { 2, { 3, 0, 1, 2, 7, 4, 5, 6 } } },

        { 102, { 3, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 153, { 3, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 51, { 3, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 204, { 3, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 15, { 3, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 240, { 3, { 0, 1, 2, 3, 4, 5, 6, 7 } } },

        { 7, { 5, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 11, { 5, { 6, 5, 4, 7, 2, 1, 0, 3 } } },
        { 13, { 5, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 14, { 5, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 19, { 5, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 25, { 5, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 35, { 5, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 38, { 5, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 49, { 5, { 2, 6, 7, 3, 1, 5, 4, 0 } } },
        { 50, { 5, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 70, { 5, { 6, 2, 1, 5, 7, 3, 0, 4 } } },
        { 76, { 5, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 98, { 5, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 100, { 5, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 112, { 5, { 3, 0, 1, 2, 7, 4, 5, 6 } } },
        { 137, { 5, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 140, { 5, { 5, 1, 0, 4, 6, 2, 3, 7 } } },
        { 145, { 5, { 7, 4, 0, 3, 6, 5, 1, 2 } } },
        { 152, { 5, { 3, 7, 4, 0, 2, 6, 5, 1 } } },
        { 176, { 5, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 196, { 5, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 200, { 5, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 208, { 5, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 224, { 5, { 2, 3, 0, 1, 6, 7, 4, 5 } } },

        { 63, { 6, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 111, { 6, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 119, { 6, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 159, { 6, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 187, { 6, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 207, { 6, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 221, { 6, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 238, { 6, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 243, { 6, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 246, { 6, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 249, { 6, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 252, { 6, { 3, 7, 4, 0, 2, 6, 5, 1 } } } };

    bool allLevelModified = true;
    size_t cellIncrease = 0;

    while (allLevelModified) {
        allLevelModified = false;

        for (size_t proxyLevel = 2; proxyLevel <= c.size(); ++proxyLevel) {
            const size_t level = c.size() - proxyLevel;
            std::cout << "refining at tree depth: " << level + 1 << ", cell increase: " << cellIncrease << std::endl;
            cellIncrease = 0;
            const auto& cLayer = c[level];
            bool modified = true;
            while (modified) {
                modified = false;

                for (size_t patternStart = 7; patternStart > 1; --patternStart) {
                    for (size_t cLayerIdx = 0; cLayerIdx < cLayer.size(); ++cLayerIdx) {
                        const auto& cLayerCell = cLayer[cLayerIdx];

                        if (cLayerCell.c[0] != cLayerCell.c[1]) {
                            continue;
                        }

                        size_t minCornerMinSide = cLayerCell.p[0][3] - cLayerCell.p[0][0], maxCornerMinSide = 0;
                        for (const auto& cubeCorner : cubeCorners) {
                            const std::string hash = HASHIT(cLayerCell.p[0][cubeCorner[0]], cLayerCell.p[1][cubeCorner[1]], cLayerCell.p[2][cubeCorner[2]]);
                            const size_t cornerMinSide = posToMinSideAndAdjCells.at(hash).first;
                            minCornerMinSide = std::min(minCornerMinSide, cornerMinSide);
                            maxCornerMinSide = std::max(maxCornerMinSide, cornerMinSide);
                        }
                        if (maxCornerMinSide / 8.0 > minCornerMinSide || maxCornerMinSide < 0.5 * (cLayerCell.p[0][3] - cLayerCell.p[0][0])) {
                            modified = true;
                            allLevelModified = true;

                            // refine this cell
                            RefineCell(level, cLayerIdx);
                            cellIncrease += 26;

                            // update posToMinSideAndAdjCells
                            for (const size_t x : cLayerCell.p[0]) {
                                for (const size_t y : cLayerCell.p[1]) {
                                    for (const size_t z : cLayerCell.p[2]) {
                                        const std::string hash = HASHIT(x, y, z);
                                        // first
                                        if (!posToMinSideAndAdjCells.count(hash)) {
                                            posToMinSideAndAdjCells[hash].first = cLayerCell.p[0][1] - cLayerCell.p[0][0];
                                        }
                                        else {
                                            posToMinSideAndAdjCells[hash].first = std::min(posToMinSideAndAdjCells.at(hash).first,
                                                cLayerCell.p[0][1] - cLayerCell.p[0][0]);
                                        }

                                        // second
                                        posToMinSideAndAdjCells[hash].second.erase(HASHIT2(level, cLayerIdx));
                                        const size_t xEnd = (x - cLayerCell.p[0][0]) / (cLayerCell.p[0][1] - cLayerCell.p[0][0]);
                                        const std::vector<size_t> xRange = (xEnd == 0 ? std::vector<size_t>{ 0 } :
                                            (xEnd == 3 ? std::vector<size_t>{ 2 } : std::vector<size_t>{ xEnd - 1, xEnd }));
                                        const size_t yEnd = (y - cLayerCell.p[1][0]) / (cLayerCell.p[1][1] - cLayerCell.p[1][0]);
                                        const std::vector<size_t> yRange = (yEnd == 0 ? std::vector<size_t>{ 0 } :
                                            (yEnd == 3 ? std::vector<size_t>{ 2 } : std::vector<size_t>{ yEnd - 1, yEnd }));
                                        const size_t zEnd = (z - cLayerCell.p[2][0]) / (cLayerCell.p[2][1] - cLayerCell.p[2][0]);
                                        const std::vector<size_t> zRange = (zEnd == 0 ? std::vector<size_t>{ 0 } :
                                            (zEnd == 3 ? std::vector<size_t>{ 2 } : std::vector<size_t>{ zEnd - 1, zEnd }));
                                        for (const size_t i : xRange) {
                                            for (const size_t j : yRange) {
                                                for (const size_t k : zRange) {
                                                    const size_t ijkIdx = i + 3 * j + 9 * k;
                                                    posToMinSideAndAdjCells[hash].second[HASHIT2(level + 1, c[level + 1].size() - 27 + ijkIdx)] =
                                                        std::make_pair(level + 1, c[level + 1].size() - 27 + ijkIdx);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (maxCornerMinSide / 2.0 > minCornerMinSide) {
                            // find an appropriate refinement template that fits to the cell
                            uint8_t cubeBlackCode = 0;
                            uint8_t digit = 128;
                            size_t blackDotCount = 0;
                            for (const auto& cubeCorner : cubeCorners) {
                                const std::string hash = HASHIT(cLayerCell.p[0][cubeCorner[0]], cLayerCell.p[1][cubeCorner[1]], cLayerCell.p[2][cubeCorner[2]]);
                                if (posToMinSideAndAdjCells.at(hash).first < 0.5 * (cLayerCell.p[0][3] - cLayerCell.p[0][0])) {
                                    ++blackDotCount;
                                    cubeBlackCode += digit;
                                }
                                digit /= 2;
                            }
                            // only find templates for those that have black dots >= patternStart
                            if (blackDotCount < patternStart) {
                                continue;
                            }

                            // if the pattern does not need to add any black dots, continue
                            if (!cubeBlackCodeTable.count(cubeBlackCode)) {
                                continue;
                            }

                            //update posToMinSideAndAdjCells.first
                            modified = true;
                            allLevelModified = true;
                            const auto& insertNewPtChoices = cubeBlackCodeTable.at(cubeBlackCode);
                            const std::array<std::string, 8> cubeCornerHashes = {
                                HASHIT(cLayerCell.p[0][0], cLayerCell.p[1][0], cLayerCell.p[2][0]),
                                HASHIT(cLayerCell.p[0][3], cLayerCell.p[1][0], cLayerCell.p[2][0]),
                                HASHIT(cLayerCell.p[0][3], cLayerCell.p[1][3], cLayerCell.p[2][0]),
                                HASHIT(cLayerCell.p[0][0], cLayerCell.p[1][3], cLayerCell.p[2][0]),
                                HASHIT(cLayerCell.p[0][0], cLayerCell.p[1][0], cLayerCell.p[2][3]),
                                HASHIT(cLayerCell.p[0][3], cLayerCell.p[1][0], cLayerCell.p[2][3]),
                                HASHIT(cLayerCell.p[0][3], cLayerCell.p[1][3], cLayerCell.p[2][3]),
                                HASHIT(cLayerCell.p[0][0], cLayerCell.p[1][3], cLayerCell.p[2][3]) };
                            size_t highestScore = 0;
                            std::vector<uint8_t> highestScoreChoice = insertNewPtChoices[0];
                            for (const auto& insertNewPtChoice : insertNewPtChoices) {
                                size_t score = 0;
                                for (const uint8_t newPt : insertNewPtChoice) {
                                    const auto& adjCells = posToMinSideAndAdjCells.at(cubeCornerHashes[newPt]).second;
                                    size_t adjCellsMinSide = 0;
                                    for (const auto& adjCellAddress : adjCells) {
                                        const auto& adjCellP = c[adjCellAddress.second.first][adjCellAddress.second.second].p;
                                        size_t adjCellMinSide = 0;
                                        for (const auto& cubeCorner : cubeCorners) {
                                            const std::string hash = HASHIT(adjCellP[0][cubeCorner[0]], adjCellP[1][cubeCorner[1]], adjCellP[2][cubeCorner[2]]);
                                            adjCellMinSide = (adjCellMinSide < 0.5 ? posToMinSideAndAdjCells.at(hash).first :
                                                (adjCellMinSide > posToMinSideAndAdjCells.at(hash).first ? posToMinSideAndAdjCells.at(hash).first :
                                                    adjCellMinSide));
                                        }
                                        adjCellsMinSide = (adjCellsMinSide < 0.5 ? adjCellMinSide :
                                            (adjCellsMinSide > adjCellMinSide ? adjCellMinSide : adjCellsMinSide));
                                    }
                                    score += tri.maxSide() / adjCellsMinSide;
                                }
                                if (score > highestScore) {
                                    highestScore = score;
                                    highestScoreChoice = insertNewPtChoice;
                                }
                            }
                            for (const uint8_t newPt : highestScoreChoice) {
                                posToMinSideAndAdjCells.at(cubeCornerHashes[newPt]).first /= 3;
                            }
                        }
                    }
                }
            }
        }
        std::cout << "-----" << std::endl;
    }

    std::unordered_map<std::string, size_t> hXMap;

    for (size_t level = 0; level < c.size(); ++level) {
        std::cout << "replacing cell with hex templates at tree depth: " << level + 1 << std::endl;
        const auto& cLayer = c[level];
        for (size_t cLayerIdx = 0; cLayerIdx < cLayer.size(); ++cLayerIdx) {
            const auto& cLayerCell = cLayer[cLayerIdx];
            if (cLayerCell.c[0] != cLayerCell.c[1]) {
                continue;
            }

            uint8_t cubeBlackCode = 0;
            uint8_t digit = 128;
            for (const auto& cubeCorner : cubeCorners) {
                const std::string hash = HASHIT(cLayerCell.p[0][cubeCorner[0]], cLayerCell.p[1][cubeCorner[1]], cLayerCell.p[2][cubeCorner[2]]);
                if (posToMinSideAndAdjCells.at(hash).first < 0.5 * (cLayerCell.p[0][3] - cLayerCell.p[0][0])) {
                    cubeBlackCode += digit;
                }
                digit /= 2;
            }

            const auto& tT = transitionTemplates.at(cubeBlackCode);

            std::array<std::array<double, 3>, 8> eightPos;
            for (size_t i = 0; i < 8; ++i) {
                for (size_t j = 0; j < 3; ++j) {
                    eightPos[i][j] = double(cLayerCell.p[j][cubeCorners[tT.second[i]][j]]);
                }
            }
            if (tT.first == 1) {
                std::array<size_t, 8> elem;
                // 0-7
                for (size_t i = 0; i < 8; ++i) {
                    const std::string hash = HASHIT(eightPos[i][0], eightPos[i][1], eightPos[i][2]);
                    if (!hXMap.count(hash)) {
                        hXMap[hash] = hX.size();
                        hX.emplace_back(eightPos[i]);
                    }
                    elem[i] = hXMap.at(hash);
                }
                hE.emplace_back(elem);
            }
            else if (tT.first == 2) {
                std::array<size_t, 16> ptsIdx;

                std::array<double, 3> xyz;

                // 0-7
                for (size_t i = 0; i < 8; ++i) {
                    const std::string hash = HASHIT(eightPos[i][0], eightPos[i][1], eightPos[i][2]);
                    if (!hXMap.count(hash)) {
                        hXMap[hash] = hX.size();
                        hX.emplace_back(eightPos[i]);
                    }
                    ptsIdx[i] = hXMap.at(hash);
                }

                // 8
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                std::string hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[8] = hXMap.at(hash);

                // 9
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[9] = hXMap.at(hash);

                // 10
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[10] = hXMap.at(hash);

                // 11
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[11] = hXMap.at(hash);

                // 12
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[12] = hXMap.at(hash);

                // 13
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[13] = hXMap.at(hash);

                // 14
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[14] = hXMap.at(hash);

                // 15
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[15] = hXMap.at(hash);

                hE.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
                hE.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[9], ptsIdx[8], ptsIdx[4], ptsIdx[5], ptsIdx[13], ptsIdx[12] });
                hE.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[2], ptsIdx[10], ptsIdx[13], ptsIdx[5], ptsIdx[6], ptsIdx[14] });
                hE.push_back({ ptsIdx[11], ptsIdx[10], ptsIdx[2], ptsIdx[3], ptsIdx[15], ptsIdx[14], ptsIdx[6], ptsIdx[7] });
                hE.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
            }
            else if (tT.first == 3) {
                std::array<size_t, 32> ptsIdx;

                std::array<double, 3> xyz;

                // 0-7
                for (size_t i = 0; i < 8; ++i) {
                    const std::string hash = HASHIT(eightPos[i][0], eightPos[i][1], eightPos[i][2]);
                    if (!hXMap.count(hash)) {
                        hXMap[hash] = hX.size();
                        hX.emplace_back(eightPos[i]);
                    }
                    ptsIdx[i] = hXMap.at(hash);
                }

                // 8
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                std::string hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[8] = hXMap.at(hash);

                // 9
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[9] = hXMap.at(hash);

                // 10
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[10] = hXMap.at(hash);

                // 11
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[11] = hXMap.at(hash);

                // 12
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[12] = hXMap.at(hash);

                // 13
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[13] = hXMap.at(hash);

                // 14
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[14] = hXMap.at(hash);

                // 15
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[15] = hXMap.at(hash);

                // 16
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[16] = hXMap.at(hash);

                // 17
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[17] = hXMap.at(hash);

                // 18
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[18] = hXMap.at(hash);

                // 19
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[19] = hXMap.at(hash);

                // 20
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[20] = hXMap.at(hash);

                // 21
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[21] = hXMap.at(hash);

                // 22
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[5][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[22] = hXMap.at(hash);

                // 23
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[23] = hXMap.at(hash);

                // 24
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[24] = hXMap.at(hash);

                // 25
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[25] = hXMap.at(hash);

                // 26
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[26] = hXMap.at(hash);

                // 27
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[27] = hXMap.at(hash);

                // 28
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[28] = hXMap.at(hash);

                // 29
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[29] = hXMap.at(hash);

                // 30
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[30] = hXMap.at(hash);

                // 31
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[31] = hXMap.at(hash);

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
                hE.push_back({ ptsIdx[24], ptsIdx[25], ptsIdx[28], ptsIdx[29], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
                hE.push_back({ ptsIdx[31], ptsIdx[20], ptsIdx[22], ptsIdx[30], ptsIdx[4], ptsIdx[24], ptsIdx[29], ptsIdx[7] });
                hE.push_back({ ptsIdx[21], ptsIdx[26], ptsIdx[27], ptsIdx[23], ptsIdx[25], ptsIdx[5], ptsIdx[6], ptsIdx[28] });
            }
            else if (tT.first == 5) {
                std::array<size_t, 32> ptsIdx;

                std::array<double, 3> xyz;

                // 0-7
                for (size_t i = 0; i < 8; ++i) {
                    const std::string hash = HASHIT(eightPos[i][0], eightPos[i][1], eightPos[i][2]);
                    if (!hXMap.count(hash)) {
                        hXMap[hash] = hX.size();
                        hX.emplace_back(eightPos[i]);
                    }
                    ptsIdx[i] = hXMap.at(hash);
                }

                // 8
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                std::string hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[8] = hXMap.at(hash);

                // 9
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[9] = hXMap.at(hash);

                // 10
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[10] = hXMap.at(hash);

                // 11
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[11] = hXMap.at(hash);

                // 12
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[12] = hXMap.at(hash);

                // 13
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[13] = hXMap.at(hash);

                // 14
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[14] = hXMap.at(hash);

                // 15
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[15] = hXMap.at(hash);

                // 16
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[16] = hXMap.at(hash);

                // 17
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 9.0 * 5 + eightPos[3][i] / 9.0 * 4 + (eightPos[1][i] - eightPos[0][i]) / 9.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[17] = hXMap.at(hash);

                // 18
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 9.0 * 5 + eightPos[0][i] / 9.0 * 4 + (eightPos[1][i] - eightPos[0][i]) / 9.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[18] = hXMap.at(hash);

                // 19
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[19] = hXMap.at(hash);

                // 20
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[20] = hXMap.at(hash);

                // 21
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 9.0 * 5 + eightPos[3][i] / 9.0 * 4 +
                        (eightPos[1][i] - eightPos[0][i]) / 9.0 + (eightPos[4][i] - eightPos[0][i]) / 9.0 * 5;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[21] = hXMap.at(hash);

                // 22
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 9.0 * 5 + eightPos[0][i] / 9.0 * 4 +
                        (eightPos[1][i] - eightPos[0][i]) / 9.0 + (eightPos[4][i] - eightPos[0][i]) / 9.0 * 5;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[22] = hXMap.at(hash);

                // 23
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[23] = hXMap.at(hash);

                // 24
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[24] = hXMap.at(hash);

                // 25
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 9.0 * 5 + eightPos[2][i] / 9.0 * 4 + (eightPos[0][i] - eightPos[3][i]) / 9.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[25] = hXMap.at(hash);

                // 26
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 9.0 * 5 + eightPos[3][i] / 9.0 * 4 + (eightPos[0][i] - eightPos[3][i]) / 9.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[26] = hXMap.at(hash);

                // 27
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[27] = hXMap.at(hash);

                // 28
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[28] = hXMap.at(hash);

                // 29
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 9.0 * 5 + eightPos[2][i] / 9.0 * 4 +
                        (eightPos[0][i] - eightPos[3][i]) / 9.0 + (eightPos[4][i] - eightPos[0][i]) / 9.0 * 5;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[29] = hXMap.at(hash);

                // 30
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 9.0 * 5 + eightPos[3][i] / 9.0 * 4 +
                        (eightPos[0][i] - eightPos[3][i]) / 9.0 + (eightPos[4][i] - eightPos[0][i]) / 9.0 * 5;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[30] = hXMap.at(hash);

                // 31
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[31] = hXMap.at(hash);

                hE.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[9], ptsIdx[8], ptsIdx[4], ptsIdx[5], ptsIdx[13], ptsIdx[12] });
                hE.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[2], ptsIdx[10], ptsIdx[13], ptsIdx[5], ptsIdx[6], ptsIdx[14] });
                hE.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
                hE.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15], ptsIdx[4], ptsIdx[5], ptsIdx[6], ptsIdx[7] });
                hE.push_back({ ptsIdx[16], ptsIdx[17], ptsIdx[18], ptsIdx[19], ptsIdx[20], ptsIdx[21], ptsIdx[22], ptsIdx[23] });
                hE.push_back({ ptsIdx[0], ptsIdx[8], ptsIdx[17], ptsIdx[16], ptsIdx[4], ptsIdx[12], ptsIdx[21], ptsIdx[20] });
                hE.push_back({ ptsIdx[17], ptsIdx[8], ptsIdx[11], ptsIdx[18], ptsIdx[21], ptsIdx[12], ptsIdx[15], ptsIdx[22] });
                hE.push_back({ ptsIdx[19], ptsIdx[18], ptsIdx[11], ptsIdx[3], ptsIdx[23], ptsIdx[22], ptsIdx[15], ptsIdx[7] });
                hE.push_back({ ptsIdx[20], ptsIdx[21], ptsIdx[22], ptsIdx[23], ptsIdx[4], ptsIdx[12], ptsIdx[15], ptsIdx[7] });
                hE.push_back({ ptsIdx[24], ptsIdx[25], ptsIdx[26], ptsIdx[27], ptsIdx[28], ptsIdx[29], ptsIdx[30], ptsIdx[31] });
                hE.push_back({ ptsIdx[3], ptsIdx[11], ptsIdx[25], ptsIdx[24], ptsIdx[7], ptsIdx[15], ptsIdx[29], ptsIdx[28] });
                hE.push_back({ ptsIdx[11], ptsIdx[10], ptsIdx[26], ptsIdx[25], ptsIdx[15], ptsIdx[14], ptsIdx[30], ptsIdx[29] });
                hE.push_back({ ptsIdx[26], ptsIdx[10], ptsIdx[2], ptsIdx[27], ptsIdx[30], ptsIdx[14], ptsIdx[6], ptsIdx[31] });
                hE.push_back({ ptsIdx[28], ptsIdx[29], ptsIdx[30], ptsIdx[31], ptsIdx[7], ptsIdx[15], ptsIdx[14], ptsIdx[6] });
            }
            else if (tT.first == 6) {
                std::array<size_t, 68> ptsIdx;

                std::array<double, 3> xyz;

                // 0-7
                for (size_t i = 0; i < 8; ++i) {
                    const std::string hash = HASHIT(eightPos[i][0], eightPos[i][1], eightPos[i][2]);
                    if (!hXMap.count(hash)) {
                        hXMap[hash] = hX.size();
                        hX.emplace_back(eightPos[i]);
                    }
                    ptsIdx[i] = hXMap.at(hash);
                }

                // 8
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                std::string hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[8] = hXMap.at(hash);

                // 9
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[9] = hXMap.at(hash);

                // 10
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[10] = hXMap.at(hash);

                // 11
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[11] = hXMap.at(hash);

                // 12
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[12] = hXMap.at(hash);

                // 13
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[13] = hXMap.at(hash);

                // 14
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[14] = hXMap.at(hash);

                // 15
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[5][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[15] = hXMap.at(hash);

                // 16
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[16] = hXMap.at(hash);

                // 17
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 9.0 * 5 + eightPos[3][i] / 9.0 * 4 + (eightPos[1][i] - eightPos[0][i]) / 9.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[17] = hXMap.at(hash);

                // 18
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 9.0 * 5 + eightPos[0][i] / 9.0 * 4 + (eightPos[1][i] - eightPos[0][i]) / 9.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[18] = hXMap.at(hash);

                // 19
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[19] = hXMap.at(hash);

                // 20
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[20] = hXMap.at(hash);

                // 21
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 9.0 * 5 + eightPos[2][i] / 9.0 * 4 + (eightPos[0][i] - eightPos[3][i]) / 9.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[21] = hXMap.at(hash);

                // 22
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 9.0 * 5 + eightPos[3][i] / 9.0 * 4 + (eightPos[0][i] - eightPos[3][i]) / 9.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[22] = hXMap.at(hash);

                // 23
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[23] = hXMap.at(hash);

                // 24
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[24] = hXMap.at(hash);

                // 25
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[25] = hXMap.at(hash);

                // 26
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[26] = hXMap.at(hash);

                // 27
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[27] = hXMap.at(hash);

                // 28
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[28] = hXMap.at(hash);

                // 29
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 18.0 * 13 + eightPos[1][i] / 18.0 * 5 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 5 + (eightPos[4][i] - eightPos[0][i]) / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[29] = hXMap.at(hash);

                // 30
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 18.0 * 11 + eightPos[1][i] / 18.0 * 7 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 5 + (eightPos[4][i] - eightPos[0][i]) / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[30] = hXMap.at(hash);

                // 31
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 18.0 * 13 + eightPos[0][i] / 18.0 * 5 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 11 + (eightPos[4][i] - eightPos[0][i]) / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[31] = hXMap.at(hash);

                // 32
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 18.0 * 13 + eightPos[0][i] / 18.0 * 5 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 13 + (eightPos[4][i] - eightPos[0][i]) / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[32] = hXMap.at(hash);

                // 33
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 18.0 * 13 + eightPos[1][i] / 18.0 * 5 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 13 + (eightPos[4][i] - eightPos[0][i]) / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[33] = hXMap.at(hash);

                // 34
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[34] = hXMap.at(hash);

                // 35
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 18.0 * 17 + eightPos[1][i] / 18.0 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 7 + (eightPos[4][i] - eightPos[0][i]) / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[35] = hXMap.at(hash);

                // 36
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 18.0 * 17 + eightPos[1][i] / 18.0 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 11 + (eightPos[4][i] - eightPos[0][i]) / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[36] = hXMap.at(hash);

                // 37
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[37] = hXMap.at(hash);

                // 38
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[38] = hXMap.at(hash);

                // 39
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 18.0 * 11 + eightPos[1][i] / 18.0 * 7 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 17 + (eightPos[4][i] - eightPos[0][i]) / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[39] = hXMap.at(hash);

                // 40
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 18.0 * 11 + eightPos[0][i] / 18.0 * 7 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 17 + (eightPos[4][i] - eightPos[0][i]) / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[40] = hXMap.at(hash);

                // 41
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[41] = hXMap.at(hash);

                // 42
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[42] = hXMap.at(hash);

                // 43
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[43] = hXMap.at(hash);

                // 44
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[44] = hXMap.at(hash);

                // 45
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[45] = hXMap.at(hash);

                // 46
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[46] = hXMap.at(hash);

                // 47
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 18.0 * 13 + eightPos[1][i] / 18.0 * 5 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 5 + (eightPos[4][i] - eightPos[0][i]) / 3.0 * 2;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[47] = hXMap.at(hash);

                // 48
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 18.0 * 11 + eightPos[1][i] / 18.0 * 7 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 5 + (eightPos[4][i] - eightPos[0][i]) / 3.0 * 2;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[48] = hXMap.at(hash);

                // 49
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 18.0 * 13 + eightPos[0][i] / 18.0 * 5 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 11 + (eightPos[4][i] - eightPos[0][i]) / 3.0 * 2;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[49] = hXMap.at(hash);

                // 50
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 18.0 * 13 + eightPos[0][i] / 18.0 * 5 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 13 + (eightPos[4][i] - eightPos[0][i]) / 3.0 * 2;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[50] = hXMap.at(hash);

                // 51
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 18.0 * 13 + eightPos[1][i] / 18.0 * 5 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 13 + (eightPos[4][i] - eightPos[0][i]) / 3.0 * 2;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[51] = hXMap.at(hash);

                // 52
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[52] = hXMap.at(hash);

                // 53
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 18.0 * 17 + eightPos[1][i] / 18.0 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 7 + (eightPos[4][i] - eightPos[0][i]) / 3.0 * 2;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[53] = hXMap.at(hash);

                // 54
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 18.0 * 17 + eightPos[1][i] / 18.0 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 11 + (eightPos[4][i] - eightPos[0][i]) / 3.0 * 2;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[54] = hXMap.at(hash);

                // 55
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[55] = hXMap.at(hash);

                // 56
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[56] = hXMap.at(hash);

                // 57
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 18.0 * 11 + eightPos[1][i] / 18.0 * 7 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 17 + (eightPos[4][i] - eightPos[0][i]) / 3.0 * 2;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[57] = hXMap.at(hash);

                // 58
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 18.0 * 11 + eightPos[0][i] / 18.0 * 7 +
                        (eightPos[3][i] - eightPos[0][i]) / 18.0 * 17 + (eightPos[4][i] - eightPos[0][i]) / 3.0 * 2;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[58] = hXMap.at(hash);

                // 59
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[59] = hXMap.at(hash);

                // 60
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[60] = hXMap.at(hash);

                // 61
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 9.0 * 5 + eightPos[3][i] / 9.0 * 4 + (eightPos[1][i] - eightPos[0][i]) / 9.0 +
                        (eightPos[4][i] - eightPos[0][i]);
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[61] = hXMap.at(hash);

                // 62
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 9.0 * 5 + eightPos[0][i] / 9.0 * 4 + (eightPos[1][i] - eightPos[0][i]) / 9.0 +
                        (eightPos[4][i] - eightPos[0][i]);
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[62] = hXMap.at(hash);

                // 63
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[63] = hXMap.at(hash);

                // 64
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[64] = hXMap.at(hash);

                // 65
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 9.0 * 5 + eightPos[2][i] / 9.0 * 4 + (eightPos[0][i] - eightPos[3][i]) / 9.0 +
                        (eightPos[4][i] - eightPos[0][i]);
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[65] = hXMap.at(hash);

                // 66
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 9.0 * 5 + eightPos[3][i] / 9.0 * 4 + (eightPos[0][i] - eightPos[3][i]) / 9.0 +
                        (eightPos[4][i] - eightPos[0][i]);
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[66] = hXMap.at(hash);

                // 67
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[67] = hXMap.at(hash);

                hE.push_back({ ptsIdx[0], ptsIdx[1], ptsIdx[9], ptsIdx[8], ptsIdx[24], ptsIdx[25], ptsIdx[30], ptsIdx[29] });
                hE.push_back({ ptsIdx[9], ptsIdx[1], ptsIdx[2], ptsIdx[10], ptsIdx[31], ptsIdx[26], ptsIdx[27], ptsIdx[32] });
                hE.push_back({ ptsIdx[8], ptsIdx[9], ptsIdx[10], ptsIdx[11], ptsIdx[12], ptsIdx[13], ptsIdx[14], ptsIdx[15] });
                hE.push_back({ ptsIdx[0], ptsIdx[8], ptsIdx[17], ptsIdx[16], ptsIdx[24], ptsIdx[29], ptsIdx[35], ptsIdx[34] });
                hE.push_back({ ptsIdx[10], ptsIdx[2], ptsIdx[23], ptsIdx[22], ptsIdx[32], ptsIdx[27], ptsIdx[41], ptsIdx[40] });
                hE.push_back({ ptsIdx[16], ptsIdx[17], ptsIdx[18], ptsIdx[19], ptsIdx[34], ptsIdx[35], ptsIdx[36], ptsIdx[37] });
                hE.push_back({ ptsIdx[20], ptsIdx[21], ptsIdx[22], ptsIdx[23], ptsIdx[38], ptsIdx[39], ptsIdx[40], ptsIdx[41] });
                hE.push_back({ ptsIdx[19], ptsIdx[18], ptsIdx[11], ptsIdx[3], ptsIdx[37], ptsIdx[36], ptsIdx[33], ptsIdx[28] });
                hE.push_back({ ptsIdx[3], ptsIdx[11], ptsIdx[21], ptsIdx[20], ptsIdx[28], ptsIdx[33], ptsIdx[39], ptsIdx[38] });
                hE.push_back({ ptsIdx[24], ptsIdx[25], ptsIdx[30], ptsIdx[29], ptsIdx[42], ptsIdx[43], ptsIdx[48], ptsIdx[47] });
                hE.push_back({ ptsIdx[31], ptsIdx[26], ptsIdx[27], ptsIdx[32], ptsIdx[49], ptsIdx[44], ptsIdx[45], ptsIdx[50] });
                hE.push_back({ ptsIdx[48], ptsIdx[43], ptsIdx[25], ptsIdx[30], ptsIdx[13], ptsIdx[5], ptsIdx[1], ptsIdx[9] });
                hE.push_back({ ptsIdx[31], ptsIdx[26], ptsIdx[44], ptsIdx[49], ptsIdx[9], ptsIdx[1], ptsIdx[5], ptsIdx[13] });
                hE.push_back({ ptsIdx[42], ptsIdx[43], ptsIdx[48], ptsIdx[47], ptsIdx[4], ptsIdx[5], ptsIdx[13], ptsIdx[12] });
                hE.push_back({ ptsIdx[44], ptsIdx[45], ptsIdx[50], ptsIdx[49], ptsIdx[5], ptsIdx[6], ptsIdx[14], ptsIdx[13] });
                hE.push_back({ ptsIdx[47], ptsIdx[48], ptsIdx[30], ptsIdx[29], ptsIdx[12], ptsIdx[13], ptsIdx[9], ptsIdx[8] });
                hE.push_back({ ptsIdx[49], ptsIdx[50], ptsIdx[32], ptsIdx[31], ptsIdx[13], ptsIdx[14], ptsIdx[10], ptsIdx[9] });
                hE.push_back({ ptsIdx[24], ptsIdx[29], ptsIdx[35], ptsIdx[34], ptsIdx[42], ptsIdx[47], ptsIdx[53], ptsIdx[52] });
                hE.push_back({ ptsIdx[32], ptsIdx[27], ptsIdx[41], ptsIdx[40], ptsIdx[50], ptsIdx[45], ptsIdx[59], ptsIdx[58] });
                hE.push_back({ ptsIdx[34], ptsIdx[35], ptsIdx[36], ptsIdx[37], ptsIdx[52], ptsIdx[53], ptsIdx[54], ptsIdx[55] });
                hE.push_back({ ptsIdx[38], ptsIdx[39], ptsIdx[40], ptsIdx[41], ptsIdx[56], ptsIdx[57], ptsIdx[58], ptsIdx[59] });
                hE.push_back({ ptsIdx[52], ptsIdx[53], ptsIdx[54], ptsIdx[55], ptsIdx[60], ptsIdx[61], ptsIdx[62], ptsIdx[63] });
                hE.push_back({ ptsIdx[56], ptsIdx[57], ptsIdx[58], ptsIdx[59], ptsIdx[64], ptsIdx[65], ptsIdx[66], ptsIdx[67] });
                hE.push_back({ ptsIdx[37], ptsIdx[36], ptsIdx[33], ptsIdx[28], ptsIdx[55], ptsIdx[54], ptsIdx[51], ptsIdx[46] });
                hE.push_back({ ptsIdx[28], ptsIdx[33], ptsIdx[39], ptsIdx[38], ptsIdx[46], ptsIdx[51], ptsIdx[57], ptsIdx[56] });
                hE.push_back({ ptsIdx[55], ptsIdx[54], ptsIdx[51], ptsIdx[46], ptsIdx[63], ptsIdx[62], ptsIdx[15], ptsIdx[7] });
                hE.push_back({ ptsIdx[46], ptsIdx[51], ptsIdx[57], ptsIdx[56], ptsIdx[7], ptsIdx[15], ptsIdx[65], ptsIdx[64] });
                hE.push_back({ ptsIdx[53], ptsIdx[35], ptsIdx[36], ptsIdx[54], ptsIdx[61], ptsIdx[17], ptsIdx[18], ptsIdx[62] });
                hE.push_back({ ptsIdx[39], ptsIdx[40], ptsIdx[58], ptsIdx[57], ptsIdx[21], ptsIdx[22], ptsIdx[66], ptsIdx[65] });
                hE.push_back({ ptsIdx[42], ptsIdx[47], ptsIdx[53], ptsIdx[52], ptsIdx[4], ptsIdx[12], ptsIdx[61], ptsIdx[60] });
                hE.push_back({ ptsIdx[50], ptsIdx[45], ptsIdx[59], ptsIdx[58], ptsIdx[14], ptsIdx[6], ptsIdx[67], ptsIdx[66] });
                hE.push_back({ ptsIdx[53], ptsIdx[47], ptsIdx[29], ptsIdx[35], ptsIdx[61], ptsIdx[12], ptsIdx[8], ptsIdx[17] });
                hE.push_back({ ptsIdx[40], ptsIdx[32], ptsIdx[50], ptsIdx[58], ptsIdx[22], ptsIdx[10], ptsIdx[14], ptsIdx[66] });
                hE.push_back({ ptsIdx[36], ptsIdx[33], ptsIdx[51], ptsIdx[54], ptsIdx[18], ptsIdx[11], ptsIdx[15], ptsIdx[62] });
                hE.push_back({ ptsIdx[51], ptsIdx[33], ptsIdx[39], ptsIdx[57], ptsIdx[15], ptsIdx[11], ptsIdx[21], ptsIdx[65] });
                hE.push_back({ ptsIdx[17], ptsIdx[8], ptsIdx[11], ptsIdx[18], ptsIdx[61], ptsIdx[12], ptsIdx[15], ptsIdx[62] });
                hE.push_back({ ptsIdx[11], ptsIdx[10], ptsIdx[22], ptsIdx[21], ptsIdx[15], ptsIdx[14], ptsIdx[66], ptsIdx[65] });
            }
        }
    }

    return Hexahedra(tri, hE, hX);
}