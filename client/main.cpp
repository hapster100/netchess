#include "common.hpp"
#include "shapes.hpp"
#include "board_state.hpp"
#include "../framework/netf.hpp"

enum class ChessNetMessage : uint8_t {
  Start,
  Move,
};

class ChessNetClient : public netf::client_interface<ChessNetMessage> {
public:
  void Move(int xFrom, int yFrom, int xTo, int yTo) {
    netf::message<ChessNetMessage> move;
    move.header.id = ChessNetMessage::Move;
    move << xFrom << yFrom << xTo << yTo;
    Send(move);
  }
};

class BoardState : IBoardState {
  std::map<ShapePosition, Shape*> shapes = {};
  ChessColor& turn;
  bool isLigalMove(ShapePosition from, ShapePosition to) {
    if(isWhichTurnShape(from)) {
      Shape* shape = shapes[from];
      auto posibleMoves = shape->getPosibleMoves(*this, from);
      for(auto move : posibleMoves)
        if(move.x == to.x && move.y == to.y)
          return true;
    }
    return false;
  }
public:
  BoardState(ChessColor& turn) : turn(turn) {
    for(int i = 0; i < 8; i++) {
      shapes[{i, 1}] = new Pawn(ChessColor::White);
      shapes[{i, 6}] = new Pawn(ChessColor::Black);
    }

    shapes[{1, 0}] = new Knight(ChessColor::White);
    shapes[{6, 0}] = new Knight(ChessColor::White);
    shapes[{3, 0}] = new Queen(ChessColor::White);
    shapes[{4, 0}] = new King(ChessColor::White);
    shapes[{2, 0}] = new Bishop(ChessColor::White);
    shapes[{5, 0}] = new Bishop(ChessColor::White);
    shapes[{0, 0}] = new Rook(ChessColor::White);
    shapes[{7, 0}] = new Rook(ChessColor::White);

    shapes[{1, 7}] = new Knight(ChessColor::Black);
    shapes[{6, 7}] = new Knight(ChessColor::Black);
    shapes[{3, 7}] = new Queen(ChessColor::Black);
    shapes[{4, 7}] = new King(ChessColor::Black);
    shapes[{2, 7}] = new Bishop(ChessColor::Black);
    shapes[{5, 7}] = new Bishop(ChessColor::Black);
    shapes[{0, 7}] = new Rook(ChessColor::Black);
    shapes[{7, 7}] = new Rook(ChessColor::Black);

  }

  bool isEmptyCell(int x, int y) {
    return shapes.count({x, y}) == 0;
  }

  bool isWhichTurnShape(ShapePosition pos) {
    return !isEmptyCell(pos.x, pos.y) && whichTurn() == whichShape(pos.x, pos.y);
  }

  bool tryMove(ShapePosition from, ShapePosition to) {
    if(isLigalMove(from, to)) {
      shapes[to] = shapes[from];
      shapes.erase(from);
      turn = turn == ChessColor::White ? ChessColor::Black : ChessColor::White;
      return true;
    }
    return false;
  }

  std::vector<ShapePosition> PosibleMoves(ShapePosition from) {
    if(isWhichTurnShape(from)) {
      return shapes[from]->getPosibleMoves(*this, from);
    }
    return {};
  }

  ChessColor whichTurn() {
    return turn;
  }

  ChessColor whichShape(int x, int y) {
    return shapes[{x, y}]->getColor();
  }

  auto getShapes() {
    return std::vector<std::pair<ShapePosition, Shape*>>(shapes.begin(), shapes.end());
  }

};

class ChessBoard {
  BoardState gameState;
  ShapePosition* selectedShape = nullptr;

  ChessNetClient& client;
  ChessColor& turnColor;

  bool invert = false;
public:

  ChessBoard(ChessColor& turn, ChessNetClient& client) 
  : client(client), turnColor(turn), gameState(turn) {}

  void doInvert() {
    invert = !invert;
  }

  void mousePress(sf::RenderWindow& win) {
    int x = sf::Mouse::getPosition(win).x * 8 / win.getSize().x;
    int y = sf::Mouse::getPosition(win).y * 8 / win.getSize().y;

    if(x > 7 || x < 0 || y > 7 || y < 0) return;

    if(invert) {
      x = 7 - x;
      y = 7 - y;
    }
    
    if(selectedShape) {
      if(gameState.tryMove(*selectedShape, {x, y})) {
        client.Move(selectedShape->x, selectedShape->y, x, y);
      }
      selectedShape = nullptr;
    } else {
      if(!gameState.isEmptyCell(x, y)) {
        if(gameState.whichTurn() == gameState.whichShape(x, y))
          selectedShape = new ShapePosition(x, y);
      }
    }
  }

  void forceMove(ShapePosition from, ShapePosition to) {
    gameState.tryMove(from, to);
    if(turnColor == ChessColor::White) std::cout << "white" << std::endl;
    else std::cout << "black" << std::endl;
  }

  void draw(sf::RenderWindow& win) {

    float boardSize = win.getSize().x;
    
    float cellSize = boardSize / 8.0;
    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
    
    for(int i = 0; i < 8; i++) {
      for(int j = 0; j < 8; j++) {
        cell.setFillColor((i+j)%2 ? firstCellColor : secondCellColor);
        
        cell.setPosition(cellSize * i, cellSize * j);
        win.draw(cell);
      }
    }

    if(selectedShape) {
      cell.setSize(sf::Vector2f(cellSize - 8, cellSize -  8));
      cell.setFillColor(sf::Color::Green);
      cell.setPosition(
        cellSize * (invert ? 7 - selectedShape->x : selectedShape->x) + 4,
        cellSize * (invert ? 7 - selectedShape->y : selectedShape->y) + 4
      );
      win.draw(cell);

      std::vector<ShapePosition> moves = gameState.PosibleMoves(*selectedShape);
      for(auto move : moves) {
        cell.setPosition(
          cellSize * (invert ? 7 - move.x : move.x) + 4,
          cellSize * (invert ? 7 - move.y : move.y) + 4
        );

        if(gameState.isEmptyCell(move.x, move.y)) {
          cell.setFillColor(sf::Color::Yellow);
        } else {
          cell.setFillColor(sf::Color::Red);
        }

        win.draw(cell);
      }
    } 
      

    for(auto shape : gameState.getShapes()) {
      float xPos = (invert ? 7 - shape.first.x : shape.first.x) * cellSize; 
      float yPos = (invert ? 7 - shape.first.y : shape.first.y) * cellSize;

      shape.second->draw(win, sf::Vector2f(xPos, yPos), sf::Vector2f(cellSize, cellSize));
    }

  }
};

int main(int argn, char** argv) {
  sf::RenderWindow win(sf::VideoMode(800, 800), "netchees");

  ChessColor yourColor = ChessColor::White;
  ChessColor turnColor = ChessColor::White;
  bool gameStarted = false;

  ChessNetClient client;

  client.Connect(argv[1], 135);

  ChessBoard board(turnColor, client);
  
  while (win.isOpen()) {
    sf::Event event;
    while (win.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        win.close();
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && gameStarted && yourColor == turnColor) {
        board.mousePress(win);
      }
    }

    if(!client.Incoming().empty()) {
      netf::owned_message<ChessNetMessage> msg = client.Incoming().pop_front();
      if(msg.msg.header.id == ChessNetMessage::Start) {
        bool iWhite;
        msg.msg >> iWhite;
        if(!iWhite) {
          yourColor = ChessColor::Black;
        } else {
          yourColor = ChessColor::White;
          board.doInvert();
        }
        std::cout << "Im " << (iWhite ? "white" : "black") << std::endl; 
        gameStarted = true;
      } else if(msg.msg.header.id == ChessNetMessage::Move) {
        int xf, yf, xt, yt;
        msg.msg >> yt >> xt >> yf >> xf;
        std::cout << "move: " << xf << ' ' << yf << " -> " << xt << ' ' << yt << std::endl;
        board.forceMove({xf, yf}, {xt, yt});
        if(turnColor == ChessColor::White) std::cout << "white" << std::endl;
        else std::cout << "black" << std::endl;
      }
    }


    win.clear(sf::Color::White);
    
    board.draw(win);

    win.display();
  }
  
  return 0;
}