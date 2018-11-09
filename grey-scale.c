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

uint8_t weights[3] = {77, 151, 28};
uint8_t *source = NULL;
uint8_t *destination = NULL;
uint64_t columns = 0;
uint64_t lines = 0;
int max_color = 255;
char *input_filename, *output_filename;
int input_file_binary = 0;

void help() {
    printf("Command: ./grey-scale input output\n");
    printf("You may also define the weights for the RGB colors by:\n");
    printf("  ./grey-scale red green blue input output\n");
    printf("Example: ./grey-scale image/2052.ppm output/2052.pgm\n");
}

int parse_arguments(int argc, char *argv[]) {
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
            return 1;
    }
    return 0;
}

int parse_header(FILE *fd, size_t *header_size) {
    size_t buf_len = 1024;
    *header_size = 0;
    char buffer[buf_len];
    if (fgets(buffer, buf_len, fd) == NULL) {
        perror("parse_header fgets");
        return 1;
    }

    // parse magic number
    // P3 (ascii) or P6 (binary)
    if (buffer[0] != 'P') {
        fprintf(stderr, "Invalid magic header.\n");
        return 1;
    }
    switch (buffer[1]) {
        case '3':
            break;
        case '6':
            input_file_binary = 1;
            break;
        default:
            fprintf(stderr, "Invalid magic header.\n");
            return 1;
    }
    if (buffer[2] != 10) {
        fprintf(stderr, "No new line after magic number.\n");
        return 1;
    }

    size_t i = 3;
    int comment_mode = 0;
    while (1) {
        // if buffer ended, load more bytes from file
        if (i == 0 && buffer[i] == 0) {
            break;
        }
        if (buffer[i] == 0) {
            *header_size += i;
            i = 0;
            if (fgets(buffer, buf_len, fd) == NULL) {
                perror("parse_header while-fgets");
                return 1;
            }
            continue;
        }
        // if we are in comment, wait for the new line
        if (comment_mode) {
            if (buffer[i] == '\n') {
                comment_mode = 0;
            }
            i++;
            continue;
        }
        // if this is '#', enter comment mode
        if (buffer[i] == '#') {
            comment_mode = 1;
            i++;
            continue;
        }
        if (buffer[i] == ' ') {
            i++;
            continue;
        }

        break;
    }
    if (buffer[i] == 0) {
        fprintf(stderr, "parse_header: got EOF\n");
        return 1;
    }
    *header_size += i;

    // seek to the beginning of the valid 'lines columns'
    if (fseek(fd, *header_size, SEEK_SET) != 0) {
        perror("parse_header: fseek fd before dimensions");
        return 1;
    }
    if (fscanf(fd, "%ld %ld\n", &columns, &lines) <= 0) {
        perror("parse_header: fscanf parse dimensions");
        return 1;
    }
    if (fscanf(fd, "%d\n", &max_color) <= 0) {
        perror("parse_header: fscanf max color line");
        return 1;
    }
    long offset = ftell(fd);
    if (offset == -1) {
        perror("parse_header: getting end header offset");
        return 1;
    }
    *header_size = offset;

    return 0;
}

int parse_content_binary(FILE *fd, size_t header_size) {
    long end_file_offset;
    if (fseek(fd, 0, SEEK_END) == -1) {
        perror("parse_content_binary: could not seek to end");
        return 1;
    }
    end_file_offset = ftell(fd);
    if (end_file_offset == -1) {
        perror("parse_content_binary: ftell end file offset");
        return 1;
    }
    if (fseek(fd, header_size, SEEK_SET) == -1) {
        perror("parse_content_binary: could not seek after header");
        return 1;
    }
    size_t content_size = end_file_offset - header_size;
    if (fread(source, content_size, 1, fd) <= 0) {
        perror("parse_content_binary: fread");
        return 1;
    }
    return 0;
}

int parse_file() {
    FILE *fd = fopen(input_filename, "rb");
    if (fd == NULL) {
        return 1;
    }

    size_t header_size;
    if (parse_header(fd, &header_size) != 0) {
        fclose(fd);
        return 1;
    }

    // allocate memory for source data
    source = (uint8_t*)malloc(columns*lines*3);
    if (source == NULL) {
        perror("parse_file: allocating memory");
        fclose(fd);
        return 1;
    }

    int status;
    if (input_file_binary) {
        status = parse_content_binary(fd, header_size);
    } else {
        status = 0;
    }
    fclose(fd);
    return status;
}

int write_file() {
    FILE *fd = fopen(output_file, "wb");

    return 0;
}

void cleanup() {
    if (source != NULL) {
        free(source);
    }
    if (destination != NULL) {
        free(destination);
    }
}

int main(int argc, char **argv) {
    if (parse_arguments(argc, argv) != 0) {
        help();
        return 1;
    }
    if (parse_file() != 0) {
        cleanup();
        return 1;
    }

    destination = (uint8_t*)malloc(lines*columns);
    if (destination == NULL) {
        cleanup();
        return 1;
    }

    grey_scale(source, destination, columns, lines, weights);

    for (uint64_t i = 0; i < lines; i++) {
        for (uint64_t j = 0; j < columns; j++) {
            printf("%hx ", source[i*columns + j]);
        }
    }
    printf("\n");

    cleanup();
    return 0;
}

