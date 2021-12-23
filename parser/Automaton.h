#pragma once

#include <vector>
#include <cstdint>
#include <unordered_set>
#include "Rule.h"
#include "Vertex.h"
#include "Rules.h"

using std::unordered_set;
using std::vector;

struct Automaton {

  Automaton(Rules rules) : Grammar(rules) {
    BuildAutomaton();
  }

  Rules          Grammar;
  vector<Vertex> DFA;

  Vertex Closure(Vertex vertex);

  Vertex GOTO(Vertex vertex, char X);

  void BuildAutomaton();

  unordered_set<char> GetFirst(string::iterator begin, string::iterator end, unordered_set<char> Candidates);

  int32_t Contains(Vertex vertex);

};

unordered_set<char> Automaton::GetFirst(string::iterator begin, string::iterator end, unordered_set<char> Candidates) {
  if (begin == end) {
    return Candidates;
  }
  return Grammar.FIRST[*begin];
}

Vertex Automaton::Closure(Vertex vertex) {
  if (vertex.Situations.size() == 0) {
    return vertex;
  }
  bool changed = true;
  Vertex NewVertex = vertex;
  while (changed) {
    changed = false;
    uint32_t sze = NewVertex.Situations.size();
    for (uint32_t i = 0; i < sze; ++i) {
      for (auto rule : Grammar.SetOfRules) {
        if (NewVertex.Situations[i].NextInRule() == rule.LeftPart) {
          Situation NewSituation(rule, 0, 
            GetFirst(NewVertex.Situations[i].ThisRule.RightPart.begin() + 
                     NewVertex.Situations[i].CurrentPosition  + 1,
                     NewVertex.Situations[i].ThisRule.RightPart.end(), 
                     NewVertex.Situations[i].NEXT));
          if (!NewVertex.Contains(NewSituation)) {
            NewVertex.Situations.push_back(NewSituation);
            changed = true;
          }
        }
      }
    }

  }
  return NewVertex;
}

Vertex Automaton::GOTO(Vertex vertex, char X) {
  Vertex NewVertex;
  for (auto Elem : vertex.Situations) {
    if (Elem.NextInRule() == X) {
      NewVertex.Situations.push_back(Next(Elem));
    }
  }
  return Closure(NewVertex);
}

int32_t Automaton::Contains(Vertex vertex) {
  for (uint32_t i = 0; i < DFA.size(); ++i) {
    bool are_same = true;
    for (uint32_t j = 0; j < vertex.Situations.size(); ++j) {
      bool contains = false;
      for (uint32_t k = 0; k < DFA[i].Situations.size(); ++k) {
        if (DFA[i].Situations[k] == vertex.Situations[j]) {
          contains = true;
        } 
      }
      if (!contains) {
        are_same = false;
        break;
      }
    }
    if (are_same) {
      return i;
    }
  }
  return -1;
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
        if (NewVer.Situations.size()) {
          int32_t index = 0;
          if ((index = Contains(NewVer)) >= 0) {
            DFA[i].GOTO[Elem] = index;
          }
          else {
            DFA.push_back(NewVer);
            DFA[i].GOTO[Elem] = DFA.size() - 1;
            changed = true;
          }
        }
      }
      for (auto Elem : Grammar.Terminals) {
        Vertex NewVer = GOTO(DFA[i], Elem);
        if (NewVer.Situations.size()) {
          int32_t index = 0;
          if ((index = Contains(NewVer)) >= 0) {
            DFA[i].GOTO[Elem] = index;
          }
          else {
            DFA.push_back(NewVer);
            DFA[i].GOTO[Elem] = DFA.size() - 1;
            changed = true;
          }
        }
      }
    }
  }
}

