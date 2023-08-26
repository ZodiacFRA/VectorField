
#ifdef GL_ES
    precision highp float;
    precision highp int;
#endif

vec4 v_color;
vec2 v_texCoords;

uniform sampler2D u_texture;

uniform float u_blurSize;
uniform float u_intensity;

void main() {
    vec4 sum = vec4(0);

    // blur in y (vertical)
    // take nine samples, with the distance blurSize between them
    sum += texture2D(u_texture, vec2(v_texCoords.x - 4.0*u_blurSize, v_texCoords.y)) * 0.05;
    sum += texture2D(u_texture, vec2(v_texCoords.x - 3.0*u_blurSize, v_texCoords.y)) * 0.09;
    sum += texture2D(u_texture, vec2(v_texCoords.x - 2.0*u_blurSize, v_texCoords.y)) * 0.12;
    sum += texture2D(u_texture, vec2(v_texCoords.x - u_blurSize, v_texCoords.y)) * 0.15;
    sum += texture2D(u_texture, vec2(v_texCoords.x, v_texCoords.y)) * 0.16;
    sum += texture2D(u_texture, vec2(v_texCoords.x + u_blurSize, v_texCoords.y)) * 0.15;
    sum += texture2D(u_texture, vec2(v_texCoords.x + 2.0*u_blurSize, v_texCoords.y)) * 0.12;
    sum += texture2D(u_texture, vec2(v_texCoords.x + 3.0*u_blurSize, v_texCoords.y)) * 0.09;
    sum += texture2D(u_texture, vec2(v_texCoords.x + 4.0*u_blurSize, v_texCoords.y)) * 0.05;

    // blur in y (vertical)
    // take nine samples, with the distance blurSize between them
    sum += texture2D(u_texture, vec2(v_texCoords.x, v_texCoords.y - 4.0*u_blurSize)) * 0.05;
    sum += texture2D(u_texture, vec2(v_texCoords.x, v_texCoords.y - 3.0*u_blurSize)) * 0.09;
    sum += texture2D(u_texture, vec2(v_texCoords.x, v_texCoords.y - 2.0*u_blurSize)) * 0.12;
    sum += texture2D(u_texture, vec2(v_texCoords.x, v_texCoords.y - u_blurSize)) * 0.15;
    sum += texture2D(u_texture, vec2(v_texCoords.x, v_texCoords.y)) * 0.16;
    sum += texture2D(u_texture, vec2(v_texCoords.x, v_texCoords.y + u_blurSize)) * 0.15;
    sum += texture2D(u_texture, vec2(v_texCoords.x, v_texCoords.y + 2.0*u_blurSize)) * 0.12;
    sum += texture2D(u_texture, vec2(v_texCoords.x, v_texCoords.y + 3.0*u_blurSize)) * 0.09;
    sum += texture2D(u_texture, vec2(v_texCoords.x, v_texCoords.y + 4.0*u_blurSize)) * 0.05;

    gl_FragColor = sum * u_intensity + texture2D(u_texture, v_texCoords);
}