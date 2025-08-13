# Seam Carving in C

Content-aware image resizing using the classic seam-carving algorithm (Avidan & Shamir, SIGGRAPH 2007). This project is based on the Programming 2 (SS 2025) assignment at Saarland University, Compiler Design Lab and Foundations of Artificial Intelligence Group.

The repository provides a command-line tool to:
- Print basic image statistics
- Output the minimum-energy vertical seam
- Perform content-aware width reduction by repeatedly removing minimum-energy seams

Note: This README mirrors the assignment specifications. If your local structure differs, adapt paths and commands accordingly.

---

## Overview

Seam carving resizes images by removing or inserting paths (seams) of minimal perceptual importance, instead of uniformly scaling. In this project, we only consider downsizing in width by removing vertical seams.

Algorithm summary:
1. Compute the local energy of each pixel based on color differences to neighbors.
2. Compute total energy per pixel (dynamic programming from top to bottom).
3. Identify the minimal-energy vertical seam using the total energies and tie-breaking rules.
4. Remove that seam. Repeat for n steps.

For theoretical background and demos:
- http://www.faculty.idc.ac.il/arik/SCWeb/imret/

---

## Build

A Makefile is provided. From the project root:
```bash
make
```

This builds the following binaries in bin/:
- bin/carve_debug  — Debug build with sanitizers (recommended during development)
- bin/carve_opt    — Optimized build for performance testing
- bin/testrunner   — Internal test runner used by the testing scripts

Example:
```bash
./bin/carve_debug -n 10 test/data/owl.ppm
```

---

## Usage

Command-line syntax:
```
carve_debug [-s] [-p] [-n <count>] <image file>
```

Argument semantics:
- -s  Print statistics (width, height, brightness) and exit.
- -p  Print the x-indices (one per line) of the minimal-energy vertical seam from top to bottom and exit.
- -n <count>  Run <count> seam-removal steps and write the result to out.ppm.
- <image file>  Input image (PPM, see format below). This can appear in any position.

Precedence if multiple flags are present: -s > -p > -n.

Examples:
```bash
# 1) Statistics only
./bin/carve_debug -s test/data/owl.ppm

# 2) Print minimal-energy seam (x-index per row)
./bin/carve_debug -p test/data/owl.ppm

# 3) Remove 20 seams and produce out.ppm
./bin/carve_debug -n 20 test/data/owl.ppm
```

Non-PPM convenience wrapper:
- Use scripts/run.sh to convert from/to common formats via ImageMagick.
```bash
./scripts/run.sh input.jpg output.jpg -n 20
```

---

## What to Implement

You must implement only the specified parts, leaving provided infrastructure intact.

Do implement (and may add helper functions in these files without changing existing signatures):
- src/main.c
  - statistics
  - find_print_min_path
  - find_and_carve_path
- src/image.c
  - All required image utilities EXCEPT do not modify image_init, image_destroy, image_read_from_file
- src/energy.c
- src/indexing.c
- Include headers as needed: image.h, energy.h, indexing.h

Do NOT edit:
- src/argparser.c
- Makefile
- main function in src/main.c
- image_init, image_destroy, image_read_from_file in src/image.c

Do NOT:
- Add extra source/header files
- Delete existing functions (even if you do not implement them)

---

## Specifications

### 1) Print Statistics (-s)

Implement the function statistics in main.c:
- Brightness of a pixel = (R + G + B) / 3 (integer arithmetic only)
- Image brightness = average of all pixel brightness values over all pixels (integer arithmetic only)
- Output (and only this) to stdout:
```
printf("width: %u\n", <width>);
printf("height: %u\n", <height>);
printf("brightness: %u\n", <brightness>);
```

### 2) Minimal-Energy Seam (-p)

Implement find_print_min_path in main.c to:
- Compute local and total energy (see Algorithm below)
- Reconstruct the minimal-energy vertical seam
- Print exactly one x-index per line, from top row to bottom row
- Print nothing else to stdout

Tie-breaking:
- If multiple optimal seams end at the bottom, choose the lowest x-coordinate among minima.
- When backtracking at a pixel with multiple optimal predecessors, prefer top-center, then top-left, then top-right.

### 3) Carving (-n <count>)

Implement find_and_carve_path in main.c:
- Perform <count> steps, each step removes one vertical seam, reducing the image’s internal width by 1.
- After all steps, write the output image to out.ppm.
- The written output image must have the original width: pad a black column on the right for each removed seam (i.e., a black border of width <count>).
- Do not print anything to stdout.

Constraints:
- 0 <= count <= image width

---

## Algorithm (DP)

Local energy:
- For a pixel (x, y), local energy is the sum of squared differences across R, G, B to its left neighbor (x-1, y) if present and its top neighbor (x, y-1) if present.
- Color difference between pixels a and b:
  (a.r - b.r)^2 + (a.g - b.g)^2 + (a.b - b.b)^2

Total energy:
- total(x, y) = local(x, y) + min(total(x-1, y-1), total(x, y-1), total(x+1, y-1)) considering only in-bounds predecessors; for the top row, total = local.

Seam:
- A seam is a list of x-indices, one per row, where adjacent rows differ by -1, 0, or +1 in x.
- After filling total energies row-by-row, pick the minimal total in the bottom row and backtrack to reconstruct the seam using the tie-breaking rules above.

Dynamic programming motivation:
- Avoids exponential recomputation by storing totals row-by-row.

---

## Image Format (PPM, ASCII P3)

- Line 1: P3
- Line 2: <width> <height>
- Line 3: 255
- Then height × width pixels, each pixel has three integers (R G B), each in [0, 255], row-major order.

Reading:
- image_read_from_file (provided) reads the file and exits with EXIT_FAILURE on corrupt input.

Representation:
- Pixels are stored as struct pixel { uint8_t r, g, b; } in a 1D array of length width * height, row-major.
- Use yx_index(x, y, width) to compute indices.
- Mirror the same indexing for any total-energy arrays.

---

## Testing

Run public tests from the project root:
```bash
python ./test/run_tests.py
# or
make check
```

- Test images are under test/data/
- Expected outputs are under test/ref_output/
- You must pass all public tests for a task to receive points for that task.
- The evaluation uses the main branch state at the deadline noted by the course.

Run a specific test:
```bash
./test/run_tests.py -l          # list all tests
./test/run_tests.py -f <name>   # run a single test by name
```

You may add custom tests:
- Register in test/test_registry.py
- Add C unit tests in src/unit_tests.c

---

## Scripts

Convenience shell scripts may be available under scripts/:
- format.sh   — Run clang-format manually
- lint.sh     — Static checks and hints
- run.sh      — Convert non-PPM input to PPM, run carver, convert back
- cutup.sh    — Run with increasing -n values for visual demos

Example:
```bash
./scripts/run.sh input.jpg output.jpg -n 30
./scripts/cutup.sh input.jpg
```

---

## Sanitizers and Debugging

- AddressSanitizer (ASAN) and UndefinedBehaviorSanitizer (UBSAN) are enabled in local tests.
- Ensure no memory leaks or undefined behavior.
- Free all allocated memory.
- You may use valgrind locally:
```bash
valgrind --leak-check=full ./bin/carve_debug -n 5 test/data/owl.ppm
```

---

## Development Notes and Constraints

- Keep the provided Makefile unchanged (grading uses a replacement).
- Do not commit large binary assets; only source files (plus llm.txt).
- Only edit: main.c, image.c, energy.c, indexing.c, and corresponding headers image.h, energy.h, indexing.h.
- Do not remove any existing functions; you may add helpers while keeping signatures unchanged.
- Start with small test images (e.g., test/data/small1.ppm, test/data/small2.ppm) for debugging.

---

## LLM Usage Policy

- LLMs are permitted but should be used sparingly.
- You must fully understand your submitted code and be able to explain it.
- Document all LLM usage verbosely in a file named llm.txt at the repository root and commit it.

---

## Acknowledgements

- Seam Carving for Content-Aware Image Resizing, Shai Avidan and Ariel Shamir, SIGGRAPH 2007.
- “Improved Seam Carving for Video Retargeting,” Rubinstein, Shamir, Avidan, SIGGRAPH 2008.

---

## License

If this repository is for coursework submission, follow your course’s sharing policy. Otherwise, add a LICENSE file (e.g., MIT, BSD, or Apache-2.0) to clarify usage rights.

---
