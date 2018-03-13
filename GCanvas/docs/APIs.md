### GCanvas API Support Summary


#### 1.Term Explanation
* Implemented:  API is implemented .
* **Undefined**: API is not defined.
* *Empty*: API is defined but not implemented.

#### 2.API support summary for canvas

1. Context APIs

| API Name                 | API Type                | Status        |
| ------------------------ | ----------------------- | ------------- |
| fillStyle                | Attribute getter/setter | Implemented   |
| strokeStyle              | Attribute getter/setter | Implemented   |
| shadowColor              | Attribute getter/setter | **Undefined** |
| shadowBlur               | Attribute getter/setter | **Undefined** |
| shadowOffsetX            | Attribute getter/setter | **Undefined** |
| shadowOffsetY            | Attribute getter/setter | **Undefined** |
| createLinearGradient()   | Method                  | Implemented   |
| createPattern()          | Method                  | Implemented   |
| createRadialGradient()   | Method                  | Implemented   |
| addColorStop()           | Method                  | Implemented   |
| isPointInPath()          | Method                  | *Empty*       |
| createEvent()            | Method                  | **Undefined** |
| toDataURL()              | Method                  | **Undefined** |
| lineCap                  | Attribute getter/setter | Implemented   |
| lineJoin                 | Attribute getter/setter | Implemented   |
| lineWidth                | Attribute getter/setter | Implemented   |
| miterLimit               | Attribute getter/setter | Implemented   |
| font                     | Attribute getter/setter | Implemented   |
| textAlign                | Attribute getter/setter | Implemented   |
| textBaseline             | Attribute getter/setter | Implemented   |
| globalAlpha              | Attribute getter/setter | Implemented   |
| globalCompositeOperation | Attribute getter/setter | Implemented   |
| rect()                   | Method                  | Implemented   |
| fillRect()               | Method                  | Implemented   |
| strokeRect()             | Method                  | Implemented   |
| clearRect()              | Method                  | Implemented   |
| fill()                   | Method                  | Implemented   |
| stroke()                 | Method                  | Implemented   |
| beginPath()              | Method                  | Implemented   |
| moveTo()                 | Method                  | Implemented   |
| closePath()              | Method                  | Implemented   |
| lineTo()                 | Method                  | Implemented   |
| clip()                   | Method                  | Implemented   |
| quadraticCurveTo()       | Method                  | Implemented   |
| bezierCurveTo()          | Method                  | Implemented   |
| arc()                    | Method                  | Implemented   |
| arcTo()                  | Method                  | Implemented   |
| scale()                  | Method                  | Implemented   |
| rotate()                 | Method                  | Implemented   |
| translate()              | Method                  | Implemented   |
| transform()              | Method                  | Implemented   |
| setTransform()           | Method                  | Implemented   |
| fillText()               | Method                  | Implemented   |
| strokeText()             | Method                  | Implemented   |
| measureText()            | Method                  | *Empty*       |
| drawImage()              | Method                  | Implemented   |
| createImageData()        | Method                  | Implemented   |
| getImageData()           | Method                  | Implemented   |
| putImageData()           | Method                  | Implemented   |
| save()                   | Method                  | Implemented   |
| restore()                | Method                  | Implemented   |
| getContext()             | Method                  | Implemented   |
| loadTexture()            | Method                  | Implemented   |
| unloadTexture()          | Method                  | Implemented   |
| resetTransform()         | Method                  | Implemented   |
| render()                 | Method                  | Implemented   |
| capture()                | Method                  | *Empty*       |
| resetClip()              | Method                  | Implemented   |

2. ImageData APIs

| API Name | API Type                | Status      |
| -------- | ----------------------- | ----------- |
| width    | Attribute getter/setter | Implemented |
| height   | Attribute getter/setter | Implemented |
| data     | Attribute getter/setter | Undefined   |
| src      | Attribute getter/setter | Implemented |
| complete | Attribute getter/setter | Implemented |

#### 3.API support summary for WebGL
1.Context APIs

| API Name                                | API Type | Status        |
| --------------------------------------- | -------- | ------------- |
| viewport()                              | Method   | Implemented   |
| vertexAttribPointer()                   | Method   | Implemented   |
| vertexAttrib2fv()                       | Method   | Implemented   |
| useProgram()                            | Method   | Implemented   |
| uniformMatrix4fv()                      | Method   | Implemented   |
| uniformMatrix3fv()                      | Method   | Implemented   |
| uniformMatrix2fv()                      | Method   | Implemented   |
| uniform4iv()                            | Method   | Implemented   |
| uniform4i()                             | Method   | Implemented   |
| uniform4fv()                            | Method   | Implemented   |
| uniform4f()                             | Method   | Implemented   |
| uniform3iv()                            | Method   | Implemented   |
| uniform3i()                             | Method   | Implemented   |
| uniform3fv()                            | Method   | Implemented   |
| uniform3f()                             | Method   | Implemented   |
| uniform2iv()                            | Method   | Implemented   |
| uniform2i()                             | Method   | Implemented   |
| uniform2fv()                            | Method   | Implemented   |
| uniform2f()                             | Method   | Implemented   |
| uniform1iv()                            | Method   | Implemented   |
| uniform1i()                             | Method   | Implemented   |
| uniform1fv()                            | Method   | Implemented   |
| uniform1f()                             | Method   | Implemented   |
| texParameteri()                         | Method   | Implemented   |
| texImage2D()                            | Method   | Implemented   |
| shaderSource()                          | Method   | Implemented   |
| scissor()                               | Method   | Implemented   |
| renderbufferStorage()                   | Method   | Implemented   |
| pixelStorei()                           | Method   | Implemented   |
| linkProgram()                           | Method   | Implemented   |
| lineWidth()                             | Method   | Implemented   |
| getUniformLocation()                    | Method   | Implemented   |
| getShaderParameter()                    | Method   | Implemented   |
| getAttribLocation()                     | Method   | Implemented   |
| generateMipmap()                        | Method   | Implemented   |
| frontFace()                             | Method   | Implemented   |
| framebufferTexture2D()                  | Method   | Implemented   |
| flush()                                 | Method   | Implemented   |
| enableVertexAttribArray()               | Method   | Implemented   |
| enable()                                | Method   | Implemented   |
| drawElements()                          | Method   | Implemented   |
| disableVertexAttribArray()              | Method   | Implemented   |
| disable()                               | Method   | Implemented   |
| depthMask()                             | Method   | Implemented   |
| depthFunc()                             | Method   | Implemented   |
| deleteTexture()                         | Method   | Implemented   |
| deleteShader()                          | Method   | Implemented   |
| deleteRenderbuffer()                    | Method   | Implemented   |
| deleteProgram()                         | Method   | Implemented   |
| deleteFramebuffer()                     | Method   | Implemented   |
| deleteBuffer()                          | Method   | Implemented   |
| cullFace()                              | Method   | Implemented   |
| createTexture()                         | Method   | Implemented   |
| createShader()                          | Method   | Implemented   |
| createRenderbuffer()                    | Method   | Implemented   |
| createProgram()                         | Method   | Implemented   |
| createFramebuffer()                     | Method   | Implemented   |
| createBuffer()                          | Method   | Implemented   |
| compileShader()                         | Method   | Implemented   |
| colorMask()                             | Method   | Implemented   |
| clearStencil()                          | Method   | Implemented   |
| clearDepth()                            | Method   | Implemented   |
| clearColor()                            | Method   | Implemented   |
| clear()                                 | Method   | Implemented   |
| bufferData()                            | Method   | Implemented   |
| blendFuncSeparate()                     | Method   | Implemented   |
| blendFunc()                             | Method   | Implemented   |
| blendEquationSeparate()                 | Method   | Implemented   |
| blendEquation()                         | Method   | Implemented   |
| bindRenderbuffer()                      | Method   | Implemented   |
| bindFramebuffer()                       | Method   | Implemented   |
| bindBuffer()                            | Method   | Implemented   |
| bindAttribLocation()                    | Method   | Implemented   |
| attachShader()                          | Method   | Implemented   |
| activeTexture()                         | Method   | Implemented   |
| validateProgram()                       | Method   | Implemented   |
| texSubImage2D()                         | Method   | Implemented   |
| texParameterf()                         | Method   | Implemented   |
| stencilOpSeparate()                     | Method   | Implemented   |
| stencilOp()                             | Method   | Implemented   |
| stencilMaskSeparate()                   | Method   | Implemented   |
| stencilMask()                           | Method   | Implemented   |
| stencilFuncSeparate()                   | Method   | Implemented   |
| stencilFunc()                           | Method   | Implemented   |
| sampleCoverage()                        | Method   | Implemented   |
| readPixels()                            | Method   | Implemented   |
| polygonOffset()                         | Method   | Implemented   |
| isTexture()                             | Method   | Implemented   |
| isShader()                              | Method   | Implemented   |
| isRenderbuffer()                        | Method   | Implemented   |
| isProgram()                             | Method   | Implemented   |
| isFramebuffer()                         | Method   | Implemented   |
| isEnabled()                             | Method   | Implemented   |
| isContextLost()                         | Method   | *Empty*       |
| isBuffer()                              | Method   | Implemented   |
| getVertexAttribOffset()                 | Method   | Implemented   |
| getVertexAttrib()                       | Method   | Implemented   |
| getUniform()                            | Method   | Implemented   |
| getTexParameter()                       | Method   | Implemented   |
| getSupportedExtensions()                | Method   | Implemented   |
| getShaderSource()                       | Method   | Implemented   |
| getShaderPrecisionFormat()              | Method   | Implemented   |
| getShaderInfoLog()                      | Method   | Implemented   |
| getRenderbufferParameter()              | Method   | Implemented   |
| getProgramParameter()                   | Method   | Implemented   |
| getProgramInfoLog()                     | Method   | Implemented   |
| getParameter()                          | Method   | Implemented   |
| getFramebufferAttachmentParameter()     | Method   | Implemented   |
| getExtension()                          | Method   | Implemented   |
| getError()                              | Method   | Implemented   |
| getContextAttributes()                  | Method   | *Empty*       |
| getBufferParameter()                    | Method   | Implemented   |
| getAttachedShaders()                    | Method   | Implemented   |
| getActiveUniform()                      | Method   | Implemented   |
| getActiveAttrib()                       | Method   | Implemented   |
| drawArrays / drawArraysInstancedANGLE() | Method   | Implemented   |
| detachShader()                          | Method   | Implemented   |
| depthRange()                            | Method   | Implemented   |
| copyTexSubImage2D()                     | Method   | Implemented   |
| copyTexImage2D()                        | Method   | Implemented   |
| compressedTexSubImage2D()               | Method   | Implemented   |
| compressedTexImage2D()                  | Method   | Implemented   |
| checkFramebufferStatus()                | Method   | Implemented   |
| bufferSubData()                         | Method   | Implemented   |
| blendColor()                            | Method   | Implemented   |
| bindTexture()                           | Method   | Implemented   |
| commit()                                | Method   | **Undefined** |
| finish()                                | Method   | **Undefined** |
| framebufferRenderbuffer()               | Method   | Implemented   |
| hint()                                  | Method   | **Undefined** |