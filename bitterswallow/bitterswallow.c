#include "common.h"
#include "sharedfuncs.h"

int get_meta( int connfd, int *type, int *magic);
int handle_checks( int connfd, int size, int type, int magic );
int switch_sums( int connfd, int type, int size);
int send_greetz(int connfd);

int ff( int connfd )
{
    int type = 0;
    int magic = 0;
    int retval = send_greetz(connfd);

    if ( retval == -1 )
    {
        return -1;
    }

    while (1)
    {
        int size = get_meta( connfd, &type, &magic );

        if ( size == 0 )
        {
            DEBUG_PRINT("size is 0\n")   
            return 0;
        }

        if (!handle_checks( connfd, size, type, magic) )
            break;
    }

    close(connfd);

    return 1;        
}

int handle_checks( int connfd, int size, int type, int magic )
{
    unsigned char sum[64];
    unsigned char data[0x400];
    memset(data, 0, 0x400);
    memset(sum, 0, 64);
    int len = 0;

    printf("%x %x %x\n", size, type, magic);

    if ( recvdata(connfd, (char*)data, size) == -1 )
    {
        return 0;
    }


    switch (type & 0x3f)
    {
        case 0x00:
            md2(data, size, sum);
            len = 16; 
            break;
        case 0x01:
            md4(data, size, sum); 
            len = 16; 
            break;
        case 0x02:
            md5(data, size, sum); 
            len = 16; 
            break;
        case 0x03:
            crc16_ccitt(data, size, sum); 
            len = 2; 
            break;
        case 0x04:
            crc32(data, size, sum); 
            len = 4; 
            break;
        case 0x05:
            sum8(data, size, sum); 
            len = 1; 
            break;
            break;
        case 0x06:
            sum16(data, size, sum); 
            len = 2; 
            break;
        case 0x07:
            sum32(data, size, sum); 
            len = 4; 
            break;
        case 0x08:
            mul8(data, size, sum); 
            len = 1; 
            break;
        case 0x09:
            mul16(data, size, sum); 
            len = 2; 
            break;
        case 0x0a:
            mul32(data, size, sum); 
            len = 4; 
            break;
        case 0x0b:
            div8(data, size, sum); 
            len = 1; 
            break;
        case 0x0c:
            div16(data, size, sum); 
            len = 2; 
            break;
        case 0x0d:
            div32(data, size, sum); 
            len = 4; 
            break;
        case 0x0e:
            sub8(data, size, sum); 
            len = 1; 
            break;
        case 0x0f:
            sub16(data, size, sum); 
            len = 2; 
            break;
        case 0x10:
            sub32(data, size, sum); 
            len = 4; 
            break;
        case 0x11:
            mod8(data, size, sum); 
            len = 1; 
            break;
        case 0x12:
            mod16(data, size, sum); 
            len = 2; 
            break;
        case 0x13:
            mod32(data, size, sum); 
            len = 4; 
            break;
        case 0x14:
            cos128(data, size, sum); 
            len = 16; 
            break;
        case 0x15:
            acos128(data, size, sum); 
            len = 16; 
            break;
        case 0x16:
            sin128(data, size, sum); 
            len = 16; 
            break;
        case 0x17:
            asin128(data, size, sum); 
            len = 16; 
            break;
        case 0x18:
            tan128(data, size, sum); 
            len = 16; 
            break;
        case 0x19:
            atan128(data, size, sum); 
            len = 16; 
            break;
        case 0x1a:
            len = 0; 
            break;
        case 0x1b:
            fletcher16(data, size, sum); 
            len = 2; 
            break;
        case 0x1c:
            fletcher32(data, size, sum); 
            len = 4; 
            break;
        case 0x1d:
            adler8(data, size, sum); 
            len = 1; 
            break;
        case 0x1e:
            adler16(data, size, sum); 
            len = 2; 
            break;
        case 0x1f:
            adler32(data, size, sum); 
            len = 4; 
            break;
        case 0x20:
            aphash(data, size, sum); 
            len = 4; 
            break;
        case 0x21:
            rshash(data, size, sum); 
            len = 4; 
            break;
        case 0x22:
            jshash(data, size, sum); 
            len = 4; 
            break;
        case 0x23:
            pjwhash(data, size, sum); 
            len = 4; 
            break;
        case 0x24:
            elfhash(data, size, sum); 
            len = 4; 
            break;
        case 0x25:
            bkdrhash(data, size, sum); 
            len = 4; 
            break;
        case 0x26:
            sdbmhash(data, size, sum); 
            len = 4; 
            break;
        case 0x27:
            djbhash(data, size, sum); 
            len = 4; 
            break;
        case 0x28:
            dekhash(data, size, sum); 
            len = 4; 
            break;
        case 0x29:
            bphash(data, size, sum); 
            len = 4; 
            break;
        case 0x2a:
            fnvhash(data, size, sum); 
            len = 4; 
            break;
        case 0x2b:
            xor8(data, size, sum); 
            len = 1; 
            break;
        case 0x2c:
            xor16(data, size, sum); 
            len = 2; 
            break;
        case 0x2d:
            xor32(data, size, sum); 
            len = 4; 
            break;
        case 0x2e:
            sha1(data, size, sum); 
            len = 20; 
            break;
        case 0x2f:
            sha224(data, size, sum); 
            len = 32; 
            break;
        case 0x30:
            sha256(data, size, sum); 
            len = 32; 
            break;
        case 0x31:
            sha384(data, size, sum); 
            len = 64; 
            break;
        case 0x32:
            sha512(data, size, sum); 
            len = 64; 
            break;
        case 0x33:
            murmurhash2(data, size, sum); 
            len = 4; 
            break;
        case 0x34:
            murmurhash3_x86_32(data, size, sum); 
            len = 4; 
            break;
        case 0x35:
            murmurhash3_x86_128(data, size, sum); 
            len = 4; 
            break;
        case 0x36:
            murmurhash3_x64_128(data, size, sum); 
            len = 16; 
            break;
        case 0x37:
            ed2k(data, size, sum); 
            len = 16; 
            break;
        case 0x38:
            len = 0; 
            break;
        case 0x39:
            len = 0; 
            break;
        case 0x3a:
            len = 0; 
            break;
        case 0x3b:
            len = 0; 
            break;
        case 0x3c:
            len = 0; 
            break;
        case 0x3d:
            len = 0; 
            break;
        case 0x3e:
            len = 0; 
            break;
        case 0x3f:
            len = 0; 
            break;
    };

    send_data(connfd, (char*)sum, len);

    send_string(connfd, "Would you like to sum another? (y/n): ");
    recvdata(connfd, (char*)&len, 1);
    if ( len == 'y' || len == 'Y')
    {
        return 1;
    } else
    {
        return 0;
    }

}

int send_greetz(int connfd)
{
    int c = 0;
    int retval = 0;
    
    if ( send_string( connfd, "Welcome to the sums.\n") == -1 )
    {
        retval = -1;
    }

    if ( send_string( connfd, "Are you ready? (y/n): ") == -1 )
    {
        retval = -1;
    }

    if (recvdata(connfd, (char*)&c, 1) == -1)
    {
        retval = -1;
    }

    if ( c == 'y' || c == 'Y')
    {
        retval = 0;
    } else
    {
        retval = -1;
    }

    return retval;
}

int get_meta( int connfd, int *type, int *magic)
{
    int size;
    int ltype;
    int lmagic;

    if ( type == NULL )
    {
        return 0;
    }

    if ( magic == NULL )
    {
        return 0;
    }

    if ( recvdata( connfd, (char *)&ltype, 1 ) == -1 )
    {
        return 0;
    }

    *type = ltype;

    switch (ltype & 0x3f)
    {
        case 0x00:
            memcpy(&lmagic, "MD2\x00", 4);
            goto getsize;
            break;
        case 0x01:
            memcpy(&lmagic, "MD4\x00", 4);
            goto getsize;
            break;
        case 0x02:
            memcpy(&lmagic, "MD5\x00", 4);
            goto getsize;
            break;
        case 0x03:
            memcpy(&lmagic, "CR16", 4);
            goto getsize;
            break;
        case 0x04:
            memcpy(&lmagic, "CR32", 4);
            goto getsize;
            break;
        case 0x05:
            memcpy(&lmagic, "SUM8", 4);
            goto getsize;
            break;
        case 0x06:
            memcpy(&lmagic, "SM16", 4);
            goto getsize;
            break;
        case 0x07:
            memcpy(&lmagic, "SM32", 4);
            goto getsize;
            break;
        case 0x08:
            memcpy(&lmagic, "MUL8", 4);
            goto getsize;
            break;
        case 0x09:
            memcpy(&lmagic, "ML16", 4);
            goto getsize;
            break;
        case 0x0a:
            memcpy(&lmagic, "ML32", 4);
            goto getsize;
            break;
        case 0x0b:
            memcpy(&lmagic, "DIV8", 4);
            goto getsize;
            break;
        case 0x0c:
            memcpy(&lmagic, "DV16", 4);
            goto getsize;
            break;
        case 0x0d:
            memcpy(&lmagic, "DV32", 4);
            goto getsize;
            break;
        case 0x0e:
            memcpy(&lmagic, "SUB8", 4);
            goto getsize;
            break;
        case 0x0f:
            memcpy(&lmagic, "SB16", 4);
            goto getsize;
            break;
        case 0x10:
            memcpy(&lmagic, "SB32", 4);
            goto getsize;
            break;
        case 0x11:
            memcpy(&lmagic, "MOD8", 4);
            goto getsize;
            break;
        case 0x12:
            memcpy(&lmagic, "MD16", 4);
            goto getsize;
            break;
        case 0x13:
            memcpy(&lmagic, "MD32", 4);
            goto getsize;
            break;
        case 0x14:
            memcpy(&lmagic, "COS\x00", 4);
            goto getsize;
            break;
        case 0x15:
            memcpy(&lmagic, "ACOS", 4);
            goto getsize;
            break;
        case 0x16:
            memcpy(&lmagic, "SIN\x00", 4);
            goto getsize;
            break;
        case 0x17:
            memcpy(&lmagic, "ASIN", 4);
            goto getsize;
            break;
        case 0x18:
            memcpy(&lmagic, "TAN\x00", 4);
            goto getsize;
            break;
        case 0x19:
            memcpy(&lmagic, "ATAN", 4);
            goto getsize;
            break;
        case 0x1a:
            memcpy(&lmagic, "BUGE", 4);
            goto out;
            break;
        case 0x1b:
            memcpy(&lmagic, "FL16", 4);
            goto getsize;
            break;
        case 0x1c:
            memcpy(&lmagic, "FL32", 4);
            goto getsize;
            break;
        case 0x1d:
            memcpy(&lmagic, "ADL8", 4);
            goto getsize;
            break;
        case 0x1e:
            memcpy(&lmagic, "AD16", 4);
            goto getsize;
            break;
        case 0x1f:
            memcpy(&lmagic, "AD32", 4);
            goto getsize;
            break;
        case 0x20:
            memcpy(&lmagic, "APHA", 4);
            goto getsize;
            break;
        case 0x21:
            memcpy(&lmagic, "RSHA", 4);
            goto getsize;
            break;
        case 0x22:
            memcpy(&lmagic, "JSHA", 4);
            goto getsize;
            break;
        case 0x23:
            memcpy(&lmagic, "PJWH", 4);
            goto getsize;
            break;
        case 0x24:
            memcpy(&lmagic, "ELFH", 4);
            goto getsize;
            break;
        case 0x25:
            memcpy(&lmagic, "BKDR", 4);
            goto getsize;
            break;
        case 0x26:
            memcpy(&lmagic, "SDBM", 4);
            goto getsize;
            break;
        case 0x27:
            memcpy(&lmagic, "DJBH", 4);
            goto getsize;
            break;
        case 0x28:
            memcpy(&lmagic, "DEKH", 4);
            goto getsize;
            break;
        case 0x29:
            memcpy(&lmagic, "BPHA", 4);
            goto getsize;
            break;
        case 0x2a:
            memcpy(&lmagic, "FNVH", 4);
            goto getsize;
            break;
        case 0x2b:
            memcpy(&lmagic, "XOR8", 4);
            goto getsize;
            break;
        case 0x2c:
            memcpy(&lmagic, "XR16", 4);
            goto getsize;
            break;
        case 0x2d:
            memcpy(&lmagic, "XR32", 4);
            goto getsize;
            break;
        case 0x2e:
            memcpy(&lmagic, "BSA2", 4);
            goto getsize;
            break;
        case 0x2f:
            memcpy(&lmagic, "BSA3", 4);
            goto getsize;
            break;
        case 0x30:
            memcpy(&lmagic, "BSA4", 4);
            goto getsize;
            break;
        case 0x31:
            memcpy(&lmagic, "BSA5", 4);
            goto getsize;
            break;
        case 0x32:
            memcpy(&lmagic, "BSA6", 4);
            goto getsize;
            break;
        case 0x33:
            memcpy(&lmagic, "MMH2", 4);
            goto getsize;
            break;
        case 0x34:
            memcpy(&lmagic, "MM32", 4);
            goto getsize;
            break;
        case 0x35:
            memcpy(&lmagic, "M64A", 4);
            goto getsize;
            break;
        case 0x36:
            memcpy(&lmagic, "M64B", 4);
            goto getsize;
            break;
        case 0x37:
            memcpy(&lmagic, "ED2K", 4);
            goto getsize;
            break;
        case 0x38:
            memcpy(&lmagic, "UNK\x00", 4);
            goto getsize;
            break;
        case 0x39:
            memcpy(&lmagic, "UNK\x00", 4);
            goto getsize;
            break;
        case 0x3a:
            memcpy(&lmagic, "UNK\x00", 4);
            goto getsize;
            break;
        case 0x3b:
            memcpy(&lmagic, "UNK\x00", 4);
            goto getsize;
            break;
        case 0x3c:
            memcpy(&lmagic, "UNK\x00", 4);
            goto getsize;
            break;
        case 0x3d:
            memcpy(&lmagic, "UNK\x00", 4);
            goto getsize;
            break;
        case 0x3e:
            memcpy(&lmagic, "UNK\x00", 4);
            goto getsize;
            break;
        case 0x3f:
            memcpy(&lmagic, "UNK\x00", 4);
            goto getsize;
            break;
    };

getsize:
    if ( recvdata(connfd, (char*)&size, 2) == -1)
    {
        return 0;
    }

    if ( size > 0x400 )
    {
        size = 0x400;
    }

out:
    size = size &0xffff;
    *magic = lmagic;
    return size;
}

int port = 6492;

int main(int argc, char **argv)
{
    int sockfd = SetupSock( port, AF_INET, "em0" );

    accept_loop( sockfd, ff, "bitterswallow");

    return 0;

}
