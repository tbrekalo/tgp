#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cctype>
#include <string_view>
#include <tuple>

#include "doctest/doctest.h"

using namespace std::literals;

using Result = std::tuple<std::string_view, double, bool>;

static auto parse_digits(std::string_view str) -> Result {
  auto start_len = str.size();
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
  return {str, sign * dst, start_len != str.length()};
}

TEST_CASE("parse_digits") {
  using doctest::Approx;
  CHECK_EQ(parse_digits("1"sv), std::tuple{""sv, Approx(1.), true});
  CHECK_EQ(parse_digits("1+1"sv), std::tuple{"+1"sv, Approx(1.), true});
  CHECK_EQ(parse_digits("1-1"sv), std::tuple{"-1"sv, Approx(1.), true});
  CHECK_EQ(parse_digits("1--1"sv), std::tuple{"--1"sv, Approx(1.), true});
  CHECK_EQ(parse_digits("-1"sv), std::tuple{""sv, Approx(-1.), true});
  CHECK_EQ(parse_digits("--1"sv), std::tuple{""sv, Approx(1.), true});
  CHECK_EQ(parse_digits("---1"sv), std::tuple{""sv, Approx(-1.), true});
  CHECK_EQ(parse_digits("-+-1"sv), std::tuple{""sv, Approx(1.), true});
  CHECK_EQ(parse_digits("-+-1+1"sv), std::tuple{"+1"sv, Approx(1.), true});
  CHECK_EQ(parse_digits("-+-1-1"sv), std::tuple{"-1"sv, Approx(1.), true});

  CHECK_EQ(parse_digits("1."sv), std::tuple{""sv, Approx(1.), true});
  CHECK_EQ(parse_digits("-1."sv), std::tuple{""sv, Approx(-1.), true});
  CHECK_EQ(parse_digits("+1."sv), std::tuple{""sv, Approx(1.), true});
  CHECK_EQ(parse_digits("+-1."sv), std::tuple{""sv, Approx(-1.), true});
  CHECK_EQ(parse_digits("+--1."sv), std::tuple{""sv, Approx(1.), true});
  CHECK_EQ(parse_digits("-1.+1"sv), std::tuple{"+1"sv, Approx(-1.), true});
  CHECK_EQ(parse_digits("-1.-1"sv), std::tuple{"-1"sv, Approx(-1.), true});

  CHECK_EQ(parse_digits(".1"sv), std::tuple{""sv, Approx(0.1), true});
  CHECK_EQ(parse_digits("-.1"sv), std::tuple{""sv, Approx(-0.1), true});
  CHECK_EQ(parse_digits("+.1"sv), std::tuple{""sv, Approx(0.1), true});
  CHECK_EQ(parse_digits("++.1"sv), std::tuple{""sv, Approx(0.1), true});
  CHECK_EQ(parse_digits("--.1"sv), std::tuple{""sv, Approx(0.1), true});
  CHECK_EQ(parse_digits("-+-.1"sv), std::tuple{""sv, Approx(0.1), true});
  CHECK_EQ(parse_digits("---.1"sv), std::tuple{""sv, Approx(-0.1), true});
  CHECK_EQ(parse_digits("---.1+"sv), std::tuple{"+"sv, Approx(-0.1), true});
  CHECK_EQ(parse_digits("---.1-"sv), std::tuple{"-"sv, Approx(-0.1), true});

  CHECK_EQ(parse_digits("0.0"sv), std::tuple{""sv, Approx(0.0), true});
  CHECK_EQ(parse_digits("1.0"sv), std::tuple{""sv, Approx(1.0), true});
  CHECK_EQ(parse_digits("1.1"sv), std::tuple{""sv, Approx(1.1), true});
  CHECK_EQ(parse_digits("1.1.1"sv), std::tuple{".1"sv, Approx(1.1), true});
  CHECK_EQ(parse_digits("1.1.1.2"sv), std::tuple{".1.2"sv, Approx(1.1), true});
  CHECK_EQ(parse_digits("..1"), std::tuple{".1", Approx(0.), true});

  CHECK_EQ(parse_digits("(1)"), std::tuple{"(1)", Approx(0.), false});
}

static auto parse_expr(std::string_view str, double dst) -> Result {
  for (auto opr = '+'; not str.empty() and str[0] != ')';) {
    if (str[0] == '(' or str[0] == '*' or str[0] == '/') {
      auto [rhs, val, did_parse] = parse_expr(str.substr(1), 1.);
    }
  }

  return {str, dst, true};
}

TEST_CASE("parse_expr") {
  using doctest::Approx;
  CHECK_EQ(parse_expr("1*1", 0.), std::tuple{""sv, Approx(0.), true});
  CHECK_EQ(parse_expr("(1*1)", 0.), std::tuple{""sv, Approx(0.), true});
  CHECK_EQ(parse_expr("1(1*1)", 0.), std::tuple{""sv, Approx(0.), true});
  CHECK_EQ(parse_expr("1*(1*1)", 0.), std::tuple{""sv, Approx(0.), true});
  CHECK_EQ(parse_expr("1*((1)*(1))", 0.), std::tuple{""sv, Approx(0.), true});
}
