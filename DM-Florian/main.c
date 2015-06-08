#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>


void hashSHA1 ( char * message )
{
    uint16_t zerosToAdd = 0;
    uint16_t i = 0;

    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476;
    uint32_t h4 = 0xC3D2E1F0;

    uint64_t messageLength = strlen( message );

    message = ( char * ) realloc( message , ( strlen( message ) + 1 ) * sizeof( char ) );

    strcat( message , "1" );

    printf("%s\n", message);



    zerosToAdd = ( 448 - ( messageLength * sizeof( char ) + 1 ) ) / 8;
    printf("zero : %d\n", zerosToAdd);
    for ( i = 0 ; i < zerosToAdd ; i++ )
    {
        message = ( char * ) realloc( message , ( strlen( message ) + 1 ) * sizeof( char ) );
        strcat( message , "\0" );
    }

    printf("%d\n", strlen(message));
    sprintf ( message , "%" PRIu64 , messageLength);
}

int main()
{
    char * message = "test";
    char * test = ( char * ) malloc( ( strlen( message ) + 1 ) * sizeof( char ) );
    strcpy( test , message );

    //printf("length : %s\n", test);

    hashSHA1( test );

    system( "pause" );

    return 0;
}
