#version 420

const int MAX_POINT_LIGHTS = 4;
const int MAX_SPOT_LIGHTS = 4;

in vec3 f_position;
in vec3 f_normal;

layout(std140) uniform GColors
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emissive;
    vec4 transparency;
    float shininess;
    float intensity;
    float diffuseBlend;
}colors;


struct BaseLight
{
    vec4 color;
    float ambientIntensity;
    float diffuseIntensity;
};
    
struct DirectionalLight 
{
    vec4 direction;
    BaseLight base;
};

struct PointLight 
{
    vec4 position;
    BaseLight base;
};

struct SpotLight 
{
    vec4 direction;
    PointLight point;
};

layout(std140) uniform GLights
{
    DirectionalLight basic;
    PointLight point[1];
    SpotLight spot[1];
    bool toggle[2];

}light;

layout(std140) uniform Eye
{
    vec4 position;
}eye;

uniform sampler2D diffuseTexture;

out vec4 f_out;

vec4 LightBasic(BaseLight source, vec4 direction, vec3 normal)
{
    vec4 ambient = vec4(0, 0, 0, 0); 
    vec4 diffuse = vec4(0, 0, 0, 0);
    vec4 specular = vec4(0, 0, 0, 0);

    ambient = source.color * colors.ambient * source.ambientIntensity; 

    float diffuseFactor = dot(normal, -direction.xyz);
    if (diffuseFactor > 0) 
    {
        diffuse = source.color * colors.diffuse * source.diffuseIntensity * diffuseFactor;
        vec3 v_toEye = normalize(eye.position.xyz - f_position);
        vec3 l_reflect = normalize(reflect(direction.xyz, normal));
        float specularFactor = dot(v_toEye, l_reflect);
        specularFactor = pow(specularFactor, colors.shininess);
        if(specularFactor > 0)
        {
            specular = source.color * colors.specular * colors.intensity * specularFactor;
        }
    }

    return (ambient + diffuse + specular);
}

vec4 LightDir(vec3 normal)
{
    return LightBasic(light.basic.base, light.basic.direction, normal);
}

vec4 LightPt(PointLight pt, vec3 normal)
{
    vec3 dir = f_position - pt.position.xyz;
    float l = length(dir);
    dir = normalize(dir);

    vec4 color = LightBasic(pt.base, vec4(dir, 1.0), normal);

    return color;
}

vec4 LightSpt(SpotLight sp, vec3 normal)
{
    vec3 l_toPix = normalize(f_position - sp.point.position.xyz);
    float spotFactor = dot(l_toPix, sp.direction.xyz);
    if( spotFactor > 0)
    {
        vec4 color = LightPt(sp.point, normal) * spotFactor;
        return vec4(l_toPix, 1.0);//vec4(color.xyz, 1.0);
    }
    else
    {
        return vec4(0, 0, 0, 0);
    }
}

out vec4 colout;

void main(void)
{
    vec3 normal = normalize(f_normal);
    vec4 totalLight = vec4(0, 0, 0, 0);//LightDir(normal);

   // for ( int i = 0; i < 1; i++ )
   //      totalLight += LightPt(light.point[i], normal);

    for ( int i = 0; i < 1; i++ )
         totalLight += LightSpt(light.spot[i], normal);

   // if(light.basic.base.ambientIntensity == 0.1)
    colout = vec4(totalLight.xyz, 1.0);
  //  else 
   // colout = vec4(1.0, 1.0, 1.0, 1.0);
}

