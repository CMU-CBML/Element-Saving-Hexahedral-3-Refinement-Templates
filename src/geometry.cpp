#define _CRT_SECURE_NO_WARNINGS
#include "geometry.h"

Triangle::Triangle(const size_t maxSide, const char* triFileName) : mS(maxSide), tFN(triFileName) {
    // read input triangle mesh
    // merge duplicated points into the same index
    // merge duplicated faces into the same face
    // f[indexOfTriangle][indexOfVertex]
    // x[indexOfVertex][x, y, z]
    // each file read line length cannot exceed 256
    std::cout << "--------------------\n";

    FILE* dF = fopen(tFN, "r");
    if (dF == NULL) {
        std::cerr << "cannot read triangular mesh";
        std::exit(1);
    }
    std::cout << "read triangular mesh " << triFileName << std::endl;
    std::unordered_map<std::string, size_t> uniqueVertexMap;
    std::vector<size_t> rawToUniqueIdxMap;
    std::unordered_set<std::string> faceSet;
    char ln[256];
    std::array<double, 3> xyz;
    size_t n0 = 0, n1 = 0, n2 = 0;
    constexpr double INFTY = std::numeric_limits<double>::infinity();
    l = { INFTY, INFTY, INFTY };
    h = { -INFTY, -INFTY, -INFTY };
    while (fgets(ln, sizeof(ln), dF)) {
        if (strncmp(ln, "v ", 2) == 0) {
            if (sscanf(ln, "v %lf %lf %lf", &xyz[0], &xyz[1], &xyz[2]) == 3) {
                const std::string hsh = HASHIT(xyz[0], xyz[1], xyz[2]);

                if (uniqueVertexMap.find(hsh) == uniqueVertexMap.end()) {
                    const size_t newIndex = x.size();
                    uniqueVertexMap[hsh] = newIndex;
                    x.emplace_back(xyz);
                    rawToUniqueIdxMap.emplace_back(newIndex);
                }
                else {
                    rawToUniqueIdxMap.emplace_back(uniqueVertexMap.at(hsh));
                }
            }
        }
        else if (strncmp(ln, "f ", 2) == 0) {
            if (sscanf(ln, "f %zu %zu %zu", &n0, &n1, &n2) == 3 ||
                sscanf(ln, "f %zu//%*u %zu//%*u %zu//%*u", &n0, &n1, &n2) == 3 ||
                sscanf(ln, "f %zu/%*u %zu/%*u %zu/%*u", &n0, &n1, &n2) == 3 ||
                sscanf(ln, "f %zu/%*u/%*u %zu/%*u/%*u %zu/%*u/%*u", &n0, &n1, &n2) == 3) {
                const size_t idx0 = rawToUniqueIdxMap.at(n0 - 1);
                const size_t idx1 = rawToUniqueIdxMap.at(n1 - 1);
                const size_t idx2 = rawToUniqueIdxMap.at(n2 - 1);

                const std::vector<size_t> sortedIdx = { idx0, idx1, idx2 };
                const std::string faceKey = SORTHASHITN(sortedIdx);
                if (faceSet.count(faceKey)) {
                    continue;
                }
                faceSet.insert(faceKey);

                f.push_back({ idx0, idx1, idx2 });

                for (const size_t& v_idx : { idx0, idx1, idx2 }) {
                    l[0] = std::min(l[0], x[v_idx][0]);
                    l[1] = std::min(l[1], x[v_idx][1]);
                    l[2] = std::min(l[2], x[v_idx][2]);
                    h[0] = std::max(h[0], x[v_idx][0]);
                    h[1] = std::max(h[1], x[v_idx][1]);
                    h[2] = std::max(h[2], x[v_idx][2]);
                }
            }
        }
    }
    fclose(dF);
    xB = x;
    s = std::max({ h[0] - l[0], h[1] - l[1], h[2] - l[2] });

    // rescale
#pragma warning(disable: 6993)
#pragma omp parallel for
    for (long long i = 0; i < static_cast<long long>(x.size()); ++i) {
        x[i][0] = (x[i][0] - 0.5 * l[0] - 0.5 * h[0]) / s * maxSide + maxSide / 2.0;
        x[i][1] = (x[i][1] - 0.5 * l[1] - 0.5 * h[1]) / s * maxSide + maxSide / 2.0;
        x[i][2] = (x[i][2] - 0.5 * l[2] - 0.5 * h[2]) / s * maxSide + maxSide / 2.0;
    }
}

void Triangle::UnifyPermutation() {
    std::cout << "--------------------\nunify triangle mesh permutation\n";

    // map edge to triangle index
    std::unordered_map<std::string, size_t> edgeToTri;

    // neighbors for each triangle
    std::vector<std::vector<size_t>> neighborTris(f.size());

    // build edge-to-triangle-index map and triangle neighbor relationships
    for (size_t i = 0; i < f.size(); ++i) {
        const auto& face = f[i];

        // process each edge of the triangle
        for (size_t j = 0; j < 3; ++j) {
            const size_t v0 = face[j];
            const size_t v1 = face[(j + 1) % 3];

            // create a canonical hash of the edge
            const std::string edgeHash = SORTHASHIT2(v0, v1);
            if (edgeToTri.count(edgeHash)) {
                const size_t otherFace = edgeToTri.at(edgeHash);
                neighborTris[i].push_back(otherFace);
                neighborTris[otherFace].push_back(i);
            }
            else {
                edgeToTri[edgeHash] = i;
            }
        }
    }

    // set of unprocessed triangles
    std::unordered_set<size_t> unprocessedTris;
    for (size_t i = 0; i < f.size(); ++i) {
        unprocessedTris.insert(i);
    }
    // process each triangle
    while (!unprocessedTris.empty()) {
        std::queue<size_t> queue;
        queue.push(*unprocessedTris.begin());
        unprocessedTris.erase(unprocessedTris.begin());

        while (!queue.empty()) {
            const size_t current = queue.front();
            queue.pop();

            for (const size_t neighbor : neighborTris[current]) {
                if (!unprocessedTris.count(neighbor)) {
                    continue;
                }

                const auto& currentFace = f[current];
                auto& neighborFace = f[neighbor];

                // find shared edge between current and neighbor triangles
                std::vector<size_t> sharedVertices;
                const std::unordered_set<size_t> currentVertices(currentFace.begin(), currentFace.end());
                for (const size_t v : neighborFace) {
                    if (currentVertices.count(v)) {
                        sharedVertices.push_back(v);
                    }
                }

                if (sharedVertices.size() < 2) {
                    std::cerr << "triangles share " << sharedVertices.size() << " vertices, expected 2";
                    std::exit(1);
                }

                // check order in current triangle
                bool currentOrder = false;
                for (size_t i = 0; i < 3; ++i) {
                    if (currentFace[i] == sharedVertices[0] && currentFace[(i + 1) % 3] == sharedVertices[1]) {
                        currentOrder = true;
                        break;
                    }
                }

                // check order in neighbor triangle
                bool neighborOrder = false;
                for (size_t i = 0; i < 3; ++i) {
                    if (neighborFace[i] == sharedVertices[0] && neighborFace[(i + 1) % 3] == sharedVertices[1]) {
                        neighborOrder = true;
                        break;
                    }
                }

                // flip neighbor if winding is consistent with current triangle
                if (currentOrder == neighborOrder) {
                    std::swap(neighborFace[0], neighborFace[2]);
                }

                queue.push(neighbor);
                unprocessedTris.erase(neighbor);
            }
        }
    }
}

const bool Triangle::IsPointInside(const double x0, const double y0, const double z0) const {
    double windingNumber = 0;
    // iterate all triangles
#pragma omp parallel for reduction(+:windingNumber)
    for (long long i = 0; i < static_cast<long long>(f.size()); ++i) {
        const auto& face = f[i];
        const size_t v0 = face[0];
        const auto& A = x[v0];
        const double ax = A[0] - x0, ay = A[1] - y0, az = A[2] - z0;
        const double anorm = std::sqrt(ax * ax + ay * ay + az * az);
        const size_t v1 = face[1];
        const size_t v2 = face[2];
        // get corner coordinates
        const auto& B = x[v1];
        const auto& C = x[v2];
        const double bx = B[0] - x0, by = B[1] - y0, bz = B[2] - z0;
        const double cx = C[0] - x0, cy = C[1] - y0, cz = C[2] - z0;
        const double bnorm = std::sqrt(bx * bx + by * by + bz * bz);
        const double cnorm = std::sqrt(cx * cx + cy * cy + cz * cz);
        // compute cross and dot values
        const double numerator = (ay * bz - az * by) * cx
            + (az * bx - ax * bz) * cy + (ax * by - ay * bx) * cz;
        const double denominator = anorm * bnorm * cnorm +
            (ax * bx + ay * by + az * bz) * cnorm +
            (bx * cx + by * cy + bz * cz) * anorm +
            (cx * ax + cy * ay + cz * az) * bnorm;
        // compute contribution to total angle
        windingNumber += std::atan2(numerator, denominator + (denominator == 0));
    }
    // transform to winding number
    // if winding number is between 1+4n and 3+4n, where n is integer, the point is inside the mesh, otherwise the point is outside the mesh
    windingNumber /= 3.141592653589793238;
    const double remainder = std::fmod(windingNumber - 1, 4);
    return (remainder + 4 * (remainder < 0)) <= 2;
}

const double Triangle::GetShortestDistance(const double x0, const double y0, const double z0) const {
    constexpr double INFTY = std::numeric_limits<double>::infinity();
    std::vector<double> distances(f.size());
    // iterate all triangles
#pragma omp parallel for
    for (long long i = 0; i < static_cast<long long>(f.size()); ++i) {
        const auto& face = f[i];
        const auto& a = x[face[0]];
        const std::array<double, 3> ap = { x0 - a[0], y0 - a[1], z0 - a[2] };
        const auto& b = x[face[1]];
        const auto& c = x[face[2]];
        const std::array<double, 3> ab = { b[0] - a[0], b[1] - a[1], b[2] - a[2] };
        const std::array<double, 3> ac = { c[0] - a[0], c[1] - a[1], c[2] - a[2] };
        const std::array<double, 3> bc = { c[0] - b[0], c[1] - b[1], c[2] - b[2] };
        // a
        const double d1 = ab[0] * ap[0] + ab[1] * ap[1] + ab[2] * ap[2];
        const double d2 = ac[0] * ap[0] + ac[1] * ap[1] + ac[2] * ap[2];
        if (d1 <= 0 && d2 <= 0) {
            distances[i] = ap[0] * ap[0] + ap[1] * ap[1] + ap[2] * ap[2];
            continue;
        }
        // b
        const std::array<double, 3> bp = { x0 - b[0], y0 - b[1], z0 - b[2] };
        const double d3 = ab[0] * bp[0] + ab[1] * bp[1] + ab[2] * bp[2];
        const double d4 = ac[0] * bp[0] + ac[1] * bp[1] + ac[2] * bp[2];
        if (d3 >= 0 && d4 <= d3) {
            distances[i] = bp[0] * bp[0] + bp[1] * bp[1] + bp[2] * bp[2];
            continue;
        }
        // c
        const std::array<double, 3> cp = { x0 - c[0], y0 - c[1], z0 - c[2] };
        const double d5 = ab[0] * cp[0] + ab[1] * cp[1] + ab[2] * cp[2];
        const double d6 = ac[0] * cp[0] + ac[1] * cp[1] + ac[2] * cp[2];
        if (d6 >= 0 && d5 <= d6) {
            distances[i] = cp[0] * cp[0] + cp[1] * cp[1] + cp[2] * cp[2];
            continue;
        }
        // ab
        const double vc = d1 * d4 - d3 * d2;
        std::array<double, 3> q{};
        if (vc <= 0 && d1 >= 0 && d3 <= 0) {
            const double v = (d1 == d3 ? INFTY : d1 / (d1 - d3));
            q[0] = (x0 - a[0] - v * ab[0]);
            q[1] = (y0 - a[1] - v * ab[1]);
            q[2] = (z0 - a[2] - v * ab[2]);
            distances[i] = q[0] * q[0] + q[1] * q[1] + q[2] * q[2];
            continue;
        }
        // ac
        const double vb = d5 * d2 - d1 * d6;
        if (vb <= 0 && d2 >= 0 && d6 <= 0) {
            const double v = (d2 == d6 ? INFTY : d2 / (d2 - d6));
            q[0] = (x0 - a[0] - v * ac[0]);
            q[1] = (y0 - a[1] - v * ac[1]);
            q[2] = (z0 - a[2] - v * ac[2]);
            distances[i] = q[0] * q[0] + q[1] * q[1] + q[2] * q[2];
            continue;
        }
        // bc
        const double va = d3 * d6 - d5 * d4;
        if (va <= 0 && d4 >= d3 && d5 >= d6) {
            const double v = (d4 + d5 == d3 + d6 ? INFTY : (d4 - d3) / ((d4 - d3) + (d5 - d6)));
            q[0] = (x0 - b[0] - v * bc[0]);
            q[1] = (y0 - b[1] - v * bc[1]);
            q[2] = (z0 - b[2] - v * bc[2]);
            distances[i] = q[0] * q[0] + q[1] * q[1] + q[2] * q[2];
            continue;
        }
        // abc
        const double denom = (va + vb + vc == 0 ? INFTY : 1.0 / (va + vb + vc));
        const double v = vb * denom;
        const double w = vc * denom;
        q[0] = (x0 - a[0] - v * ab[0] - w * ac[0]);
        q[1] = (y0 - a[1] - v * ab[1] - w * ac[1]);
        q[2] = (z0 - a[2] - v * ab[2] - w * ac[2]);
        distances[i] = q[0] * q[0] + q[1] * q[1] + q[2] * q[2];
    }
    double shortestDistance = distances[0];
    for (const double distance : distances) {
        shortestDistance = std::min(shortestDistance, distance);
    }
    return std::sqrt(shortestDistance);
}

const double Triangle::GetSignedDistance(const double x0, const double y0, const double z0) const {
    return (IsPointInside(x0, y0, z0) ? -GetShortestDistance(x0, y0, z0) : GetShortestDistance(x0, y0, z0));
}

const std::vector<size_t> Triangle::GetTrianglesInBox(const std::vector<size_t>& candidateTris,
    const size_t minx, const size_t miny, const size_t minz, const size_t side) const {
    std::vector<size_t> trisInBox;
    const size_t halfSide = side / 2;
    const double cubeCenterx = double(minx + halfSide);
    const double cubeCentery = double(miny + halfSide);
    const double cubeCenterz = double(minz + halfSide);
    for (const size_t i : candidateTris) {
        const auto& face = f[i];
        const auto& p0 = x[face[0]];
        const auto& p1 = x[face[1]];
        const auto& p2 = x[face[2]];
        const std::array<double, 3> tp0 = { p0[0] - cubeCenterx, p0[1] - cubeCentery, p0[2] - cubeCenterz };
        const std::array<double, 3> tp1 = { p1[0] - cubeCenterx, p1[1] - cubeCentery, p1[2] - cubeCenterz };
        const std::array<double, 3> tp2 = { p2[0] - cubeCenterx, p2[1] - cubeCentery, p2[2] - cubeCenterz };
        const std::array<double, 3> e0 = { tp1[0] - tp0[0], tp1[1] - tp0[1], tp1[2] - tp0[2] };
        const std::array<double, 3> e1 = { tp2[0] - tp1[0], tp2[1] - tp1[1], tp2[2] - tp1[2] };
        const std::array<double, 3> e2 = { tp0[0] - tp2[0], tp0[1] - tp2[1], tp0[2] - tp2[2] };
        const std::array<std::array<double, 3>, 3> triEdges = { e0, e1, e2 };
        auto AxisTest = [&](const std::array<double, 3>& axis) -> bool {
            const double dot0 = tp0[0] * axis[0] + tp0[1] * axis[1] + tp0[2] * axis[2];
            const double dot1 = tp1[0] * axis[0] + tp1[1] * axis[1] + tp1[2] * axis[2];
            const double dot2 = tp2[0] * axis[0] + tp2[1] * axis[1] + tp2[2] * axis[2];
            const double triMin = std::min({ dot0, dot1, dot2 });
            const double triMax = std::max({ dot0, dot1, dot2 });
            const double r = halfSide * (std::abs(axis[0]) + std::abs(axis[1]) + std::abs(axis[2]));
            return !(triMin > r || triMax < -r);
            };
        if (!AxisTest({ 1, 0, 0 }) || !AxisTest({ 0, 1, 0 }) || !AxisTest({ 0, 0, 1 })) {
            continue;
        }
        const std::array<double, 3> normalAxis = {
            e0[1] * e1[2] - e0[2] * e1[1],
            e0[2] * e1[0] - e0[0] * e1[2],
            e0[0] * e1[1] - e0[1] * e1[0]
        };
        if (!AxisTest(normalAxis)) {
            continue;
        }
        bool crossAxesOk = true;
        for (const auto& edge : triEdges) {
            const std::array<double, 3> crossAxisx = { 0, edge[2], -edge[1] };
            const std::array<double, 3> crossAxisy = { -edge[2], 0, edge[0] };
            const std::array<double, 3> crossAxisz = { edge[1], -edge[0], 0 };
            if (!AxisTest(crossAxisx) || !AxisTest(crossAxisy) || !AxisTest(crossAxisz)) {
                crossAxesOk = false;
                break;
            }
        }
        if (!crossAxesOk) {
            continue;
        }
        trisInBox.emplace_back(i);
    }
    return trisInBox;
}

const double Triangle::IntersectRayWithTriangles(const std::array<double, 3>& startPt, const std::array<double, 3>& ray, const size_t startFaceIdx) const {
    double minT = std::numeric_limits<double>::infinity();
    for (size_t fIdx = 0; fIdx < f.size(); ++fIdx) {
        if (fIdx == startFaceIdx) {
            continue;
        }

        const auto& face = f[fIdx];
        const auto& v0 = x[face[0]];
        const auto& v1 = x[face[1]];
        const auto& v2 = x[face[2]];
        const std::array<double, 3> edge1 = { v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
        const std::array<double, 3> edge2 = { v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2] };
        const std::array<double, 3> pvec = {
            ray[1] * edge2[2] - ray[2] * edge2[1],
            ray[2] * edge2[0] - ray[0] * edge2[2],
            ray[0] * edge2[1] - ray[1] * edge2[0] };
        const double det = edge1[0] * pvec[0] + edge1[1] * pvec[1] + edge1[2] * pvec[2];
        if (det == 0) {
            continue;
        }
        const double invDet = 1.0 / det;
        const std::array<double, 3> tvec = { startPt[0] - v0[0], startPt[1] - v0[1], startPt[2] - v0[2] };
        const double u = (tvec[0] * pvec[0] + tvec[1] * pvec[1] + tvec[2] * pvec[2]) * invDet;
        if (u < 0 || u > 1) {
            continue;
        }
        const std::array<double, 3> qvec = {
            tvec[1] * edge1[2] - tvec[2] * edge1[1],
            tvec[2] * edge1[0] - tvec[0] * edge1[2],
            tvec[0] * edge1[1] - tvec[1] * edge1[0] };
        const double v = (ray[0] * qvec[0] + ray[1] * qvec[1] + ray[2] * qvec[2]) * invDet;
        if (v < 0 || u + v > 1) {
            continue;
        }

        const double t = abs((edge2[0] * qvec[0] + edge2[1] * qvec[1] + edge2[2] * qvec[2]) * invDet);
        minT = std::min(minT, t);
    }
    return minT;
}

void Triangle::WriteTriangleToObj(const char* triFileName) const {
    std::cout << "--------------------\nwrite triangular mesh to obj" << std::endl;

    FILE* dF = fopen(triFileName, "w");
    if (dF == NULL) {
        std::cerr << "cannot write triangular mesh";
        std::exit(1);
    }
    for (const auto& xI : xB)
        fprintf(dF, "v %.17e %.17e %.17e\n", xI[0], xI[1], xI[2]);
    for (const auto& fI : f)
        fprintf(dF, "f %zu %zu %zu\n", fI[0] + 1, fI[1] + 1, fI[2] + 1);
    fclose(dF);
}

void Triangle::WriteTriangleToVtk(const char* triFileName) const {
    std::cout << "--------------------\nwrite triangular mesh to vtk" << std::endl;

    FILE* dF = fopen(triFileName, "w");
    if (dF == NULL) {
        std::cerr << "cannot write triangular mesh";
        std::exit(1);
    }
    fprintf(dF, "# vtk DataFile Version 2.0\nTree_Hex\nASCII\nDATASET UNSTRUCTURED_GRID\nPOINTS %zu double\n", xB.size());
    for (const auto& xI : xB)
        fprintf(dF, "%.17e %.17e %.17e\n", xI[0], xI[1], xI[2]);
    fprintf(dF, "CELLS %zu %zu\n", f.size(), f.size() * 4);
    for (const auto& fI : f)
        fprintf(dF, "3 %zu %zu %zu\n", fI[0], fI[1], fI[2]);
    fprintf(dF, "CELL_TYPES %zu\n", f.size());
    for (size_t i = 0; i < f.size(); ++i)
        fprintf(dF, "5\n");
    fclose(dF);
}

const size_t Triangle::maxSide() const { return mS; }
const std::array<double, 3>& Triangle::lowestPoint() const { return l; }
const std::array<double, 3>& Triangle::highestPoint() const { return h; }
const double Triangle::sideLength() const { return s; }
const std::vector<std::array<size_t, 3>>& Triangle::face() const { return f; }
const std::vector<std::array<double, 3>>& Triangle::point() const { return x; }
const std::vector<double>& Triangle::shapeDiameterFunction() const { return sDF; }

Tree::Cell::Cell() {
    p = { { {}, {}, {} } };
    c = {};
}
Tree::Cell::Cell(const std::vector<size_t>& trisInside, const size_t lx, const size_t ly, const size_t lz, const size_t s) : trisIn(trisInside) {
    p = { { { lx, lx + s / 3, lx + s / 3 * 2, lx + s }, { ly, ly + s / 3, ly + s / 3 * 2, ly + s }, { lz, lz + s / 3, lz + s / 3 * 2, lz + s } } };
    c = {};
}

void Tree::RefineCell(const size_t level, const size_t id) {
    // create twenty-seven new cells
    if (c.size() == level + 1) {
        c.emplace_back();
    }
    for (size_t i = 0; i < 27; ++i) {
        c[level][id].c[i] = c[level + 1].size() + i;
    }
    const auto& p = c[level][id].p;
    const size_t side = p[0][1] - p[0][0];
    for (size_t z = 0; z < 3; ++z) {
        for (size_t y = 0; y < 3; ++y) {
            for (size_t x = 0; x < 3; ++x) {
                c[level + 1].emplace_back(tri.GetTrianglesInBox(c[level][id].trisIn, p[0][x], p[1][y], p[2][z], side), p[0][x], p[1][y], p[2][z], side);
            }
        }
    }
}

Tree::Tree(Triangle& triangleMesh, const size_t initializeTreeMethod, const double refinementThreshold) : tri(triangleMesh) {
    std::cout << "--------------------\ninitialize tree" << std::endl;

    const clock_t startTime = std::clock();

    // initialize c
    c.emplace_back();
    std::vector<size_t> allTris(tri.face().size());
#pragma omp parallel for
    for (long long i = 0; i < static_cast<long long>(allTris.size()); ++i) {
        allTris[i] = i;
    }
    c[0].emplace_back(allTris, 0, 0, 0, tri.maxSide());
    
    switch (initializeTreeMethod)
    {
    case 0:
        InitializeTreeMethod0(refinementThreshold);
        break;
    default:
        return;
    }
}

void Tree::WriteTreeToVtk(const char* octFileName) const {
    std::cout << "--------------------\nwrite tree to vtk" << std::endl;

    FILE* dF = fopen(octFileName, "w");
    if (dF == NULL) {
        std::cerr << "cannot write tree mesh";
        std::exit(1);
    }

    std::unordered_map<std::string, size_t> posToIndex;
    std::vector<std::array<double, 3>> verts;
    std::vector<std::array<size_t, 8>> elems;
    for (const auto& cLayer : c) {
        for (const auto& cLayerCell : cLayer) {
            if (cLayerCell.c[0] != cLayerCell.c[1]) {
                continue;
            }
            elems.emplace_back();
            const double x = (double(cLayerCell.p[0][0]) / tri.maxSide() - 0.5) * tri.sideLength() + 0.5 * tri.lowestPoint()[0] + 0.5 * tri.highestPoint()[0];
            const double y = (double(cLayerCell.p[1][0]) / tri.maxSide() - 0.5) * tri.sideLength() + 0.5 * tri.lowestPoint()[1] + 0.5 * tri.highestPoint()[1];
            const double z = (double(cLayerCell.p[2][0]) / tri.maxSide() - 0.5) * tri.sideLength() + 0.5 * tri.lowestPoint()[2] + 0.5 * tri.highestPoint()[2];

            const double side = double((cLayerCell.p[0][3] - cLayerCell.p[0][0])) / tri.maxSide() * tri.sideLength();
            std::string hash = HASHIT(x, y, z);
            if (!posToIndex.count(hash)) {
                posToIndex[hash] = verts.size();
                verts.push_back({ x, y, z });
            }
            elems.back()[0] = posToIndex.at(hash);
            hash = HASHIT(x + side, y, z);
            if (!posToIndex.count(hash)) {
                posToIndex[hash] = verts.size();
                verts.push_back({ x + side, y, z });
            }
            elems.back()[1] = posToIndex.at(hash);
            hash = HASHIT(x + side, y + side, z);
            if (!posToIndex.count(hash)) {
                posToIndex[hash] = verts.size();
                verts.push_back({ x + side, y + side, z });
            }
            elems.back()[2] = posToIndex.at(hash);
            hash = HASHIT(x, y + side, z);
            if (!posToIndex.count(hash)) {
                posToIndex[hash] = verts.size();
                verts.push_back({ x, y + side, z });
            }
            elems.back()[3] = posToIndex.at(hash);
            hash = HASHIT(x, y, z + side);
            if (!posToIndex.count(hash)) {
                posToIndex[hash] = verts.size();
                verts.push_back({ x, y, z + side });
            }
            elems.back()[4] = posToIndex.at(hash);
            hash = HASHIT(x + side, y, z + side);
            if (!posToIndex.count(hash)) {
                posToIndex[hash] = verts.size();
                verts.push_back({ x + side, y, z + side });
            }
            elems.back()[5] = posToIndex.at(hash);
            hash = HASHIT(x + side, y + side, z + side);
            if (!posToIndex.count(hash)) {
                posToIndex[hash] = verts.size();
                verts.push_back({ x + side, y + side, z + side });
            }
            elems.back()[6] = posToIndex.at(hash);
            hash = HASHIT(x, y + side, z + side);
            if (!posToIndex.count(hash)) {
                posToIndex[hash] = verts.size();
                verts.push_back({ x, y + side, z + side });
            }
            elems.back()[7] = posToIndex.at(hash);
        }
    }

    fprintf(dF, "# vtk DataFile Version 2.0\nTree_Hex\nASCII\nDATASET UNSTRUCTURED_GRID\nPOINTS %zu double\n", verts.size());
    for (const auto& x : verts)
        fprintf(dF, "%.17e %.17e %.17e\n", x[0], x[1], x[2]);
    fprintf(dF, "CELLS %zu %zu\n", elems.size(), elems.size() * 9);
    for (const auto& elem : elems)
        fprintf(dF, "8 %zu %zu %zu %zu %zu %zu %zu %zu\n", elem[0], elem[1], elem[2], elem[3], elem[4], elem[5], elem[6], elem[7]);
    fprintf(dF, "CELL_TYPES %zu\n", elems.size());
    for (const auto& elem : elems)
        fprintf(dF, "12\n");
    fclose(dF);
}

Hexahedra Tree::RemoveHangingNodes(const size_t removeHangingNodesMethod) {
    std::cout << "--------------------\nremove hanging nodes in the adaptive tree" << std::endl;

    switch (removeHangingNodesMethod)
    {
    case 0:
        return RemoveHangingNodesMethod0();
    case 1:
        return RemoveHangingNodesMethod1();
    case 2:
        return RemoveHangingNodesMethod2();
    case 3:
        return RemoveHangingNodesMethod3();
    case 4:
        return RemoveHangingNodesMethod4();
    default:
        return Hexahedra(tri, {}, {});
    }
}

Hexahedra::Hexahedra(Triangle& triangleMesh, const std::vector<std::array<size_t, 8>>& elems, const std::vector<std::array<double, 3>>& pts) :
    tri(triangleMesh), e(elems), x(pts) {
    std::cout << "--------------------\ninitialize hexahedral mesh" << std::endl;
}

void Hexahedra::RemoveOutsideElements() {
    std::cout << "--------------------\nremove outside elements" << std::endl;

    tri.UnifyPermutation();
    std::vector<double> signedDistanceField(e.size());
#pragma omp parallel for
    for (long long i = 0; i < static_cast<long long>(e.size()); ++i) {
        std::array<double, 3> centerPt{};
        for (size_t idx : e[i]) {
            centerPt[0] += x[idx][0] / 8.0;
            centerPt[1] += x[idx][1] / 8.0;
            centerPt[2] += x[idx][2] / 8.0;
        }
        signedDistanceField[i] = tri.GetSignedDistance(centerPt[0], centerPt[1], centerPt[2]);
    }

    size_t index = 0;
    e.erase(std::remove_if(e.begin(), e.end(), [&index, &signedDistanceField](const auto& element) {
        return signedDistanceField[index++] > 0;
        }), e.end());

    // remove unused points
    std::vector<int> indexMap(x.size(), -1);
    std::vector<std::array<double, 3>> newX;
    newX.reserve(x.size() / 2);
    for (auto& elem : e) {
        for (auto& pt : elem) {
            int newIdx = indexMap[pt];
            if (newIdx == -1) {
                newIdx = static_cast<int>(newX.size());
                indexMap[pt] = newIdx;
                newX.push_back(x[pt]);
            }
            pt = static_cast<size_t>(newIdx);
        }
    }
    x = std::move(newX);
}

void Hexahedra::WriteHexahedraToVtk(const char* hexFileName) const {
    std::cout << "--------------------\nwrite hexahedral mesh to vtk" << std::endl;

    FILE* dF = fopen(hexFileName, "w");
    if (dF == NULL) {
        std::cerr << "cannot write hexahedral mesh";
        std::exit(1);
    }
    fprintf(dF, "# vtk DataFile Version 2.0\nTree_Hex\nASCII\nDATASET UNSTRUCTURED_GRID\nPOINTS %zu double\n", x.size());
    for (const auto& xI : x) {
        const double xT = (xI[0] / tri.maxSide() - 0.5) * tri.sideLength() + 0.5 * tri.lowestPoint()[0] + 0.5 * tri.highestPoint()[0];
        const double yT = (xI[1] / tri.maxSide() - 0.5) * tri.sideLength() + 0.5 * tri.lowestPoint()[1] + 0.5 * tri.highestPoint()[1];
        const double zT = (xI[2] / tri.maxSide() - 0.5) * tri.sideLength() + 0.5 * tri.lowestPoint()[2] + 0.5 * tri.highestPoint()[2];
        fprintf(dF, "%.17e %.17e %.17e\n", xT, yT, zT);
    }
    fprintf(dF, "CELLS %zu %zu\n", e.size(), e.size() * 9);
    for (const auto& eI : e) {
        fprintf(dF, "8 %zu %zu %zu %zu %zu %zu %zu %zu\n", eI[0], eI[1], eI[2], eI[3], eI[4], eI[5], eI[6], eI[7]);
    }
    fprintf(dF, "CELL_TYPES %zu\n", e.size());
    for (size_t i = 0; i < e.size(); ++i) {
        fprintf(dF, "12\n");
    }
    fclose(dF);
}

void MeshAllObjFiles(const size_t initializeTreeMethod, const double refinementThreshold) {
    // the side length of the bounding cube
    // range: 3^n, n = 0, 1, ..., inf
    // default value: 3^17 = 129140163
    // not recommended to change this value
    const size_t maxSide = 129140163;

    // methods to remove hanging nodes in the adaptive tree
    // 0: vanilla 3-refinement method (Octree-Based Generation of Hexahedral Element Meshes)
    // 1: strongly-balanced 3-refinement method (Octree-Based Reasonable-Quality Hexahedral Mesh Generation Using a New Set of Refinement Templates)
    // 2: moderately-balanced pairing 3-refinement method (A Consistent Octree Hanging Node Elimination Algorithm for Hexahedral Mesh Generation)
    // 3: 256-template moderately-balanced 3-refinement method
    // 4: 4096-template moderately-balanced 3-refinement method
    const size_t removeHangingNodesMethod = 4;

    // define input and output directories
    const std::string inputDir = "./inputBoundaries";
    const std::string outputDir = "./outputFiles";
    std::filesystem::create_directories(outputDir);

    // scan for all .obj files in current directory
    for (const auto& entry : std::filesystem::directory_iterator(inputDir)) {
        if (entry.path().extension() == ".obj") {
            const clock_t startTime = std::clock();
            const std::string inputFileName = entry.path().filename().string();
            const std::string baseName = entry.path().stem().string();

            // initialize triangle mesh
            Triangle boundary(maxSide, (inputDir + "/" + inputFileName).c_str());

            // time for initializing the triangle mesh
            const double initTime = static_cast<double>(std::clock() - startTime) / CLOCKS_PER_SEC;
            const std::string timeFileName = outputDir + "/" + baseName + "-timeCost.txt";
            FILE* timeFile = fopen(timeFileName.c_str(), "w");
            if (timeFile == NULL) {
                std::cerr << "cannot write time cost file";
                std::exit(1);
            }
            fprintf(timeFile, "%.17e\n", initTime);

            // initialize tree
            Tree scaffold(boundary, initializeTreeMethod, refinementThreshold);

            // output initial tree
            scaffold.WriteTreeToVtk((outputDir + "/" + baseName + "-initialTree.vtk").c_str());

            // remove hanging nodes in the tree
            Hexahedra hexMesh = scaffold.RemoveHangingNodes(removeHangingNodesMethod);

            // output refined tree
            scaffold.WriteTreeToVtk((outputDir + "/" + baseName + "-refinedTree.vtk").c_str());

            // output hex mesh scaffold
            hexMesh.WriteHexahedraToVtk((outputDir + "/" + baseName + "-hexMesh.vtk").c_str());

            // time for building the tree
            const double treeTime = static_cast<double>(std::clock() - startTime) / CLOCKS_PER_SEC;
            fprintf(timeFile, "%.17e\n", treeTime);

            // remove outside hex elements
            hexMesh.RemoveOutsideElements();

            // output interior hex mesh
            hexMesh.WriteHexahedraToVtk((outputDir + "/" + baseName + "-interiorHexMesh.vtk").c_str());

            // time for removing outside hexelements
            const double rOTime = static_cast<double>(std::clock() - startTime) / CLOCKS_PER_SEC;
            fprintf(timeFile, "%.17e\n", rOTime);

            fclose(timeFile);
        }
    }
}