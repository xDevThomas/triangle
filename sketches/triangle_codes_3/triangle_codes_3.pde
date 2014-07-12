
float triangle_size = 8;
int code_words = 16;

int turtle_angle = 0;
int turtle_x = 0;
int turtle_y = 0;

PFont monoFont;
int[] code;

float coordX(int x, int y) {
  // Triangle to cartesian coordinate transform.
  // +X points bottom-right on the triangular grid.
  // This turns out to be a 30 degree angle down from the cartesian axis.
  // +Y is straight down.
  return width/2 + triangle_size * (x * 0.8660254037844387);
}
float coordY(int x, int y) { 
  return height/2 + triangle_size * (y + x * 0.5);
}   

void move() {
  // Move the turtle forward by one unit
  switch (turtle_angle) {
    case 0:  turtle_y++; break;
    case 1:  turtle_x--; break;
    case 2:  turtle_y--; turtle_x++; break;
  }
}

void turn(int a) {
  // Turn the turtle by an amount
  turtle_angle += a;
  if (turtle_angle < 0) turtle_angle += 3;
  turtle_angle %= 3;
}


void bit(boolean b) {
  // Triangle to the left/right of the turtle.

  // Move forward, turn, move forward, turn,
  // create triangle from the last three points.

  float tx0 = coordX(turtle_x, turtle_y), ty0 = coordY(turtle_x, turtle_y);
  move();
  turn(b ? -1 : 1);
  float tx1 = coordX(turtle_x, turtle_y), ty1 = coordY(turtle_x, turtle_y);
  move();
  turn(b ? -1 : 1);
  float tx2 = coordX(turtle_x, turtle_y), ty2 = coordY(turtle_x, turtle_y);

  triangle(tx0, ty0, tx1, ty1, tx2, ty2);
} 

void quietBit(boolean b) {
  // Triangle to the left/right of the turtle.
  // Don't draw anything.

  move();
  turn(b ? -1 : 1);
  move();
  turn(b ? -1 : 1);
} 

void reset() {
  stroke(0, 80);
  fill(0, 10);
  turtle_x = 0;
  turtle_y = 0;
  turtle_angle = 0;
}

void bits(int b, int len) {
  while (len > 0) {
    len--;
    bit(( (b >> len) & 1 ) != 0 );
  }
}

void quietBits(int b, int len) {
  while (len > 0) {
    len--;
    quietBit(( (b >> len) & 1 ) != 0 );
  }
}

void setup() {  
  size(800, 600);
  monoFont = createFont("Monospaced", 10);
  code = new int[code_words];
}

void improveCode( int baselineX, int baselineY, float goalX, float goalY ) {
  int[] c = new int[code.length];

  for (int iter = 0; iter < 1000; iter++) {

    // Backup copy of code array before we modify it
    arrayCopy(code, c);
  
    // Make random changes  
    for (int flips = 0; flips < 10; flips++) {
      code[(int)random(code.length)] ^= 1 << (int)random(32);
    }
    
    reset();

    for (int i = 0; i < code.length; i++) {
      quietBits(code[i], 32);
    }
  
    float d1x = coordX(baselineX, baselineY) - goalX;
    float d1y = coordY(baselineX, baselineY) - goalY;
  
    float d2x = coordX(turtle_x, turtle_y) - goalX;
    float d2y = coordY(turtle_x, turtle_y) - goalY;
   
    if (d2x*d2x + d2y*d2y <= d1x*d1x + d1y*d1y) {
      // This random change didn't hurt. Keep it!
      baselineX = turtle_x;
      baselineY = turtle_y;
    } else {
      // Revert
      arrayCopy(c, code);
    }
  }
}

void draw() {
  background(255);

  fill(0);
  textFont(monoFont);
  String s = "";
  for (int i = 0; i < code.length; i++) {
    s += hex(code[i]);
  }
  text(s, 15, 25);
 
  reset();
 
  for (int i = 0; i < code.length; i++) {
    bits(code[i], 32);
  }

  improveCode( turtle_x, turtle_y, mouseX, mouseY );
}
