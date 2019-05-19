struct standardShader{
	GLuint shader;

	GLint* attributes;
	GLint* uniforms;

	// UNIFORM BLOCK
	GLuint uniformBindingPoint;
	GLuint uniformBlock;
	GLint  uniformOffset[3]; // hax, is determined by uniformNames's size, which is 3

	void generateUniformBlock()
	{
		// ----------------------------------------------------------------
		// HANDLING THE UNIFORM BLOCK
		// ----------------------------------------------------------------
		const GLuint uniformBlockIndex = glGetUniformBlockIndex(shader, "Uniform");
		uniformBindingPoint = 1;
		glUniformBlockBinding(shader, uniformBlockIndex, uniformBindingPoint);

		glGenBuffers(1, &uniformBlock);
		{
			// Allocate space for the uniform block buffer
			GLint uniformBlockSize;
			glGetActiveUniformBlockiv(shader, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);
			glBindBuffer(GL_UNIFORM_BUFFER, uniformBlock);
			glBufferData(GL_UNIFORM_BUFFER, uniformBlockSize, nullptr, GL_DYNAMIC_DRAW);
		}

		const GLchar* uniformName[] = {
			"Uniform.objectToWorldMatrix",
			"Uniform.modelViewProjectionMatrix",
			"Uniform.cameraPosition" };

		const int numBlockUniforms = sizeof(uniformName) / sizeof(uniformName[0]);

		// Map uniform names to indices within the block
		GLuint uniformIndex[numBlockUniforms];
		glGetUniformIndices(shader, numBlockUniforms, uniformName, uniformIndex);
		assert(uniformIndex[0] < 10000);

		// Map indices to byte offsets
		glGetActiveUniformsiv(shader, numBlockUniforms, uniformIndex, GL_UNIFORM_OFFSET, uniformOffset);
		assert(uniformOffset[0] >= 0);
		// ----------------------------------------------------------------
	}

	standardShader(std::string vertexShader, std::string pixelShader)
	{
		shader = createShaderProgram(loadTextFile(vertexShader), loadTextFile(pixelShader));

		generateUniformBlock();
	}

	standardShader(std::string vertexShader, std::string geometryShader, std::string pixelShader)
	{
		shader = createShaderProgram(loadTextFile(vertexShader), loadTextFile(geometryShader), loadTextFile(pixelShader));

		generateUniformBlock();
	}

};