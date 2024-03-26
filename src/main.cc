#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cstdio>

#include "doctest/doctest.h"

using namespace std::literals;

static auto parse_digits(char const* s, double* d) -> int {
  auto sign = 1, n = 0, i = 0;
  for (sscanf(s, "%lf%n", d, &n); *s && !n; sscanf(s + (++i), "%lf%n", d, &n))
    sign *= (*(s + i) == '-') ? -1. : 1.;
  *d *= sign;
  return i + n;
}

TEST_CASE("parse_digits") {
  using doctest::Approx;
  auto parser =
      [x = 0.](std::string_view str) mutable -> std::tuple<double, int> {
    auto ret = std::tuple{x, parse_digits(str.data(), &x)};
    return ret;
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
  auto i = 0, n = 0;
  auto buff_curr = 0., buff_next = 0.;
  for (*d = 0.; *(s + i) && *(s + i) != ')';) {
    if (*(s + i) == '(') {
      n = parse_expr(s + i + 1, &buff_curr) + 2;
    } else {
      n = parse_digits(s + i, &buff_curr);
    }
    *d += buff_curr;
    i += n;
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
  CHECK_EQ(parser("(1)+(1)"), std::tuple{Approx(2.), 7}); // TODO: continue

  // CHECK_EQ(parser("1(1)"), std::tuple{Approx(1.), 1});
  // CHECK_EQ(parser("(1)(1)"), std::tuple{Approx(1.), 1});
}
