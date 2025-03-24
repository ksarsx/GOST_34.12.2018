#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "algorithm.h"

#define BLOCK_SIZE 16
#define KEY_SIZE 32
#define ROUNDS 10
/*
typedef struct {
    uint8_t roundKeys[ROUNDS + 1][BLOCK_SIZE];
} KeysSchedule;
*/

void iterationKey(uint8_t* in_1, uint8_t* in_2, uint8_t* out) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        out[i] = in_1[i] ^ in_2[i];
    }
}
void nonlinearTransformation(uint8_t* in, uint8_t* out) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        out[i] = in[i] ^ Pi[i];
    }
}
void nonlinearTransformationInverted(uint8_t* in, uint8_t* out) {
    
}
void linearTransformation() {

}
void linearTransformationInverted() {

}

void encrypt(){

}
void decrypt(){

}

int main()
{
    printf("Hello World!\n");
    
}

