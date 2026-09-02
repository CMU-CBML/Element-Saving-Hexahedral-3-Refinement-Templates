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

class TriangleBvh {
private:
    struct Node {
        std::array<double, 3> bmin;
        std::array<double, 3> bmax;
        size_t start; // first index into mTriOrder
        size_t count; // number of triangles
        size_t left; // left child index
        size_t right; // right child index
        bool leaf;
    };
    const std::vector<std::array<double, 3>>& mVertices; // x
    const std::vector<std::array<size_t, 3>>& mFaces; // f
    std::vector<Node> mNodes;
    std::vector<size_t> mTriOrder; // triangle ids grouped by leaf
    std::vector<std::array<double, 3>> mCentroid;  // triangle centroids
public:
    TriangleBvh(const std::vector<std::array<double, 3>>& vertices,
        const std::vector<std::array<size_t, 3>>& faces,
        const size_t maxLeafSize = 4) : mVertices(vertices), mFaces(faces) {
        // maxLeafSize is the maximum number of triangles in the leaf nodes
        Build(maxLeafSize == 0 ? 1 : maxLeafSize);
    }

    // closest hit of the line startPt + t * ray with the mesh
    // skips triangle skipFaceIdx, which is the current face idx
    // returns min |t| over all hits, or +inf if none
    double ClosestIntersection(const std::array<double, 3>& startPt, const std::array<double, 3>& ray,
        const size_t skipFaceIdx) const {
        double bestT = std::numeric_limits<double>::infinity();

        struct StackEntry {
            size_t idx = 0;// the next node to be checked
            double minAbsT = 0;
        };
        std::vector<StackEntry> stack;

        double te, tx;
        if (LineBoxInterval(mNodes[0], startPt, ray, te, tx)) {
            stack.push_back({ 0, MinAbsT(te, tx) });
        }

        while (!stack.empty()) {
            const StackEntry e = stack.back();
            stack.pop_back();

            if (e.minAbsT >= bestT) {
                continue;
            }
            const Node& node = mNodes[e.idx];
            if (node.leaf) {
                for (size_t i = 0; i < node.count; ++i) {
                    const size_t fIdx = mTriOrder[node.start + i];
                    if (fIdx == skipFaceIdx) {
                        continue;
                    }
                    const double t = IntersectTriangle(startPt, ray, fIdx);
                    if (t < bestT) {
                        bestT = t;
                    }
                }
            }
            else {
                const std::array<size_t, 2> child = { node.left, node.right };
                std::array<double, 2> minAbs = { 0.0, 0.0 };
                std::array<bool, 2> valid = { false, false };
                for (size_t c = 0; c < 2; ++c) {
                    if (LineBoxInterval(mNodes[child[c]], startPt, ray, te, tx)) {
                        const double m = MinAbsT(te, tx);
                        if (m < bestT) {
                            minAbs[c] = m;
                            valid[c] = true;
                        }
                    }
                }
                if (valid[0] && valid[1]) {
                    // push the farther child first so the nearer one is on top
                    const size_t farFirst = (minAbs[0] <= minAbs[1]) ? 1 : 0;
                    stack.push_back({ child[farFirst], minAbs[farFirst] });
                    stack.push_back({ child[1 - farFirst], minAbs[1 - farFirst] });
                }
                else if (valid[0]) {
                    stack.push_back({ child[0], minAbs[0] });
                }
                else if (valid[1]) {
                    stack.push_back({ child[1], minAbs[1] });
                }
            }
        }
        return bestT;
    }

private:
    void Build(const size_t maxLeafSize) {
        const size_t n = mFaces.size();
        mTriOrder.resize(n);
        mCentroid.resize(n);
        for (size_t i = 0; i < n; ++i) {
            mTriOrder[i] = i;
            const auto& a = mVertices[mFaces[i][0]];
            const auto& b = mVertices[mFaces[i][1]];
            const auto& c = mVertices[mFaces[i][2]];
            for (size_t k = 0; k < 3; ++k) {
                mCentroid[i][k] = (a[k] + b[k] + c[k]) / 3.0;
            }
        }
        BuildRecursive(0, n, maxLeafSize);
    }

    size_t BuildRecursive(const size_t begin, const size_t end, const size_t maxLeafSize) {
        const size_t nodeIdx = mNodes.size();
        mNodes.emplace_back();

        // triangle bbox + centroid bbox of [begin, end)
        std::array<double, 3> bmin = { std::numeric_limits<double>::infinity(),
            std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity() };
        std::array<double, 3> bmax = { -std::numeric_limits<double>::infinity(),
            -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity() };
        std::array<double, 3> cmin = { std::numeric_limits<double>::infinity(),
            std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity() };
        std::array<double, 3> cmax = { -std::numeric_limits<double>::infinity(),
            -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity() };

        for (size_t i = begin; i < end; ++i) {
            const size_t t = mTriOrder[i];
            for (const size_t v : mFaces[t]) {
                for (size_t k = 0; k < 3; ++k) {
                    bmin[k] = std::min(bmin[k], mVertices[v][k]);
                    bmax[k] = std::max(bmax[k], mVertices[v][k]);
                }
            }
            for (size_t k = 0; k < 3; ++k) {
                cmin[k] = std::min(cmin[k], mCentroid[t][k]);
                cmax[k] = std::max(cmax[k], mCentroid[t][k]);
            }
        }

        const size_t count = end - begin;
        Node& node = mNodes[nodeIdx];
        node.bmin = bmin;
        node.bmax = bmax;
        if (count <= maxLeafSize) {
            node.leaf = true;
            node.start = begin;
            node.count = count;
            return nodeIdx;
        }

        // median split along the longest centroid extent
        size_t axis = 0;
        for (size_t k = 1; k < 3; ++k) {
            if (cmax[k] - cmin[k] > cmax[axis] - cmin[axis]) {
                axis = k;
            }
        }
        const size_t mid = begin + count / 2;
        std::nth_element(mTriOrder.begin() + begin, mTriOrder.begin() + mid,
            mTriOrder.begin() + end, [this, axis](const size_t a, const size_t b) {
                return mCentroid[a][axis] < mCentroid[b][axis];
            });

        const size_t leftIdx = BuildRecursive(begin, mid, maxLeafSize);
        const size_t rightIdx = BuildRecursive(mid, end, maxLeafSize);
        // mNodes may have reallocated during recursion -> re-index by id
        mNodes[nodeIdx].leaf = false;
        mNodes[nodeIdx].count = 0;
        mNodes[nodeIdx].left = leftIdx;
        mNodes[nodeIdx].right = rightIdx;
        return nodeIdx;
    }

    // slab test: parameter interval of the line o + t * d inside the node box
    static bool LineBoxInterval(const Node& node,
        const std::array<double, 3>& o, const std::array<double, 3>& d,
        double& tEnter, double& tExit) {
        tEnter = -std::numeric_limits<double>::infinity();
        tExit = std::numeric_limits<double>::infinity();
        for (size_t k = 0; k < 3; ++k) {
            if (d[k] == 0.0) {
                if (o[k] < node.bmin[k] || o[k] > node.bmax[k]) {
                    return false;
                }
            }
            else {
                double t1 = (node.bmin[k] - o[k]) / d[k];
                double t2 = (node.bmax[k] - o[k]) / d[k];
                if (t1 > t2) {
                    std::swap(t1, t2);
                }
                if (t1 > tEnter) {
                    tEnter = t1;
                }
                if (t2 < tExit) {
                    tExit = t2;
                }
                if (tEnter > tExit) {
                    return false;
                }
            }
        }
        return true;
    }

    // smallest |t| the line can attain inside the interval [tEnter, tExit]
    static double MinAbsT(const double tEnter, const double tExit) {
        if (tEnter <= 0.0 && tExit >= 0.0) {
            return 0.0;
        }
        return std::min(std::abs(tEnter), std::abs(tExit));
    }

    // Moller-Trumbore
    double IntersectTriangle(const std::array<double, 3>& startPt, const std::array<double, 3>& ray, const size_t fIdx) const {
        const auto& face = mFaces[fIdx];
        const auto& v0 = mVertices[face[0]];
        const auto& v1 = mVertices[face[1]];
        const auto& v2 = mVertices[face[2]];
        const double e1x = v1[0] - v0[0], e1y = v1[1] - v0[1], e1z = v1[2] - v0[2];
        const double e2x = v2[0] - v0[0], e2y = v2[1] - v0[1], e2z = v2[2] - v0[2];
        const double px = ray[1] * e2z - ray[2] * e2y;
        const double py = ray[2] * e2x - ray[0] * e2z;
        const double pz = ray[0] * e2y - ray[1] * e2x;
        const double det = e1x * px + e1y * py + e1z * pz;
        if (det == 0.0) {
            return std::numeric_limits<double>::infinity();
        }
        const double invDet = 1.0 / det;
        const double tx = startPt[0] - v0[0], ty = startPt[1] - v0[1], tz = startPt[2] - v0[2];
        const double u = (tx * px + ty * py + tz * pz) * invDet;
        if (u < 0.0 || u > 1.0) {
            return std::numeric_limits<double>::infinity();
        }
        const double qx = ty * e1z - tz * e1y;
        const double qy = tz * e1x - tx * e1z;
        const double qz = tx * e1y - ty * e1x;
        const double v = (ray[0] * qx + ray[1] * qy + ray[2] * qz) * invDet;
        if (v < 0.0 || u + v > 1.0) {
            return std::numeric_limits<double>::infinity();
        }
        return std::abs((e2x * qx + e2y * qy + e2z * qz) * invDet);
    }
};

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
