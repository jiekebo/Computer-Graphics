Here is a few basic steps for your first exercise!

  1) Install CMake, if you do not already got it

  2) Use CMake to generate your make/solution files

  3) Compile and link

  4) Confirm that you got a ''blue'' screen.

Use the teaching assitant to get a good feeling about how the framework works. 

When class is over you should know:

  a) What is the purpose of every file in the folder: src/graphics/*
  b) What is the purpose of every file in the folder: src/solution/*
  c) What is going on in the file: main.cpp
  d) How the render pipeline works. Explain every arrow and box in the file ''pipeline_overview.png''

Enjoy

  Knud, Kenny


P.S:

If running on DIKUs system and all fails then try this


 cd project/src
 gcc -I../src -lglut -lGL -lGLU main.cpp
 ./a.out

