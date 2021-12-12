#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>

using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::string;
using std::pair;
using std::set;

struct Rule {

  Rule() = default;

  Rule(char LeftPart, string RightPart) : LeftPart(LeftPart), RightPart(RightPart) {}

  char   LeftPart;
  string RightPart;

};

bool operator ==(const Rule& one, const Rule& another) {
  return one.LeftPart == another.LeftPart && one.RightPart == another.RightPart;
}

bool operator <(const Rule& one, const Rule& another) {
  return one.RightPart < another.RightPart;
}

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

Situation Next(Situation& St) {
  Situation NewSt(St);
  NewSt.CurrentPosition++;
  return NewSt;
}

bool operator ==(const Situation& one, const Situation& another) {
  return one.ThisRule == another.ThisRule 
    && one.CurrentPosition == another.CurrentPosition
    && one.NEXT == another.NEXT;
}

struct Vertex {

  vector<Situation>            Situations;
  unordered_map<char, int32_t> GOTO;

  bool Conatains(Situation St) {
    for (auto Elem : Situations) {
      if (Elem == St) {
        return true;
      }
    }
    return false;
  }

};

struct Rules {
  
  Rule                                      FirstRule;
  unordered_set<char>                       NonTerminals;
  unordered_set<char>                       Terminals;
  set<Rule>                                 SetOfRules;
  unordered_map<char, unordered_set<char> > FIRST;

  template<typename ...Args>
  Rules(string FirstRule, Args... Rules) : FirstRule(FirstRule[0], FirstRule.substr(3)) {
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
    InitFirst();
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
    SetOfRules.insert(Rule(rule[0], rule.substr(3)));
    NonTerminals.insert(rule[0]);
    __getRule(rules...);
  }

};

struct Automaton {

  Automaton(Rules rls) : Grammar(rls) {
    BuildAutomaton();
  }

  Rules          Grammar;
  vector<Vertex> DFA;

  Vertex Closure(Vertex I);

  Vertex GOTO(Vertex I, char X);

  void BuildAutomaton();

  unordered_set<char> GetFirst(string::iterator begin, string::iterator end, unordered_set<char> Candidates);

  bool Contains(Vertex Ver);

};

unordered_set<char> Automaton::GetFirst(string::iterator begin, string::iterator end, unordered_set<char> Candidates) {
  if (begin == end) {
    return Candidates;
  }
  return Grammar.FIRST[*begin];
}

Vertex Automaton::Closure(Vertex I) {
  if (I.Situations.size() == 0) {
    return I;
  }
  bool changed = true;
  Vertex J = I;
  while (changed) {
    changed = false;
    uint32_t sze = J.Situations.size();
    for (uint32_t i = 0; i < sze; ++i) {
      for (auto Rl : Grammar.SetOfRules) {
        if (J.Situations[i].NextInRule() == Rl.LeftPart) {
          Situation NewSituation(Rl, 0, 
            GetFirst(J.Situations[i].ThisRule.RightPart.begin() + J.Situations[i].CurrentPosition  + 1,
              J.Situations[i].ThisRule.RightPart.end(), J.Situations[i].NEXT));
          if (!J.Conatains(NewSituation)) {
            J.Situations.push_back(NewSituation);
            changed = true;
          }
        }
      }
    }

  }
  return J;
}

Vertex Automaton::GOTO(Vertex I, char X) {
  Vertex J;
  for (auto Elem : I.Situations) {
    if (Elem.NextInRule() == X) {
      J.Situations.push_back(Next(Elem));
    }
  }
  return Closure(J);
}

bool Automaton::Contains(Vertex Ver) {
  for (uint32_t i = 0; i < DFA.size(); ++i) {
    bool are_same = true;
    for (uint32_t j = 0; j < Ver.Situations.size(); ++j) {
      bool contains = false;
      for (uint32_t k = 0; k < DFA[i].Situations.size(); ++k) {
        if (DFA[i].Situations[k] == Ver.Situations[j]) {
          contains = true;
        } 
      }
      if (!contains) {
        are_same = false;
        break;
      }
    }
    if (are_same) {
      return true;
    }
  }
  return false;
}

void Automaton::BuildAutomaton() {
  bool changed = true;
  Vertex FirstVertex;
  unordered_set<char> Nxt;
  Nxt.insert('$');
  FirstVertex.Situations.push_back(Situation(Grammar.FirstRule, 0, Nxt));
  DFA.push_back(Closure(FirstVertex));
  while (changed) {
    changed = false;
    uint32_t sze = DFA.size();
    for (uint32_t i = 0; i < sze; ++i) {
      for (auto Elem : Grammar.NonTerminals) {
        Vertex NewVer = GOTO(DFA[i], Elem);
        if (NewVer.Situations.size() && !Contains(NewVer)) {
          DFA.push_back(NewVer);
          DFA[i].GOTO[Elem] = DFA.size() - 1;
          changed = true;
        }
      }
      for (auto Elem : Grammar.Terminals) {
        Vertex NewVer = GOTO(DFA[i], Elem);
        if (NewVer.Situations.size() && !Contains(NewVer)) {
          DFA.push_back(NewVer);
          DFA[i].GOTO[Elem] = DFA.size() - 1;
          changed = true;
        }
      }
    }
  }
}


int main()
{ 
  Rules Grammar("E->S", "S->ac", "S->bDc", "S->Da", "D->a");
  Automaton Aut(Grammar);
}