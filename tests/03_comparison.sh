#比較演算子のテスト
assert 1 "1 == 1;"
assert 0 "1 == 0;"
assert 1 "1 != 0;"
assert 0 "1 != 1;"

assert 1 "(2 * 2) == (2 + 2);"
assert 0 "2 == (1 / 1);"
assert 1 "(2 * 2) != 0;"
assert 0 "(3 * 3) != (3 + 3 + 3);"

assert 1 "0 < 1;"
assert 1 "0 <= 1;"
assert 1 "1 <= 1;"
assert 0 "2 < 1;"
assert 0 "2 <= 1;"

assert 1 "-1 < -0;"
assert 1 "-1 <= -0;"
assert 1 "-1 <= -1;"
assert 0 "-1 < -2;"
assert 0 "-1 <= -2;"

assert 1 "1 > 0;"
assert 1 "1 >= 0;"
assert 1 "1 >= 1;"
assert 0 "1 > 2;"
assert 0 "1 >= 2;"

assert 1 "-0 > -1;"
assert 1 "-0 >= -1;"
assert 1 "-1 >= -1;"
assert 0 "-2 > -1;"
assert 0 "-2 >= -1;"