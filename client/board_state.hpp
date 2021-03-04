#pragma once

#include "common.hpp"

struct IBoardState {
  virtual ChessColor whichTurn() = 0;
  virtual ChessColor whichShape(int x, int y) = 0;
  virtual bool isEmptyCell(int x, int y) = 0;
};