#include <iostream>
#include <unordered_map>
#include <set>
#include <vector>
#include <cassert>
using namespace std;

const int N = 2e5;

/*
 * Generates a testcase for the "Missing a Point" Problem:
 *
 *    https://www.codechef.com/problems/PTMSSNG
 *
 * that should cause solutions using an unordered_map to time out,
 * using the "Blowing Up Unordered Map" technique described here:
 *
 *    https://codeforces.com/blog/entry/62393
 *
 * Essentially, generates a constraints-conformant testcase where all 
 * Rectangle coords are the multiples of a known unordered_map bucket_count (p),
 * so all coordinates get mapped to the same bucket, causing worse-case performance.
 *
 * Usually, we would pick p >= 100'000; however, the requirement that
 * coords have absolute value <= 1e9 makes this a little harder - the 
 * "maxCoordMultiplier" is quite small for such large p, and so we get
 * fewer *distinct* coordinates hashing to the same value, and the resulting
 * testcase turns out *not* to cause a timeout for solutions using unordered_map.
 *
 * A smaller, but still reasonably large, value of p like 26267 is a good compromise;
 * a sample unordered_map-based solution takes over one minute to process the testcase
 * resulting from this choice of p.
 */


int main() {
    // On my system, an unordered_map will eventually grow to have p buckets.
    const auto p = 26267;
    const auto maxCoordVal = 1'000'000'000;
    const auto maxCoordMultiplier = maxCoordVal / p;

    struct Vertex
    {
        int x = -1;
        int y = -1;
        bool operator<(const Vertex& other) const
        {
            if (x != other.x)
                return x < other.x;
            return y < other.y;
        }
    };
    struct Rectangle
    {
        Vertex vertices[4];
    };
    vector<Rectangle> rectangles;
    set<Vertex> vertices;

    // Should generate N valid rectangles, where all 4N vertices are distinct, and where each 
    // vertex coord is a multiple of p not exceeding maxCoordVal.
    // Since all coordinates are multiples of p, when the unordered_map grows to have p buckets, 
    // the coordinates will all end up in the same bucket, which is the worse-case scenario for an 
    // unordered_map, efficiency-wise.
    for (int i = 0 ; i < maxCoordMultiplier / 2; i += 2)
    {
        for (int j = 0; j < maxCoordMultiplier / 2; j += 2)
        {
            // Should be a non-degenerate rectangle.
            const Vertex topLeft = {(i * 2) * p, (j * 2) * p};
            const Vertex topRight = {(i * 2 + 1) * p, (j * 2) * p};
            const Vertex bottomLeft = {(i * 2) * p, (j * 2 + 1) * p};
            const Vertex bottomRight = {(i * 2 + 1) * p, (j * 2 + 1) * p};

            Rectangle rectangle { topLeft, topRight, bottomLeft, bottomRight };
            rectangles.push_back(rectangle);

        }
        if (rectangles.size() > N)
            break;
    }

    while (rectangles.size() > N)
        rectangles.pop_back();

    const int numRectangles = rectangles.size();
    // Verify coords are unique.
    for (const auto& rectangle : rectangles)
    {
        for (const auto& vertex : rectangle.vertices)
        {
            assert(vertices.find(vertex) == vertices.end());
            vertices.insert(vertex);
        }
    }
    // Mark one random vertex as not-to-be-printed.
    auto& vertexToOmit = rectangles[rand() % rectangles.size()].vertices[rand() % 4];
    cerr << "Will omit the vertex: " << vertexToOmit.x << " " << vertexToOmit.y << endl;
    vertexToOmit.x = -1;
    vertexToOmit.y = -1;

    cout << 1 << endl;
    cout << numRectangles << endl;
    for (const auto& rectangle : rectangles)
    {
        for (const auto& vertex : rectangle.vertices)
        {
            if (vertex.x == -1 && vertex.y == -1)
                continue;
            cout << vertex.x << " " << vertex.y << endl;
        }
    }
}

