#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Function to check if the key is valid
bool valid_key(string key);

int main(int argc, string argv[])
{
    // Check command-line argument count
    if (argc != 2)
    {
        printf("Usage: ./substitution key\n");
        return 1;
    }

    string key = argv[1];

    // Validate key
    if (!valid_key(key))
    {
        printf("Key must contain 26 unique alphabetic characters.\n");
        return 1;
    }

    // Get plaintext from user
    string plaintext = get_string("plaintext: ");

    // Prepare ciphertext
    printf("ciphertext: ");

    for (int i = 0, n = strlen(plaintext); i < n; i++)
    {
        char c = plaintext[i];

        if (isupper(c))  // Uppercase letters
        {
            int index = c - 'A';            // Find alphabetical index (0–25)
            char sub = toupper(key[index]); // Substitute from key, keep uppercase
            printf("%c", sub);
        }
        else if (islower(c))  // Lowercase letters
        {
            int index = c - 'a';
            char sub = tolower(key[index]); // Substitute from key, keep lowercase
            printf("%c", sub);
        }
        else
        {
            printf("%c", c);  // Leave non-alphabetic characters unchanged
        }
    }

    printf("\n");
    return 0;
}

// Function to validate key
bool valid_key(string key)
{
    // Must be 26 characters
    if (strlen(key) != 26)
    {
        return false;
    }

    // Track seen letters
    bool seen[26] = {false};

    for (int i = 0; i < 26; i++)
    {
        if (!isalpha(key[i]))
        {
            return false;  // Must be alphabetic only
        }

        int index = toupper(key[i]) - 'A';
        if (seen[index])
        {
            return false;  // Duplicate found
        }
        seen[index] = true;
    }
    return true;
}
