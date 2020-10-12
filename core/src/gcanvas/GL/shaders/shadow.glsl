#define SHADOW_SHADER "SHADOW"

#define SHADOW_SHADER_VS                   "\
attribute vec4 a_position;                  \n\
attribute vec4 a_srcColor;                  \n\
attribute vec2 a_texCoord;                  \n\
uniform mat4 u_modelView;                   \n\
varying vec2 v_texCoord;                    \n\
varying vec4 v_desColor;                    \n\
void main()                                 \n\
{                                           \n\
   gl_Position = u_modelView * a_position;  \n\
    v_desColor = a_srcColor;                \n\
   v_texCoord = a_texCoord;                 \n\
}"

#define SHADOW_SHADER_PS           "\
precision mediump float;            \n\
uniform vec4 u_shadowColor;         \n\
varying vec2 v_texCoord;            \n\
uniform sampler2D u_texture;        \n\
void main()                         \n\
{                                   \n\
    vec4 texColor = texture2D( u_texture, v_texCoord);    \n\
    float alpha = u_shadowColor.a * texColor.a;            \n\
    gl_FragColor = vec4(u_shadowColor.rgb * texColor.a, alpha); \n\
}"