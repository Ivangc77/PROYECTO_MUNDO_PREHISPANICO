#version 330 core
in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;
uniform vec3 dayNightTint;  // Color de tinte para día/noche
uniform float dayNightMix;  // Factor de mezcla (0.0 = noche completa, 1.0 = día completo)

void main()
{
    vec4 skyboxColor = texture(skybox, TexCoords);
    
    // Mezclar el color del skybox con el tinte día/noche
    vec3 tintedColor = mix(skyboxColor.rgb * 0.15, skyboxColor.rgb, dayNightMix);
    
    // Aplicar el tinte de color
    tintedColor = tintedColor * dayNightTint;
    
    FragColor = vec4(tintedColor, 1.0);
}