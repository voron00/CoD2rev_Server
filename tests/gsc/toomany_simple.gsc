callee( a )
{
    x = a + 1;
}

main()
{
    // Non-fatal error: too many parameters. The VM must recover and continue.
    callee( 1, 2, 3 );
    printint( 12345 );
}
