assert 123 "
main()
{
    return 123;
}"

assert 1 "
main()
{
    x = 1;
    return x;
}"


assert 1 "
main()
{
    x = 1;
    return x;
    return x;
}"

assert 2 "
main()
{
    x = 1;
    return x + 1;
    return x;
}"

assert 3 "
main()
{
    return 3;
    x = 1;
    return x + 1;
    return x;
}"