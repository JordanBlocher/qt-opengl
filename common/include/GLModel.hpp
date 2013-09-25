#ifndef GLMODEL_H
#define GLMODEL_H

#include <vector>
#include <glm/glm.hpp>
#include <cstring>

#include "GLNode.hpp"

enum MODEL {CUBE};

class GLModel : public GLNode
{
 public:
    GLModel(const char*);
    GLModel(MODEL, const char*);
    GLModel(const char*, const char*);
    ~GLModel();

    std::shared_ptr<std::vector<glm::vec3>> Positions();
    std::shared_ptr<std::vector<Material>> Materials();
    glm::mat4 Matrix();
    GLuint VBO(int);
    void setVBO(GLuint, int);
    void setMatrix(glm::mat4);

 private:
    bool Load(const char*);
    void AddVertex(std::vector<std::string>);
    void AddColor(std::vector<std::string>);
    void AddNormal(std::vector<std::string>);
    void AddUV(std::vector<std::string>);
    void AddTriangle(std::vector<std::string>);
    bool AddMaterial(const char*);
    void Clean();
    std::string toString(MODEL);

    std::vector<std::string> source;
    std::string path;
    MODEL type;
    int mtlIndex;

    std::shared_ptr<std::vector<glm::vec3>> positions;
    std::shared_ptr<std::vector<glm::vec3>> normals;
    std::shared_ptr<std::vector<glm::vec2>> uvs;
    std::shared_ptr<std::vector<Triangle>> triangles;
    std::shared_ptr<std::vector<Material>> materials;

    glm::mat4 matrix;
    GLuint vao;
    GLuint vbo[4];


};



#endif 

