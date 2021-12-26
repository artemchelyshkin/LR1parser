#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include "Situation.h"

using std::vector;
using std::unordered_map;

struct Vertex {

  vector<Situation>            Situations;
  unordered_map<char, int32_t> GOTO;

  bool Contains(Situation situation) const {
    for (auto Elem : Situations) {
      if (Elem == situation) {
        return true;
      }
    }
    return false;
  }

};

bool operator == (const Vertex& one, const Vertex& another) {
  for (uint32_t i = 0; i < one.Situations.size(); ++i) {
    bool found = false;
    for (uint32_t j = 0; j < another.Situations.size(); ++j) {
      if (!another.Contains(one.Situations[i])) {
        return false;
      }
    }
  }
  return one.Situations.size() == another.Situations.size();
}
