#define SHADOW_SHADER "SHADOW"

#define SHADOW_SHADER_VS "\
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

#define SHADOW_SHADER_PS "\
precision mediump float;                    \n\
varying vec4 v_desColor;                    \n\
varying vec2 v_texCoord;                    \n\
uniform sampler2D image;                    \n\
uniform float u_xDelta;                     \n\
uniform float u_yDelta;                     \n\
uniform float u_weight[13];                 \n\
const int sample=12;                        \n\
void main(void)                             \n\
{                                           \n\
  vec2 offset=vec2(0,0);                    \n\
  vec4 color=v_desColor;                    \n\
  color.a = texture2D( image, v_texCoord ).a *u_weight[0];                  \n\
  for (int i=1; i<=sample; i++) {           \n\
    offset.x+=u_xDelta;                     \n\
    offset.y+=u_yDelta;                     \n\
    color.a += texture2D( image, ( v_texCoord+offset ) ).a *u_weight[i];    \n\
    color.a += texture2D( image, ( v_texCoord-offset ) ).a *u_weight[i];    \n\
  }                                         \n\
  color.rgb *= color.a;                     \n\
  gl_FragColor = color;                     \n\
}"
