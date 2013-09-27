#ifndef GLMODEL_H
#define GLMODEL_H

#include <vector>
#include <glm/glm.hpp>
#include <cstring>
#include <map>

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
    std::shared_ptr<std::vector<glm::vec3>> Normals();
    std::shared_ptr<std::vector<glm::vec2>> UVs();
    std::vector<Triangle> Triangles(size_t);
    std::vector<Material> Materials();
    glm::mat4 Matrix();
    GLuint VBO(int);
    GLuint VAO(size_t);
    void setVBO(GLuint, int);
    void addVAO(GLuint);
    void setMatrix(glm::mat4);
    size_t TSize() const;

 private:
    bool Load(const char*);
    void AddVertex(std::vector<std::string>);
    void AddColor(std::vector<std::string>);
    void AddNormal(std::vector<std::string>);
    void AddUV(std::vector<std::string>);
    void AddTriangle(std::vector<std::string>);
    bool AddMaterials(const char*);
    void Clean();
    std::string toString(MODEL);

    std::vector<std::string> source;
    std::string path;
    MODEL type;

    std::shared_ptr<std::vector<glm::vec3>> positions;
    std::shared_ptr<std::vector<glm::vec3>> normals;
    std::shared_ptr<std::vector<glm::vec2>> uvs;
    std::shared_ptr<std::vector<std::vector<Triangle>>> triangles;
    std::shared_ptr<std::map<std::string, Material>> materials;
    int mtlIndex;

    glm::mat4 matrix;
    std::vector<GLuint> vao;
    GLuint vbo[4];


};

#endif 

