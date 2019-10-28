#define RADIAL_SHADER "RADIAL"



#define RADIAL_SHADER_VS                    "\
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

#define RADIAL_SHADER_PS                                        "\
precision highp float;                                         \n\
precision highp  int;                                          \n\
varying vec2      v_inPos;                                     \n\
varying vec4      v_desColor;                                  \n\
varying vec2      v_texCoord;                                  \n\
uniform sampler2D u_texture;                                   \n\
uniform bool      b_hasTexture;                                \n\
uniform vec3      u_startPos;                                  \n\
uniform vec3      u_endPos;                                    \n\
uniform int       u_stopCount;                                 \n\
uniform vec4      u_stopColor0;                                \n\
uniform vec4      u_stopColor1;                                \n\
uniform vec4      u_stopColor2;                                \n\
uniform vec4      u_stopColor3;                                \n\
uniform vec4      u_stopColor4;                                \n\
uniform float     u_stop0;                                     \n\
uniform float     u_stop1;                                     \n\
uniform float     u_stop2;                                     \n\
uniform float     u_stop3;                                     \n\
uniform float     u_stop4;                                     \n\
void main()                                                                   \n\
{                                                                             \n\
   vec4 finalColor = vec4(1.0, 1.0, 1.0, 1.0);                                \n\
   vec2 x_c0  = v_inPos - u_startPos.xy;                                      \n\
   vec2 c1_c0 = u_endPos.xy - u_startPos.xy;                                  \n\
   float t0 = dot(c1_c0, c1_c0) - u_endPos.z * u_endPos.z;                    \n\
   float t1 = dot(x_c0, c1_c0) + u_startPos.z * u_endPos.z;                   \n\
   float t2 = dot(x_c0, x_c0) - u_startPos.z * u_startPos.z;                  \n\
   float t3 = t1 * t1 - t0 * t2;                                              \n\
   float t4 = sqrt(t3) / t0;                                                  \n\
   float t5 = t1 / t0;                                                        \n\
   float t  = max(t4 + t5, t5 - t4);                                          \n\
   if (u_startPos.z + t * u_endPos.z < 0.0) {                                 \n\
   		gl_FragColor = finalColor;                                          \n\
   }                                                                        \n\
   else{                                                                    \n\
        if (u_stopCount >= 1 && t < u_stop0) {                              \n\
   		    finalColor = u_stopColor0;                                      \n\
        }                                                                   \n\
        else if (u_stopCount >= 2 && u_stop0 <= t && t <= u_stop1) {        \n\
   		    float w0 = t - u_stop0;                                         \n\
   		    float w1 = u_stop1 - t;                                         \n\
   		    float w  = w0 + w1;                                             \n\
   		    finalColor = u_stopColor0 * w1 / w + u_stopColor1 * w0 / w;     \n\
        }                                                                   \n\
        else if (u_stopCount == 2 && u_stop1 < t) {                         \n\
   	        finalColor = u_stopColor1;                                      \n\
        }                                                                   \n\
        else if (u_stopCount >= 3 && u_stop1 <= t && t <= u_stop2) {        \n\
   		    float w0 = t - u_stop1;                                         \n\
   		    float w1 = u_stop2 - t;                                         \n\
   		    float w  = w0 + w1;                                             \n\
   		    finalColor = u_stopColor1 * w1 / w + u_stopColor2 * w0 / w;     \n\
        }                                                                   \n\
        else if (u_stopCount == 3 && u_stop2 < t) {                         \n\
   		    finalColor = u_stopColor2;                                      \n\
        }                                                                   \n\
        else if (u_stopCount >= 4 && u_stop2 <= t && t <= u_stop3) {        \n\
   		    float w0 = t - u_stop2;                                         \n\
   		    float w1 = u_stop3 - t;                                         \n\
   		    float w  = w0 + w1;                                             \n\
   		    finalColor = u_stopColor2 * w1 / w + u_stopColor3 * w0 / w;     \n\
        }                                                                   \n\
        else if (u_stopCount == 4 && u_stop3 < t) {                         \n\
   		    finalColor = u_stopColor3;                                      \n\
        }									                                \n\
        else if (u_stopCount >= 5 && u_stop3 <= t && t <= u_stop4) {        \n\
   		    float w0 = t - u_stop3;                                         \n\
   		    float w1 = u_stop4 - t;                                         \n\
   		    float w  = w0 + w1;                                             \n\
   		    finalColor = u_stopColor3 * w1 / w + u_stopColor4 * w0 / w;     \n\
        }                                                                   \n\
        else if (u_stopCount == 5 && u_stop4 < t) {                         \n\
   		    finalColor = u_stopColor4;                                      \n\
        }									                                \n\
        if (b_hasTexture) {                                                 \n\
            vec4 marsk = texture2D(u_texture, v_texCoord);                  \n\
            gl_FragColor = vec4(finalColor.rgb, marsk.a);                   \n\
        }                                                                   \n\
        else {                                                              \n\
   		    gl_FragColor = finalColor; 			                            \n\
        }                                                                   \n\
    }                                                                       \n\
}"





