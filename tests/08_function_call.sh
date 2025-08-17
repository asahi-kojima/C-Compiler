assert 1 "
test_func()
{
    return 1;
}

main()
{
    x = test_func();
    return x;
}"

assert 1 "
test_func()
{
    return 1;
}
main()
{
    return test_func();
}"

assert 6 "
test_func()
{
    x = 1;
    y = 2;
    z = x + y;
}
main()
{
    return test_func() + test_func();
}"

assert 6 "
test_func()
{
    x = 1;
    y = 2;
    z = x + y;
}
main()
{
    x = test_func();
    y = test_func();
    z = x + y;
}"