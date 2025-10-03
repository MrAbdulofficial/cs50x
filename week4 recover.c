#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // Ensure proper usage
    if (argc != 2)
    {
        printf("Usage: ./recover IMAGE\n");
        return 1;
    }

    // Open input file
    FILE *inptr = fopen(argv[1], "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", argv[1]);
        return 1;
    }

    // Buffer to store 512 bytes at a time
    BYTE buffer[512];

    // Counter for image files
    int filecount = 0;

    // Output file pointer
    FILE *outptr = NULL;

    // Filename for recovered images
    char filename[8];

    // Read 512 bytes into buffer until end of card
    while (fread(buffer, sizeof(BYTE), 512, inptr) == 512)
    {
        // Check if start of a new JPEG
        if (buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            // If already found a JPEG, close previous one
            if (outptr != NULL)
            {
                fclose(outptr);
            }

            // Create new filename
            sprintf(filename, "%03i.jpg", filecount);
            outptr = fopen(filename, "w");
            filecount++;
        }

        // If we’ve already found a JPEG, keep writing to it
        if (outptr != NULL)
        {
            fwrite(buffer, sizeof(BYTE), 512, outptr);
        }
    }

    // Close last file if open
    if (outptr != NULL)
    {
        fclose(outptr);
    }

    // Close input file
    fclose(inptr);

    return 0;
}
