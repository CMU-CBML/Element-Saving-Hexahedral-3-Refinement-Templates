#include "geometry.h"

void Tree::InitializeTreeMethod0(const double refinementThreshold) {
    // compute shape diameter function for all triangle faces
    tri.ComputeShapeDiameterFunction();
    const auto& sdf = tri.shapeDiameterFunction();
    
    size_t cellIncrease = 1;
    while (true) {
        // adaptively refine cells based on the refinement threshold
        std::cout << "initializing at tree depth: " << c.size() << ", cell increase: " << cellIncrease << std::endl;

        cellIncrease = 0;

        const size_t currentCSize = c.size();

        for (size_t id = 0; id < c[currentCSize - 1].size(); ++id) {
            const auto& currentCell = c[currentCSize - 1][id];

            const size_t cellSize = currentCell.p[0][3] - currentCell.p[0][0];

            // skip the cell if there is no triangle inside or the cell has reached the resolution
            if (currentCell.trisIn.size() == 0 || cellSize == 1) {
                continue;
            }

            // skip the cell if its edge length is smaller than the refinement threshold * 
            bool skip = true;
            for (const size_t triIdx : currentCell.trisIn) {
                if (cellSize > refinementThreshold * sdf[triIdx]) {
                    skip = false;
                    break;
                }
            }
            if (skip) {
                continue;
            }

            RefineCell(currentCSize - 1, id);
            cellIncrease += 26;
        }

        if (c.size() == currentCSize || currentCSize == 10) {
            break;
        }
    }
}