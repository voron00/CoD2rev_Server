bad( a )
{
    x = a + 1;
}

main()
{
    // Recover from the error, then make sure loops and locals still work.
    bad( 1, 2, 3 );

    sum = 0;
    for ( i = 0; i < 3; i = i + 1 )
    {
        sum = sum + i;
    }

    printint( sum );
    printint( 3333 );
}
