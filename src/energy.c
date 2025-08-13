#include "energy.h"

#include <stdlib.h>
#include <string.h>

#include "indexing.h"
#include "util.h"

uint32_t max(uint32_t a, uint32_t b) { return a > b ? a : b; }

uint32_t min(uint32_t a, uint32_t b) { return a < b ? a : b; }

/**
 * Calculate the difference of two color values @p a and @p b.
 * The result is the sum of the squares of the differences of the three (red,
 * green and blue) color components.
 * */
inline uint32_t diff_color(struct pixel const a, struct pixel const b) {
    // TODO implement (assignment 3.2)
    // Pixel a RGB
    int redA = a.r;
    int greenA = a.g;
    int blueA = a.b;

    // Pixel b RGB
    int redB = b.r;
    int greenB = b.g;
    int blueB = b.b;

    uint32_t colourDifference = (redA - redB) * (redA - redB) +
                                (greenA - greenB) * (greenA - greenB) +
                                (blueA - blueB) * (blueA - blueB);
    return colourDifference;
    // NOT_IMPLEMENTED;
    // UNUSED(a);
    // UNUSED(b);
}

/**
 * Calculate the total energy at every pixel of the image @p `img`,
 * but only considering columns with index less than @p `w`.
 * To this end, first calculate the local energy and use it to calculate the
 * total energy.
 * @p `energy` is expected to have allocated enough space
 * to represent the energy for every pixel of the whole image @p `img.
 * @p `w` is the width up to (excluding) which column in the image the energy
 * should be calculated. The energy is expected to be stored exactly analogous
 * to the image, i.e. you should be able to access the energy of a pixel with
 * the same array index.
 */
void calculate_energy(uint32_t* const energy, struct image* const img,
                      int const w) {
    // TODO implement (assignment 3.2)
    // Goes from top to bottom once a row has been processed left to right.
    // To calculate local energy.
    for (int height = 0; height < img->h; height++) {
        // Goes from left to right.
        for (int column = 0; column < w; column++) {
            int index = yx_index(height, column, img->w);
            // EDGE CASE: (0, 0) top-most-left-most pixel.
            if (height == 0 && column == 0) {
                energy[index] = 0;
                continue;
            }
            // EDGE CASE: Just the left pixel for the difference.
            if (height == 0) {
                energy[index] = diff_color(
                    img->pixels[index],
                    img->pixels[yx_index(height, column - 1, img->w)]);
                continue;
            }
            // EDGE CASE: Just the top pixel for the difference.
            if (column == 0) {
                energy[index] = diff_color(
                    img->pixels[index],
                    img->pixels[yx_index(height - 1, column, img->w)]);
                continue;
            }
            // Normal Case: Both the left and the top pixels.
            energy[index] =
                diff_color(img->pixels[index],
                           img->pixels[yx_index(height, column - 1, img->w)]) +
                diff_color(img->pixels[index],
                           img->pixels[yx_index(height - 1, column, img->w)]);
        }
    }

    // To calculate total energy.
    for (int height = 1; height < img->h; height++) {
        for (int column = 0; column < w; column++) {
            int index = yx_index(height, column, img->w);

            // Normal case: starting from the second row.
            // upperPixels[0] == upperLeft
            // upperPixels[1] == upperTop
            // upperPixels[2] == upperRight
            uint32_t upperPixels[3];
            int count = 0;

            // Add upperLeft Pixel
            // NOT at the left-most column!
            // count++ is post increament, count is first used to 0, and then
            // increase to +1.
            if (column > 0) {
                upperPixels[count++] =
                    energy[yx_index(height - 1, column - 1, img->w)];
            }

            // Always add the upperTop Pixel.
            upperPixels[count++] = energy[yx_index(height - 1, column, img->w)];

            // Add upperRight Pixel
            // NOT at the right-most column!
            if (column < w - 1) {
                upperPixels[count++] =
                    energy[yx_index(height - 1, column + 1, img->w)];
            }

            // To find the smallest local energy Pixel.
            uint32_t smallestEnergy;
            if (count > 0) {
                smallestEnergy = upperPixels[0];
                for (int i = 0; i < count; i++) {
                    if (upperPixels[i] < smallestEnergy) {
                        smallestEnergy = upperPixels[i];
                    }
                }
            }
            energy[index] += smallestEnergy;
        }
    }
}

/**
 * Calculate the index of the column with the least energy in bottom row.
 * Expects that @p `energy` holds the energy of every pixel of @p `img` up to
 * column (excluding) @p `w`. Columns with index `>= w` are not considered as
 * part of the image.
 * @p `w0` states the width of the energy matrix @p `energy`
 * @p `h` states the height of the energy matrix @p `energy`
 */
int calculate_min_energy_column(uint32_t const* const energy, int const w0,
                                int const w, int const h) {
    // TODO implement (assignment 3.2)

    // Manipulating energy directly.
    // Got the left-most energy in the bottom row.
    int smallestEnergy = energy[yx_index(h - 1, 0, w0)];
    // To keep track of the columns as we iterate through.
    int smallestEnergyColumn = 0;
    for (int i = 0; i < w; i++) {
        if (energy[yx_index(h - 1, i, w0)] < smallestEnergy) {
            smallestEnergy = energy[yx_index(h - 1, i, w0)];
            smallestEnergyColumn = i;
        }
    }

    return smallestEnergyColumn;
    // NOT_IMPLEMENTED;
    // UNUSED(energy);
    // UNUSED(w0);
    UNUSED(w);
    // UNUSED(h);
}

/**
 * Calculate the optimal path (i.e. least energy), according to the energy
 * entries in @p `energy` up to (excluding) column @p `w`. The path is stored in
 * @p `seam`. Columns with index `>= w` are not considered as part of the image.
 * @p `x` is the index in the bottom row where the seam starts.
 * @p `w0` states the width of the energy matrix @p `energy`
 * @p `h` states the height of the energy matrix @p `energy`
 */
void calculate_optimal_path(uint32_t const* const energy, int const w0,
                            int const w, int const h, int x,
                            uint32_t* const seam) {
    // TODO implement (assignment 3.2)
    // Got the bottom most index.
    // x = calculate_min_energy_column(energy, w0, w, h);
    int count = h - 1;
    seam[count--] = x;

    // Now starting the preference comparison from the second-most-bottom row.
    // Bottom most row is h - 1 thanks to C indexing.
    for (int y = h - 2; y >= 0; y--) {
        // Assuming upperTop is the least already. Thanks to preference.
        uint32_t leastEnergy = energy[yx_index(y, x, w0)];
        int offset = 0;

        // Get upperLeft Pixel (if possible).
        if (x > 0) {
            uint32_t upperLeft = energy[yx_index(y, x - 1, w0)];
            if (upperLeft < leastEnergy) {
                leastEnergy = upperLeft;
                offset = -1;
            }
        }

        // Get upperRight (if possible).
        if (x < w - 1) {
            uint32_t upperRight = energy[yx_index(y, x + 1, w0)];
            if (upperRight < leastEnergy) {
                leastEnergy = upperRight;
                offset = 1;
            }
        }

        x += offset;
        seam[count--] = x;
    }
    /*NOT_IMPLEMENTED;
    UNUSED(energy);
    UNUSED(w0);
    UNUSED(w);
    UNUSED(h);
    UNUSED(x);
    UNUSED(seam);*/
}
