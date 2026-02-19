#include "geometry.h"

Hexahedra Tree::RemoveHangingNodesMethod0() {
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

    std::unordered_map<std::string, size_t> posToMinSide;
    for (const auto& cLayer : c) {
        for (const auto& cLayerCell : cLayer) {
            if (cLayerCell.c[0] != cLayerCell.c[1]) {
                continue;
            }

            for (const auto& cubeCorner : cubeCorners) {
                const std::string hash = HASHIT(cLayerCell.p[0][cubeCorner[0]], cLayerCell.p[1][cubeCorner[1]], cLayerCell.p[2][cubeCorner[2]]);
                if (!posToMinSide.count(hash)) {
                    posToMinSide[hash] = cLayerCell.p[0][3] - cLayerCell.p[0][0];
                }
                else {
                    posToMinSide[hash] = std::min(posToMinSide.at(hash), cLayerCell.p[0][3] - cLayerCell.p[0][0]);
                }
            }
        }
    }

    const std::unordered_map<uint8_t, std::vector<uint8_t>> cubeBlackCodeTable = {
        { 160, { 1, 3 } },
        { 129, { 3, 4 } },
        { 24, { 0, 7 } },
        { 72, { 0, 5 } },
        { 10, { 5, 7 } },
        { 132, { 1, 4 } },
        { 66, { 2, 5 } },
        { 80, { 0, 2 } },
        { 36, { 1, 6 } },
        { 33, { 3, 6 } },
        { 18, { 2, 7 } },
        { 5, { 4, 6 } },

        { 25, { 0 } },
        { 35, { 3 } },
        { 50, { 7 } },
        { 196, { 4 } },
        { 140, { 1 } },
        { 70, { 2 } },
        { 145, { 4 } },
        { 19, { 2 } },
        { 224, { 3 } },
        { 100, { 6 } },
        { 112, { 0 } },
        { 14, { 7 } },
        { 208, { 2 } },
        { 49, { 6 } },
        { 38, { 1 } },
        { 11, { 5 } },
        { 152, { 7 } },
        { 200, { 5 } },
        { 76, { 0 } },
        { 137, { 3 } },
        { 13, { 6 } },
        { 176, { 1 } },
        { 98, { 5 } },
        { 7, { 4 } },

        { 65, { 0, 2, 3, 4, 5, 6 } },
        { 130, { 1, 2, 3, 4, 5, 7 } },
        { 20, { 0, 1, 2, 4, 6, 7 } },
        { 40, { 0, 1, 3, 5, 6, 7 } },

        { 131, { 1, 2, 3, 4, 5 } },
        { 104, { 0, 3, 5, 6, 7 } },
        { 134, { 1, 2, 3, 4, 7 } },
        { 148, { 1, 2, 4, 6, 7 } },
        { 42, { 0, 1, 3, 5, 7 } },
        { 41, { 0, 1, 3, 5, 6 } },
        { 194, { 2, 3, 4, 5, 7 } },
        { 21, { 0, 1, 2, 4, 6 } },
        { 84, { 0, 2, 4, 6, 7 } },
        { 52, { 0, 1, 4, 6, 7 } },
        { 67, { 0, 2, 3, 4, 5 } },
        { 146, { 1, 2, 4, 5, 7 } },
        { 193, { 2, 3, 4, 5, 6 } },
        { 73, { 0, 2, 3, 5, 6 } },
        { 97, { 0, 3, 4, 5, 6 } },
        { 168, { 1, 3, 5, 6, 7 } },
        { 56, { 0, 1, 5, 6, 7 } },
        { 81, { 0, 2, 4, 5, 6 } },
        { 69, { 0, 2, 3, 4, 6 } },
        { 44, { 0, 1, 3, 6, 7 } },
        { 162, { 1, 3, 4, 5, 7 } },
        { 22, { 0, 1, 2, 4, 7 } },
        { 138, { 1, 2, 3, 5, 7 } },
        { 28, { 0, 1, 2, 6, 7 } },

        { 133, { 1, 2, 3, 4, 6 } },
        { 26, { 0, 1, 2, 5, 7 } },
        { 88, { 0, 2, 5, 6, 7 } },
        { 82, { 0, 2, 4, 5, 7 } },
        { 74, { 0, 2, 3, 5, 7 } },
        { 37, { 0, 1, 3, 4, 6 } },
        { 164, { 1, 3, 4, 6, 7 } },
        { 161, { 1, 3, 4, 5, 6 } },

        { 228, { 3, 4, 6, 7 } },
        { 216, { 2, 5, 6, 7 } },
        { 78, { 0, 2, 3, 7 } },
        { 114, { 0, 4, 5, 7 } },
        { 177, { 1, 4, 5, 6 } },
        { 27, { 0, 1, 2, 5 } },
        { 39, { 0, 1, 3, 4 } },
        { 141, { 1, 2, 3, 6 } },

        { 77, { 0, 2, 3, 6 } },
        { 116, { 0, 4, 6, 7 } },
        { 209, { 2, 4, 5, 6 } },
        { 99, { 0, 3, 4, 5 } },
        { 139, { 1, 2, 3, 5 } },
        { 178, { 1, 4, 5, 7 } },
        { 232, { 3, 5, 6, 7 } },
        { 23, { 0, 1, 2, 4 } },
        { 198, { 2, 3, 4, 7 } },
        { 46, { 0, 1, 3, 7 } },
        { 57, { 0, 1, 5, 6 } },
        { 156, { 1, 2, 6, 7 } },

        { 169, { 1, 3, 5, 6 } },
        { 89, { 0, 2, 5, 6 } },
        { 30, { 0, 1, 2, 7 } },
        { 163, { 1, 3, 4, 5 } },
        { 210, { 2, 4, 5, 7 } },
        { 202, { 2, 3, 5, 7 } },
        { 106, { 0, 3, 5, 7 } },
        { 149, { 1, 2, 4, 6 } },
        { 180, { 1, 4, 6, 7 } },
        { 225, { 3, 4, 5, 6 } },
        { 101, { 0, 3, 4, 6 } },
        { 172, { 1, 3, 6, 7 } },
        { 92, { 0, 2, 6, 7 } },
        { 58, { 0, 1, 5, 7 } },
        { 120, { 0, 5, 6, 7 } },
        { 197, { 2, 3, 4, 6 } },
        { 75, { 0, 2, 3, 5 } },
        { 83, { 0, 2, 4, 5 } },
        { 135, { 1, 2, 3, 4 } },
        { 45, { 0, 1, 3, 6 } },
        { 166, { 1, 3, 4, 7 } },
        { 86, { 0, 2, 4, 7 } },
        { 53, { 0, 1, 4, 6 } },
        { 154, { 1, 2, 5, 7 } },

        { 170, { 1, 3, 5, 7 } },
        { 60, { 0, 1, 6, 7 } },
        { 105, { 0, 3, 5, 6 } },
        { 85, { 0, 2, 4, 6 } },
        { 195, { 2, 3, 4, 5 } },
        { 150, { 1, 2, 4, 7 } },

        { 90, { 0, 2, 5, 7 } },
        { 165, { 1, 3, 4, 6 } },

        { 118, { 0, 4, 7 } },
        { 55, { 0, 1, 4 } },
        { 110, { 0, 3, 7 } },
        { 115, { 0, 4, 5 } },
        { 185, { 1, 5, 6 } },
        { 179, { 1, 4, 5 } },
        { 236, { 3, 6, 7 } },
        { 241, { 4, 5, 6 } },
        { 47, { 0, 1, 3 } },
        { 217, { 2, 5, 6 } },
        { 248, { 5, 6, 7 } },
        { 205, { 2, 3, 6 } },
        { 31, { 0, 1, 2 } },
        { 59, { 0, 1, 5 } },
        { 230, { 3, 4, 7 } },
        { 103, { 0, 3, 4 } },
        { 155, { 1, 2, 5 } },
        { 244, { 4, 6, 7 } },
        { 220, { 2, 6, 7 } },
        { 143, { 1, 2, 3 } },
        { 79, { 0, 2, 3 } },
        { 157, { 1, 2, 6 } },
        { 206, { 2, 3, 7 } },
        { 242, { 4, 5, 7 } },

        { 174, { 1, 3, 7 } },
        { 93, { 0, 2, 6 } },
        { 107, { 0, 3, 5 } },
        { 211, { 2, 4, 5 } },
        { 121, { 0, 5, 6 } },
        { 171, { 1, 3, 5 } },
        { 109, { 0, 3, 6 } },
        { 151, { 1, 2, 4 } },
        { 87, { 0, 2, 4 } },
        { 213, { 2, 4, 6 } },
        { 117, { 0, 4, 6 } },
        { 182, { 1, 4, 7 } },
        { 62, { 0, 1, 7 } },
        { 199, { 2, 3, 4 } },
        { 234, { 3, 5, 7 } },
        { 227, { 3, 4, 5 } },
        { 124, { 0, 6, 7 } },
        { 61, { 0, 1, 6 } },
        { 233, { 3, 5, 6 } },
        { 214, { 2, 4, 7 } },
        { 203, { 2, 3, 5 } },
        { 186, { 1, 5, 7 } },
        { 158, { 1, 2, 7 } },
        { 188, { 1, 6, 7 } },

        { 122, { 0, 5, 7 } },
        { 218, { 2, 5, 7 } },
        { 181, { 1, 4, 6 } },
        { 91, { 0, 2, 5 } },
        { 94, { 0, 2, 7 } },
        { 173, { 1, 3, 6 } },
        { 167, { 1, 3, 4 } },
        { 229, { 3, 4, 6 } },

        { 111, { 0, 3 } },
        { 187, { 1, 5 } },
        { 252, { 6, 7 } },
        { 221, { 2, 6 } },
        { 159, { 1, 2 } },
        { 63, { 0, 1 } },
        { 238, { 3, 7 } },
        { 119, { 0, 4 } },
        { 249, { 5, 6 } },
        { 243, { 4, 5 } },
        { 207, { 2, 3 } },
        { 246, { 4, 7 } },

        { 250, { 5, 7 } },
        { 231, { 3, 4 } },
        { 219, { 2, 5 } },
        { 189, { 1, 6 } },
        { 126, { 0, 7 } },
        { 95, { 0, 2 } },
        { 183, { 1, 4 } },
        { 245, { 4, 6 } },
        { 222, { 2, 7 } },
        { 237, { 3, 6 } },
        { 175, { 1, 3 } },
        { 123, { 0, 5 } },

        { 235, { 3, 5 } },
        { 125, { 0, 6 } },
        { 190, { 1, 7 } },
        { 215, { 2, 4 } },

        { 251, { 5 } },
        { 127, { 0 } },
        { 239, { 3 } },
        { 254, { 7 } },
        { 247, { 4 } },
        { 223, { 2 } },
        { 253, { 6 } },
        { 191, { 1 } },

        { 142, { 1, 2, 3, 7 } },
        { 201, { 2, 3, 5, 6 } },
        { 113, { 0, 4, 5, 6 } },
        { 226, { 3, 4, 5, 7 } },
        { 71, { 0, 2, 3, 4 } },
        { 108, { 0, 3, 6, 7 } },
        { 184, { 1, 5, 6, 7 } },
        { 54, { 0, 1, 4, 7 } },
        { 147, { 1, 2, 4, 5 } },
        { 212, { 2, 4, 6, 7 } },
        { 29, { 0, 1, 2, 6 } },
        { 43, { 0, 1, 3, 5 } } };

    const std::unordered_map<uint8_t, std::pair<uint8_t, std::array<uint8_t, 8>>> transitionTemplates = {
        // single cell
        { 0, { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } } },

        // node
        { 32, { 2, { 2, 3, 0, 1, 6, 7, 4, 5 } } },
        { 4, { 2, { 5, 6, 2, 1, 4, 7, 3, 0 } } },
        { 1, { 2, { 7, 6, 5, 4, 3, 2, 1, 0 } } },
        { 128, { 2, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 2, { 2, { 6, 7, 3, 2, 5, 4, 0, 1 } } },
        { 64, { 2, { 1, 2, 3, 0, 5, 6, 7, 4 } } },
        { 8, { 2, { 4, 5, 1, 0, 7, 6, 2, 3 } } },
        { 16, { 2, { 3, 0, 1, 2, 7, 4, 5, 6 } } },

        // edge
        { 12, { 3, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 34, { 3, { 2, 1, 5, 6, 3, 0, 4, 7 } } },
        { 17, { 3, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 3, { 3, { 7, 3, 2, 6, 4, 0, 1, 5 } } },
        { 6, { 3, { 5, 4, 7, 6, 1, 0, 3, 2 } } },
        { 136, { 3, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 96, { 3, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 9, { 3, { 4, 0, 3, 7, 5, 1, 2, 6 } } },
        { 68, { 3, { 1, 0, 4, 5, 2, 3, 7, 6 } } },
        { 144, { 3, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { 192, { 3, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 48, { 3, { 3, 0, 1, 2, 7, 4, 5, 6 } } },

        // face
        { 102, { 4, { 1, 5, 6, 2, 0, 4, 7, 3 } } },
        { 153, { 4, { 0, 3, 7, 4, 1, 2, 6, 5 } } },
        { 51, { 4, { 3, 2, 6, 7, 0, 1, 5, 4 } } },
        { 204, { 4, { 0, 4, 5, 1, 3, 7, 6, 2 } } },
        { 15, { 4, { 4, 7, 6, 5, 0, 3, 2, 1 } } },
        { 240, { 4, { 0, 1, 2, 3, 4, 5, 6, 7 } } },

        // volume
        { 255, { 5, { 0, 1, 2, 3, 4, 5, 6, 7 } } } };

    std::unordered_map<std::string, size_t> hXMap;

    size_t cellIncrease = 0;

    for (size_t level = 0; level < c.size(); ++level) {
        std::cout << "refining at tree depth: " << level + 1 << ", cell increase: " << cellIncrease << std::endl;
        cellIncrease = 0;
        const auto& cLayer = c[level];

        std::unordered_set<std::string> blackPos;
        for (const auto& cLayerCell : cLayer) {
            if (cLayerCell.c[0] != cLayerCell.c[1]) {
                continue;
            }

            for (const auto& cubeCorner : cubeCorners) {
                const std::string hash = HASHIT(cLayerCell.p[0][cubeCorner[0]], cLayerCell.p[1][cubeCorner[1]], cLayerCell.p[2][cubeCorner[2]]);
                if (posToMinSide.at(hash) < 0.4 * (cLayerCell.p[0][3] - cLayerCell.p[0][0])) {
                    blackPos.insert(hash);
                }
            }
        }

        while (true) {
            const size_t blackPosCount = blackPos.size();

            for (const auto& cLayerCell : cLayer) {
                if (cLayerCell.c[0] != cLayerCell.c[1]) {
                    continue;
                }

                uint8_t cubeBlackCode = 0;
                uint8_t digit = 128;
                for (const auto& cubeCorner : cubeCorners) {
                    const std::string hash = HASHIT(cLayerCell.p[0][cubeCorner[0]], cLayerCell.p[1][cubeCorner[1]], cLayerCell.p[2][cubeCorner[2]]);
                    cubeBlackCode += (blackPos.count(hash) ? digit : 0);
                    digit /= 2;
                }

                if (!cubeBlackCodeTable.count(cubeBlackCode)) {
                    continue;
                }

                for (const size_t pos : cubeBlackCodeTable.at(cubeBlackCode)) {
                    const std::string hash = HASHIT(cLayerCell.p[0][cubeCorners[pos][0]], cLayerCell.p[1][cubeCorners[pos][1]], cLayerCell.p[2][cubeCorners[pos][2]]);
                    blackPos.insert(hash);
                }
            }

            if (blackPos.size() == blackPosCount) {
                break;
            }
        }

        for (size_t cLayerIdx = 0; cLayerIdx < cLayer.size(); ++cLayerIdx) {
            const auto& cLayerCell = cLayer[cLayerIdx];

            if (cLayerCell.c[0] != cLayerCell.c[1]) {
                continue;
            }

            uint8_t cubeBlackCode = 0;
            uint8_t digit = 128;
            for (const auto& cubeCorner : cubeCorners) {
                const std::string hash = HASHIT(cLayerCell.p[0][cubeCorner[0]], cLayerCell.p[1][cubeCorner[1]], cLayerCell.p[2][cubeCorner[2]]);
                cubeBlackCode += (blackPos.count(hash) ? digit : 0);
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
                std::array<size_t, 15> ptsIdx;

                std::array<double, 3> xyz;

                // 10
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                std::string hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[10] = hXMap.at(hash);

                // 0
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[0] = hXMap.at(hash);

                // 2
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[2] = hXMap.at(hash);

                // 6
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[6] = hXMap.at(hash);

                // 5
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[5] = hXMap.at(hash);

                // 11
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[11] = hXMap.at(hash);

                // 12
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[12] = hXMap.at(hash);

                // 14
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[14] = hXMap.at(hash);

                // 13
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[13] = hXMap.at(hash);

                // 1
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[1] = hXMap.at(hash);

                // 3
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[3] = hXMap.at(hash);

                // 7
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[7] = hXMap.at(hash);

                // 4
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[4] = hXMap.at(hash);

                // 8
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[5][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[8] = hXMap.at(hash);

                // 9
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[9] = hXMap.at(hash);

                const size_t rotate = tT.second[0];
                const std::array<size_t, 2> xRange = (cubeCorners[rotate][0] == 3 ? std::array<size_t, 2>{ 2, 3 } : std::array<size_t, 2>{ 0, 1 });
                const std::array<size_t, 2> yRange = (cubeCorners[rotate][1] == 3 ? std::array<size_t, 2>{ 2, 3 } : std::array<size_t, 2>{ 0, 1 });
                const std::array<size_t, 2> zRange = (cubeCorners[rotate][2] == 3 ? std::array<size_t, 2>{ 2, 3 } : std::array<size_t, 2>{ 0, 1 });

                c[level + 1].emplace_back(tri.GetTrianglesInBox(cLayerCell.trisIn,
                    cLayerCell.p[0][xRange[0]], cLayerCell.p[1][yRange[0]], cLayerCell.p[2][zRange[0]], cLayerCell.p[0][1] - cLayerCell.p[0][0]),
                    cLayerCell.p[0][xRange[0]], cLayerCell.p[1][yRange[0]], cLayerCell.p[2][zRange[0]], cLayerCell.p[0][1] - cLayerCell.p[0][0]);
                ++cellIncrease;
                for (const size_t x : xRange) {
                    for (const size_t y : yRange) {
                        for (const size_t z : zRange) {
                            const std::string hash = HASHIT(cLayerCell.p[0][x], cLayerCell.p[1][y], cLayerCell.p[2][z]);
                            if (!posToMinSide.count(hash)) {
                                posToMinSide[hash] = cLayerCell.p[0][1] - cLayerCell.p[0][0];
                            }
                            else {
                                posToMinSide[hash] = std::min(posToMinSide.at(hash), cLayerCell.p[0][1] - cLayerCell.p[0][0]);
                            }
                        }
                    }
                }

                hE.push_back({ ptsIdx[1], ptsIdx[2], ptsIdx[6], ptsIdx[4], ptsIdx[8], ptsIdx[12], ptsIdx[14], ptsIdx[10] });
                hE.push_back({ ptsIdx[3], ptsIdx[4], ptsIdx[6], ptsIdx[5], ptsIdx[9], ptsIdx[10], ptsIdx[14], ptsIdx[13] });
                hE.push_back({ ptsIdx[7], ptsIdx[8], ptsIdx[10], ptsIdx[9], ptsIdx[11], ptsIdx[12], ptsIdx[14], ptsIdx[13] });
            }
            else if (tT.first == 3) {
                std::array<size_t, 28> ptsIdx;

                std::array<double, 3> xyz;

                // 15
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                std::string hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[15] = hXMap.at(hash);

                // 17
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[5][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[17] = hXMap.at(hash);

                // 21
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i] / 3.0 * 2 + eightPos[3][i] / 3.0 + (eightPos[5][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[21] = hXMap.at(hash);

                // 23
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i] / 3.0 * 2 + eightPos[0][i] / 3.0 + (eightPos[5][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[23] = hXMap.at(hash);

                // 0
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[0] = hXMap.at(hash);

                // 2
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[2] = hXMap.at(hash);

                // 11
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[11] = hXMap.at(hash);

                // 9
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[9] = hXMap.at(hash);

                // 24
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[24] = hXMap.at(hash);

                // 25
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[25] = hXMap.at(hash);

                // 27
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[27] = hXMap.at(hash);

                // 26
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[26] = hXMap.at(hash);

                // 1
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[1] = hXMap.at(hash);

                // 3
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[3] = hXMap.at(hash);

                // 6
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[6] = hXMap.at(hash);

                // 10
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[10] = hXMap.at(hash);

                // 12
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[12] = hXMap.at(hash);

                // 18
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[18] = hXMap.at(hash);

                // 4
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[4] = hXMap.at(hash);

                // 5
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[3][i] / 3.0 + (eightPos[1][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[5] = hXMap.at(hash);

                // 7
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[7] = hXMap.at(hash);

                // 8
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[0][i] / 3.0 + (eightPos[1][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[8] = hXMap.at(hash);

                // 13
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[5][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[13] = hXMap.at(hash);

                // 14
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[14] = hXMap.at(hash);

                // 16
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[16] = hXMap.at(hash);

                // 19
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[19] = hXMap.at(hash);

                // 20
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i] / 3.0 * 2 + eightPos[3][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[20] = hXMap.at(hash);

                // 22
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[0][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[22] = hXMap.at(hash);

                const size_t lnStart = tT.second[0];
                const size_t lnEnd = tT.second[3];
                const std::vector<size_t> xRange = (cubeCorners[lnStart][0] != cubeCorners[lnEnd][0]) ? std::vector<size_t>{ 0, 1, 2, 3 } :
                    (cubeCorners[lnStart][0] == 3 ? std::vector<size_t>{ 2, 3 } : std::vector<size_t>{ 0, 1 });
                const std::vector<size_t> yRange = (cubeCorners[lnStart][1] != cubeCorners[lnEnd][1]) ? std::vector<size_t>{ 0, 1, 2, 3 } :
                    (cubeCorners[lnStart][1] == 3 ? std::vector<size_t>{ 2, 3 } : std::vector<size_t>{ 0, 1 });
                const std::vector<size_t> zRange = (cubeCorners[lnStart][2] != cubeCorners[lnEnd][2]) ? std::vector<size_t>{ 0, 1, 2, 3 } :
                    (cubeCorners[lnStart][2] == 3 ? std::vector<size_t>{ 2, 3 } : std::vector<size_t>{ 0, 1 });
                for (size_t i = 0; i < xRange.size() - 1; ++i) {
                    for (size_t j = 0; j < yRange.size() - 1; ++j) {
                        for (size_t k = 0; k < zRange.size() - 1; ++k) {
                            c[level + 1].emplace_back(tri.GetTrianglesInBox(cLayerCell.trisIn,
                                cLayerCell.p[0][xRange[i]], cLayerCell.p[1][yRange[j]], cLayerCell.p[2][zRange[k]], cLayerCell.p[0][1] - cLayerCell.p[0][0]),
                                cLayerCell.p[0][xRange[i]], cLayerCell.p[1][yRange[j]], cLayerCell.p[2][zRange[k]], cLayerCell.p[0][1] - cLayerCell.p[0][0]);
                            ++cellIncrease;
                        }
                    }
                }
                for (const size_t x : xRange) {
                    for (const size_t y : yRange) {
                        for (const size_t z : zRange) {
                            const std::string hash = HASHIT(cLayerCell.p[0][x], cLayerCell.p[1][y], cLayerCell.p[2][z]);
                            if (!posToMinSide.count(hash)) {
                                posToMinSide[hash] = cLayerCell.p[0][1] - cLayerCell.p[0][0];
                            }
                            else {
                                posToMinSide[hash] = std::min(posToMinSide.at(hash), cLayerCell.p[0][1] - cLayerCell.p[0][0]);
                            }
                        }
                    }
                }

                hE.push_back({ ptsIdx[1], ptsIdx[2], ptsIdx[5], ptsIdx[4], ptsIdx[13], ptsIdx[25], ptsIdx[21], ptsIdx[15] });
                hE.push_back({ ptsIdx[5], ptsIdx[2], ptsIdx[11], ptsIdx[8], ptsIdx[21], ptsIdx[25], ptsIdx[27], ptsIdx[23] });
                hE.push_back({ ptsIdx[7], ptsIdx[8], ptsIdx[11], ptsIdx[10], ptsIdx[17], ptsIdx[23], ptsIdx[27], ptsIdx[19] });
                hE.push_back({ ptsIdx[4], ptsIdx[5], ptsIdx[8], ptsIdx[7], ptsIdx[15], ptsIdx[21], ptsIdx[23], ptsIdx[17] });
                hE.push_back({ ptsIdx[14], ptsIdx[15], ptsIdx[17], ptsIdx[16], ptsIdx[20], ptsIdx[21], ptsIdx[23], ptsIdx[22] });
                hE.push_back({ ptsIdx[12], ptsIdx[13], ptsIdx[15], ptsIdx[14], ptsIdx[24], ptsIdx[25], ptsIdx[21], ptsIdx[20] });
                hE.push_back({ ptsIdx[20], ptsIdx[21], ptsIdx[23], ptsIdx[22], ptsIdx[24], ptsIdx[25], ptsIdx[27], ptsIdx[26] });
                hE.push_back({ ptsIdx[16], ptsIdx[17], ptsIdx[19], ptsIdx[18], ptsIdx[22], ptsIdx[23], ptsIdx[27], ptsIdx[26] });
            }
            else if (tT.first == 4) {
                std::array<size_t, 48> ptsIdx;

                std::array<double, 3> xyz;

                // 21
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                std::string hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[21] = hXMap.at(hash);

                // 22
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[22] = hXMap.at(hash);

                // 25
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[5][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[25] = hXMap.at(hash);

                // 26
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[26] = hXMap.at(hash);

                // 32
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[6][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 6.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[32] = hXMap.at(hash);

                // 33
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[7][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 6.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[33] = hXMap.at(hash);

                // 34
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[5][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 6.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[34] = hXMap.at(hash);

                // 35
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[4][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 6.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                hXMap[hash] = hX.size();
                hX.emplace_back(xyz);
                ptsIdx[35] = hXMap.at(hash);

                // 0
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[0] = hXMap.at(hash);

                // 3
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[3] = hXMap.at(hash);

                // 15
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[15] = hXMap.at(hash);

                // 12
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[12] = hXMap.at(hash);

                // 44
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[44] = hXMap.at(hash);

                // 45
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[45] = hXMap.at(hash);

                // 47
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[47] = hXMap.at(hash);

                // 46
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i];
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[46] = hXMap.at(hash);

                // 1
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[1] = hXMap.at(hash);

                // 2
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[2] = hXMap.at(hash);

                // 4
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[4] = hXMap.at(hash);

                // 7
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[7] = hXMap.at(hash);

                // 8
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[0][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[8] = hXMap.at(hash);

                // 11
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[11] = hXMap.at(hash);

                // 13
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[13] = hXMap.at(hash);

                // 14
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[14] = hXMap.at(hash);

                // 16
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[16] = hXMap.at(hash);

                // 19
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[5][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[19] = hXMap.at(hash);

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

                // 31
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[31] = hXMap.at(hash);

                // 5
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[2][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[5] = hXMap.at(hash);

                // 6
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[3][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[6] = hXMap.at(hash);

                // 9
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[1][i] / 3.0;
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

                // 17
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[5][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[17] = hXMap.at(hash);

                // 18
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[18] = hXMap.at(hash);

                // 20
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[0][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[20] = hXMap.at(hash);

                // 23
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[1][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[23] = hXMap.at(hash);

                // 24
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[4][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[24] = hXMap.at(hash);

                // 27
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[5][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[27] = hXMap.at(hash);

                // 29
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[3][i] / 3.0 * 2 + eightPos[6][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[29] = hXMap.at(hash);

                // 30
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[2][i] / 3.0 * 2 + eightPos[7][i] / 3.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[30] = hXMap.at(hash);

                // 36
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i] / 3.0 * 2 + eightPos[1][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[36] = hXMap.at(hash);

                // 37
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i] / 3.0 * 2 + eightPos[0][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[37] = hXMap.at(hash);

                // 38
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[4][i] / 3.0 * 2 + eightPos[3][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[38] = hXMap.at(hash);

                // 39
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[5][i] / 3.0 * 2 + eightPos[2][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[39] = hXMap.at(hash);

                // 40
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[0][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[40] = hXMap.at(hash);

                // 41
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i] / 3.0 * 2 + eightPos[1][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[41] = hXMap.at(hash);

                // 42
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[7][i] / 3.0 * 2 + eightPos[2][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[42] = hXMap.at(hash);

                // 43
                for (size_t i = 0; i < 3; ++i) {
                    xyz[i] = eightPos[6][i] / 3.0 * 2 + eightPos[3][i] / 3.0 + (eightPos[4][i] - eightPos[0][i]) / 12.0;
                }
                hash = HASHIT(xyz[0], xyz[1], xyz[2]);
                if (!hXMap.count(hash)) {
                    hXMap[hash] = hX.size();
                    hX.emplace_back(xyz);
                }
                ptsIdx[43] = hXMap.at(hash);

                const size_t lnStart = tT.second[0];
                const size_t lnEnd = tT.second[4];
                const std::vector<size_t> xRange = (cubeCorners[lnStart][0] == cubeCorners[lnEnd][0]) ? std::vector<size_t>{ 0, 1, 2, 3 } :
                    (cubeCorners[lnStart][0] == 3 ? std::vector<size_t>{ 2, 3 } : std::vector<size_t>{ 0, 1 });
                const std::vector<size_t> yRange = (cubeCorners[lnStart][1] == cubeCorners[lnEnd][1]) ? std::vector<size_t>{ 0, 1, 2, 3 } :
                    (cubeCorners[lnStart][1] == 3 ? std::vector<size_t>{ 2, 3 } : std::vector<size_t>{ 0, 1 });
                const std::vector<size_t> zRange = (cubeCorners[lnStart][2] == cubeCorners[lnEnd][2]) ? std::vector<size_t>{ 0, 1, 2, 3 } :
                    (cubeCorners[lnStart][2] == 3 ? std::vector<size_t>{ 2, 3 } : std::vector<size_t>{ 0, 1 });
                for (size_t i = 0; i < xRange.size() - 1; ++i) {
                    for (size_t j = 0; j < yRange.size() - 1; ++j) {
                        for (size_t k = 0; k < zRange.size() - 1; ++k) {
                            c[level + 1].emplace_back(tri.GetTrianglesInBox(cLayerCell.trisIn,
                                cLayerCell.p[0][xRange[i]], cLayerCell.p[1][yRange[j]], cLayerCell.p[2][zRange[k]], cLayerCell.p[0][1] - cLayerCell.p[0][0]),
                                cLayerCell.p[0][xRange[i]], cLayerCell.p[1][yRange[j]], cLayerCell.p[2][zRange[k]], cLayerCell.p[0][1] - cLayerCell.p[0][0]);
                            ++cellIncrease;
                        }
                    }
                }
                for (const size_t x : xRange) {
                    for (const size_t y : yRange) {
                        for (const size_t z : zRange) {
                            const std::string hash = HASHIT(cLayerCell.p[0][x], cLayerCell.p[1][y], cLayerCell.p[2][z]);
                            if (!posToMinSide.count(hash)) {
                                posToMinSide[hash] = cLayerCell.p[0][1] - cLayerCell.p[0][0];
                            }
                            else {
                                posToMinSide[hash] = std::min(posToMinSide.at(hash), cLayerCell.p[0][1] - cLayerCell.p[0][0]);
                            }
                        }
                    }
                }

                hE.push_back({ ptsIdx[17], ptsIdx[18], ptsIdx[22], ptsIdx[21], ptsIdx[36], ptsIdx[37], ptsIdx[33], ptsIdx[32] });
                hE.push_back({ ptsIdx[21], ptsIdx[22], ptsIdx[26], ptsIdx[25], ptsIdx[32], ptsIdx[33], ptsIdx[35], ptsIdx[34] });
                hE.push_back({ ptsIdx[25], ptsIdx[26], ptsIdx[30], ptsIdx[29], ptsIdx[34], ptsIdx[35], ptsIdx[43], ptsIdx[42] });
                hE.push_back({ ptsIdx[32], ptsIdx[33], ptsIdx[35], ptsIdx[34], ptsIdx[36], ptsIdx[37], ptsIdx[43], ptsIdx[42] });
                hE.push_back({ ptsIdx[36], ptsIdx[37], ptsIdx[43], ptsIdx[42], ptsIdx[44], ptsIdx[45], ptsIdx[47], ptsIdx[46] });
                hE.push_back({ ptsIdx[16], ptsIdx[17], ptsIdx[21], ptsIdx[20], ptsIdx[44], ptsIdx[36], ptsIdx[32], ptsIdx[38] });
                hE.push_back({ ptsIdx[20], ptsIdx[21], ptsIdx[25], ptsIdx[24], ptsIdx[38], ptsIdx[32], ptsIdx[34], ptsIdx[40] });
                hE.push_back({ ptsIdx[38], ptsIdx[32], ptsIdx[34], ptsIdx[40], ptsIdx[44], ptsIdx[36], ptsIdx[42], ptsIdx[46] });
                hE.push_back({ ptsIdx[24], ptsIdx[25], ptsIdx[29], ptsIdx[28], ptsIdx[40], ptsIdx[34], ptsIdx[42], ptsIdx[46] });
                hE.push_back({ ptsIdx[18], ptsIdx[19], ptsIdx[23], ptsIdx[22], ptsIdx[37], ptsIdx[45], ptsIdx[39], ptsIdx[33] });
                hE.push_back({ ptsIdx[22], ptsIdx[23], ptsIdx[27], ptsIdx[26], ptsIdx[33], ptsIdx[39], ptsIdx[41], ptsIdx[35] });
                hE.push_back({ ptsIdx[33], ptsIdx[39], ptsIdx[41], ptsIdx[35], ptsIdx[37], ptsIdx[45], ptsIdx[47], ptsIdx[43] });
                hE.push_back({ ptsIdx[26], ptsIdx[27], ptsIdx[31], ptsIdx[30], ptsIdx[35], ptsIdx[41], ptsIdx[47], ptsIdx[43] });
            }
            else if (tT.first == 5) {
                RefineCell(level, cLayerIdx);
                cellIncrease += 26;

                for (const size_t x : cLayerCell.p[0]) {
                    for (const size_t y : cLayerCell.p[1]) {
                        for (const size_t z : cLayerCell.p[2]) {
                            const std::string hash = HASHIT(x, y, z);
                            if (!posToMinSide.count(hash)) {
                                posToMinSide[hash] = cLayerCell.p[0][1] - cLayerCell.p[0][0];
                            }
                            else {
                                posToMinSide[hash] = std::min(posToMinSide.at(hash), cLayerCell.p[0][1] - cLayerCell.p[0][0]);
                            }
                        }
                    }
                }
            }
        }
    }

    return Hexahedra(tri, hE, hX);
}