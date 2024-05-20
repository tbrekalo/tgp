# Code golf floating point arithmetic

Challenge: Implement floating point arithmetic parser and evaluator in less than 20 lines of C/C++ code.

```c++
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
```

## Build and evaluate

```bash
make release
./build/bin/tgp-bench
```

Test suite can be found in `src/main.cc`
