#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

/*Le SHA-1 prend un message d'un maximum de 2^64 bits en entrée.
Quatre fonctions booléennes sont définies, elles prennent 3 mots de 32 bits
en entrée et calculent un mot de 32 bits. Une fonction spécifique de rotation
est également disponible, elle permet de déplacer les bits vers la gauche
(le mouvement est circulaire et les bits reviennent à droite).
Une de ces rotations n'était pas présente dans le SHA-0, elle permet
de casser certaines caractéristiques linéaires dans la structure.
Cela permet d'éviter une attaque sur les bits neutres décrite par Eli Biham,
technique reprise pour calculer la collision complète sur SHA-0 (Antoine Joux et al.).

Le SHA-1 commence par ajouter à la fin du message un bit à 1 suivi d'une série de bits à 0,
puis la longueur du message initial (en bits) codée sur 64 bits. La série de 0 a une longueur
telle que la séquence ainsi prolongée a une longueur multiple de 512 bits. L'algorithme
travaille ensuite successivement sur des blocs de 512 bits.
Pour chaque bloc l'algorithme calcule 80 tours (ou rondes, « rounds » en anglais) successifs
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
        printf("%d ", t_array[ i ]);
    }
    printf("\n");
}

uint8_t * preprocessing ( char * message , uint64_t * size , uint8_t * out )
{
    uint16_t zerosToAdd = 0;
    uint16_t i = 0;

    uint64_t messageLength = strlen( message );

    uint64_t byteInTreatment = messageLength;

    charArrayToInt( message , out );

    out = ( uint8_t * ) realloc( out , ( byteInTreatment + 1 ) * sizeof( uint8_t ) );
    //Add 1 at the string's end

    out[ byteInTreatment ] = 0x80; // =0b10000000
    byteInTreatment++;

    zerosToAdd = ( ( 448 - ( byteInTreatment * 8 ) ) % 512 ) / 8;

    out = ( uint8_t * ) realloc( out , ( byteInTreatment + zerosToAdd ) * sizeof( uint8_t ) );

    for ( i = 0 ; i < zerosToAdd ; i++ )
    {
        out[ byteInTreatment ] = 0;
        byteInTreatment++;
    }

    for ( i = 0 ; i < 8 ; i++ )
    {
        out[ byteInTreatment ] = messageLength << ( 8 * ( 7 - i ) );
        byteInTreatment++;
    }

    *size = byteInTreatment;
    return out;
}

void divideInBlocks (uint8_t * message , uint64_t size , uint32_t ** messageDivided)
{
    uint64_t numberOfBlocks = size >> 6;

    printf("\nDivide\n");
    displayArray(size, message);
    printf("\n");

    uint64_t i = 0;
    uint8_t j = 0;
    uint8_t k = 0;
    uint8_t ind = 0;
    uint8_t dec = 0;

    for ( i = 0 ; i < numberOfBlocks ; i++ )
    {
        for ( j = 0 ; j < 16 ; j++ )
        {
            for ( k = 0 ; k < 4 ; k++ )
            {
                ind = ( j * 4 ) + k;
//                printf("%d ", message[ind]);
//                dec = 8 * ( 3 - k );
//                messageDivided[i][j] &=  ( ( (uint32_t) message[ind] ) << dec );
            }
        }
    }
}

void hashSHA1 ( char * message )
{
    uint64_t size = strlen ( message );
    uint8_t * test = ( uint8_t * ) calloc(  size , sizeof( uint8_t ) );;

    printf("\nPreprocessing\n");
    test = preprocessing( message , &size , test );
    printf("Back to hashSHA1\n");
    displayArray(size, test);

//    uint64_t numberOfBlocks = size >> 6;//Divide by 64
//    uint32_t ** divided = (uint32_t**) calloc( numberOfBlocks, sizeof(uint8_t*) );
////
//    divideInBlocks( test , size , divided );

//    uint32_t h0 = 0x67452301;
//    uint32_t h1 = 0xEFCDAB89;
//    uint32_t h2 = 0x98BADCFE;
//    uint32_t h3 = 0x10325476;
//    uint32_t h4 = 0xC3D2E1F0;
    free( test );
    //free( divided );
}

int main()
{
    char * message = "abcdefghijklmnopqrstuvwxyz";

    hashSHA1( message );

    system( "pause" );

    return 0;
}
