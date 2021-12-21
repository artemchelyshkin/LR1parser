#pragma once

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <cstdint>
#include "Rule.h"

using std::unordered_map;
using std::unordered_set;
using std::set;
using std::string;

struct Rules {
  
  Rule                                      FirstRule;
  unordered_set<char>                       NonTerminals;
  unordered_set<char>                       Terminals;
  set<Rule>                                 SetOfRules;
  unordered_map<char, unordered_set<char> > FIRST;
  uint32_t                                  size = 0;

  template<typename ...Args>
  Rules(string FirstRule, Args... Rules) : FirstRule(FirstRule[0], FirstRule.substr(3), 0) {
    size++;
    NonTerminals.insert(FirstRule[0]);
    SetOfRules.insert(this->FirstRule);
    __getRule(Rules...);
  }

  void __getRule() {
    for (auto rule : SetOfRules) {
      for (uint32_t i = 0; i < rule.RightPart.size(); ++i) {
        if (NonTerminals.count(rule.RightPart[i]) == 0) {
          Terminals.insert(rule.RightPart[i]);
        }
      }
    }
    InitFirst();
  }

  Rule GetRuleById(uint32_t RuleId) {
    for (auto rule : SetOfRules) {
      if (rule.RuleId == RuleId) {
        return rule;
      }
    }
  }

  void merge(unordered_set<char>& first, unordered_set<char> second) {
    for (auto Elem : second) {
      first.insert(Elem);
    }
  }

  void InitFirst() {
    for (auto Elem : Terminals) {
      unordered_set<char> First;
      First.insert(Elem);
      FIRST[Elem] = First;
    }
    bool changed = true;
    while (changed) {
      changed = false;
      for (auto rule : SetOfRules) {
        uint32_t PrevSize = FIRST[rule.LeftPart].size();
        merge(FIRST[rule.LeftPart], FIRST[rule.RightPart[0]]);
        if (FIRST[rule.LeftPart].size() != PrevSize) {
          changed = true;
        }
      }
    }
  }

  template<typename ...Args>
  void __getRule(string rule, Args... rules) {
    SetOfRules.insert(Rule(rule[0], rule.substr(3), size));
    size++;
    NonTerminals.insert(rule[0]);
    __getRule(rules...);
  }

};

