#version 420

const int MAX_POINT_LIGHTS = 1;
const int MAX_SPOT_LIGHTS = 1;

layout(location=0) in vec3 f_position;
layout(location=1) in vec3 f_normal;

uniform GColors
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
    vec4 ambientIntensity;
    vec4 diffuseIntensity;
};
    
struct DirectionalLight 
{
    vec4 color;
    vec4 ambientIntensity;
    vec4 diffuseIntensity;
    vec4 direction;
};

struct PointLight 
{
    vec4 color;
    vec4 ambientIntensity;
    vec4 diffuseIntensity;
    vec4 position;
    vec4 constant;
    vec4 linear;
    vec4 exp;
};

struct SpotLight 
{
    vec4 color;
    vec4 ambientIntensity;
    vec4 diffuseIntensity;
    vec4 direction;
    vec4 position;
    vec4 constant;
    vec4 linear;
    vec4 exp;
    vec4 cutoff;
};

uniform GLights
{
    DirectionalLight basic;

}light;

uniform Eye
{
    vec4 position;
}eye;

uniform sampler2D diffuseTexture;

out vec4 f_out;

vec4 LightBasic(BaseLight source, vec4 direction, vec4 normal)
{
    vec4 ambient = vec4(0, 0, 0, 0); 
    vec4 diffuse = vec4(0, 0, 0, 0);
    vec4 specular = vec4(0, 0, 0, 0);

    ambient = source.color * source.ambientIntensity.x; 

    float diffuseFactor = dot(normal, -direction);
    if (diffuseFactor > 0) 
    {
        diffuse = source.color * source.diffuseIntensity.x * diffuseFactor;
        vec4 v_toEye = normalize(eye.position - vec4(f_position, 1.0f));
        vec4 l_reflect = normalize(reflect(direction, normal));
        float specularFactor = dot(v_toEye, l_reflect);
        specularFactor = pow(specularFactor, colors.shininess);
        if(specularFactor > 0)
        {
            specular = source.color * colors.intensity * specularFactor;
        }
    }

    return (ambient );//+ diffuse + specular);
}

vec4 LightDir(vec4 normal)
{
    return LightBasic(BaseLight(light.basic.color, light.basic.ambientIntensity, light.basic.diffuseIntensity), light.basic.direction, normal);
}

vec4 LightPt(PointLight pt, vec4 normal)
{
    vec4 dir = vec4(f_position, 1.0f) - pt.position;
    float l = length(dir);
    dir = normalize(dir);

    vec4 color = LightBasic(BaseLight(pt.color, pt.ambientIntensity, pt.diffuseIntensity), dir, normal);
    float attn = pt.constant.x + pt.linear.x * l + pt.exp.x * l * l;
    return color/attn;
}

vec4 LightSpt(SpotLight sp, vec4 normal)
{
    vec4 l_toPix = normalize(vec4(f_position, 1.0f) - sp.position);
    float spotFactor = dot(l_toPix, sp.direction);
    if(spotFactor > sp.cutoff.x)
    {
        vec4 color = LightPt(PointLight(sp.color, sp.ambientIntensity, sp.diffuseIntensity, sp.position, sp.constant, sp.linear, sp.exp), normal);
        return color * (1.0 - (1.0 - spotFactor) * 1.0/(1.0 - sp.cutoff));
    }
    else
    {
        return vec4(0, 0, 0, 0);
    }
}

out vec4 colout;

void main(void)
{
    vec4 normal = vec4(normalize(f_normal), 1.0f);
    vec4 totalLight = LightDir(normal);

    if(light.basic.ambientIntensity.x == 0.5f)
    colout = colors.diffuse * totalLight;
    else
    colout = vec4(0, 0, 1, 0);

}

