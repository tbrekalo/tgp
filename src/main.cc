#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <string_view>

static auto eval(std::string_view expr) noexcept -> double {
  auto ret = 0.;
  for (auto pos = 0uz; pos < expr.size(); ++pos) {
    ret = ret * 10. + (expr[pos] - '0');
  }

  return ret;
};

TEST_CASE("ints") {
  CHECK_EQ(eval("0"), 0.0);
  CHECK_EQ(eval("1"), 1.0);
  CHECK_EQ(eval("11"), 11.0);
  CHECK_EQ(eval("011"), 11.0);
}
