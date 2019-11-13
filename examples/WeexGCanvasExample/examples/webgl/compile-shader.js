function compile({ vShader, fShader, gl, options }) {

    let textureCount = 0;

    function createShaderProgram(vShaderSource, fShaderSource, gl) {

        function loadShader(gl, type, source) {
            var shader = gl.createShader(type);
            gl.shaderSource(shader, source);
            gl.compileShader(shader);
            if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
                throw 'An error occurred compiling the shaders: ' + gl.getShaderInfoLog(shader);
            }
            return shader;
        }

        const fShader = loadShader(gl, gl.FRAGMENT_SHADER, fShaderSource);
        const vShader = loadShader(gl, gl.VERTEX_SHADER, vShaderSource);

        const program = gl.createProgram();
        gl.attachShader(program, vShader);
        gl.attachShader(program, fShader);
        gl.linkProgram(program);

        if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
            throw 'Unable to initialize the shader program: ' + gl.getProgramInfoLog(program);
        }

        const uniforms = [];
        const attributes = [];

        const attributeCount = gl.getProgramParameter(program, gl.ACTIVE_ATTRIBUTES);

        for (let i = 0; i < attributeCount; i++) {
            const attribute = gl.getActiveAttrib(program, i);
            attributes.push(attribute);
        }
        const uniformCount = gl.getProgramParameter(program, gl.ACTIVE_UNIFORMS);
        for (let i = 0; i < uniformCount; i++) {
            const uniform = gl.getActiveUniform(program, i);
            uniforms.push(uniform);
        }

        return {
            program: program,
            uniforms, attributes
        };
    }

    function getType(value, gl) {
        const types = [
            'FLOAT', 'FLOAT_VEC2', 'FLOAT_VEC3', 'FLOAT_VEC4', 'FLOAT_MAT2', 'FLOAT_MAT3', 'FLOAT_MAT4',
            'INT', 'INT_VEC2', 'INT_VEC3', 'INT_VEC4',
            'BOOL', 'BOOL_VEC2', 'BOOL_VEC3', 'BOOL_VEC4',
            'SAMPLER_2D', 'SAMPLER_CUBE'
        ];

        for (let i = 0; i < types.length; i++) {
            if (gl[types[i]] === value) {
                return types[i]
            }
        }

        throw 'get type failed ' + value;
    }

    function attributeManager(attribute, program, gl) {
        const manager = {};
        const type = getType(attribute.type, gl);
        const position = gl.getAttribLocation(program, attribute.name);

        const baseType = type.split('_')[0];
        const vecType = type.split('_').length > 1 ? type.split('_')[1] : 'VEC1';
        const vecSize = Number(vecType[3]);

        const arrayTypeMap = {
            'FLOAT': Float32Array,
            'INT': Int16Array,
            'BOOL': Uint8Array
        }
        const ArrayType = arrayTypeMap[baseType];

        manager.fill = function (buffer) {
            gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
            gl.vertexAttribPointer(position, vecSize, gl[baseType], false, 0, 0);
            gl.enableVertexAttribArray(position);
        };

        manager.createBuffer = function (value) {
            const buffer = gl.createBuffer();
            gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
            gl.bufferData(gl.ARRAY_BUFFER, new ArrayType(value), gl.STATIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER, null);
            return buffer;
        };

        return {
            type, ...manager
        }
    }

    function uniformManager(uniform, program, gl) {
        const manager = {};
        const type = getType(uniform.type, gl);
        const position = gl.getUniformLocation(program, uniform.name);

        const baseType = type.split('_')[0];
        const vecType = type.split('_').length > 1 ? type.split('_')[1] : 'VEC1';
        const baseVecType = vecType.substr(0, 3);
        const vecSize = Number(vecType[3]);

        const arrayTypeMap = {
            'FLOAT': Float32Array,
            'INT': Int32Array,
            'BOOL': Uint8Array
        }
        const ArrayType = arrayTypeMap[baseType];

        let uniformMethodName;
        switch (baseVecType) {
            case 'VEC':
                uniformMethodName = ['uniform', vecSize, baseType === 'FLOAT' ? 'f' : 'i', 'v'].join('');
                manager.fill = function (data) {
                    gl[uniformMethodName](position, new ArrayType(data));
                }
                break;
            case 'MAT':
                uniformMethodName = ['uniform', 'Matrix', vecSize, 'fv'].join('');
                manager.fill = function (data) {
                    gl[uniformMethodName](position, false, new ArrayType(data));
                }
                break;
            case '2D': {
                const textureUnitIndex = textureCount;
                const textureUnitName = `TEXTURE${textureUnitIndex}`;
                if (textureCount === maxTextureImageUnits) {
                    throw 'texture size exceed max texture image units';
                } else {
                    textureCount++;
                }

                manager.createTexture = function (image, flipY = true) {
                    const texture = gl.createTexture();

                    gl.activeTexture(gl[textureUnitName]);
                    gl.bindTexture(gl.TEXTURE_2D, texture);

                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

                    if (flipY) {
                        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
                    }

                    if (image instanceof Uint8Array) {
                        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, image);
                    } else {
                        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
                    }

                    if (flipY) {
                        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, false);
                    }

                    gl.bindTexture(gl.TEXTURE_2D, null);
                    return texture;
                };
                manager.fill = function (texture) {
                    gl.activeTexture(gl[textureUnitName]);
                    gl.bindTexture(gl.TEXTURE_2D, texture);
                    gl.uniform1i(position, textureUnitIndex);
                }

                manager.update = function (texture, image) {
                    gl.activeTexture(gl[textureUnitName]);
                    gl.bindTexture(gl.TEXTURE_2D, texture);
                    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
                }
                break;
            }
            default:
                throw 'baseVecType invalid';
        }

        return { type, ...manager }
    }

    const maxTextureImageUnits = gl.getParameter(gl.MAX_TEXTURE_IMAGE_UNITS);
    const { program, attributes: attributeList, uniforms: uniformList } = createShaderProgram(vShader, fShader, gl);

    const attributes = {};
    const uniforms = {};
    attributeList.forEach(function (attribute) {
        attributes[attribute.name] = attributeManager(attribute, program, gl);
    });
    uniformList.forEach(function (uniform) {
        let name = uniform.name;
        if (name.endsWith('[0]')) {
            name = name.replace('[0]', '');
        }
        uniforms[name] = uniformManager(uniform, program, gl);
    });

    gl.useProgram(program);

    return {
        program,
        uniforms,
        attributes,

        drawArrays: function (count) {
            gl.drawArrays(gl.TRIANGLES, 0, count);
        },

        drawElements: function (count) {
            gl.drawElements(gl.TRIANGLES, count, gl.UNSIGNED_SHORT, 0);
        },

        createElementsBuffer: function (value) {
            const buffer = gl.createBuffer();
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
            gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(value), gl.STATIC_DRAW);
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
            return buffer;
        },

        fillElements: function (buffer) {
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
            return buffer;
        },

        createFramebuffer: function (width, height) {
            const framebuffer = gl.createFramebuffer();
            const tex = gl.createTexture();
            gl.activeTexture(gl.TEXTURE0);
            gl.bindTexture(gl.TEXTURE_2D, tex);
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
            gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
            gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, tex, 0);
            gl.bindFramebuffer(gl.FRAMEBUFFER, null);
            return {
                framebuffer,
                texture: tex
            };
        }
    };
}

export { compile };