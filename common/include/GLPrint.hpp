#ifndef GLPRINT_H
#define GLPRINT_H

#include <ostream>
#include <glm/glm.hpp>
#include <iomanip>

std::ostream &operator<< (std::ostream &out, const glm::vec4 &vec);
std::ostream &operator<< (std::ostream &out, const glm::mat4 &mat);

std::ostream &operator<< (std::ostream &out, const glm::vec4 &vec) 
{
    out << "[" 
        << std::setprecision(4) << std::fixed  << std::setw(8) 
        << vec[0] << std::setw(10) << vec[1] << std::setw(10) << vec[2] << std::setw(10) << vec[3] 
        << " ]";
    return out;
}

std::ostream &operator<< (std::ostream &out, const glm::mat4 &mat) 
{

    glm::mat4 tmat = glm::transpose(mat);
    out << "[" 
        << mat[0] << std::endl << " " << tmat[1] << std::endl << " " << tmat[2] << std::endl << " " << tmat[3]
        << "]";
    return out;
}

#endif

