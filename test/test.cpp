#include "gtest/gtest.h"
#include "LR_1Table.h"

TEST(SimpleTest, LR1Test) {
  LR_1Table res("E->S", "S->ac", "S->bDc", "S->Da", "D->a");
  EXPECT_EQ(res.Contains("bac"), true);
  EXPECT_EQ(res.Contains("aa"), true);
  EXPECT_EQ(res.Contains("ab"), false);
}

/*
TEST(LanguageTest, concat) {
  _language a, b;
  {
    a.largest_string = 7, a.largest_suffix = 12;
    b.largest_string = 2, b.largest_suffix = 3;
    EXPECT_EQ((a ^ b).largest_string, 9);
    EXPECT_EQ((a ^ b).largest_suffix, 14);
  }
  {
    a.largest_string = 12, a.largest_suffix = 14;
    b.largest_string = limit, b.largest_suffix = limit;
    EXPECT_EQ((a ^ b).largest_string, limit);
    EXPECT_EQ((a ^ b).largest_suffix, limit);
  }
  {
    a.largest_string = 4, a.largest_suffix = 14;
    b.largest_string = -1, b.largest_suffix = 100;
    EXPECT_EQ((a ^ b).largest_string, -1);
    EXPECT_EQ((a ^ b).largest_suffix, 100);
  }
  {
    a.largest_string = 0, a.largest_suffix = 5;
    b.largest_string = 6, b.largest_suffix = 10;
    EXPECT_EQ((a ^ b).largest_string, 6);
    EXPECT_EQ((a ^ b).largest_suffix, 11);
  } 
  {
    a.largest_string = -1, a.largest_suffix = 5;
    b.largest_string = 4, b.largest_suffix = 10;
    EXPECT_EQ((a ^ b).largest_string, -1);
    EXPECT_EQ((a ^ b).largest_suffix, 10);
  }
}

TEST(LanguageTest, plus) {
  _language a, b;
  {
    a.largest_string = 12, a.largest_suffix = 14;
    b.largest_string = limit, b.largest_suffix = limit;
    EXPECT_EQ((a | b).largest_string, limit);
    EXPECT_EQ((a | b).largest_suffix, limit);
  }
  {
    a.largest_string = -1, a.largest_suffix = 14;
    b.largest_string = 12, b.largest_suffix = 0;
    EXPECT_EQ((a | b).largest_string, 12);
    EXPECT_EQ((a | b).largest_suffix, 14);
  }
  {
    a.largest_string = -1, a.largest_suffix = 0;
    b.largest_string = 0, b.largest_suffix = limit;
    EXPECT_EQ((a | b).largest_string, 0);
    EXPECT_EQ((a | b).largest_suffix, limit);
  }
  {
    a.largest_string = -1, a.largest_suffix = 12;
    b.largest_string = -1, b.largest_suffix = 20;
    EXPECT_EQ((a | b).largest_string, -1);
    EXPECT_EQ((a | b).largest_suffix, 20);
  }
  {
    a.largest_string = 14, a.largest_suffix = 25;
    b.largest_string = 11, b.largest_suffix = limit;
    EXPECT_EQ((a | b).largest_string, 14);
    EXPECT_EQ((a | b).largest_suffix, limit);
  }
}

TEST(LanguageTest, klini_star) {
  _language a;
  {
    a.largest_string = -1;
    a.largest_suffix = 13;
    EXPECT_EQ((+a).largest_string, 0);
    EXPECT_EQ((+a).largest_suffix, 13);
  }
  {
    a.largest_string = 2;
    a.largest_suffix = 3;
    EXPECT_EQ((+a).largest_string, limit);
    EXPECT_EQ((+a).largest_suffix, limit);
  }
  {
    a.largest_string = 0;
    a.largest_suffix = limit;
    EXPECT_EQ((+a).largest_string, 0);
    EXPECT_EQ((+a).largest_suffix, limit);
  }
  {
    a.largest_string = -1;
    a.largest_suffix = limit;
    EXPECT_EQ((+a).largest_string, 0);
    EXPECT_EQ((+a).largest_suffix, limit);
  }
}

struct reg_expression_test : testing::Test {
  reg_expression* rg;
  
  reg_expression_test() {
    rg = new reg_expression();
  }

  ~reg_expression_test()
  {
    delete rg;
  }
};

TEST_F(reg_expression_test, test_exceptions) {
  EXPECT_THROW(rg->parse(), std::logic_error);
  rg->letter_x('b');
  EXPECT_THROW(rg->parse(), std::logic_error);
  EXPECT_THROW(rg->find_largest_suffix(), std::logic_error);
  rg->letter_x('b');
  EXPECT_THROW(rg->find_largest_suffix(), std::logic_error);
  rg->letter_x('b');
  rg->expression("ab.");
  EXPECT_THROW(rg->find_largest_suffix(), std::logic_error);
}

struct reg_expression_state {
  std::string str;
  char symb;
  int result;
};

struct reg_expression_suffix_test : reg_expression_test, testing::WithParamInterface<reg_expression_state> {
  reg_expression_suffix_test() {
    rg->expression(GetParam().str);
    rg->letter_x(GetParam().symb);
    rg->parse();
  }
};

TEST_P(reg_expression_suffix_test, test1) {
  auto as = GetParam();
  auto res = rg->find_largest_suffix();
  EXPECT_EQ(as.result, res);
}

INSTANTIATE_TEST_CASE_P(Default, reg_expression_suffix_test,
  testing::Values(
    reg_expression_state{ "ab+c.aba.*.bac.+.+*", 'b', 1 },
    reg_expression_state{ "ab.", 'b', 1 },
    reg_expression_state{ "1", 'b', 0 },
    reg_expression_state{ "acb..bab.c.*.ab.ba.+.+*a.", 'b', 0 },
    reg_expression_state{ "b*", 'b', limit },
    reg_expression_state{ "cb.a+ba.*.b.", 'b', 2 },
    reg_expression_state{ "bac.+*", 'b', limit }
)); 

struct parser_test : testing::Test {
  parse_reg_expression* pr;

  parser_test() {
    pr = new parse_reg_expression();
    pr->character = 'b';
  }

  ~parser_test()
  {
    delete pr;
  }
};

TEST_F(parser_test, stack_check) {
  pr->converted_reg_expression.push(_language());
  EXPECT_THROW(pr->process('.'), std::invalid_argument);
  EXPECT_THROW(pr->process('+'), std::invalid_argument);
  pr->converted_reg_expression.pop();
  EXPECT_THROW(pr->process('*'), std::invalid_argument);
  size_t initial_size = (std::rand() % 100) + 5;
  for (int i = 0; i < initial_size; ++i) {
    pr->converted_reg_expression.push(_language());
  }
  pr->process('+');
  EXPECT_EQ(pr->converted_reg_expression.size(), initial_size - 1);
  pr->process('*');
  initial_size--;
  EXPECT_EQ(pr->converted_reg_expression.size(), initial_size);
  pr->process('.');
  EXPECT_EQ(pr->converted_reg_expression.size(), initial_size - 1);
  EXPECT_THROW(pr->get_result(), std::invalid_argument);
  EXPECT_THROW(pr->process('s'), std::invalid_argument);
}
*/
int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
