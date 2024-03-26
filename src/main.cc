#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cstdio>

#include "doctest/doctest.h"

using namespace std::literals;

static int parse_sign(char const* s, int* d) {
  int i = 0;
  for (*d = 1; *(s + i) == '+' || *(s + i) == '-'; ++i)
    *d *= *(s + i) == '-' ? -1. : 1.;
  return i;
};

static auto parse_numeric(char const* s, double* d) -> int {
  int sign, n, i = parse_sign(s, &sign);
  int f = sscanf(s + i, "%lf%n", d, &n);
  *d *= sign;
  return f * (i + n);
}

TEST_CASE("parse_numeric") {
  using doctest::Approx;
  auto parser = [](std::string_view str) -> std::tuple<double, int> {
    auto x = 0.;
    return std::tuple{x, parse_numeric(str.data(), &x)};
  };

  CHECK_EQ(parser("1"), std::tuple{Approx(1.), 1});
  CHECK_EQ(parser("+1"), std::tuple{Approx(1.), 2});
  CHECK_EQ(parser("-1"), std::tuple{Approx(-1.), 2});
  CHECK_EQ(parser("1-1"), std::tuple{Approx(1.), 1});
  CHECK_EQ(parser("-1-1"), std::tuple{Approx(-1.), 2});
  CHECK_EQ(parser("--1"), std::tuple{Approx(1.), 3});
  CHECK_EQ(parser("---1"), std::tuple{Approx(-1.), 4});
  CHECK_EQ(parser("--+-1"), std::tuple{Approx(-1.), 5});
  CHECK_EQ(parser("-+-1"), std::tuple{Approx(1.), 4});
  CHECK_EQ(parser("-+-1+1"), std::tuple{Approx(1.), 4});
  CHECK_EQ(parser("-+-1-1"), std::tuple{Approx(1.), 4});

  CHECK_EQ(parser("1."), std::tuple{Approx(1.), 2});
  CHECK_EQ(parser("-1."), std::tuple{Approx(-1.), 3});
  CHECK_EQ(parser("1.-1."), std::tuple{Approx(1.), 2});
  CHECK_EQ(parser("-.1-.1"), std::tuple{Approx(-.1), 3});
  CHECK_EQ(parser("--1.0"), std::tuple{Approx(1.), 5});
  CHECK_EQ(parser("--1..0"), std::tuple{Approx(1.), 4});
}

static auto parse_expr(char const* s, double* d) -> int {
  auto i = 0, n_cur = 0, n_next = 0, n_sign = 0, sign = 1;
  auto cur = 0., next = 0.;
  for (*d = 0.; *(s + i) && *(s + i) != ')'; sign = 1) {
    n_sign = parse_sign(s + i, &sign);
    if (sscanf(s + i + n_sign, "%lf%n", &cur, &n_cur); n_cur == 0)
      n_cur = parse_expr(s + i + 1, &cur) + 2;
    i += n_sign + n_cur;
    for (auto c = s + i; *c && *c != ')' && *c != '+' && *c != '-'; c = s + i) {
      n_next = parse_expr(c + (*c == '*' || *c == '/'), &next);
      cur = *c != '/' ? cur * next : cur / next;
      i += n_next + (*c == '*' || *c == '/');
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
  CHECK_EQ(parser("(1+1)"), std::tuple{Approx(2.), 5});
  CHECK_EQ(parser("(1)+(1)"), std::tuple{Approx(2.), 7});  // TODO: continue

  CHECK_EQ(parser("1(1)"), std::tuple{Approx(1.), 4});
  CHECK_EQ(parser("(1)1"), std::tuple{Approx(1.), 4});
  CHECK_EQ(parser("(1)(1)"), std::tuple{Approx(1.), 6});
  CHECK_EQ(parser("(1)/(1)"), std::tuple{Approx(1.), 7});
}
