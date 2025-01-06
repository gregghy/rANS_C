#include <stdlib.h>

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
