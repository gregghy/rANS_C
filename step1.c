#include <stdio.h>
#include <stdlib.h>

int encode_step(int, int);
int encode (int*, int);
int decode_step(int*);
int* decode (int, int);
void reverseArr(int*, int);

int main() {
  int data_input[5] = {3, 2, 4, 1, 5};
  printf("Result encoding: %d\n", encode(data_input, 5));

  printf("Encoded in %b bits\n", encode(data_input, 5));
  int* decoded = decode(encode(data_input, 5), 5);
  for (int i = 0; i < 5; i++) {
    printf("Decoded: %d\n", decoded[i]);
  }
}

void reverseArr(int* arr, int size) {
  for (int i = 0; i < size/2; i++) {
    int temp = arr[i];
    arr[i] = arr[size - 1 - i];
    arr[size - 1 - i] = temp;
  }
}

int decode_step(int* x) {
  int s = *x%10;
  *x /= 10;
  return s;
}

int* decode (int x, int size) {
  int* symbols = malloc(sizeof(int)*size);
  for (int i = 0; i < size; i++) {
    int s = decode_step(&x);
    symbols[i] = s;
  }

  reverseArr(symbols, size);

  return symbols;
}

int encode_step(int x_prec, int s) {
  int x = x_prec * 10 + s;
  return x;
}

int encode (int* data_input, int size) {
  int x = 0;
  for (int s = 0; s < size; s++) {
    x = encode_step (x, data_input[s]);
  }

  
  return x;
}
