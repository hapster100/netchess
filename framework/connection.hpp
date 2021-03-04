#pragma once

#include "common.hpp"
#include "message.hpp"
#include "tsqueue.hpp"

namespace netf {
  template<class T>
  class connection : public std::enable_shared_from_this<connection<T>> {
    void ReadHeader() {
      asio::async_read(m_socket, asio::buffer(&m_tmpMsgIn.header, sizeof(message_header<T>)),
        [this](std::error_code ec, size_t len) {
          if(!ec) {
            if(m_tmpMsgIn.header.size > 0) {
              m_tmpMsgIn.body.resize(m_tmpMsgIn.header.size);
              ReadBody();
            } else {
              AddToIncomingMessageQueue();
            }


          } else {
            std::cout << "[" << id << "] Read Header Fail.\n";
            m_socket.close();
          }
        }
      );
    }

    void ReadBody() {
      asio::async_read(m_socket, asio::buffer(m_tmpMsgIn.body.data(), m_tmpMsgIn.body.size()),
        [this](std::error_code ec, size_t len) {
          if(!ec) {
            AddToIncomingMessageQueue();
          } else {
            std::cout << "[" << id << "] Read Body Fail.\n";
            m_socket.close();
          }
        }
      );
    }

    void WriteHeader() {
      asio::async_write(m_socket, asio::buffer(&m_qMsgOut.front().header, sizeof(message_header<T>)),
        [this](std::error_code ec, std::size_t len) {
          if(!ec) {
            if(m_qMsgOut.front().body.size() > 0) {
              WriteBody();
            } else {
              m_qMsgOut.pop_front();
              if(!m_qMsgOut.empty()) {
                WriteHeader();
              }
            }
          } else {
            std::cout << "[" << id << "] Write Header Fail.\n";
            m_socket.close();
          }
        }
      );
    }

    void WriteBody() {
      asio::async_write(m_socket, asio::buffer(m_qMsgOut.front().body.data(), m_qMsgOut.front().body.size()), 
        [this](std::error_code ec, std::size_t length) {
          if (!ec)
          {
            m_qMsgOut.pop_front();
            if (!m_qMsgOut.empty())
            {
              WriteHeader();
            }
          }
          else
          {
            std::cout << "[" << id << "] Write Body Fail.\n";
            m_socket.close();
          }
        }
      );
    }

    void AddToIncomingMessageQueue() {
      if(m_nOwnerType == owner::server) {
        m_qMsgIn.push_back({ this->shared_from_this(), m_tmpMsgIn });
      } else {
        m_qMsgIn.push_back({ nullptr, m_tmpMsgIn });
      }

      ReadHeader();
    }

  public:
    enum class owner {
      server, 
      client
    };
  
    connection(owner parent, asio::io_context& context, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
    : m_asioContext(context), m_socket(std::move(socket)), m_qMsgIn(qIn) {
      m_nOwnerType = parent;
    }
    virtual ~connection() {}


    uint32_t GetID() const {
      return id;
    }

    void ConnectToClient(uint32_t uid = 0) {
      if(m_nOwnerType == owner::server) {
        if(m_socket.is_open()) {
          id = uid;
          ReadHeader();
        } 
      }
    }

    void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoits) {
      if(m_nOwnerType == owner::client) {
        asio::async_connect(m_socket, endpoits,
          [this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
            if(!ec) {
              ReadHeader();
            }
          }
        );
      }
    }

    void Disconnect() {
      if(IsConnected())
        asio::post(m_asioContext, [this]() { m_socket.close(); });
    }

    bool IsConnected() const {
      return m_socket.is_open();
    }

    void Send(const message<T>& msg) {
      asio::post(m_asioContext, 
        [this, msg]() {
          bool bWritingMessage = !m_qMsgOut.empty();
          m_qMsgOut.push_back(msg);
          if(!bWritingMessage) {
            WriteHeader();
          }
        }
      );
    }

  protected:
    uint32_t id = 0;
    asio::ip::tcp::socket m_socket;
    asio::io_context& m_asioContext;
    tsqueue<message<T>> m_qMsgOut;
    tsqueue<owned_message<T>>& m_qMsgIn;
    message<T> m_tmpMsgIn;
    owner m_nOwnerType = owner::server;
  };
  
}
