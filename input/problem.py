import sys
from random import randint
"""
Genera tiles diferents de 4 colors, per intentar resoldre el problema de
square tiling.
"""

def rand_tile(colors):
    return [str(randint(1, colors)) for x in range(4)]

def main():
    if len(sys.argv) < 4:
        print("Falten arguments")
        return

    num_tiles = int(sys.argv[1])
    num_colors = int(sys.argv[2])
    width = int(sys.argv[3])
    height = int(sys.argv[4])
    sys.stdout = open("input", "w")

    print(num_tiles, num_colors, width, height)

    tiles = []
    while len(tiles) != num_tiles:
        rtile = rand_tile(num_colors)
        if not rtile in tiles:
            tiles.append(rtile)

    for tile in tiles:
        print(' '.join(tile))

if __name__ == "__main__":
    main()
