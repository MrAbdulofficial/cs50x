height = 0
while height < 1 or height > 8:
    try:
        height = int(input("height: "))
    except ValueError:
        print("Enter a vadil height")

for i in range(height):
    print(" " * (height - 1 - i), end = "")
    print("#" * (1 + i) + "  " + "#" * (1 + i))
