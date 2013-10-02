Assignment 05
=============

Assignment 5 loads models using the assimp library. Models are loaded with command line arguments. 

There is no work around (right now) for unnamed materials in .mtl files, assimp does not load them. The mtl file is assumed to be in the same directory as the obj, but the program can take any path.

The arrow keys will move the model forward and back and rotate it. Shift + arrow up/down will make the model bigger or smaller.

Color handled by the fragment shader.

There is a large devel branch with neat buffer stuff... not working..
