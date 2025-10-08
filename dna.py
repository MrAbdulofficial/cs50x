import csv
import sys

def main():
    # Check for correct usage
    if len(sys.argv) != 3:
        print("Usage: python dna.py data.csv sequence.txt")
        sys.exit(1)

    # Read database file into a list of dictionaries
    database = []
    with open(sys.argv[1], newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            database.append(row)
    # Get the list of STRs from the first row
    strs = list(database[0].keys())[1:]

    # Read DNA sequence file
    with open(sys.argv[2], 'r') as file:
        dna_sequence = file.read()

    # Compute longest run of each STR in the DNA sequence
    str_counts = {}
    for s in strs:
        str_counts[s] = longest_match(dna_sequence, s)

    # Check database for matching profile
    for person in database:
        match = True
        for s in strs:
            if int(person[s]) != str_counts[s]:
                match = False
                break
        if match:
            print(person["name"])
            return

    print("No match")

# Function to find longest match of a subsequence in a sequence
def longest_match(sequence, subsequence):
    longest_run = 0
    sub_len = len(subsequence)
    seq_len = len(sequence)

    for i in range(seq_len):
        count = 0
        while True:
            start = i + count * sub_len
            end = start + sub_len
            if sequence[start:end] == subsequence:
                count += 1
            else:
                break
        longest_run = max(longest_run, count)
    return longest_run


if __name__ == "__main__":
    main()
