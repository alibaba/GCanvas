#define TEXTURE_SHADER "TEXTURE"

#define TEXTURE_SHADER_VS "\
attribute vec4 a_position;                  \n\
attribute vec4 a_srcColor;                  \n\
attribute vec2 a_texCoord;                  \n\
uniform mat4 u_modelView;                   \n\
varying vec4 v_desColor;                    \n\
varying vec2 v_texCoord;                    \n\
void main()                                 \n\
{                                           \n\
    gl_Position = u_modelView * a_position; \n\
    v_desColor = a_srcColor;                \n\
    v_texCoord = a_texCoord;                \n\
}"

#define TEXTURE_SHADER_PS "\
precision mediump float;            \n\
varying vec4 v_desColor;            \n\
varying vec2 v_texCoord;            \n\
uniform sampler2D u_texture;        \n\
uniform bool b_premultipliedAlpha;  \n\
void main()                         \n\
{                                   \n\
    vec4 texColor;                  \n\
    if(v_texCoord.x < 0.0 || v_texCoord.x > 1.0 ||  \n\
    v_texCoord.y < 0.0 || v_texCoord.y > 1.0) {     \n\
        texColor = vec4(0.0,0.0,0.0,0.0);           \n\
    }else{                                          \n\
        texColor = texture2D(u_texture, v_texCoord);\n\
    }                                               \n\
    texColor = texColor * v_desColor.a;             \n\
    gl_FragColor = texColor;                        \n\
}"
