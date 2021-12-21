#pragma once

#include "Automaton.h"
#include <unordered_map>
#include <vector>
#include <memory>

using std::pair;
using std::vector;
using std::unordered_map;

enum TableLogic {
  Shift,
  GoTo,
  Reduce,
  Accept,
  Error
};

struct TableBuilder {
  
  Automaton                                                                  Aut;
  std::unique_ptr<vector<unordered_map<char, pair<TableLogic, int32_t> > > > Table;

  TableBuilder(Rules Grammar) : Aut(Grammar) {
    Table = std::make_unique<vector<unordered_map<char, pair<TableLogic, int32_t> > > >();
    Table->assign(Aut.DFA.size(), unordered_map<char, pair<TableLogic, int32_t> >());
    for (auto Column = Table->begin(); Column != Table->end(); Column++) {
      for (auto Elem : Aut.Grammar.NonTerminals) {
        Column->insert(std::make_pair(Elem, std::make_pair(Error, -1)));
      }
      for (auto Elem : Aut.Grammar.Terminals) {
        Column->insert(std::make_pair(Elem, std::make_pair(Error, -1)));
      }
      Column->insert(std::make_pair('$', std::make_pair(Error, -1)));
    }
    BuildTable();
  }

  void BuildTable();

  void AddToTable(uint32_t Position, char Elem, pair<TableLogic, int32_t> Condidate) {
    if ((*Table)[Position][Elem] == Condidate) {
      return;
    }
    if ((*Table)[Position][Elem] != std::make_pair(Error, -1)) {
      throw std::logic_error{ "Bad Grammar, unresolved conflicts" };
    }
    (*Table)[Position][Elem] = Condidate;
  }

  std::unique_ptr<vector<unordered_map<char, pair<TableLogic, int32_t> > > > GetTable() {
    return std::move(Table);
  }

};

void TableBuilder::BuildTable() {
  for (uint32_t i = 0; i < Aut.DFA.size(); ++i) {
    for (auto situation : Aut.DFA[i].Situations) {
      if (situation.ThisRule.LeftPart == Aut.Grammar.FirstRule.LeftPart && situation.NextInRule() == '#') {
        AddToTable(i, '$', std::make_pair(Accept, 0));
      }
      else {
        if (situation.NextInRule() == '#') {
          for (auto NextEl : situation.NEXT) {
            AddToTable(i, NextEl, std::make_pair(Reduce, situation.ThisRule.RuleId));
          }
        }
      }
      if (Aut.Grammar.Terminals.count(situation.NextInRule()) > 0 &&
        Aut.DFA[i].GOTO.count(situation.NextInRule()) > 0) {
        AddToTable(i, situation.NextInRule(), std::make_pair(Shift, Aut.DFA[i].GOTO[situation.NextInRule()]));
      }
    }
    for (auto NonTermGoTo : Aut.DFA[i].GOTO) {
      if (Aut.Grammar.NonTerminals.count(NonTermGoTo.first)) {
        AddToTable(i, NonTermGoTo.first, std::make_pair(GoTo, NonTermGoTo.second));
      }
    }
  }
 }

