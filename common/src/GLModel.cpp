#include "GLModel.hpp"

#include <QApplication>
#include <QKeyEvent>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/bind.hpp>
#include <csignal>
#include <sstream>

#include "GLPrint.hpp"

std::vector<glm::vec3> v_in;
std::vector<glm::vec2> uv_in;
std::vector<glm::vec3> n_in;

GLModel::GLModel(const char* name) : GLNode(name)
{
    this->positions = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->normals = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->uvs = std::shared_ptr<std::vector<glm::vec2>>(new std::vector<glm::vec2>);
    this->triangles = std::shared_ptr<std::vector<Triangle>>(new std::vector<Triangle>);
    this->materials = std::shared_ptr<std::vector<Material>>(new std::vector<Material>);
    path = "assets/models/";
    this->mtlIndex = 0;
}

GLModel::GLModel(MODEL type, const char* name) : GLNode(name)
{
    this->positions = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->normals = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->uvs = std::shared_ptr<std::vector<glm::vec2>>(new std::vector<glm::vec2>);
    this->triangles = std::shared_ptr<std::vector<Triangle>>(new std::vector<Triangle>);
    this->materials = std::shared_ptr<std::vector<Material>>(new std::vector<Material>);
    path = "assets/models/";
    this->mtlIndex = 0;
    this->type = type;
    this->Load(this->toString(type).c_str());
}

GLModel::GLModel(const char* filename, const char* name) : GLNode(name)
{
    this->positions = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->normals = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->uvs = std::shared_ptr<std::vector<glm::vec2>>(new std::vector<glm::vec2>);
    this->triangles = std::shared_ptr<std::vector<Triangle>>(new std::vector<Triangle>);
    this->materials = std::shared_ptr<std::vector<Material>>(new std::vector<Material>);
    path = "";
    this->mtlIndex = 0;
    this->Load(filename);
}

GLModel::~GLModel()
{
}

bool GLModel::Load(const char* filename)
{
    std::ifstream fin(path + filename, std::ios::binary | std::ios::in);
    if(fin)
    {
        char line[256];
        while( fin.getline(line, 256) )
        {
            boost::split(source, line, boost::is_any_of(" "));
            source.erase( std::remove_if( source.begin(), source.end(),                    boost::bind( &std::string::empty, _1 ) ), source.end() );
            if( source[0] == "v" )
            {
                this->AddVertex(source);
            }
            else if( source[0] == "vt" )
            {
                this->AddUV(source);
            }
            else if( source[0] == "vn" )
            {
                this->AddNormal(source);
            }
            else if( source[0] == "f" )
            {
                this->AddTriangle(source);
            }
            else if( source[0] == "mtllib" )
            {
                this->AddMaterial( (source[1]).c_str() );
                this->mtlIndex++;
            }
            source.clear();
        }
        this->Clean();
        v_in.clear();
        uv_in.clear();
        n_in.clear();
        return true;
    }
    return false;
}

void GLModel::AddVertex(std::vector<std::string> v)
{
    glm::vec3 vertex( std::stof(v[1]), std::stof(v[2]), std::stof(v[3]) );
    v_in.push_back(vertex);
}

void GLModel::AddUV(std::vector<std::string> t)
{
    glm::vec2 uv( std::stof(t[1]), std::stof(t[2]) );
    uv_in.push_back(uv);
}

void GLModel::AddNormal(std::vector<std::string> n)
{
    glm::vec3 normal( std::stof(n[1]), std::stof(n[2]), std::stof(n[3]) );
    n_in.push_back(normal);
}

void GLModel::AddTriangle(std::vector<std::string> t)
{
    std::vector<std::string> data;
    std::vector<unsigned int> vtemp, uvtemp, ntemp;

    for(unsigned int i=1; i< t.size(); i++)
    {
        std::istringstream sin(t[i]);
        int temp;
        sin >> temp;
        vtemp.push_back(temp);

        if ( sin.tellg() != -1L )  // !"1"
        {
            sin.get();  // read garbage
            if ( (char)sin.peek() == '/' )  // "1//1"
            {
                sin.get();
                sin >> temp;
                ntemp.push_back(temp);
            }
            else
            {
                sin >> temp;
                uvtemp.push_back(temp);
               
                if ( sin.tellg() != -1L )   // "1/1"
                {
                    sin.get();
                    sin >> temp;
                    ntemp.push_back(temp);
                }
            }
        }
    }
    glm::vec3 v(vtemp[0], vtemp[1], vtemp[2]);
    glm::vec3 uv(uvtemp[0], uvtemp[1], uvtemp[2]);
    glm::vec3 n(ntemp[0], ntemp[1], ntemp[2]);
    Triangle triangle = {v, uv, n, this->mtlIndex};
    this->triangles->push_back(triangle);

    if( t.size() == 5 ) // Handle quads
    {
        glm::vec3 v(vtemp[2], vtemp[3], vtemp[0]);
        glm::vec3 uv(uvtemp[2], uvtemp[3], uvtemp[0]);
        glm::vec3 n(ntemp[2], ntemp[3], ntemp[0]);
        Triangle triangle = {v, uv, n, this->mtlIndex};
        this->triangles->push_back(triangle);
    }
    vtemp.clear();
    uvtemp.clear();
    ntemp.clear();
}
//Only one material now
bool GLModel::AddMaterial(const char* filename)
{
    std::ifstream fin(path + filename, std::ios::binary | std::ios::in);
    if(fin)
    {
        char line[256];
        Material mtl;
        std::vector<std::string> mtlsource;
        while( fin.getline(line, 256) )
        {
            boost::split(mtlsource, line, boost::is_any_of(" "));
            mtlsource.erase( std::remove_if( mtlsource.begin(), mtlsource.end(),                    boost::bind( &std::string::empty, _1 ) ), mtlsource.end() );
            if( mtlsource[0] == "newmtl" )
            {
                mtl.name = mtlsource[1];
            }
            else if( mtlsource[0] == "Ka" )
            {
                mtl.ambient = glm::vec3(std::stof( mtlsource[1] ), std::stof( mtlsource[2] ), std::stof( mtlsource[2] ) );
            }
            else if( mtlsource[0] == "Kd" )
            {
                mtl.diffuse = glm::vec3(std::stof( mtlsource[1] ), std::stof( mtlsource[2] ), std::stof( mtlsource[2] ) );
            }
            else if( mtlsource[0] == "Ks" )
            {
                mtl.specular = glm::vec3(std::stof( mtlsource[1] ), std::stof( mtlsource[2] ), std::stof( mtlsource[2] ) );
            }
            else if( mtlsource[0] == "d")
            {
                mtl.transparency = std::stof( mtlsource[1] );
            }
        }
        this->materials->push_back(mtl);
        mtlsource.clear();

        return true;
    }

    std::cerr<<"Material file not found. Please enter the path of the material file:"<<std::endl;
    std::string newFilename;
    std::cin >> newFilename;
    this->AddMaterial(newFilename.c_str());
    return false;
}

void GLModel::Clean()
{
    unsigned int index;
    for(unsigned int i=0; i<triangles->size(); i++)
    {   
        for(int j=0; j<3; j++)
        {
            if( v_in.size() > 0 )
            {
                index = (*this->triangles)[i].vertex[j];
                this->positions->push_back(v_in[index -1]);
            }
            if( uv_in.size() > 0 )
            {
                index = (*this->triangles)[i].uv[j];
                this->uvs->push_back(uv_in[index -1]);
            }
             if( n_in.size() > 0 )
            {
                index = (*this->triangles)[i].normal[j];
                this->normals->push_back(n_in[index -1]);
            }
        }   
    }
}


std::shared_ptr<std::vector<glm::vec3>> GLModel::Positions()
{
    return this->positions;
}

std::shared_ptr<std::vector<Material>> GLModel::Materials()
{
    return this->materials;
}

void GLModel::setMatrix(glm::mat4 m)
{
    this->matrix = m;
}

glm::mat4 GLModel::Matrix()
{
    return this->matrix;
}

void GLModel::setVBO(GLuint bufferId, int index)
{
    this->vbo[index] = bufferId;
}

GLuint GLModel::VBO(int index)
{
    return this->vbo[index];
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
