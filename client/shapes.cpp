#include "shapes.hpp"

Shape::Shape(ChessColor side) : side(side) {}
std::vector<ShapePosition> Shape::getPosibleMoves(IBoardState& state, ShapePosition from) {
  return {};
}
ChessColor Shape::getColor() { return side; }

void Shape::draw(sf::RenderWindow& win, sf::Vector2f pos, sf::Vector2f size) {
  sf::Sprite sprite;
  sprite.setTexture(shapeTexture);
  sprite.setPosition(pos + size * 0.05f);
  sprite.setScale((size.x * 0.9) / shapeTexture.getSize().x, (size.y * 0.9) /shapeTexture.getSize().y);
  win.draw(sprite);
}

Knight::Knight(ChessColor side) : Shape(side) {
  shapeTexture.loadFromFile("./img/shapes.png", sf::IntRect(1174, 
    side == ChessColor::Black ? 0 : 370, 
    260, 260));
  shapeTexture.setSmooth(true);
}

std::vector<ShapePosition> Knight::getPosibleMoves(IBoardState& state, ShapePosition from) {
  std::vector<ShapePosition> moves = {};
  
  for(int i = -2; i < 3; i++) {
    if(i != 0) {
      int j = 3 - std::abs(i);
      if(state.isEmptyCell(from.x + i, from.y + j) 
      || state.whichShape(from.x + i, from.y + j) != side)
        moves.push_back({from.x + i, from.y + j});
      if(state.isEmptyCell(from.x + i, from.y - j) 
      || state.whichShape(from.x + i, from.y - j) != side)
        moves.push_back({from.x + i, from.y - j});
    }
  }

  return moves;
}

King::King(ChessColor side) : Shape(side) {
  shapeTexture.loadFromFile("./img/shapes.png", sf::IntRect(570, 
    side == ChessColor::Black ? 0 : 370, 
    265, 265));
  shapeTexture.setSmooth(true);  
}  

std::vector<ShapePosition> King::getPosibleMoves(IBoardState& state, ShapePosition from) {
  std::vector<ShapePosition> moves = {};

  for(int i = from.x - 1; i < from.x + 2; i++) {
    for(int j = from.y - 1; j < from.y + 2; j++) {
      if(i == 0 && j == 0) continue;
      if(state.isEmptyCell(i, j) || state.whichShape(i, j) != side) moves.push_back({i, j});
    }
  }

  return moves;
}

Queen::Queen(ChessColor side) : Shape(side) {
  shapeTexture.loadFromFile("./img/shapes.png", sf::IntRect(870, 
    side == ChessColor::Black ? 0 : 370, 
    280, 280));
  shapeTexture.setSmooth(true);  
}

std::vector<ShapePosition> Queen::getPosibleMoves(IBoardState& state, ShapePosition from) {
  std::vector<ShapePosition> moves = {};

  for(int i = from.x + 1, j = from.y; i < 8; i++) {
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }
  }
  for(int i = from.x, j = from.y - 1;j >= 0; j--) {
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }
  }
  for(int i = from.x - 1, j = from.y; i >= 0; i--) {
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }
  }
  for(int i = from.x, j = from.y + 1; j < 8 ; j++) {
    
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }   
  
  }

  for(int i = from.x + 1, j = from.y + 1; i < 8 && j < 8; i++, j++) {
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }
  }
  for(int i = from.x + 1, j = from.y - 1; i < 8 && j >= 0; i++, j--) {
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }
  }
  for(int i = from.x - 1, j = from.y + 1; i >= 0 && j < 8; i--, j++) {
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }
  }
  for(int i = from.x - 1, j = from.y - 1; i >= 0 && j >= 0; i--, j--) {
    
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }   
  
  }

  return moves;
}

Bishop::Bishop(ChessColor side) : Shape(side) {
  shapeTexture.loadFromFile("./img/shapes.png", sf::IntRect(260, 
    side == ChessColor::Black ? 0 : 370, 
    280, 280));
  shapeTexture.setSmooth(true);  
}

std::vector<ShapePosition> Bishop::getPosibleMoves(IBoardState& state, ShapePosition from) {
  std::vector<ShapePosition> moves = {};

  for(int i = from.x + 1, j = from.y + 1; i < 8 && j < 8; i++, j++) {
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }
  }
  for(int i = from.x + 1, j = from.y - 1; i < 8 && j >= 0; i++, j--) {
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }
  }
  for(int i = from.x - 1, j = from.y + 1; i >= 0 && j < 8; i--, j++) {
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }
  }
  for(int i = from.x - 1, j = from.y - 1; i >= 0 && j >= 0; i--, j--) {
    
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }   
  
  }

  return moves;
}

Rook::Rook(ChessColor side) : Shape(side) {
  shapeTexture.loadFromFile("./img/shapes.png", sf::IntRect(0, 
    side == ChessColor::Black ? 0 : 370, 
    220, 250));
  shapeTexture.setSmooth(true);
}

std::vector<ShapePosition> Rook::getPosibleMoves(IBoardState& state, ShapePosition from) {
  std::vector<ShapePosition> moves = {};

  for(int i = from.x + 1, j = from.y; i < 8; i++) {
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }
  }
  for(int i = from.x, j = from.y - 1;j >= 0; j--) {
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }
  }
  for(int i = from.x - 1, j = from.y; i >= 0; i--) {
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }
  }
  for(int i = from.x, j = from.y + 1; j < 8 ; j++) {
    
    if(state.isEmptyCell(i, j)) moves.push_back({i, j});
    else {
      if(state.whichShape(i, j) != side) moves.push_back({i, j});
      break;
    }   
  
  }

  return moves;
}

Pawn::Pawn(ChessColor side) : Shape(side) {
  shapeTexture.loadFromFile("./img/shapes.png", sf::IntRect(1500, 
    side == ChessColor::Black ? 0 : 370, 
    280, 280));
  shapeTexture.setSmooth(true);  
}

std::vector<ShapePosition> Pawn::getPosibleMoves(IBoardState& state, ShapePosition from) {
  std::vector<ShapePosition> moves = {};

  if(side == ChessColor::White) {

    if(state.isEmptyCell(from.x, from.y + 1)) {
      moves.push_back({from.x, from.y + 1});
      
      if(from.y == 1 && state.isEmptyCell(from.x, from.y + 2)) 
        moves.push_back({from.x, from.y + 2});
    }

    if(!state.isEmptyCell(from.x + 1, from.y + 1) && state.whichShape(from.x + 1, from.y + 1) == ChessColor::Black)
      moves.push_back({from.x + 1, from.y + 1});
    if(!state.isEmptyCell(from.x - 1, from.y + 1) && state.whichShape(from.x - 1, from.y + 1) == ChessColor::Black)
      moves.push_back({from.x - 1, from.y + 1});
  
  } else {
    if(state.isEmptyCell(from.x, from.y - 1)) {
      moves.push_back({from.x, from.y - 1});
      
      if(from.y == 6 && state.isEmptyCell(from.x, from.y - 2)) 
        moves.push_back({from.x, from.y - 2});
    }
    if(!state.isEmptyCell(from.x + 1, from.y - 1) && state.whichShape(from.x + 1, from.y - 1) == ChessColor::White)
      moves.push_back({from.x + 1, from.y - 1});
    if(!state.isEmptyCell(from.x - 1, from.y - 1) && state.whichShape(from.x - 1, from.y - 1) == ChessColor::White)
      moves.push_back({from.x - 1, from.y - 1});
  
  }

  return moves;
}
