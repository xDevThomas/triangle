/*
 * Triangular Geometry library (implementation)
 *
 * Copyright <c> 2013 Micah Elizabeth Scott. <beth@scanlime.org>
 */

#include "triangular.h"

namespace Triangular {

// Move one unit in the +A axis
static inline Point nextPosA(Point ref, unsigned lutData)
{
    if (lutData & Grid::NBR_POS) {
        if (lutData & Grid::DIR_NEG)
            return Point(unsigned(ref) - 1);
        else
            return Point(unsigned(ref) + 1);
    }
    return Point::invalid();
}

// Move one unit in the -A axis
static inline Point nextNegA(Point ref, unsigned lutData)
{
    if (lutData & Grid::NBR_NEG) {
        if (lutData & Grid::DIR_NEG)
            return Point(unsigned(ref) + 1);
        else
            return Point(unsigned(ref) - 1);
    }
    return Point::invalid();
}

// Move one unit in the vertical direction, between LED strips
static inline Point nextV(Point ref, unsigned lutData)
{
    unsigned v = lutData & Grid::V_MASK;
    if (v == Grid::V_NONE)
        return Point::invalid();
    else
        return Point(v);
}

// Generic move by one unit along any axis
Point Grid::next(Point ref, int axis)
{
    if (ref == Point::invalid())
        return ref;

    unsigned lutData = lut[ref];

    // Decompose into +A, -A, or Vertical motion
    switch (axis) {

    case +A_AXIS:
        return nextPosA(ref, lutData);
    case -A_AXIS:
        return nextNegA(ref, lutData);

    case +B_AXIS:
        if (lutData & ODD)
            return nextV(ref, lutData);
        else
            return nextNegA(ref, lutData);
    case -B_AXIS:
        if (lutData & ODD)
            return nextPosA(ref, lutData);
        else
            return nextV(ref, lutData);

    case +C_AXIS:
        if (lutData & ODD)
            return nextNegA(ref, lutData);
        else
            return nextV(ref, lutData);
    case -C_AXIS:
        if (lutData & ODD)
            return nextV(ref, lutData);
        else
            return nextPosA(ref, lutData);
    
    default:
        return Point::invalid(); 
    }
}

bool Grid::move(Point &ref, int axis)
{
    Point n = next(ref, axis);
    if (n == Point::invalid())
        return false;
    ref = n;
    return true;
}

unsigned Grid::neighbors(Point ref, Point results[3])
{
    if (ref == Point::invalid())
        return 0;

    unsigned lutData = lut[ref];
    unsigned count = 0;

    if ((results[count] = nextNegA(ref, lutData)) != Point::invalid()) count++;
    if ((results[count] = nextPosA(ref, lutData)) != Point::invalid()) count++;
    if ((results[count] = nextV(ref, lutData))    != Point::invalid()) count++;

    return count;
}

unsigned Grid::distance(Point p1, Point p2)
{
    // Current best distance from p1, for each grid location.
    uint16_t distanceMemo[count];
    for (unsigned i = 0; i < count; ++i)
        distanceMemo[i] = 0xFFFF;

    // FIFO queue of triangles to examine
    Point fifo[count];
    unsigned fifoLoad = 0, fifoStore = 0;

    // Visit point p1 first. We stop when we've reached p2.
    fifo[fifoStore++] = p1;
    distanceMemo[p1] = 0;

    // Keep looking as long as the FIFO isn't empty.
    while (fifoLoad != fifoStore) {

        // Dequeue next point to examine
        Point current = fifo[fifoLoad++];
        if (fifoLoad == count)
            fifoLoad = 0;

        // Calculate minimum distance for its neighbors
        unsigned nbrDist = distanceMemo[current] + 1;

        // Can we do better than the already-memoized distances for each neighbor?
        Point nbr[3];
        unsigned nbrCount = neighbors(current, nbr);
        for (unsigned i = 0; i < nbrCount; ++i) {
            Point nbrPoint = nbr[i];

            if (nbrDist < distanceMemo[nbrPoint]) {
                // Yes, we can improve it. Lower this neighbor's distance, and put it
                // back on the FIFO so we'll examine its children later.
                distanceMemo[nbrPoint] = nbrDist;
                fifo[fifoStore++] = nbrPoint;
                if (fifoStore == count)
                    fifoStore = 0;
            }
        }
    }

    // Return the best route we found to p2.
    return distanceMemo[p2];
}


}  // namespace Triangular
