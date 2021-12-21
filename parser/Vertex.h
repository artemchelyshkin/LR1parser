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

  bool Conatains(Situation situation) {
    for (auto Elem : Situations) {
      if (Elem == situation) {
        return true;
      }
    }
    return false;
  }

};
