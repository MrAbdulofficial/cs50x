#include <cs50.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// Points assigned to each letter of the alphabet
int POINTS[] = {1, 3, 6, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 7, 1, 1, 9, 4, 4, 8, 4, 10};

// Function prototype
int compute_score(string word);

int main(void)
{
    // Get input words from both players
    string word1 = get_string("Player 1: ");
    string word2 = get_string("Player 2: ");

    // Score both words
    int score1 = compute_score(word1);
    int score2 = compute_score(word2);

    // Print results
    printf("Player 1 Score: %i\n", score1);
    printf("Player 2 Score: %i\n", score2);

    // Determine outcome
    if (score1 > score2)
    {
        printf("Player 1 wins!\n");
        printf("Player 2 loses!\n");
    }
    else if (score2 > score1)
    {
        printf("Player 2 wins!\n");
        printf("Player 1 loses!\n");
    }
    else
    {
        printf("It's a Tie!\n");
    }
}

// Function to compute score of a word
int compute_score(string word)
{
    int score = 0;
    for (int i = 0; i < strlen(word); i++)
    {
        if (isalpha(word[i]))
        {
            char c = toupper(word[i]); // Convert to uppercase
            score += POINTS[c - 'A'];  // Add points
        }
    }
    return score;
}
