#pragma once

#include <algorithm>
#include <array>
#include <cfloat>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// encode three numbers
#define HASHIT(x, y, z) std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z)

// encode two numbers
#define HASHIT2(x, y) std::to_string(x) + " " + std::to_string(y)

// sort and encode two numbers
#define SORTHASHIT2(x, y) (x > y ? std::to_string(y) + " " + std::to_string(x) : std::to_string(x) + " " + std::to_string(y))

// sort and encode n numbers
// add a space " " before the string to distinguish it from other hash stylesconforming closure
#define SORTHASHITN(boundary) ([&]() { \
    std::vector<size_t> tmp = (boundary); \
    std::sort(tmp.begin(), tmp.end()); \
    std::string s; \
    for (size_t i = 0; i < tmp.size(); ++i) { \
        s += " " + std::to_string(tmp[i]); \
    } \
    return s; \
}())

class Triangle {
private:
    // the side length of the resized bounding cube
    const size_t mS;

    // triangle file name: tFN
    const char* tFN;

    // range of the original input triangles
    // l: the position of the lowest point
    // h: the position of the highest point
    // s: the side length of the bounding cube
    std::array<double, 3> l, h;
    double s;

    // face
    std::vector<std::array<size_t, 3>> f;

    // point coordinate
    // xB is a backup of the original coordinates before resizing
    std::vector<std::array<double, 3>> x, xB;

    // shape diameter function
    std::vector<double> sDF;
public:
    // unify the permutation of the triangle mesh
    void UnifyPermutation();

    // check if a point is inside or outside the triangle mesh
    const bool IsPointInside(const double x, const double y, const double z) const;

    // compute the shortest distance from the triangle mesh to a given point
    const double GetShortestDistance(const double x0, const double y0, const double z0) const;

    // compute the signed distance of a given point
    const double GetSignedDistance(const double x0, const double y0, const double z0) const;

    // extract triangles in the give box
    const std::vector<size_t> GetTrianglesInBox(const std::vector<size_t>& candidateTris,
        const size_t minx, const size_t miny, const size_t minz, const size_t side) const;

    // get the intersection distance t of a ray with the triangle mesh
    const double IntersectRayWithTriangles(const std::array<double, 3>& startPt, const std::array<double, 3>& ray, const size_t startFaceIdx) const;

    // read input triangle mesh
    // initialize bounding cube, scale input triangle mesh to the bounding cube size
    Triangle(const size_t maxSide, const char* triFileName);

    // compute shape diameter function
    void ComputeShapeDiameterFunction();

    // write triangle mesh
    void WriteTriangleToObj(const char* triFileName) const;
    void WriteTriangleToVtk(const char* triFileName) const;

    // output portal
    const size_t maxSide() const;
    const std::array<double, 3>& lowestPoint() const;
    const std::array<double, 3>& highestPoint() const;
    const double sideLength() const;
    const std::vector<std::array<size_t, 3>>& face() const;
    const std::vector<std::array<double, 3>>& point() const;
    const std::vector<double>& shapeDiameterFunction() const;
};

class Hexahedra {
private:
    // hexahedral element
    std::vector<std::array<size_t, 8>> e;

    // point coordinate
    std::vector<std::array<double, 3>> x;

    // triangle mesh address
    Triangle& tri;
public:
    // initialize the hexahedral mesh
    Hexahedra(Triangle& triangleMesh, const std::vector<std::array<size_t, 8>>& elems, const std::vector<std::array<double, 3>>& pts);

    // remove outside hex elements
    void RemoveOutsideElements();

    // write hexahedral mesh
    void WriteHexahedraToVtk(const char* hexFileName) const;
};

class Tree {
public:
    struct Cell {
    public:
        // point coordinate list
        // { { xLow, xMidLow, xMidHigh, xHigh }, { yLow, yMidLow, yMidHigh, yHigh }, { zLow, zMidLow, zMidHigh, zHigh } }
        std::array<std::array<size_t, 4>, 3> p;

        // children
        std::array<size_t, 27> c;

        // triangles inside the cell
        std::vector<size_t> trisIn;

        Cell();
        Cell(const std::vector<size_t>& trisInside, const size_t lx, const size_t ly, const size_t lz, const size_t s);
    };
private:
    // cell
    std::vector<std::vector<Cell>> c;

    // triangle mesh address
    Triangle& tri;

    void InitializeTreeMethod0(const double refinementThreshold);

    Hexahedra RemoveHangingNodesMethod0();
    Hexahedra RemoveHangingNodesMethod1();
    Hexahedra RemoveHangingNodesMethod2();
    Hexahedra RemoveHangingNodesMethod3();
    Hexahedra RemoveHangingNodesMethod4();
public:
    // initialize the tree from [0, 0, 0] to [maxSide, maxSide, maxSide]
    // refine the tree until all cells have refinement error below refinement threshold
    Tree(Triangle& triangleMesh, const size_t initializeTreeMethod, const double refinementThreshold);

    // refine the cell at the specified id
    void RefineCell(const size_t level, const size_t id);

    // choose a method to remove hanging nodes in the adaptive tree
    Hexahedra RemoveHangingNodes(const size_t removeHangingNodesMethod);

    // write tree structure
    void WriteTreeToVtk(const char* octFileName) const;
};

void MeshAllObjFiles(const size_t initializeTreeMethod, const double refinementThreshold);