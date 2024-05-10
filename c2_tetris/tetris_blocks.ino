void rotate(int *blockData, char dir) {
  int rows = *blockData >> 3 & 7;
  int cols = *blockData & 7;
  int block[rows][cols];
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      block[i][j] = *blockData >> (11 - (i * cols + j)) & 1;
    }
  }

  int newBlock = 0;

  int counter = 0;
  switch (dir) {
    case 'L':
      for (int i = 0; i < cols; i++) {
        for (int j = rows - 1; j >= 0; j--) {
          newBlock |= block[j][i] << counter;
          counter++;
        }
      }
      break;
    case 'R':
      for (int i = cols - 1; i >= 0; i--) {
        for (int j = 0; j < rows; j++) {
          newBlock |= block[j][i] << counter;
          counter++;
        }
      }
      break;
  }

  newBlock = newBlock << (12 - counter);

  newBlock |= cols << 3; // flip rows and cols
  newBlock |= rows;

  *blockData = newBlock;
}

//12
//34
//56
//
//531  246
//642  135
//
//123
//456
//
//41
//52
//63

void resetGrid() {
  for (int i = 0; i < gridH; i++) {
    gridSum[i] = 0;
    for (int j = 0; j < gridW; j++) {
      grid[i][j] = 0;
      activeGrid[i][j] = 0;
    }
  }
}

void resetActiveGrid() {
  for (int i = 0; i < gridH; i++) {
    for (int j = 0; j < gridW; j++) {
      activeGrid[i][j] = 0;
    }
  }
}

void checkGameOver() {
  for (int i = 0; i < gridW; i++) {
    if (grid[0][i] == 1) {
      gameOver = true;
    }
  }
}

void checkBlockLRCollision(char c) {
  setActiveGrid();

  boolean filledCollision = false;

  for (int i = 0; i < gridH; i++) {
    for (int j = 0; j < gridW; j++) {
      if (activeGrid[i][j] == grid[i][j] && activeGrid[i][j] == 1) { // collision into filled grid space
        filledCollision = true;
      }
    }
  }

  if (filledCollision) {
    switch (c) {
      case 'L':
        activeBlockX++;
        break;
      case 'R':
        activeBlockX--;
        break;
    }
  }
}

void checkBlockRotateCollision() {
  setActiveGrid();

  boolean filledCollision = false;

  for (int i = 0; i < gridH; i++) {
    for (int j = 0; j < gridW; j++) {

      if (activeGrid[i][j] == grid[i][j] && activeGrid[i][j] == 1) { // collision into filled grid space
        filledCollision = true;
      }
    }
  }

  if (filledCollision) {

    rotate(&activeBlock, 'L');

    //    newBlock = true;
    //    int tempGrid[gridW][gridH];
    //    for (int u = 0; u < gridW; u++) {
    //      for (int v = 0; v < gridH - 1; v++) {
    //        if (activeGrid[u][v + 1] == 1) {
    //          tempGrid[u][v] = 1;
    //        } else {
    //          tempGrid[u][v] = 0;
    //        }
    //      }
    //    }
    //
    //    for (int u = 0; u < gridW; u++) {
    //      for (int v = 0; v < gridH - 1; v++) {
    //        if (tempGrid[u][v] == 1) {
    //          grid[u][v] = tempGrid[u][v];
    //        }
    //      }
    //    }

  }
}

void checkFloorCollision() {
  setActiveGrid();

  boolean floorCollision = false;

  for (int i = 0; i < gridH; i++) {
    for (int j = 0; j < gridW; j++) {
      if (i < gridH - 1) { // not last row
        // filled block below
        if (activeGrid[i][j] == 1 && grid[i + 1][j] == 1) {
          floorCollision = true;
        }
      } else if (i == gridH - 1) {
        if (activeGrid[i][j] == 1) {
          floorCollision = true;
        }
      }
    }
  }

  if (floorCollision) {
    newBlock = true;
    for (int u = 0; u < gridH; u++) {
      for (int v = 0; v < gridW; v++) {
        if (activeGrid[u][v] == 1) {
          grid[u][v] = 1;
        }
      }
    }
  }
}

void snapToFloor() {
  while (!newBlock) {
    activeBlockY++;
    checkFloorCollision();
  }
}

void setActiveGrid() {
  int rows = activeBlock >> 3 & 7;
  int cols = activeBlock & 7;
  int block[rows][cols];

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      block[i][j] = activeBlock >> (11 - (i * cols + j)) & 1;
    }
  }

  // prevent rotating out of frame

  if (activeBlockX + cols > gridW) {
    activeBlockX = gridW - cols;
  } else if (activeBlockX < 0) {
    activeBlockX = 0;
  }

  // set active grid

  for (int i = 0; i < gridH; i++) {
    for (int j = 0; j < gridW; j++) {
      activeGrid[i][j] = 0;
      if (i >= activeBlockY && i < activeBlockY + rows && j >= activeBlockX && j < activeBlockX + cols) {
        activeGrid[i][j] = block[i - activeBlockY][j - activeBlockX];
      }
    }
  }
}

void checkSum() {
  int counter = 0;
  for (int i = 0; i < gridH; i++) {
    gridSum[i] = 0;
    for (int j = 0; j < gridW; j++) {
      gridSum[i] += grid[i][j];
    }
    if (gridSum[i] == gridW) {
      counter++;
      needClear = true;
      activeBlock = 0;
      resetActiveGrid();
    }
  }

  score += counter == 4 ? 5 : counter;
}

void clearRows() {
  for (int i = gridH - 1; i >= 0; i--) {
    if (gridSum[i] == gridW) {
      for (int u = i; u > 0; u--) {
        for (int j = 0; j < gridW; j++) {
          grid[u][j] = grid[u - 1][j];
        }
        gridSum[u] = gridSum[u - 1];
      }
      i++;
    }
  }

  needClear = false;
}
