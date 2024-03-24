#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cctype>
#include <string_view>
#include <tuple>
#include <utility>

#include "doctest/doctest.h"

using namespace std::literals;

using Result = std::tuple<std::string_view, double>;

static auto parse_digits(std::string_view str) -> Result {
  auto sign = 1., dst = 0., base = 10.;
  for (auto stage = 0; stage < 6 and not str.empty(); str.remove_prefix(1)) {
    if (stage == 0 and (str[0] == '-' or str[0] == '+')) {
      sign *= str[0] == '+' ? 1. : -1.;
    } else if (stage |= 2; std::isdigit(str[0])) {
      dst = stage == 2 ? (dst * base + (str[0] - '0'))
                       : (dst + (str[0] - '0') / base);
    } else if (stage += str[0] == '.' ? 2 : 4; stage > 4) {
      break;
    }
  }
  return {str, sign * dst};
}

TEST_CASE("parse_digits") {
  CHECK_EQ(parse_digits("1"sv), std::tuple{""sv, doctest::Approx(1.)});
  CHECK_EQ(parse_digits("1+1"sv), std::tuple{"+1"sv, doctest::Approx(1.)});
  CHECK_EQ(parse_digits("1-1"sv), std::tuple{"-1"sv, doctest::Approx(1.)});
  CHECK_EQ(parse_digits("1--1"sv), std::tuple{"--1"sv, doctest::Approx(1.)});
  CHECK_EQ(parse_digits("-1"sv), std::tuple{""sv, doctest::Approx(-1.)});
  CHECK_EQ(parse_digits("--1"sv), std::tuple{""sv, doctest::Approx(1.)});
  CHECK_EQ(parse_digits("---1"sv), std::tuple{""sv, doctest::Approx(-1.)});
  CHECK_EQ(parse_digits("-+-1"sv), std::tuple{""sv, doctest::Approx(1.)});
  CHECK_EQ(parse_digits("-+-1+1"sv), std::tuple{"+1"sv, doctest::Approx(1.)});
  CHECK_EQ(parse_digits("-+-1-1"sv), std::tuple{"-1"sv, doctest::Approx(1.)});

  CHECK_EQ(parse_digits("1."sv), std::tuple{""sv, doctest::Approx(1.)});
  CHECK_EQ(parse_digits("-1."sv), std::tuple{""sv, doctest::Approx(-1.)});
  CHECK_EQ(parse_digits("+1."sv), std::tuple{""sv, doctest::Approx(1.)});
  CHECK_EQ(parse_digits("+-1."sv), std::tuple{""sv, doctest::Approx(-1.)});
  CHECK_EQ(parse_digits("+--1."sv), std::tuple{""sv, doctest::Approx(1.)});
  CHECK_EQ(parse_digits("-1.+1"sv), std::tuple{"+1"sv, doctest::Approx(-1.)});
  CHECK_EQ(parse_digits("-1.-1"sv), std::tuple{"-1"sv, doctest::Approx(-1.)});

  CHECK_EQ(parse_digits(".1"sv), std::tuple{""sv, doctest::Approx(0.1)});
  CHECK_EQ(parse_digits("-.1"sv), std::tuple{""sv, doctest::Approx(-0.1)});
  CHECK_EQ(parse_digits("+.1"sv), std::tuple{""sv, doctest::Approx(0.1)});
  CHECK_EQ(parse_digits("++.1"sv), std::tuple{""sv, doctest::Approx(0.1)});
  CHECK_EQ(parse_digits("--.1"sv), std::tuple{""sv, doctest::Approx(0.1)});
  CHECK_EQ(parse_digits("-+-.1"sv), std::tuple{""sv, doctest::Approx(0.1)});
  CHECK_EQ(parse_digits("---.1"sv), std::tuple{""sv, doctest::Approx(-0.1)});
  CHECK_EQ(parse_digits("---.1+"sv), std::tuple{"+"sv, doctest::Approx(-0.1)});
  CHECK_EQ(parse_digits("---.1-"sv), std::tuple{"-"sv, doctest::Approx(-0.1)});

  CHECK_EQ(parse_digits("0.0"sv), std::tuple{""sv, doctest::Approx(0.0)});
  CHECK_EQ(parse_digits("1.0"sv), std::tuple{""sv, doctest::Approx(1.0)});
  CHECK_EQ(parse_digits("1.1"sv), std::tuple{""sv, doctest::Approx(1.1)});
  CHECK_EQ(parse_digits("1.1.1"sv), std::tuple{".1"sv, doctest::Approx(1.1)});
  CHECK_EQ(parse_digits("1.1.1.2"sv),
           std::tuple{".1.2"sv, doctest::Approx(1.1)});
  CHECK_EQ(parse_digits("..1"), std::tuple{".1", 0});

  CHECK_EQ(parse_digits("(1)"), std::tuple{"(1)", 0});
}

static auto parse_expr(std::string_view str) -> Result {
  auto dst = 0.;
  auto kill = false;
  while (not str.empty() and str[0] != ')') {
    auto [rhs, val] = parse_digits(str);
    if (rhs[0] == '(' or rhs[0] == '/' or rhs[0] == '*') {
      // dst = rhs[0] == '/' ? dst / val : dst * val;
    }
  }

  return {str, dst};
}

TEST_CASE("parse_expr") {
  CHECK_EQ(parse_expr("1*1"), std::tuple{""sv, doctest::Approx(0.)});
  CHECK_EQ(parse_expr("(1*1)"), std::tuple{""sv, doctest::Approx(0.)});
}
