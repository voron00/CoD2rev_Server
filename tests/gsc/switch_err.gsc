main()
{
    // Non-integer switch index: VM must recover and continue after the switch.
    x = 1.5;
    switch ( x )
    {
        case 1:
            printint( 111 );
            break;
        default:
            printint( 222 );
            break;
    }
    printint( 333 );
}
