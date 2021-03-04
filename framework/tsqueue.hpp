#pragma once
#include "common.hpp"

namespace netf {
  template<class T>
  class tsqueue {
  public:
    tsqueue() = default;
    tsqueue(const tsqueue<T>&) = delete;
    virtual ~tsqueue() {
      clear();
    }

    const T& front() {
      std::scoped_lock lock(mux);
      return deq.front();
    }

    const T& back() {
      std::scoped_lock lock(mux);
      return deq.back();
    }

    void push_back(const T& item) {
      std::scoped_lock lock(mux);
      deq.emplace_back(item);
    }

    void push_front(const T& item) {
      std::scoped_lock lock(mux);
      deq.emplace_front(item);
    }

    bool empty() {
      std::scoped_lock lock(mux);
      return deq.empty();
    }

    void clear() {
      std::scoped_lock lock(mux);
      deq.clear();
    }
    
    size_t count() {
      std::scoped_lock lock(mux);
      return deq.size();
    }

    T pop_front() {
      std::scoped_lock lock(mux);
      auto tmp = std::move(deq.front());
      deq.pop_front();
      return tmp;
    }

    T pop_back() {
      std::scoped_lock lock(mux);
      auto tmp = std::move(deq.back());
      deq.pop_back();
      return tmp;
    }

  protected:
    std::mutex mux;
    std::deque<T> deq;
  };
}