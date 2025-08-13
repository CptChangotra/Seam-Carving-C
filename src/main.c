#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "argparser.h"
#include "energy.h"
#include "image.h"
#include "util.h"

/**
 * Compute the brightness and print the statistics of @p `img`,
 * i.e. width, height & brightness.
 */
void statistics(struct image* img) {
    // TODO implement (assignment 3.1)
    /* implement and use the function:
     * `image_brightness`
     */
    uint32_t width = img->w;
    uint32_t height = img->h;
    uint32_t brightness = image_brightness(img);
    printf("width: %u\n", width);
    printf("height: %u\n", height);
    printf("brightness: %u\n", brightness);
    // NOT_IMPLEMENTED;
    // UNUSED(img);
}

/**
 * Find & print the minimal path of @p `img`.
 */
void find_print_min_path(struct image* img) {
    // TODO implement (assignment 3.2)
    /* implement and use the functions:
     * - `calculate_energy`
     * - `calculate_min_energy_column`
     * - `calculate_optimal_path`
     * in `energy.c`
     */

    // Allocate memory for the energy array.
    uint32_t* totalEnergy = malloc(img->h * img->w * sizeof(uint32_t));
    if (!totalEnergy) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    // Call calculate energy.
    calculate_energy(totalEnergy, img, img->w);
    int x = calculate_min_energy_column(totalEnergy, img->w, img->w, img->h);
    uint32_t* indexes = malloc(img->h * sizeof(uint32_t));
    if (!indexes) {
        perror("malloc");
        free(totalEnergy);
        exit(EXIT_FAILURE);
    }
    calculate_optimal_path(totalEnergy, img->w, img->w, img->h, x, indexes);

    for (int i = 0; i < img->h; i++) {
        printf("%d\n", indexes[i]);
    }

    free(totalEnergy);
    free(indexes);
    // NOT_IMPLEMENTED;
    // UNUSED(img);
}

/**
 * Find & carve out @p `n` minimal paths in @p `img`.
 * The image size stays the same, instead for every carved out path there is a
 * column of black pixels appended to the right.
 */
void find_and_carve_path(struct image* const img, int n) {
    // TODO implement (assignment 3.3)
    /* implement and use the functions from assignment 3.2 and:
     * - `carve_path`
     * - `image_write_to_file`
     * in `image.c`.
     */

    if (n >= 0 && n <= img->w) {
        int imageWidth = img->w;
        for (int i = 0; i < n; i++) {
            uint32_t* totalEnergy = malloc(img->h * img->w * sizeof(uint32_t));
            if (!totalEnergy) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            // Call calculate energy.
            calculate_energy(totalEnergy, img, imageWidth);
            int x = calculate_min_energy_column(totalEnergy, img->w, imageWidth,
                                                img->h);

            // Index holds the seam.
            uint32_t* indexes = malloc(img->h * sizeof(uint32_t));
            if (!indexes) {
                perror("malloc");
                free(totalEnergy);
                exit(EXIT_FAILURE);
            }
            calculate_optimal_path(totalEnergy, img->w, imageWidth, img->h, x,
                                   indexes);
            // Indexes now holds the seam.

            carve_path(img, imageWidth, indexes);
            imageWidth--;
            free(totalEnergy);
            free(indexes);
        }
    }
    image_write_to_file(img, "out.ppm");
}

/**
 * Parse the arguments and call the appropriate functions as specified by the
 * arguments.
 */
int main(int const argc, char** const argv) {
    // DO NOT EDIT
    bool show_min_path = false;
    bool show_statistics = false;
    int n_steps = -1;

    char const* const filename =
        parse_arguments(argc, argv, &show_min_path, &show_statistics, &n_steps);
    if (!filename) return EXIT_FAILURE;

    struct image* img = image_read_from_file(filename);

    if (show_statistics) {
        statistics(img);
        image_destroy(img);
        return EXIT_SUCCESS;
    }

    if (show_min_path) {
        find_print_min_path(img);
    } else {
        if (n_steps < 0 || n_steps > img->w) n_steps = img->w;

        find_and_carve_path(img, n_steps);
    }

    image_destroy(img);
    return EXIT_SUCCESS;
}
