#version 330 core

out vec4 FragColor;

in vec3 OUT_NORAML;  
in vec3 OUT_FRAGMENT_POSITION;  
  
uniform vec3 light_pos; 
uniform vec3 view_pos; 
uniform vec4 light_color;
uniform vec4 PHONG_COLOR;

void main()
{
    // ambient
    float ambientStrength = 0.15;
    vec4 ambient = ambientStrength * light_color;
  	
    // diffuse 
    vec3 norm = normalize(OUT_NORAML);
    vec3 lightDir = normalize(light_pos - OUT_FRAGMENT_POSITION);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * light_color;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(view_pos - OUT_FRAGMENT_POSITION);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec4 specular = specularStrength * spec * light_color;  
        
    vec4 result = (ambient + diffuse + specular) * PHONG_COLOR;
    FragColor = vec4(result.rgb, 1.0);
} 