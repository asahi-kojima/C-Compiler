#変数のテスト
assert 0 "
main()
{
    x = 0;
}"

assert 1 "
main()
{
    y = x = 1;
}"

assert 1 "
main()
{
    y = x = 1;
    y;
}"

assert 2 "
main()
{
    x = 1;
    y = x + 1;
}"

assert 3 "
main()
{
    x = 1;
    y = 2;
    z = x + y;
}"

assert 7 "
main()
{
    x = 1;
    y = 2;
    z = x + y;
    2 * z + 1;
}"

assert 1 "
main()
{
    xx = 1;
}"


assert 1 "
add()
{
    x = 1;
    y = 2;
    z = x + y;
}
main()
{
    xx = 1;
    xy = 2;
    xx;
}"