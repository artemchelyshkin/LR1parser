#pragma once

#include <memory>
#include <stack>
#include "Rule.h"
#include "TableBuilder.h"
#include "Rules.h"

using std::stack;

struct LR_1Table {

  Rules                                                                       Grammar;
  stack<uint32_t>                                                             Stack;
  std::unique_ptr<vector<unordered_map<char, pair<TableLogic, int32_t> > > >  Table;

  template<class ...Rule>
  LR_1Table(Rule... rules) : Grammar(rules...) {
    std::unique_ptr<TableBuilder> Builder = std::make_unique<TableBuilder>(Grammar);
    Table = Builder->GetTable();
  }

  bool Contains(string word);

  void MakeShift(char Elem, uint32_t NewPosition) {
    Stack.push(Elem);
    Stack.push(NewPosition);
  }

  void MakeReduce(Rule ReducingRule) {
    for (int32_t lastelem = ReducingRule.RightPart.size() - 1; lastelem >= 0; lastelem--) {
      Stack.pop();
      auto Elem = Stack.top();
      Elem == ReducingRule.RightPart[lastelem];
      Stack.pop();
    }
    MakeGOTO(ReducingRule.LeftPart);
  }

  void MakeGOTO(char RuleName) {
    uint32_t CurrPos = Stack.top();
    auto NewStep = (*Table)[CurrPos][RuleName];
    Stack.push(RuleName);
    Stack.push(NewStep.second);
  }

  bool MakeAccept(char Letter) {
    if (Stack.size() == 3 && Letter == '$') {
      return true;
    }
    else {
      return false;
    }
  }

};

bool LR_1Table::Contains(string word) {
  while (!Stack.empty()) {
    Stack.pop();
  }
  Stack.push(0);
  word += "$";
  uint32_t CurrentPositionInTable = 0;
  for (auto CurrentLetter = word.begin(); CurrentLetter != word.end();) {
    auto GetRule = (*Table)[CurrentPositionInTable][*CurrentLetter];
    switch (GetRule.first) {
    case Shift:
      MakeShift(*CurrentLetter, GetRule.second);
      CurrentLetter++;
      CurrentPositionInTable = Stack.top();
      break;
    case Reduce:
      MakeReduce(Grammar.GetRuleById(GetRule.second));
      CurrentPositionInTable = Stack.top();
      break;
    case Accept:
      return MakeAccept(*CurrentLetter);
    case Error:
      return false;
    default:
      break;
    }
  }
  return true;
}

