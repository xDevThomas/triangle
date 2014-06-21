/*
 * Triangular Geometry library.
 *
 * Copyright <c> 2013 Micah Elizabeth Scott. <beth@scanlime.org>
 *
 * This is a set of data types to support drawing on a grid of
 * pixels that are equilateral triangles rather than squares.
 * Instead of an X and Y axis, we use three axes which are not
 * linearly independent. In other words, there is more than one
 * way to describe the coordinates of any particular triangle.
 *
 * This coordinate system begins with an "origin" triangle. Axes
 * are the sides of this triangle, and this triangle's orientation
 * is defined as "even". Adjacent triangles which are rotated 180
 * degrees have the "odd" orientation. Axes are named A, B, and C,
 * and they're laid out in a counterclockwise fashion:
 *
 *           +
 *       /  / \  ^
 *   +C v  / . \  \ +B
 *        +-----+
 *           ->
 *           +A
 *
 * We assume that the triangular grid is made up of LED strips which
 * always follow the A axis. The strips may be oriented along +A or
 * -A, allowing them to zig-zag back and forth. These are the only
 * restrictions. Arbitrary shapes of triangular LED matrix can be
 * expressed by providing a lookup table which includes a few key
 * characteristics for each triangle.
 */

#pragma once
#include <stdint.h>

namespace Triangular {

// Axes, can be negative or positive.
static const int NO_AXIS = 0;
static const int A_AXIS = 1;
static const int B_AXIS = 2;
static const int C_AXIS = 3;


/*
 * Data type for one point on the triangular grid.
 * Stored as a physical LED index. These are lightweight
 * objects that don't themselves know about the grid structure.
 */
class Point {
    uint16_t index;
    static const uint16_t INVALID_INDEX = (uint16_t) -1;

public:
    /// Construct a new Point with a given index
    Point(unsigned index) : index(index) {}

    /// Uninitialized, to support arrays of points.
    Point() {}

    /// Invalid index.
    static Point invalid() {
        return Point(INVALID_INDEX);
    }

    bool operator == (Point other) const {
        return index == other.index;
    }

    /// Cast to index
    operator unsigned () const {
        return index;
    }
};


/*
 * Represents the shape and connectedness of a triangular grid.
 * This object is constructed with a lookup table that holds characteristics
 * for each pixel in the grid. Each LUT entry includes several flags ORed
 * with the index of the vertically adjacent triangle.
 */
class Grid {
    const uint16_t *lut;
    const unsigned count;

public:
    // Triangle orientation
    static const uint16_t EVEN = 0 << 15;
    static const uint16_t ODD = 1 << 15;

    // Strip direction along "A" axis.
    static const uint16_t DIR_POS = 0 << 14;
    static const uint16_t DIR_NEG = 1 << 14;

    // Neighbors along "A" axis.
    static const uint16_t NBR_POS = 1 << 13;
    static const uint16_t NBR_NEG = 1 << 12;

    // Vertical adjacency, low 12 bits
    static const uint16_t V_MASK = 0x0FFF;
    static const uint16_t V_NONE = V_MASK & (-1);

    Grid(const uint16_t *lut, unsigned lutSize)
        : lut(lut), count(lutSize / sizeof lut[0]) {}

    /* 
     * Calculate the next triangle in a particular direction.
     * Returns an invalid point if we moved past an edge, or if
     * the input point was invalid.
     */
    Point next(Point ref, int axis);

    // Move one unit, return 'true' if successful, 'false' if we can't.
    bool move(Point &ref, int axis);

    // Fill an array with all neighbors of a given triangle, and return its neighbor count.
    unsigned neighbors(Point ref, Point results[3]);

    // Measure the distance between two points, using a breadth-first search.
    unsigned distance(Point p1, Point p2);
};


}  // namespace Triangular
