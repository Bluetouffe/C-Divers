#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

/*Pour chaque bloc l'algorithme calcule 80 tours (ou rondes, « rounds » en anglais) successifs
et applique une série de transformations sur l'entrée. La première étape consiste à calculer
80 valeurs sur 32 bits. Les 16 premières valeurs sont obtenues directement à partir du bloc
« message » en entrée. Les 64 autres sont calculées successivement. Le SHA-1 les obtient grâce
à une rotation (absente dans SHA-0) qui est appliquée sur le résultat d'un XOR, il utilise pour
cela 4 mots obtenus dans les itérations précédentes. On définit ensuite cinq variables qui sont
initialisées avec des constantes (spécifiées par le standard), le SHA-1 utilise encore 4 autres
constantes dans ses calculs. Si un bloc de 512 bits a déjà été calculé auparavant, les variables
sont initialisées avec les valeurs obtenues à la fin du calcul sur le bloc précédent.

Il s'ensuit 80 tours qui alternent des rotations, des additions entre les variables et les
constantes. Selon le numéro du tour, le SHA-1 utilise une des quatre fonctions booléennes.
L'une de ces fonctions est appliquée sur 3 des 5 variables disponibles. Les variables sont mises
à jour pour le tour suivant grâce à des permutations et une rotation. En résumé, le SHA-1 change
sa méthode de calcul tous les 20 tours et utilise les sorties des tours précédents.

À la fin des 80 tours, on additionne le résultat avec le vecteur initial. Lorsque tous les
blocs ont été traités, les cinq variables concaténées (5 × 32 = 160 bits) représentent la signature.
*/

uint32_t const H0 = 0x67452301;
uint32_t const H1 = 0xEFCDAB89;
uint32_t const H2 = 0x98BADCFE;
uint32_t const H3 = 0x10325476;
uint32_t const H4 = 0xC3D2E1F0;

uint32_t const K1 = 0x5A827999;
uint32_t const K2 = 0x6ED9EBA1;
uint32_t const K3 = 0x8F1BBCDC;
uint32_t const K4 = 0xCA62C1D6;

typedef struct hashValue{
    uint32_t h0;
    uint32_t h1;
    uint32_t h2;
    uint32_t h3;
    uint32_t h4;
}hashValue;

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
        }
    }


    hashValue hashValues;

    hashValues.h0 = H0;
    hashValues.h1 = H1;
    hashValues.h2 = H2;
    hashValues.h3 = H3;
    hashValues.h4 = H4;

    displayArray(size, test);

    for ( i = 0 ; i < numberOfBlocks ; i++ )
    {
        //W initialization
        for ( j = 0 ; j < 80 ; j++ )
        {
            if ( j < 16)
            {
                W[j] = divided[i][j];
            }
            else
            {
                bufferROTL = ( ( ( W[j-3] ^ W[j-8] ) ^ W[j-14] ) ^ W[j-16] );
                W[j] = ( bufferROTL << 1 ) | ( bufferROTL >> ( 32 - 1 ) );
                printf("W%d : %x\n", j , W[j]);
            }
        }
        //End W initialization

        a = hashValues.h0;
        b = hashValues.h1;
        c = hashValues.h2;
        d = hashValues.h3;
        e = hashValues.h4;


        for ( j = 0 ; j < 80 ; j++ )
        {
            if ( j < 20)
            {
                f = ( ( b & c ) | ( ~(b) & d ) );
                kt = K1;
            }
            if ( ( j > 19 ) && ( j < 40 ) )
            {
                f = ( ( b ^ c ) ^ d );
                kt = K2;
            }
            if ( ( j > 39 ) && ( j < 60 ) )
            {
                f = ( ( b & c ) | ( b & d ) ) | ( c & d );
                kt = K3;
            }
            else
            {
                f = ( ( b ^ c ) ^ d );
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

            hashValues.h0 += a;
            hashValues.h1 += b;
            hashValues.h2 += c;
            hashValues.h3 += d;
            hashValues.h4 += e;
        }
    }

    printf("Resulting hash is :\n%X\n%X\n%X\n%X\n%X\n\n", hashValues.h0 , hashValues.h1 , hashValues.h2 , hashValues.h3 , hashValues.h4 );

    free(test);
    free(divided);
}

int main()
{
    char * message = "";
//    char * message = "abcdefghijklmnopqrstuvwxyz";
    hashSHA1( message );

    return 0;
}
