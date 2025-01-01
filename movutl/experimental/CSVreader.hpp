#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

template <typename T, int SIZE> class CSVreader {

private:
  auto split(const std::string& s, const char delim) const {
    Element el;
    el.cnt = 0;
    std::vector<T> elems;
    std::string item;
    for(char ch : s) {
      if(ch == delim) {
        if(!item.empty()) {
          el.append(std::stof(item));
        }
        item.clear();
      } else {
        item += ch;
      }
    }
    if(!item.empty()) el.append(std::stof(item));
    return el;
  }

public:
  struct Element {
    T value[SIZE];
    int cnt{0};
    void append(const T& v) {
      if(cnt >= SIZE) return;
      value[cnt] = v;
      cnt++;
    }
    void clear() { cnt = 0; }
  };
  std::vector<Element> els;

  CSVreader() = delete;
  CSVreader(const std::string fname, const char del = ',') {
    std::cout << "reading...." << fname << std::endl;
    std::ifstream infile(fname);
    if(!infile.is_open()) {
      std::cerr << "file cannot open!" << std::endl;
      return;
    }
    std::string line;
    while(std::getline(infile, line)) {
      if((line[0] == '/' && line[1] == '/') || line[0] == '#')continue;
      auto v = split(line, del);
      els.push_back(v);
    }
  }
};
