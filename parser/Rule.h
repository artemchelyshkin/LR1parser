#pragma once

#include <cstdint>
#include <string>

using std::string;

struct Rule {

  uint32_t RuleId = 0;

  Rule() = default;

  Rule(char LeftPart, string RightPart) : LeftPart(LeftPart), RightPart(RightPart) {}

  Rule(char LeftPart, string RightPart, uint32_t RuleId) : 
    LeftPart(LeftPart), 
    RightPart(RightPart),
    RuleId(RuleId) {}

  char   LeftPart;
  string RightPart;

};

bool operator ==(const Rule& one, const Rule& another) {
  return one.LeftPart == another.LeftPart && one.RightPart == another.RightPart;
}

bool operator <(const Rule& one, const Rule& another) {
  return one.RightPart < another.RightPart;
}

