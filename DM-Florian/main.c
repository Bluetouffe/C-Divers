#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

uint32_t const H0 = 0x67452301;
uint32_t const H1 = 0xEFCDAB89;
uint32_t const H2 = 0x98BADCFE;
uint32_t const H3 = 0x10325476;
uint32_t const H4 = 0xC3D2E1F0;

uint32_t const K1 = 0x5A827999;
uint32_t const K2 = 0x6ED9EBA1;
uint32_t const K3 = 0x8F1BBCDC;
uint32_t const K4 = 0xCA62C1D6;


void charArrayToInt ( char * s_array , uint8_t * i_array )
{
    uint64_t sizeString = strlen( s_array );
    uint64_t i = 0;
    for ( i = 0 ; i <= sizeString ; i++ )
    {
        i_array[ i ] = s_array[ i ];
    }
}

void displayArray ( uint64_t size, uint8_t * t_array )
{
    uint64_t i = 0;

    for ( i = 0 ; i < size ; i++ )
    {
        printf("%X ", t_array[ i ]);
    }
    printf("\n");
}

uint8_t * preprocessing ( char * message , uint64_t * size)
{
    uint8_t * out = ( uint8_t * ) calloc(  *size , sizeof( uint8_t ) );

    uint16_t zerosToAdd = 0;
    uint16_t i = 0;

    uint64_t messageLength = strlen( message );

    uint64_t byteInTreatment = messageLength;

    charArrayToInt( message , out );

    out = ( uint8_t * ) realloc( out , ( byteInTreatment + 1 ) * sizeof( uint8_t ) );
    //Add 1 at the string's end

    out[ byteInTreatment ] = 0x80; // =0b10000000

    zerosToAdd = ( ( 448 - ( (byteInTreatment) * 8 ) ) % 512 ) / 8;

    out = ( uint8_t * ) realloc( out , ( byteInTreatment + zerosToAdd ) * sizeof( uint8_t ) );

    for ( i = 0 ; i < zerosToAdd ; i++ )
    {
        byteInTreatment++;
        out[ byteInTreatment ] = 0;
    }

    for ( i = 0 ; i < 8 ; i++ )
    {
        out[ byteInTreatment ] = messageLength << ( 8 * ( 7 - i ) );
        byteInTreatment++;
    }

    *size = byteInTreatment;
    return out;
}

void hashSHA1 ( char * message )
{
    uint8_t j = 0;
    uint8_t k = 0;
    uint8_t dec = 0;

    uint32_t a = 0;
    uint32_t b = 0;
    uint32_t c = 0;
    uint32_t d = 0;
    uint32_t e = 0;
    uint32_t f = 0;
    uint32_t kt = 0;
    uint32_t bufferROTL = 0;
    uint32_t bufferCondensate = 0;

	uint64_t i = 0;
    uint64_t ind = 0;
    uint64_t size = 0;
    uint64_t zerosToAdd = 0;
    uint64_t numberOfBlocks = 0;

    uint8_t * test = NULL;
    uint32_t W[80];
    uint64_t ** divided = NULL;

    size = strlen ( message );
    test = ( uint8_t * ) calloc(  size , sizeof( uint8_t ) );

    zerosToAdd = ( ( 448 - ( size * 8 ) ) % 512 ) / 8;
    size = ceil( ( (float)(size + zerosToAdd) / 64.0) ) * 64;

    numberOfBlocks = size >> 6;//Divide by 64

    divided = (uint64_t**) malloc( numberOfBlocks * sizeof(uint64_t*) );

	for (i = 0 ; i < numberOfBlocks ; i++ )
	{
		divided[ i ] = ( uint64_t* ) malloc( 16 * sizeof( uint32_t ) );
	}

    test = preprocessing( message , &size);
    ///////////////////////
    displayArray(size, test);
    ///////////////////////

    printf("After divide : \n");
    for ( i = 0 ; i < numberOfBlocks ; i++ )
    {
        for ( j = 0 ; j < 16 ; j++ )
        {
            divided[i][j] = 0;
            for ( k = 0 ; k < 4 ; k++ )
            {
                ind = (64*i) + ( j * 4 ) + k;
                dec = 8 * ( 3 - k );
                divided[i][j] |=  (uint32_t) test[ind] << dec;
            }
            printf("%08X ", divided[i][j]);
        }
    }
    printf("\n");

    uint32_t h0 = H0;
    uint32_t h1 = H1;
    uint32_t h2 = H2;
    uint32_t h3 = H3;
    uint32_t h4 = H4;

    for ( i = 0 ; i < numberOfBlocks ; i++ )
    {
        //W initialization
        for ( j = 0 ; j < 80 ; j++ )
        {
            if ( j < 16 )
            {
                W[j] = divided[i][j];
                printf("%08X ", divided[i][j]);
            }
            else
            {
                bufferROTL = ( ( ( W[j-3] ^ W[j-8] ) ^ W[j-14] ) ^ W[j-16] );
                W[j] = ( bufferROTL << 1 ) | ( bufferROTL >> ( 32 - 1 ) );
            }
        }
        //End W initialization

        a = h0;
        b = h1;
        c = h2;
        d = h3;
        e = h4;

        for ( j = 0 ; j < 80 ; j++ )
        {
            if ( j < 20 )
            {
                f = (d ^ (b & (c ^ d)));
                kt = K1;
            }
            else if ( j < 40 )
            {
                f = b ^ c  ^ d;
                kt = K2;
            }
            else if ( j < 60 )
            {
                f = ((b & c) | (d & (b | c)));
                kt = K3;
            }
            else
            {
                f = b ^ c ^ d;
                kt = K4;
            }

            bufferROTL = ( a << 5 ) | ( a >> (32 - 5) );

            bufferCondensate = bufferROTL + f + e + kt + W[j];

            e = d;
            d = c;
            bufferROTL = ( b << 30 ) | ( b >> (32 - 30) );
            c = bufferROTL;
            b = a;
            a = bufferCondensate;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
        printf("\nResulting hash is : %08X %08X %08X %08X %08X\n", h0 , h1 , h2 , h3 , h4 );
    }

    free(test);
    free(divided);
}

int main()
{
//    char * message = "";
    char * message = "The quick brown fox jumps over the lazy dog";
//    char * message  = "test";
    printf("Size message : %d\n\n", strlen(message));
//    char * message = "The quick brown fox jumps over the lazy cog";
//    char * message = "abcdefghijklmnopqrstuvwxyz";
    hashSHA1( message );

    return 0;
}
