#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cctype>
#include <string_view>
#include <tuple>

#include "doctest/doctest.h"

using namespace std::literals;

struct Result {
  std::string_view str;
  double val;
  bool is_ok;

  template <class... Ts>
  friend auto operator==(Result const& lhs,
                         std::tuple<Ts...> const& rhs) noexcept -> bool {
    return std::tie(lhs.str, lhs.val, lhs.is_ok) == rhs;
  };
};

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
  CHECK_EQ(parse_digits("1)"), std::tuple{")", Approx(1.), true});
  CHECK_EQ(parse_digits("-1)"), std::tuple{")", Approx(-1.), true});
  CHECK_EQ(parse_digits("-1)-"), std::tuple{")-", Approx(-1.), true});
  CHECK_EQ(parse_digits("-1 -"), std::tuple{" -", Approx(-1.), true});
}

static auto parse_expr(std::string_view str) -> Result {
  auto dst = 0.;
  auto res = Result{};
  while (not str.empty()) {
    if (not(std::isdigit(str[0]) or str[0] == '(' or str[0] == '+' or
            str[0] == '-')) {
      return {str, dst, false};
    }
    if (str[0] == '(') {
      auto len = 0uz, b = 1uz;
      while (len < str.length() and b) {
        ++len;
        b += (str[len] == '(') - 1uz * (str[len] == ')');
      }
      if (b != 0) {
        return {str, dst, false};
      }
      res = parse_expr(str.substr(1uz, len - 1uz));
      if (not res.is_ok or not res.str.empty()) {
        return {str, dst, false};
      }
      res.str = str.substr(len + 1uz);
    } else {
      res = parse_digits(str);
    }

    while (not res.str.empty() and (res.str[0] == '*' or res.str[0] == '/')) {
      auto next = parse_expr(res.str.substr(1));
      if (not next.is_ok) {
        return {str, dst, false};
      }
      res = {
          .str = next.str,
          .val = res.str[0] == '/' ? res.val / next.val : res.val * next.val,
          .is_ok = true,
      };
    }
    str = res.str;
    dst += res.val;
  }

  return {str, dst, true};
}

TEST_CASE("parse_expr") {
  using doctest::Approx;
  CHECK_EQ(parse_expr("(1)"), std::tuple{""sv, Approx(1.), true});
  CHECK_EQ(parse_expr("1+1"), std::tuple{""sv, Approx(2.), true});
  CHECK_EQ(parse_expr("(1+1)"), std::tuple{""sv, Approx(2.), true});
  CHECK_EQ(parse_expr("1*1"), std::tuple{""sv, Approx(1.), true});
  CHECK_EQ(parse_expr("1*(1)"), std::tuple{""sv, Approx(1.), true});
  CHECK_EQ(parse_expr("1+1*2"), std::tuple{""sv, Approx(3.), true});
  CHECK_EQ(parse_expr("1+1*2/2"), std::tuple{""sv, Approx(2.), true});
  CHECK_EQ(parse_expr("1+1*(2/2)"), std::tuple{""sv, Approx(2.), true});
  CHECK_EQ(parse_expr("(1+1)*(2/2)"), std::tuple{""sv, Approx(2.), true});
  CHECK_EQ(parse_expr("(1+1)*((2+2)/2)"), std::tuple{""sv, Approx(4.), true});
  CHECK_EQ(parse_expr("1-1/-2"), std::tuple{""sv, Approx(1.5), true});
  CHECK_EQ(parse_expr("1-1/--2"), std::tuple{""sv, Approx(0.5), true});
  CHECK_EQ(parse_expr("1-1/--2*-1"), std::tuple{""sv, Approx(1.5), true});

  CHECK_EQ(parse_expr("(1 + 1)"), std::tuple{"(1 + 1)"sv, Approx(0.), false});
  CHECK_EQ(parse_expr("(1+ 1)"), std::tuple{"(1+ 1)"sv, Approx(0.), false});
  CHECK_EQ(parse_expr("(1 +1)"), std::tuple{"(1 +1)"sv, Approx(0.), false});
  CHECK_EQ(parse_expr("(1a+1)"), std::tuple{"(1a+1)"sv, Approx(0.), false});
  CHECK_EQ(parse_expr("(1+a1)"), std::tuple{"(1+a1)"sv, Approx(0.), false});

  CHECK_EQ(parse_expr("((1+1)"), std::tuple{"((1+1)", Approx(0.), false});
  CHECK_EQ(parse_expr("(1+1))"), std::tuple{")", Approx(2.), false});
}
