#pragma once
#include "common.hpp"

namespace netf {
  template<class T>
  struct message_header {
    T id{};
    uint32_t size = 0;
  };

  template<class T>
  struct message {
    message_header<T> header{};
    std::vector<uint8_t> body;

    size_t size() const {
      return body.size();
    }

    friend std::ostream& operator<< (std::ostream& os, const message<T>& msg) {
      os << "ID: " << int(msg.header.id) << " Size: " << msg.size() << std::endl;
      return os;
    }

    template<class DT>
    friend message<T>& operator<< (message<T>& msg, const DT& data) {
      static_assert(std::is_standard_layout<DT>::value, "Data is too complex");
      
      size_t i = msg.body.size();

      msg.body.resize(i + sizeof(DT));
      std::memcpy(msg.body.data() + i, &data, sizeof(DT));
      msg.header.size = msg.size();

      return msg;
    }

    template<class DT>
    friend message<T>& operator>> (message<T>& msg, DT& data) {
      static_assert(std::is_standard_layout<DT>::value, "Data is too complex");
      size_t i = msg.body.size() - sizeof(DT);

      std::memcpy(&data, msg.body.data() + i, sizeof(DT));
      msg.body.resize(i);
      msg.header.size = msg.size();

      return msg;
    }
  };

  template <class T>
  struct connection;
  

  template <class T>
  struct owned_message {
    std::shared_ptr<connection<T>> remote = nullptr;
    message<T> msg;

    friend std::ostream& operator<< (std::ostream& os, const owned_message<T>& msg) {
      os << msg.msg;
      return os;
    }
  };

}


