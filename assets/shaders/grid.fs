#version 330 core
out vec4 FragColor;
in vec3 worldPos;

uniform float gridSize = 1.0;
uniform vec3 gridColor = vec3(0.6, 0.6, 0.6);
uniform vec3 lineColor = vec3(0.1, 0.1, 0.1);

void main()
{
    // Scale the world position by grid size
    vec2 pos = worldPos.xz / gridSize;

    // Find fractional part to detect lines
    vec2 grid = abs(fract(pos - 0.5) - 0.5) / fwidth(pos);

    float line = min(grid.x, grid.y); // nearest line
    float alpha = 1.0 - min(line, 1.0);

    FragColor = vec4(mix(gridColor, lineColor, alpha), 1.0);
}
