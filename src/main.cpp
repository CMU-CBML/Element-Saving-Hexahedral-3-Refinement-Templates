#include "geometry.h"

int main() {
	// methods to initialize the tree
	// 0: shape-diameter-function method (Consistent Mesh Partitioning and Skeletonisation Using the Shape Diameter Function)
	const size_t initializeTreeMethod = 0;
	const double refinementThreshold = 0.5;

	// traverse all .obj files in .\inputBoundaries, and output files in .\outputFiles
	MeshAllObjFiles(initializeTreeMethod, refinementThreshold);

	return 0;
}