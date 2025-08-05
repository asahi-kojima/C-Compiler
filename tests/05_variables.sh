#変数のテスト
assert 0 "x = 0;"

assert 1 "y = x = 1;"

assert 1 "
y = x = 1;
y;"

assert 2 "
x = 1;
y = x + 1;"

assert 3 "
x = 1;
y = 2;
z = x + y;"

assert 7 "
x = 1;
y = 2;
z = x + y;
2 * z + 1;"