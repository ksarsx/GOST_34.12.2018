#include <stdio.h>
#include <locale.h>
#include <windows.h>
#include <wincrypt.h>
#include <stdlib.h>

// uint8_t, uint64_t
#include <stdint.h>
// memcpy
#include <string.h>

// ������ ����� � ������(16 ���� = 128 ���)
#define KUZNECHIK_BLOCK_SIZE 16

// ���� ���� (����) �������� ��� ������ ���� ����������� ����� �� 64 ����
typedef uint64_t chunk[2];

// ������� ������� ����������� �������������� ��������� ���� 34.12-2015
const uint8_t Pi[] = {
    0xFC, 0xEE, 0xDD, 0x11, 0xCF, 0x6E, 0x31, 0x16, 0xFB, 0xC4, 0xFA, 0xDA, 0x23, 0xC5, 0x04, 0x4D,
    0xE9, 0x77, 0xF0, 0xDB, 0x93, 0x2E, 0x99, 0xBA, 0x17, 0x36, 0xF1, 0xBB, 0x14, 0xCD, 0x5F, 0xC1,
    0xF9, 0x18, 0x65, 0x5A, 0xE2, 0x5C, 0xEF, 0x21, 0x81, 0x1C, 0x3C, 0x42, 0x8B, 0x01, 0x8E, 0x4F,
    0x05, 0x84, 0x02, 0xAE, 0xE3, 0x6A, 0x8F, 0xA0, 0x06, 0x0B, 0xED, 0x98, 0x7F, 0xD4, 0xD3, 0x1F,
    0xEB, 0x34, 0x2C, 0x51, 0xEA, 0xC8, 0x48, 0xAB, 0xF2, 0x2A, 0x68, 0xA2, 0xFD, 0x3A, 0xCE, 0xCC,
    0xB5, 0x70, 0x0E, 0x56, 0x08, 0x0C, 0x76, 0x12, 0xBF, 0x72, 0x13, 0x47, 0x9C, 0xB7, 0x5D, 0x87,
    0x15, 0xA1, 0x96, 0x29, 0x10, 0x7B, 0x9A, 0xC7, 0xF3, 0x91, 0x78, 0x6F, 0x9D, 0x9E, 0xB2, 0xB1,
    0x32, 0x75, 0x19, 0x3D, 0xFF, 0x35, 0x8A, 0x7E, 0x6D, 0x54, 0xC6, 0x80, 0xC3, 0xBD, 0x0D, 0x57,
    0xDF, 0xF5, 0x24, 0xA9, 0x3E, 0xA8, 0x43, 0xC9, 0xD7, 0x79, 0xD6, 0xF6, 0x7C, 0x22, 0xB9, 0x03,
    0xE0, 0x0F, 0xEC, 0xDE, 0x7A, 0x94, 0xB0, 0xBC, 0xDC, 0xE8, 0x28, 0x50, 0x4E, 0x33, 0x0A, 0x4A,
    0xA7, 0x97, 0x60, 0x73, 0x1E, 0x00, 0x62, 0x44, 0x1A, 0xB8, 0x38, 0x82, 0x64, 0x9F, 0x26, 0x41,
    0xAD, 0x45, 0x46, 0x92, 0x27, 0x5E, 0x55, 0x2F, 0x8C, 0xA3, 0xA5, 0x7D, 0x69, 0xD5, 0x95, 0x3B,
    0x07, 0x58, 0xB3, 0x40, 0x86, 0xAC, 0x1D, 0xF7, 0x30, 0x37, 0x6B, 0xE4, 0x88, 0xD9, 0xE7, 0x89,
    0xE1, 0x1B, 0x83, 0x49, 0x4C, 0x3F, 0xF8, 0xFE, 0x8D, 0x53, 0xAA, 0x90, 0xCA, 0xD8, 0x85, 0x61,
    0x20, 0x71, 0x67, 0xA4, 0x2D, 0x2B, 0x09, 0x5B, 0xCB, 0x9B, 0x25, 0xD0, 0xBE, 0xE5, 0x6C, 0x52,
    0x59, 0xA6, 0x74, 0xD2, 0xE6, 0xF4, 0xB4, 0xC0, 0xD1, 0x66, 0xAF, 0xC2, 0x39, 0x4B, 0x63, 0xB6
};

// ������� ��������� ����������� ��������������
static const uint8_t Pi_reverse[] = {
    0xA5, 0x2D, 0x32, 0x8F, 0x0E, 0x30, 0x38, 0xC0, 0x54, 0xE6, 0x9E, 0x39, 0x55, 0x7E, 0x52, 0x91,
    0x64, 0x03, 0x57, 0x5A, 0x1C, 0x60, 0x07, 0x18, 0x21, 0x72, 0xA8, 0xD1, 0x29, 0xC6, 0xA4, 0x3F,
    0xE0, 0x27, 0x8D, 0x0C, 0x82, 0xEA, 0xAE, 0xB4, 0x9A, 0x63, 0x49, 0xE5, 0x42, 0xE4, 0x15, 0xB7,
    0xC8, 0x06, 0x70, 0x9D, 0x41, 0x75, 0x19, 0xC9, 0xAA, 0xFC, 0x4D, 0xBF, 0x2A, 0x73, 0x84, 0xD5,
    0xC3, 0xAF, 0x2B, 0x86, 0xA7, 0xB1, 0xB2, 0x5B, 0x46, 0xD3, 0x9F, 0xFD, 0xD4, 0x0F, 0x9C, 0x2F,
    0x9B, 0x43, 0xEF, 0xD9, 0x79, 0xB6, 0x53, 0x7F, 0xC1, 0xF0, 0x23, 0xE7, 0x25, 0x5E, 0xB5, 0x1E,
    0xA2, 0xDF, 0xA6, 0xFE, 0xAC, 0x22, 0xF9, 0xE2, 0x4A, 0xBC, 0x35, 0xCA, 0xEE, 0x78, 0x05, 0x6B,
    0x51, 0xE1, 0x59, 0xA3, 0xF2, 0x71, 0x56, 0x11, 0x6A, 0x89, 0x94, 0x65, 0x8C, 0xBB, 0x77, 0x3C,
    0x7B, 0x28, 0xAB, 0xD2, 0x31, 0xDE, 0xC4, 0x5F, 0xCC, 0xCF, 0x76, 0x2C, 0xB8, 0xD8, 0x2E, 0x36,
    0xDB, 0x69, 0xB3, 0x14, 0x95, 0xBE, 0x62, 0xA1, 0x3B, 0x16, 0x66, 0xE9, 0x5C, 0x6C, 0x6D, 0xAD,
    0x37, 0x61, 0x4B, 0xB9, 0xE3, 0xBA, 0xF1, 0xA0, 0x85, 0x83, 0xDA, 0x47, 0xC5, 0xB0, 0x33, 0xFA,
    0x96, 0x6F, 0x6E, 0xC2, 0xF6, 0x50, 0xFF, 0x5D, 0xA9, 0x8E, 0x17, 0x1B, 0x97, 0x7D, 0xEC, 0x58,
    0xF7, 0x1F, 0xFB, 0x7C, 0x09, 0x0D, 0x7A, 0x67, 0x45, 0x87, 0xDC, 0xE8, 0x4F, 0x1D, 0x4E, 0x04,
    0xEB, 0xF8, 0xF3, 0x3E, 0x3D, 0xBD, 0x8A, 0x88, 0xDD, 0xCD, 0x0B, 0x13, 0x98, 0x02, 0x93, 0x80,
    0x90, 0xD0, 0x24, 0x34, 0xCB, 0xED, 0xF4, 0xCE, 0x99, 0x10, 0x44, 0x40, 0x92, 0x3A, 0x01, 0x26,
    0x12, 0x1A, 0x48, 0x68, 0xF5, 0x81, 0x8B, 0xC7, 0xD6, 0x20, 0x0A, 0x08, 0x00, 0x4C, 0xD7, 0x74
};

// ������ ��������� ��������������
const uint8_t linear_vector[] = {
    0x94, 0x20, 0x85, 0x10, 0xC2, 0xC0, 0x01, 0xFB,
    0x01, 0xC0, 0xC2, 0x10, 0x85, 0x20, 0x94, 0x01
};

// ������� X 
void X(chunk a, chunk b, chunk c) 
{
    c[0] = a[0] ^ b[0];
    c[1] = a[1] ^ b[1];
}

// ������� S
void S(chunk in_out) 
{
    // �������
    int i;
    // ������� � ������������� � ������
    uint8_t *byte = (int8_t*) in_out;
    for (i = 0; i < KUZNECHIK_BLOCK_SIZE; i++) 
    {
        byte[i] = Pi[byte[i]];
    }
}

// �������� ������� S
void S_reverse(chunk in_out) 
{
    // �������
    int i;
    // ������� � ������������� � ������
    uint8_t *byte = (int8_t*) in_out;
    for (i = 0; i < KUZNECHIK_BLOCK_SIZE; i++) 
    {
        byte[i] = Pi_reverse[byte[i]];
    }
}

// ������� ��������� � ���� �����
uint8_t GF_mult(uint8_t a, uint8_t b) 
{    
    uint8_t c;

    c = 0;
    while (b) 
    {        
        if (b & 1)
            c ^= a;
        a = (a << 1) ^ (a & 0x80 ? 0xC3 : 0x00);
        b >>= 1;
    }

    return c;
}

// ������� R
void R(uint8_t *in_out) 
{
    // �������
    int i;
    // �����������
    uint8_t acc = in_out[15];
    // ������� � ������������� � ������
    uint8_t *byte = (int8_t*) in_out;
    for (i = 14; i >= 0; i--) 
    {
        byte[i + 1] = byte[i];
        acc ^= GF_mult(byte[i], linear_vector[i]);
    }
    byte[0] = acc;
}

// �������� ������� R
void R_reverse(uint8_t *in_out) 
{
    // �������
    int i;
    // �����������
    uint8_t acc = in_out[0];
    // ������� � ������������� � ������
    uint8_t *byte = (int8_t*) in_out;

    for (int i = 0; i < 15; i++) 
    {
        byte[i] = byte[i + 1];
        acc ^= GF_mult(byte[i], linear_vector[i]);
    }

    byte[15] = acc;
}

// ������� L
void L(uint8_t* in_out) 
{
    // �������
    int i;
    for (i = 0; i < KUZNECHIK_BLOCK_SIZE; i++) 
        R(in_out);
}

// �������� ������� L
void L_reverse(uint8_t *in_out) 
{
    // �������
    int i;
    for (int i = 0; i < 16; i++)
        R_reverse(in_out);
}

// ��������� ������������ ������
void gen_round_keys(uint8_t* key, chunk* round_keys) 
{
    // �������
    int i;

    // ���������
    uint8_t cs[32][KUZNECHIK_BLOCK_SIZE] = {0};

    // ��������� �������� � ������� L-�������������� ������ ��������
    for (i = 0; i < 32; i++) 
    {
        cs[i][15] = i + 1;
        L(cs[i]);
    }

    // ������������ ����� (������ � ��������)
    chunk ks[2] = {0};
    // ��������� ���� ����������
    // ��������� = ������������ ���� = (�������������� � ��������� �� ����)[����� �����][����� �����]
    round_keys[0][0] = ks[0][0] = ((chunk*) key)[0][0];
    round_keys[0][1] = ks[0][1] = ((chunk*) key)[0][1];
    round_keys[1][0] = ks[1][0] = ((chunk*) key)[1][0];
    round_keys[1][1] = ks[1][1] = ((chunk*) key)[1][1];

    // ��������� ���������� ������ � �������������� ��������
    for (i = 1; i <= 32; i++) 
    {
        // ����� ����
        chunk new_key = {0};

        // �������������� X
        // (void*) ��� ��������� �������������� � �������� ����, ������������ � �������
        X(ks[0], (void*)cs[i - 1], new_key);
        // �������������� S
        S(new_key);
        // �������������� L
        // (uint8_t*) ��� ��������� �������������� � �������� ����, ������������ � �������
        L((uint8_t*)&new_key);
        // �������������� X
        X(new_key, ks[1], new_key);

        // �������� �����
        ks[1][0] = ks[0][0];
        ks[1][1] = ks[0][1];

        // ���������� ����� ����
        ks[0][0] = new_key[0];
        ks[0][1] = new_key[1];

        // ������ 8 �������� ���� �������� �� ����������� ������� ������� �����
        if ((i > 0) && (i % 8 == 0)) 
        {
            round_keys[(i >> 2)][0] = ks[0][0];
            round_keys[(i >> 2)][1] = ks[0][1];

            round_keys[(i >> 2) + 1][0] = ks[1][0];
            round_keys[(i >> 2) + 1][1] = ks[1][1];
        }
    }
}

// ������� ����������
// ������������ ������ ���������� � �������� ������
void kuznechik_encrypt(chunk *round_keys, chunk in, chunk out) 
{
    // �������
    int i;
    // �����
    chunk p;
    // �������� ����� ������� ������
    memcpy(p, in, sizeof(chunk));
    // � ������� 10 ��������
    for (i = 0; i < 10; i++) 
    {
        // �������������� X
        X(p, round_keys[i], p);
        // ��� ���� �������� ����� ���������
        if (i < 9)
        {
            // �������������� S
            S(p);
            // �������������� L
            L((uint8_t*)&p);
        }
    }
    // �������� ���������� ���������
    memcpy(out, p, sizeof(chunk));
}

void kuznechik_decrypt(chunk *round_keys, chunk in, chunk out)
{
    // �������
    int i;
    // �����
    chunk p;
    // �������� ����� ������� ������
    memcpy(p, in, sizeof(chunk));

    // �������������� X
    X(p, round_keys[9], p);
    for (i = 8; i >= 0; i--) 
    {
        // �������������� L
        L_reverse((uint8_t*)&p);
        // �������������� S
        S_reverse(p);
        // �������������� X
        X(p, round_keys[i], p);
    }
    // �������� ���������� ���������
    memcpy(out, p, sizeof(chunk));
}

// ������ �����
void print_chunk(chunk p) 
{
    int i;
    for (i = 0; i < sizeof(chunk); i++) 
        printf("0x%02X ", ((uint8_t *)p)[i]);

    printf("\n");
}

// ��������� ����� ����������
void generate_key(uint8_t* key) {
    HCRYPTPROV hCryptProv = 0;

    // �������� ����������������� ��������
    if (!CryptAcquireContext(&hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        printf(stderr, "������: �� ������� �������� ����������������� ��������\n");
        return;
    }

    // ���������� 32 ��������� �����
    if (!CryptGenRandom(hCryptProv, 32, key)) {
        printf(stderr, "������: �� ������� ������������� ��������� �����\n");
        CryptReleaseContext(hCryptProv, 0);
        return;
    }

    // ����������� ��������
    CryptReleaseContext(hCryptProv, 0);
}

// ������� ����������������� ������ key
char* key_to_hex_string(const uint8_t* key) {
    // ������ ���� -> 2 ������� (��������, 0xFF -> "FF"), ���� ������� ����������
    char* result = (char*)malloc(32 * 2 + 1); // 64 ������� + '\0'
    if (result == NULL) {
        return NULL; // ������ ��������� ������
    }

    // ����������� ������ ���� � ��� ����������������� �������
    for (int i = 0; i < 32; i++) {
        // ����������� ���� key[i] ��� ��� ������� (��������, 0xAB -> "AB")
        sprintf(result + i * 2, "%02x", key[i]);
    }

    return result;
}

// ������� ����������� ����������������� ������ � ������ ������ key[32]
int hex_string_to_key(const char* hex_str, uint8_t* key) {
    // ��������� ����� ������: 64 ������� (32 ����� * 2) + �������� '\0'
    size_t len = strlen(hex_str);
    if (len < 64) {
        fprintf(stderr, "������: ������ ������� ��������, ��������� 64 �������\n");
        return -1;
    }

    // ���������, ��� ��� ������� � ���������� ����������������� (0-9, a-f, A-F)
    for (size_t i = 0; i < 64; i++) {
        if (!isxdigit((unsigned char)hex_str[i])) {
            fprintf(stderr, "������: ������������ ������ '%c' �� ������� %zu\n", hex_str[i], i);
            return -1;
        }
    }

    // ����������� ������ ���� �������� � ����
    for (size_t i = 0; i < 32; i++) {
        char byte_str[3] = {hex_str[i * 2], hex_str[i * 2 + 1], '\0'};
        key[i] = (uint8_t)strtoul(byte_str, NULL, 16);
    }

    return 0; // �����
}

uint8_t** text_to_blocks(const char* text, int* cnt) {
    size_t text_len = strlen(text);
    *cnt = (text_len + KUZNECHIK_BLOCK_SIZE - 1) / KUZNECHIK_BLOCK_SIZE;

    uint8_t** blocks = malloc(*cnt * sizeof(uint8_t*));
    if (blocks == NULL) {
        fprintf(stderr, "������: �� ������� �������� ������ ��� ����������\n");
        *cnt = 0;
        return NULL;
    }

    for (int i = 0; i < *cnt; i++) {
        blocks[i] = malloc(KUZNECHIK_BLOCK_SIZE);
        if (blocks[i] == NULL) {
            for (int j = 0; j < i; j++) free(blocks[j]);
            free(blocks);
            fprintf(stderr, "������: �� ������� �������� ������ ��� �����\n");
            *cnt = 0;
            return NULL;
        }
        size_t start = i * KUZNECHIK_BLOCK_SIZE;
        size_t copy_len = text_len - start > KUZNECHIK_BLOCK_SIZE ? KUZNECHIK_BLOCK_SIZE : text_len - start;
        memcpy(blocks[i], text + start, copy_len);
        for (size_t j = copy_len; j < KUZNECHIK_BLOCK_SIZE; j++) {
            blocks[i][j] = 0;
        }
    }

    return blocks;
}



int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "ru");

    printf("�������� 0, ���� ������ ����������� �����\n�������� 1, ���� � ��� ���� 32 ������ ���� � �� ������ ����������� �����\n");
    
    int mode;
    scanf("%d", &mode);

    if (!mode) { // ���������

        printf("���� � ��� ���� ���� ���� ���������� ������� 0\n���� ������ ������������� �����, ������� 1\n");
        scanf("%d", &mode);

        uint8_t key[32];

        if (!mode) {
            char* key_str_16 = (char*)malloc(sizeof(char) * 64);
            scanf("%s", key_str_16);
            hex_string_to_key(key_str_16, key);
        }
        else {
            generate_key(key);
            char* key_str_16 = key_to_hex_string(key);
            printf("\n��� ��������������� ���� - %s\n\n", key_str_16);
        }

        // ������������ �����
        chunk round_keys[10] = {0};  

        // ��������� ������������ ������
        gen_round_keys(key, round_keys);
        
        // ����� ������������ ������
        printf("������������� �����:\n");
        for (int i = 0; i < 10; i++) print_chunk(round_keys[i]);

        ////////////////////////////////////////////////////////////////

        char open_text[500];
        int cnt = 0;
        while (1) {
            scanf("%c", &open_text[cnt]);
            if (open_text[cnt] == '~') {
                open_text[cnt] = '\0';
                break;
            }
            cnt++;
        }
        
        

        int* num_of_blocks = (int*)malloc(sizeof(int));
        uint8_t** blocks = text_to_blocks(open_text, num_of_blocks);
        
        /*
        for (int i = 0; i < *num_of_blocks; i++) {
            for (int j = 0; j < 16; j++) {
                printf("%02x ", blocks[i][j]);
            }
            printf("\n");
        }*/

        for (int i = 0; i < *num_of_blocks; i++) {
            // �������� ������
            uint8_t* data = blocks[i];
            
        }

        

    }
    else { // ������������

    }
    
    /*
    // ���� (256 ��� = 32 ����)
    uint8_t key[32];
    generate_key(key);
    

    // ������������ �����
    chunk round_keys[10] = {0};  

    // ��������� ������������ ������
    gen_round_keys(key, round_keys);

    // ����� ������������ ������
    int i;
    printf("������������� �����:\n");
    for (i = 0; i < 10; i++)
        print_chunk(round_keys[i]);

    // �������� ������
    uint8_t data[KUZNECHIK_BLOCK_SIZE] = { 
        0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x00,
        0xff,0xee,0xdd,0xcc,0xbb,0xaa,0x99,0x88 
    };

    // ����� �������� ������
    printf("�������� ������:\n");
    // (void*) ��� ��������� �������������� � �������� ����, ������������ � �������
    print_chunk((void*)data);

    // ������������� ������
    chunk encrypted;

    // ����������
    // (void*) ��� ��������� �������������� � �������� ����, ������������ � �������
    kuznechik_encrypt(round_keys, (void*)data, encrypted);

    // ����� ������������� ������
    printf("������������� ������:\n");
    print_chunk(encrypted);

    // ��������� �����������
    chunk decrypted;

    // �����������
    kuznechik_decrypt(round_keys, encrypted, decrypted);

    // ����� ������������� ������
    printf("�������������� ������:\n");
    print_chunk(decrypted);
    */
    return 0;
}