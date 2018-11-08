#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void grey_scale(
    uint8_t *image_src,
    uint8_t *image_dst,
    uint64_t columns,
    uint64_t lines,
    uint8_t weights[3]
);

void help() {
    printf("Command: ./grey-scale input output\n");
    printf("You may also define the weights for the RGB colors by:\n");
    printf("  ./grey-scale red green blue input output\n");
    printf("Example: ./grey-scale image/2052.ppm output/2052.pgm\n");
}

int read_ppm_file_binary(
        FILE *fd,
        uint8_t **colors,
        long length
) {
    long offset_current, offset_endfile, bytes_left;
    offset_current = ftell(fd);
    fseek(fd, 0, SEEK_END);
    offset_endfile = ftell(fd);
    fseek(fd, offset_current, SEEK_SET);

    bytes_left = offset_endfile - offset_current;
    if (bytes_left < length*3) {
        fprintf(stderr, "Error: input file is too short\n");
        return 1;
    }
    *colors = (uint8_t*)malloc(bytes_left);
    if (*colors == NULL) {
        perror("1Error: ");
        return 1;
    }

    if (fread(*colors, bytes_left, 1, fd) != 1) {
        free(*colors);
        return 1;
    }

    return 0;
}

int read_ppm_file(
        char *filename,
        uint8_t **colors,
        uint64_t *lines,
        uint64_t *columns
) {
    FILE *fd;
    fd = fopen(filename, "rb");
    if (fd == NULL) {
        perror("3Error: ");
        return 1;
    }

    char magic[2];
    if (fscanf(fd, "%c%c\n", &magic[0], &magic[1]) <= 0) {
        perror("4Error: ");
        fclose(fd);
        return 1;
    }
    if (magic[0] != 'P') {
        fclose(fd);
        fprintf(stderr, "Error: invalid file's magic number\n");
        return 1;
    }

    int binary;
    switch (magic[1]) {
        case '3':
            binary = 0;
            break;
        case '6':
            binary = 1;
            break;
        default:
            fclose(fd);
            fprintf(stderr, "Error: invalid file's magic number\n");
            return 2;
    }

    if (fscanf(fd, "%ld%ld\n", columns, lines) <= 0) {
        perror("5Error: ");
        fclose(fd);
        return 1;
    }
    if (*columns == 0 || *lines == 0) {
        perror("Error: ");
        fclose(fd);
        return 1;
    }

    int maximum_color;
    if (fscanf(fd, "%d\n", &maximum_color) <= 0) {
        perror("Error: ");
        fclose(fd);
        return 1;
    }

    int status;
    if (binary) {
        status = read_ppm_file_binary(fd, colors, (*columns)*(*lines));
    } else {
        printf("ASCII PPM file is not supported for now.\n");
        status = 1;
    }
    fclose(fd);
    return status;
}

int save_pgm_file(
        char *filename,
        uint8_t *colors,
        uint64_t columns,
        uint64_t lines
) {
    FILE *fd;
    fd = fopen(filename, "wb");
    if (fd == NULL) {
        return 1;
    }

    if (fprintf(fd, "P5\n%ld %ld\n%d\n", columns, lines, 255) < 0) {
        fclose(fd);
        return 1;
    }

    if (fwrite(colors, columns*lines, 1, fd) != 1) {
        fclose(fd);
        return 1;
    }

    fclose(fd);
    return 0;
}

int main(int argc, char **argv) {
    uint8_t weights[3] = {77, 151, 28};
    uint8_t *source = NULL;
    uint8_t *destination = NULL;
    uint64_t columns = 0;
    uint64_t lines = 0;

    char *input_filename, *output_filename;
    switch (argc) {
        case 3:
            input_filename = argv[1];
            output_filename = argv[2];
            break;
        case 6:
            sscanf(argv[1], "%hhd", &weights[0]);
            sscanf(argv[2], "%hhd", &weights[1]);
            sscanf(argv[3], "%hhd", &weights[2]);
            input_filename = argv[4];
            output_filename = argv[5];
            break;
        default:
            help();
            return 0;
    }

    if (read_ppm_file(input_filename, &source, &columns, &lines) != 0) {
        return 1;
    }

    destination = (uint8_t*)malloc(lines*columns);
    if (destination == NULL) {
        free(source);
        return 1;
    }

    grey_scale(source, destination, columns, lines, weights);
    if (save_pgm_file(
                output_filename,
                destination,
                columns,
                lines
            ) != 0) {
        free(source);
        free(destination);
        return 1;
    }

    free(source);
    free(destination);
    return 0;
}

