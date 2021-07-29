/*
Realizado por: Santos Martínez Daniela
Fecha: 20/Julio/21
Versión: 1.0
*/


#include <iostream>
#include <cmath>

//	GLEW
#include <GL/glew.h>

//	GLFW
#include <GLFW/glfw3.h>

//	Other Libs
#include "stb_image.h"

//	GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//	Load Models
#include "SOIL2/SOIL2.h"

//	Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

//	Prototipos de funciones 
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void animacion();

//	Dimensiones de la pantalla 
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

//	Camara
float cam_x=4.0f, cam_y=10.0f, cam_z=60.0f;	//Variables de la posicion inicial de la camara
Camera  camera(glm::vec3(cam_x, cam_y, cam_z));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;

//	Opción del canal de TV
float opTV = 0;

//	Ángulo inicial de la puerta principal 
float anguloPuertaPrincipalInicial = 0;

//	Ángulo final de la puerta principal 
float anguloPuertaPrincipalFinal = 90.0f;

//	Incremento o disminución del ángulo de la puerta principal
float anguloPuertaPrincipalVariacion = 0;

//	Mostrar el mantel 
bool opMantel = false;

//	Mostrar los platos 
bool opPlatos = false;

//	Posición inicial de los libros en z
float inicial_libro_1 = -30.8;
float inicial_libro_2 = -30.8;
float inicial_libro_3 = -30.8;
float inicial_libro_4 = -30.8;

//	Activación de movimiento de la pelota 
bool recorrido_pelota = false;

//	Activar y desactivar los recorridos de la pelota 
bool recorrido1 = true;
bool recorrido2 = false;
bool recorrido3 = false;
bool recorrido4 = false;

//	Posición inicial de la pelota 
float posInicialPelota_x = -17.7;
float posInicialPelota_y = 3;
float posInicialPelota_z = 15.6;

//	Variación de la pelota (DeltaY y DeltaZ)
float movPelY = 0.0;
float movPelZ = 0.0;

//	Ángulo de rotacion de la pelota 
float rotPel = 0.0;

//	Light atributos
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

//	Deltatime
GLfloat deltaTime = 0.0f;	// Tiempo entre el frame actual y el último frame
GLfloat lastFrame = 0.0f;  	// Tiempo del ultimo frame 

//	Keyframes. Para guardar posiciones
float L1 = 0, L2=0, L3=0, L4 = 0;

//	Número máximo de frames 
#define MAX_FRAMES 6

//	La linea de la animación tendra 160 pasos
int i_max_steps = 160;
int i_curr_steps = 0;

typedef struct _frame
{
	//Variables para GUARDAR Key Frames

	//	Variables para la posicion e incremento del primer libro 
	float L1;
	float inc_L1;

	//	Variables para la posicion e incremento del segundo libro
	float L2;
	float inc_L2;

	//	Variables para la posicion e incremento del tercer libro
	float L3;
	float inc_L3;

	//	Variables para la posicion e incremento del cuarto libro 
	float L4;
	float inc_L4;

}FRAME;

//	Se crea un frame 
FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//	Introducir datos
bool play = false;			//	Cuando esta activada o desactivada la animación de los frames 
int playIndex = 0;			//	Indice de la animación 

//	Guardar frames 
void saveFrame(void)
{
	printf("frameindex %d\n", FrameIndex);
	KeyFrame[FrameIndex].L1 = L1;
	KeyFrame[FrameIndex].L2 = L2;
	KeyFrame[FrameIndex].L3 = L3;
	KeyFrame[FrameIndex].L4 = L4;
	
	FrameIndex++;
}

//	Resetear frames 
void resetElements(void)
{
	L1 = KeyFrame[0].L1;
	L2 = KeyFrame[0].L2;
	L3 = KeyFrame[0].L3;
	L4 = KeyFrame[0].L4;
}

//	Función de interpolación 
//	Calculo de los puntos medios entre la posición inicial y final 
void interpolation(void)
{
	//	Incremento = (Posición inicial - Posición final) / Número máximo de pasos 
	KeyFrame[playIndex].inc_L1 = (KeyFrame[playIndex + 1].L1 - KeyFrame[playIndex].L1) / i_max_steps;
	KeyFrame[playIndex].inc_L2 = (KeyFrame[playIndex + 1].L2 - KeyFrame[playIndex].L2) / i_max_steps;
	KeyFrame[playIndex].inc_L3 = (KeyFrame[playIndex + 1].L3 - KeyFrame[playIndex].L3) / i_max_steps;
	KeyFrame[playIndex].inc_L4 = (KeyFrame[playIndex + 1].L4 - KeyFrame[playIndex].L4) / i_max_steps;
}

//	Posición de los point lights
glm::vec3 pointLightPositions[] =
{
	glm::vec3(10,-10,0),
	glm::vec3(0,-10,10),
	glm::vec3(-10,-10,0),
	glm::vec3(0,-10,-10)
};

//	Función principal
int main()
{
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Final", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");

	//	Carga de modelos de la casa y los objetos 

	//	Casa
	Model Casa((char*)"Models/casa/casa.obj");
	Model Casa2((char*)"Models/casa/casa2.obj");
	Model Casa3((char*)"Models/casa/casa3.obj");
	Model Casa4((char*)"Models/casa/casa4.obj");
	Model Casa5((char*)"Models/casa/casa5.obj");
	Model Casa6((char*)"Models/casa/casa6.obj");
	Model Entrada((char*)"Models/casa/entrada.obj");
	Model Pasto((char*)"Models/casa/pasto.obj");
	Model Barra((char*)"Models/casa/barra.obj");
	
	//	Puerta principal 
	Model PuertaP((char*)"Models/casa/puertaP.obj");

	//	Comedor
	Model MesaComedor((char*)"Models/objetos/mesaComedor.obj");
	Model Mantel((char*)"Models/objetos/mantel.obj");
	Model Silla((char*)"Models/objetos/silla.obj");
	Model Plato((char*)"Models/objetos/plato.obj");
	Model Cortinas((char*)"Models/objetos/cortinas.obj");
	Model Cuadro((char*)"Models/objetos/cuadro.obj");
	Model Reloj((char*)"Models/objetos/reloj.obj");

	//	Sala
	Model Sillon((char*)"Models/objetos/sillon.obj");
	Model Mesita((char*)"Models/objetos/mesita.obj");
	Model Librero((char*)"Models/objetos/librero.obj");
	Model Lampara((char*)"Models/objetos/lampara.obj");
	Model SillonIndividual((char*)"Models/objetos/sillonIndividual.obj");
	Model Espejo((char*)"Models/objetos/espejo.obj");
	Model MesitaDeLampara((char*)"Models/objetos/mesitaDeLampara.obj");
	Model MuebleEspejo((char*)"Models/objetos/muebleEspejo.obj");
	Model CamaPerro((char*)"Models/objetos/camaPerro.obj");

	//	Libros
	Model Libro1((char*)"Models/objetos/libro1.obj");
	Model Libro2((char*)"Models/objetos/libro2.obj");
	Model Libro3((char*)"Models/objetos/libro3.obj");
	Model Libro4((char*)"Models/objetos/libro4.obj");

	//	Puerta movible de la sala 
	Model PM1((char*)"Models/casa/PuertaMovible1.obj");
	Model PM2((char*)"Models/casa/puertaMovible2.obj");

	//	TV
	Model TV((char*)"Models/objetos/tv.obj");
	Model TVPantalla0((char*)"Models/objetos/tvPantalla0.obj");
	Model TVPantalla((char*)"Models/objetos/tvPantalla.obj");
	Model TVPantalla2((char*)"Models/objetos/tvPantalla2.obj");
	Model TVPantalla3((char*)"Models/objetos/tvPantalla3.obj");
	Model TVPantalla4((char*)"Models/objetos/tvPantalla4.obj");
	Model TVPantalla5((char*)"Models/objetos/tvPantalla5.obj");
	Model TVPantalla6((char*)"Models/objetos/tvPantalla6.obj");
	Model TVPantalla7((char*)"Models/objetos/tvPantalla7.obj");
	Model TVPantalla8((char*)"Models/objetos/tvPantalla8.obj");
	Model TVPantalla9((char*)"Models/objetos/tvPantalla9.obj");

	//	Aro de basquetbol y balón
	Model Aro((char*)"Models/casa/aro.obj");
	Model Balon((char*)"Models/casa/balon.obj");


	//Inicialización de KeyFrames
	for(int i=0; i<MAX_FRAMES; i++)
	{

		KeyFrame[i].L1 = 0;
		KeyFrame[i].inc_L1 = 0;

		KeyFrame[i].L2 = 0;
		KeyFrame[i].inc_L2 = 0;

		KeyFrame[i].L3 = 0;
		KeyFrame[i].inc_L3 = 0;

		KeyFrame[i].L4 = 0;
		KeyFrame[i].inc_L4 = 0;
	}



	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	//Se creael cubo para el skybox
	GLfloat skyboxVertices[] = 
	{
		//Posiciones 
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//	SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1,&skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	// Texturas del skybox (1 por cada cara del cubo) en una lista 
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.tga");
	faces.push_back("SkyBox/left.tga");
	faces.push_back("SkyBox/top.tga");
	faces.push_back("SkyBox/bottom.tga");
	faces.push_back("SkyBox/back.tga");
	faces.push_back("SkyBox/front.tga");
	
	//	Se manda a cargar a memoria con ayuda de Texture.h
	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();


		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);


		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects'.
		// == ==========================
		
		
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);


		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);

		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glBindVertexArray(VAO);
		glm::mat4 tmp = glm::mat4(1.0f); //Temp


		//	Carga de modelos
		//	Algunos no tienen movimiento por lo que no se carga objeto por objeto y se hace todo junto, como la casa
		//	Los que tienen movimiento si se carga uno por uno, como la puerta y los libros 
		glm::mat4 model(1);

		//	casa
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Casa.Draw(lightingShader);

		//	casa2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Casa2.Draw(lightingShader);

		//	casa3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Casa3.Draw(lightingShader);

		//casa4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Casa4.Draw(lightingShader);

		//casa5
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Casa5.Draw(lightingShader);

		//casa6
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Casa6.Draw(lightingShader);

		// Barra del techo 1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, -2, 0));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Barra.Draw(lightingShader);

		// Barra del techo 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, -2, 5));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Barra.Draw(lightingShader);

		// Barra del techo 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, -2, -5));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Barra.Draw(lightingShader);

		// Barra del techo 4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, -2, -10));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Barra.Draw(lightingShader);

		// Barra del techo 5
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, -2, -15));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Barra.Draw(lightingShader);

		// Barra del techo 6
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, -2, -20));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Barra.Draw(lightingShader);

		//	puertaMovible1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PM1.Draw(lightingShader);

		//	puertaMovible2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PM2.Draw(lightingShader);

		//	puertaPrincipal 
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(16.8, 6.5, -3));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(anguloPuertaPrincipalInicial + anguloPuertaPrincipalVariacion), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(3, 0, 0));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PuertaP.Draw(lightingShader);

		//	Entrada
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(1.5, 0, -0.3));
		model = glm::scale(model, glm::vec3(0.045f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Entrada.Draw(lightingShader);

		//	Pasto
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, -20, 1));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pasto.Draw(lightingShader);

		//	Aro
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Aro.Draw(lightingShader);

		//	Balon1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(posInicialPelota_x, posInicialPelota_y + movPelY, posInicialPelota_z + movPelZ));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		model = glm::rotate(model, glm::radians(rotPel), glm::vec3(1.0f, 0.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Balon.Draw(lightingShader);

		//	Balon2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-4, posInicialPelota_y, 12));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Balon.Draw(lightingShader);

		//	Sillon
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(2, 2, -43));
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Sillon.Draw(lightingShader);

		//	Mesita
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(9, 2, -43));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Mesita.Draw(lightingShader);

		//MesaComedor
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(3.8, 2.8, -15));
		model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		MesaComedor.Draw(lightingShader);

		//	Si opMantel es "true" mostrara el modelo del mantel 
		if (opMantel == true && opPlatos == false )
		{
			//Mantel
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(3.8, 2.9, -15));
			model = glm::scale(model, glm::vec3(1.1f, 1.11f, 1.1f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			Mantel.Draw(lightingShader);
		}

		//	Si opMantel y opPlatos son "true" mostrara 6 platos sobre el mantel 
		if (opMantel == true && opPlatos == true)
		{
			//	Mantel
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(3.8, 2.9, -15));
			model = glm::scale(model, glm::vec3(1.1f, 1.11f, 1.1f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			Mantel.Draw(lightingShader);

			//	Plato1
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(2.3, 5, -13.5));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			Plato.Draw(lightingShader);

			//	Plato2
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(2.3, 5, -16.4));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			Plato.Draw(lightingShader);

			//	Plato3
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(5.2, 5, -13.5));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			Plato.Draw(lightingShader);

			//	Plato4
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(5.2, 5, -16.4));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			Plato.Draw(lightingShader);

			//	Plato5
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(0.7, 5, -15));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			Plato.Draw(lightingShader);

			//	Plato6
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(6.9, 5, -14.9));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			Plato.Draw(lightingShader);
		}

		//	Silla1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(2.8, 2.8, -15.8));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Silla.Draw(lightingShader);

		//	Silla2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(6, 2.8, -15.8));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Silla.Draw(lightingShader);

		//	Silla3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(1.5, 2.8, -14.3));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Silla.Draw(lightingShader);

		//	Silla4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(4.7, 2.8, -14.3));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Silla.Draw(lightingShader);

		//	Silla5
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(2.7, 2.8, -14.5));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Silla.Draw(lightingShader);

		//	Cortinas
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(4.2, 7.3, -4));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.7f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cortinas.Draw(lightingShader);

		//	Cuadro
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(18.7, 8.5, -25.8));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cuadro.Draw(lightingShader);

		//	Reloj
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(11.4, 10.5, -27.3));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Reloj.Draw(lightingShader);

		//	Silla6
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(4.7, 2.8, -15.5));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Silla.Draw(lightingShader);

		//	Librero
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-8.9, 4.2, -31.2));
		model = glm::scale(model, glm::vec3(1.5f, 1.8f, 1.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Librero.Draw(lightingShader);

		//	Los libros se mueven en el eje z 
		//	Libro1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-8.9, 4.2, inicial_libro_1 + L1));
		model = glm::scale(model, glm::vec3(1.5f, 1.8f, 1.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Libro1.Draw(lightingShader);

		//	Libro2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-8.9, 4.2, inicial_libro_2 + L2));
		model = glm::scale(model, glm::vec3(1.5f, 1.8f, 1.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Libro2.Draw(lightingShader);

		//Libro3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-8.9, 4.2, inicial_libro_3 + L3));
		model = glm::scale(model, glm::vec3(1.5f, 1.8f, 1.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Libro3.Draw(lightingShader);

		//	Libro4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-8.9, 4.2, inicial_libro_4 + L4));
		model = glm::scale(model, glm::vec3(1.5f, 1.8f, 1.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Libro4.Draw(lightingShader);

		//	mesitaDeLampara
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(1.7, 1.9, -35.5));
		model = glm::scale(model, glm::vec3(0.6f, 0.7f, 0.6f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		MesitaDeLampara.Draw(lightingShader);

		//	Espejo
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-8.4, 6.7, -52.2));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Espejo.Draw(lightingShader);

		//	MuebleEspejo
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-8.4, 4.5, -53.2));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		MuebleEspejo.Draw(lightingShader);

		//	SillonIndividual
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(2.9, 2.0, -38.5));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		SillonIndividual.Draw(lightingShader);

		//	Lampara
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(1.7, 1.2, -35.5));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.Draw(lightingShader);

		//	CamaPerro
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(16, 2.1, -54));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		CamaPerro.Draw(lightingShader);

		//	TV
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(5.5, 2, -43));
		model = glm::scale(model, glm::vec3(1.5f, 1.8f, 1.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		TV.Draw(lightingShader);

		//	tvPantalla
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(5.5, 15.5, -43));
		model = glm::scale(model, glm::vec3(1.5f, 1.8f, 1.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// De acuerdo al número que tiene guardado opTV es que mostrara un modelo diferente  
		if (opTV == 0)
		{
			TVPantalla0.Draw(lightingShader);
		}
		else if (opTV == 1)
		{
			TVPantalla.Draw(lightingShader);
		}
		else if (opTV == 2)
		{
			TVPantalla2.Draw(lightingShader);
		}
		else if (opTV == 3)
		{
			TVPantalla3.Draw(lightingShader);
		}
		else if (opTV == 4)
		{
			TVPantalla4.Draw(lightingShader);
		}
		else if (opTV == 5)
		{
			TVPantalla5.Draw(lightingShader);
		}
		else if (opTV == 6)
		{
			TVPantalla6.Draw(lightingShader);
		}
		else if (opTV == 7)
		{
			TVPantalla7.Draw(lightingShader);
		}
		else if (opTV == 8)
		{
			TVPantalla8.Draw(lightingShader);
		}
		else if (opTV == 9)
		{
			TVPantalla9.Draw(lightingShader);
		}


		glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		
		//	Draw the light object (using light's vertex attributes)
		//	Podrian borrarse, solo es para visualizar de donde salen los puntos de luz
		glBindVertexArray(lightVAO);
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.5f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);


		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();


	return 0;
}


void animacion()
{
		//Movimiento de los libros en el librero 
		if (play)	//	Si esta activa la animación de los libros
		{
			if (i_curr_steps >= i_max_steps) //	Si aún no se llega al numero de pasos máximo  
			{
				playIndex++;
				if (playIndex > FrameIndex - 2)	//	End of total animation?
				{
					printf("termina anim\n");
					playIndex = 0;
					play = false;
				}
				else //	Next frame interpolations
				{
					i_curr_steps = 0; //	Se resetea el contador de pasos 
									  
					interpolation();	//	Calculo de interpolación
				}
			}
			else
			{
				//	Se guarda cada frame con el valor incrementado 

				L1 += KeyFrame[playIndex].inc_L1;
				L2 += KeyFrame[playIndex].inc_L2;
				L3 += KeyFrame[playIndex].inc_L3;
				L4 += KeyFrame[playIndex].inc_L4;

				i_curr_steps++;
			}
		}

		//	Movimiento de la pelota 
		if (recorrido_pelota)	//	Si esta activa la animación de la pelota
		{
			//	Si esta activo el primer recorrido (En el eje Z en el pasto)
			if (recorrido1)
			{
				rotPel = -180;
				movPelZ += 0.05f; //	Aumenta la distancia de la pelota en el eje Z
				if (movPelZ > 12)	//	La distancia máxima que puede recorrer en Z
				{
					//	Se desactiva el primer recorrido y se activa el segundo
					recorrido1 = false;
					recorrido2 = true;
				}
			}

			//	Si esta activo el segundo recorrido (En el eje Z y Y)
			if (recorrido2)
			{
				//	Mientras aumenta la distancia en Y, disminuye la de Z para ir en diagonal 
				rotPel = 54.654;
				movPelZ -= 0.2f;
				movPelY += 0.282f;
				if (movPelY > 17 && movPelZ < 0) //	Si se llego al punto máximo en Y y minimo en Z 
				{
					//	Se desactiva el segundo recorrido y se activa el tercero
					recorrido2 = false;
					recorrido3 = true;
				}
			}

			//	Si esta activo el tercer recorrido (En el eje Y)
			if (recorrido3)
			{
				rotPel = -90;
				movPelY -= 0.2f;	//	Disminuye la distancia de la pelota en el eje Y
				if (movPelY < 0)	//	El punto minimo al que se puede llegar en Y
				{
					//	Se desactiva el tercer recorrido y se activa el cuarto
					recorrido3 = false;
					recorrido4 = true;
				}
			}

			//	Si esta activo el cuarto recorrido (En el eje Z)
			if (recorrido4)
			{

				rotPel = -180;
				movPelZ += 0.05f;	//	Aumenta la distancia de la pelota en el eje Z
				if (movPelZ > 0)	//	Para llegar al punto inicial de la pelota 
				{
					//	Se desactiva el cuarto recorrido y se activa el primero para iniar de nuevo 
					recorrido4 = false;
					recorrido1 = true;
				}

			}
		}
}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_L])
	{
		if (play == false && (FrameIndex > 1))
		{

			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}

	}

	//	Guardar el frame (Posición de los libros) al presionar K 
	if (keys[GLFW_KEY_K])
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}

	}

	//	Cerrar la ventana del programa 
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	//	Para cambiar el canal de TV
	//	Al presiionar la tecla T disminuira el contador opTV
	if (keys[GLFW_KEY_T])
	{
		if (opTV == 9)
		{
			opTV = 8;
		}
		else if (opTV == 8)
		{
			opTV = 7;
		}
		else if (opTV == 7)
		{
			opTV = 6;
		}
		else if (opTV == 6)
		{
			opTV = 5;
		}
		else if (opTV == 5)
		{
			opTV = 4;
		}
		else if (opTV == 4)
		{
			opTV = 3;
		}
		else if (opTV == 3)
		{
			opTV = 2;
		}
		else if (opTV == 2)
		{
			opTV = 1;
		}
		else if (opTV == 1)
		{
			opTV = 0;
		}
		else if (opTV == 0)
		{
			opTV = 9;
		}
	}

	//	Al presiionar la tecla Y aumentara el contador opTV
	if (keys[GLFW_KEY_Y])
	{
		if (opTV == 0)
		{
			opTV = 1;
		}
		else if (opTV == 1)
		{
			opTV = 2;
		}
		else if (opTV == 2)
		{
			opTV = 3;
		}
		else if (opTV == 3)
		{
			opTV = 4;
		}
		else if (opTV == 4)
		{
			opTV = 5;
		}
		else if (opTV == 5)
		{
			opTV = 6;
		}
		else if (opTV == 6)
		{
			opTV = 7;
		}
		else if (opTV == 7)
		{
			opTV = 8;
		}
		else if (opTV == 8)
		{
			opTV = 9;
		}
		else if (opTV == 9)
		{
			opTV = 0;
		}
	}

	//	Al presiionar la tecla O cambiara el estado de la variable bool opMantel que mostrara o quitara el matel de la mesa del comedor 
	//	Si quita el mantel también se desactiva la opción de mostrar los platos 
	if (keys[GLFW_KEY_O])
	{
		if (opMantel == true)
		{
			opMantel = false;
			opPlatos = false;
		}
		else if (opMantel == false)
		{
			opMantel = true;
		}
	}

	//	Al presiionar la tecla P cambiara el estado de la variable bool opPlatos que mostrara o quitara los platos de la mesa del comedor 
	if (keys[GLFW_KEY_P])
	{
		if (opPlatos == true)
		{
			opPlatos = false;
		}
		else if (opPlatos == false)
		{
			opPlatos = true;
		}
	}

	//Abrir y cerrar puerta
	if (keys[GLFW_KEY_N])
	{
		if (anguloPuertaPrincipalVariacion <= anguloPuertaPrincipalFinal)
		{
			//	Incrementa el angulo de la puerta principal, esto significa que se abrira 
			anguloPuertaPrincipalVariacion += 1.5f;
		}
	}
	else if (keys[GLFW_KEY_M])
	{
		if (anguloPuertaPrincipalVariacion >= anguloPuertaPrincipalInicial)
		{
			//	Disminución del angulo de la puerta principal, esto significa que se cerrara 
			anguloPuertaPrincipalVariacion -= 1.5f;
		}
		
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}


void DoMovement()
{
	//	Mover los libros 
	//	A la derecha
	if (keys[GLFW_KEY_1])
	{

		if (L1 > -4.95)
			L1 -= 0.12f;

	}

	if (keys[GLFW_KEY_2])
	{
		if (L2 > -4.95)
			L2 -= 0.12f;
			
	}

	if (keys[GLFW_KEY_3])
	{
		if (L3 > -4.95)
			L3 -= 0.12f;
		
	}

	if (keys[GLFW_KEY_4])
	{
		if (L4 > -4.95)
			L4 -= 0.12f;

	}

	//	A la izquierda
	if (keys[GLFW_KEY_5])
	{
		if (L1 < 0)
			L1 += 0.12f;

	}

	if (keys[GLFW_KEY_6])
	{
		if (L2 < 0)
			L2 += 0.12f;

	}

	if (keys[GLFW_KEY_7])
	{
		if (L3 < 0)
			L3 += 0.12f;

	}

	if (keys[GLFW_KEY_8])
	{
		if (L4 < 0)
			L4 += 0.12f;

	}

	//	Activacion del movimiento de la pelota 
	if (keys[GLFW_KEY_U])
	{
		recorrido_pelota = true;
	}

	//	Desactivación del movimiento de la pelota 
	if (keys[GLFW_KEY_I])
	{
		recorrido_pelota = false;
	}

	// Controlar la camara 
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

}
