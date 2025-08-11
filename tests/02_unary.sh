#単項演算子のテスト
assert 1 "main(){+1;}"
assert 2 "main(){1 - -1;}"
assert 2 "main(){+2 * +1;}"
assert 12 "main(){-4 * -3;}"
assert 16 "main(){(-4 * -3) - -4;}"