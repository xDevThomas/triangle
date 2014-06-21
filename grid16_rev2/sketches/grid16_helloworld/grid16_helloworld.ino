#include <OctoWS2811.h>
#include "triangular.h"
#include "hcolor.h"

using namespace Triangular;

static const int ledCount = 16;
DMAMEM int displayMemory[ledCount*6];
int drawingMemory[ledCount*6];
OctoWS2811 leds(ledCount, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz);
HPixelBuffer<ledCount> pixbuf;

#define REV1

// This table describes the way the LED strips are arragned in the 16-pixel triangular grid.
#ifdef REV1
static const uint16_t grid16_lut[] = {
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
static const Point center(9);

#else // REV2

static const uint16_t grid16_lut[] = {
  /*  0 */  Grid::EVEN | Grid::DIR_NEG |                                 2,

  /*  1 */  Grid::EVEN | Grid::DIR_POS |                 Grid::NBR_POS | 7,
  /*  2 */  Grid::ODD  | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | 0,
  /*  3 */  Grid::EVEN | Grid::DIR_POS | Grid::NBR_NEG |                 5,

  /*  4 */  Grid::EVEN | Grid::DIR_NEG | Grid::NBR_NEG |                 14,
  /*  5 */  Grid::ODD  | Grid::DIR_NEG | Grid::NBR_NEG | Grid::NBR_POS | 3,
  /*  6 */  Grid::EVEN | Grid::DIR_NEG | Grid::NBR_NEG | Grid::NBR_POS | 12,
  /*  7 */  Grid::ODD  | Grid::DIR_NEG | Grid::NBR_NEG | Grid::NBR_POS | 1,
  /*  8 */  Grid::EVEN | Grid::DIR_NEG |                 Grid::NBR_POS | 10,

  /*  9 */  Grid::EVEN | Grid::DIR_POS |                 Grid::NBR_POS | Grid::V_NONE,
  /* 10 */  Grid::ODD  | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | 8,
  /* 11 */  Grid::EVEN | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | Grid::V_NONE,
  /* 12 */  Grid::ODD  | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | 6,
  /* 13 */  Grid::EVEN | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | Grid::V_NONE,
  /* 14 */  Grid::ODD  | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | 4,
  /* 15 */  Grid::EVEN | Grid::DIR_POS | Grid::NBR_NEG |                 Grid::V_NONE,
};  
static const Point center(6);
#endif

Grid grid(grid16_lut, sizeof grid16_lut);


void setup()
{
  leds.begin();
}

void loop()
{
  const float bpm = 60;
  float beat = millis() * (bpm / 60000.0f);

  // Background and fade.
  for (unsigned i = 0; i < ledCount; ++i) {
    Point p = i;
    unsigned dist = grid.distance(p, center);
    float alpha = 0.5 * (sin(dist * 0.4 + beat * (M_PI * 2)) + 1);
    pixbuf.pixels[i].color = lerp(HColor8(0), HColor8(0x080000), alpha);
  }

  // Throbbing
  static const int8_t shape[] = {
    -A_AXIS, -C_AXIS, A_AXIS, -B_AXIS, -B_AXIS, -B_AXIS, -B_AXIS,
    -A_AXIS, -A_AXIS, -A_AXIS, -A_AXIS, -A_AXIS,
  };
  HColor colors[] = {
    HColor8(0x00ffff),
    HColor8(0xffffff),
    HColor8(0xff0000),
    HColor8(0x00ff00),
    HColor8(0x0000ff),
  };

  Point p = center;
  float fBeat = beat * 0.6f;
  unsigned nColor = unsigned(fBeat) % (sizeof colors / sizeof colors[0]);
  float flash = 1.0f / (1.0f + fmod(fBeat, 1.0f) * 30.0f);
  HColor c = lerp(HColor8(0), colors[nColor], flash);

  for (int i = 0; i < (sizeof shape / sizeof shape[0]); ++i) { 
    grid.move(p, shape[i]);
    pixbuf.pixels[p].color = pixbuf.pixels[p].color + c;
  }

  pixbuf.show(leds);
}
