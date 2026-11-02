add( a, b )
{
    return a + b;
}

main()
{
    total = 0;

    for ( i = 0; i < 4; i = i + 1 )
    {
        total = add( total, i );
    }

    switch ( total )
    {
        case 6:
            printint( 600 );
            break;
        default:
            printint( 0 );
            break;
    }

    printint( total );
}
