/*----------------------------------------------------------*/
/*------------- Proyecto -----------------------------------*/
/*------------- 2023-1 -------------------------------------*/
/*------------- Alumno: Cruz Zamora Joel David -------------*/
/*------------- No. Cuenta: 315282113 ----------------------*/
/*----------------------------------------------------------*/

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor *monitors;
float escalaGeneral = 20.0f;

//Varibles
float t, valorR, valorG, valorB = 0.0f;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 90.0f));
float MovementSpeed = 0.1f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);				//Editado. Posicion de la fuente de luz
glm::vec3 lightDirection(0.0f, -1.0f, 0.0f);				//Editado. Direccion de la luz natural

// posiciones
//float x = 0.0f;
//float y = 0.0f;
float
movAuto_x = 0.0f,
movAuto_y = 0.0f,
movAuto_z = 50.0f,
orienta = 0.0f,
giroPuertas = 0.0f;
		
		
float giroLlantas = 0.0f;

bool
animacion = false,
animacionPuertas = false,
estadoPuertas = false,
animacion2 = false,
recorrido1 = true,
recorrido2 = false,
recorrido3 = false,
recorrido4 = false,
recorrido5 = false,
recorrido6 = false;

bool etapa1, etapa2, etapa3, etapa4, etapa5;


//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		rotRodIzq = 0.0f,
		rotRodDer = 0.0f,
		giroMonito = 0.0f,
		brazoDer = 0.0f,
		brazoIzq = 0.0f,
		giroCabeza = 0.0f;

float	incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		rodIzqInc = 0.0f,
		rodDerInc = 0.0f,
		giroMonitoInc = 0.0f,
		brazoDerInc = 0.0f,
		brazoIzqInc = 0.0f,
		giroCabezaInc = 0.0f;

#define MAX_FRAMES 40							//Cantidad de cuadros clave
int i_max_steps = 60;							//cuadros clave intermedios
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float rotRodDer;

	float giroMonito;
	float brazoDer;
	float brazoIzq;
	float giroCabeza;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 10;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].rotRodDer = rotRodDer;
	KeyFrame[FrameIndex].giroMonito = giroMonito;
	KeyFrame[FrameIndex].brazoDer = brazoDer;
	KeyFrame[FrameIndex].brazoIzq = brazoIzq;
	KeyFrame[FrameIndex].giroCabeza = giroCabeza;
	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	rotRodDer = KeyFrame[0].rotRodDer;
	giroMonito = KeyFrame[0].giroMonito;
	brazoDer = KeyFrame[0].brazoDer;
	brazoIzq = KeyFrame[0].brazoIzq;
	giroCabeza = KeyFrame[0].giroCabeza;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rodIzqInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	rodDerInc = (KeyFrame[playIndex + 1].rotRodDer - KeyFrame[playIndex].rotRodDer) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;
	brazoDerInc = (KeyFrame[playIndex + 1].brazoDer - KeyFrame[playIndex].brazoDer) / i_max_steps;
	brazoIzqInc = (KeyFrame[playIndex + 1].brazoIzq - KeyFrame[playIndex].brazoIzq) / i_max_steps;
	giroCabezaInc = (KeyFrame[playIndex + 1].giroCabeza - KeyFrame[playIndex].giroCabeza) / i_max_steps;
}

void animate(void)
{
	/* animaciones en las luces
	lightPosition.x = 100.0f * cos(t);
	lightPosition.z = 100.0f * sin(t);

	t += 0.05f;

	valorR = cos(t);
	valorG = sin(t);
	valorB = tan(t);
	*/

	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rodIzqInc;
			rotRodDer += rodDerInc;
			giroMonito += giroMonitoInc;
			brazoDer += brazoDerInc;				//agregado
			brazoIzq += brazoIzqInc;
			giroCabeza += giroCabezaInc;
			i_curr_steps++;
		}
	}

	//Vehículo
	if (animacion)
	{
		if (etapa1) // en esta etapa el carro retrocede
		{
			if (movAuto_z >= 0.0f)
			{
				giroLlantas -= 2.0f;
				movAuto_z -= 0.2f;
			}
			else
			{
				etapa1 = false;
				etapa2 = true;
			}
		}

		if (etapa2) // en esta etapa el carro sube
		{
			if (movAuto_y <= 50.0f)
				movAuto_y += 0.2f;
			else
			{
				etapa2 = false;
				etapa3 = true;
			}
		}

		if (etapa3) // en esta etapa el carro avanza flotando
		{
			if (movAuto_z <= 150.0f)
				movAuto_z += 0.2f;
			else
			{
				etapa3 = false;
				etapa4 = true;
			}
		}

		if (etapa4) // en esta etapa el carro baja
		{
			if (movAuto_y >= 0.0f)
				movAuto_y -= 0.2f;
			else
			{
				etapa4 = false;
				etapa5 = true;
			}
		}

		if (etapa5) // en esta etapa el carro avanza en el suelo
		{
			if (movAuto_z <= 200.0f)
			{
				giroLlantas += 2.0f;
				movAuto_z += 0.2f;
			}
			else
				etapa5 = false;
		}
	}

	if (animacion2)
	{
		if (recorrido1) // origen a (0,0,200)
		{
			movAuto_x = 0.0f;
			movAuto_z += 2.0f;
			orienta = 0.0f;
			if (movAuto_z >= 200.0f)
			{
				recorrido1 = false;
				recorrido2 = true;
			}
		}

		if (recorrido2) // (0,0,200) a (-250,0,200)
		{
			orienta = -90.0f;
			movAuto_x -= 2.0f;

			if (movAuto_x <= -250.0f)
			{
				recorrido2 = false;
				recorrido3 = true;
			}
		}

		if (recorrido3) // (-250,0,200) a (0,0,-200)
		{
			movAuto_x += (0.625f * 2.0f);
			movAuto_z -= 2.0f;
			orienta = 147.99f;
			if (movAuto_x >= 0.0f)
			{
				recorrido3 = false;
				recorrido4 = true;
			}
		}

		if (recorrido4) // (0,0,-200) a (-250,0,-200)
		{
			movAuto_x -= 2.0f;
			orienta = -90.0f;
			if (movAuto_x <= -250.0f)
			{
				recorrido4 = false;
				recorrido5 = true;
			}
		}

		if (recorrido5) // (-250,0,-200) a (0,0,0)
		{
			movAuto_x += (1.25 * 2.0f);
			movAuto_z += 2.0f;
			orienta = 51.3f;
			if (movAuto_x >= 0.0f)
			{
				recorrido5 = false;
				recorrido1 = true;
			}
		}
	}

	// Animacion de las puertas
	if (animacionPuertas)
	{
		if (estadoPuertas) { // Si las puertas estan abiertas
			giroPuertas -= 2.0f;
			if (giroPuertas <= 0.0f) {
				animacionPuertas = false;
				estadoPuertas = false;
			}
		}
		else { // Si las puertas estan cerradas
			giroPuertas += 2.0f;
			if (giroPuertas >= 90.0f) {
				animacionPuertas = false;
				estadoPuertas = true;
			}
		}
	}
}

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces
	{
		"resources/skybox/posx.jpg",
		"resources/skybox/negx.jpg",
		"resources/skybox/posy.jpg",
		"resources/skybox/negy.jpg",
		"resources/skybox/posz.jpg",
		"resources/skybox/negz.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj"); //modelo modificado
	Model botaDer("resources/objects/Personaje/bota.obj");
	Model piernaDer("resources/objects/Personaje/piernader.obj");
	Model piernaIzq("resources/objects/Personaje/piernader.obj");
	Model torso("resources/objects/Personaje/torso.obj");
	Model brazoDerModel("resources/objects/Personaje/brazoder.obj");
	Model brazoIzqModel("resources/objects/Personaje/brazoizq.obj");
	Model cabeza("resources/objects/Personaje/cabeza.obj");
	Model carro("resources/objects/lambo/carroceria.obj");
	Model llanta("resources/objects/lambo/Wheel.obj");
	Model casa("resources/objects/casa/casa.obj"); //modelo agregado
	Model techo("resources/objects/casa/techo.obj"); //modelo agregado
	Model puerta_dorm("resources/objects/puerta_dorm/puerta.obj"); //modelo agregado
	Model puerta_ban("resources/objects/puerta_ban/puerta.obj"); //modelo agregado
	Model puerta_ext("resources/objects/puerta_ext/puerta.obj"); //modelo agregado
	Model cama_mat("resources/objects/cama_matrimonial/cama.obj"); //modelo agregado
	Model cama("resources/objects/cama/cama.obj"); //modelo agregado

	/*
	ModelAnim animacionPersonaje("resources/objects/Personaje1/PersonajeBrazo.dae");
	animacionPersonaje.initShaders(animShader.ID);

	ModelAnim ninja("resources/objects/ZombieWalk/ZombieWalk.dae");
	ninja.initShaders(animShader.ID);
	*/

	//Inicialización de KeyFrames
	//aqui poner frames para la animacion automatica
	/*for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
		KeyFrame[i].brazoDer = 0;
		KeyFrame[i].giroCabeza = 0;
	}*/

	//Posiciones
	//Mueve cabeza
	KeyFrame[0].posX = 0.0f;
	KeyFrame[0].posY = 0.0f;
	KeyFrame[0].posZ = 0.0f;
	KeyFrame[0].rotRodIzq = 0.0f;
	KeyFrame[0].rotRodDer = 0.0f;
	KeyFrame[0].brazoDer = 0.0f;
	KeyFrame[0].brazoIzq = 0.0f;
	KeyFrame[0].giroMonito = 0.0f;
	KeyFrame[0].giroCabeza = 0.0f;

	KeyFrame[1].posX = 0.0f;
	KeyFrame[1].posY = 0.0f;
	KeyFrame[1].posZ = 0.0f;
	KeyFrame[1].rotRodIzq = 0.0f;
	KeyFrame[1].rotRodDer = 0.0f;
	KeyFrame[1].brazoDer = 0.0f;
	KeyFrame[1].brazoIzq = 0.0f;
	KeyFrame[1].giroMonito = 0.0f;
	KeyFrame[1].giroCabeza = -20.0f;

	KeyFrame[2].posX = 0.0f;
	KeyFrame[2].posY = 0.0f;
	KeyFrame[2].posZ = 0.0f;
	KeyFrame[2].rotRodIzq = 0.0f;
	KeyFrame[2].rotRodDer = 0.0f;
	KeyFrame[2].brazoDer = 0.0f;
	KeyFrame[2].brazoIzq = 0.0f;
	KeyFrame[2].giroMonito = 0.0f;
	KeyFrame[2].giroCabeza = 20.0f;

	KeyFrame[3].posX = 0.0f;
	KeyFrame[3].posY = 0.0f;
	KeyFrame[3].posZ = 0.0f;
	KeyFrame[3].rotRodIzq = 0.0f;
	KeyFrame[3].rotRodDer = 0.0f;
	KeyFrame[3].brazoDer = 0.0f;
	KeyFrame[3].brazoIzq = 0.0f;
	KeyFrame[3].giroMonito = 0.0f;
	KeyFrame[3].giroCabeza = 0.0f;

	//Camina
	KeyFrame[4].posX = 0.0f;
	KeyFrame[4].posY = 0.0f;
	KeyFrame[4].posZ = 4.0f;
	KeyFrame[4].rotRodIzq = -60.0f;
	KeyFrame[4].rotRodDer = 60.0f;
	KeyFrame[4].brazoDer = 50.0f;
	KeyFrame[4].brazoIzq = -50.0f;
	KeyFrame[4].giroMonito = 0.0f;
	KeyFrame[4].giroCabeza = 0.0f;

	KeyFrame[5].posX = 0.0f;
	KeyFrame[5].posY = 0.0f;
	KeyFrame[5].posZ = 8.0f;
	KeyFrame[5].rotRodIzq = 60.0f;
	KeyFrame[5].rotRodDer = -60.0f;
	KeyFrame[5].brazoDer = -50.0f;
	KeyFrame[5].brazoIzq = 50.0f;
	KeyFrame[5].giroMonito = 0.0f;
	KeyFrame[5].giroCabeza = 0.0f;

	KeyFrame[6].posX = 0.0f;
	KeyFrame[6].posY = 0.0f;
	KeyFrame[6].posZ = 12.0f;
	KeyFrame[6].rotRodIzq = -60.0f;
	KeyFrame[6].rotRodDer = 60.0f;
	KeyFrame[6].brazoDer = 50.0f;
	KeyFrame[6].brazoIzq = -50.0f;
	KeyFrame[6].giroMonito = 0.0f;
	KeyFrame[6].giroCabeza = 0.0f;

	KeyFrame[7].posX = 0.0f;
	KeyFrame[7].posY = 0.0f;
	KeyFrame[7].posZ = 16.0f;
	KeyFrame[7].rotRodIzq = 60.0f;
	KeyFrame[7].rotRodDer = -60.0f;
	KeyFrame[7].brazoDer = -50.0f;
	KeyFrame[7].brazoIzq = 50.0f;
	KeyFrame[7].giroMonito = 0.0f;
	KeyFrame[7].giroCabeza = 0.0f;

	//Salta
	KeyFrame[8].posX = 0.0f;
	KeyFrame[8].posY = 4.0f;
	KeyFrame[8].posZ = 26.0f;
	KeyFrame[8].rotRodIzq = -60.0f;
	KeyFrame[8].rotRodDer = 60.0f;
	KeyFrame[8].brazoDer = 50.0f;
	KeyFrame[8].brazoIzq = -50.0f;
	KeyFrame[8].giroMonito = 0.0f;
	KeyFrame[8].giroCabeza = 0.0f;

	KeyFrame[9].posX = 0.0f;
	KeyFrame[9].posY = 0.0f;
	KeyFrame[9].posZ = 36.0f;
	KeyFrame[9].rotRodIzq = -60.0f;
	KeyFrame[9].rotRodDer = 60.0f;
	KeyFrame[9].brazoDer = 50.0f;
	KeyFrame[9].brazoIzq = -50.0f;
	KeyFrame[9].giroMonito = 0.0f;
	KeyFrame[9].giroCabeza = 0.0f;

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);
		
		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.8f, 0.8f, 0.8f));				//Editado. Caras menos iluminadas
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.1f, 0.1f, 0.1f));				//Editado. Caras mas iluminadas
		staticShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));				//Editado. Reflejos con color

		
		staticShader.setVec3("pointLight[0].position", lightPosition);						//Posicion de una fuente de luz
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.5f, 0.5f, 0.5f));			//Editado
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));			//Editado
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);								//Determinar cuanto abarcara 
		staticShader.setFloat("pointLight[0].quadratic", 0.0032f);							//Determinar cuanto abarcara

		/*
		staticShader.setVec3("pointLight[1].position", glm::vec3(180.0f, 1.0f, 0.0f));		//Otra fuente de luz
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.8f, 0.8f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f + valorR, 0.0f + valorG, 0.0f + valorB));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.00032f);

		staticShader.setVec3("pointLight[2].position", glm::vec3(0.0f, 1.0f, 180.0f));		//Otra fuente de luz
		staticShader.setVec3("pointLight[2].ambient", glm::vec3(0.0f, 0.5f, 1.0f));
		staticShader.setVec3("pointLight[2].diffuse", glm::vec3(0.0f, 0.9f, 0.0f));
		staticShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[2].constant", 1.0f);
		staticShader.setFloat("pointLight[2].linear", 0.009f);
		staticShader.setFloat("pointLight[2].quadratic", 0.0032f);
		*/

		staticShader.setVec3("spotLight.position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight.constant", 1.0f);
		staticShader.setFloat("spotLight.linear", 0.0009f);
		staticShader.setFloat("spotLight.quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);
		
		/*
		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);
	
		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.2f));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		animacionPersonaje.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Segundo Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------

		model = glm::translate(glm::mat4(1.0f), glm::vec3(40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		ninja.Draw(animShader);
		*/

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);
		/*
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-80.0f, -1.0f, 0.0f));							//Aqui se implemento el modelo nuevo (casa brujas)
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		casaBrujas.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-80.0f, -1.0f, 80.0f));							//Aqui se implemento el modelo nuevo (cubito)
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		cubito.Draw(staticShader);
		
		model = glm::translate(glm::mat4(1.0f), glm::vec3(300.0f, 0.0f, 300.0f));							//Aqui se implemento el modelo nuevo (ciudad)
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		ciudad.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 0.0f, -10.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		casaDoll.Draw(staticShader);
		*/

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);

		//--------------------------------------------------------------------------------------------------------------------------
		// Casa
		//--------------------------------------------------------------------------------------------------------------------------

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));			// Toda la estructura de la casa
		model = glm::scale(model, glm::vec3(escalaGeneral));
		staticShader.setMat4("model", model);
		casa.Draw(staticShader);
		/*
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 70.0f));			// Solo el techo de la casa
		model = glm::scale(model, glm::vec3(40.0f));
		staticShader.setMat4("model", model);
		techo.Draw(staticShader);
		
		*/
		// Puertas de dormitorios
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f * escalaGeneral, 0.0f, 0.0f));					// Puerta en el dormitorio frontal
		model = glm::rotate(model, glm::radians(giroPuertas), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		staticShader.setMat4("model", model);
		puerta_dorm.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.4f * escalaGeneral, 0.0f, -1.8f * escalaGeneral));	// Puerta en el dormitorio lateral
		model = glm::rotate(model, glm::radians(giroPuertas - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		staticShader.setMat4("model", model);
		puerta_dorm.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f * escalaGeneral, 0.0f, -5.9f * escalaGeneral));	// Puerta en el dormitorio trasero
		model = glm::rotate(model, glm::radians(-giroPuertas), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		staticShader.setMat4("model", model);
		puerta_dorm.Draw(staticShader);
		
		// Puertas de los baños
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.4f * escalaGeneral, 0.0f, -0.1f * escalaGeneral));	// Puerta en el dormitorio trasero
		model = glm::rotate(model, glm::radians(giroPuertas - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		staticShader.setMat4("model", model);
		puerta_ban.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f * escalaGeneral, 0.0f, -5.9f * escalaGeneral));	// Puerta en el dormitorio trasero
		model = glm::rotate(model, glm::radians(giroPuertas - 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		staticShader.setMat4("model", model);
		puerta_ban.Draw(staticShader);

		// Puertas exteriores
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f * escalaGeneral, 0.0f, 0.0f));					// Puerta en el dormitorio frontal
		model = glm::rotate(model, glm::radians(giroPuertas -180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		staticShader.setMat4("model", model);
		puerta_ext.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(3.8f * escalaGeneral, 0.0f, -9.6f * escalaGeneral));					// Puerta en el dormitorio frontal
		model = glm::rotate(model, glm::radians(giroPuertas), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		staticShader.setMat4("model", model);
		puerta_ext.Draw(staticShader);

		//--------------------------------------------------------------------------------------------------------------------------
		// Cama matrimonial
		//--------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.3f * escalaGeneral, 0.0f, 2.1f * escalaGeneral));					// Cama matrimonial
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaGeneral));
		staticShader.setMat4("model", model);
		cama_mat.Draw(staticShader);
		
		//--------------------------------------------------------------------------------------------------------------------------
		// Camas individuales
		//--------------------------------------------------------------------------------------------------------------------------

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.8f * escalaGeneral, 0.0f, -5.8f * escalaGeneral));					// Cama izq. del dormitorio lateral
		model = glm::scale(model, glm::vec3(escalaGeneral));
		staticShader.setMat4("model", model);
		cama.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f * escalaGeneral, 0.0f, -5.8f * escalaGeneral));					// Cama der. del dormitorio lateral
		model = glm::scale(model, glm::vec3(escalaGeneral));
		staticShader.setMat4("model", model);
		cama.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.3f * escalaGeneral, 0.0f, -6.5f * escalaGeneral));					// Cama izq. del dormitorio trasero
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaGeneral));
		staticShader.setMat4("model", model);
		cama.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.3f * escalaGeneral, 0.0f, -9.0f * escalaGeneral));					// Cama der. del dormitorio trasero
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaGeneral));
		staticShader.setMat4("model", model);
		cama.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Carro
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(15.0f + movAuto_x, -1.0f + movAuto_y, movAuto_z));
		tmp = model = glm::rotate(model, glm::radians(orienta), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		carro.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(8.5f, 3.5f, 12.9f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(1.0f, 0.0f, 0.0f));					//Giro de llanta 1
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);	//Izq delantera

		model = glm::translate(tmp, glm::vec3(-8.5f, 3.5f, 12.9f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-giroLlantas), glm::vec3(1.0f, 0.0f, 0.0f));					//Giro de llanta 2
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);	//Der delantera

		model = glm::translate(tmp, glm::vec3(-8.5f, 3.5f, -14.5f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-giroLlantas), glm::vec3(1.0f, 0.0f, 0.0f));					//Giro de llanta 3
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);	//Der trasera

		model = glm::translate(tmp, glm::vec3(8.5f, 3.5f, -14.5f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(1.0f, 0.0f, 0.0f));					//Giro de llanta 4
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);	//Izq trase
		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		model = glm::translate(model, glm::vec3(posX, posY, posZ));
		tmp = model = glm::rotate(model, glm::radians(giroMonito), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		torso.Draw(staticShader);

		//Pierna Der
		model = glm::translate(tmp, glm::vec3(-0.5f, 0.0f, -0.1f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(-rotRodDer), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		piernaDer.Draw(staticShader);

		//Pie Der
		model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", model);
		botaDer.Draw(staticShader);

		//Pierna Izq
		model = glm::translate(tmp, glm::vec3(0.5f, 0.0f, -0.1f));
		model = glm::rotate(model, glm::radians(-rotRodIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		piernaIzq.Draw(staticShader);

		//Pie Iz
		model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", model);
		botaDer.Draw(staticShader);	//Izq trase

		//Brazo derecho
		model = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.75f, 2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-brazoDer), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		brazoDerModel.Draw(staticShader);

		//Brazo izquierdo
		model = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.75f, 2.5f, 0));
		model = glm::rotate(model, glm::radians(-brazoIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		brazoIzqModel.Draw(staticShader);

		//Cabeza
		model = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(giroCabeza), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0));
		staticShader.setMat4("model", model);
		cabeza.Draw(staticShader);
		// -------------------------------------------------------------------------------------------------------------------------
		// Caja Transparente --- Siguiente Práctica
		// -------------------------------------------------------------------------------------------------------------------------
		/*glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -70.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		cubo.Draw(staticShader);
		glEnable(GL_BLEND);*/
		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	//Para configurar la camara
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//Para mover el modelo
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posX++;
	//Para las piernas
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodDer--;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		rotRodDer++;
	//Giro de todo el cuerpo
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		giroMonito++;
	//Movimiento de los brazos
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		brazoDer++;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		brazoDer--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		brazoIzq++;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		brazoIzq--;
	//Movimiento de la cabeza
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		giroCabeza--;
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
		giroCabeza++;

	//R: resetea el carro, cuando ya acabó su animacion
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		if (movAuto_z >= 200.0f)
		{
			movAuto_z = 50.0f;
			animacion = false;
		}
	}

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		movAuto_x = -200.0f;
		animacion2 = false;
		/*
		if (movAuto_z >= 200.0f)
		{
			movAuto_z = 50.0f;
			animacion = false;
		}*/
	}

	// tecla para activar animacion de todas las puertas
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		animacionPuertas ^= true;
	}

	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		animacion ^= true;

		if (movAuto_z == 50.0f)
			etapa1 = true;
	}
	
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		animacion2 ^= true;
		movAuto_z = 0.0f;
		//if (movAuto_z == -200.0f)
			recorrido1 = true;
	}

	//To play KeyFrame animation 
	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
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
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}