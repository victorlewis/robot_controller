#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma warning(disable:4996)

#include "intrin.h"

struct header
{
	char initial[2];
	int version;
	int cams;
	int numtriangles;
};

struct mesh
{
	int cams;
	int numtriangles;
	float *position;
	float *rotation;
	int *triangles;
};

mesh readMesh(char *filename)
{
	mesh outputmesh;

	FILE *filepointer;

	header inputheader; // read the initial bytes directly into a well ordered struct

	filepointer = fopen(filename, "rb"); // open for writing in binary
	if (!filepointer) printf("An error occured when opening file."); // error checking

	fread(&inputheader, sizeof(header), 1, filepointer); // read header

	outputmesh.cams = inputheader.cams;
	outputmesh.numtriangles = inputheader.numtriangles;


	outputmesh.position = (float*)malloc(sizeof(float) * 3 * outputmesh.cams);
	outputmesh.rotation = (float*)malloc(sizeof(float) * 4 * outputmesh.cams);
	outputmesh.triangles = (int*)malloc(sizeof(int) * 3 * outputmesh.numtriangles);

	fread(outputmesh.position, sizeof(float) * 3, outputmesh.cams, filepointer); // an array is already a pointer
	fread(outputmesh.rotation, sizeof(float) * 4, outputmesh.cams, filepointer); // no need to do &inputmesh.rotation
	fread(outputmesh.triangles, sizeof(int) * 3, outputmesh.numtriangles, filepointer);
	fclose(filepointer);

	return outputmesh;
	// TODO: FREE ALL THAT MEMORY
}

struct ucharImage
{
	unsigned char* pixel;
	int width, height;
};

ucharImage ucharRGBtoRGBA(ucharImage inputRGB)
{
	ucharImage outputRGBA;
	printf("transform: width: %i height:%i \n", inputRGB.width, inputRGB.height);

	outputRGBA.pixel = (unsigned char*)calloc(inputRGB.width*inputRGB.height * 4, sizeof(unsigned char));
	outputRGBA.width = inputRGB.width;
	outputRGBA.height = inputRGB.height;

	int p = 0;
	for (int i = 0; i < inputRGB.height*inputRGB.width * 3; i += 3)
	{
		outputRGBA.pixel[p + 0] = inputRGB.pixel[i + 0];
		outputRGBA.pixel[p + 1] = inputRGB.pixel[i + 1];
		outputRGBA.pixel[p + 2] = inputRGB.pixel[i + 2];
		outputRGBA.pixel[p + 3] = 1;
		p += 4;
	}

	return outputRGBA;
}


ucharImage stbLoadImage(const char* path)
{
	// STB IMAGE TEST
	int stb_width, stb_height, stb_bytesPerPixel;
	ucharImage img;

	img.pixel = stbi_load(path, &stb_width, &stb_height, &stb_bytesPerPixel, 0);
	img.width = stb_width;
	img.height = stb_height;

	return img;

	std::cout << "Loaded image with STB library." << std::endl;
}

struct ucharImage3D
{
	unsigned char* pixel;
	int width, height, depth;
};

ucharImage3D stbLoadImage3D(std::string path, std::string extension, int depth)
{
	ucharImage3D out3D;

	out3D.depth = depth;

	// CHECK THE SIZE
	{
		std::ostringstream out;
		out << std::internal << std::setfill('0') << std::setw(4) << 0;
		std::string outstring = path + out.str() + extension;
		const char *outchar = outstring.c_str();

		int stb_width, stb_height, stb_bytesPerPixel;

		ucharImage img;

		img.pixel = stbi_load(outchar, &stb_width, &stb_height, &stb_bytesPerPixel, 0);
		img.width = stb_width;
		img.height = stb_height;

		out3D.width = stb_width;
		out3D.height = stb_height;
	}

	out3D.pixel = (unsigned char*)malloc(sizeof(unsigned char)*out3D.width*out3D.height*depth);

	// STB IMAGE TEST
	for (int z = 0; z < depth; z++)
	{
		std::ostringstream out;
		out << std::internal << std::setfill('0') << std::setw(4) << z;
		std::string outstring = path + out.str() + extension;
		const char *outchar = outstring.c_str();

		int stb_width, stb_height, stb_bytesPerPixel;

		ucharImage img;

		img.pixel = stbi_load(outchar, &stb_width, &stb_height, &stb_bytesPerPixel, 0);
		img.width = stb_width;
		img.height = stb_height;

		memcpy(out3D.pixel + z*(stb_width*stb_height), img.pixel, stb_width*stb_height*sizeof(unsigned char));
	}
	return out3D;
}