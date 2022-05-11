#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/camera.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/model.h>
#include <learnopengl/shader_m.h>

#include <iostream>
#include <vector>
#include <string>

#include"Submarin.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

glm::vec3 cameraPos(70.0f, 10.0f, 10.0f);
Camera camera(cameraPos);

Submarin sub(glm::vec3(50.0f, -3.0f, 0.0f));

// set the mouse position to be the center of the screen
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;
bool isCameraFixed = false;

glm::vec3 lightPos(-1.0f, 1.0f, -1.0f);
glm::vec3 lightDirection = glm::normalize(lightPos);
glm::mat4 lightSpaceMatrix;
unsigned int depthMap;


// world Y axis
glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!isCameraFixed) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		// coordinates flip to correspond to the coordinate system
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void keyInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, 0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, 0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, 0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, 0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(DOWN, 0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(UP, 0.03f);
	}

	//Submarin
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		sub.ProcessKeyboard(K_FORWARD, 0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		sub.ProcessKeyboard(K_LEFT, 0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		sub.ProcessKeyboard(K_RIGHT, 0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		sub.ProcessKeyboard(K_BACKWARD, 0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		sub.ProcessKeyboard(K_UP, 0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		sub.ProcessKeyboard(K_DOWN, 0.03f);
	}
}

bool init()
{
	// Load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		system("pause");
		return false;
	}

	// Configure the global openGL state
	glEnable(GL_DEPTH_TEST);

	return true;
}

GLFWwindow* windowInit()
{
	//configure opengl
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create window
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game", NULL, NULL);

	//error checking
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return window;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	//initialize texture id and bind it
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else {
			std::cout << "Could not load texture at path: " << faces[i] << std::endl;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;

}

//skybox
unsigned int cubemapTexture;
unsigned int skyboxVAO, skyboxVBO;
const float skyboxVertices[] = {
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};
std::vector<std::string> faces2{
		 FileSystem::getPath("Assets/skybox/right.jpg"),
		 FileSystem::getPath("Assets/skybox/left.jpg"),
		 FileSystem::getPath("Assets/skybox/top.jpg"),
		 FileSystem::getPath("Assets/skybox/bottom.jpg"),
		 FileSystem::getPath("Assets/skybox/front.jpg"),
		 FileSystem::getPath("Assets/skybox/back.jpg")
};
std::vector<std::string> faces{
		 FileSystem::getPath("Assets/skybox/right2.jpg"),
		 FileSystem::getPath("Assets/skybox/left2.jpg"),
		 FileSystem::getPath("Assets/skybox/top2.jpg"),
		 FileSystem::getPath("Assets/skybox/bottom2.jpg"),
		 FileSystem::getPath("Assets/skybox/front2.jpg"),
		 FileSystem::getPath("Assets/skybox/back2.jpg")
};
std::vector<std::string> faces3{
		 FileSystem::getPath("Assets/skybox/right3.jpg"),
		 FileSystem::getPath("Assets/skybox/left3.jpg"),
		 FileSystem::getPath("Assets/skybox/top3.jpg"),
		 FileSystem::getPath("Assets/skybox/bottom3.jpg"),
		 FileSystem::getPath("Assets/skybox/front3.jpg"),
		 FileSystem::getPath("Assets/skybox/back3.jpg")
};


void skyboxInit(std::vector<std::string> faces) {

	//skybox VAO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//load textures
	cubemapTexture = loadCubemap(faces);
}

void renderSkyBox(Shader& shader)
{
	//view matrix constructed to remove the movement of the camera
	glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	// projection
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

	shader.setMat4("view", viewMatrix);
	shader.setMat4("projection", projMatrix);

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}

void renderSub(Model& model, glm::mat4 modelMatrix, Shader& shader)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians(sub.getYaw()), WORLD_UP);
	// Offset the original rotation of the model
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), WORLD_UP);
	// resize the model
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.02f, 0.02f, 0.02f));

	// apply transformation matrix
	shader.setMat4("model", modelMatrix);

	model.Draw(shader);
}

void configSub(Model& subModel, Shader& shader)
{
	// view transition
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// projection transformation
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	shader.setMat4("projection", projMatrix);

	// model conversion
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, sub.getMidValPosition());
	modelMatrix = glm::rotate(modelMatrix, glm::radians(sub.getYaw() / 2), WORLD_UP);

	renderSub(subModel, modelMatrix, shader);
}

void renderWater(Model& model, Shader& shader)
{
	// view transition
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// model conversion
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	shader.setMat4("model", modelMatrix);
	// projection transformation
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	shader.setMat4("projection", projMatrix);

	model.Draw(shader);
}


void renderObj(Model& model, Shader& shader, float x, float y, float z)
{
	// view transition
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// model conversion
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), WORLD_UP);
	shader.setMat4("model", modelMatrix);
	// projection transformation
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	shader.setMat4("projection", projMatrix);

	model.Draw(shader);
}

void renderLight(Shader& shader)
{
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("lightDirection", lightDirection);
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}

int main()
{
	//window init
	GLFWwindow* window = windowInit();
	bool isInit = init();
	if (window == NULL || !isInit) {
		return -1;
	}
	glfwInit();
	//configure skybox
	skyboxInit(faces);

	//initialize shader for skybox
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	Shader waterShader("Shaders/water.vs", "Shaders/water.fs");
	Model waterModel(FileSystem::getPath("Assets/water-surface/water.obj"));
	//Model subModel(FileSystem::getPath("Assets/submarine/Seaview submarine/Seaview submarine.obj"));
	Model subModel(FileSystem::getPath("Assets/sub/Odyssey_OBJ.obj"));
	Model dolphinModel(FileSystem::getPath("Assets/Dolphin/untitled.obj"));

	Model fishModel(FileSystem::getPath("Assets/fish/fishOBJ.obj"));
	Model nemoModel(FileSystem::getPath("Assets/fish/nemo/OBJ.obj"));
	Model tropicalFishModel(FileSystem::getPath("Assets/fish/tropical/fish.obj"));
	Model jellyfishModel(FileSystem::getPath("Assets/fish/jellyfish/Jellyfish.obj"));

	while (!glfwWindowShouldClose(window))
	{
		keyInput(window);

		//render background
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		waterShader.use();
		renderWater(waterModel, waterShader);

		renderLight(waterShader);

		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		{
			skyboxInit(faces);
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		{
			skyboxInit(faces2);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			skyboxInit(faces3);

		configSub(subModel, waterShader);
		renderObj(dolphinModel, waterShader, -5.0f, 0.0f, -50.0f);
		renderObj(dolphinModel, waterShader, 0.0f, 0.0f, -60.0f);
		renderObj(dolphinModel, waterShader, -10.0f, 0.0f, -60.0f);

		renderObj(fishModel, waterShader, -15.0f, -33.0f, -62.0f);
		renderObj(fishModel, waterShader, -40.0f, -30.0f, -65.0f);
		renderObj(fishModel, waterShader, -20.0f, -35.0f, -70.0f);
		renderObj(fishModel, waterShader, -25.0f, -30.0f, -67.0f);
		renderObj(fishModel, waterShader, -30.0f, -25.0f, -60.0f);
		renderObj(nemoModel, waterShader, 42.0f, -70.0f, -65.0f);
		renderObj(nemoModel, waterShader, 48.0f, -75.0f, -60.0f);
		renderObj(nemoModel, waterShader, 45.0f, -70.0f, -65.0f);
		renderObj(nemoModel, waterShader, 35.0f, -70.0f, -60.0f);
		renderObj(nemoModel, waterShader, 38.0f, -75.0f, -67.0f);
		renderObj(nemoModel, waterShader, 33.0f, -70.0f, -60.0f);
		renderObj(nemoModel, waterShader, 30.0f, -75.0f, -69.0f);

		renderObj(tropicalFishModel, waterShader, -100.0f, -130.0f, 209.0f);
		renderObj(tropicalFishModel, waterShader, -100.0f, -135.0f, 209.0f);
		renderObj(tropicalFishModel, waterShader, -100.0f, -130.0f, 202.0f);
		renderObj(tropicalFishModel, waterShader, -105.0f, -135.0f, 209.0f);
		renderObj(tropicalFishModel, waterShader, -110.0f, -130.0f, 219.0f);
		renderObj(tropicalFishModel, waterShader, -115.0f, -135.0f, 222.0f);

		renderObj(jellyfishModel, waterShader, -50.0f, -5.0f, 117.0f);
		renderObj(jellyfishModel, waterShader, -75.0f, -7.0f, 122.0f);
		renderObj(jellyfishModel, waterShader, -60.0f, -17.0f, 115.0f);
		renderObj(jellyfishModel, waterShader, -66.0f, -12.0f, 125.0f);
		renderObj(jellyfishModel, waterShader, -77.0f, -23.0f, 119.0f);
		renderObj(jellyfishModel, waterShader, -50.0f, -19.0f, 108.0f);
		renderObj(jellyfishModel, waterShader, -85.0f, -27.0f, 105.0f);
		renderObj(jellyfishModel, waterShader, -70.0f, -15.0f, 112.0f);
		renderObj(jellyfishModel, waterShader, -60.0f, -25.0f, 105.0f);

		//render the skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		renderSkyBox(skyboxShader);



		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}