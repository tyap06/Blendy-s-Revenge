#version 330

// From vertex shader
in vec2 texcoord;
in vec3 vcsPosition;

// Application data
uniform sampler2D sampler0;
uniform sampler2D normal_map;
uniform bool usesNormalMap;
uniform vec3 fcolor;
uniform bool is_blendy;
// Lighting information
uniform vec3 lightPosition;     // Position of the light source
uniform vec3 lightColor;        // Color of the light source
uniform float shininess;        // Shininess parameter for specular reflection
uniform float ambientIntensity; // Strength of ambient lighting

// Output color
layout(location = 0) out  vec4 color;

float lerp(float start, float end, float t) {
    return start * (1-t) + end*t;
}

void main()
{
    vec4 texColor = texture(sampler0, texcoord);
    
    vec3 N;
    if (usesNormalMap) {
        N = normalize(texture(normal_map, texcoord).xyz * 2.0 - 1.0); // Normalize N map
    } else {
        // TODO: This is supposed to cause undefined behavior, but it works somehow???
        N = normalize(texture(normal_map, texcoord).xyz * 2.0 - 1.0); // Normalize N map
    }
        
    // Calculate light direction
    vec3 lightDir = normalize(lightPosition - vcsPosition);

    vec3 eyePosition = vec3(0.0, 0.0, 0.0);

    // Calculate view direction (assuming camera at (0, 0, 0))
    vec3 viewDir = normalize(eyePosition - vcsPosition);

    // Calculate halfway vector
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Calculate diffuse and specular components using Blinn-Phong model
    float diffuseIntensity = max(dot(N, lightDir), 0.0);
    float specularIntensity = pow(max(dot(N, halfwayDir), 0.0), shininess);

    // Combine ambient, diffuse, and specular components with light color
    vec3 ambientColor = lightColor * ambientIntensity;
    vec3 diffuseColor = lightColor * diffuseIntensity;
    vec3 specularColor = lightColor * specularIntensity;
    vec3 finalColor;
    if (is_blendy) {
        vec3 red = vec3(1.0, 0.5, 0.5);
        vec3 blue = vec3(0.5, 0.5, 1.0);
    
        float redInterpolated = lerp(red.x, blue.x, texcoord.x);
        float greenInterpolated = lerp(red.y, blue.y, texcoord.x);
        float blueInterpolated = lerp(red.z, blue.z, texcoord.x);

        vec3 interpolatedColAlongX = vec3(redInterpolated, greenInterpolated, blueInterpolated);

        // Calculate final color
        finalColor =  (ambientColor + (fcolor * texColor.rgb * diffuseColor) + specularColor) * interpolatedColAlongX;
        color = vec4(finalColor, texColor.a);

    } else {
        finalColor =  ambientColor + (fcolor * texColor.rgb * diffuseColor) + specularColor;
        color = vec4(finalColor, texColor.a);
    }
    
    

    //} else {
    //    color = vec4(fcolor + ambientColor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
        //color = vec4(0.0,0.0,0.0,1.0);
    //}
}

