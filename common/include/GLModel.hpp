#ifndef GLMODEL_H
#define GLMODEL_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>
#include <map>
#include <assimp/scene.h>
#include <climits>

#include "GLNode.hpp"

enum MODEL {CUBE};
const GLuint V_INDEX = 0;
const GLuint NORM_INDEX = 1;
const GLuint UV_INDEX = 2;

class GLUniform;

class GLModel : public GLNode
{
 public:
    GLModel(const char*, const GLuint);
    GLModel(MODEL, const char*, const GLuint);
    GLModel(const char*, const char*, const GLuint);
    ~GLModel();

    bool Load(const char*);
    void Allocate();
    size_t AddAttributeData(const aiMesh*, unsigned int);
    void AddMaterials(aiMaterial**, unsigned int);
    void CreateBuffers(size_t);
    void Draw(glm::mat4, std::shared_ptr<GLUniform>, std::shared_ptr<GLUniform> u = NULL);
    GLuint VAO();

    std::vector<GLuint> Faces(size_t);
    Material GetMaterial(size_t);
    glm::mat4 Matrix();
    void setMatrix(glm::mat4);
    size_t TSize() const;

 private:
    bool AddMaterials(const char*);
    std::string toString(MODEL);

    std::string path;
    MODEL type;

    std::shared_ptr<std::vector<glm::vec3>> positions;
    std::shared_ptr<std::vector<glm::vec3>> normals;
    std::shared_ptr<std::vector<glm::vec2>> uvs;
    std::shared_ptr<std::vector<std::vector<GLuint>>> faces;
    std::shared_ptr<std::vector<Material>> materials;
    std::vector<GLuint> mtlIndices;
    std::vector<GLuint> vertexOffset;

    glm::mat4 matrix;
    GLuint vao;
    GLuint vbo[4];
    int attributes;


};

#endif 

