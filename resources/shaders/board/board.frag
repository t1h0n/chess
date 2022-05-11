#version 330 core

// display from white/black point of view
uniform bool inverse_board;

uniform vec3 white_color;
uniform vec3 black_color;

uniform int side_size;

int map_to_grid_mod2(float current)
{
    return int(mod(int(current) / side_size, 2));
}

void main()
{
    int x_t = map_to_grid_mod2(gl_FragCoord.x);
    int y_t = map_to_grid_mod2(gl_FragCoord.y);
    if (bool(x_t ^ y_t ^ int(inverse_board)))
    {
        gl_FragColor = vec4(black_color, 1.0);
    }
    else
    {
        gl_FragColor = vec4(white_color, 1.0);
    }
}