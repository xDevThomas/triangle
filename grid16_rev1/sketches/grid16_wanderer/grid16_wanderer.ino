#include <OctoWS2811.h>
#include "triangular.h"
#include "color.h"

using namespace Triangular;

static const int ledCount = 16;
DMAMEM int displayMemory[ledCount*6];
int drawingMemory[ledCount*6];
OctoWS2811 leds(ledCount, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz);

// This table describes the way the LED strips are arragned in the 16-pixel triangular grid.
static const uint16_t grid16_lut[] =
{
  /*  0 */  Grid::EVEN | Grid::DIR_POS |                 Grid::NBR_POS | Grid::V_NONE,
  /*  1 */  Grid::ODD  | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | 11,
  /*  2 */  Grid::EVEN | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | Grid::V_NONE,
  /*  3 */  Grid::ODD  | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | 9,
  /*  4 */  Grid::EVEN | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | Grid::V_NONE,
  /*  5 */  Grid::ODD  | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | 7,
  /*  6 */  Grid::EVEN | Grid::DIR_POS | Grid::NBR_NEG |                 Grid::V_NONE,

  /*  7 */  Grid::EVEN | Grid::DIR_NEG | Grid::NBR_NEG |                 5,
  /*  8 */  Grid::ODD  | Grid::DIR_NEG | Grid::NBR_NEG | Grid::NBR_POS | 14,
  /*  9 */  Grid::EVEN | Grid::DIR_NEG | Grid::NBR_NEG | Grid::NBR_POS | 3,
  /* 10 */  Grid::ODD  | Grid::DIR_NEG | Grid::NBR_NEG | Grid::NBR_POS | 12,
  /* 11 */  Grid::EVEN | Grid::DIR_NEG |                 Grid::NBR_POS | 1,

  /* 12 */  Grid::EVEN | Grid::DIR_POS |                 Grid::NBR_POS | 10,
  /* 13 */  Grid::ODD  | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | 15,
  /* 14 */  Grid::EVEN | Grid::DIR_POS | Grid::NBR_NEG |                 8,

  /* 15 */  Grid::EVEN | Grid::DIR_NEG |                                 13,
};  
Grid grid(grid16_lut, sizeof grid16_lut);

static const Point center(9);

// Wandering point, starts at origin.
Point p = center;
int axis = 1;
int counter = 0;
unsigned color = 0xffffff;
bool walking = false;

void setup()
{
  leds.begin();
}

// Wander to a random neighbor, but don't eat our tail.
bool randomWalk()
{
  // Max tries before we give up and wait to fade...
  for (int i = 0; i < 20; ++i) {
    Point candidate = grid.next(p, random(1, 4));
    if (candidate != Point::invalid() && leds.getPixel(candidate) == 0) {
      p = candidate;
      return true;
    }
  }

  // Timed out, switch colors
  color = addsat(0x404040, random(0x1000000));

  // Try to find an unoccupied triangle to warp to
  for (int i = 0; i < 20; ++i) {
    Point candidate = random(ledCount);
    if (leds.getPixel(candidate) == 0) {
      p = candidate;
      return true;
    }
  }

  return false;
}

void loop()
{
  // Fade
  for (unsigned i = 0; i < ledCount; ++i)
    leds.setPixel(i, lerp(leds.getPixel(i), 0, 1));

  // Wander
  if (++counter == 20) {
    counter = 0;
    walking = randomWalk();
  }

  if (walking)
    leds.setPixel(p, color);

  leds.show();
  delay(5);
}
