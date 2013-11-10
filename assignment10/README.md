Assignment 10: Lighting
=======================
Lighting! The lighting of the model depends largely on the material properties. Our lights do have colors, the spotlight and point light are red and pink, respectively. Basic light is white. The shader is simple and does not have spotlight cutoff other than back/front, and there is no attenuation.

The lights are loaded as tiered structs into a single UBO. The eye position and toggle are in a separate UBO due to issues with offsetting the buffers. The structs are loaded into the shader light by light, so as to be able to specify the offset resulting from the floats at the tail of the buffer object.

The setup provides for any number (<8) lights, currently the demo includes the 4 required.

Issues:
Textures are broken. Did not have time to debug, but the setup is all there for when it is chased down, right now it is just a diffuse blend, but assimp provides blend values for ambient and specular as well.
The lights do not know how big the model is. The best size will fit nicely into the blender grid, approx. 4x4 - 8x8.

Controls
---------------------
Camera: arrow keys
Zoom: mouse wheel
Increase/decrease diffuse intensity: a/d (extra credit?)
Increase/decrease ambient intensity: a/d
Toggle lights: ambient (1), directional (2), pointlight (3), spotlight (4)

Group Members
--------------------- 
- Jordan Blocher  
- Henry Estela  
- Alexander McArther
