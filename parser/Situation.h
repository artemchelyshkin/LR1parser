#pragma once

#include <cstdint>
#include <unordered_set>
#include "Rule.h"

using std::unordered_set;

struct Situation {

  Rule                ThisRule;
  uint32_t            CurrentPosition;
  unordered_set<char> NEXT;

  Situation() = default;

  Situation(Rule Rl, uint32_t CurrPosition, unordered_set<char> NEXT) : 
    ThisRule(Rl), CurrentPosition(CurrPosition), NEXT(NEXT)
  {}

  char NextInRule() {
    if (ThisRule.RightPart.size() == CurrentPosition) {
      return '#';
    }
    return ThisRule.RightPart[CurrentPosition];
  }

};

Situation Next(Situation& situation) {
  Situation NewSt(situation);
  NewSt.CurrentPosition++;
  return NewSt;
}

bool operator ==(const Situation& one, const Situation& another) {
  return one.ThisRule == another.ThisRule 
    && one.CurrentPosition == another.CurrentPosition
    && one.NEXT == another.NEXT;
}

