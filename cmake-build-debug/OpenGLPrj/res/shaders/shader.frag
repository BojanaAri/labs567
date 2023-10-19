#version 330 core
out vec4 FragColor;

void main()
{
       // linearly interpolate between both textures (80% container, 20% awesomeface)
       FragColor = vec4( 0.5f, 0.2f, 0.5f, 1.0f);
}
