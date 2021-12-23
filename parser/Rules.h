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
  bool                                      ContainsEps = false;

  template<typename ...Args>
  Rules(string FirstRule, Args... Rules) : FirstRule(FirstRule[0], FirstRule.substr(3), 0) {
    NonTerminals.insert(FirstRule[0]);
    SetOfRules.insert(this->FirstRule);
    __getRule(Rules...);
  }

  void __getRule() {
    for (auto Rl : SetOfRules) {
      for (uint32_t i = 0; i < Rl.RightPart.size(); ++i) {
        if (NonTerminals.count(Rl.RightPart[i]) == 0) {
          Terminals.insert(Rl.RightPart[i]);
        }
      }
    }
    uint32_t NumberOfRules = 1;
    set<Rule> NewSetOfRules;
    GetRulesWithoutEps();
    NewSetOfRules.insert(FirstRule);
    for (auto Elem : SetOfRules) {
      if (NewSetOfRules.count(Elem) == 0) {
        Elem.RuleId = NumberOfRules;
        NumberOfRules++;
        NewSetOfRules.insert(Elem);
      }
    }
    SetOfRules = NewSetOfRules;
    InitFirst();
  }

  bool AddNewRules(set<Rule>& CurrRules, Rule rule, char CurrChar) {
    vector<uint32_t> PositionOfEpsNonTerminals;
    for (uint32_t CurrPos = 0; CurrPos < rule.RightPart.size(); ++CurrPos) {
      if (rule.RightPart[CurrPos] == CurrChar) {
        PositionOfEpsNonTerminals.push_back(CurrPos);
      }
    }
    if (PositionOfEpsNonTerminals.size() == 0) {
      return false;
    }
    uint64_t NewRules = (1LL << PositionOfEpsNonTerminals.size()) - 1;
    for (uint64_t NumberOfNewRule = 0; NumberOfNewRule < NewRules; ++NumberOfNewRule) {
      unordered_set<uint32_t> PositionOfCurrLetterContainsInRule;
      for (uint32_t ThisElem = 0; ThisElem < PositionOfEpsNonTerminals.size(); ++ThisElem) {
        if (((1LL << ThisElem) & NumberOfNewRule) != 0) {
          PositionOfCurrLetterContainsInRule.insert(PositionOfEpsNonTerminals[ThisElem]);
        }
      }
      Rule NewRule;
      NewRule.LeftPart = rule.LeftPart;
      for (uint32_t CurrPos = 0; CurrPos < rule.RightPart.size(); ++CurrPos) {
        if (rule.RightPart[CurrPos] == CurrChar) {
          if (PositionOfCurrLetterContainsInRule.count(CurrPos)) {
            NewRule.RightPart.push_back(rule.RightPart[CurrPos]);
          }
        }
        else {
          NewRule.RightPart.push_back(rule.RightPart[CurrPos]);
        }
      }
      CurrRules.insert(NewRule);
    }
    return true;
  }

  void GetRulesWithoutEps() {
    bool changed = true;
    while (changed) {
      changed = false;
      char NeedRemoval = 0;
      for (auto rule : SetOfRules) {
        if (rule.RightPart.size() == 0) {
          NeedRemoval = rule.LeftPart;
          break;
        }
      }
      if (NeedRemoval == FirstRule.LeftPart) {
        ContainsEps = true;
      }
      Rule DeleteRule(NeedRemoval, "", 0);
      SetOfRules.erase(DeleteRule);
      set<Rule> NewSetOfRules = SetOfRules;
      for (auto rule : SetOfRules) {
        if (AddNewRules(NewSetOfRules, rule, NeedRemoval)) {
          changed = true;
        }
      }
      SetOfRules = NewSetOfRules;
    }
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
      unordered_set<char> St;
      St.insert(Elem);
      FIRST[Elem] = St;
    }
    bool changed = true;
    while (changed) {
      changed = false;
      for (auto Rl : SetOfRules) {
        uint32_t PrevSize = FIRST[Rl.LeftPart].size();
        merge(FIRST[Rl.LeftPart], FIRST[Rl.RightPart[0]]);
        if (FIRST[Rl.LeftPart].size() != PrevSize) {
          changed = true;
        }
      }
    }
  }

  template<typename ...Args>
  void __getRule(string rule, Args... rules) {
    SetOfRules.insert(Rule(rule[0], rule.substr(3), 0));
    NonTerminals.insert(rule[0]);
    __getRule(rules...);
  }

};
