// SENTINEL EXPRESSIONS - 10 robot faces
// 0 neutral  1 happy  2 sad  3 angry  4 surprised
// 5 sleepy   6 love   7 star eyes  8 cute  9 wink
//
// Serial Monitor: type 0-9 to pick a face, 'a' to turn auto-cycling on/off.
// Needs the LovyanGFX library (Wokwi: Library Manager -> + -> LovyanGFX).

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// ===== Choose where this runs =====
// 1 = Wokwi simulator (rectangular ILI9341 screen)
// 0 = the real Sentinel robot (round GC9A01 screen)
#define WOKWI 1

// Smooth (flicker-free) drawing uses an off-screen buffer.
// Wokwi's simulator doesn't handle it well, so it's only ON for the real robot.
#define SMOOTH (!WOKWI)

// ===== Screen setup (same pins as the Sentinel schematic) =====
class LGFX : public lgfx::LGFX_Device {
#if WOKWI
  lgfx::Panel_ILI9341 _panel;
#else
  lgfx::Panel_GC9A01 _panel;
#endif
  lgfx::Bus_SPI _bus;
public:
  LGFX() {
    auto b = _bus.config();
    b.spi_host = SPI2_HOST;
    b.freq_write = 40000000;
    b.spi_3wire = !WOKWI;
    b.pin_sclk = 2;  b.pin_mosi = 1;  b.pin_miso = -1;  b.pin_dc = 5;
    _bus.config(b);
    _panel.setBus(&_bus);
    auto p = _panel.config();
    p.pin_cs = 4;  p.pin_rst = 6;
    p.panel_width = 240;
    p.panel_height = WOKWI ? 320 : 240;
    p.invert = !WOKWI;        // the real GC9A01 needs colour inversion
    _panel.config(p);
    setPanel(&_panel);
  }
};
LGFX tft;
LGFX_Sprite canvas(&tft);   // off-screen buffer (used when SMOOTH is on)
LovyanGFX* gfx = &tft;      // where faces get drawn: the screen, or the buffer

// ===== Colours (red, green, blue: each 0-255) =====
uint16_t MAIN, RED, PINK, YELLOW, BLUE, WHITE, BLUSH, BLACK;

// ===== Face layout =====
const int EYE_L = 78, EYE_R = 162;   // left / right eye centre (x)
const int EY = 105;            // eye centre (y)
const int EW = 52, EH = 70;    // eye width / height
const int MY = 168;            // mouth height (y)

// ---------- Building blocks ----------
void roundEye(int cx, int cy, int w, int h, uint16_t c) {
  gfx->fillRoundRect(cx - w / 2, cy - h / 2, w, h, min(18, h / 2), c);
}

void closedEye(int cx, int cy, uint16_t c) {
  gfx->fillRoundRect(cx - EW / 2, cy - 4, EW, 8, 4, c);
}

void smile(int cx, int cy, int r, int thick, uint16_t c) {   // U shape
  gfx->fillArc(cx, cy, r, r - thick, 20, 160, c);
}

void frown(int cx, int cy, int r, int thick, uint16_t c) {   // upside-down U
  gfx->fillArc(cx, cy, r, r - thick, 200, 340, c);
}

void heart(int x, int y, int s, uint16_t c) {
  gfx->fillCircle(x - s / 2, y, s / 2 + 1, c);
  gfx->fillCircle(x + s / 2, y, s / 2 + 1, c);
  gfx->fillTriangle(x - s - 1, y + 2, x + s + 1, y + 2, x, y + s + s / 2, c);
}

void star(int cx, int cy, int R, uint16_t c) {               // 5-pointed star
  float px[10], py[10];
  for (int i = 0; i < 10; i++) {
    float a = -PI / 2 + i * PI / 5;
    float rad = (i % 2 == 0) ? R : R * 0.45;
    px[i] = cx + cos(a) * rad;
    py[i] = cy + sin(a) * rad;
  }
  for (int i = 0; i < 10; i++) {
    int j = (i + 1) % 10;
    gfx->fillTriangle(cx, cy, px[i], py[i], px[j], py[j], c);
  }
}

void tear(int x, int y, uint16_t c) {
  gfx->fillCircle(x, y, 7, c);
  gfx->fillTriangle(x - 7, y - 1, x + 7, y - 1, x, y - 16, c);
}

// Cut a slanted piece off the top of an eye (for sad / angry looks)
// side: -1 = left eye, +1 = right eye. outer = true cuts the outside corner.
void slantCut(int cx, int side, bool outer) {
  int top = EY - EH / 2 - 1;
  int edgeX = outer ? cx + side * (EW / 2 + 1) : cx - side * (EW / 2 + 1);
  int otherX = outer ? cx - side * (EW / 2 + 1) : cx + side * (EW / 2 + 1);
  gfx->fillTriangle(edgeX, top, edgeX, top + EH * 0.45, otherX, top, BLACK);
}

// ---------- The 10 expressions ----------
// Smile with rounded ends (looks softer)
void roundSmile(int cx, int cy, int r, int thick, int a0, int a1, uint16_t c) {
  gfx->fillArc(cx, cy, r, r - thick, a0, a1, c);
  float mid = r - thick / 2.0;
  gfx->fillCircle(cx + cos(a0 * PI / 180) * mid, cy + sin(a0 * PI / 180) * mid, thick / 2, c);
  gfx->fillCircle(cx + cos(a1 * PI / 180) * mid, cy + sin(a1 * PI / 180) * mid, thick / 2, c);
}

void faceNeutral(bool shut) {
  int lx = 80, rx = 160, ey = 100;              // eyes spaced apart
  if (shut) {
    gfx->fillRoundRect(lx - 12, ey - 3, 24, 6, 3, WHITE);    // blink lines
    gfx->fillRoundRect(rx - 12, ey - 3, 24, 6, 3, WHITE);
  } else {
    gfx->fillEllipse(lx, ey, 12, 18, WHITE);     // tall oval eyes
    gfx->fillEllipse(rx, ey, 12, 18, WHITE);
  }
  roundSmile(120, 130, 34, 4, 50, 130, WHITE);  // thin, wide, gentle smile
}

void faceHappy() {
  // Bounce: hops up and lands, over and over
  int hop = -fabs(sin(millis() / 200.0)) * 8;    // 8 = hop height, 200 = speed
  int lx = 80, rx = 160, ey = 106 + hop;
  roundSmile(lx, ey, 15, 5, 200, 340, YELLOW);    // happy eyes
  roundSmile(rx, ey, 15, 5, 200, 340, YELLOW);
  gfx->fillArc(120, 140 + hop, 26, 0, 0, 180, YELLOW);   // open mouth (moved lower)
}

void faceSad(bool shut) {
  if (shut) { closedEye(EYE_L, EY + 8, MAIN); closedEye(EYE_R, EY + 8, MAIN); }
  else {
    roundEye(EYE_L, EY, EW, EH, MAIN);
    roundEye(EYE_R, EY, EW, EH, MAIN);
    slantCut(EYE_L, -1, true);                    // droopy outer corners
    slantCut(EYE_R, +1, true);
  }
  frown(120, MY + 22, 28, 6, MAIN);
  tear(EYE_L - 14, EY + 52, BLUE);
}

void faceAngry(bool shut) {
  if (shut) { closedEye(EYE_L, EY, RED); closedEye(EYE_R, EY, RED); }
  else {
    roundEye(EYE_L, EY, EW, EH, RED);
    roundEye(EYE_R, EY, EW, EH, RED);
    slantCut(EYE_L, -1, false);                   // frowning inner corners
    slantCut(EYE_R, +1, false);
  }
  frown(120, MY + 18, 24, 6, RED);
}

void faceSurprised() {
  gfx->fillCircle(EYE_L, EY, 34, MAIN);
  gfx->fillCircle(EYE_R, EY, 34, MAIN);
  gfx->fillCircle(EYE_L, EY, 13, BLACK);           // pupils
  gfx->fillCircle(EYE_R, EY, 13, BLACK);
  gfx->fillArc(120, MY + 4, 16, 10, 0, 360, MAIN);   // "O" mouth
}

void faceSleepy() {
  closedEye(EYE_L, EY + 10, MAIN);
  closedEye(EYE_R, EY + 10, MAIN);
  gfx->fillRoundRect(108, MY, 24, 5, 2, MAIN);
  gfx->setTextColor(MAIN);
  gfx->setTextSize(2);  gfx->drawString("z", 168, 62);
  gfx->setTextSize(3);  gfx->drawString("Z", 184, 38);
}

void faceLove() {
  heart(EYE_L, EY - 10, 24, PINK);
  heart(EYE_R, EY - 10, 24, PINK);
  smile(120, MY - 22, 30, 7, PINK);
}

void faceStar() {
  star(EYE_L, EY, 36, YELLOW);
  star(EYE_R, EY, 36, YELLOW);
  smile(120, MY - 26, 38, 8, MAIN);
}

void faceCute(bool shut) {
  if (shut) { closedEye(EYE_L, EY, MAIN); closedEye(EYE_R, EY, MAIN); }
  else {
    gfx->fillCircle(EYE_L, EY, 30, MAIN);
    gfx->fillCircle(EYE_R, EY, 30, MAIN);
    gfx->fillCircle(EYE_L - 10, EY - 10, 9, WHITE);   // sparkle highlights
    gfx->fillCircle(EYE_R - 10, EY - 10, 9, WHITE);
    gfx->fillCircle(EYE_L + 10, EY + 10, 4, WHITE);
    gfx->fillCircle(EYE_R + 10, EY + 10, 4, WHITE);
  }
  gfx->fillEllipse(EYE_L - 12, EY + 44, 14, 7, BLUSH); // rosy cheeks
  gfx->fillEllipse(EYE_R + 12, EY + 44, 14, 7, BLUSH);
  smile(111, MY - 8, 10, 3, MAIN);                 // little "w" mouth
  smile(129, MY - 8, 10, 3, MAIN);
}

void faceWink() {
  roundEye(EYE_L, EY, EW, EH, MAIN);
  gfx->fillArc(EYE_R, EY + 14, 26, 19, 200, 340, MAIN);  // closed "^" eye
  gfx->fillEllipse(EYE_R + 12, EY + 44, 14, 7, BLUSH);
  smile(120, MY - 22, 30, 7, MAIN);
}

// ---------- Drawing + switching ----------
const int COUNT = 10;
const char* NAMES[COUNT] = { "neutral", "happy", "sad", "angry", "surprised",
                             "sleepy", "love", "star eyes", "cute", "wink" };

void drawOutline() {
#if WOKWI
  gfx->drawCircle(120, 120, 119, tft.color565(60, 60, 60));  // shows where the round screen ends
#endif
}

void drawFace(int e, bool shut, bool fullClear) {
  gfx->fillRect(10, 25, 220, 200, BLACK);   // clear the face area
  switch (e) {
    case 0: faceNeutral(shut); break;
    case 1: faceHappy();       break;
    case 2: faceSad(shut);     break;
    case 3: faceAngry(shut);   break;
    case 4: faceSurprised();   break;
    case 5: faceSleepy();      break;
    case 6: faceLove();        break;
    case 7: faceStar();        break;
    case 8: faceCute(shut);    break;
    case 9: faceWink();        break;
  }
  drawOutline();
  if (SMOOTH) canvas.pushSprite(0, 0);      // send the finished frame to the screen
}

// Faces that move all the time
bool isAnimated(int e) { return e == 1; }

bool canBlink(int e) { return e == 0 || e == 2 || e == 3 || e == 8; }

int expr = 0;
bool autoPlay = false;   // true = cycle through all faces
uint32_t lastChange = 0, nextBlink = 0;

void show(int e) {
  expr = e;
  drawFace(expr, false, true);
  lastChange = millis();
  nextBlink = millis() + random(1500, 3500);
  Serial.print("Face: ");
  Serial.println(NAMES[expr]);
}

void setup() {
  Serial.begin(115200);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);              // backlight on

  tft.init();
  MAIN   = tft.color565(170, 60, 255);  // purple
  RED    = tft.color565(255, 50, 40);
  PINK   = tft.color565(255, 70, 150);
  YELLOW = tft.color565(255, 210, 0);
  BLUE   = tft.color565(80, 170, 255);
  WHITE  = tft.color565(255, 255, 255);
  BLUSH  = tft.color565(255, 120, 170);
  BLACK  = tft.color565(0, 0, 0);

  tft.fillScreen(BLACK);
  if (SMOOTH) {
    canvas.setColorDepth(16);
    canvas.createSprite(240, 240);
    canvas.fillScreen(BLACK);
    gfx = &canvas;
  }
  Serial.println("Type 0-9 to pick a face, 'a' to toggle auto-cycle");
  show(0);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c >= '0' && c <= '9') { autoPlay = false; show(c - '0'); }
    if (c == 'a') { autoPlay = !autoPlay; Serial.println(autoPlay ? "Auto ON" : "Auto OFF"); }
  }

  if (autoPlay && millis() - lastChange > 3000) show((expr + 1) % COUNT);

  static uint32_t lastAnim = 0;
  if (isAnimated(expr) && millis() - lastAnim > 40) {   // about 25 frames per second
    lastAnim = millis();
    drawFace(expr, false, true);
  }

  if (canBlink(expr) && millis() > nextBlink) {
    drawFace(expr, true, false);     // eyes shut
    delay(15);
    drawFace(expr, false, false);    // eyes open
    nextBlink = millis() + random(1000, 2500);
  }

  delay(10);
}
