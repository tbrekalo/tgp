#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cstdio>

#include "doctest/doctest.h"

using namespace std::literals;

static auto parse_expr(char const* s, double* d, int n = 1 << 30) -> int {
  auto sign = 0, n_sign = 0, n_cur = 0;
  double cur = 0, next = 0;
  auto z = s;
  for (*d = 0.; *z && n;) {
    for (n_sign = 0; *(z + n_sign) == '+' || *(z + n_sign) == '-'; ++n_sign)
      sign *= *(z + n_sign) == '-' ? -1. : 1.;
    z += sscanf(z + n_sign, "%lf%n", &cur, &n_cur) ? n_cur
                                                   : parse_expr(z, &next);
  }
  return z - s;
}

TEST_CASE("parse_expr") {
  using doctest::Approx;
  auto parser = [](std::string_view str) {
    auto x = 0.;
    auto ret = std::tuple{x, parse_expr(str.data(), &x)};
    return ret;
  };

  // CHECK_EQ(parser("1"), std::tuple{Approx(1.), 1});
  // CHECK_EQ(parser("1+1"), std::tuple{Approx(2.), 3});
  // CHECK_EQ(parser("1++1"), std::tuple{Approx(2.), 4});
  // CHECK_EQ(parser("1++-1"), std::tuple{Approx(0.), 5});
  // CHECK_EQ(parser("(1)"), std::tuple{Approx(1.), 3});
  // CHECK_EQ(parser("((1))"), std::tuple{Approx(1.), 5});
  // CHECK_EQ(parser("(1+1)"), std::tuple{Approx(2.), 5});
  // CHECK_EQ(parser("(1)+(1)"), std::tuple{Approx(2.), 7});
  // CHECK_EQ(parser("((1))+(1)"), std::tuple{Approx(2.), 9});
  // CHECK_EQ(parser("((1))+-+(1)"), std::tuple{Approx(0.), 11});

  // CHECK_EQ(parser("2(1)"), std::tuple{Approx(2.), 4});
  // CHECK_EQ(parser("(1)2"), std::tuple{Approx(2.), 4});
  // CHECK_EQ(parser("(1)(1)"), std::tuple{Approx(1.), 6});
  // CHECK_EQ(parser("(1)/(1)"), std::tuple{Approx(1.), 7});
  // CHECK_EQ(parser("(1)/-(1)"), std::tuple{Approx(-1.), 8});
  // CHECK_EQ(parser("(2)*(1.5)"), std::tuple{Approx(3.), 9});
  // CHECK_EQ(parser("2(1+1)1/1"), std::tuple{Approx(4.), 9});
  // CHECK_EQ(parser("2(1+1)1/-1"), std::tuple{Approx(-4.), 10});
  // CHECK_EQ(parser("2(1+1)-1/-1"), std::tuple{Approx(5.), 11});
  // CHECK_EQ(parser("2(1+(1))-1/-1"), std::tuple{Approx(5.), 13});
  // CHECK_EQ(parser("2((1+(1)))-1/-1"), std::tuple{Approx(5.), 15});
  // CHECK_EQ(parser("2*((1+(1)))-1/-1"), std::tuple{Approx(5.), 16});
  // CHECK_EQ(parser("2*((1+(1/2)))-1/-1"), std::tuple{Approx(4.), 18});

  // CHECK_EQ(parser("2.*.3"), std::tuple{Approx(.6), 5});
  // CHECK_EQ(parser("3./.3"), std::tuple{Approx(10.), 5});
  // CHECK_EQ(parser("-10./-(--2)"), std::tuple{Approx(5.), 11});
  // CHECK_EQ(parser("-10./+(--2)"), std::tuple{Approx(-5.), 11});
  // CHECK_EQ(parser("-(10)/(((2+1)*2))"),
  //          std::tuple{Approx(-1.6666666666666667), 17});
  // CHECK_EQ(parser("-(.2*(2*(3(.3/(.3*(((1))))))/.1)+.2)"),
  //          std::tuple{Approx(-12.2), 36});
  // CHECK_EQ(parser("-(-7.3)/-(.2*(2*(3(.3/(.3*(((1))))))/.1)+.2)"),
  //          std::tuple{Approx(-0.5983606557377049), 44});
  // CHECK_EQ(parser("1/.2(1)"), std::tuple{Approx(5.), 7});
  // CHECK_EQ(parser("1/2.(1)2"), std::tuple{Approx(1.), 8});
  // CHECK_EQ(parser("1/2.(2)"), std::tuple{Approx(1.), 7});
}
