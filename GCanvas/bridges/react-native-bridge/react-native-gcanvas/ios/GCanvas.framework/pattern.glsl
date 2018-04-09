#define PATTERN_SHADER "PATTERN"


#define PATTERN_SHADER_VS                    "\
attribute vec4 a_position;                  \n\
attribute vec4 a_srcColor;                  \n\
attribute vec2 a_texCoord;                  \n\
uniform mat4   u_modelView;                 \n\
varying vec4   v_desColor;                  \n\
varying vec2   v_inPos;                     \n\
void main()                                 \n\
{                                           \n\
   gl_Position = u_modelView * a_position;  \n\
   v_desColor = a_srcColor;                 \n\
   v_inPos = a_position.xy;                 \n\
}"


#define PATTERN_SHADER_PS                                                                    "\
precision mediump float;                                                                    \n\
varying vec4      v_desColor;                                                               \n\
varying vec2      v_inPos;               	                                                \n\
uniform sampler2D u_texture;                                                                \n\
uniform bool      repeatX;                                                                  \n\
uniform bool      repeatY;                                                                  \n\
uniform vec2      textureSize;                                                              \n\
void main()                                                                                 \n\
{                                                                                           \n\
   vec4 finalColor = v_desColor;                                                            \n\
   vec2 texcoord = mod(v_inPos, textureSize) / textureSize;                                 \n\
   if ((repeatX && !repeatY && v_inPos.y <= textureSize.y) ||                               \n\
       (!repeatX && repeatY && v_inPos.x <= textureSize.x) ||                               \n\
       (repeatX && repeatY) ||                                                              \n\
       (!repeatX && !repeatY && v_inPos.x <= textureSize.x && v_inPos.y <= textureSize.y))  \n\
   {                                                                                        \n\
   	   finalColor = texture2D(u_texture, texcoord);                                         \n\
   }                                                                                        \n\
   gl_FragColor = finalColor;                                                               \n\
}"






