#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cstdio>
#include <string_view>

#include "doctest/doctest.h"

using namespace std::literals;

static auto parse_digits(char const* s, double* d) -> char const* {
  auto sign = 1, n = 0;
  for (sscanf(s, "%lf%n", d, &n); *s && !n; sscanf(++s, "%lf%n", d, &n))
    sign *= (*s == '-') ? -1. : 1.;
  *d *= sign;
  return s + n;
}

TEST_CASE("parse_digits") {
  using doctest::Approx;
  auto parser = [x = 0.](std::string_view str) mutable
      -> std::tuple<double, std::string_view> {
    auto ret = std::tuple{x, parse_digits(str.data(), &x)};
    return ret;
  };

  CHECK_EQ(parser("1"), std::tuple{Approx(1.), ""});
  CHECK_EQ(parser("-1"), std::tuple{Approx(-1.), ""});
  CHECK_EQ(parser("1-1"), std::tuple{Approx(1.), "-1"});
  CHECK_EQ(parser("-1-1"), std::tuple{Approx(-1.), "-1"});
  CHECK_EQ(parser("--1"), std::tuple{Approx(1.), ""});
  CHECK_EQ(parser("---1"), std::tuple{Approx(-1.), ""});
  CHECK_EQ(parser("--+-1"), std::tuple{Approx(-1.), ""});
  CHECK_EQ(parser("-+-1"), std::tuple{Approx(1.), ""});
  CHECK_EQ(parser("-+-1+1"), std::tuple{Approx(1.), "+1"});
  CHECK_EQ(parser("-+-1-1"), std::tuple{Approx(1.), "-1"});

  CHECK_EQ(parser("1."), std::tuple{Approx(1.), ""});
  CHECK_EQ(parser("-1."), std::tuple{Approx(-1.), ""});
  CHECK_EQ(parser("1.-1."), std::tuple{Approx(1.), "-1."});
  CHECK_EQ(parser("-.1-.1"), std::tuple{Approx(-.1), "-.1"});
  CHECK_EQ(parser("--1.0"), std::tuple{Approx(1.), ""});
  CHECK_EQ(parser("--1..0"), std::tuple{Approx(1.), ".0"});
}

static auto parse_expr(char const* s, double *d) -> char const* {

}

TEST_CASE("parse_expr") {
  using doctest::Approx;
  auto parser = [x = 0.](std::string_view str) mutable
      -> std::tuple<double, std::string_view> {
    auto ret = std::tuple{x, parse_expr(str.data(), &x)};
    return ret;
  };

}
