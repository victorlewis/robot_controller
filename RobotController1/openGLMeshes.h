struct vertexMesh{
	int num_vertices;
	int num_triangles;
	int num_indices;

	float *position;
	int *triangles;
};

struct lightfieldMesh : vertexMesh{
	float *rotation;
};

struct lightfieldMeshId : vertexMesh{
	float *rotation;
	uint32_t* id;
};

struct vertexMeshGL : vertexMesh{
	GLuint positionBuffer;
	GLuint indexBuffer;

	vertexMeshGL(int in_num_vertices, int in_num_triangles){
		num_vertices = in_num_vertices;
		num_triangles = in_num_triangles;
		position = (float*)malloc(sizeof(float)*num_vertices * 3);
		triangles = (int*)malloc(sizeof(int)*num_triangles * 3);
		num_indices = num_triangles * 3;
	}

	void genOpenGLBuffers(void){
		positionBuffer = GL_NONE;

		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, position, GL_STATIC_DRAW);

		indexBuffer = GL_NONE;
		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_triangles * 3, triangles, GL_STATIC_DRAW);
	}
};

struct lightfieldMeshGL : lightfieldMesh{
	lightfieldMeshGL(int in_num_vertices, int in_num_triangles){
		num_vertices = in_num_vertices;
		num_triangles = in_num_triangles;
		position = (float*)malloc(sizeof(float)*num_vertices * 3);
		rotation = (float*)malloc(sizeof(float)*num_vertices * 4);
		triangles = (int*)malloc(sizeof(int)*num_triangles * 3);
		num_indices = num_triangles * 3;
	}

	GLuint positionBuffer;
	GLuint rotationBuffer;
	GLuint indexBuffer;

	void genOpenGLBuffers(void){
		positionBuffer = GL_NONE;
		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, position, GL_STATIC_DRAW);

		rotationBuffer = GL_NONE;
		glGenBuffers(1, &rotationBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, rotationBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_vertices * 4, rotation, GL_STATIC_DRAW);

		indexBuffer = GL_NONE;
		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_triangles * 3, triangles, GL_STATIC_DRAW);
	}

};

lightfieldMeshGL generateLFPlane(int rows, int columns, int width_size, int height_size)
{
	int index_width = rows - 1; // because a grid with 3x3 vertex rows and columns has only 2x2 quads, thefore 2x2 * 2 triangles
	int index_height = columns - 1;

	int in_num_vertices = rows* columns; int in_num_triangles = index_width*index_height * 2;
	lightfieldMeshGL output(in_num_vertices, in_num_triangles);

	// Set up vertices
	int v = 0; int u = 0;
	float widthf = rows;

	float heightf = columns;

	float camNum = 0.0f;

	for (float y = 0.0f; y < heightf; y++)
	{
		for (float x = 0.0f; x < widthf; x++)
		{
			output.position[v++] = x / (widthf - 1);
			output.position[v++] = y / (heightf - 1);
			output.position[v++] = 0.0f;

			output.rotation[u++] = 0.0;
			output.rotation[u++] = 0.0;
			output.rotation[u++] = 0.0;

			//output.rotation[u++] = 15.0f;
			//output.rotation[u++] = 18.0f;
			//output.rotation[u++] = -44.0f;

			output.rotation[u++] = camNum;
			camNum++;

		}
	}

	// Set up indices
	int i = 0;
	output.triangles = (int*)malloc(sizeof(int)*output.num_triangles * 3);

	// going quad by quad
	for (int y = 0; y < index_height; y++)
	{
		int base = y*rows;
		for (int x = 0; x < index_width; x++)
		{
			// |7
			output.triangles[i++] = x + base;
			output.triangles[i++] = x + rows + base;
			output.triangles[i++] = x + 1 + base;

			// /_|
			output.triangles[i++] = x + 1 + base;
			output.triangles[i++] = x + 1 + rows + base;
			output.triangles[i++] = x + rows + base;
		}
	}

	output.genOpenGLBuffers();

	return output;
}

lightfieldMeshGL generateLFCube(int rows, int columns, int width_size, int height_size)
{
	int index_width = rows - 1; // because a grid with 3x3 vertex rows and columns has only 2x2 quads, thefore 2x2 * 2 triangles
	int index_height = columns - 1;

	int in_num_vertices = rows* columns * 6; int in_num_triangles = index_width*index_height * 2 * 6;
	lightfieldMeshGL output(in_num_vertices, in_num_triangles);

	// Set up vertices
	int v = 0; int u = 0;
	float widthf = rows;
	float heightf = columns;
	float camNum = 0.0f;

	// Set up indices
	int i = 0;
	output.triangles = (int*)malloc(sizeof(int)*output.num_triangles * 3);

	Vector3 rotArray[6] = { Vector3(0.0, 0.0, 0.0),
		Vector3(0.0, 1.57079, 0.0),
		Vector3(0.0, 1.57079 * 2, 0.0),
		Vector3(0.0, 1.57079 * 3, 0.0),
		Vector3(1.57079, 0.0, 0.0),
		Vector3(-1.57079, 0.0, 0.0) };

	for (int cubeFace = 0; cubeFace < 6; cubeFace++)
	{
		Vector3 curRot = rotArray[cubeFace];

		Matrix4x4 curRotMatrix = Matrix4x4::roll(curRot.z) *
			Matrix4x4::yaw(curRot.y) *
			Matrix4x4::pitch(curRot.x);

		for (float y = 0.0f; y < heightf; y++)
		{
			for (float x = 0.0f; x < widthf; x++)
			{
				Vector4 curPos = Vector4(x / (widthf - 1) - 0.5, y / (heightf - 1) - 0.5, -0.5, 0.0);
				curPos = curRotMatrix*curPos;

				output.position[v++] = curPos.x;
				output.position[v++] = curPos.y;
				output.position[v++] = curPos.z;

				output.rotation[u++] = curRot.x;
				output.rotation[u++] = curRot.y;
				output.rotation[u++] = curRot.z;
				//output.rotation[u++] = 0.0;
				//output.rotation[u++] = 0.0;
				//output.rotation[u++] = 0.0;

				//output.rotation[u++] = 15.0f;
				//output.rotation[u++] = 18.0f;
				//output.rotation[u++] = -44.0f;

				output.rotation[u++] = camNum;
				camNum++;

			}
		}


		// going quad by quad
		for (int y = 0; y < index_height; y++)
		{
			int base = y*rows + cubeFace*columns*rows;

			for (int x = 0; x < index_width; x++)
			{
				// |7
				output.triangles[i++] = x + base;
				output.triangles[i++] = x + 1 + base;
				output.triangles[i++] = x + rows + base;

				// /_|
				output.triangles[i++] = x + 1 + base;
				output.triangles[i++] = x + 1 + rows + base;
				output.triangles[i++] = x + rows + base;
			}
		}



	}


	output.genOpenGLBuffers();

	return output;
}


vertexMeshGL generateCube()
{
	int in_num_vertices = 24; int in_num_triangles = 12;
	vertexMeshGL output(in_num_vertices, in_num_triangles);

	float focusDist = 1.0f;
	float boxX = 1.0f*focusDist;
	float boxY = 1.0f*focusDist;
	float boxZ = 1.0f*focusDist;

	float position[] = {
		0.0f, boxY, 0.0f,
		0.0f, boxY, boxZ,
		boxX, boxY, boxZ,
		boxX, boxY, 0.0f,
		0.0f, boxY, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, boxZ,
		0.0f, boxY, boxZ,
		boxX, boxY, boxZ,
		boxX, 0.0f, boxZ,
		boxX, 0.0f, 0.0f,
		boxX, boxY, 0.0f,
		boxX, boxY, 0.0f,
		boxX, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, boxY, 0.0f,
		0.0f, boxY, boxZ,
		0.0f, 0.0f, boxZ,
		boxX, 0.0f, boxZ,
		boxX, boxY, boxZ,
		0.0f, 0.0f, boxZ,
		0.0f, 0.0f, 0.0f,
		boxX, 0.0f, 0.0f,
		boxX, 0.0f, boxZ };

	// center the box
	for (int i = 0; i < 24; i++)
	{
		position[i * 3 + 0] = position[i * 3 + 0] - 0.5;
		position[i * 3 + 1] = position[i * 3 + 1] - 0.5;
		position[i * 3 + 2] = position[i * 3 + 2] - 0.5;
	}

	int index[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23 };

	memcpy(output.position, position, sizeof(float)*output.num_vertices * 3);
	memcpy(output.triangles, index, sizeof(float)*output.num_triangles * 3);

	output.genOpenGLBuffers();

	return output;
}

vertexMeshGL generateFatCube()
{
	int in_num_vertices = 24; int in_num_triangles = 12;
	vertexMeshGL output(in_num_vertices, in_num_triangles);

	float focusDist = 1.0f;
	float boxX = 100.0f;
	float boxY = 100.0f;
	float boxZ = 1.0f*focusDist;

	float position[] = {
		0.0f, boxY, 0.0f,
		0.0f, boxY, boxZ,
		boxX, boxY, boxZ,
		boxX, boxY, 0.0f,
		0.0f, boxY, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, boxZ,
		0.0f, boxY, boxZ,
		boxX, boxY, boxZ,
		boxX, 0.0f, boxZ,
		boxX, 0.0f, 0.0f,
		boxX, boxY, 0.0f,
		boxX, boxY, 0.0f,
		boxX, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, boxY, 0.0f,
		0.0f, boxY, boxZ,
		0.0f, 0.0f, boxZ,
		boxX, 0.0f, boxZ,
		boxX, boxY, boxZ,
		0.0f, 0.0f, boxZ,
		0.0f, 0.0f, 0.0f,
		boxX, 0.0f, 0.0f,
		boxX, 0.0f, boxZ };

	// center the box
	for (int i = 0; i < 24; i++)
	{
		position[i * 3 + 0] = position[i * 3 + 0] - 0.5;
		position[i * 3 + 1] = position[i * 3 + 1] - 0.5;
		position[i * 3 + 2] = position[i * 3 + 2] - 0.5;
	}

	int index[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23 };

	memcpy(output.position, position, sizeof(float)*output.num_vertices * 3);
	memcpy(output.triangles, index, sizeof(float)*output.num_triangles * 3);

	output.genOpenGLBuffers();

	return output;
}


// THESE ARE BASED ON THE TRIANGLES, NOT THE VERTEX SLICES
// So "72x24" IS ACTUALLY 1658 VERTICES/CAMERAS, NOT 1728. YOU CANNOT HAVE 1728 VERTICES 
// NUMBER OF SEGMENTS IS EQUAL TO BOTH THE NUMBER OF VERTICES ON A RING, AND THE NUMBER OF TRIANGLES ON A RING
// NOTE THAT BLENDER'S APPROACH HAS BOTH TRIS AND QUADS



vertexMeshGL generateUVSphereVertex(int rings, int segments)
{
	int in_num_vertices = segments * (rings - 1) + 2; int in_num_triangles = segments*rings * 2 - (segments * 2);
	std::cout << "Num vertices: " << in_num_vertices << " and tris: " << in_num_triangles << std::endl;

	vertexMeshGL output(in_num_vertices, in_num_triangles);

	int p = 0;
	int vertexSize = sizeof(float) * 3;

	// ADD THE BOTTOM VERTEX
	output.position[p++] = 0;
	output.position[p++] = 1.0f;
	output.position[p++] = 0.0f;

	// THERE ARE ACTUALLY rings-1 RINGS OF VERTICES.
	// START AT 1, OR YOU WILL ADD THE TOP-VERTEX AGAIN
	for (int r = 1; r < rings; r++)
	{
		float theta = ((float)(r) / (rings))*PI;

		// NUMBER OF SEGMENTS IS JUST THE NUMBER OF VERTICES ON A RING
		for (int s = 0; s < segments; s++)
		{
			float phi = ((float)(s) / segments) * 2 * PI; // azimuth goes around 0 .. 2*PI
			//printf("lon is %f, lat if %f\n", lon, lat);
			// OPENGL IS Y UP, Z FORWARD
			float x = sin(theta)*cos(phi);
			float z = sin(theta)*sin(phi);
			float y = cos(theta);

			//printf("x: %f, y: %f, z: %f\n", x, -z, y);

			// TO-DO: CHECK CORRECTNESS OF THESE
			output.position[p++] = x;
			output.position[p++] = y;
			output.position[p++] = z;
		}
	}

	// ADD THE LAST, BOTTOM VERTEX
	output.position[p++] = 0;
	output.position[p++] = -1.0f;
	output.position[p++] = 0;

	//	for (int i = 0; i < in_num_vertices; i++)
	//	{
	//		printf("x: %f y: %f z: %f\n", output.position[i * 3 + 0], output.position[i * 3 + 1], output.position[i * 3 + 2]);
	//	}
	// GENERATE THE TRIANGLES

	// Triangle fan at top
	int t = 0;
	int v1 = 0; int v2 = 1; int v3 = 2;
	for (int i = 0; i < segments; i++)
	{
		output.triangles[t++] = v1;
		output.triangles[t++] = v2;
		output.triangles[t++] = v3;
		v2++; v3++;
		if (v3 > segments) v3 = 1;
	}

	//// RINGS
	for (int r = 0; r < rings - 2; r++)
	{
		// V1 | V2
		// V3 | V4

		v1 = r*segments + 1;
		v2 = r*segments + 1 + 1;
		v3 = (r + 1)*segments + 1;
		int v4 = (r + 1)*segments + 1 + 1;

		for (int s = 0; s < segments; s++)
		{
			// UPPER TRIANGLE
			output.triangles[t++] = v1;
			output.triangles[t++] = v3;
			output.triangles[t++] = v2;

			// LOWER TRIANGLE
			output.triangles[t++] = v2;
			output.triangles[t++] = v3;
			output.triangles[t++] = v4;

			v1++; v2++; v3++; v4++;

			if (v2 > r*segments + segments) v2 = r*segments + 1;
			if (v4 > (r + 1)*segments + segments) v4 = (r + 1)*segments + 1;
		}
	}

	// Triangle fan at the bottom
	v1 = in_num_vertices - 1 - segments;
	v2 = in_num_vertices - 1 - segments + 1;
	v3 = in_num_vertices - 1;

	for (int i = 0; i < segments; i++)
	{
		output.triangles[t++] = v1;
		output.triangles[t++] = v2;
		output.triangles[t++] = v3;
		v1++; v2++;
		if (v2 > in_num_vertices - 1 - 1) v2 = in_num_vertices - 1 - segments;
	}

	output.genOpenGLBuffers();

	return output;

}


// LFMESH VERSION
lightfieldMeshGL generateLightfieldUVSphere(int rings, int segments)
{
	int in_num_vertices = segments * (rings - 1) + 2; int in_num_triangles = segments*rings * 2 - (segments * 2);
	std::cout << "Num vertices: " << in_num_vertices << " and tris: " << in_num_triangles << std::endl;

	lightfieldMeshGL output(in_num_vertices, in_num_triangles);

	int p = 0;
	int o = 0;
	float cameraID = 0.0f;
	int vertexSize = sizeof(float) * 3;

	// ADD THE BOTTOM VERTEX
	output.position[p++] = 0;
	output.position[p++] = 1.0f;
	output.position[p++] = 0.0f;

	// ADD THE TOP ROTATION
	output.rotation[o++] = PI / 2.0f;
	output.rotation[o++] = 0.0f;
	output.rotation[o++] = 0.0f;
	output.rotation[o++] = cameraID;
	cameraID += 1.0f;

	// THERE ARE ACTUALLY rings-1 RINGS OF VERTICES.
	// START AT 1, OR YOU WILL ADD THE TOP-VERTEX AGAIN
	for (int r = 1; r < rings; r++)
	{
		float theta = ((float)(r) / (rings))*PI;

		// NUMBER OF SEGMENTS IS JUST THE NUMBER OF VERTICES ON A RING
		for (int s = 0; s < segments; s++)
		{
			float phi = ((float)(s) / segments) * 2 * PI; // azimuth goes around 0 .. 2*PI
			//printf("lon is %f, lat if %f\n", lon, lat);
			// OPENGL IS Y UP, Z FORWARD
			float x = sin(theta)*cos(phi);
			float z = sin(theta)*sin(phi);
			float y = cos(theta);

			//printf("x: %f, y: %f, z: %f\n", x, -z, y);

			output.position[p++] = x;
			output.position[p++] = y;
			output.position[p++] = z;

			// ADD THE ROTATION
			output.rotation[o++] = PI / 2.0f - theta;
			//output.rotation[o++] = 0.0f;
			output.rotation[o++] = -phi + 1.5*PI;
			output.rotation[o++] = 0.0f;
			output.rotation[o++] = cameraID;
			cameraID += 1.0f;
		}
	}

	// ADD THE LAST, BOTTOM VERTEX
	output.position[p++] = 0;
	output.position[p++] = -1.0f;
	output.position[p++] = 0;

	// ADD THE TOP ROTATION
	output.rotation[o++] = -PI / 2.0f;
	output.rotation[o++] = 0.0f;
	output.rotation[o++] = 0.0f;
	output.rotation[o++] = cameraID;
	cameraID += 1.0f;

	//	for (int i = 0; i < in_num_vertices; i++)
	//	{
	//		printf("x: %f y: %f z: %f\n", output.position[i * 3 + 0], output.position[i * 3 + 1], output.position[i * 3 + 2]);
	//	}
	// GENERATE THE TRIANGLES

	// Triangle fan at bottom
	int t = 0;
	int v1 = 0; int v2 = 1; int v3 = 2;
	for (int i = 0; i < segments; i++)
	{
		output.triangles[t++] = v1;
		output.triangles[t++] = v2;
		output.triangles[t++] = v3;
		v2++; v3++;
		if (v3 > segments) v3 = 1;

	}

	//// RINGS
	for (int r = 0; r < rings - 2; r++)
	{
		// V1 | V2
		// V3 | V4

		v1 = r*segments + 1;
		v2 = r*segments + 1 + 1;
		v3 = (r + 1)*segments + 1;
		int v4 = (r + 1)*segments + 1 + 1;

		for (int s = 0; s < segments; s++)
		{
			// UPPER TRIANGLE
			output.triangles[t++] = v1;
			output.triangles[t++] = v3;
			output.triangles[t++] = v2;

			// LOWER TRIANGLE
			output.triangles[t++] = v2;
			output.triangles[t++] = v3;
			output.triangles[t++] = v4;

			v1++; v2++; v3++; v4++;

			if (v2 > r*segments + segments) v2 = r*segments + 1;
			if (v4 > (r + 1)*segments + segments) v4 = (r + 1)*segments + 1;
		}
	}

	// Triangle fan at the bottom
	v1 = in_num_vertices - 1 - segments;
	v2 = in_num_vertices - 1 - segments + 1;
	v3 = in_num_vertices - 1;

	for (int i = 0; i < segments; i++)
	{
		output.triangles[t++] = v1;
		output.triangles[t++] = v2;
		output.triangles[t++] = v3;
		v1++; v2++;
		if (v2 > in_num_vertices - 1 - 1) v2 = in_num_vertices - 1 - segments;
	}

	output.genOpenGLBuffers();

	return output;

}



lightfieldMeshGL loadCameraMesh(const char* path, float scaling)
{
	FILE * file = fopen(path, "r");

	if (file == NULL){
		printf("Can't open file!\n");
	}

	std::vector<float> temp_vertices;
	std::vector<float> temp_rotations;
	std::vector<int> temp_indices;
	int temp_numVertices = 0;
	int temp_numTriangles = 0;

	while (1){
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0)
		{
			float vertex[3];
			fscanf(file, "%f %f %f\n",
				&vertex[0], &vertex[1], &vertex[2]
				);

			temp_vertices.push_back(vertex[0] * scaling);
			temp_vertices.push_back(vertex[1] * scaling);
			temp_vertices.push_back(vertex[2] * scaling);

			temp_numVertices++;

			printf("v");
		}
		else if (strcmp(lineHeader, "r") == 0)
		{
			float rot[4];
			fscanf(file, "%f %f %f %f\n",
				&rot[0], &rot[1], &rot[2], &rot[3]);

			temp_rotations.push_back(rot[0]);
			temp_rotations.push_back(rot[1]);
			temp_rotations.push_back(rot[2]);
			temp_rotations.push_back(rot[3]);

			printf("r");
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			int indices[3];
			fscanf(file, "%i %i %i\n", &indices[0], &indices[1], &indices[2]);
			temp_indices.push_back(indices[0]);
			temp_indices.push_back(indices[1]);
			temp_indices.push_back(indices[2]);
			temp_numTriangles++;

			printf("i");

		}
		else
		{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	fclose(file);

	printf("Opened camera mesh with %i vertices and %i triangles\n", temp_numVertices, temp_numTriangles);

	lightfieldMeshGL output(temp_numVertices, temp_numTriangles);

	output.position = &temp_vertices[0];
	output.rotation = &temp_rotations[0];
	output.triangles = &temp_indices[0];

	output.genOpenGLBuffers();

	return output;
}

// APERTURE-BASED VERSION
struct uvMeshGL
{
	int num_vertices;
	int num_triangles;
	int num_indices;

	float *position;
	float *uv;
	int *triangles;

	GLuint positionBuffer;
	GLuint uvBuffer;
	GLuint indexBuffer;

	uvMeshGL(int in_num_vertices, int in_num_triangles){
		num_vertices = in_num_vertices;
		num_triangles = in_num_triangles;
		position = (float*)malloc(sizeof(float)*num_vertices * 3);
		uv = (float*)malloc(sizeof(float)*num_vertices * 2);
		triangles = (int*)malloc(sizeof(int)*num_triangles * 3);
		num_indices = num_triangles * 3;
	}

	void genOpenGLBuffers(void){
		positionBuffer = GL_NONE;
		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, position, GL_STATIC_DRAW);

		uvBuffer = GL_NONE;
		glGenBuffers(1, &uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 2, uv, GL_STATIC_DRAW);

		indexBuffer = GL_NONE;
		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_triangles * 3, triangles, GL_STATIC_DRAW);
	}
};

uvMeshGL generateQuad()
{
	uvMeshGL output(4, 2);

	float quadVertices[12] = {
		-1.0f, -1.0f, -3.0f,
		1.0f, -1.0f, -3.0f,
		-1.0f, 1.0f, -3.0f,
		1.0f, 1.0f, -3.0f,
	};

	float uv[8] = {
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0
	};

	int triangles[6] = { 0, 1, 3,
		0, 2, 3 };

	memcpy(output.position, quadVertices, sizeof(float)*output.num_vertices * 3);
	memcpy(output.uv, uv, sizeof(float)*output.num_vertices * 2);
	memcpy(output.triangles, triangles, sizeof(int)*output.num_triangles * 3);

	output.genOpenGLBuffers();

	return output;
}

uvMeshGL generateUVPlane(int rows, int columns, int width_size, int height_size)
{
	int index_width = rows - 1; // because a grid with 3x3 vertex rows and columns has only 2x2 quads, thefore 2x2 * 2 triangles
	int index_height = columns - 1;

	int in_num_vertices = rows* columns; int in_num_triangles = index_width*index_height * 2;
	uvMeshGL output(in_num_vertices, in_num_triangles);

	// Set up vertices
	int v = 0; int u = 0;
	float widthf = rows;

	float heightf = columns;

	float camNum = 0.0f;

	for (float y = 0.0f; y < heightf; y++)
	{
		for (float x = 0.0f; x < widthf; x++)
		{
			output.position[v++] = x / (widthf - 1);
			output.position[v++] = y / (heightf - 1);
			output.position[v++] = 0.0f;

			output.uv[u++] = x / (widthf - 1);
			output.uv[u++] = 1 - (y / (heightf - 1));

			camNum++;
		}
	}

	// Set up indices
	int i = 0;
	output.triangles = (int*)malloc(sizeof(int)*output.num_triangles * 3);

	// going quad by quad
	for (int y = 0; y < index_height; y++)
	{
		int base = y*rows;
		for (int x = 0; x < index_width; x++)
		{
			// |7
			output.triangles[i++] = x + base;
			output.triangles[i++] = x + rows + base;
			output.triangles[i++] = x + 1 + base;

			// /_|
			output.triangles[i++] = x + 1 + base;
			output.triangles[i++] = x + 1 + rows + base;
			output.triangles[i++] = x + rows + base;
		}
	}

	output.genOpenGLBuffers();

	return output;
}


struct cameraBuffer
{
	int numCameras;
	float *position;
	float *rotation;

	GLuint positionBuffer;
	GLuint rotationBuffer;

	cameraBuffer(int in_numCameras){
		numCameras = in_numCameras;
		position = (float*)malloc(sizeof(float)*numCameras * 3);
		rotation = (float*)malloc(sizeof(float)*numCameras * 4);
	}

	void genOpenGLBuffers(void){
		positionBuffer = GL_NONE;
		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numCameras * 3, position, GL_STATIC_DRAW);

		rotationBuffer = GL_NONE;
		glGenBuffers(1, &rotationBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, rotationBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numCameras * 4, rotation, GL_STATIC_DRAW);
	}

};

cameraBuffer cameraBuffer_generateLFPlane(int rows, int columns, int width, int height)
{
	int in_num_vertices = rows* columns;
	cameraBuffer output(in_num_vertices);

	// Set up vertices
	int v = 0; int u = 0;

	float widthf = rows;
	float heightf = columns;

	float camNum = 0.0f;

	for (float y = 0.0f; y < heightf; y++)
	{
		for (float x = 0.0f; x < widthf; x++)
		{
			output.position[v++] = x / (widthf - 1);
			output.position[v++] = y / (heightf - 1);
			output.position[v++] = -1;

			output.rotation[u++] = 0.0;
			output.rotation[u++] = 0.0;
			output.rotation[u++] = 0.0;
			output.rotation[u++] = camNum;

			//output.rotation[u++] = 15.0f;
			//output.rotation[u++] = 18.0f;
			//output.rotation[u++] = -44.0f;
			camNum++;
		}
	}

	output.genOpenGLBuffers();

	return output;
}



cameraBuffer openFromChanFile(char* path)
{
	int cameraNum = 0;
	std::vector<float> loadedcams;

	float inputs[6];
	float garbage[2];

	std::cout << "Started loading camera params." << std::endl;

	FILE* fd = fopen(path, "r");

	while (!feof(fd)) {
		fscanf(fd, "%f %f %f %f %f %f %f %f\n", &garbage[0], &inputs[0], &inputs[1], &inputs[2], &inputs[3], &inputs[4], &inputs[5], &garbage[1]);
		loadedcams.push_back(inputs[0]);
		loadedcams.push_back(inputs[1]);
		loadedcams.push_back(inputs[2]);
		loadedcams.push_back(inputs[3]);
		loadedcams.push_back(inputs[4]);
		loadedcams.push_back(inputs[5]);
		cameraNum++;
	}
	fclose(fd);

	cameraBuffer output(cameraNum);

	int l = 0;
	int v = 0;
	int u = 0;

	float camNumForRot = 0.0f;

	for (int i = 0; i < cameraNum; i++)
	{
		output.position[v++] = loadedcams[l++];
		output.position[v++] = loadedcams[l++];
		output.position[v++] = loadedcams[l++];

		/*output.rotation[u++] = loadedcams[l++] * 0.0;
		output.rotation[u++] = loadedcams[l++] * 0.0;
		output.rotation[u++] = loadedcams[l++] * 0.0;*/

		output.rotation[u++] = loadedcams[l++] * 0.0174533;
		output.rotation[u++] = loadedcams[l++] * 0.0174533;
		output.rotation[u++] = loadedcams[l++] * 0.0174533;

		output.rotation[u++] = camNumForRot;

		camNumForRot += 1.0f;
	}

	std::cout << "Loaded " << cameraNum << " cameras successfully." << std::endl;

	output.genOpenGLBuffers();

	return output;
}

cameraBuffer openFromChanFile(char* path, float scaling)
{
	int cameraNum = 0;
	std::vector<float> loadedcams;

	float inputs[6];
	float garbage[2];

	std::cout << "Started loading camera params." << std::endl;

	FILE* fd = fopen(path, "r");

	while (!feof(fd)) {
		fscanf(fd, "%f %f %f %f %f %f %f %f\n", &garbage[0], &inputs[0], &inputs[1], &inputs[2], &inputs[3], &inputs[4], &inputs[5], &garbage[1]);
		loadedcams.push_back(inputs[0] * scaling);
		loadedcams.push_back(inputs[1] * scaling);
		loadedcams.push_back(inputs[2] * scaling);
		loadedcams.push_back(inputs[3]);
		loadedcams.push_back(inputs[4]);
		loadedcams.push_back(inputs[5]);
		cameraNum++;
	}
	fclose(fd);

	cameraBuffer output(cameraNum);

	int l = 0;
	int v = 0;
	int u = 0;

	float camNumForRot = 0.0f;

	for (int i = 0; i < cameraNum; i++)
	{
		output.position[v++] = loadedcams[l++];
		output.position[v++] = loadedcams[l++];
		output.position[v++] = loadedcams[l++];

		/*output.rotation[u++] = loadedcams[l++] * 0.0;
		output.rotation[u++] = loadedcams[l++] * 0.0;
		output.rotation[u++] = loadedcams[l++] * 0.0;*/

		output.rotation[u++] = loadedcams[l++] * 0.0174533;
		output.rotation[u++] = loadedcams[l++] * 0.0174533;
		output.rotation[u++] = loadedcams[l++] * 0.0174533;

		output.rotation[u++] = camNumForRot;

		camNumForRot += 1.0f;
	}

	std::cout << "Loaded " << cameraNum << " cameras successfully." << std::endl;

	output.genOpenGLBuffers();

	return output;
}



struct lightfieldMeshIdGL : lightfieldMeshId{
	lightfieldMeshIdGL(int in_num_vertices, int in_num_triangles){
		num_vertices = in_num_vertices;
		num_triangles = in_num_triangles;
		id = (uint32_t*)malloc(sizeof(uint32_t)*num_vertices);
		position = (float*)malloc(sizeof(float)*num_vertices * 3);
		rotation = (float*)malloc(sizeof(float)*num_vertices * 4);
		triangles = (int*)malloc(sizeof(int)*num_triangles * 3);
		num_indices = num_triangles * 3;
	}

	GLuint idBuffer;
	GLuint positionBuffer;
	GLuint rotationBuffer;
	GLuint indexBuffer;

	void genOpenGLBuffers(void){
		idBuffer = GL_NONE;
		glGenBuffers(1, &idBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, idBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint32_t) * num_vertices, id, GL_STATIC_DRAW);

		positionBuffer = GL_NONE;
		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, position, GL_STATIC_DRAW);

		rotationBuffer = GL_NONE;
		glGenBuffers(1, &rotationBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, rotationBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_vertices * 4, rotation, GL_STATIC_DRAW);

		indexBuffer = GL_NONE;
		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_triangles * 3, triangles, GL_STATIC_DRAW);
	}

};

lightfieldMeshIdGL generateLFPlaneId(int rows, int columns, int width_size, int height_size)
{
	int index_width = rows - 1; // because a grid with 3x3 vertex rows and columns has only 2x2 quads, thefore 2x2 * 2 triangles
	int index_height = columns - 1;

	int in_num_vertices = rows* columns; int in_num_triangles = index_width*index_height * 2;
	lightfieldMeshIdGL output(in_num_vertices, in_num_triangles);

	// Set up vertices
	int v = 0; int u = 0; int d = 0;
	float widthf = rows;

	float heightf = columns;

	uint32_t camNum = 0;

	for (float y = 0.0f; y < heightf; y++)
	{
		for (float x = 0.0f; x < widthf; x++)
		{
			output.position[v++] = x / (widthf - 1);
			output.position[v++] = y / (heightf - 1);
			output.position[v++] = 0.0f;

			output.rotation[u++] = 0.0;
			output.rotation[u++] = 0.0;
			output.rotation[u++] = 0.0;
			output.rotation[u++] = 0.0;

			//output.rotation[u++] = 15.0f;
			//output.rotation[u++] = 18.0f;
			//output.rotation[u++] = -44.0f;

			output.id[d++] = camNum;

			camNum++;

		}
	}

	// Set up indices
	int i = 0;
	output.triangles = (int*)malloc(sizeof(int)*output.num_triangles * 3);

	// going quad by quad
	for (int y = 0; y < index_height; y++)
	{
		int base = y*rows;
		for (int x = 0; x < index_width; x++)
		{
			// |7
			output.triangles[i++] = x + base;
			output.triangles[i++] = x + rows + base;
			output.triangles[i++] = x + 1 + base;

			// /_|
			output.triangles[i++] = x + 1 + base;
			output.triangles[i++] = x + 1 + rows + base;
			output.triangles[i++] = x + rows + base;
		}
	}

	output.genOpenGLBuffers();

	return output;
}




uvMeshGL generateUVSphere(int rings, int segments)
{
	int in_num_vertices = segments * (rings - 1) + 2; int in_num_triangles = segments*rings * 2 - (segments * 2);
	std::cout << "Num vertices: " << in_num_vertices << " and tris: " << in_num_triangles << std::endl;

	uvMeshGL output(in_num_vertices, in_num_triangles);

	int p = 0;
	int vertexSize = sizeof(float) * 3;

	// ADD THE BOTTOM VERTEX
	output.position[p++] = 0;
	output.position[p++] = 1.0f;
	output.position[p++] = 0.0f;

	// THERE ARE ACTUALLY rings-1 RINGS OF VERTICES.
	// START AT 1, OR YOU WILL ADD THE TOP-VERTEX AGAIN
	for (int r = 1; r < rings; r++)
	{
		float theta = ((float)(r) / (rings))*PI;

		// NUMBER OF SEGMENTS IS JUST THE NUMBER OF VERTICES ON A RING
		for (int s = 0; s < segments; s++)
		{
			float phi = ((float)(s) / segments) * 2 * PI; // azimuth goes around 0 .. 2*PI
			//printf("lon is %f, lat if %f\n", lon, lat);
			// OPENGL IS Y UP, Z FORWARD
			float x = sin(theta)*cos(phi);
			float z = sin(theta)*sin(phi);
			float y = cos(theta);

			//printf("x: %f, y: %f, z: %f\n", x, -z, y);

			// TO-DO: CHECK CORRECTNESS OF THESE
			output.position[p++] = x;
			output.position[p++] = y;
			output.position[p++] = z;
		}
	}

	// ADD THE LAST, BOTTOM VERTEX
	output.position[p++] = 0;
	output.position[p++] = -1.0f;
	output.position[p++] = 0;

	//	for (int i = 0; i < in_num_vertices; i++)
	//	{
	//		printf("x: %f y: %f z: %f\n", output.position[i * 3 + 0], output.position[i * 3 + 1], output.position[i * 3 + 2]);
	//	}
	// GENERATE THE TRIANGLES

	// Triangle fan at top
	int t = 0;
	int v1 = 0; int v2 = 1; int v3 = 2;
	for (int i = 0; i < segments; i++)
	{
		output.triangles[t++] = v1;
		output.triangles[t++] = v2;
		output.triangles[t++] = v3;
		v2++; v3++;
		if (v3 > segments) v3 = 1;
	}

	//// RINGS
	for (int r = 0; r < rings - 2; r++)
	{
		// V1 | V2
		// V3 | V4

		v1 = r*segments + 1;
		v2 = r*segments + 1 + 1;
		v3 = (r + 1)*segments + 1;
		int v4 = (r + 1)*segments + 1 + 1;

		for (int s = 0; s < segments; s++)
		{
			// UPPER TRIANGLE
			output.triangles[t++] = v1;
			output.triangles[t++] = v3;
			output.triangles[t++] = v2;

			// LOWER TRIANGLE
			output.triangles[t++] = v2;
			output.triangles[t++] = v3;
			output.triangles[t++] = v4;

			v1++; v2++; v3++; v4++;

			if (v2 > r*segments + segments) v2 = r*segments + 1;
			if (v4 > (r + 1)*segments + segments) v4 = (r + 1)*segments + 1;
		}
	}

	// Triangle fan at the bottom
	v1 = in_num_vertices - 1 - segments;
	v2 = in_num_vertices - 1 - segments + 1;
	v3 = in_num_vertices - 1;

	for (int i = 0; i < segments; i++)
	{
		output.triangles[t++] = v1;
		output.triangles[t++] = v2;
		output.triangles[t++] = v3;
		v1++; v2++;
		if (v2 > in_num_vertices - 1 - 1) v2 = in_num_vertices - 1 - segments;
	}

	// GENERATE THE UV

	/*float2 direction_to_equirectangular_range(float3 dir, float4 range)
	{
	if (is_zero(dir))
	return make_float2(0.0f, 0.0f);

	float u = (atan2f(dir.y, dir.x) - range.y) / range.x;
	float v = (acosf(dir.z / len(dir)) - range.w) / range.z;

	return make_float2(u, v);
	}*/

	int u = 0;

	for (int v = 0; v < in_num_vertices; v++)
	{
		int pos = v * 3;
		float dir_x = output.position[pos + 0];
		float dir_y = output.position[pos + 2] * -1.0;
		float dir_z = output.position[pos + 1] * -1.0;

		float length_dir = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);

		float range_x = -1.570796;
		float range_y = 1.570796;
		float range_z = -1.570796;
		float range_w = 1.570796;

		if (dir_x == 0.0 && dir_y == 0.0 && dir_z == 0.0)
		{
			output.uv[u++] = 0.0;
			output.uv[u++] = 0.0;
		}
		else
		{
			float uv_u = (atan2f(dir_y, dir_x) - range_y) / range_x;
			float uv_v = (acosf(dir_z / length_dir) - range_w) / range_z;
			output.uv[u++] = uv_u*0.5 + 0.5;
			output.uv[u++] = uv_v*0.5 + 0.5;
		}
	}
	printf("Num uvs: %i\n");

	output.genOpenGLBuffers();

	return output;

}

uvMeshGL generateUVSphereFisheye(int rings, int segments)
{
	int in_num_vertices = segments * (rings - 1) + 2; int in_num_triangles = segments*rings * 2 - (segments * 2);
	std::cout << "Num vertices: " << in_num_vertices << " and tris: " << in_num_triangles << std::endl;

	uvMeshGL output(in_num_vertices, in_num_triangles);

	int p = 0;
	int vertexSize = sizeof(float) * 3;

	// ADD THE BOTTOM VERTEX
	output.position[p++] = 0;
	output.position[p++] = 1.0f;
	output.position[p++] = 0.0f;

	// THERE ARE ACTUALLY rings-1 RINGS OF VERTICES.
	// START AT 1, OR YOU WILL ADD THE TOP-VERTEX AGAIN
	for (int r = 1; r < rings; r++)
	{
		float theta = ((float)(r) / (rings))*PI;

		// NUMBER OF SEGMENTS IS JUST THE NUMBER OF VERTICES ON A RING
		for (int s = 0; s < segments; s++)
		{
			float phi = ((float)(s) / segments) * 2 * PI; // azimuth goes around 0 .. 2*PI
			//printf("lon is %f, lat if %f\n", lon, lat);
			// OPENGL IS Y UP, Z FORWARD
			float x = sin(theta)*cos(phi);
			float z = sin(theta)*sin(phi);
			float y = cos(theta);

			//printf("x: %f, y: %f, z: %f\n", x, -z, y);

			// TO-DO: CHECK CORRECTNESS OF THESE
			output.position[p++] = x;
			output.position[p++] = y;
			output.position[p++] = z;
		}
	}

	// ADD THE LAST, BOTTOM VERTEX
	output.position[p++] = 0;
	output.position[p++] = -1.0f;
	output.position[p++] = 0;

	//	for (int i = 0; i < in_num_vertices; i++)
	//	{
	//		printf("x: %f y: %f z: %f\n", output.position[i * 3 + 0], output.position[i * 3 + 1], output.position[i * 3 + 2]);
	//	}
	// GENERATE THE TRIANGLES

	// Triangle fan at top
	int t = 0;
	int v1 = 0; int v2 = 1; int v3 = 2;
	for (int i = 0; i < segments; i++)
	{
		output.triangles[t++] = v1;
		output.triangles[t++] = v2;
		output.triangles[t++] = v3;
		v2++; v3++;
		if (v3 > segments) v3 = 1;
	}

	//// RINGS
	for (int r = 0; r < rings - 2; r++)
	{
		// V1 | V2
		// V3 | V4

		v1 = r*segments + 1;
		v2 = r*segments + 1 + 1;
		v3 = (r + 1)*segments + 1;
		int v4 = (r + 1)*segments + 1 + 1;

		for (int s = 0; s < segments; s++)
		{
			// UPPER TRIANGLE
			output.triangles[t++] = v1;
			output.triangles[t++] = v3;
			output.triangles[t++] = v2;

			// LOWER TRIANGLE
			output.triangles[t++] = v2;
			output.triangles[t++] = v3;
			output.triangles[t++] = v4;

			v1++; v2++; v3++; v4++;

			if (v2 > r*segments + segments) v2 = r*segments + 1;
			if (v4 > (r + 1)*segments + segments) v4 = (r + 1)*segments + 1;
		}
	}

	// Triangle fan at the bottom
	v1 = in_num_vertices - 1 - segments;
	v2 = in_num_vertices - 1 - segments + 1;
	v3 = in_num_vertices - 1;

	for (int i = 0; i < segments; i++)
	{
		output.triangles[t++] = v1;
		output.triangles[t++] = v2;
		output.triangles[t++] = v3;
		v1++; v2++;
		if (v2 > in_num_vertices - 1 - 1) v2 = in_num_vertices - 1 - segments;
	}

	// GENERATE THE UV

	/*float2 direction_to_equirectangular_range(float3 dir, float4 range)
	{
	if (is_zero(dir))
	return make_float2(0.0f, 0.0f);

	float u = (atan2f(dir.y, dir.x) - range.y) / range.x;
	float v = (acosf(dir.z / len(dir)) - range.w) / range.z;

	return make_float2(u, v);
	}*/

	int u = 0;

	for (int v = 0; v < in_num_vertices; v++)
	{
		int pos = v * 3;
		float dir_x = output.position[pos + 0];
		float dir_y = output.position[pos + 2] * -1.0;
		float dir_z = output.position[pos + 1] * -1.0;

		//float length_dir = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);

		float r = atan2f(sqrtf(dir_y*dir_y + dir_z*dir_z), dir_x) / 3.14159;
		float phi = atan2f(dir_z, dir_y);

		output.uv[u++] = r * cosf(phi) + 0.5f;
		output.uv[u++] = r * sinf(phi) + 0.5f;




		/*float range_x = -1.570796;
		float range_y = 1.570796;
		float range_z = -1.570796;
		float range_w = 1.570796;

		if (dir_x == 0.0 && dir_y == 0.0 && dir_z == 0.0)
		{
			output.uv[u++] = 0.0;
			output.uv[u++] = 0.0;
		}
		else
		{
			float uv_u = (atan2f(dir_y, dir_x) - range_y) / range_x;
			float uv_v = (acosf(dir_z / length_dir) - range_w) / range_z;
			output.uv[u++] = uv_u*0.5 + 0.5;
			output.uv[u++] = uv_v*0.5 + 0.5;
		}*/
	}
	printf("Num uvs: %i\n");

	output.genOpenGLBuffers();

	return output;

}

struct OCamCalibData
{
	float* coeffs;
	int polysize;
	float center_x;
	float center_y;
	float width;
	float height;
};

uvMeshGL generateUVSphereOcam(int rings, int segments, OCamCalibData calibData, float uvOffset)
{
	int in_num_vertices = segments * (rings - 1) + 2; int in_num_triangles = segments*rings * 2 - (segments * 2);
	std::cout << "Num vertices: " << in_num_vertices << " and tris: " << in_num_triangles << std::endl;

	uvMeshGL output(in_num_vertices, in_num_triangles);

	int p = 0;
	int vertexSize = sizeof(float) * 3;

	// ADD THE BOTTOM VERTEX
	output.position[p++] = 0;
	output.position[p++] = 1.0f;
	output.position[p++] = 0.0f;

	// THERE ARE ACTUALLY rings-1 RINGS OF VERTICES.
	// START AT 1, OR YOU WILL ADD THE TOP-VERTEX AGAIN
	for (int r = 1; r < rings; r++)
	{
		float theta = ((float)(r) / (rings))*PI;

		// NUMBER OF SEGMENTS IS JUST THE NUMBER OF VERTICES ON A RING
		for (int s = 0; s < segments; s++)
		{
			float phi = ((float)(s) / segments) * 2 * PI; // azimuth goes around 0 .. 2*PI
			//printf("lon is %f, lat if %f\n", lon, lat);
			// OPENGL IS Y UP, Z FORWARD
			float x = sin(theta)*cos(phi);
			float z = sin(theta)*sin(phi);
			float y = cos(theta);

			//printf("x: %f, y: %f, z: %f\n", x, -z, y);

			// TO-DO: CHECK CORRECTNESS OF THESE
			output.position[p++] = x;
			output.position[p++] = y;
			output.position[p++] = z;
		}
	}

	// ADD THE LAST, BOTTOM VERTEX
	output.position[p++] = 0;
	output.position[p++] = -1.0f;
	output.position[p++] = 0;

	//	for (int i = 0; i < in_num_vertices; i++)
	//	{
	//		printf("x: %f y: %f z: %f\n", output.position[i * 3 + 0], output.position[i * 3 + 1], output.position[i * 3 + 2]);
	//	}
	// GENERATE THE TRIANGLES

	// Triangle fan at top
	int t = 0;
	int v1 = 0; int v2 = 1; int v3 = 2;
	for (int i = 0; i < segments; i++)
	{
		output.triangles[t++] = v1;
		output.triangles[t++] = v2;
		output.triangles[t++] = v3;
		v2++; v3++;
		if (v3 > segments) v3 = 1;
	}

	//// RINGS
	for (int r = 0; r < rings - 2; r++)
	{
		// V1 | V2
		// V3 | V4

		v1 = r*segments + 1;
		v2 = r*segments + 1 + 1;
		v3 = (r + 1)*segments + 1;
		int v4 = (r + 1)*segments + 1 + 1;

		for (int s = 0; s < segments; s++)
		{
			// UPPER TRIANGLE
			output.triangles[t++] = v1;
			output.triangles[t++] = v3;
			output.triangles[t++] = v2;

			// LOWER TRIANGLE
			output.triangles[t++] = v2;
			output.triangles[t++] = v3;
			output.triangles[t++] = v4;

			v1++; v2++; v3++; v4++;

			if (v2 > r*segments + segments) v2 = r*segments + 1;
			if (v4 > (r + 1)*segments + segments) v4 = (r + 1)*segments + 1;
		}
	}

	// Triangle fan at the bottom
	v1 = in_num_vertices - 1 - segments;
	v2 = in_num_vertices - 1 - segments + 1;
	v3 = in_num_vertices - 1;

	for (int i = 0; i < segments; i++)
	{
		output.triangles[t++] = v1;
		output.triangles[t++] = v2;
		output.triangles[t++] = v3;
		v1++; v2++;
		if (v2 > in_num_vertices - 1 - 1) v2 = in_num_vertices - 1 - segments;
	}

	// GENERATE THE UV

	/*float2 direction_to_equirectangular_range(float3 dir, float4 range)
	{
	if (is_zero(dir))
	return make_float2(0.0f, 0.0f);

	float u = (atan2f(dir.y, dir.x) - range.y) / range.x;
	float v = (acosf(dir.z / len(dir)) - range.w) / range.z;

	return make_float2(u, v);
	}*/

	int u = 0;

	/*float coeffs[] = {12, 592.066480, 287.229079, -28.625608, 59.370156, 10.409210, 4.432917, 22.650670, -15.327758, -17.963767, 14.120643, 15.916149, 3.694360 };
	int polysize = 13;

	float center_y = 550.228185;
	float center_x = 673.795866;
	float affine_c = 0.998653;
	float affine_d = -0.162848;
	float affine_e = 0.159344;
	float height = 960;
	float width = 1280;*/

	for (int v = 0; v < in_num_vertices; v++)
	{
		int pos = v * 3;
		//float dir_x = output.position[pos + 0];
		//float dir_y = output.position[pos + 1] *-1.0;

		float dir_x = output.position[pos + 0] * -1.0;
		float dir_y = output.position[pos + 1];
		float dir_z = output.position[pos + 2] * -1.0;

		if (dir_z < 0.0)
		{
			output.uv[u++] = 0.0;
			output.uv[u++] = 0.0;
		}
		else {
			//float length_dir = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);

			float norm = sqrt(dir_x*dir_x + dir_y*dir_y);
			float theta = atan(-dir_z / norm);

			float rho = 0.0;

			//for (int i = 0; i > polysize; i++)
			for (int i = calibData.polysize - 1; i >= 0; i--)
				rho = rho * theta + calibData.coeffs[i];

			float uu = dir_x / norm * rho;
			float vv = dir_y / norm * rho;

			float v0 = calibData.center_y;
			float u0 = calibData.center_x;

			float c = 1;
			float d = 0;
			float e = 0;

			output.uv[u++] = (uu*c + vv*d + u0) / calibData.width * 0.5 + uvOffset;
			output.uv[u++] = (uu*e + vv + v0) / calibData.height;

			//output.uv[u++] = (uu*c + vv*d + u0) / calibData.width;
			//output.uv[u++] = (uu*e + vv + v0) / calibData.height;
		}

	}
	printf("Num uvs: %i\n");

	output.genOpenGLBuffers();

	return output;

}
