#include <ostream>
#include <glm/glm.hpp>
#include <iomanip>

using namespace glm;

std::ostream &operator<< (std::ostream &out, const vec4 &vec);
std::ostream &operator<< (std::ostream &out, const mat4 &mat);

std::ostream &operator<< (std::ostream &out, const vec4 &vec) {
    out << "[" 
        << std::setprecision(4) << std::fixed  << std::setw(8) 
        << vec[0] << std::setw(10) << vec[1] << std::setw(10) << vec[2] << std::setw(10) << vec[3] 
        << " ]";

    return out;
}

std::ostream &operator<< (std::ostream &out, const mat4 &mat) {
    
    mat4 tmat = transpose(mat);
    out << "[" 
        << mat[0] << std::endl << " " << tmat[1] << std::endl << " " << tmat[2] << std::endl << " " << tmat[3]
        << "]";
    return out;
}



