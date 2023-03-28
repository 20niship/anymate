#pragma once

#include <movutl/core/assert.hpp>
#include <iterator>

template <typename T, int LEN = 1024> struct Pool {
  struct Chunk {
    T items[LEN];
    Chunk* next;
    size_t idx; ///< index of chunklist
  };
  class iterator  {
  public:
    Chunk* chunk;
    unsigned pos;
    iterator() {}
    ~iterator() {}
    iterator(Chunk* chunk, unsigned pos) : chunk(chunk), pos(pos) {}
    inline iterator(const iterator& it) : chunk(it.chunk), pos(it.pos) {}
    inline iterator& operator++() {
      ++pos;
      if(pos == LEN && chunk->next) {
        pos   = 0;
        chunk = chunk->next;
      }
      return *this;
    }
    inline iterator operator++(int) {
      iterator tmp(*this);
      operator++();
      return tmp;
    }
    inline bool operator==(const iterator& rhs) { return chunk == rhs.chunk && pos == rhs.pos; }
    inline bool operator!=(const iterator& rhs) { return !(chunk == rhs.chunk && pos == rhs.pos); }

    bool operator<(iterator const& rhs) const { return (chunk->idx < rhs.chunk->index) || ((chunk->idx == rhs.chunk->index) && (pos < rhs.pos)); }
    bool operator<=(iterator const& rhs) const { return (chunk->idx < rhs.chunk->index) || ((chunk->idx == rhs.chunk->index) && (pos <= rhs.pos)); }
    bool operator>(iterator const& rhs) const { return (chunk->idx > rhs.chunk->index) || ((chunk->idx == rhs.chunk->index) && (pos > rhs.pos)); }
    bool operator>=(iterator const& rhs) const { return (chunk->idx > rhs.chunk->index) || ((chunk->idx == rhs.chunk->index) && (pos >= rhs.pos)); }

    T& operator*() { return chunk->items[pos]; }
    T& operator->() { return chunk->items[pos]; }
  };

private:
  Chunk* beg_m;
  Chunk* end_m;
  unsigned pos_m;
  unsigned chunk_num;

public:
  void init() {
    beg_m       = (Chunk*)malloc(sizeof(Chunk));
    beg_m->next = NULL;
    beg_m->idx  = 0;
    end_m       = beg_m;
    chunk_num   = 1;
  }

  void clear() {
    Chunk* cur = beg_m;
    chunk_num  = 1;
    while(cur != end_m) {
      Chunk* tmp = cur->next;
      free(cur);
      cur = tmp;
    }
  }

  Pool() : pos_m(0) { init(); }
  ~Pool() { clear(); }
  unsigned size() const { return LEN * (chunk_num - 1) + pos_m; }
  iterator begin() const { return iterator(beg_m, 0); }
  iterator end() const { return iterator(end_m, pos_m); }
  void* alloc_item() {
    MU_ASSERT(pos_m <= LEN);
    if(pos_m == LEN) {
      Chunk* chunk = (Chunk*)malloc(sizeof(Chunk));
      chunk->idx   = chunk_num;
      chunk_num++;
      MU_ASSERT(chunk != NULL);
      chunk->next = NULL;
      end_m->next = chunk;
      end_m       = chunk;
      pos_m       = 0;
    }
    T* mem = &end_m->items[pos_m];
    ++pos_m;
    return mem;
  }
};
