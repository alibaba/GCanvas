#define LINEAR_SHADER "LINEAR"



#define LINEAR_SHADER_VS                    "\
attribute vec4 a_position;                  \n\
attribute vec4 a_srcColor;                  \n\
attribute vec2 a_texCoord;                  \n\
uniform mat4   u_modelView;                 \n\
varying vec2   v_inPos;                     \n\
varying vec4   v_desColor;                  \n\
varying vec2   v_texCoord;                  \n\
void main()                                 \n\
{                                           \n\
   gl_Position = u_modelView * a_position;  \n\
   v_inPos = a_position.xy;                 \n\
   v_desColor = a_srcColor;                 \n\
   v_texCoord = a_texCoord;                 \n\
}"


#define LINEAR_SHADER_PS                                       "\
precision highp float;                                         \n\
precision highp  int;                                          \n\
varying vec2      v_inPos;                                     \n\
varying vec2      v_texCoord;                                  \n\
varying vec4      v_desColor;                                  \n\
uniform sampler2D u_texture;                                   \n\
uniform bool      b_hasTexture;                                \n\
uniform vec2      u_startPos;                                  \n\
uniform vec2      u_endPos;                                    \n\
uniform int       u_stopCount;                                 \n\
uniform vec4      u_stop0;                                     \n\
uniform vec4      u_stop1;                                     \n\
uniform vec4      u_stop2;                                     \n\
uniform vec4      u_stop3;                                     \n\
uniform vec4      u_stop4;                                     \n\
uniform float      u_colorStop0;                               \n\
uniform float      u_colorStop1;                               \n\
uniform float      u_colorStop3;                               \n\
uniform float      u_colorStop2;                               \n\
uniform float      u_colorStop4;                               \n\
void main()                                                                                \n\
{                                                                                          \n\
   vec4 finalColor = vec4(1.0, 1.0, 1.0, 1.0);                                             \n\
   vec2 vecA = u_startPos - v_inPos;                                                       \n\
   vec2 vecB = u_endPos - u_startPos;                                                      \n\
   float t = -dot(vecA, vecB) / dot(vecB, vecB);                                           \n\
   if (u_stopCount == 1) {                                                                 \n\
        finalColor = u_stop0;                                                              \n\
   };                                                                                      \n\
   if (u_stopCount > 1) {                                                                  \n\
   		finalColor = mix(u_stop0, u_stop1, smoothstep(u_colorStop0, u_colorStop1, t));     \n\
   }                                                                                       \n\
   if (u_stopCount > 2) {                                                                  \n\
   		finalColor = mix(finalColor, u_stop2, smoothstep(u_colorStop1, u_colorStop2, t));  \n\
   }                                                                                       \n\
   if (u_stopCount > 3) {                                                                  \n\
   		finalColor = mix(finalColor, u_stop3, smoothstep(u_colorStop2, u_colorStop3, t));  \n\
   }                                                                                       \n\
   if (u_stopCount > 4) {                                                                  \n\
   		finalColor = mix(finalColor, u_stop4, smoothstep(u_colorStop3, u_colorStop4, t));  \n\
   }                                                                                       \n\
   if (b_hasTexture) {                                                        \n\
        vec4 marsk = texture2D(u_texture, v_texCoord);                        \n\
        gl_FragColor = vec4(finalColor.xyz, finalColor.w * marsk.w);          \n\
   } else {                                                                   \n\
   		gl_FragColor = vec4(finalColor.xyzw); 						          \n\
   }                                                                          \n\
}"







