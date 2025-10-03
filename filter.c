#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bmp.h"

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int avg = round((image[i][j].rgbtRed + image[i][j].rgbtGreen + image[i][j].rgbtBlue) / 3.0);
            image[i][j].rgbtRed = avg;
            image[i][j].rgbtGreen = avg;
            image[i][j].rgbtBlue = avg;
        }
    }
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int originalRed = image[i][j].rgbtRed;
            int originalGreen = image[i][j].rgbtGreen;
            int originalBlue = image[i][j].rgbtBlue;

            int sepiaRed = round(.393 * originalRed + .769 * originalGreen + .189 * originalBlue);
            int sepiaGreen = round(.349 * originalRed + .686 * originalGreen + .168 * originalBlue);
            int sepiaBlue = round(.272 * originalRed + .534 * originalGreen + .131 * originalBlue);

            image[i][j].rgbtRed = (sepiaRed > 255) ? 255 : sepiaRed;
            image[i][j].rgbtGreen = (sepiaGreen > 255) ? 255 : sepiaGreen;
            image[i][j].rgbtBlue = (sepiaBlue > 255) ? 255 : sepiaBlue;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            RGBTRIPLE temp = image[i][j];
            image[i][j] = image[i][width - j - 1];
            image[i][width - j - 1] = temp;
        }
    }
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE copy[height][width];

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            copy[i][j] = image[i][j];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int sumRed = 0, sumGreen = 0, sumBlue = 0, count = 0;

            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < height && nj >= 0 && nj < width)
                    {
                        sumRed += copy[ni][nj].rgbtRed;
                        sumGreen += copy[ni][nj].rgbtGreen;
                        sumBlue += copy[ni][nj].rgbtBlue;
                        count++;
                    }
                }
            }

            image[i][j].rgbtRed = round(sumRed / (float)count);
            image[i][j].rgbtGreen = round(sumGreen / (float)count);
            image[i][j].rgbtBlue = round(sumBlue / (float)count);
        }
    }
}

// Detect edges using Sobel operator
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE copy[height][width];

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            copy[i][j] = image[i][j];

    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int Gy[3][3] = {
        {-1, -2, -1},
        {0,  0,  0},
        {1,  2,  1}
    };

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int sumRedX = 0, sumGreenX = 0, sumBlueX = 0;
            int sumRedY = 0, sumGreenY = 0, sumBlueY = 0;

            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di;
                    int nj = j + dj;

                    if (ni >= 0 && ni < height && nj >= 0 && nj < width)
                    {
                        sumRedX += copy[ni][nj].rgbtRed * Gx[di + 1][dj + 1];
                        sumGreenX += copy[ni][nj].rgbtGreen * Gx[di + 1][dj + 1];
                        sumBlueX += copy[ni][nj].rgbtBlue * Gx[di + 1][dj + 1];

                        sumRedY += copy[ni][nj].rgbtRed * Gy[di + 1][dj + 1];
                        sumGreenY += copy[ni][nj].rgbtGreen * Gy[di + 1][dj + 1];
                        sumBlueY += copy[ni][nj].rgbtBlue * Gy[di + 1][dj + 1];
                    }
                }
            }

            int red = round(sqrt(sumRedX * sumRedX + sumRedY * sumRedY));
            int green = round(sqrt(sumGreenX * sumGreenX + sumGreenY * sumGreenY));
            int blue = round(sqrt(sumBlueX * sumBlueX + sumBlueY * sumBlueY));

            image[i][j].rgbtRed = (red > 255) ? 255 : red;
            image[i][j].rgbtGreen = (green > 255) ? 255 : green;
            image[i][j].rgbtBlue = (blue > 255) ? 255 : blue;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: ./filter [filter] infile outfile\n");
        return 1;
    }

    char *filter_type = argv[1];
    char *infile = argv[2];
    char *outfile = argv[3];

    FILE *inptr = fopen(infile, "r");
    if (!inptr)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    FILE *outptr = fopen(outfile, "w");
    if (!outptr)
    {
        fclose(inptr);
        printf("Could not create %s.\n", outfile);
        return 3;
    }

    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;

    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(inptr);
        fclose(outptr);
        printf("Unsupported file format.\n");
        return 4;
    }

    int height = bi.biHeight;
    int width = bi.biWidth;

    RGBTRIPLE image[height][width];

    // Read image pixels
    for (int i = 0; i < height; i++)
    {
        fread(image[i], sizeof(RGBTRIPLE), width, inptr);
        fseek(inptr, (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4, SEEK_CUR); // padding
    }

    // Apply filter
    if (strcmp(filter_type, "grayscale") == 0) grayscale(height, width, image);
    else if (strcmp(filter_type, "sepia") == 0) sepia(height, width, image);
    else if (strcmp(filter_type, "reflect") == 0) reflect(height, width, image);
    else if (strcmp(filter_type, "blur") == 0) blur(height, width, image);
    else if (strcmp(filter_type, "edges") == 0) edges(height, width, image);
    else
    {
        printf("Unknown filter.\n");
        return 5;
    }

    // Write image
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    for (int i = 0; i < height; i++)
    {
        fwrite(image[i], sizeof(RGBTRIPLE), width, outptr);
        for (int k = 0; k < (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4; k++)
            fputc(0x00, outptr); // padding
    }

    fclose(inptr);
    fclose(outptr);

    return 0;
}
