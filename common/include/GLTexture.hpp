#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "GLNode.hpp"

#include <ImageMagick/Magick++.h>

class GLTexture : GLNode
{
 
 public:
    GLTexture();
    GLTexture(const char*, GLenum, const char*);
    ~GLTexture();
    
    bool Load();
    void Bind(GLenum);

protected:
    Magick::Image* image;
    Magick::Blob blob;
    std::string file;

};

#endif 

