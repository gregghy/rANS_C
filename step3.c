#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>


#define SIZE_ALPHA 26
#define SIZE_DATA 8
#define M 28
#define N 16
//2**N - 1 = 65535
#define MASK 0xFFFF
//2**2N - 1 = 4'294'967'295
#define BOUND  0xFFFFFFFF


typedef unsigned long long ull; 

ull encode_step (int*, int*, int*, int, int, ull*, int*);
ull encode (int*, int*, int*, int*, ull*, int*);
int decode_step (ull*, int*, int*, ull*, int*);
int bin_search(int, int*, int);
int* decode (ull, int*, int*, int*, ull*, int*);
void reverseArr(int*, int);
int find_index(int, int*, int);
void write16bits(uint16_t, uint16_t*, int*);
uint16_t read16bits(uint16_t*, int*);

int main() {
  int alphabet[SIZE_ALPHA] = {97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122};
  int freq[SIZE_ALPHA] = {2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  int cumul[SIZE_ALPHA] = {0, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};

  int data_input[SIZE_DATA] = {122, 97, 121, 98, 120, 99, 119, 100};
  int size_data = SIZE_DATA;
  int size_cumul = SIZE_ALPHA;

  ull* overflow_tab = calloc(SIZE_DATA, sizeof(ull));
  int renormalizations = 0;

  printf("Input = [122, 97, 121, 98]\n\n");
  uint32_t encoded = encode(alphabet, freq, cumul, data_input, overflow_tab, &renormalizations);
  printf("encoded: %d\n\n", encoded);
  int* decoded = decode(encoded, freq, cumul, alphabet, overflow_tab, &renormalizations);
  for (int i = 0; i < size_data; i++) {
  printf("decoded = %d\n", decoded[i]);
  }

}

uint16_t read16bits(uint16_t* overflow_tab, int* renormalizations) {
  uint16_t value = overflow_tab[*renormalizations];
  overflow_tab[*renormalizations] = 0;
  *renormalizations = *renormalizations - 1;
  return value;
}

void write16bits(uint16_t value, uint16_t* overflow_tab, int* renormalizations) {
  overflow_tab[*renormalizations] = value;
  *renormalizations = *renormalizations + 1;
  return;
}

int* decode(ull x, int* freq, int* cumul, int* alphabet, ull* overflow_tab, int* renormalizations) {
  int* decoded = malloc(sizeof(int)*SIZE_DATA);
  *renormalizations = *renormalizations - 1;
  for (int i = 0; i < SIZE_DATA; i++) {
    printf("before x = %d\n", x);
    int s = decode_step(&x, cumul, freq, overflow_tab, renormalizations);
    printf("after x = %d\n", x);
    decoded[i] = alphabet[s];
  }
  reverseArr(decoded, SIZE_DATA);
  return decoded;
}

int bin_search(int x, int* arr, int size) {
  int min = 0, max = size-1;
  int closest = -1;
  while (min <= max) {
    int avg = (min+max)/2;
    if (arr[avg] == x) {
      return avg;
    } else if (arr[avg] < x) {
      closest = avg;
      min = avg+1;
    } else {
      max = avg - 1;
    }
  }
  return closest;
}

int decode_step(ull* x, int* cumul, int* freq, ull* overflow_tab, int* renormalizations) {
  int block_id = floor(*x/M);
  int slot = *x%M;

  // cumulus[s] <= slot < cumulus[s+1]
  int s = bin_search(slot, cumul, SIZE_ALPHA);
  printf("s = %d\n", s);

  *x = block_id*freq[s] + slot - cumul[s];
  //printf("x_test = %d\n", x_test);
  if (*x == 0 && *renormalizations >= 0) {
    *x = overflow_tab[*renormalizations];
    *renormalizations = *renormalizations - 1;
  }
  return s;
}

ull encode_step(int* alphabet, int* freq, int* cumul, int x_prec, int s, ull* overflow_tab, int* renormalizations) {
  //int x = (floor(x_prec/freq[s]))*M + cumul[s] + x_prec%freq[s];
  int index = find_index(s, alphabet, SIZE_ALPHA);
  int block_id = floor(x_prec/freq[index]);
  int slot = cumul[index] + (x_prec%freq[index]);

  ull x_test = block_id*M + slot;

  if (x_test > BOUND) {
    overflow_tab[*renormalizations] = x_prec;
    *renormalizations = *renormalizations+1;
    //write16bits(x_prec & MASK, overflow_tab, renormalizations);
    x_prec = 0;
    block_id = floor(x_prec/freq[index]);
    slot = cumul[index] + (x_prec%freq[index]);
  }
  ull x = block_id*M + slot;

  return x;
}

ull encode (int* alphabet, int* freq, int* cumul, int* data_input, ull* overflow_tab, int* renormalizations) {
  ull x = 0;
  for (int s = 0; s < SIZE_DATA; s++) {
    printf("before x: %d\n", x);
    x = encode_step(alphabet, freq, cumul, x, data_input[s], overflow_tab, renormalizations);
    printf("after x: %d\n", x);
  }

  return x;
}

int find_index(int x, int* a, int size) {
  for (int i = 0; i<size; i++) {
    if (a[i] == x) {
      return i;
    }
  }
  return -1;
}

void reverseArr(int* arr, int size) {
  for (int i = 0; i < size/2; i++) {
    int temp = arr[i];
    arr[i] = arr[size - 1 - i];
    arr[size - 1 - i] = temp;
  }
}

