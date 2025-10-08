#include <stdio.h>
#include <cs50.h>
#include <math.h>

int main(void)
{
    long number;
    do
    {
        number = get_long("Number: ");
    }
    while (number < 0);

    int sum = 0;
    long temp = number;
    int digit_count = 0;

    while (temp > 0)
    {
        int digit = temp % 10;
        if (digit_count % 2 == 1)
        {
            digit *= 2;
            if (digit > 9)
            {
                digit = (digit % 10) + 1;
            }
        }
        sum += digit;
        temp /= 10;
        digit_count++;
    }

    if (sum % 10 != 0)
    {
        printf("INVALID\n");
        return 0;
    }

    long start_digits = number;
    while (start_digits >= 100)
    {
        start_digits /= 10;
    }

    if ((start_digits == 34 || start_digits == 37) && digit_count == 15)
    {
        printf("AMEX\n");
    }
    else if (start_digits >= 51 && start_digits <= 55 && digit_count == 16)
    {
        printf("MASTERCARD\n");
    }
    else if ((start_digits / 10 == 4) && (digit_count == 13 || digit_count == 16))
    {
        printf("VISA\n");
    }
    else
    {
        printf("INVALID\n");
    }

}
