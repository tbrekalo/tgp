#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cstdio>

#include "doctest/doctest.h"

static int parse_expr(char const* s, double* d, int a = 1 << 30, int m = 1) {
  auto sign = 1., n_cur = 0., c = 0., n = 0.;
  auto z = s, q = s, y = s;
  for (auto n_cur = 0; a && *z && *z != ')'; sign = 1, n_cur = 0, --a) {
    for (c = n = 0.; *z == '+' || *z == '-'; ++z) sign *= *z == '-' ? -1. : 1.;
    z += sscanf(z, "%lf%n", &c, &n_cur) ? n_cur : (parse_expr(z + 1, &c) + 2);
    for (y = z; m && *y && *y != ')' && *y != '+' && *y != '-'; y = z) {
      for (q = y; *q && (*q == '*' || *q == '/'); ++q) n = 0.;
      z += (q - y) + parse_expr(q, &n, 1, *y != '/');
      c = *y != '/' ? c * n : c / n;
    }
    *d += sign * c;
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

  CHECK_EQ(parser("2/2/2"), std::tuple{Approx(.5), 5});
  CHECK_EQ(parser("2/2/2-0.5"), std::tuple{Approx(0.), 9});
  CHECK_EQ(parser("2/(1+1)/2"), std::tuple{Approx(.5), 9});
  CHECK_EQ(parser("2*2*2"), std::tuple{Approx(8.), 5});
  CHECK_EQ(parser("2*(1+1)*2"), std::tuple{Approx(8.), 9});
  CHECK_EQ(parser("2*(1+2/2+1)*2"), std::tuple{Approx(12.), 13});
  CHECK_EQ(parser("2*(1+2/2+2)*2"), std::tuple{Approx(16.), 13});
  CHECK_EQ(parser("2*2/-2*-(--4)"), std::tuple{Approx(8.), 13});

  CHECK_EQ(parser("(1)/(1)"), std::tuple{Approx(1.), 7});
  CHECK_EQ(parser("(1)/-(1)"), std::tuple{Approx(-1.), 8});
  CHECK_EQ(parser("(2)*(1.5)"), std::tuple{Approx(3.), 9});
  CHECK_EQ(parser("2*((1+(1)))-1/-1"), std::tuple{Approx(5.), 16});
  CHECK_EQ(parser("2*((1+(1/2)))-1/-1"), std::tuple{Approx(4.), 18});
  CHECK_EQ(parser("2*((1+(1/++2)))-1/-1"), std::tuple{Approx(4.), 20});
  CHECK_EQ(parser("2*((1+(1/--2)))-1/-1"), std::tuple{Approx(4.), 20});
  CHECK_EQ(parser("2*((1+(1/-+-2)))-1/-1"), std::tuple{Approx(4.), 21});
  CHECK_EQ(parser("2*((1+(1/-+2)))-1/-1"), std::tuple{Approx(2.), 20});
  CHECK_EQ(parser("2*((1+(1/-+2)))-1/-1*2"), std::tuple{Approx(3.), 22});

  CHECK_EQ(parser("2.*.3"), std::tuple{Approx(.6), 5});
  CHECK_EQ(parser("3./.3"), std::tuple{Approx(10.), 5});
  CHECK_EQ(parser("-10./-(--2)"), std::tuple{Approx(5.), 11});
  CHECK_EQ(parser("-10./+(--2)"), std::tuple{Approx(-5.), 11});
  CHECK_EQ(parser("-(10)/(((2+1)*2))"),
           std::tuple{Approx(-1.6666666666666667), 17});
  CHECK_EQ(parser("(.3/(.3*(((1)))))"), std::tuple{Approx(1.), 17});
  CHECK_EQ(parser("(3*(.3/(.3*(((1))))))"), std::tuple{Approx(3.), 21});
  CHECK_EQ(parser("(.2*(2*(3*(.3/(.3*(((1))))))/.1)+.2)"),
           std::tuple{Approx(12.2), 36});
  CHECK_EQ(parser("-(.2*(2*(3*(.3/(.3*(((1))))))/.1)+.2)"),
           std::tuple{Approx(-12.2), 37});
  CHECK_EQ(parser("-(-7.3)/-(.2*(2*(3*(.3/(.3*(((1))))))/.1)+.2)"),
           std::tuple{Approx(-0.5983606557377049), 45});
  CHECK_EQ(parser("1/.2*(1)"), std::tuple{Approx(5.), 8});
  CHECK_EQ(parser("1/2.*(1)*2"), std::tuple{Approx(1.), 10});
  CHECK_EQ(parser("1/2.*(2)"), std::tuple{Approx(1.), 8});
  CHECK_EQ(parser("((5+3)*4-(7+2)*6+10-2*(4+5))*3+(8-2)*5-7*(6-3)+12"),
           std::tuple{Approx(-69.), 49});
  CHECK_EQ(parser("((5+3)/4----(-7+2)*6+10/2*(-4+5))*3+(-8---2)/5-7*(6-3)+12"),
           std::tuple{Approx(-80.), 57});

  CHECK_EQ(parser("-(---((((--1)))))"), std::tuple{Approx(1.), 17});
  CHECK_EQ(parser("-10./-(--2)"), std::tuple{Approx(5.), 11});
}
