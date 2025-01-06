#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "aux.h"

#define SIZE_ALPHA 26
#define SIZE_DATA 16
#define M 128
#define N 16
//2**N - 1 = 65535
#define MASK 65535
//2**2N - 1 = 4294967295
#define BOUND  4294967295

typedef unsigned long long llu;

llu encode_step (int*, int*, int*, int, int, uint16_t*, int*);
llu encode (int*, int*, int*, int*, uint16_t*, int*);
int decode_step (llu*, int*, int*, uint16_t*, int*);
int* decode (llu, int*, int*, int*, uint16_t*, int*);
void write16bits(uint16_t, uint16_t*, int*);
uint16_t read16bits(uint16_t*, int*);

int main() {
  char alphabet_char[SIZE_ALPHA] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
  int alphabet_ansi[SIZE_ALPHA] = {97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122};
  int freq[SIZE_ALPHA] = {10, 2, 4, 5, 11, 3, 3, 8, 9, 1, 1, 5, 3, 9, 10, 2, 1, 8, 8, 12, 4, 1, 3, 1, 3, 1};
  int cumul[SIZE_ALPHA] = {0, 10, 12, 16, 21, 32, 35, 38, 46, 55, 56, 57, 62, 65, 74, 84, 86, 87, 95, 103, 115, 119, 120, 123, 124, 127};
  //M = 128 instead of 100 cause 128 = 2^7, << 7 faster than * 100 (guess?)
  int data_input[SIZE_DATA] = {122, 97, 121, 98, 120, 99, 119, 100, 118, 101, 117, 102, 116, 103, 115, 104};
  uint16_t* overflow_tab = calloc(SIZE_DATA, sizeof(uint16_t));
  int renormalizations = 0;

  printf("before encoding\n");
  printf("MASK: %b\n", MASK);
  printf("BOUND: %b\n", BOUND);
  llu encoded = encode(alphabet_ansi, freq, cumul, data_input, overflow_tab, &renormalizations);
  printf("encoded: %d\n\n", encoded);
  int* decoded = decode(encoded, freq, cumul, alphabet_ansi, overflow_tab, &renormalizations);
  for (int i = 0; i < SIZE_DATA; i++) {
  printf("decoded = %d\n", decoded[i]);
  }

}

int* decode(llu x, int* freq, int* cumul, int* alphabet, uint16_t* overflow_tab, int* renormalizations) {
  *renormalizations = *renormalizations - 1;
  int* decoded = malloc(sizeof(int)*SIZE_DATA);
  for (int i = 0; i < SIZE_DATA; i++) {
    printf("x before decoding: %d\n",x);
    int s = decode_step(&x, cumul, freq, overflow_tab, renormalizations);
    decoded[i] = alphabet[s];
    printf("x after decoding: %d\n", x);
  }
  reverseArr(decoded, SIZE_DATA);
  return decoded;
}

int decode_step(llu* x, int* cumul, int* freq, uint16_t* overflow_tab, int* renormalizations) {
  int block_id = floor(*x/M);
  int slot = *x%M;

  // cumulus[s] <= slot < cumulus[s+1]
  int s = bin_search(slot, cumul, SIZE_ALPHA);

  llu x_test = block_id*freq[s] + slot - cumul[s];

  if (x_test < MASK && *renormalizations >= 0) {
    *x = block_id*freq[s] + slot - cumul[s];
    *x = (*x << N) + read16bits(overflow_tab, renormalizations);
  }
  else {
    *x = block_id*freq[s] + slot - cumul[s];
  }
  return s;
}

uint16_t read16bits(uint16_t* overflow_tab, int* renormalizations) {
  uint16_t value = overflow_tab[*renormalizations];
  printf("decoding value: %16b\n", value);
  printf("decoding index: %d\n", *renormalizations);
  overflow_tab[*renormalizations] = 0;
  *renormalizations = *renormalizations - 1;
  return value;
}

void write16bits(uint16_t value, uint16_t* overflow_tab, int* renormalizations) {
  printf("renormalizations = %d\n", *renormalizations);
  printf("value = %16b\n", value);
  overflow_tab[*renormalizations] = value;
  *renormalizations = *renormalizations + 1;
  return;
}

llu encode_step(int* alphabet, int* freq, int* cumul, int x_prec, int s, uint16_t* overflow_tab, int* renormalizations) {
  //int x = (floor(x_prec/freq[s]))*M + cumul[s] + x_prec%freq[s];
  int index = find_index(s, alphabet, SIZE_ALPHA); //sicuramente corretto
  int block_id = floor(x_prec/freq[index]); //sembrerebbe corretto
  //printf("block id : %d\n", block_id);
  int slot = cumul[index] + (x_prec%freq[index]);
  printf("slot: %d\n", slot);
  llu x_test = (block_id * M) + slot;
  //llu x_test = ((int)(floor(x_prec/freq[index]))) + cumul[index] + (x_prec%freq[index]);

  if (x_test < x_prec) {
    printf("WTFWTFWtFWtFWtFWtF\n");
  }
  if (x_test > BOUND) {
    printf("x_prec & MASK: %16b\n", x_prec & MASK);
    write16bits(x_prec & MASK, overflow_tab, renormalizations);
    x_prec = x_prec >> N;
    int block_id = floor(x_prec/freq[index]);
    int slot = cumul[index] + (x_prec%freq[index]);
  }

  //llu x = ((int)(floor(x_prec/freq[index]))<<7) + cumul[index] + (x_prec%freq[index]); 
  llu x = (block_id * M) + slot;
  return x;
}

llu encode (int* alphabet, int* freq, int* cumul, int* data_input, uint16_t* overflow_tab, int* renormalizations) {
  llu x = MASK;
  for (int s = 0; s < SIZE_DATA; s++) {
    printf("x before encoding: %d\n", x);
    x = encode_step(alphabet, freq, cumul, x, data_input[s], overflow_tab, renormalizations);
    printf("x after encoding: %d\n", x);
  }

  return x;
}
