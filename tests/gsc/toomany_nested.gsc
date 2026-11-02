leaf()
{
    x = 1;
}

middle()
{
    // Non-fatal error inside a nested script call.
    leaf( 1, 2, 3, 4 );
    return 777;
}

main()
{
    result = middle();
    printint( result );
    printint( 88888 );
}
