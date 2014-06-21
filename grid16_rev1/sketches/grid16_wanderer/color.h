/*
 * Color utilities.
 * Micah Elizabeth Scott <beth@scanlime.org>
 */

#pragma once

static inline int lerp(int c1, int c2, int alpha) {
  // Linear interpolate, with alpha in range [0, 256].
  // Returns c1 if alpha==0, or c2 if alpha==256.
  int r1 = c1 & 0xFF0000;
  int g1 = c1 & 0x00FF00;
  int b1 = c1 & 0x0000FF;
  int r2 = c2 & 0xFF0000;
  int g2 = c2 & 0x00FF00;
  int b2 = c2 & 0x0000FF;
  int invA = 0x100 - alpha;
  return ((((r1 * invA) + (r2 * alpha)) >> 8) & 0xFF0000) |
         ((((g1 * invA) + (g2 * alpha)) >> 8) & 0x00FF00) |
         ((((b1 * invA) + (b2 * alpha)) >> 8) & 0x0000FF) ;
}

static inline int addsat(int c1, int c2) {
  int r1 = c1 & 0xFF0000;
  int g1 = c1 & 0x00FF00;
  int b1 = c1 & 0x0000FF;
  int r2 = c2 & 0xFF0000;
  int g2 = c2 & 0x00FF00;
  int b2 = c2 & 0x0000FF;
  r1 += r2;
  g1 += g2;
  b1 += b2;
  if (r1 & 0xFF000000) r1 = 0xFF0000;
  if (g1 & 0xFFFF0000) g1 = 0x00FF00;
  if (b1 & 0xFFFFFF00) b1 = 0x0000FF;
  return r1 | g1 | b1;
}
