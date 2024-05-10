#define pinA 5
#define pinB 6
#define pinC 7
#define pinD 8
#define pinE 9

int blockO = 0b111100010010;
int blockI = 0b111100100001;
int blockS = 0b011110010011;
int blockZ = 0b110011010011;
int blockL = 0b101011011010;
int blockJ = 0b010111011010;
int blockT = 0b111010010011;

int blocks[] = {blockO, blockI, blockS, blockZ, blockL, blockJ, blockT};
int totalBlocks = 7;

String startText[] = {"PRESS", "ANY", "BUTTON"};
String gameoverText[] = {"GAME", "OVER"};

boolean isDownA = false;
boolean isDownB = false;
boolean isDownC = false;
boolean isDownD = false;
boolean isDownE = false;

boolean isDownPrevA = false;
boolean isDownPrevB = false;
boolean isDownPrevC = false;
boolean isDownPrevD = false;
boolean isDownPrevE = false;

const int gridW = 10;
const int gridH = 20;

int grid[gridH][gridW];
int gridSum[gridH];

int activeBlock;
int activeBlockX;
int activeBlockY;
int activeGrid[gridH][gridW];

boolean start = false;
boolean gameOver = false;
boolean newBlock = true;
boolean needClear = false;
float interval = 500;
long timestamp = 0;
int score = 0;

const int displayW = gridW * 3;
const int displayH = gridH + 4;
const int offsetL = 3;
String displayLines[displayH];
String displayGrid[gridH + 2];
String displayScore[4];
String displayBtn[4];

void setup() {
  randomSeed(analogRead(0));

  Serial.begin(115200);

  digitalWrite(LED_BUILTIN, OUTPUT);

  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  pinMode(pinC, INPUT_PULLUP);
  pinMode(pinD, INPUT_PULLUP);
  pinMode(pinE, INPUT_PULLUP);

  resetGrid();
}

void loop() {

  isDownA = !digitalRead(pinA);
  isDownB = !digitalRead(pinB);
  isDownC = !digitalRead(pinC);
  isDownD = !digitalRead(pinD);
  isDownE = !digitalRead(pinE);

  if ((isDownA && !isDownPrevA) ||
      (isDownB && !isDownPrevB) ||
      (isDownC && !isDownPrevC) ||
      (isDownD && !isDownPrevD) ||
      (isDownE && !isDownPrevE)) {
    if (!gameOver && !start) {
      start = true;
      newBlock = true;
      score = 0;
    } else if (gameOver) {
      gameOver = false;
    }
  }

  if (!start || gameOver) {
    isDownPrevA = isDownA;
    isDownPrevB = isDownB;
    isDownPrevC = isDownC;
    isDownPrevD = isDownD;
    isDownPrevE = isDownE;
  }

  //////// check start

  if (gameOver) {
    render('g');
    delay(25);
    return;
  } else if (!start) {
    render('s');
    delay(25);
    return;
  }

  //////// input logic

  if (isDownE && !isDownPrevE) {
    rotate(&activeBlock, 'R');
    checkBlockRotateCollision();
  }

  if (isDownA && !isDownPrevA) {
    activeBlockX--;
    checkBlockLRCollision('L');
  }

  if (isDownC && !isDownPrevC) {
    activeBlockX++;
    checkBlockLRCollision('R');
  }

  float intervalMod = interval;
  if (isDownB) {
    intervalMod = interval * 0.2;
  }

  if (isDownD && !isDownPrevD) {
    snapToFloor();
    checkSum();
    if (needClear) {
      timestamp = millis();
    }
  }

  //////// game logic

  if (newBlock && !needClear) {
    int index = random(totalBlocks);
    activeBlock = blocks[index];
    activeBlockX = gridW / 2;
    activeBlockY = 0;
    newBlock = false;
  }

  checkGameOver();
  if (gameOver) {
    start = false;
    resetGrid();
    return;
  }

  if (millis() - timestamp > intervalMod) {
    timestamp = millis();
    checkFloorCollision();
    if (!newBlock && !needClear) {
      activeBlockY++;
    } else {
      if (needClear) {
        clearRows();
      }
      checkSum();
    }
  }

  //////// display

  render('p');
  delay(25);


  //////// input logic

  isDownPrevA = isDownA;
  isDownPrevB = isDownB;
  isDownPrevC = isDownC;
  isDownPrevD = isDownD;
  isDownPrevE = isDownE;
}

void resetDisplay() {
  for (int i = 0; i < displayH; i++) {
    displayLines[i] = "";
    for (int j = 0; j < offsetL; j++) {
      displayLines[i] += " ";
    }
  }
}

void render(char mode) {
  resetDisplay();

  switch (mode) {
    case 's':
      drawStartGrid();
      break;
    case 'g':
      drawGameoverGrid();
      break;
    case 'p':
      drawGameGrid();
      break;
  }

  drawScore();

  for (int i = 0; i < displayH; i++) {
    if (i < gridH + 2) {
      displayLines[i] += displayGrid[i];
    }
    if (i < 4) {
      displayLines[i] += "    ";
      displayLines[i] += displayScore[i];
    }
  }

  drawBtn(isDownD, "◓");
  for (int i = 0; i < 4; i++) {
    displayLines[displayH - 15 + i] += "      ";
    displayLines[displayH - 15 + i] += displayBtn[i];
  }

  drawBtn(isDownE, "↻");
  for (int i = 0; i < 4; i++) {
    displayLines[displayH - 15 + i] += " ";
    displayLines[displayH - 15 + i] += displayBtn[i];
  }

  drawBtn(isDownA, "◐");
  for (int i = 0; i < 4; i++) {
    displayLines[displayH - 11 + i] += "   ";
    displayLines[displayH - 11 + i] += displayBtn[i];
  }

  drawBtn(isDownC, "◑");
  for (int i = 0; i < 4; i++) {
    displayLines[displayH - 11 + i] += " ";
    displayLines[displayH - 11 + i] += displayBtn[i];
  }

  drawBtn(isDownB, "◒");
  for (int i = 0; i < 4; i++) {
    displayLines[displayH - 7 + i] += "      ";
    displayLines[displayH - 7 + i] += displayBtn[i];
  }

  String displayMsg = "";
  for (int i = 0; i < displayH; i++) {
    displayMsg += displayLines[i];
    displayMsg += "\n";
  }

  Serial.println(displayMsg);
}

void drawStartGrid() {
  displayGrid[0] = "┏";
  displayGrid[gridH + 1] = "┗";
  for (int j = 0; j < gridW; j++) {
    displayGrid[0] += "━━";
    displayGrid[gridH + 1] += "━━";
  }
  displayGrid[0] += "┓";
  displayGrid[gridH + 1] += "┛";

  for (int i = 0; i < gridH; i++) {
    displayGrid[i + 1] = "┃";
    for (int j = 0; j < gridW; j++) {
      if (i / 2 >= 2 && i / 2 < 5 && i % 2 == 0 && millis() % 1000 < 600) {
        int index = i / 2 - 2;
        int len = startText[index].length();
        int offset = (gridW - len) / 2 + len % 2;
        if (j >= offset && j < offset + len) {
          if (len % 2 == 1) {
            displayGrid[i + 1] += startText[index].charAt(j - offset);
            displayGrid[i + 1] += i % 2 ? " " : "·";
          } else {
            displayGrid[i + 1] += i % 2 ? "·" : " ";
            displayGrid[i + 1] += startText[index].charAt(j - offset);
          }
        } else {
          if (i % 2 == 0) {
            displayGrid[i + 1] += " ·";
          } else {
            displayGrid[i + 1] += "· ";
          }
        }
      } else {
        if (i % 2 == 0) {
          displayGrid[i + 1] += " ·";
        } else {
          displayGrid[i + 1] += "· ";
        }
      }

    }
    displayGrid[i + 1] += "┃";
  }
}

void drawGameoverGrid() {
  for (int i = 0; i < gridH; i++) {
    if (i / 2 >= 2 && i / 2 < 4 && i % 2 == 0) {
      displayGrid[i + 1] = "┃";
      for (int j = 0; j < gridW; j++) {
        int index = i / 2 - 2;
        int len = gameoverText[index].length();
        int offset = (gridW - len) / 2 + len % 2;
        if (j >= offset && j < offset + len && millis() % 1000 < 600) {
          if (len % 2 == 1) {
            displayGrid[i + 1] += gameoverText[index].charAt(j - offset);
            displayGrid[i + 1] += i % 2 ? " " : "·";
          } else {
            displayGrid[i + 1] += i % 2 ? "·" : " ";
            displayGrid[i + 1] += gameoverText[index].charAt(j - offset);
          }
        } else {
          if (i % 2 == 0) {
            displayGrid[i + 1] += " ·";
          } else {
            displayGrid[i + 1] += "· ";
          }
        }
      }
      displayGrid[i + 1] += "┃";
    }
  }
}

void drawGameGrid() {
  displayGrid[0] = "┏";
  displayGrid[gridH + 1] = "┗";
  for (int j = 0; j < gridW; j++) {
    displayGrid[0] += "━━";
    displayGrid[gridH + 1] += "━━";
  }
  displayGrid[0] += "┓";
  displayGrid[gridH + 1] += "┛";

  for (int i = 0; i < gridH; i++) {
    displayGrid[i + 1] = "┃";
    for (int j = 0; j < gridW; j++) {
      if (needClear && gridSum[i] == gridW) {
        int period = floor(interval * 0.3);
        if ((millis() - timestamp) % period < period * 0.5) {
          displayGrid[i + 1] += "  ";
        } else {
          displayGrid[i + 1] += "XX";
        }
      } else {
        if (activeGrid[i][j] == 1) {
          displayGrid[i + 1] += "()";
        } else if (grid[i][j] == 1) {
          displayGrid[i + 1] += "[]";
        } else {
          if (i % 2 == 0) {
            displayGrid[i + 1] += " ·";
          } else {
            displayGrid[i + 1] += "· ";
          }
        }
      }
    }
    displayGrid[i + 1] += "┃";
  }
}

void drawScore() {
  for (int i = 0; i < 4; i++) {
    switch (i) {
      case 0: {
          displayScore[i] = "┏━━━━━━━┓";
        }
        break;
      case 1: {
          displayScore[i] = "┃ SCORE ┃";
        }
        break;
      case 2: {
          displayScore[i] = "┃ ";
          String scoreStr = String(score);
          for (int j = 0; j < 5; j++) {
            if (j < 5 - scoreStr.length()) {
              displayScore[i] += "0";
            } else {
              displayScore[i] += scoreStr[j - 5 + scoreStr.length()];
            }
          }
          displayScore[i] += " ┃";
        }
        break;
      case 3: {
          displayScore[i] = "┗━━━━━━━┛";
        }
        break;
    }
  }
}

void drawBtn(boolean state, String c) {
  for (int i = 0; i < 4; i++) {
    switch (i) {
      case 0: {
          displayBtn[i] = state ? "     " : "┏━━━┓";
        }
        break;
      case 1: {
          if (state) {
            displayBtn[i] = "┏━━━┓";
          } else {
            displayBtn[i] = "┃ ";
            displayBtn[i] += c;
            displayBtn[i] += " ┃";
          }
        }
        break;
      case 2: {
          if (state) {
            displayBtn[i] = "┃ ";
            displayBtn[i] += c;
            displayBtn[i] += " ┃";
          } else {
            displayBtn[i] = "┣━━━┫";
          }
        }
        break;
      case 3: {
          displayBtn[i] = "┗━━━┛";
        }
        break;
    }
  }
}

//┏━━━━━━━━━━━━━━━━━━━━┓
//┃ · · · · · · · · · ·┃
//┃· · · · · · · · · · ┃
//┗━━━━━━━━━━━━━━━━━━━━┛
