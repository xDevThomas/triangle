import java.util.Map;

float triangle_size = 8;
int word_len = 8;
int code_words = 128;
int max_flip_sqrt = 10;
float goal_x, goal_y;

int turtle_angle = 0;
int turtle_x = 0;
int turtle_y = 0;
HashMap<Long, Boolean> turtle_triangle_memo;

PFont monoFont;
int[] code;
float iterations_per_frame = 200;
float goal_milliseconds_per_improvement = 10.0;

float coordX(int x, int y) {
  // Triangle to cartesian coordinate transform.
  // +X points bottom-right on the triangular grid.
  // This turns out to be a 30 degree angle down from the cartesian axis.
  // +Y is straight down.
  return width/2 + triangle_size * (x * 0.8660254037844387);
}
float coordY(int x, int y) { 
  return height*3/4 + triangle_size * (y + x * 0.5);
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


void bit(boolean b, boolean draw) {
  // Triangle to the left/right of the turtle.

  // Move forward, turn, move forward, turn,
  // create triangle from the last three points.
  // Optionally draw it.

  int tx0 = turtle_x, ty0 = turtle_y;
  move();
  turn(b ? -1 : 1);
  int tx1 = turtle_x, ty1 = turtle_y;
  move();
  turn(b ? -1 : 1);
  int tx2 = turtle_x, ty2 = turtle_y;

  // 16-bit vertex keys
  long k0 = ((tx0 & 0xFF) << 8) | (ty0 & 0xFF);
  long k1 = ((tx1 & 0xFF) << 8) | (ty1 & 0xFF);
  long k2 = ((tx2 & 0xFF) << 8) | (ty2 & 0xFF);

  // Sorted triangle key
  long k;
  if (k0 < k1) {
    if (k0 < k2) {
      if (k1 < k2) {
        k = k0 | (k1 << 16) | (k2 << 32);
      } else {
        k = k0 | (k2 << 16) | (k1 << 32);
      }
    } else {
      k = k2 | (k0 << 16) | (k1 << 32);
    }
  } else {
    if (k0 < k2) {
      k = k1 | (k0 << 16) | (k2 << 32);
    } else {
      if (k1 < k2) {
        k = k1 | (k2 << 16) | (k0 << 32);
      } else {
        k = k2 | (k1 << 16) | (k0 << 32);
      }
    }
  }

  if (turtle_triangle_memo.containsKey(k)) {
    // This is a duplicate triangle.
    return;
  }

  // We've been to this triangle.
  turtle_triangle_memo.put(k, true);

  if (draw) {
    triangle( coordX(tx0, ty0), coordY(tx0, ty0),
              coordX(tx1, ty1), coordY(tx1, ty1),
              coordX(tx2, ty2), coordY(tx2, ty2) );
  }
} 

void reset() {
  stroke(255, 50);
  fill(255, 80);
  turtle_x = 0;
  turtle_y = 0;
  turtle_angle = 0;
  turtle_triangle_memo = new HashMap<Long, Boolean>();
}

void bits(int b, int len, boolean draw) {
  while (len > 0) {
    len--;
    bit(( (b >> len) & 1 ) != 0, draw );
  }
}

void setup() {  
  size(1024, 768);
  monoFont = createFont("Monospaced", 12);
  code = new int[code_words];

  int mask = 0xFFFFFFFF >>> (32 - word_len);
  for (int i = 0; i < code_words; i++) {
    code[i] = (int)random(mask) & mask;
  }
}

void improveCode( int baselineX, int baselineY, int baselineCount, float goalX, float goalY ) {
  int[] c = new int[code.length];

  int iters = (int)iterations_per_frame;
  for (int iter = 0; iter < iters; iter++) {

    // Backup copy of code array before we modify it
    arrayCopy(code, c);
  
    // Make random changes  
    int num_flips = (int)random(max_flip_sqrt);
    num_flips *= num_flips;

    for (int flips = 0; flips < num_flips; flips++) {

      // Different types of changes:
      //   - Flip a single bit
      //   - Invert a word
      //   - Copy a word to another nearby location, up to 16 words away.
      //   - No-op

      int mask = 0xFFFFFFFF >>> (32 - word_len);
      int r = (int)random(0x10000);
      int rWord = (r >> 8) * code.length / 0x100;
      int rBit = mask & (1 << ((r >> 3) & 31));
      int rDelta = ((r >> 3) & 31) - 16;
      int rDest = rWord + rDelta;

      switch (r & 3) {

        case 0:
          // Copy a whole word to a random place.
          // This encourages reuse at a word level, promoting the development of
          // larger structures and evolving word-sized building blocks that replecate more
          // when they're more useful toward the goal.
          
          if (rDest >= 0 && rDest < code.length) {
            code[rDest] = code[rWord];
          }
          break;

        case 1:
          // Flip word. This lets the building blocks work in either chirality.
          
          code[rWord] ^= mask;
          break; 

        case 2:
          // Bit flip / nop

          code[rWord] ^= rBit;
          break;
      } 
    }
    
    reset();

    for (int i = 0; i < code.length; i++) {
      bits(code[i], word_len, false);
    }
  
    float d1x = coordX(baselineX, baselineY) - goalX;
    float d1y = coordY(baselineX, baselineY) - goalY;
  
    float d2x = coordX(turtle_x, turtle_y) - goalX;
    float d2y = coordY(turtle_x, turtle_y) - goalY;
   
    int iCount = turtle_triangle_memo.size();
    float dist1 = d1x*d1x + d1y*d1y;
    float dist2 = d2x*d2x + d2y*d2y;
   
    if (turtle_angle != 0 &&            // Angle changed; pattern will be bounded
        (dist2 < dist1 ||               // A clear improvement in accuracy toward goal
         (turtle_x == baselineX &&      // Or the same endpoint with a different path
          turtle_y == baselineY &&
          iCount >= baselineCount))) {  // And not less complex
   
      // This random change didn't hurt. Keep it!
      baselineX = turtle_x;
      baselineY = turtle_y;
      baselineCount = iCount;
    } else {
      // Revert
      arrayCopy(c, code);
    }
  }
}

void mousePressed() {
  goal_x = mouseX;
  goal_y = mouseY;
}

void mouseDragged() {
  goal_x = mouseX;
  goal_y = mouseY;
}

void draw() {
  background(0);

  fill(180, 96, 96, 190);
  ellipse(goal_x, goal_y, 5, 5);

  fill(200);
  textFont(monoFont);
  String s = "";
  for (int i = 0; i < code.length; i++) {
    if ((i * word_len) % 256 == 0) {
      s += "\n";
    }
    s += hex(code[i], word_len/4) + " ";
  }
  text(s, 15, 0);
  text("iterations_per_frame = " + iterations_per_frame, 15, height - 16);
 
  reset();
 
  for (int i = 0; i < code.length; i++) {
    bits(code[i], word_len, true);
  }
  
  int baselineX = turtle_x;
  int baselineY = turtle_y;
  int baselineCount = turtle_triangle_memo.size();

  noStroke();
  fill(255, 40);  

  // Tiled copies of the pattern
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < code.length; i++) {
      bits(code[i], word_len, true);
    }
  }

  noFill();
  stroke(255, 192);  
  int[] histogram = new int[1 << word_len];
  for (int i = 0; i < code.length; i++) {
    histogram[code[i]]++;
  }
  for (int i = 0; i < histogram.length; i++) {
    int x = i + width - 40 - histogram.length;
    int h = histogram[i];
    if (h > 0) {
      line(x, 100 - h, x, 100);
    }
  }

  int t1 = millis();
  improveCode( baselineX, baselineY, baselineCount, goal_x, goal_y );
  int t2 = millis();
  iterations_per_frame = constrain(iterations_per_frame -
    (t2 - t1 - goal_milliseconds_per_improvement) * 0.8, 1.0, 100000.0); 
}
