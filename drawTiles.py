#!/usr/bin/env python3
import turtle as T
from random import randint
from math import sqrt
from time import sleep
import canvasvg
import sys

# Tamany de les tiles i pre-càlcul de distància al centre
width = height = 40
center = width * sqrt(2)/2

def drawTile(colors, x, y):
    """
    Dibuixa una tile al tauler
    Es lleig però només son quatre triangles
    """
    # Primer triangle - north
    T.fillcolor(colors[0])
    T.penup()
    T.setpos(x*width, -y*height)
    T.pendown()
    T.setheading(0)
    T.begin_fill()
    T.forward(width)
    T.right(90+45)
    T.forward(center)
    T.right(90)
    T.forward(center)
    T.end_fill()

    # Quart triangle - west
    T.fillcolor(colors[3])
    T.begin_fill()
    T.setheading(-90)
    T.forward(height)
    T.left(90+45)
    T.forward(center)
    T.left(90)
    T.forward(center)
    T.end_fill()

    # Tercer triangle - south
    T.fillcolor(colors[2])
    T.penup()
    T.setpos(x*width, -(y+1)*height)
    T.pendown()
    T.begin_fill()
    T.setheading(0)
    T.forward(width)
    T.left(90+45)
    T.forward(center)
    T.left(90)
    T.forward(center)
    T.end_fill()

    # Segon triangle - east
    T.fillcolor(colors[1])
    T.penup()
    T.setpos((x+1)*width, -(y+1)*height)
    T.pendown()
    T.begin_fill()
    T.setheading(90)
    T.forward(width)
    T.left(90+45)
    T.forward(center)
    T.left(90)
    T.forward(center)
    T.end_fill()
    T.penup()

def get_tiles(filename):
    foo = open(filename, "r")
    tiles = foo.readlines()
    foo.close()
    return tiles

def get_patterns(filename):
    foo = open(filename, "r")
    ts = foo.readlines()
    foo.close()
    return ts


def setupTurtle(width, height):
    T.setup(width=width, height=height, startx=0, starty=0)
    T.setworldcoordinates(0,-T.window_height(),T.window_width(),0)
    T.tracer(0,0)
    T.speed(0)
    T.penup()
    T.pendown()
    T.pencolor('black')

def drawPatterns(palettes, colors, tiles):
    max_width = len(tiles[0])
    for i, palette in enumerate(palettes):
        T.penup()
        T.setpos(width*(i % max_width), -height*(len(tiles)+1+1.5*(i//max_width)))
        T.pendown()
        T.write(i+1)
        drawTile(palette, i % max_width, len(tiles) + 1 + 1.5*(i//max_width))

def drawTiles(tiles, palettes):
    for i, tiles in enumerate(tiles):
        for j, tile in enumerate(tiles):
            drawTile(palettes[tile] if tile > -1 else ["white"]*4, j, i)

def save(filename):
    #T.hideturtle()
    #T.update()
    #T.done()
    canvasvg.warnings(canvasvg.NONE) #remove annoying warning: canvas2svg warning: Items of type 'image' are not supported.
    canvasvg.saveall(filename, T.getscreen().getcanvas())

def main():
    global width
    global height
    global center

    if (len(sys.argv) < 4):
        print("Usage: %s <inputTilesFile> <tilesSolutionFile> {color|bw} <outputSVGFile> [tileSize]" % (sys.argv[0]))
        print("     tileSize is 40 by default")
        return

    colors = []
    if sys.argv[3] == "color":
        colors = ["red","lime","yellow","blue","orange","lightyellow", "lightgray", "gray", "magenta"]
    else:
        colors = ["white","white","silver","dimgray","gray","black"]
        colors.reverse()

    if len(sys.argv) > 5:
        width = int(sys.argv[5])
        height = width
        center = sqrt(2) * width / 2

    raw_ts = get_patterns(sys.argv[1])[1:]
    raw_tiles = get_tiles(sys.argv[2])

    palettes = [[colors[(int(y)-1) % len(colors)] for y in x.split()] for x in raw_ts]
    tiles = [[int(y)-1 for y in x.split()] for x in raw_tiles]

    if tiles == "0":
        print("No solution")
        return

    w_width = width * len(raw_tiles[0].split())
    w_height = height * (len(raw_tiles) + 1)
    setupTurtle(w_width, w_height)

    drawPatterns(palettes, colors, tiles)
    drawTiles(tiles, palettes)

    save(sys.argv[4])
    return 0 #C has done a lot of damage xD

main()
