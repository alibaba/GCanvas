#define DEFAULT_SHADER "DEFAULT"

#define DEFAULT_SHADER_VS "\
attribute vec4 a_position;                  \n\
attribute vec4 a_srcColor;                  \n\
attribute vec2 a_texCoord;                  \n\
uniform mat4 u_modelView;                   \n\
varying vec4 v_desColor;                    \n\
varying vec2 v_texCoord;                    \n\
void main()                                 \n\
{                                           \n\
   gl_Position = u_modelView * a_position;  \n\
   v_desColor = a_srcColor;                 \n\
   v_texCoord = a_texCoord;                 \n\
}"

#define DEFAULT_SHADER_PS "\
precision mediump float;            \n\
varying vec4 v_desColor;            \n\
varying vec2 v_texCoord;            \n\
uniform sampler2D u_texture;        \n\
uniform bool b_hasTexture;          \n\
uniform bool b_overrideTextureColor;     \n\
uniform vec4 v_textureColor;        \n\
void main()                         \n\
{                                   \n\
   vec4 texColor;                   \n\
   if (b_hasTexture) {                                              \n\
       texColor = texture2D( u_texture, v_texCoord);                \n\
       if (b_overrideTextureColor) {                                     \n\
           gl_FragColor = vec4(v_desColor.rgb, texColor.a);     \n\
       } else {                                                     \n\
           gl_FragColor = vec4(texColor.rgb, v_desColor.a * texColor.a);     \n\
       }                                \n\
   } else {                             \n\
       gl_FragColor = v_desColor;       \n\
   }                                    \n\
}"
