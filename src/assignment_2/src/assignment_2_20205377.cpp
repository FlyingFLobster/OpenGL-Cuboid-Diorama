// @author Zachary Kidd-Smith / 20205377
// Computer Graphics Assignment 2 Project.
// Large amounts of code learned and taken from the textbook at LearnOpenGL.com

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath> //for sin.
#include "shader.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>

// Shader header files containing raw string literals of shaders. (Might require c++11).
#include "light_cube.vs.h"
#include "light_cube.fs.h"
#include "default_object.vs.h"
#include "default_object.fs.h"

// Function headers.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xXos, double yPos);
unsigned int getTexture(const char* filePath);

// GLOBAL VARIABES:
// Camera Variables.
glm::vec3 cameraPos   = glm::vec3(0.0f, 1.0f,  3.0f); // Start camera up and back a bit from the scene.
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
float pitch = 0.0f;
float yaw = -90.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;

// Delta Time.
float deltaTime = 0.0f; // Time between last and current frames.
float lastFrame = 0.0f; // When the last frame happened.

// Lighting.
glm::vec3 lightPosition(1.2f, 1.0f, 2.0f);
float brightnessModifier = 0.0f;

// Controls.
int fLastKeyStatus = GLFW_RELEASE;
bool lightFollows = true;
int rLastKeyStatus = GLFW_RELEASE;
bool sitAnimation = false;
float sitModifier = 0.0f;
int pLastKeyStatus = GLFW_RELEASE;
bool perspective = true;
int oLastKeyStatus = GLFW_RELEASE;
bool darkish = true;

int main()
{
//-WINDOW SETUP.
	glfwInit(); //Initialize GLFW

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Set version context to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Using core profile (less backwards compatibility.)

	// Make window object.
	// Variables passed in are the window's width, height, and title.
	GLFWwindow* window = glfwCreateWindow(800, 600, "CG Assignment 2 20205377", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window object." << std::endl;
		glfwTerminate();
		return -1;
	}

	// Make the window the main context on the current thread.
	glfwMakeContextCurrent(window);

	// Make the window capture the mouse.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLAD before calling any OpenGL functions.
	// This is done by passing GLAD the function to load the address of the OpenGL function pointers (which are OS-specific.)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Set size of viewport (window that is displayed to the user).
	// Variables in order are: viewport's X location, Y location, width, and height.
	glViewport(0, 0, 800, 600);

	// REGISTER CALLBACK FUNCTIONS.
	// -Should be done after creating window, and before the render loop starts.
	// Set the window resize callback function to framebuffer-size-callback().
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Set the mouse cursor callback function.
	glfwSetCursorPosCallback(window, mouse_callback);


	// Create Shaders from object class.
	Shader defaultShader(default_object_vs, default_object_fs);
	Shader lightCubeShader(light_cube_vs, light_cube_fs);

	// Vertices of a cube.
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	    };
	//Make VBO (Vertex Buffer Object) for storing vertices and sending to the gpu in big batches.
	unsigned int VBO;
	glGenBuffers(1, &VBO); //Generates 1 buffer object, and stores it in VBO.

	//All these settings have to be called everytime we want to draw an object, which is cumbersome,
	//Luckily VAO (Vertex Array Object)'s can be used to store these settings and repeat them.
	//Additionally, VAO's are required by Core OpenGL.
	//The VAO will record a bunch of the following settings for drawing our objects and replay them when we use it.
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Opengl has lots of buffers, The buffer type for a vertex buffer object is GL_ARRAY_BUFFER,
	//so we bind it to that target.
	//Now whenever we make any buffer calls to the GL_ARRAY_BUFFER target, our variable VBO will be configured.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Copy vertex data into the buffer's memory (Will go into VBO since we targeted that with GL_ARRAY_BUFFER).
	//Arguments are: target, data size, data, and how we want the gpu to manage the data, options are:
	//
	//GL_STREAM_DATA: the data is set only once and used by the gpu only a few times.
	//GL_STATIC_DRAW: the data is set only once and used many times.
	//GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	//Tell opengl how to interpret the vertex data (Format in the VBO).
	//Arguments:
	//1: which vertex attribute to configure (location = 0 in vertex shader).
	//2: Size of vertex attribute (vector3 so 3 values).
	//3: Type of data (vectors in glsl consist of floating point values).
	//4: Normalize data yes/no (used to normalize integer values, but we aren't using those).
	//5: The stride length between vertex attributes.
	//6: Offset of where the position data begins in the buffer.
	
	// POSITION ATTRIBUTE.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //SHADERS: Stride changed to 6.
	glEnableVertexAttribArray(0); //Takes in vertex attribute location, and enables that vertex attribute.

	// NORMAL ATTRIBUTE.
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// TEXTURE COORDINATE ATTRIBUTE.
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// New VAO for lighting object.
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Set texture sampling types.
	// S, T, and R corresponed to the texture coordinate's X, Y, and Z axis.
	// Only need two here since we are just working with 2D textures.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	// This sets the texture border colour for when using the Clamp to border sampling.
	float borderColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Texture Filtering for when an object is large and a texture is low resoluton:
	// Two most important options are GL_NEAREST for nearest neighbour,
	// and GL_LINEAR for bi/linear filtering.
	//
	// Linear gives a more blurry/realistic output, but Nearest can be good for a more
	// pixellated look.
	//
	// Which filtering method used can be set differently for both upwards and downwards
	// scaling of the textures.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	// Mipmaps: Like culling distance for texture quality, when an object is smaller or
	// farther away, a smaller version of its texture is used to save on both memory
	// and to remove artifacting from scaling the texture in real time.
	//
	// OpenGL can generate mipmaps of textures for us so we don't have to do it manually.
	//
	// When switching between mipmap layers when getting closer/further away from an object,
	// artifacting can occur, which is why it is possible to also filter between different
	// levels of mipmaps using Nearest or Linear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load all textures for the scene.
	unsigned int texture_grass = getTexture(FileSystem::getPath("resources/assignment_2_textures/m10_base_floor_ground_02.png").c_str());
	unsigned int texture_stone = getTexture(FileSystem::getPath("resources/assignment_2_textures/m10_base_wall_03.png").c_str());
	unsigned int texture_sky = getTexture(FileSystem::getPath("resources/assignment_2_textures/m10_sky.png").c_str());
	unsigned int texture_tree = getTexture(FileSystem::getPath("resources/assignment_2_textures/m10_01_tree_01.png").c_str());
	unsigned int texture_wood_planks = getTexture(FileSystem::getPath("resources/assignment_2_textures/m10_wood_passage_L.png").c_str());
	unsigned int texture_chain = getTexture(FileSystem::getPath("resources/assignment_2_textures/chain.jpg").c_str());
	unsigned int texture_stairs = getTexture(FileSystem::getPath("resources/assignment_2_textures/m10_stair.png").c_str());
	unsigned int texture_bones = getTexture(FileSystem::getPath("resources/assignment_2_textures/bonepile.jpg").c_str());
	unsigned int texture_rust = getTexture(FileSystem::getPath("resources/assignment_2_textures/rust.jpg").c_str());
	unsigned int texture_crestfallen_face = getTexture(FileSystem::getPath("resources/assignment_2_textures/crestfallen_face.png").c_str());
	unsigned int texture_plate = getTexture(FileSystem::getPath("resources/assignment_2_textures/platemail.jpg").c_str());
	unsigned int texture_chud_hair = getTexture(FileSystem::getPath("resources/assignment_2_textures/chud_hair.png").c_str());
	unsigned int texture_chud_face = getTexture(FileSystem::getPath("resources/assignment_2_textures/chud_face.png").c_str());

	

	// Projections:
	// Orthographic:
	// Left right bottom top boundaries. size of near and far planes.
	// All coordinates between these X, Y, and Z range values get converted to normalized device coordinates.
	//glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

	// Projection:
	// FOV (Apparently 45 is realistic, but video games use higher (I'm used to 90)).
	// Aspect Ratio (Viewport width / height).
	// Near Plane.
	// Far Plane.
	//glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	// Enable Depth Buffering (Z-Buffer).
	glEnable(GL_DEPTH_TEST);

	// Initialize the View Matrix outside of the render loop.
	glm::mat4 view = glm::mat4(1.0f);

	// COLORS:
	// RGB with each value ranging from 0 to 1.
	//glm::vec3 coral(1.0f, 0.5f, 0.31f);
	//glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

//-RENDER LOOP
	// Each iteration of this loop is a frame.
	while (!glfwWindowShouldClose(window))
	{
		// Time Calculations.
		float time = glfwGetTime();
		deltaTime = time - lastFrame;
		lastFrame = time;

		//CHECK INPUT.
		processInput(window); // Call my input processing function.

		//RENDERING COMMANDS.
		//-Clear Buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //RGBA
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activate lighting shaders.
		defaultShader.use();
		defaultShader.setVec3("light.position", lightPosition);
		defaultShader.setVec3("viewPosition", cameraPos);

		// Material properties.
//		defaultShader.setVec3("material.ambient", 1.0f, 1.0f, 1.0f);
//		defaultShader.setVec3("material.diffuse", 1.0, 1.0f, 1.0f);
//		defaultShader.setVec3("material.specular", 0.5, 0.5f, 0.5f);
//		^ These three are sent in as a texture instead of a plain colour.
		defaultShader.setFloat("material.shininess", 35.0f); // Default shininess, should be set seperately for every texture used.

		// Light properties.
		if (darkish)
		{
			defaultShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
			defaultShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
			defaultShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		}
		else
		{
			defaultShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
			defaultShader.setVec3("light.diffuse", 0.0f, 0.0f, 0.0f);
			defaultShader.setVec3("light.specular", 0.0f, 0.0f, 0.0f);
		}

		defaultShader.setFloat("light.constant", 1.0f); // For light over a distance of 20
		defaultShader.setFloat("light.linear", 0.022f + brightnessModifier);
		defaultShader.setFloat("light.quadratic", 0.20f + brightnessModifier);

		//SHADERS
		//Update the uniform ourColor, used in the fragment shader.
//		float greenValue = sin(time) / 2.0f + 0.5f; // Vary the colour in the range 0.0 to 1.0 using sin.
//		int ourColorLocation = glGetUniformLocation(shaderProgram, "ourColor"); // Get location of the uniform.
//		glUniform4f(ourColorLocation, 0.0f, greenValue, 0.0f, 1.0f); //Set the uniform's value.

		// Transform object over time.
//		glm::mat4 transformation = glm::mat4(1.0f);
		//transformation = glm::translate(transformation, glm::vec3((float)sin(time), (float)sin(time), 0.0f));
		//transformation = glm::rotate(transformation, (float)sin(time), glm::vec3(0.0f, 0.0f, 1.0f));
		//transformation = glm::scale(transformation, glm::vec3(0.5f, 0.5f, 0.0f));

		// Send the transformation matrix to the vertex shader.
		// WARNING: Needs to update every frame or object disappears!!!
//		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
//		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));

		// Model matrix.
		// Transformations to apply to ALL object's vertices totransform them to the global world space.
		glm::mat4 model = glm::mat4(1.0f);
//		model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		// View Matrix.
		// Moving the camera backwards is the same as moving the rest of the scene forwards.
		// Right now the camera is at the origin in world space, so we need to move it back
		// to see things properly.
		//
		// The view matrix will be used to move the entire scene around inverse to where we
		// want the camera to move, giving the illusion of moving the camera through the
		// world.
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		// Projection Matrix, switches between perspective or orphographic when pressing P.
		glm::mat4 projection;
		if (perspective)
		{
			projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
		}
		else
		{
			projection = glm::ortho(0.0f, 4.0f, 0.0f, 3.0f, 0.1f, 100.0f);
		}
		// Coordinates transformations.
//		model = glm::rotate(model, time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

		defaultShader.setMat4("projection", projection);
		defaultShader.setMat4("view", view);
//		defaultShader.setMat4("model", model);

		// Bind the texture.
		//glBindTexture(GL_TEXTURE_2D, texture); // Bind that array so future texture commands affect it.
		
		// Bind VAO for vertex array to be drawn next.
		glBindVertexArray(VAO);

		//Draw primitive using the active shader, vertex attribute config and VBO data bound using the VAO.
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		

		// DRAW ALL THE DIFFERENT OBJECTS.

		// NON-COMPOSITE.
		// ----- Grassy plain.
			// Set Texture.
		glBindTexture(GL_TEXTURE_2D, texture_grass);
		defaultShader.setFloat("material.shininess", 5.0f);
		model = glm::mat4();
		model = glm::scale(model, glm::vec3(5.0f, 0.1f, 5.0f)); // Scale to be a wide flatish area.
		defaultShader.setMat4("model", model); // Send model matrix to shaders.
		glDrawArrays(GL_TRIANGLES, 0, 36); // 36 Vertices in a cube.

		// ----- Skybox.
		glBindTexture(GL_TEXTURE_2D, texture_sky);
		defaultShader.setFloat("material.shininess", 2.0f);
		model = glm::mat4();
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f)); // Scale to be a wide flatish area.
		defaultShader.setMat4("model", model); // Send model matrix to shaders.
		glDrawArrays(GL_TRIANGLES, 0, 36); // 36 Vertices in a cube.

		// COMPOSITE.
		// ----- Tree. (4 parts, trunk + 3 branches.)
		glm::vec3 tree_trunk_scale = glm::vec3(0.4, 4.0f, 0.4f); // Tree trunk.
		glm::vec3 tree_trunk_position = glm::vec3(-2.0f, (tree_trunk_scale.y / 2), -1.0f);

		glm::vec3 tree_branch_scale = glm::vec3(0.1f, 2.0f, 0.1f); // Branches.
		glm::vec3 tree_branch_positions[] = {
			glm::vec3(tree_trunk_position.x, tree_trunk_scale.y - 0.5f, tree_trunk_position.z),
			glm::vec3(tree_trunk_position.x, tree_trunk_scale.y - 1.0f, tree_trunk_position.z),
			glm::vec3(tree_trunk_position.x, tree_trunk_scale.y - 0.2f, tree_trunk_position.z)
		};
		//					^ I'm using the position and scale of the trunk where possible
		//					  so that the trunk and branches are more tightly bound as a
		//					  composite object.

		glm::vec3 tree_branch_rotations[] = {
			glm::vec3(0.9f, 0.4f, 0.0f),
			glm::vec3(0.5f, 0.0f, 0.5f),
			glm::vec3(0.0f, 0.0f, 1.2f)
		};

			// Draw tree trunk.
		glBindTexture(GL_TEXTURE_2D, texture_tree);
		defaultShader.setFloat("material.shininess", 5.0f);
		model = glm::mat4();
		model = glm::translate(model, tree_trunk_position);
		model = glm::scale(model, tree_trunk_scale);
		defaultShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
			// Draw tree branches.
		for (int i = 0; i < 3; i++)
		{
			// Transformations happen in reverse order, scale -> rotate -> translate is the most sane
			// since the scaling and rotation can happen at the origin.
			model = glm::mat4(1.0f);
			model = glm::translate(model, tree_branch_positions[i]);
			model = glm::rotate(model, glm::radians(90.0f), tree_branch_rotations[i]);
			model = glm::scale(model, tree_branch_scale);
			defaultShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ----- Stone Bench (3 parts, top + 2 legs)
		glm::vec3 bench_top_scale = glm::vec3(0.7f, 0.3f, 0.3f);
		glm::vec3 bench_top_positon = glm::vec3(1.5f, 0.5f, -1.0f);

		glm::vec3 bench_legs_scale = glm::vec3(0.1, 0.35, 0.3);
		glm::vec3 bench_legs_positions[] = {
			glm::vec3(bench_top_positon.x - 0.3, 0.2, bench_top_positon.z),
			glm::vec3(bench_top_positon.x + 0.3, 0.2, bench_top_positon.z)
		};

			// Draw bench top.
		glBindTexture(GL_TEXTURE_2D, texture_stairs);
		defaultShader.setFloat("material.shininess", 15.0f);
		model = glm::mat4();
		model = glm::translate(model, bench_top_positon);
		model = glm::scale(model, bench_top_scale);
		defaultShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

			// Draw bench legs.
		for (int i = 0; i < 2; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, bench_legs_positions[i]);
			model = glm::scale(model, bench_legs_scale);
			defaultShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ----- Crestfallen Knight (8 parts: body, legs, arms (double jointed), legs)
		// 	 Sits on the bench, has a looping animation.
		glm::vec3 crestfallen_base_scale = glm::vec3(0.18, 0.6, 0.12); // body, sitting on top of bench.
		glm::vec3 crestfallen_base_position = glm::vec3(bench_top_positon.x, bench_top_positon.y + (bench_top_scale.y + 0.15), bench_top_positon.z + (bench_top_scale.z / 2));

		glm::vec3 crestfallen_legs_scale = glm::vec3(0.07, 0.6, 0.07); // Legs.
		glm::vec3 crestfallen_legs_positions[] = {
			glm::vec3(crestfallen_base_position.x - 0.05, crestfallen_base_position.y - (crestfallen_base_scale.y), crestfallen_base_position.z + 0.04),
			glm::vec3(crestfallen_base_position.x + 0.05, crestfallen_base_position.y - (crestfallen_base_scale.y), crestfallen_base_position.z + 0.04)
		};

		// Two seperate parts for both arms, forearms are in slightly different poses.
		glm::vec3 crestfallen_upperarms_scale = glm::vec3(0.05, 0.4, 0.05);
		glm::vec3 crestfallen_upperarms_positions[] = {
			glm::vec3(crestfallen_base_position.x - 0.12, crestfallen_base_position.y + 0.12, crestfallen_base_position.z + 0.08),
			glm::vec3(crestfallen_base_position.x + 0.12, crestfallen_base_position.y + 0.12, crestfallen_base_position.z + 0.08)
		};
		glm::vec3 crestfallen_upperarms_rotation = glm::vec3(-1.0f, 0.0f, 0.0f);

		
		glm::vec3 crestfallen_forearms_scale = glm::vec3(0.05, 0.3, 0.05);
		glm::vec3 crestfallen_forearms_positions[] = {
			glm::vec3(crestfallen_upperarms_positions[0].x + 0.06, crestfallen_upperarms_positions[0].y, crestfallen_upperarms_positions[0].z + 0.08),
			glm::vec3(crestfallen_upperarms_positions[1].x - 0.06, crestfallen_upperarms_positions[1].y, crestfallen_upperarms_positions[1].z + 0.08)
		};
		glm::vec3 crestfallen_forearms_rotation[] = {
			glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		};

		// Head has a looping animation (Supposed to look like he's got a bad headache from hollowing).
		glm::vec3 crestfallen_head_scale = glm::vec3(0.1, 0.15, 0.1);
		glm::vec3 crestfallen_head_position = glm::vec3(crestfallen_base_position.x, crestfallen_base_position.y + (crestfallen_base_scale.y / 1.8), crestfallen_base_position.z + 0.1);

			// Draw crestfallen base.
		glBindTexture(GL_TEXTURE_2D, texture_chain);
		defaultShader.setFloat("material.shininess", 35.0f);
		model = glm::mat4();
		model = glm::translate(model, crestfallen_base_position);
		model = glm::scale(model, glm::vec3(crestfallen_base_scale.x, crestfallen_base_scale.y + ((float)sin(time) * 0.02), crestfallen_base_scale.z));
		// ^ Small breathing animation by adding a small amount to the body's y scale over time.
		defaultShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

			// Draw crestfallen legs.
		for (int i = 0; i < 2; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, crestfallen_legs_positions[i]);
			model = glm::scale(model, crestfallen_legs_scale);
			defaultShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
			// Draw crestfallen arms.
		// Upper arms
		for (int i = 0; i < 2; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, crestfallen_upperarms_positions[i]);
			model = glm::rotate(model, glm::radians(25.0f), crestfallen_upperarms_rotation);
			model = glm::scale(model, crestfallen_upperarms_scale);
			defaultShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// Forearms
		for (int i = 0; i < 2; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, crestfallen_forearms_positions[i]);
			model = glm::rotate(model, glm::radians(15.0f), crestfallen_forearms_rotation[i]);
			model = glm::scale(model, crestfallen_forearms_scale);
			defaultShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
			// Draw crestfallen head.
		model = glm::mat4();
		model = glm::translate(model, crestfallen_head_position);
		model = glm::rotate(model, (float)(sin(time) * 0.5), glm::vec3(0.0, 1.0, 0.0));
		// ^ Head shaking animation, common behaviour for human's experiencing hollowing.
		model = glm::rotate(model, glm::radians(25.0f), glm::vec3(1.0, 0.0, 0.0));
		model = glm::scale(model, crestfallen_head_scale);
		defaultShader.setMat4("model", model);
			// Drawing the textures onto the head here is a bit different, because one side of the cube
			// should have his face and the others should, so each side has to be drawn seperately.
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindTexture(GL_TEXTURE_2D, texture_crestfallen_face);
		defaultShader.setFloat("material.shininess", 15.0f);
		glDrawArrays(GL_TRIANGLES, 6, 6);
		glBindTexture(GL_TEXTURE_2D, texture_chain);
		defaultShader.setFloat("material.shininess", 35.0f);
		glDrawArrays(GL_TRIANGLES, 12, 6);
		glDrawArrays(GL_TRIANGLES, 18, 6);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		// ----- Archway (4 parts)
		glm::vec3 arch_center = glm::vec3(0.0, 0.0, -2.0); // Center of the arch, to orient its parts around.
		glm::vec3 arch_legs_scale = glm::vec3(0.3, 2.0, 0.3);
		glm::vec3 arch_legs_positions[] = {
			glm::vec3(arch_center.x - 0.5, 1.0, arch_center.z),
			glm::vec3(arch_center.x + 0.5, 1.0, arch_center.z)
		};

		glm::vec3 arch_tops_scale = glm::vec3(0.3, 0.8, 0.3);
		glm::vec3 arch_tops_positions[] = {
			glm::vec3(arch_center.x - 0.2, 2.0, arch_center.z),
			glm::vec3(arch_center.x + 0.2, 2.0, arch_center.z)
		};

		glm::vec3 arch_tops_rotations[] = {
			glm::vec3(0.0, 0.0, -1.0),
			glm::vec3(0.0, 0.0, 1.0)
		};
		
			// Draw arch legs.
		glBindTexture(GL_TEXTURE_2D, texture_stone);
		defaultShader.setFloat("material.shininess", 15.0f);
		for (int i = 0; i < 2; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, arch_legs_positions[i]);
			model = glm::scale(model, arch_legs_scale);
			defaultShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

			// Draw arch tops.
		for (int i = 0; i < 2; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, arch_tops_positions[i]);
			model = glm::rotate(model, glm::radians(45.0f), arch_tops_rotations[i]);
			model = glm::scale(model, arch_tops_scale);
			defaultShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ----- Bonfire (3 parts (mound, blade, handle), 2 textures (bones, rust))
		glm::vec3 bonfire_mound_scale = glm::vec3(0.3f, 0.2f, 0.3f);
		glm::vec3 bonfire_mound_position = glm::vec3(0.0f, 0.1f, 0.0f);

		glm::vec3 bonfire_blade_scale = glm::vec3(0.04, 0.6, 0.01);
		glm::vec3 bonfire_blade_position = glm::vec3(bonfire_mound_position.x + 0.04, 0.5, bonfire_mound_position.z);
		glm::vec3 bonfire_blade_rotation = glm::vec3(0.0f, 0.0f, -1.0f);
		
		glm::vec3 bonfire_handle_scale = glm::vec3(0.1, 0.04, 0.04);
		glm::vec3 bonfire_handle_position = glm::vec3(bonfire_blade_position.x + 0.035, bonfire_blade_position.y + (bonfire_blade_scale.y / 2) - 0.1, bonfire_blade_position.z);
		glm::vec3 bonfire_handle_rotation = glm::vec3(0.0f, 0.0f, -1.0f);

			// Draw bonfire mound.
		glBindTexture(GL_TEXTURE_2D, texture_bones);
		defaultShader.setFloat("material.shininess", 3.0f);
		model = glm::mat4();
		model = glm::translate(model, bonfire_mound_position);
		model = glm::scale(model, bonfire_mound_scale);
		defaultShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

			// Draw bonfire blade.
		glBindTexture(GL_TEXTURE_2D, texture_rust);	
		defaultShader.setFloat("material.shininess", 9.0f);
		model = glm::mat4();
		model = glm::translate(model, bonfire_blade_position);
		model = glm::rotate(model, glm::radians(10.0f), bonfire_blade_rotation);
		model = glm::scale(model, bonfire_blade_scale);
		defaultShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

			// Draw bonfire blade handle.
		model = glm::mat4();
		model = glm::translate(model, bonfire_handle_position);
		model = glm::rotate(model, glm::radians(10.0f), bonfire_handle_rotation);
		model = glm::scale(model, bonfire_handle_scale);
		defaultShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// ----- Chosen Undead (chud for short).
		// Slightly simpler than the crestfallen knight to make more complex animation easier.
		// 6 parts: legs(2), body, arms(2), head.
		// Modify values for animation.
		if (sitAnimation)
		{
			sitModifier = sitModifier - (0.1 * deltaTime);
		}
		else
		{
			sitModifier = 0.0f;
		}
		if (sitModifier < -0.4f)
		{
			sitModifier = -0.4f; 
		}

		glm::vec3 chud_legs_scale = glm::vec3(0.04, 0.6, 0.04); // Legs.
		glm::vec3 chud_legs_positions[] = {
			glm::vec3(bonfire_mound_position.x - 0.03, 0.3, bonfire_mound_position.z + 0.5),
			glm::vec3(bonfire_mound_position.x + 0.03, 0.3, bonfire_mound_position.z + 0.5),
		};

		glm::vec3 chud_body_scale = glm::vec3(0.14, 0.4, 0.1); // Body.
		glm::vec3 chud_body_position = glm::vec3(bonfire_mound_position.x, chud_legs_positions[0].y + (chud_legs_scale.y / 1.5) + sitModifier , chud_legs_positions[0].z);
		// ^ The y value of the body is lowered over time when the R key is pressed, bringin all parts attached to the body down with it.
		
		glm::vec3 chud_arms_scale = glm::vec3(0.04, 0.4, 0.04); // Arms.
		glm::vec3 chud_arms_positions[] = {
			glm::vec3(chud_body_position.x - 0.09, chud_body_position.y, chud_body_position.z),
			glm::vec3(chud_body_position.x + 0.09, chud_body_position.y, chud_body_position.z),
		};
		
		glm::vec3 chud_head_scale = glm::vec3(0.1, 0.15, 0.1);
		glm::vec3 chud_head_position = glm::vec3(chud_body_position.x, chud_body_position.y + (chud_body_scale.y) -0.12, chud_body_position.z);		

			// Draw chud legs.
		glBindTexture(GL_TEXTURE_2D, texture_plate);
		defaultShader.setFloat("material.shininess", 35.0f);
		for (int i = 0; i < 2; i++)
		{
			model = glm::mat4(1.0f);
			if (sitAnimation) // When R is pressed, the legs slowly sit down over time.
			{
				// Normalize the sitModifier value.
				float normed_sit = (sitModifier - 0.0f) / (-0.4f - 0.0f);
				// Lerp the rotation of the legs.
				float rotationOverTime = 0.0f * (1.0f - normed_sit) + 90.0f * normed_sit;
				float scaleOverTime = 1.0f * (1.0f - normed_sit) + 2.0f * normed_sit;

				// Apply animation transformations.
				model = glm::translate(model, glm::vec3(chud_legs_positions[i].x, chud_legs_positions[i].y + sitModifier / 2, chud_legs_positions[i].z + sitModifier / 2));
				model = glm::rotate(model, glm::radians(rotationOverTime), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(chud_legs_scale.x, chud_legs_scale.y / scaleOverTime, chud_arms_scale.z));
			}
			else // No animations necessary, standing idle.
			{
				model = glm::translate(model, chud_legs_positions[i]);
				model = glm::scale(model, chud_legs_scale);
			}
			defaultShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
			// Draw chud body.
		model = glm::mat4();
		model = glm::translate(model, chud_body_position);
		model = glm::scale(model, chud_body_scale);
		defaultShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
			// Draw chud arms.
		for (int i = 0; i < 2; i++)
		{
			model = glm::mat4(1.0f);
			if (sitAnimation) // When R is pressed, the arms slowly relax over time.
			{
				// Normalize the sitModifier value.
				float normed_sit = (sitModifier - 0.0f) / (-0.4f - 0.0f);
				// Lerp the rotation of the arms
				float rotationOverTime = 0.0f * (1.0f - normed_sit) + 45.0f * normed_sit;
				float scaleOverTime = 1.0f * (1.0f - normed_sit) + 2.0f * normed_sit;

				// Apply animation transformations.
				model = glm::translate(model, glm::vec3(chud_arms_positions[i].x, chud_arms_positions[i].y, chud_arms_positions[i].z + sitModifier / 4));
				model = glm::rotate(model, glm::radians(rotationOverTime), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(chud_arms_scale.x, chud_arms_scale.y / scaleOverTime, chud_arms_scale.z));
			}
			else // No animations necessary, arms at their side.
			{
				model = glm::translate(model, chud_arms_positions[i]);
				model = glm::scale(model, chud_arms_scale);
			}
			defaultShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
			// Draw chud head.
		model = glm::mat4();
		model = glm::translate(model, chud_head_position);
		model = glm::scale(model, chud_head_scale);
		defaultShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, texture_chud_face);
		defaultShader.setFloat("material.shininess", 15.0f); // Keep this same brightness for face and hair.
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindTexture(GL_TEXTURE_2D, texture_chud_hair);
		glDrawArrays(GL_TRIANGLES, 6, 6);
		glDrawArrays(GL_TRIANGLES, 12, 6);
		glDrawArrays(GL_TRIANGLES, 18, 6);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		// ----- Grave Marker (2 parts, base and marker).
		glm::vec3 grave_base_scale(0.25f, 0.2f, 0.15f);
		glm::vec3 grave_base_position(-2.0f, 0.1f, 2.0f);

		glm::vec3 grave_marker_scale(0.1f, 0.4f, 0.02f);
		glm::vec3 grave_marker_position = glm::vec3(grave_base_position.x, grave_base_position.y + 0.2, grave_base_position.z);

			// Draw grave base.
		glBindTexture(GL_TEXTURE_2D, texture_stone);
		defaultShader.setFloat("material.shininess", 3.0f);
		model = glm::mat4();
		model = glm::translate(model, grave_base_position);
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, grave_base_scale);
		defaultShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

			// Draw grave marker.
		glBindTexture(GL_TEXTURE_2D, texture_wood_planks);
		model = glm::mat4();
		model = glm::translate(model, grave_marker_position);
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, grave_marker_scale);
		defaultShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// ----- Chest (6 parts, 5 for the container, 1 for the lid).
		glm::vec3 chest_parts_scales[] = {
			glm::vec3(1.0, 0.01, 1.0), // Bottom of chest.
			
			glm::vec3(1.0, 1.0, 0.01), // Back and front of chest.
			glm::vec3(1.0, 1.0, 0.01),
			
			glm::vec3(0.01, 1.0, 1.0), // Sides of the chest.
			glm::vec3(0.01, 1.0, 1.0)
		};

		glm::vec3 chest_parts_positions[] = {
			glm::vec3(2.0, 0.1, 2.0),
			
			glm::vec3(2.0, 0.5, 1.5),
			glm::vec3(2.0, 0.5, 2.5),
			
			glm::vec3(1.5, 0.5, 2.0),
			glm::vec3(2.5, 0.5, 2.0)
		};

		glm::vec3 chest_lid_scale = glm::vec3(1.0, 0.01, 1.0);
		glm::vec3 chest_lid_position = glm::vec3(2.1, 1.3, 2.0);

			// Draw container of chest.
		glBindTexture(GL_TEXTURE_2D, texture_wood_planks);
		defaultShader.setFloat("material.shininess", 3.0f);
		for (int i = 0; i < 5; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, chest_parts_positions[i]);
			model = glm::scale(model, chest_parts_scales[i]);
			defaultShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

			// Draw lid of chest.
		model = glm::mat4(1.0f);
		model = glm::translate(model, chest_lid_position);
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		model = glm::scale(model, chest_lid_scale);
		defaultShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// Render the light cube.
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.01f));
		lightCubeShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//CHECK EVENTS AND SWAP BUFFERS.
		glfwPollEvents(); //Checks whether events have happened, and calls their corresponding callback functions.
		glfwSwapBuffers(window); //Swaps back buffer once it is ready to be displayed.
	}

	glfwTerminate(); // Cleans all of GLFW's resources.

	return 0;
}

// Callback method that gets called whenever the user resizes the window.
// Adjusts the viewport to match the new size.
// GLFW calls this function when the resizing happens, and provides the values for the arguments.
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0,0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	const float sensitivity = 0.1f;

	// Set initial camera position to where the mouse starts so that the camera
	// doesn't move suddenly on first input.
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos; // Reversed since Y ranges from bottom to top of screen.
	lastX = xPos;
	lastY = yPos;

	// Apply camera sensitivity.
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	// Apply limits to pitch so that the camera can't look up and back around,
	// when the value of pitch goes above 90 degrees or below -90 degrees, lookAt() will flop.
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	//std::cout << pitch << std::endl;

	// Apply new pitch and yaw to the camera vectors.
	glm::vec3 direction;	
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void processInput(GLFWwindow *window)
{
	// GLint polymode;
	const float defaultSpeed = 2.5f * deltaTime;
	float cameraSpeed = defaultSpeed;

	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraSpeed *= 2;
	
	// Checks whether the escape key is pressed, and if so, signals the window to close.
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// Camera movement.
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	// Limit Camera position to inside the skybox.
	// Skybox is 20x20x20 centered on the origin, so 10 in each direction.
	// 10 - 1 so that the camera can't clip out of the skybox.
	if (cameraPos.x > 9.0)
		cameraPos.x = 9.0;
	if (cameraPos.x < -9.0)
		cameraPos.x = -9.0;
	if (cameraPos.y > 9.0)
		cameraPos.y = 9.0;
	if (cameraPos.y < -9.0)
		cameraPos.x = -9.0;
	if (cameraPos.z > 9.0)
		cameraPos.z = 9.0;
	if (cameraPos.z < -9.0)
		cameraPos.z = -9.0;

	// Toggle the light source following the camera.
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && fLastKeyStatus == GLFW_RELEASE)
	{
		lightFollows = !lightFollows;
	}
	fLastKeyStatus = glfwGetKey(window, GLFW_KEY_F);

	// If the light is set to following (default on), update it's position to match the camera's position.
	if (lightFollows == true)
	{
		lightPosition = cameraPos;
	}

	// Sitting animation trigger.
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && rLastKeyStatus == GLFW_RELEASE)
	{
		sitAnimation = !sitAnimation;
	}
	rLastKeyStatus = glfwGetKey(window, GLFW_KEY_R);

	// Light brightness modifier.
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		brightnessModifier = brightnessModifier + 0.01;	
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		brightnessModifier = brightnessModifier - 0.01;

	// Limit brightness modifier.
	if (brightnessModifier > 0.9f)
		brightnessModifier = 0.9f;
	if (brightnessModifier < -0.19f)
		brightnessModifier = -0.19f;

	// Perspective / Orthographic switch.	
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && pLastKeyStatus == GLFW_RELEASE)
	{
		perspective = !perspective;
	}
	pLastKeyStatus = glfwGetKey(window, GLFW_KEY_P);

	// Dark Lighting / Bright Lighting switch.	
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && oLastKeyStatus == GLFW_RELEASE)
	{
		darkish = !darkish;
	}
	oLastKeyStatus = glfwGetKey(window, GLFW_KEY_O);

	//Just for fun, pressing space will turn wireframe mode on or off. ---Causes a stack smash for some reason.
/*	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		glGetIntegerv(GL_POLYGON_MODE, &polymode);
		if (polymode == GL_FILL)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (polymode == GL_LINE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
*/
}

// Gets the texture from a given filePath.
unsigned int getTexture(const char* filePath)
{
	unsigned int texture;

	// Image file Loading using stb_image.
	int width, height, numChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filePath, &width, &height, &numChannels, 0);

	// Create texture.
	glGenTextures(1, &texture); // Generate 1 texture array.
	glBindTexture(GL_TEXTURE_2D, texture); // Bind that array so future texture commands affect it.

	// Account for difference in channels between jpg and png images!!!
	GLenum channelType = GL_RGB; // By default, assume the image is a 3 channel png.
					// (Since I'm probably not gonna use any bitmap images.)
	if (numChannels == 4) // If image has 4 channels (like a png.)
	{
		channelType = GL_RGBA;
	}

	if (data) // Make sure the image was actually loaded properly.
	{
		glTexImage2D(GL_TEXTURE_2D, 0, channelType, width, height, 0, channelType, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmap for currently bound texture.
	}
	else
	{
		std::cout << "Failed to load texture." << std::endl;
	}

	// Free the memory storing the image data now that we have it as a texture.
	stbi_image_free(data);

	return texture;
}
