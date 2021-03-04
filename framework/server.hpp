#pragma once

#include "common.hpp"
#include "tsqueue.hpp"
#include "message.hpp"
#include "connection.hpp"

namespace netf {
  template<class T>
  class server_interface {
  public:
    server_interface(uint16_t port)
      : m_acceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {

    }

    virtual ~server_interface() {
      Stop();
    }

    bool Start() {
      try {
        WaitForClientConnection();
        thrContext = std::thread([this]() {
          m_asioContext.run();
        });
      } catch(std::exception& e) {
        std::cerr << "[SERVER] Exception: " << e.what() << std::endl;
        return false;
      }

      std::cout << "[SERVER] Started!" << std::endl;
      return true;
    }

    void Stop() {
      m_asioContext.stop();
      if(thrContext.joinable()) thrContext.join();
      std::cout << "[SERVER] Stopped!" << std::endl;
    }

    void WaitForClientConnection() {
      m_acceptor.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
          if(!ec) {
            std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << std::endl;
          
            std::shared_ptr<connection<T>> newcon = std::make_shared<connection<T>>(
              connection<T>::owner::server, m_asioContext, std::move(socket), m_qMsgIn);

            if(OnClientConnect(newcon)) {
              m_connections.push_back(std::move(newcon));
              m_connections.back()->ConnectToClient(++nIDCounter);

              std::cout << "[" << m_connections.back()->GetID() << "] Connection Approved!\n";
            } else {
              std::cout << "Connection Denied\n";
            }
          
          } else {
            std::cerr << "[SERVER] New Connection Error: " << ec.message() << std::endl;
          }
          WaitForClientConnection();
        }
      );
    }

    void MessageClient(std::shared_ptr<connection<T>> client, const message<T>& msg) {
      if(client && client->IsConnected()) {
        client->Send(msg);
      } else {
        OnClientDisconnect(client);
        client.reset();
        m_connections.erase(std::remove(m_connections.begin(), m_connections.end(), client), m_connections.end());
      }
    }

    void MessageAllClients(const message<T>& msg, std::shared_ptr<connection<T>> pIgnoreClient = nullptr) {
      bool bInvalidClientExests = false;
      for(auto& client : m_connections) {
        if(client && client->IsConnected()) {
          if(client != pIgnoreClient)
            client->Send(msg);
        } else {
          OnClientDisconnect(client);
          client.reset();
          bInvalidClientExests = true;
        }

        if(bInvalidClientExests) {
          m_connections.erase(std::remove(m_connections.begin(), m_connections.end(), nullptr), m_connections.end());
        }

      }
    }

    void Update(int nMaxMessages = -1) {
      size_t msgCount = 0;
      while(msgCount < nMaxMessages && !m_qMsgIn.empty()) {
        auto msg = m_qMsgIn.pop_front();
        OnMessage(msg.remote, msg.msg);
        msgCount++;
      }
    }

  protected:
    virtual bool OnClientConnect(std::shared_ptr<connection<T>> client) {
      return false;
    }
    virtual void OnClientDisconnect(std::shared_ptr<connection<T>> client) {
      
    }
    virtual void OnMessage(std::shared_ptr<connection<T>> client, const message<T>& msg) {
    }

    std::deque<std::shared_ptr<connection<T>>> m_connections;

    tsqueue<owned_message<T>> m_qMsgIn;

    asio::io_context m_asioContext;
    std::thread thrContext;

    asio::ip::tcp::acceptor m_acceptor;

    uint32_t nIDCounter = 100;

  };
}
