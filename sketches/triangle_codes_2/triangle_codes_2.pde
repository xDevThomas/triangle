
float turtle_angle = 0;
float turtle_move_length = 20.0;
float turtle_turn_angle = PI * 4 / 6;
float turtle_x = 400;
float turtle_y = 400;

int[] code;


void bit(boolean b) {
  // Triangle to the left/right of the turtle.

  // Move forward, turn, move forward, turn,
  // create triangle from the last three points.

  float tx0 = turtle_x, ty0 = turtle_y;
  turtle_x -= turtle_move_length * cos(turtle_angle);
  turtle_y -= turtle_move_length * sin(turtle_angle);
  turtle_angle += b ? -turtle_turn_angle : turtle_turn_angle;
  float tx1 = turtle_x, ty1 = turtle_y;
  turtle_x -= turtle_move_length * cos(turtle_angle);
  turtle_y -= turtle_move_length * sin(turtle_angle);
  turtle_angle += b ? -turtle_turn_angle : turtle_turn_angle;
  float tx2 = turtle_x, ty2 = turtle_y;

  triangle(tx0, ty0, tx1, ty1, tx2, ty2);
} 

void quietBit(boolean b) {
  // Triangle to the left/right of the turtle.
  // Don't draw anything.

  turtle_x -= turtle_move_length * cos(turtle_angle);
  turtle_y -= turtle_move_length * sin(turtle_angle);
  turtle_angle += b ? -turtle_turn_angle : turtle_turn_angle;
  turtle_x -= turtle_move_length * cos(turtle_angle);
  turtle_y -= turtle_move_length * sin(turtle_angle);
  turtle_angle += b ? -turtle_turn_angle : turtle_turn_angle;
} 

void resetTo(float x, float y) {
  stroke(0);
  fill(0, 10);
  turtle_x = x;
  turtle_y = y;
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
  code = new int[8];
}

void improveCode( float baselineX, float baselineY, float goalX, float goalY ) {
  int[] c = new int[code.length];

  for (int iter = 0; iter < 1000; iter++) {

    // Backup copy of code array before we modify it
    arrayCopy(code, c);
  
    // Make random changes  
    for (int flips = 0; flips < 100; flips++) {
      code[(int)random(code.length)] ^= 1 << (int)random(32);
    }
    
    resetTo(width/2, height/2);

    for (int i = 0; i < code.length; i++) {
      quietBits(code[i], 32);
    }
  
    float d1x = baselineX - goalX;
    float d1y = baselineY - goalY;
  
    float d2x = turtle_x - goalX;
    float d2y = turtle_y - goalY;
   
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
  String s = "Shape ";
  for (int i = 0; i < code.length; i++) {
    s += hex(code[i]);
  }
  text(s, 30, 40);
 
  resetTo(width/2, height/2);
  for (int i = 0; i < code.length; i++) {
    bits(code[i], 32);
  }

  improveCode( turtle_x, turtle_y, mouseX, mouseY );
}
