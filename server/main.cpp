#include <iostream>
#include "../framework/netf.hpp"

enum class ChessNetMessage : u_int32_t {
  Start,
  Move,
};

class ChessNetServer : public netf::server_interface<ChessNetMessage> {
  std::shared_ptr<netf::connection<ChessNetMessage>> first;
  std::shared_ptr<netf::connection<ChessNetMessage>> second;
  void StartGame() {
    netf::message<ChessNetMessage> white, black;

    white.header.id = ChessNetMessage::Start;
    black.header.id = ChessNetMessage::Start;

    white << true;
    black << false;

    std::srand(time(nullptr));
    if(std::rand()%2) {
      MessageClient(first, white);
      MessageClient(second, black);
    } else {
      MessageClient(first, black);
      MessageClient(second, white);
    }

    std::cout << "game was started" << std::endl;

  }
public:
  ChessNetServer(uint16_t port) : netf::server_interface<ChessNetMessage>(port) {}
  virtual bool OnClientConnect(std::shared_ptr<netf::connection<ChessNetMessage>> client) {
    if(!first) {
      first = client;
      return true;
    }
    if(!second) {
      second = client;
      StartGame();
      return true;
    }
    return false;
  }
  virtual void OnClientDisconnect(std::shared_ptr<netf::connection<ChessNetMessage>> client) {

  }
  virtual void OnMessage(
    std::shared_ptr<netf::connection<ChessNetMessage>> client, 
    const netf::message<ChessNetMessage>& msg
  ) {
    MessageAllClients(msg, client);
  }
};

int main(int argc, char const *argv[])
{
  ChessNetServer s(135);
  s.Start();

  while (true) {
    s.Update(7);
  }
  
  return 0;
}
