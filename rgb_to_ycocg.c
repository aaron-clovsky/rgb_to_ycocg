/*******************************************************************************
RGB <-> YCoCg conversion implementation and test

Copyright (c) 2026 Aaron Clovsky

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

/*******************************************************************************
Headers
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*******************************************************************************
Conversion
*******************************************************************************/
void rgb_to_ycocg(uint8_t R,
                  uint8_t G,
                  uint8_t B,
                  uint8_t * Y,
                  uint8_t * Co,
                  uint8_t * Cg)
{
    int iR = (int)R;
    int iG = (int)G;
    int iB = (int)B;
    int iY;
    int iCo;
    int iCg;

    iY = (iR + (iG << 1) + iB + 1) >> 2;
    iCo = (iR - iB) >> 1;
    iCg = ((iG << 1) - iR - iB + 1) >> 2;

    *Y = (uint8_t)iY;
    *Co = (uint8_t)iCo;
    *Cg = (uint8_t)iCg;
}

void ycocg_to_rgb(uint8_t Y,
                  uint8_t Co,
                  uint8_t Cg,
                  uint8_t * R,
                  uint8_t * G,
                  uint8_t * B)
{
    int iY = (int)Y;
    int iCo = (int8_t)Co;
    int iCg = (int8_t)Cg;
    int iR;
    int iG;
    int iB;

    iR = iY - iCg + iCo;
    iG = iY + iCg;
    iB = iY - iCg - iCo;

    *R = (uint8_t)((iR < 0) ? 0 : (iR > 255) ? 255 : iR);
    *G = (uint8_t)((iG < 0) ? 0 : (iG > 255) ? 255 : iG);
    *B = (uint8_t)((iB < 0) ? 0 : (iB > 255) ? 255 : iB);
}

/*******************************************************************************
Utilities
*******************************************************************************/
void help(void)
{
    puts("Usage: %s <MODE> [OPTIONS]");
    puts(" modes: RGB | YCoCg | analyze | all | allRGB | allYCoCg");
    puts("  RGB Options: <R> <G> <B>");
    puts("  YCoCg Options: <Y> <Co> <Cg>");
    exit(2);
}

/*******************************************************************************
main()
*******************************************************************************/
int main(int argc, char **argv)
{
    bool analyze = false;
    bool verify = false;
    bool all = false;
    bool rgb = false;

    switch (argc) {
        default:
            help();
            exit(2);
        case 2:
            verify = true;

            if (strcmp(argv[1], "analyze") == 0) {
                analyze = true;
            } else if (strcmp(argv[1], "all") == 0) {
                all = true;
            } else if (strcmp(argv[1], "allRGB") == 0) {
                all = false;
                rgb = true;
            } else if (strcmp(argv[1], "allYCoCg") == 0) {
                all = false;
                rgb = false;
            } else {
                help();
                exit(2);
            }

            break;
        case 5:
            verify = false;

            if (strcmp(argv[1], "RGB") == 0) {
                rgb = true;
                break;
            } else if (strcmp(argv[1], "YCoCg") == 0) {
                rgb = false;
                break;
            } else {
                help();
                exit(2);
            }

            break;
    }

    if (analyze) {
        unsigned same = 0; /* Number of conversions with no difference */
        unsigned diff = 0; /* Number of conversions with any difference */
        unsigned diff_one = 0; /* Number of conversions where */
                               /* exactly one field differs by exactly one */
        unsigned diff_two = 0; /* Number of conversions where
                                  two fields differ by exactly one */
        unsigned diff_any_one = 0; /* Number of conversions where any number
                                      of fields differ by as much as one */
        unsigned diff_any_two = 0; /* Number of conversions where any number
                                      of fields differ by as much as two */

        for (unsigned r = 0; r <= 255; r++) {
            for (unsigned g = 0; g <= 255; g++) {
                for (unsigned b = 0; b <= 255; b++) {
                    uint8_t y, co, cg; /* RGB -> YCoCg Result */
                    uint8_t rr, gg, bb; /* RGB -> YCoCg -> RGB Result */
                    int d_r, d_g, d_b; /* Difference between Original
                                          and double converted values */

                    rgb_to_ycocg((uint8_t)r, (uint8_t)g, (uint8_t)b,
                                 &y, &co, &cg);

                    ycocg_to_rgb(y, co, cg,
                                 &rr, &gg, &bb);

                    /* Calculate difference */
                    d_r = rr - r;
                    d_r = (d_r < 0) ? -d_r : d_r;
                    d_g = gg - g;
                    d_g = (d_g < 0) ? -d_g : d_g;
                    d_b = bb - b;
                    d_b = (d_b < 0) ? -d_b : d_b;

                    /* Identical or not */
                    if (r == rr && g == gg && b == bb) {
                        same++;
                    }
                    else {
                        diff++;
                    }

                    /* Off by exactly one in exactly one channel */
                    if ((d_r == 1 && d_g == 0 && d_b == 0)
                     || (d_r == 0 && d_g == 1 && d_b == 0)
                     || (d_r == 0 && d_g == 0 && d_b == 1)) {
                        diff_one++;
                    }

                    /* Off by exactly one in exactly two channels */
                    if ((d_r == 0 && d_g == 1 && d_b == 1)
                     || (d_r == 1 && d_g == 0 && d_b == 1)
                     || (d_r == 1 && d_g == 1 && d_b == 0)) {
                        diff_two++;

                        puts("Two channels off by one: ");
                        printf(" -> r: %d, g: %d, b: %d\n",
                               (int)r, (int)g, (int)b);
                        printf(" --> Y: %d, Co: %d, Cg: %d\n",
                               (int)y, (int)co, (int)cg);
                        printf(" ---> rr: %d, gg: %d, bb: %d\n\n",
                               (int)rr, (int)gg, (int)bb);
                    }

                    /* Off by no more than one in any number of channels */
                    if (d_r <= 1 && d_g <= 1 && d_b <= 1) {
                        diff_any_one++;
                    }

                    /* Off by no more than two in any number of channels */
                    if (d_r <= 2 && d_g <= 2 && d_b <= 2) {
                        diff_any_two++;
                    }
                    else {
                        puts("Off by more than two in any number of channels:");
                        printf(" -> r: %d, g: %d, b: %d\n",
                               (int)r, (int)g, (int)b);
                        printf(" --> Y: %d, Co: %d, Cg: %d\n",
                               (int)y, (int)co, (int)cg);
                        printf(" ---> rr: %d, gg: %d, bb: %d\n\n",
                               (int)rr, (int)gg, (int)bb);
                    }
                }
            }
        }

        printf("Total: 2^24: %u\n", 1 << 24);
        printf("Identical: %u\n", same);
        printf("Different: %u\n", diff);
        printf("Off by one in one channel: %u\n", diff_one);
        printf("Off by one in two channels: %u\n", diff_two);
        printf("Off by no more than one in all channels: %u\n", diff_any_one);
        printf("Off by no more than two in all channels: %u\n", diff_any_two);
    } else if (verify) {
        if (all) {
            for (unsigned r = 0; r <= 255; r++) {
                for (unsigned g = 0; g <= 255; g++) {
                    for (unsigned b = 0; b <= 255; b++) {
                        uint8_t y, co, cg;
                        uint8_t rr, gg, bb;

                        rgb_to_ycocg((uint8_t)r, (uint8_t)g, (uint8_t)b,
                                     &y, &co, &cg);

                        ycocg_to_rgb(y, co, cg,
                                     &rr, &gg, &bb);

                        printf("%u,%u,%u,%u,%u,%u,%u,%u,%u\n",
                               r, g, b,
                               (unsigned)y, (unsigned)co, (unsigned)cg,
                               rr, gg, bb);
                    }
                }
            }
        } else if (rgb) {
            for (unsigned r = 0; r <= 255; r++) {
                for (unsigned g = 0; g <= 255; g++) {
                    for (unsigned b = 0; b <= 255; b++) {
                        uint8_t y, co, cg;

                        rgb_to_ycocg((uint8_t)r, (uint8_t)g, (uint8_t)b,
                                     &y, &co, &cg);

                        printf("%u,%u,%u,%u,%u,%u\n",
                               r, g, b,
                               (unsigned)y, (unsigned)co, (unsigned)cg);
                    }
                }
            }
        } else {
            for (unsigned y = 0; y <= 255; y++) {
                for (unsigned co = 0; co <= 255; co++) {
                    for (unsigned cg = 0; cg <= 255; cg++) {
                        uint8_t r, g, b;

                        ycocg_to_rgb((uint8_t)y, (uint8_t)co, (uint8_t)cg,
                                     &r, &g, &b);

                        printf("%u,%u,%u,%u,%u,%u\n",
                               y, co, cg,
                               (unsigned)r, (unsigned)g, (unsigned)b);
                    }
                }
            }
        }
    } else if (rgb) {
        int r, g, b;
        uint8_t y, co, cg;

        r = atoi(argv[2]);
        g = atoi(argv[3]);
        b = atoi(argv[4]);

        if (r < 0 || r > 255
         || g < 0 || g > 255
         || b < 0 || b > 255) {
            puts("Input out of range [0-255]");
            exit(2);
        }

        rgb_to_ycocg((uint8_t)r, (uint8_t)g, (uint8_t)b,
                     &y, &co, &cg);

        printf("RGB: %d,%d,%d -> YCoCg: %u,%u,%u\n",
               r, g, b,
               (unsigned)y, (unsigned)co, (unsigned)cg);
    } else {
        int y, co, cg;
        uint8_t r, g, b;

        y = atoi(argv[2]);
        co = atoi(argv[3]);
        cg = atoi(argv[4]);

        if (y < 0 || y > 255
         || co < 0 || co > 255
         || cg < 0 || cg > 255) {
            puts("Input out of range [0-255]");
            exit(2);
        }

         ycocg_to_rgb((uint8_t)y, (uint8_t)co, (uint8_t)cg,
                      &r, &g, &b);

        printf("YCoCg: %d,%d,%d -> RGB: %u,%u,%u\n",
               y, co, cg,
               (unsigned)r, (unsigned)g, (unsigned)b);
    }

    return 0;
}
