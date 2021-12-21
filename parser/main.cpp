#include <iostream>
#include "LR_1Table.h"

int main()
{
  LR_1Table res("E->S", "S->ac", "S->bDc", "S->Da", "D->a");
  std::cout << res.Contains("bac") << '\n';
  std::cout << res.Contains("aa") << '\n';
  std::cout << res.Contains("ab") << '\n';
}
