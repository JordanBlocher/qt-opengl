#ifndef GLMODEL_H
#define GLMODEL_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>
#include <map>
#include <assimp/scene.h>
#include <climits>
#include <ImageMagick/Magick++.h>

#include "GLNode.hpp"

enum MODEL {CUBE};
const GLuint V_INDEX = 0;
const GLuint NORM_INDEX = 1;
const GLuint UV_INDEX = 2;

class GLUniform;
class GLTexture;

class GLModel : public GLNode
{
 public:
    GLModel(MODEL, const char*, const GLuint);
    GLModel(const char*, const char*, const GLuint);
    ~GLModel();

    bool CreateVAO();
    glm::mat4 Matrix();
    void setMatrix(glm::mat4);

    void Draw(std::shared_ptr<GLUniform>, GLuint);

 private:
    void Allocate();
    void AddAttributeData(const aiMesh*, unsigned int);
    void AddMaterials(aiMaterial**, unsigned int);
    void CreateVBOs();

    bool AddMaterials(const char*);
    std::string toString(MODEL);

    std::string path;
    std::string filename;
    MODEL type;

    std::shared_ptr<std::vector<std::vector<glm::vec3>>> positions;
    std::shared_ptr<std::vector<std::vector<glm::vec3>>> normals;
    std::shared_ptr<std::vector<std::vector<glm::vec2>>> uvs;
    std::shared_ptr<std::vector<std::vector<GLuint>>> faces;
    std::shared_ptr<std::vector<Material>> materials;
    std::shared_ptr<std::vector<GLTexture>> textures;
    std::vector<GLuint> mtlIndices;
    size_t e_size;
    size_t v_size;

    glm::mat4 matrix;
    GLuint vao;
    GLuint vbo[4];
    GLuint attributes;

};

#endif 

