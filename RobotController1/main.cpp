/*
Reference Frames:
Object: The object being rendered (the Shape in this example) relative to its own origin
World:  Global reference frame
Body:   Controlled by keyboard and mouse
Head:   Controlled by tracking (or fixed relative to the body for non-VR)0
Camera: Fixed relative to the head. The camera is the eye.
*/

// REMEMBER TO COMPILE THE SHADERS TO BINARY
// OR THEY WILL BE ACCESSIBLE AS PLAIN TEXT!

// Uncomment to add VR support
//#define _VR

////////////////////////////////////////////////////////////////////////////////
#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS
#include "asio.hpp"

#include <thread>

#include "matrix.h"
#include "OpenGLutils.h"

#ifdef _VR
#   include "OpenVRutils.h"
#endif

#include "shaderCompilation.h"
#include "openGLMeshes.h"
#include "simple_io.h"

// THE VIDEO FRAME
ucharImage cell_image;

////////////////////////////////////////////////////////////////////////////////
// THE GSTREAMER LIBS
extern "C" {
	#include <gst/gst.h>
	#include <gst/app/gstappsink.h>
	#include <stdbool.h>
	#include <stdio.h>
}
#include "gstreamer_functions.h"

////////////////////////////////////////////////////////////////////////////////

GLFWwindow* window = nullptr;

#ifdef _VR
vr::IVRSystem* hmd = nullptr;
#endif

#ifndef Shape	
#   define Shape Cube
#endif


using asio::ip::tcp;

int main(const int argc, const char* argv[]) {
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	uint32_t framebufferWidth = 1280, framebufferHeight = 720;

#   ifdef _VR
	const int numEyes = 2;
	hmd = initOpenVR(framebufferWidth, framebufferHeight);
	assert(hmd);
#   else
	const int numEyes = 1;
#   endif

	const int windowHeight = framebufferHeight;
	const int windowWidth = (framebufferWidth * windowHeight) / framebufferHeight;

	// INITIALIZE OPENGL
	window = initOpenGL(windowWidth, windowHeight, "Lightfield Player");

	// INITIALIZE THE SERVER COMMUNICATION
	asio::io_context io_context;

	tcp::resolver resolver(io_context);
	tcp::resolver::results_type endpoints =
		resolver.resolve("206.189.30.103", "5000");

	tcp::socket socket(io_context);
	asio::connect(socket, endpoints);

	asio::error_code error;

	// CAMERA INITIAL POSITION
	//Vector3 bodyTranslation(-0.15, -0.113, 3.0);
	Vector3 bodyTranslation(0.0f, 0.0f, 0.0f);

	Vector3 bodyRotation;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	standardFrameBuffer framebuffer(framebufferWidth, framebufferHeight, numEyes); // final render target

	GLuint trilinearSampler = GL_NONE;
	{
		glGenSamplers(1, &trilinearSampler);
		glSamplerParameteri(trilinearSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(trilinearSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(trilinearSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(trilinearSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Load the cell texture
	//ucharImage cell_image = stbLoadImage("cameron.jpg");

	//ucharImage cell_image_rgba = ucharRGBtoRGBA(cell_image);

	//GLuint celltex;
	//glGenTextures(1, &celltex);
	//glBindTexture(GL_TEXTURE_2D, celltex);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, loaded_splashscreen.width, loaded_splashscreen.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loaded_splashscreen.pixel);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, cell_image_rgba.width, cell_image_rgba.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, cell_image_rgba.pixel);
	//printf("ping!\n"); // program doesn't execute without this... wtf

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	standardShader unlit_shader("unlit.vrt", "fisheye.pix");

	const GLint unlit_positionAttribute = glGetAttribLocation(unlit_shader.shader, "position");
	const GLint unlit_uvAttribute = glGetAttribLocation(unlit_shader.shader, "texCoord");
	const GLint unlit_colorTextureUniform = glGetUniformLocation(unlit_shader.shader, "colorTexture");
	printf("Ping!\n");

	//uvMeshGL fullscreenQuad = generateQuad();

	//OCamCalibData leftCalibData;
	//leftCalibData.center_y = 537.809290;
	//leftCalibData.center_x = 677.360829;
	//leftCalibData.height = 960;
	//leftCalibData.width = 1280;
	//leftCalibData.polysize = 12;
	//leftCalibData.coeffs = (float*)malloc(sizeof(float)*leftCalibData.polysize);
	//float leftCalibCoeffs[12] = { 605.790428, 300.173872, -25.630741, 59.753381, 13.066510, 3.343557, 19.851506, -11.427071, -13.822894, 12.982593, 13.931213, 3.207146 };
	//for (int i = 0; i < leftCalibData.polysize; i++) leftCalibData.coeffs[i] = leftCalibCoeffs[i];

	//OCamCalibData rightCalibData;
	//rightCalibData.center_y = 537.809290;
	//rightCalibData.center_x = 677.360829;
	//rightCalibData.height = 960;
	//rightCalibData.width = 1280;
	//rightCalibData.polysize = 12;
	//rightCalibData.coeffs = (float*)malloc(sizeof(float)*rightCalibData.polysize);
	//float tempRightCoeffs[12] = { 605.790428, 300.173872, -25.630741, 59.753381, 13.066510, 3.343557, 19.851506, -11.427071, -13.822894, 12.982593, 13.931213, 3.207146 };
	//for (int i = 0; i < rightCalibData.polysize; i++) rightCalibData.coeffs[i] = tempRightCoeffs[i];

	/*OCamCalibData leftCalibData;
	leftCalibData.center_y = 1073.248894;
	leftCalibData.center_x = 1077.740811;
	leftCalibData.height = 2048;
	leftCalibData.width = 2048;
	leftCalibData.polysize = 12;
	leftCalibData.coeffs = (float*)malloc(sizeof(float)*leftCalibData.polysize);
	float leftCalibCoeffs[12] = { 983.668474, 574.269638, -10.194424, 79.666736, 46.977084, -4.612648, 15.766929, 20.238338, -1.751897, -4.743670, 0.239001, 0.488726 };

	for (int i = 0; i < leftCalibData.polysize; i++) leftCalibData.coeffs[i] = leftCalibCoeffs[i];

	OCamCalibData rightCalibData;
	rightCalibData.center_y = 998.449863;
	rightCalibData.center_x = 1010.77310;
	rightCalibData.height = 2048;
	rightCalibData.width = 2048;
	rightCalibData.polysize = 11;
	rightCalibData.coeffs = (float*)malloc(sizeof(float)*rightCalibData.polysize);
	float tempRightCoeffs[11] = { 991.300300, 586.381273, - 0.464130, 80.502435, 46.458477, - 1.106387, 17.021867, 20.604190, - 1.330144, - 6.559736, - 1.668794 };
	for (int i = 0; i < rightCalibData.polysize; i++) rightCalibData.coeffs[i] = tempRightCoeffs[i];*/

	OCamCalibData leftCalibData;
	leftCalibData.center_y = 442.876513;
	leftCalibData.center_x = 652.818122;
	leftCalibData.height = 960;
	leftCalibData.width = 1280;
	leftCalibData.polysize = 9;
	leftCalibData.coeffs = (float*)malloc(sizeof(float)*leftCalibData.polysize);
	float leftCalibCoeffs[9] = { 714.548586, 423.433640, 8.101962, 59.054346, 23.938875, - 17.463169, - 11.449815, 1.158872, 1.243106 };

	for (int i = 0; i < leftCalibData.polysize; i++) leftCalibData.coeffs[i] = leftCalibCoeffs[i];

	OCamCalibData rightCalibData;
	rightCalibData.center_y = 444.986716;
	rightCalibData.center_x = 658.122973;
	rightCalibData.height = 960;
	rightCalibData.width = 1280;
	rightCalibData.polysize = 9;
	rightCalibData.coeffs = (float*)malloc(sizeof(float)*rightCalibData.polysize);
	float tempRightCoeffs[9] = { 716.309346, 420.575126, 5.441282, 59.448713, 21.625563, - 20.866173, - 12.239077, 1.658369, 1.432656 };
	for (int i = 0; i < rightCalibData.polysize; i++) rightCalibData.coeffs[i] = tempRightCoeffs[i];
	uvMeshGL eyeSpheres[2] = { generateUVSphereOcam(256, 128, leftCalibData, 0.5), generateUVSphereOcam(256, 128, rightCalibData, 0.0) };

	//uvMeshGL eyeSpheres[2] = { generateQuad(), generateQuad() };



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("Starting Gstreamer!\n");

	std::thread t1(gstreamer_main);

	GLuint celltex;
	glGenTextures(1, &celltex);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#   ifdef _VR
	vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
#   endif

	double lastvideoTime = glfwGetTime();

	// Main loop:
	int timer = 0;

	GLint curDisplayFrame = 0;

	// TIME STUFF
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// START OF PLAYER LOOP
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float framebufferWidth_f = framebufferWidth;
	float framebufferHeight_f = framebufferHeight;

	const float nearPlaneZ = -0.1f;
	const float farPlaneZ = -200.0f;
	const float verticalFieldOfView = 45.0f * PI / 180.0f;
	const float horizontalFieldOfView = 2 * atan((framebufferWidth_f / framebufferHeight_f)*tan(verticalFieldOfView / 2));

	double previousTime = glfwGetTime();
	uint32_t speed = 256;

	////////////////////////////////////////////////////////////////////////
	while (!glfwWindowShouldClose(window)) {
		assert(glGetError() == GL_NONE);

		// GET FRAMERATE
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1 sec ago
			// printf and reset timer
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
			//std::cout << "Current position is: " << bodyTranslation[0] << " " << bodyTranslation[1] << " " << bodyTranslation[2] << std::endl;
			//std::cout << "Current rotation is: " << bodyRotation[0] << " " << bodyRotation[1] << " " << bodyRotation[2] << std::endl;
		}

		Matrix4x4 eyeToHead[numEyes], projectionMatrix[numEyes], headToBodyMatrix;
#       ifdef _VR
		getEyeTransformations(hmd, trackedDevicePose, nearPlaneZ, farPlaneZ, headToBodyMatrix.data, eyeToHead[0].data, eyeToHead[1].data, projectionMatrix[0].data, projectionMatrix[1].data);
#       else
		projectionMatrix[0] = Matrix4x4::perspective(float(framebufferWidth), float(framebufferHeight), nearPlaneZ, farPlaneZ, verticalFieldOfView);
#       endif

		// printf("float nearPlaneZ = %f, farPlaneZ = %f; int width = %d, height = %d;\n", nearPlaneZ, farPlaneZ, framebufferWidth, framebufferHeight);

		const Matrix4x4& bodyToWorldMatrix =
			Matrix4x4::translate(bodyTranslation) *
			Matrix4x4::roll(bodyRotation.z) *
			Matrix4x4::yaw(bodyRotation.y) *
			Matrix4x4::pitch(bodyRotation.x);

		const Matrix4x4& headToWorldMatrix = bodyToWorldMatrix * headToBodyMatrix;
		//const Matrix4x4& objectToWorldMatrix = Matrix4x4::translate(0.0f, 0.0f, 0.0f);


		// GET THE VIDEO FRAME
		glBindTexture(GL_TEXTURE_2D, celltex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, cell_image.width, cell_image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, cell_image.pixel);

		for (int eye = 0; eye < numEyes; ++eye)
		{
			////////////////////////////////////////////////////////////////////////
			// GENERATE THE CAMERA MATRICES FOR EACH EYE
			const Matrix4x4& cameraToWorldMatrix = headToWorldMatrix * eyeToHead[eye];
			const Vector4& cameraPosition = cameraToWorldMatrix.col(3);
			Matrix4x4& objectToWorldMatrix = Matrix4x4::translate(cameraPosition.x, cameraPosition.y, cameraPosition.z);



			const Matrix4x4& modelViewProjectionMatrix = projectionMatrix[eye] * cameraToWorldMatrix.inverse() * objectToWorldMatrix;

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.buffer[eye]);
			glViewport(0, 0, framebufferWidth, framebufferHeight);

			glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// FIRST PASS - THE ENDPOINT

			
			// Draw a mesh
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			//glCullFace(GL_FRONT);

			glUseProgram(unlit_shader.shader);

			// in position
			glBindBuffer(GL_ARRAY_BUFFER, eyeSpheres[eye].positionBuffer);
			glVertexAttribPointer(unlit_positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(unlit_positionAttribute);


			// in uv
			glBindBuffer(GL_ARRAY_BUFFER, eyeSpheres[eye].uvBuffer);
			glVertexAttribPointer(unlit_uvAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(unlit_uvAttribute);

			// indexBuffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eyeSpheres[eye].indexBuffer);


			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, celltex);
			glBindSampler(0, trilinearSampler);
			glUniform1i(unlit_colorTextureUniform, 0);

			// Other uniforms in the interface block
			{
				glBindBufferBase(GL_UNIFORM_BUFFER, unlit_shader.uniformBindingPoint, unlit_shader.uniformBlock);

				GLubyte* ptr = (GLubyte*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
				memcpy(ptr + unlit_shader.uniformOffset[0], objectToWorldMatrix.data, sizeof(objectToWorldMatrix));

				memcpy(ptr + unlit_shader.uniformOffset[1], modelViewProjectionMatrix.data, sizeof(modelViewProjectionMatrix));
				memcpy(ptr + unlit_shader.uniformOffset[2], &cameraPosition.x, sizeof(Vector3));
				glUnmapBuffer(GL_UNIFORM_BUFFER);
			}

			glDrawElements(GL_TRIANGLES, eyeSpheres[eye].num_indices, GL_UNSIGNED_INT, 0);
			
#           ifdef _VR
			{
				const vr::Texture_t tex = { reinterpret_cast<void*>(intptr_t(framebuffer.colorRenderTarget[eye])), vr::API_OpenGL, vr::ColorSpace_Gamma };
				vr::VRCompositor()->Submit(vr::EVREye(eye), &tex);
			}
#           endif


		} // for each eye


		////////////////////////////////////////////////////////////////////////
#       ifdef _VR
		// Tell the compositor to begin work immediately instead of waiting for the next WaitGetPoses() call
		vr::VRCompositor()->PostPresentHandoff();
#       endif


		// Mirror to the window
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
		glViewport(0, 0, windowWidth, windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);

		// Display what has been drawn on the main window
		glfwSwapBuffers(window);

		// Check for events
		glfwPollEvents();

		// Handle events
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}

		// HANDLE THE ROBOT WASD MOVEMENT
		std::string to_send = "0,0";

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) { to_send = "0," + std::to_string(int(speed)); }
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)) { to_send = "180," + std::to_string(int(speed)); }
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) { to_send = "90," + std::to_string(int(speed)); }
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) { to_send = "270," + std::to_string(int(speed)); }
		if ((GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE)) || (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Z))) { to_send = "0,0"; }

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_R)) { to_send = "0," + std::to_string(int(speed)); }


		if (speed > 1000) speed = 1000;
		if (speed < 0) speed = 0;

		nbFrames++;
		if (currentTime - lastTime >= 0.2){ // If last printf() was more than 100ms ago
			if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_R)) { speed += 32; printf("Speed: %i\n", speed); }
			if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_F)) { speed -= 32; printf("Speed: %i\n", speed); }

			// printf and reset timer
			//printf("%f ms/frame\n", 500 / double(nbFrames));
			//nbFrames = 0;
			lastTime = currentTime;

			asio::write(socket, asio::buffer(to_send), error);
		}

		// WASD keyboard movement
		//double newTime = glfwGetTime();
		//double frameTime = newTime - previousTime;
		//previousTime = newTime;

		//const float cameraMoveSpeedPerSecond = 1.0f;
		//const float apertureSpeed = 0.05f;
		//const float focusSpeed = 1.0;

		//float cameraMoveSpeed = cameraMoveSpeedPerSecond * frameTime;
		//float apertureStep = apertureSpeed * frameTime;
		//float focusStep = focusSpeed * frameTime;

		// WASD IS DEACTIVATED FOR THE IN-GL CAMERA
		//if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) { bodyTranslation += Vector3(headToWorldMatrix * Vector4(0, 0, -cameraMoveSpeed, 0)); }
		//if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)) { bodyTranslation += Vector3(headToWorldMatrix * Vector4(0, 0, +cameraMoveSpeed, 0)); }
		//if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) { bodyTranslation += Vector3(headToWorldMatrix * Vector4(-cameraMoveSpeed, 0, 0, 0)); }
		//if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) { bodyTranslation += Vector3(headToWorldMatrix * Vector4(+cameraMoveSpeed, 0, 0, 0)); }
		//if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_C)) { bodyTranslation.y -= cameraMoveSpeed; }
		//if ((GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE)) || (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Z))) { bodyTranslation.y += cameraMoveSpeed; }
		//if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_M)) { bodyTranslation.x = 0.0; bodyTranslation.y = 0.0; bodyTranslation.z = 0.0; }

		// Keep the camera above the ground
		//if (bodyTranslation.y < -2.0f) { bodyTranslation.y = -2.0f; }

		static bool inDrag = false;
		const float cameraTurnSpeed = 0.005f;
		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
			static double startX, startY;
			double currentX, currentY;

			glfwGetCursorPos(window, &currentX, &currentY);
			if (inDrag) {
				bodyRotation.y -= float(currentX - startX) * cameraTurnSpeed;
				bodyRotation.x -= float(currentY - startY) * cameraTurnSpeed;
			}
			inDrag = true; startX = currentX; startY = currentY;
		}
		else {
			inDrag = false;
		}

		++timer;
	}

#   ifdef _VR
	if (hmd != nullptr) {
		vr::VR_Shutdown();
	}
#   endif

	// Close the GL context and release all resources
	glfwTerminate();
	//Join the thread with the main thread
	//t1.join();

	return 0;
}

#ifdef _WINDOWS
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw) {
	return main(0, nullptr);
}
#endif
