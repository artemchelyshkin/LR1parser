#include "gtest/gtest.h"
#include "LR_1Table.h"

TEST(LRTest, LR1Test1) {
  
  LR_1Table res("E->S", "S->ac", "S->bDc", "S->Da", "D->a");
  EXPECT_EQ(res.Contains("bac"), true);
  EXPECT_EQ(res.Contains("aa"), true);
  EXPECT_EQ(res.Contains("ab"), false);
}

TEST(LRTest, LR1Test2) {  
  LR_1Table res("E->S", "S->aSbS", "S->");
  EXPECT_EQ(res.Contains("aabb"), true);
  EXPECT_EQ(res.Contains("aa"), false);
  EXPECT_EQ(res.Contains(""), true);
  EXPECT_EQ(res.Contains("abaabb"), true);
  EXPECT_EQ(res.Contains("aaababbaababbb"), true);
  EXPECT_EQ(res.Contains(""), true);
}

TEST(LRTest, RulesTest1) {
  Rules Gr("E->S", "S->aSbS", "S->");
  Rules GrWithNoEps("E->S", "S->aSbS", "S->aSb", "S->abS", "S->ab");
  EXPECT_EQ(Gr.SetOfRules, GrWithNoEps.SetOfRules);
}

TEST(LRTest, RulesTest2) {
  Rules Gr("E->S", "C->", "S->CaCbCx", "C->c");
  Rules GrWithNoEps("E->S", "C->c", "S->CaCbCx", "S->abx", "S->aCbCx",
  "S->CabCx", "S->CaCbx", "S->Cabx", "S->aCbx", "S->abCx");
  EXPECT_EQ(Gr.SetOfRules, GrWithNoEps.SetOfRules);
}

TEST(LRTest, AutomatonTest) {
  Rules Gr("E->S", "S->ac", "S->bDc", "S->Da", "D->a");
  Vertex vert;
  Automaton Aut(Gr);
  {
    unordered_set<char> NXT;
    NXT.insert('$');
    Situation St(Rule('S', "bDc"), 1, NXT);
    vert.Situations.push_back(St);
  }
  {
    unordered_set<char> NXT;
    NXT.insert('$');
    Situation St(Rule('E', "S"), 1, NXT);
    vert.Situations.push_back(St);
  }
  Vertex Res1;
  {
    unordered_set<char> NXT;
    NXT.insert('$');
    Situation St(Rule('S', "bDc"), 1, NXT);
    Res1.Situations.push_back(St);
  }
  {
    unordered_set<char> NXT;
    NXT.insert('c');
    Situation St(Rule('D', "a"), 0, NXT);
    Res1.Situations.push_back(St);
  }
  {
    unordered_set<char> NXT;
    NXT.insert('$');
    Situation St(Rule('E', "S"), 1, NXT);
    Res1.Situations.push_back(St);
  }
  EXPECT_EQ(Aut.Closure(vert), Res1);
  Vertex Res2;
  {
    unordered_set<char> NXT;
    NXT.insert('$');
    Situation St(Rule('S', "bDc"), 2, NXT);
    Res2.Situations.push_back(St);
  }
  EXPECT_EQ(Aut.GOTO(vert, 'D'), Res2);
}

TEST(LRTest, ThrowTest) {
  
  EXPECT_THROW(LR_1Table("S->C", "C->CCCC", "C->c", "C->");, std::logic_error);
}


int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
