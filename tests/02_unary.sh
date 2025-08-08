#単項演算子のテスト
assert 1 "+1;"
assert 2 "1 - -1;"
assert 2 "+2 * +1;"
assert 12 "-4 * -3;"
assert 16 "(-4 * -3) - -4;"