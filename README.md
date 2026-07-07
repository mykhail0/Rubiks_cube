# Rubiks_cube

A program for terminal which lets a user play with a `NxNxN` Rubik's cube.

## Introduction

Check [Rubik's cube](https://en.wikipedia.org/wiki/Rubik%27s_Cube) for an
introduction to the puzzle if you're not familiar.

Program works in terminal, it creates a `NxNxN` Rubik's cube in an arranged
state and follows user given instructions on turning the cube's layers, showing
the cube's state if prompted.

## Requirements

Project works for a Linux system with a working gcc compiler, to run tests you
should have [valgrind](https://valgrind.org/). A [Makefile](Makefile) with a
default compilation command is supplied. It also has phony targets clean and
test. If you want to change the `N` constant, set the appropriate option for the
`cube` target's compilation.

## Specification

### Input

By default `N = 5`, this can be changed to a different constant on compilation
using the option `-DN=value`.

Program's input is a sequence of instructions to turn layers and of printing
current cube's state, the sequence ends with a dot. Characters after the dot are
ignored.

Endline is the instruction to print the cube's state.

The instruction to rotate a layer consists of three parts, which show:

- cube's face
- number of layers
- rotation's angle.

A face is determined by a letter:

- u(p)
- l(eft)
- f(ront)
- r(ight)
- b(ack)
- d(own)

If the part describing the number of layers is empty, it means the number of
layers is 1. If not, it's a decimal number signifying a positive number of
layers. This number cannot exceed constant `N` which defines the cube's size.

The part describing the rotation's angle is either empty, a single or a double
quotation mark. Empty means 90 degrees rotation, a single is -90 degrees and a
double quotation mark is 180 degrees.

An instruction to rotate a face F, with a number of layers L and with an angle A
causes a simultaneous rotation of L layers facing the face F, with angle A
clockwise.

Input data follows the following grammar, with a starting symbol Data:

```text
Data → Sequence Dot
Sequence → ε | Instruction Sequence
Instruction → Rotation | Print
Rotation → Face Layers Angle
Print → Endline
Face → Letter
Layers → One | Many
One → ε
Many → Number
Number → Digit | Number Digit
Angle → Right | MinusRight | Straight
Right → ε
MinusRight → SingleQuote
Straight → DoubleQuote
Letter → u | l | f | r | b | d
Digit → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
SingleQuote → '
DoubleQuote → "
Endline → \n
Dot → .
```

In case of wrong inputs the program exits with an error code of 1 and some error
message.

### Output

Program's output is the result of following instructions to print current states
of the cube.

Cube's state is printed using characters of 0, 1, 2, 3, 4, 5 signifying possible
colors of squares on cube's faces. Also space and '|' characters are used.

We assume that an arranged cube has color:

- 0 on the face u
- 1 on the face l
- 2 on the face f
- 3 on the face r
- 4 on the face b
- 5 on the face d

The position of the faces on the printout of the state of the cube is consistent
with the scheme:

```text
  u
l|f|r|b
  d
```

where u, l, f, r, b, d are, respectively, upper, left, frontal, right, back and
down faces.

For a constant `N` defining the size of the cube, printout of the cube's state
consists of `3 x N + 1` rows:

- first row is empty
- `N` following rows describe upper face
- `N` rows after that describe left, frontal, right and back faces
- last `N` rows describe the down face.

Each description row of upper and down faces starts with `N + 1` spaces, after
which there are `N` digits describing the colors of that face's squares.

Description rows of left, frontal, right and back faces have, in order:

- `N` digits describing colors of squares of the left face
- character |,
- `N` digits describing colors of squares of the frontal face
- character |,
- `N` digits describing colors of squares of the right face
- character |,
- `N` digits describing colors of squares of the back face.

On the printout of the state of the cube, the digits specifying the colors of
the squares of each face occupy the area of size `NxN`.

For the left, front, right and back faces, the colors of the squares are
described in the order that we would see after the rotation of the entire cube
by a multiple of the right angle on the up face - down face axis.

For the upper and down faces, the order is as we would see if we rotated the
cube by a multiple of the right angle on the left face - right face axis.

## Examples

In the folder [examples](examples) there are files `.in` with valid input
examples and files `.out` with appropriate outputs for those respective inputs.

Examples assume that `N = 5`.
