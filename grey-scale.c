#include <stdint.h>
#include <stdio.h>

void grey_scale(
    uint8_t *image_src,
    uint8_t *image_dst,
    uint64_t columns,
    uint64_t lines,
    uint8_t weights[3]
);

int main() {
    uint8_t source[6] = {100, 100, 100, 150, 150, 150};
    uint8_t destination[2];
    uint8_t weights[3] = {77, 151, 28};

    grey_scale(source, destination, 2, 1, weights);
    for (int i = 0; i < 2; i++) {
        printf("%d ", destination[i]);
    }
    printf("\n");
    return 0;
}
