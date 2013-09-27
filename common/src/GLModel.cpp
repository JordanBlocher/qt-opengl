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

//#include "GLPrint.hpp"

std::vector<glm::vec3> v_in;
std::vector<glm::vec2> uv_in;
std::vector<glm::vec3> n_in;

//Author : user763305 (SO)
std::istream& safeGetline(std::istream& is, std::string& t);

GLModel::GLModel(const char* name) : GLNode(name)
{
    this->positions = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->normals = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->uvs = std::shared_ptr<std::vector<glm::vec2>>(new std::vector<glm::vec2>);
    this->triangles = std::shared_ptr<std::vector<std::vector<Triangle>>>(new std::vector<std::vector<Triangle>>);
    this->materials = std::shared_ptr<std::map<std::string, Material>>(new std::map<std::string, Material>);
    path = "assets/models/";
    this->mtlIndex = -1;
}

GLModel::GLModel(MODEL type, const char* name) : GLNode(name)
{
    this->positions = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->normals = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->uvs = std::shared_ptr<std::vector<glm::vec2>>(new std::vector<glm::vec2>);
    this->triangles = std::shared_ptr<std::vector<std::vector<Triangle>>>(new std::vector<std::vector<Triangle>>);
    this->materials = std::shared_ptr<std::map<std::string, Material>>(new std::map<std::string, Material>);
    path = "assets/models/";
    this->type = type;
    this->mtlIndex = -1;
    this->Load(this->toString(type).c_str());
}

GLModel::GLModel(const char* filename, const char* name) : GLNode(name)
{
    this->positions = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->normals = std::shared_ptr<std::vector<glm::vec3>>(new std::vector<glm::vec3>);
    this->uvs = std::shared_ptr<std::vector<glm::vec2>>(new std::vector<glm::vec2>);
    this->triangles = std::shared_ptr<std::vector<std::vector<Triangle>>>(new std::vector<std::vector<Triangle>>);
    this->materials = std::shared_ptr<std::map<std::string, Material>>(new std::map<std::string, Material>);
    path = "";
    this->mtlIndex = -1;
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
        std::string line;
        while( safeGetline(fin, line) )
        {
            boost::trim(line);
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
                this->AddMaterials( (source[1]).c_str());
            }
            else if( source[0] == "usemtl" )
            {
                this->mtlIndex++;
            }

            source.clear();
        }
        this->Clean();
        v_in.clear();
        uv_in.clear();
        n_in.clear();
        fin.close();

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
    glm::vec3 n;
    glm::vec3 uv;
    if( uvtemp.size() >= 3 )
    {
        uv = glm::vec3(uvtemp[0], uvtemp[1], uvtemp[2]);
    }
    if( ntemp.size() >=3 )
    {
        n = glm::vec3(ntemp[0], ntemp[1], ntemp[2]);
    }
    Triangle triangle = {v, uv, n};
    this->triangles->at(mtlIndex).push_back(triangle);

    if( t.size() == 5 ) // Handle quads
    {
        v = glm::vec3(vtemp[2], vtemp[3], vtemp[0]);
        if( uvtemp.size() >= 4 )
        {
            uv = glm::vec3(uvtemp[2], uvtemp[3], uvtemp[0]);
        }
        if( ntemp.size() >=4 )
        {
            n =glm::vec3(ntemp[2], ntemp[3], ntemp[0]);
        }
        Triangle triangle = {v, uv, n};
        this->triangles->at(mtlIndex).push_back(triangle);
    }
    vtemp.clear();
    uvtemp.clear();
    ntemp.clear();
}

bool GLModel::AddMaterials(const char* filename)
{
    std::ifstream fin(path + filename, std::ios::binary | std::ios::in);
    if(fin)
    {
        std::string line;
        Material mtl;
        mtl.name = "";
        std::vector<std::string> mtlsource;
        while( safeGetline(fin, line) )
        {
            boost::trim(line);
            boost::split(mtlsource, line, boost::is_any_of(" "));
            mtlsource.erase( std::remove_if( mtlsource.begin(), mtlsource.end(),                    boost::bind( &std::string::empty, _1 ) ), mtlsource.end() );
            if( mtlsource[0] == "newmtl" )
            {
                 mtl.name = mtlsource[1] == "" ? "default" : mtlsource[1];
                 this->materials->insert(std::pair<std::string, Material>(mtl.name, mtl));
            }
            else if( mtlsource[0] == "Ka" )
            {
                (this->materials->at(mtl.name)).ambient = glm::vec3(std::stof( mtlsource[1] ), std::stof( mtlsource[2] ), std::stof( mtlsource[2] ) );
            }
            else if( mtlsource[0] == "Kd" )
            {
                (this->materials->at(mtl.name)).diffuse = glm::vec3(std::stof( mtlsource[1] ), std::stof( mtlsource[2] ), std::stof( mtlsource[2] ) );
                //std::cout<<"Kd "<<glm::vec4(mtl.diffuse, 0.0)<<std::endl;
            }
            else if( mtlsource[0] == "Ks" )
            {
                (this->materials->at(mtl.name)).specular = glm::vec3(std::stof( mtlsource[1] ), std::stof( mtlsource[2] ), std::stof( mtlsource[2] ) );
            }
            else if( mtlsource[0] == "d")
            {
                (this->materials->at(mtl.name)).transparency = std::stof( mtlsource[1] );
            }
        }
        this->triangles->resize(this->materials->size());
        fin.close();
        return true;
    }

    std::cerr<<"Material file not found. Please include in the same directory as the model. Defaulting to clay."<<std::endl;
    Material clay = {"default", glm::vec3(0.5,0.5,0.5), glm::vec3(0.5,0.5,0.5), glm::vec3(0.5,0.5,0.5), 1.0f };
    this->materials->insert(std::pair<std::string, Material>("default",clay));
    return false;
}

void GLModel::Clean()
{
    unsigned int idxv, idxuv, idxn;
    for(int m=0; m<=mtlIndex; m++)
    {
    for(unsigned int i=0; i<triangles->at(m).size(); i++)
    {   
        for(int j=0; j<3; j++)
        {
            idxv = this->triangles->at(m)[i].vertex[j];
            if( v_in.size() > idxv - 1 )
            {
                this->positions->push_back(v_in[idxv - 1]);
            }
            idxuv = this->triangles->at(m)[i].uv[j];
            if( uv_in.size() > idxuv - 1 )
            {
                this->uvs->push_back(uv_in[idxuv - 1]);
            }
            idxn = this->triangles->at(m)[i].normal[j];
            if( n_in.size() > idxn - 1 )
            {
                this->normals->push_back(n_in[idxn - 1]);
            }
        }   
    }
    }
}


std::shared_ptr<std::vector<glm::vec3>> GLModel::Positions()
{
    return this->positions;
}

std::shared_ptr<std::vector<glm::vec3>> GLModel::Normals()
{
    return this->normals;
}

std::shared_ptr<std::vector<glm::vec2>> GLModel::UVs()
{
    return this->uvs;
}

std::vector<Triangle> GLModel::Triangles(size_t index)
{
    return this->triangles->at(index);
}

std::vector<Material> GLModel::Materials()
{
    typedef std::map<std::string, Material>::iterator it;
    std::vector<Material> dest;
    for(it iterator = this->materials->begin(); iterator != this->materials->end(); iterator++) 
    {
        dest.push_back(iterator->second);
    }
    return dest;
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

GLuint GLModel::VAO(size_t index)
{
    if( index < vao.size())
        return this->vao.at(index);
    std::cerr<<"[E]: VAO not found."<<std::endl;
    return (GLuint)0;
}

void GLModel::addVAO(GLuint bufferId)
{
    this->vao.resize(vao.size() + 1);
    this->vao.push_back(bufferId);
}

size_t GLModel::TSize() const
{
    return this->triangles->size();
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

//Author : user763305 (SO)
std::istream& safeGetline(std::istream& is, std::string& t)
{
    t.clear();

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if(sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case EOF:
            if(t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:
            t += (char)c;
        }
    }
}


