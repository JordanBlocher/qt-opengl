#version 420

const int MAX_POINT_LIGHTS = 4;
const int MAX_SPOT_LIGHTS = 4;

layout(location=0) in vec3 f_position;
layout(location=1) in vec3 f_normal;
layout(location=2) in vec2 f_uv;

uniform GColors
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    vec3 transparency;
    float shininess;
    float intensity;
    float diffuseBlend;
}colors;

struct BaseLight
{
    vec3 color;
    float ambientIntensity;
    float diffuseIntensity;
};
    
struct DirectionalLight 
{
    BaseLight basic;
    vec3 direction;
};

struct Attenuation
{
    float constant;
    float linear;
    float exp;
};

struct PointLight 
{
    BaseLight basic;
    Attenuation attn;
    vec3 position;
};

struct SpotLight 
{
    PointLight point;
    vec3 direction;
    float cutoff;
};


uniform GLights
{
    int numPt;
    int numSpt;
    DirectionalLight dir;
    PointLight pt[MAX_POINT_LIGHTS];
    SpotLight spt[MAX_SPOT_LIGHTS];
 
}light;

uniform Eye
{
    vec3 position;
}eye;

uniform sampler2D diffuseTexture;

out vec4 f_out;

vec4 LightBasic(BaseLight emit, vec3 direction, vec3 normal)
{
    vec4 ambient = vec4(0, 0, 0, 0); 
    vec4 diffuse = vec4(0, 0, 0, 0);
    vec4 specular = vec4(0, 0, 0, 0); 

    ambient = vec4(emit.color, 1.0) * emit.ambientIntensity; 

    float diffuseFactor = dot(normalize(f_normal), -direction);
    if (diffuseFactor > 0) 
    {
        diffuse = vec4(emit.color, 1.0) * emit.diffuseIntensity * diffuseFactor;
        vec3 v_toEye = normalize(eye.position - f_position);
        vec3 l_reflect = normalize(reflect(direction, normal));
        float specularFactor = dot(v_toEye, l_reflect);
        specularFactor = pow(specularFactor, colors.intensity);
        if(specularFactor > 0)
        {
            specular = vec4(emit.color, 1.0) * colors.shininess * specularFactor;
        }
    }

    return (ambient + diffuse + specular);
}

vec4 LightDir(vec3 normal)
{
    return LightBasic(light.dir.basic, light.dir.direction, normal);
}

vec4 LightPt(PointLight pt, vec3 normal)
{
    vec3 dir = f_position - pt.position;
    float l = length(dir);
    dir = normalize(dir);

    vec4 color = LightBasic(pt.basic, dir, normal);
    float attn = pt.attn.constant + pt.attn.linear * l + pt.attn.exp * l * l;
    return color / attn;
}

vec4 LightSpt(SpotLight sp, vec3 normal)
{
    vec3 l_toPix = normalize(f_position - sp.point.position);
    float spotFactor = dot(l_toPix, sp.direction);
    if(spotFactor > sp.cutoff)
    {
        vec4 color = LightPt(sp.point, normal);
        return color * (1.0 - (1.0 - spotFactor) * 1.0/(1.0 - sp.cutoff));
    }
    else
    {
        return vec4(0, 0, 0, 0);
    }
}

void main(void)
{
    vec3 normal = normalize(f_normal);
    vec4 totalLight = LightDir(normal);

    for ( int i = 0; i < light.numPt; i++ )
         totalLight += LightPt(light.pt[i], normal);
  //  for ( int i = 0; i < light.numSpt; i++ )
  //       totalLight += LightSpt(light.spt[i], normal);

    f_out =  texture2D(diffuseTexture, f_uv.xy) * totalLight;
}

