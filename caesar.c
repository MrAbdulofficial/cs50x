#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(int argc, string argv[])
{
    // Ensure user provides exactly one command-line argument (the key)
    if (argc != 2)
    {
        printf("Usage: ./caesar key\n");
        return 1;
    }

    // Convert key from string to integer
    int key = atoi(argv[1]);

    // If key is not positive
    if (key < 0)
    {
        printf("Key must be a positive integer.\n");
        return 1;
    }

    // Prompt user for plaintext
    string plaintext = get_string("plaintext: ");

    printf("ciphertext: ");

    // Loop through each character of plaintext
    for (int i = 0; i < strlen(plaintext); i++)
    {
        char c = plaintext[i];

        if (isalpha(c))
        {
            if (isupper(c))
            {
                printf("%c", ((c - 'A' + key) % 26) + 'A');
            }
            // If lowercase
            else if (islower(c))
            {
                printf("%c", ((c - 'a' + key) % 26) + 'a');
            }
        }
        else
        {
            // Non-alphabetic characters stay the same
            printf("%c", c);
        }
    }

    printf("\n");

}
