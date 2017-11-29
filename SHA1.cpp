#include <iostream>
#include <cstdio>
#include <iomanip>

using namespace std;

char showHex[ 16 ] =
{
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

struct Stream
{
    Stream () : succ( NULL ) {}
    int number;
    Stream* succ;
};

int rotate ( int number, int time )
{
    unsigned int left = (unsigned)number << time;
    unsigned int right = (unsigned)number >> ( 32 - time );
    unsigned int ans = right + left;
    return ans;
}

class SHA
{
public:
    SHA (): set_message( false )
    {
        HASH[ 0 ] = 0x67452301;
        HASH[ 1 ] = 0xEFCDAB89;
        HASH[ 2 ] = 0x98BADCFE;
        HASH[ 3 ] = 0x10325476;
        HASH[ 4 ] = 0xC3D2E1F0;
    }

    void insert( int number )
    {
        if ( !set_message )
        {
            message.number = number / 16;
            message.succ = new Stream;
            message.succ->number = number %16;
            end = message.succ;
            set_message = true;
        }
        else
        {
            end->succ = new Stream;
            end->succ->number = number / 16;
            end->succ->succ = new Stream;
            end->succ->succ->number = number %16;
            end = end->succ->succ;
        }
    }

    void setLength( int number )
    {
        length = number * 8;
    }

    void reset()
    {
        HASH[ 0 ] = 0x67452301;
        HASH[ 1 ] = 0xEFCDAB89;
        HASH[ 2 ] = 0x98BADCFE;
        HASH[ 3 ] = 0x10325476;
        HASH[ 4 ] = 0xC3D2E1F0;
    }

    void PRESHA()
    {
        int number = ( ( 448 - length % 512 ) % 512 ) / 8;
        for ( int i = 0; i < number; i++ )
            if ( i == 0 )
                insert( 128 );
            else
                insert( 0 );

        int length_8[8];
        for ( int i = 0, tmp = length; i < 8; i++, tmp/=256 )
            length_8[ i ] = tmp % 256;

        for ( int i = 0; i < 8; i++ )
            insert( length_8[ 8 - i - 1 ] );
    }

    int K ( int number )
    {
        if ( number < 20 )
            return 0x5A827999;
        else if ( number < 40 )
            return 0x6ED9EBA1;
        else if ( number < 60 )
            return 0x8F1BBCDC;
        else if ( number < 80 )
            return 0xCA62C1D6;
    }

    int func ( int number, int B, int C, int D )
    {
        if ( number < 20 )
            return ( B & C ) | ( ( ~B ) & D );
        else if ( number < 40 )
            return B xor C xor D;
        else if ( number < 60 )
            return ( B & C ) | ( B & D ) | ( C & D );
        else if ( number < 80 )
            return B xor C xor D;
    }

    void DOSHA()
    {
        int number = ( ( 448 - length % 512 ) % 512 ) / 8;
        int chunk = ( length + number * 8 + 64 ) / 512;
        int MAP[ 80 ];
        Stream *tmp = &message;
        for ( int time = 0; time < chunk; time++ )
        {
            for ( int i = 0; i < 16; i++ )
            {
                int ans = 0;
                for ( int j = 0; j < 8; j++ )
                {
                    ans = ans * 16 + tmp->number;
                    tmp = tmp->succ;
                }
                MAP[ i ] = ans;
            }
            for ( int i = 16; i < 80; i++ )
                MAP[ i ] = rotate ( MAP[ i - 3 ] xor MAP[ i - 8 ] xor MAP[ i - 14 ] xor MAP[ i - 16 ], 1 );
            unsigned int HASH_table[ 5 ];
            for ( int i = 0; i < 5; i++ )
                HASH_table[ i ] = HASH[ i ];
            for ( int i = 0; i < 80; i++ )
            {
                int temp = rotate ( HASH_table[ 0 ], 5 ) + func ( i, HASH_table[ 1 ], HASH_table[ 2 ], HASH_table[ 3 ] ) + HASH_table[ 4 ] + MAP[ i ] + K ( i );
                HASH_table[ 4 ] = HASH_table[ 3 ];
                HASH_table[ 3 ] = HASH_table[ 2 ];
                HASH_table[ 2 ] = rotate ( HASH_table[ 1 ], 30 );
                HASH_table[ 1 ] = HASH_table[ 0 ];
                HASH_table[ 0 ] = temp;
            }
            for ( int i = 0; i < 5; i++ )
                HASH[ i ] += HASH_table[ i ];
        }
    }

    void ANSSHA()
    {
        for ( int i = 0; i < 5; i++ )
           cout << setfill('0') << setw(8) << hex << HASH[ i ] << " ";
        cout << endl;
    }

    void show()
    {
        Stream *tmp = &message;
        while ( tmp->succ != NULL )
        {
            cout << showHex[ tmp->number ];
            tmp = tmp->succ;
        }
        cout << showHex[ tmp->number ] << endl;
    }

private:
    bool set_message;
    int length;
    unsigned int HASH[ 5 ];
    Stream message;
    Stream *end;
};

int main ()
{
    while ( true )
    {
        cout << "Enter message: ";
        char inputChar;
        SHA Problem;
        int number = 0;
        while ( ( inputChar = getchar() ) != '\n' )
        {
            Problem.insert(inputChar);
            number++;
        }
        Problem.reset();
        Problem.setLength( number );
        Problem.PRESHA();
        Problem.DOSHA();
        Problem.ANSSHA();
        cout << endl;
    }

    return 0;
}
