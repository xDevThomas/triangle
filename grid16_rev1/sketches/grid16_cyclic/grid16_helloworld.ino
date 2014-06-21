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
int counter = 0;

void setup()
{
  leds.begin();
}

struct DitheredLED {
  float color[3];
  float residual[3];
};

DitheredLED dither[ledCount];

void loop()
{
  // Background and fade.
  for (unsigned i = 0; i < ledCount; ++i) {
    Point p = i;
    unsigned dist = grid.distance(p, center);
    float alpha = 0.5 * (sin(dist * 0.4 + millis() * 0.002) + 1);
    dither[i].color[0] = dither[i].color[0] * 0.9 + alpha * 0.0125;
    dither[i].color[1] = dither[i].color[1] * 0.9 + alpha * 0.0031;
    dither[i].color[2] = dither[i].color[2] * 0.9 + alpha * 0.0125;
  }

  dither[p].color[1] += 0.2;
  if (++counter == 8) {
    counter = 0;

    // Move in the dimmest direction
    Point best = Point::invalid();
    float bestMetric = 1e10;
    Point nbr[3];
    unsigned nbrCount = grid.neighbors(p, nbr);
    for (unsigned i = 0; i < nbrCount; ++i) {
      float metric = dither[nbr[i]].color[0] + dither[nbr[i]].color[1] + dither[nbr[i]].color[2];
      if (metric < bestMetric) {
        bestMetric = metric;
        best = nbr[i];
      }
    }

    if (best != Point::invalid()) {
      p = best;
    }
  }

  // LED dithering
  for (unsigned i = 0; i < ledCount; ++i) {
    float fr = dither[i].color[0] + dither[i].residual[0];
    float fg = dither[i].color[1] + dither[i].residual[1];
    float fb = dither[i].color[2] + dither[i].residual[2];
    int r = fr * 255 + 0.5;
    int g = fg * 255 + 0.5;
    int b = fb * 255 + 0.5;
    if (r < 0) r = 0;
    if (r > 255) r = 255;
    if (g < 0) g = 0;
    if (g > 255) g = 255;
    if (b < 0) b = 0;
    if (b > 255) b = 255;
    dither[i].residual[0] = fr - (r / 255.0);
    dither[i].residual[1] = fg - (g / 255.0);
    dither[i].residual[2] = fb - (b / 255.0);
    leds.setPixel(i, r, g, b);
  }

  leds.show();
}
