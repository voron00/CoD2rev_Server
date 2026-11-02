bad( a )
{
    x = a;
}

main()
{
    // Recover from the error, then make sure switch state still works.
    bad( 1, 2 );

    n = 1;
    switch ( n )
    {
        case 0:
            printint( 0 );
            break;
        case 1:
            printint( 1111 );
            break;
        default:
            printint( 9999 );
            break;
    }

    printint( 2222 );
}
