#pragma once

#include "common.hpp"
#include "connection.hpp"
#include "message.hpp"
#include "tsqueue.hpp"

namespace netf {
  template<class T>
  class client_interface {
    tsqueue<owned_message<T>> m_qMsgIn;
  public:
    
    client_interface() {}

    virtual ~client_interface() {
      Disconnect();
    }


    bool Connect(const std::string& host, uint16_t port) {
      try {
        asio::ip::tcp::resolver resolver(m_context);
        asio::ip::tcp::resolver::results_type m_endpoints = resolver.resolve(host, std::to_string(port));
        
        m_connection = std::make_unique<connection<T>>(
          connection<T>::owner::client, m_context,
          asio::ip::tcp::socket(m_context), m_qMsgIn
        );

        m_connection->ConnectToServer(m_endpoints);

        thrContext = std::thread([this]() { m_context.run(); });


      } catch(std::exception& e) {
        std::cerr << "Client Exception: " << e.what() << std::endl;
        return false;
      }
      
      return true;
    }

    void Disconnect() {
      if(IsConnected()) {
        m_connection->Disconnect();
      }
      
      m_context.stop();
      
      if(thrContext.joinable())
        thrContext.join();

      m_connection.release();
    }

    void Send(const message<T>& msg) {
      m_connection->Send(msg);
    }

    bool IsConnected() {
      if(m_connection) return m_connection->IsConnected();
      return false;
    }

    tsqueue<owned_message<T>>& Incoming() {
      return m_qMsgIn;
    }

  protected:
    asio::io_context m_context;
    std::thread thrContext;
    std::unique_ptr<connection<T>> m_connection;
  };
}
