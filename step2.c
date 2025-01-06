#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int encode_step (int*, int*, int*, int, int, int, int);
int encode (int*, int*, int*, int, int*, int);
int decode_step (int*, int, int*, int, int*);
int bin_search(int, int*, int);
int* decode (int, int, int, int*, int*, int*, int);
void reverseArr(int*, int);
int find_index(int, int*, int);

int main() {
  int alphabet[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  int freq[10] =     {3, 3, 2, 1, 1, 4, 3, 2, 4, 5};
  int cumul[10] =    {0, 3, 6, 8, 9, 10, 14, 17, 19, 23};
  int M = 28;
  int data_input[4] = {3, 5, 8, 0};
  int size_data = 4;
  int size_cumul = 10;

  printf("Input = [3, 5, 8, 0]\n\n");
  int encoded = encode(alphabet, freq, cumul, M, data_input, size_data);
  printf("encoded: %d\n\n", encoded);
  printf("Encoded in %b bits\n\n", encoded);
  int* decoded = decode(encoded, size_data, size_cumul, freq, cumul, alphabet, M);
  for (int i = 0; i < size_data; i++) {
  printf("decoded = %d\n", decoded[i]);
  }

}

int* decode(int x, int size_data, int size_cumul, int* freq, int* cumul, int* alphabet, int M) {
  int* decoded = malloc(sizeof(int)*size_data);
  for (int i = 0; i < size_data; i++) {
    int s = decode_step(&x, M, cumul, size_cumul, freq);
    decoded[i] = alphabet[s];
  }
  reverseArr(decoded, size_data);
  return decoded;
}

int decode_step(int* x, int M, int* cumul, int size, int* freq) {
  int block_id = floor(*x/M);
  int slot = *x%M;

  // cumul[s] <= slot < cumul[s+1]
  int s = bin_search(slot, cumul, size);

  *x = block_id*freq[s] + slot - cumul[s];
  return s;
}

int encode_step(int* alphabet, int* freq, int* cumul, int x_prec, int s, int M, int size) {
  //int x = (floor(x_prec/freq[s]))*M + cumul[s] + x_prec%freq[s];
  int index = find_index(s, alphabet, 10);
  int block_id = floor(x_prec/freq[index]);
  int slot = cumul[index] + x_prec%freq[index];

  int x = block_id*M + slot;
  return x;
}

int encode (int* alphabet, int* freq, int* cumul, int M, int* data_input, int size) {
  int x = 0;
  for (int s = 0; s < size; s++) {
    x = encode_step(alphabet, freq, cumul, x, data_input[s], M, size);
    printf("x: %d\n", x);
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

