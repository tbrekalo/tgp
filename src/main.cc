#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cstdio>

#include "doctest/doctest.h"

using namespace std::literals;

static auto parse_expr(char const* s, double* d) -> int {
  auto i = 0, n_cur = 0, n_sign = 0, sign = 1;
  auto cur = 0., next = 0.;
  for (*d = 0.; *(s + i) && *(s + i) != ')'; sign = 1, n_cur = 0) {
    for (n_sign = 0; *(s + i + n_sign) == '+' || *(s + i + n_sign) == '-';
         ++n_sign)
      sign *= *(s + i + n_sign) == '-' ? -1. : 1.;
    if (sscanf(s + i + n_sign, "%lf%n", &cur, &n_cur); n_cur == 0)
      n_cur = parse_expr(s + i + n_sign + 1, &cur) + 2;
    i += n_sign + n_cur;
    for (auto q = s, c = s + i; *c && *c != ')' && *c != '+' && *c != '-';
         c = s + i) {
      for (q = c; *q && (*q == '*' || *q == '/'); ++q)
        ;
      i += (q - c) + parse_expr(q + (*q == '('), &next) + 2 * (*q == '(');
      cur = *c != '/' ? cur * next : cur / next;
    }
    *d += sign * cur;
  }
  return i;
}

TEST_CASE("parse_expr") {
  using doctest::Approx;
  auto parser =
      [x = 0.](std::string_view str) mutable -> std::tuple<double, int> {
    auto ret = std::tuple{x, parse_expr(str.data(), &x)};
    return ret;
  };

  CHECK_EQ(parser("1"), std::tuple{Approx(1.), 1});
  CHECK_EQ(parser("1+1"), std::tuple{Approx(2.), 3});
  CHECK_EQ(parser("1++1"), std::tuple{Approx(2.), 4});
  CHECK_EQ(parser("1++-1"), std::tuple{Approx(0.), 5});
  CHECK_EQ(parser("(1)"), std::tuple{Approx(1.), 3});
  CHECK_EQ(parser("((1))"), std::tuple{Approx(1.), 5});
  CHECK_EQ(parser("(1+1)"), std::tuple{Approx(2.), 5});
  CHECK_EQ(parser("(1)+(1)"), std::tuple{Approx(2.), 7});
  CHECK_EQ(parser("((1))+(1)"), std::tuple{Approx(2.), 9});
  CHECK_EQ(parser("((1))+-+(1)"), std::tuple{Approx(0.), 11});

  CHECK_EQ(parser("1(1)"), std::tuple{Approx(1.), 4});
  CHECK_EQ(parser("(1)1"), std::tuple{Approx(1.), 4});
  CHECK_EQ(parser("(1)(1)"), std::tuple{Approx(1.), 6});
  CHECK_EQ(parser("(1)/(1)"), std::tuple{Approx(1.), 7});
  CHECK_EQ(parser("(2)*(1.5)"), std::tuple{Approx(3.), 9});
  CHECK_EQ(parser("2(1+1)1/1"), std::tuple{Approx(4.), 9});
  CHECK_EQ(parser("2(1+1)1/-1"), std::tuple{Approx(-4.), 10});
  CHECK_EQ(parser("2(1+1)-1/-1"), std::tuple{Approx(5.), 11});
  CHECK_EQ(parser("2(1+(1))-1/-1"), std::tuple{Approx(5.), 13});
  CHECK_EQ(parser("2((1+(1)))-1/-1"), std::tuple{Approx(5.), 15});
  CHECK_EQ(parser("2*((1+(1)))-1/-1"), std::tuple{Approx(5.), 16});
}
