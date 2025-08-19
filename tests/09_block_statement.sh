#このテストはコード生成の際に０を仮値としてプッシュしているから成立する。
#実装次第で失敗する可能性もある。
assert 0 "
main()
{
    {
    }
}
"

assert 1 "
main()
{
    {
        return 1;
    }
}
"

assert 1 "
main()
{
    x = 0;
    {
        x = 1;
    }
    return x;
}
"

assert 2 "
main()
{
    x = 0;
    {
        x = 1;
        x = x + 1;
    }
    return x;
}
"

assert 4 "
main()
{
    x = 0;
    {
        x = 1;
        x = x + 1;
    }

    y = 0;
    {
        y = 1;
        y = y + 1;
    }
    return x + y;
}
"

assert 10 "
main()
{
    x = 0;
    y = 0;
    while(x < 10)
    {
        x = x + 1;
        {
            y = y + 1;
        }
    }

    return y;
}
"