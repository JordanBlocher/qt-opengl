#define CHECKGLERROR if ( glGetError() != GL_NO_ERROR ) std::cout << __FILE__ <<":"<< __LINE__ << std::endl;
#include "GLModel.hpp"
#include "GLBufferObject.hpp"
#include "GLUniform.hpp"
#include "GLTexture.hpp"

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


GLModel::GLModel(MODEL type, const char* name, const GLuint attributes) : GLNode(name)
{
    this->filename = this->toString(type);
    this->type = type;
    this->path = "assets/models/";
    this->attributes = attributes;
    this->Allocate();
}

GLModel::GLModel(const char* filename, const char* name, const GLuint attributes) : GLNode(name)
{
    this->filename = filename;
    size_t found = (this->filename).find_last_of("/");
    if(found == std::string::npos)
        this->path = "./";
    else
       this->path = this->filename.substr(0,found);
    this->attributes = attributes;
    this->Allocate();
}

GLModel::~GLModel()
{
}

void GLModel::Allocate()
{
    this->e_size = 0;
    this->v_size = 0;
    this->positions = std::shared_ptr<std::vector<std::vector<glm::vec3>>>(new std::vector<std::vector<glm::vec3>>);
    this->normals = std::shared_ptr<std::vector<std::vector<glm::vec3>>>(new std::vector<std::vector<glm::vec3>>);
    this->uvs = std::shared_ptr<std::vector<std::vector<glm::vec2>>>(new std::vector<std::vector<glm::vec2>>);
    this->faces = std::shared_ptr<std::vector<std::vector<GLuint>>>(new std::vector<std::vector<GLuint>>);
    this->materials = std::shared_ptr<std::vector<Material>>(new std::vector<Material>);
    this->textures = std::shared_ptr<std::vector<GLTexture>>(new std::vector<GLTexture>);
 
}

bool GLModel::CreateVAO()
{
    //Clear
    for(size_t i=0; i<this->positions->size(); i++)
    {
        this->positions->at(i).clear();
        this->normals->at(i).clear();
        this->uvs->at(i).clear();
    }
    for(size_t i=0; i<this->faces->size(); i++)
        this->faces->at(i).clear();

    Assimp::Importer Importer;
     
    //Create the VAO
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
        this->positions->resize(scene->mNumMeshes);
        this->normals->resize(scene->mNumMeshes);
        this->uvs->resize(scene->mNumMeshes);
        this->AddMaterials(scene->mMaterials, scene->mNumMaterials);
        for(unsigned int i=0; i<scene->mNumMeshes; i++)
        {
            const aiMesh* mesh = scene->mMeshes[i];
            this->AddAttributeData(mesh, i);
        }
        this->CreateVBOs();
        
        //Unbind the VAO
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


void GLModel::AddAttributeData(const aiMesh* mesh, unsigned int index)
{
    const aiVector3D zero(0.0f, 0.0f, 0.0f);
    this->mtlIndices.push_back(mesh->mMaterialIndex);
    this->e_size += mesh->mNumFaces * 3;
    this->v_size += mesh->mNumVertices;

    this->positions->at(index).resize(mesh->mNumVertices);
    this->normals->at(index).resize(mesh->mNumVertices);
    this->uvs->at(index).resize(mesh->mNumVertices);
    this->faces->at(index).resize(mesh->mNumFaces * 3);

    // Populate the vertex attribute vectors
    for (unsigned int i = 0 ; i < mesh->mNumVertices ; i++) 
    {
        const aiVector3D* pos = &(mesh->mVertices[i]);
        const aiVector3D* norm = &(mesh->mNormals[i]);
        const aiVector3D* uv = mesh->HasTextureCoords(0) ?
                               &(mesh->mTextureCoords[0][i]) : &zero;

        this->positions->at(index).at(i) = glm::vec3(pos->x, pos->y, pos->z);
        this->normals->at(index).at(i) = glm::vec3(norm->x, norm->y, norm->z);
        this->uvs->at(index).at(i) = glm::vec2(uv->x, uv->y);
    }

    // Populate the index buffer
    for (unsigned int i = 0 ; i < mesh->mNumFaces ; i++) 
    {
        const aiFace* face = &(mesh->mFaces[i]);
        assert(face->mNumIndices == 3);
        this->faces->at(index).at(3*i) = face->mIndices[0];
        this->faces->at(index).at(3*i+1) = face->mIndices[1];
        this->faces->at(index).at(3*i+2) = face->mIndices[2];
    }

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
        aiColor3D emissive(0.0f, 0.0f, 0.0f);
        aiColor3D transparent(0.0f, 0.0f, 0.0f);
        aiString name;

        material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        material.Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        material.Get(AI_MATKEY_COLOR_SPECULAR, specular); 
        material.Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
        material.Get(AI_MATKEY_COLOR_TRANSPARENT, transparent ); 
        material.Get(AI_MATKEY_NAME, name);

        
        Material mat;
        mat.name = name.C_Str();
        mat.diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
        mat.ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
        mat.specular = glm::vec3(specular.r, specular.g, specular.b);
        mat.emissive = glm::vec3(emissive.r, emissive.g, emissive.b);
        mat.transparent = glm::vec3(transparent.r, transparent.g, transparent.b);
        mat.texture = false;
        
        //int numTextures = material.GetTextureCount(aiTextureType_DIFFUSE);

        aiString texPath;
        this->textures->resize(this->textures->size() + 1 + 1);
        if ( material.Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE,0), texPath) == AI_SUCCESS )
        {
            mat.texture = true;

            std::string location;
            if( this->path == "./")
                location = this->path + texPath.data;
            else
                location = this->path + "/" + texPath.data;
            std::cout << "Texture at " << location << std::endl;
            GLTexture texture(name.C_Str(), GL_TEXTURE_2D, location.c_str());

            if (!texture.Load()) 
            {
                printf("Error loading texture '%s'\n", location.c_str());
            }
            this->textures->at(i) = texture;
       }
       else 
       {
            GLTexture white("DefaultTexture", GL_TEXTURE_2D, "./assets/textures/white.png");
            white.Load();
            this->textures->at(i) = white;
       }

        if(std::string(name.C_Str()) != std::string("DefaultMaterial") || numMaterials == 1)
            this->materials->at(i) = mat;
    }
}

void GLModel::CreateVBOs()
{
    //Create VBOs 
    GLBufferObject vbo_pos("vbopositions",
                       sizeof(glm::vec3),
                       this->v_size,
                       GL_ARRAY_BUFFER,
                       GL_STATIC_DRAW);
    size_t offset = 0;
    for(size_t i=0; i<this->positions->size(); i++)
    {
        vbo_pos.LoadSubData(offset, 0, this->positions->at(i));
        offset += this->positions->at(i).size();
    }
    if( this->attributes > V_INDEX)
    {
         glEnableVertexAttribArray(V_INDEX);
         glVertexAttribPointer( V_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
 
    GLBufferObject vbo_norms("vbonormals",
                       sizeof(glm::vec3),
                       this->v_size,
                       GL_ARRAY_BUFFER,
                       GL_STATIC_DRAW);
    offset = 0;
    for(size_t i=0; i<this->positions->size(); i++)
    {
        vbo_norms.LoadSubData(offset, 1, this->normals->at(i));
        offset += this->positions->at(i).size();
    }
    if( this->attributes > NORM_INDEX)
    {
         glEnableVertexAttribArray(NORM_INDEX);
         glVertexAttribPointer( NORM_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    GLBufferObject vbo_uvs("vbotextures",
                       sizeof(glm::vec2),
                       this->v_size,
                       GL_ARRAY_BUFFER,
                       GL_STATIC_DRAW);
    offset = 0;
    for(size_t i=0; i<this->positions->size(); i++)
    {
        vbo_uvs.LoadSubData(offset, 2, this->uvs->at(i));
        offset += this->positions->at(i).size();
    }
    if( this->attributes > UV_INDEX)
    {
         glEnableVertexAttribArray(UV_INDEX);
         glVertexAttribPointer( UV_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }
 
    GLBufferObject vbo_elements("vboelements",
                       sizeof(GLuint),
                       this->e_size,
                       GL_ELEMENT_ARRAY_BUFFER,
                       GL_STATIC_DRAW);
    offset = 0;
    for(size_t i=0; i<this->faces->size(); i++)
    {
        vbo_elements.LoadSubData(offset, 0, this->faces->at(i));
        offset += this->faces->at(i).size();
    }
}


void GLModel::Draw(std::shared_ptr<GLUniform> fragment, GLuint program)
{
    GLint face_offset = 0;
    GLint vertex_offset = 0;
    glBindVertexArray(this->vao);
    Uniform funiform;

    bool texture, color;

    color = (fragment->getId() != UINT_MAX);
    
    if(color)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, fragment->getId());
        funiform = fragment->Get(COLOR);
    }

    //Draw Model 
    for(size_t i=0; i< this->faces->size(); i++)
    {   
        
        texture = this->materials->at(this->mtlIndices.at(i)).texture;
        if ( texture ) 
                this->textures->at(this->mtlIndices.at(i)).Bind(GL_TEXTURE0);
        if( color )
        {
            glBufferSubData(GL_UNIFORM_BUFFER,
                     		funiform.offset,
                     		funiform.size,
                            glm::value_ptr( glm::normalize(this->materials->at(this->mtlIndices.at(i)).diffuse) ));
        }
        
        if( (color && !texture) || (!color && texture) )
        {
        glDrawElementsBaseVertex(GL_TRIANGLES, 
                                 this->faces->at(i).size(),
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(GLuint) * face_offset),
                                 vertex_offset);
        }
        
        face_offset += this->faces->at(i).size();
        vertex_offset += this->positions->at(i).size();
    }
        
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

size_t GLModel::numFaces()
{
    return this->e_size;
}

size_t GLModel::numVertices()
{
    return this->v_size;
}

size_t GLModel::Size()
{
    return this->faces->size();
}

const std::vector<glm::vec3>& GLModel::Positions(size_t index)
{
    return this->positions->at(index);
}

const std::vector<GLuint>& GLModel::Faces(size_t index)
{
    return this->faces->at(index);
}

void GLModel::setMatrix(glm::mat4 m)
{
    this->matrix = m;
}

glm::mat4 GLModel::Matrix()
{
    return this->matrix;
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


