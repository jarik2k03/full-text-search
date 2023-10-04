#include "commons/parser.h"
#include <gtest/gtest.h>

TEST(CheckCalc, onSum) {
  fsp_calculator fspc;
  double result = fspc.summ(92.1, 4.4);

  EXPECT_DOUBLE_EQ(result, 96.5);
}
