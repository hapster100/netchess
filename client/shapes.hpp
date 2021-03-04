#pragma once

#include "common.hpp"
#include "board_state.hpp"

class Shape {
protected:
  sf::Texture shapeTexture;
  ChessColor side;
public:
  Shape(ChessColor side = ChessColor::White);
  ChessColor getColor();
  virtual std::vector<ShapePosition> getPosibleMoves(IBoardState& gameState, ShapePosition from);
  void draw(sf::RenderWindow& win, sf::Vector2f pos, sf::Vector2f size);
};

class Knight : public Shape {
public:
  Knight(ChessColor side);
  std::vector<ShapePosition> getPosibleMoves(IBoardState& gameState, ShapePosition from) override;
};

class King : public Shape {
public:
  King(ChessColor side);
  std::vector<ShapePosition> getPosibleMoves(IBoardState& gameState, ShapePosition from) override;
};

class Queen : public Shape {
public:
  Queen(ChessColor side);
  std::vector<ShapePosition> getPosibleMoves(IBoardState& gameState, ShapePosition from) override;
};

class Bishop : public Shape {
public:
  Bishop(ChessColor side);
  std::vector<ShapePosition> getPosibleMoves(IBoardState& gameState, ShapePosition from) override;
};

class Rook : public Shape {
public:
  Rook(ChessColor side);
  std::vector<ShapePosition> getPosibleMoves(IBoardState& gameState, ShapePosition from) override;
};

class Pawn : public Shape {
public:
  Pawn(ChessColor side);
  std::vector<ShapePosition> getPosibleMoves(IBoardState& gameState, ShapePosition from) override;
};
