# TileSAT
Reduces the Wang tiles problem, aka Wang dominoes, to CNF SAT and solves it with minisat. 

A python script made by Pol Barrachina that can be used to draw the solutions is also included.

## Building

To build you'll need a `C++17` compiler, `make` and the `gzip` library, all of which are readily available for most distros.
Optionally to draw the solutions as SVG images you'll need `Python3`, the library `tkinter` and a graphic enviroment. 

`tkinter` and `canvasvg` are only required if you want to draw the solutions.
`pip3` is the easiest way to install `canvasvg`.

### Installing dependencies on Debian based distros
```
sudo apt install libz-dev minisat python3-pip python3-tk
sudo pip3 install canvasvg
```

### Installing dependencies on Arch based distros
In arch you should get `minisat` from the aur package, but it's currently broken and it doesn't look like it's going to be fixed soon.
You can build it from [this](https://github.com/Udopia/minisat) branch. Remember to add it to your PATH.
 
```
sudo pacman -S zlib python-pip
sudo pip3 install canvasvg
```

### Compiling
From within the TileSAT root directory run
```
make
```

### Running

Solve and print the solution in ASCII
```
build/tileSAT -p input/aperiodic11
```

Solve, draw and open the graphic solution
```
build/tileSAT -d input/aperiodic11
xdg-open output/aperiodic11_*
```

## Usage

Usage: `tileSAT [Options]...  <TilesProblemFile>`
Options:
*    -p      print solution in console
*    -d      drawTiles (requires python3, drawTiles.py, tkinter and canvasvg)
*    -g      don't solve, only generate cnf. Invalidates other options

## Input format
The first line contains 4 parameters: number of tiles (*nTiles*), number of colors (*nColors*), width and height.
The next *nTiles* lines contain the colors of each tile.
Each line contains 4 integers, separated by space, which are the index of the colour of each side of the tile, starting from north/up and rotating clockwise.
Color indices range from 1 to *nColors* (inclusive).

## Limitations/quirks
* The program assumes you have minisat installed in your PATH
* The -d parameter assumes you have python3 in your PATH
* drawTiles.py has to be present in the current directory
* `-g` option creates the gziped cnf file in tmp/out.cnf.gz

## TODO (or not)
* Explore if the SVG can be created without a graphic enviroment
* Only generate the CNF file on a specified path
* Only transform minisat result to ASCII solution
* Integrate minisat to solve without creating a file while avoiding a dependency
