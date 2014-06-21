#include <OctoWS2811.h>
#include "triangular.h"
#include "hcolor.h"

using namespace Triangular;

static const int ledCount = 16;
DMAMEM int displayMemory[ledCount*6];
int drawingMemory[ledCount*6];
OctoWS2811 leds(ledCount, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz);
HPixelBuffer<ledCount> pixbuf;

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
static const Point top(6);

void setup()
{
  leds.begin();
}

void loop()
{
  const float bpm = 10;
  float beat = millis() * (bpm / 60000.0f);

  // Background and fade.
  for (unsigned i = 0; i < ledCount; ++i) {
    Point p = i;
    unsigned dist = grid.distance(p, top);
    float alpha = 0.5 * (sin(dist * 0.4 - beat * (M_PI * 2)) + 1);
    pixbuf.pixels[i].color = lerp(HColor8(0), HColor8(0x040000), alpha);
  }

#if 0
  // Throbbing
  static const int8_t shape[] = {
    B_AXIS, 0,
    -A_AXIS, C_AXIS,
    -B_AXIS, 0,
    A_AXIS, -C_AXIS,
  };
  HColor colors[] = {
    HColor8(0x000030),
    HColor8(0x000030),
    HColor8(0x000030),
    HColor8(0x00ffff),
  };
  Point p = center;
  for (unsigned i = 0; i < 4; i++) {
    float flash = 1.0f / (1.0f + fmod(beat*4 + i, 4.0f) * 4.0f);
    HColor c = lerp(HColor8(0), colors[i], flash);

    pixbuf.pixels[p].color = pixbuf.pixels[p].color + c;
    grid.move(p, shape[i*2+0]);
    pixbuf.pixels[p].color = pixbuf.pixels[p].color + c;
    grid.move(p, shape[i*2+1]);
  }
#endif

#if 1
  float alpha = 0.5 * (sin(beat * (M_PI * 0.6)) + 1);
  pixbuf.pixels[center].color = lerp(HColor8(0), HColor8(0x002222), alpha);
#endif

  pixbuf.show(leds);
}
