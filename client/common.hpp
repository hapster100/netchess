#pragma once

#include <iostream>

#include <SFML/Graphics.hpp>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

const sf::Color firstCellColor = sf::Color(255, 206, 158);
const sf::Color secondCellColor = sf::Color(209, 139, 71);

struct ShapePosition {
  int x, y;
  ShapePosition(int x = 0, int y = 0) : x(x), y(y) {}
  bool operator<(const ShapePosition& pos) const {
    return x * 8 + y < pos.x * 8 + pos.y;
  }
};

enum ChessColor {
  White,
  Black,
};
