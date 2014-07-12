
float turtle_angle = 0;
float turtle_move_length = 20.0;
float turtle_turn_angle = PI * 4 / 6;
float turtle_x = 400;
float turtle_y = 400;


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

void resetTo(float x, float y) {
  stroke(0);
  fill(0, 10);
  turtle_x = x;
  turtle_y = y;
  turtle_angle = 0;
}

void bitstring(String s) {
  for (int i = 0; i < s.length(); i++) {
    char c = s.charAt(i);
    bit( c == '1' );
  }
}

void bits(int b, int len) {
  while (len > 0) {
    len--;
    bit(( (b >> len) & 1 ) != 0 );
  }
}


void setup() {
  size(800, 600);
}

void draw() {
  int i = mouseY/2;

  background(255);

  fill(0);
  text("Shape 0x" + hex(i, 2), 30, 40);
  text("Tiled 4x", 200, 40);
  text("AA padded", 400, 40);
 
  resetTo(60, 200);
  bits(i, 8);

  resetTo(230, 200);
  bits(i | (i << 8) | (i << 16) | (i << 24), 32);

  resetTo(430, 200);
  bits(0xaa | (i << 8) | (0xaa << 16), 24);
}
