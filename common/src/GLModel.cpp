#include "GLModel.hpp"

#include <QApplication>
#include <QKeyEvent>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/bind.hpp>
#include <csignal>
#include <sstream>
#include <assert.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

//#include "GLPrint.hpp"
#include "GLBufferObject.hpp"
#include "GLUniform.hpp"

const GLuint POSITION_OFFSET = 0;
const GLuint NORMAL_OFFSET = 1;
const GLuint UV_OFFSET = 2;
const GLuint INDEX_OFFSET = 3;

GLModel::GLModel(const char* name, const GLuint attributes) : GLNode(name)
{
    path = "assets/models/";
    this->attributes = attributes;
    this->Allocate();
}

GLModel::GLModel(MODEL type, const char* name, const GLuint attributes) : GLNode(name)
{
    path = "assets/models/";
    this->type = type;
    this->attributes = attributes;
    this->Allocate();
    this->Load(this->toString(type).c_str());
}

GLModel::GLModel(const char* filename, const char* name, const GLuint attributes) : GLNode(name)
{
    path= "./";
    this->attributes = attributes;
    this->Allocate();
    this->Load(filename);
}

GLModel::~GLModel()
{
}

void GLModel::Allocate()
{
    this->positions = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->normals = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->uvs = std::shared_ptr<std::vector<glm::vec2>>(new std::vector<glm::vec2>);
    this->faces = std::shared_ptr<std::vector<std::vector<GLuint>>>(new std::vector<std::vector<GLuint>>);
    this->materials = std::shared_ptr<std::vector<Material>>(new std::vector<Material>);
    path = "";
 
}

bool GLModel::Load(const char* filename)
{
    this->positions->clear();
    this->normals->clear();
    this->uvs->clear();
    this->faces->clear();
    size_t numFaces = 0;

    Assimp::Importer Importer;
     
    // Create the VAO
    glGenVertexArrays(1, &(this->vao));
    glBindVertexArray(this->vao);

    const aiScene* scene = Importer.ReadFile(filename,
                            aiProcess_Triangulate | 
                            aiProcess_GenSmoothNormals | 
                            aiProcess_FlipUVs | 
                            aiProcess_JoinIdenticalVertices);
    if (scene) 
    {
        this->faces->resize(scene->mNumMeshes);
        this->AddMaterials(scene->mMaterials, scene->mNumMaterials);
        for(unsigned int i=0; i<scene->mNumMeshes; i++)
        {
            const aiMesh* mesh = scene->mMeshes[i];
            numFaces += this->AddAttributeData(mesh, i);
        }
        this->CreateBuffers(numFaces);
        
        // Unbind the VAO
        glBindVertexArray(0);
        
        return true;
    }
    else 
    {
        std::cerr << "[E] Could not load " << filename << std::endl;
        std::cerr << Importer.GetErrorString() << std::endl;
    }
    
    return false;
}

size_t GLModel::AddAttributeData(const aiMesh* mesh, unsigned int index)
{
    const aiVector3D zero(0.0f, 0.0f, 0.0f);
    this->mtlIndices.push_back(mesh->mMaterialIndex);

    unsigned int offsetV = this->positions->size();

    this->positions->resize(offsetV + mesh->mNumVertices);
    this->normals->resize(offsetV + mesh->mNumVertices);
    this->uvs->resize(offsetV + mesh->mNumVertices);
    this->vertexOffset.push_back(mesh->mNumVertices);
    this->faces->at(index).resize(mesh->mNumFaces * 3);

    // Populate the vertex attribute vectors
    for (unsigned int i = 0 ; i < mesh->mNumVertices ; i++) 
    {
        const aiVector3D* pos = &(mesh->mVertices[i]);
        const aiVector3D* norm = &(mesh->mNormals[i]);
        const aiVector3D* uv = mesh->HasTextureCoords(0) ?
                               &(mesh->mTextureCoords[0][i]) : &zero;

        this->positions->at(i+offsetV) = glm::vec3(pos->x, pos->y, pos->z);
        this->normals->at(i+offsetV) = glm::vec3(norm->x, norm->y, norm->z);
        this->uvs->at(i+offsetV) = glm::vec2(uv->x, uv->y);
        //std::cout<<this->positions->at(i)<<std::endl;
    }

    // Populate the index buffer
    for (unsigned int i = 0 ; i < mesh->mNumFaces ; i+=3) 
    {
        const aiFace* face = &(mesh->mFaces[i]);
        assert(face->mNumIndices == 3);
        this->faces->at(index).at(i) = face->mIndices[0];
        this->faces->at(index).at(i+1) = face->mIndices[1];
        this->faces->at(index).at(i+2) = face->mIndices[2];
    }

    return mesh->mNumFaces*3;
}

void GLModel::AddMaterials(aiMaterial** materials, unsigned int numMaterials)
{
    this->materials->resize(numMaterials);

    for ( unsigned int i = 0; i < numMaterials; ++i )
    {
        aiMaterial &material = *(materials[i]);

        aiColor3D ambient(0.0f, 0.0f, 0.0f);
        aiColor3D diffuse(0.0f, 0.0f, 0.0f);
        aiColor3D specular(0.0f, 0.0f, 0.0f);

        material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        material.Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        material.Get(AI_MATKEY_COLOR_SPECULAR, specular);
        
        Material mat;
        mat.diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
        mat.ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
        mat.specular = glm::vec3(specular.r, specular.g, specular.b);

        this->materials->at(i) = mat;
    }
}

void GLModel::CreateBuffers(size_t numFaces)
{
    //Create VBOs 
    GLBufferObject vbo_pos("vbopositions",
                       sizeof(glm::vec3),
                       this->vbo[POSITION_OFFSET],
                       this->positions);
    if( this->attributes > 0)
    {
         glEnableVertexAttribArray(0);
         glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    GLBufferObject vbo_norm("vbonormals",
                       sizeof(glm::vec3),
                       this->vbo[NORMAL_OFFSET],
                       this->normals);
    if( this->attributes > 1)
    {
         glEnableVertexAttribArray(1);
         glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    GLBufferObject vbo_uv("vbotextures",
                       sizeof(glm::vec2),
                       this->vbo[UV_OFFSET],
                       this->uvs);
    if( this->attributes > 2)
    {
         glEnableVertexAttribArray(2);
         glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }
 
    GLBufferObject vbo_face("vbofaces",
                       sizeof(GLuint),
                       numFaces,
                       this->vbo[INDEX_OFFSET],
                       GL_ELEMENT_ARRAY_BUFFER,
                       GL_STATIC_DRAW);
    size_t offset = 0;
    for(size_t i=0; i<this->faces->size(); i++)
    {
        glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 
                         (sizeof(GLuint) * offset),
                         (sizeof(GLuint) * this->faces->at(i).size()),
                         this->faces->at(i).data() );
        offset += this->faces->at(i).size();
    }
}


void GLModel::Draw(glm::mat4 vp, std::shared_ptr<GLUniform> vertex_shader, std::shared_ptr<GLUniform> frag_shader)
{
    GLint face_offset = 0;
    GLint vertex_offset = 0;
    glBindVertexArray(this->vao);
    
    //Bind Position Attributes to model
    glEnableVertexAttribArray(V_INDEX);

    glBindBuffer(GL_UNIFORM_BUFFER, vertex_shader->Buffer());
    glBufferSubData( GL_UNIFORM_BUFFER,
                     vertex_shader->Offset(),
                     vertex_shader->Size(),
                     glm::value_ptr( vp * this->matrix ));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, frag_shader->Buffer());

    //Model Fragment Loading
    for(size_t i=0; i< this->faces->size(); i++)
    {   
        std::cout<< "i "<< i<<" size "<<this->faces->at(i).size()<<std::endl;
        glBufferSubData(GL_UNIFORM_BUFFER,
                            frag_shader->Offset(),
                            frag_shader->Size(),
                            glm::value_ptr( glm::normalize(this->materials->at(this->mtlIndices.at(i)).diffuse) ));
     
        glDrawElementsBaseVertex(GL_TRIANGLES, 
                                 this->faces->at(i).size(),
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(GLuint) * face_offset),
                                 vertex_offset);
        
        //glDrawElements(GL_TRIANGLES, this->faces->at(i).size(), GL_UNSIGNED_INT, (void*)(0));
        
        face_offset += this->faces->at(i).size();
        vertex_offset += this->vertexOffset.at(i);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(V_INDEX);

    }

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindVertexArray(0);
}

std::vector<GLuint> GLModel::Faces(size_t index)
{
    return this->faces->at(index);
}

GLuint GLModel::VAO()
{
    return this->vao;
}

void GLModel::setMatrix(glm::mat4 m)
{
    this->matrix = m;
}

glm::mat4 GLModel::Matrix()
{
    return this->matrix;
}

size_t GLModel::TSize() const
{
    return this->faces->size();
}

Material GLModel::GetMaterial(size_t index)
{
    return this->materials->at(index);
}

std::string GLModel::toString(MODEL type)
{
    switch(type)
    {
        case (CUBE):
            return "cube.obj";
            break;
        default:
            return "";
            break;
    }
}


