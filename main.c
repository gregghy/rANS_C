#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "aux.h"

#define SIZE_ALPHA 26
#define SIZE_DATA 18
#define M 64
#define N 16
//2**N - 1
#define MASK 0xFFFF
//2**2N - 1
#define BOUND  0xFFFFFFFF


typedef unsigned long long ull; 

ull encode_step (int*, int*, int*, int, int, int*, int*);
ull encode (int*, int*, int*, int*, int*, int*);
int decode_step (ull*, int*, int*, int*, int*);
int* decode (ull, int*, int*, int*, int*, int*);

int main() {
  int alphabet[SIZE_ALPHA] = {97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122};
  int freq[SIZE_ALPHA] = {4, 1, 2, 3, 4, 1, 2, 4, 5, 1, 1, 2, 1, 5, 5, 1, 1, 4, 4, 6, 2, 1, 1, 1, 1, 1};
  int cumul[SIZE_ALPHA] = {0, 4, 5, 7, 10, 14, 15, 17, 21, 26, 27, 28, 30, 31, 36, 41, 42, 43, 47, 51, 57, 59, 60, 61, 62, 63};

  int data_input[SIZE_DATA] = {122, 97, 121, 98, 120, 99, 119, 100, 118, 101, 117, 102, 116, 103, 115, 104, 114, 105};

  int* overflow_tab = malloc(sizeof(int)*4); //SIZE_DATA in general safe option
  int renormalizations = 0;

  printf("Input = [122, 97, 121, 98, 120, 99, 119, 100, 118, 101, 117, 102, 116, 103, 115, 104, 114, 105]\n\n");
  uint32_t encoded = encode(alphabet, freq, cumul, data_input, overflow_tab, &renormalizations);

  for (int i = 0; i < renormalizations; i++) {
    printf("encoded: %d +\n", overflow_tab[i]);
  }
  printf("encoded: %d\n\n", encoded);
  printf("Encoded in %b + %b + %b bits \n\n", overflow_tab[0], overflow_tab[1], encoded);
  int* decoded = decode(encoded, freq, cumul, alphabet, overflow_tab, &renormalizations);
  for (int i = 0; i < SIZE_DATA; i++) {
    printf("decoded = %d\n", decoded[i]);
  }


}

int* decode(ull x, int* freq, int* cumul, int* alphabet, int* overflow_tab, int* renormalizations) {
  int* decoded = malloc(sizeof(int)*SIZE_DATA);
  *renormalizations = *renormalizations - 1;
  for (int i = 0; i < SIZE_DATA; i++) {
    int s = decode_step(&x, cumul, freq, overflow_tab, renormalizations);
    decoded[i] = alphabet[s];
  }
  reverseArr(decoded, SIZE_DATA);
  return decoded;
}

int decode_step(ull* x, int* cumul, int* freq, int* overflow_tab, int* renormalizations) {
  int block_id = floor(*x/M);
  int slot = *x%M;
  // cumulus[s] <= slot < cumulus[s+1]
  int s = bin_search(slot, cumul, SIZE_ALPHA);

  *x = block_id*freq[s] + slot - cumul[s];
  
  if (*x == 0 && *renormalizations >= 0) {
    *x = overflow_tab[*renormalizations];
    *renormalizations = *renormalizations - 1;
  }
  return s;
}

ull encode_step(int* alphabet, int* freq, int* cumul, int x_prec, int s, int* overflow_tab, int* renormalizations) {
  //int x = (floor(x_prec/freq[s]))*M + cumul[s] + x_prec%freq[s];
  int index = find_index(s, alphabet, SIZE_ALPHA);
  int freq_s = freq[index];
  int cumul_s = cumul[index];

  int block_id = floor(x_prec/freq_s);
  int slot = cumul_s + (x_prec%freq_s);

  ull x_test = (block_id<<6) + slot;

  if (x_test > BOUND) {
    overflow_tab[*renormalizations] = x_prec;
    *renormalizations = *renormalizations+1;
    x_prec = 0;
    block_id = floor(x_prec/freq_s);
    slot = cumul_s + (x_prec%freq_s);
  }
  ull x = (block_id<<6) + slot;

  return x;
}

ull encode (int* alphabet, int* freq, int* cumul, int* data_input, int* overflow_tab, int* renormalizations) {
  ull x = 0;
  for (int s = 0; s < SIZE_DATA; s++) {
    x = encode_step(alphabet, freq, cumul, x, data_input[s], overflow_tab, renormalizations);
  }

  return x;
}
