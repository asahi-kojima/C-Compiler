assert 1 "
main()
{
    if (1) 1;
}"

assert 2 "
main()
{
    if (0) 
        1;
    else
        2;
}"

assert 1 "
main()
{
    x = 1;
    y = 2;
    if (1) 
        x;
    else
        y;
}"

assert 2 "
main()
{
    x = 1;
    y = 2;
    if (0) 
        x;
    else
        y;
}"

assert 1 "
main()
{
    x = 0;
    if (x == 0) 
        1;
    else
        2;
}"

assert 2 "
main()
{
    x = 0;
    if (x != 0) 
        1;
    else
        2;
}"

assert 1 "
main()
{
    x = 1;
    if (x != 0) 
        1;
    else
        2;
}"

assert 2 "
main()
{
    x = 1;
    if (x == 0) 
        1;
    else
        2;
}"

assert 1 "
main()
{
    x = 123;
    xx = 0;
    if (x) 
        xx = 1;
    else
        xx = 2;
}"

assert 2 "
main()
{
    x = 123;
    xx = 0;
    if (x - 123) 
        xx = 1;
    else
        xx = 2;
}"

assert 123 "
main()
{
    x = 123;
    y = 123;
    
    if (x == 123)
        y = 0;
    else
        y = 1;

    if (y == 0)
        return x;

    return 1;
}"

assert 123 "
main()
{
    x = 123;
    y = 0;
    while(0)
        y = y + 1;

    return x;
}"

assert 123 "
main()
{
    x = 123;
    while(0)
        x = x + 1;

    return x;
}"

assert 10 "
main()
{
    x = 0;
    while(x < 10)
        x = x + 1;

    return x;
}"

assert 10 "
main()
{
    x = 0;
    y = 10;
    while(x < y)
        x = x + 1;

    return x;
}"

assert 10 "
main()
{
    x = 0;
    y = 10;
    while(x != y)
        x = x + 1;

    return x;
}"

assert 0 "
main()
{
    for (i = 0; i < 0; i = i + 1)
        return 1;
    return 0;
}
"

assert 1 "
main()
{
    for (i = 0; i < 1; i = i + 1)
        return 1;
    return 0;
}
"

assert 5 "
main()
{
    x = 0;
    y = 5;
    for (i = 0; i < y; i = i + 1)
        x = x + 1;
    return x;
}"