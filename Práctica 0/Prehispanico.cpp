// Std. Includes
#include <string>
#include <iostream>
#include <vector>

// AUDIO (MINIAUDIO)
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"
#include "stb_image.h"

// Properties
const GLuint WIDTH = 1280, HEIGHT = 720;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
unsigned int loadCubemap(std::vector<std::string> faces);
void UpdateDayNightCycle();

// VARIABLES GLOBALES PARA AUDIO
ma_engine* g_audioEngine = nullptr;
ma_sound* g_puroHuesoSound = nullptr;
ma_sound* g_ringSound = nullptr;  
ma_sound* g_temploMayorSound = nullptr;  
ma_sound* g_backgroundMusic = nullptr;        
glm::vec3 ringPosition = glm::vec3(0.0f, 0.7f, -0.5f);  
float ringMinDistance = 1.0f;     
float ringMaxDistance = 17.0f;  

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

//VARIABLES PARA LINK
bool thirdPersonMode = true;
glm::vec3 linkPosition = glm::vec3(5.0f, -0.5f, 30.0f);
float linkRotation = 180.0f;
float linkSpeed = 3.5f;
float cameraDistance = 2.0f;  
float cameraHeight = 0.6f;   

//VARIABLES PARA ANIMACIÓN DE LINK
float linkHeadRotX = 0.0f;  
float linkHeadRotY = 0.0f;  
float linkArmLeft = 0.0f;  
float linkArmRight = 0.0f; 
float linkLegLeft = 0.0f;  
float linkLegRight = 0.0f;  
bool linkIsMoving = false;  

//VARIABLES PARA CÁMARA AÉREA (CENITAL)
bool aerialMode = false;
glm::vec3 aerialCameraPosition = glm::vec3(0.0f, 30.0f, 0.0f); 
float aerialHeight = 30.0f;     
float aerialMinHeight = 10.0f; 
float aerialMaxHeight = 80.0f;  
float aerialMoveSpeed = 15.0f;  
float aerialZoomSpeed = 20.0f;  

//VARIABLES PARA CÁMARA DEL TEMPLO MAYOR
bool temploMayorMode = false;  
int temploMayorAngle = 1;     

// Variables para restaurar el estado de la cámara al salir del modo Templo Mayor
glm::vec3 savedCameraPosition;
glm::vec3 savedCameraFront;
glm::vec3 savedCameraUp;
bool wasCameraStateSaved = false;

//Angulos de cámara para el Templo Mayor
struct TemploCameraAngle {
    glm::vec3 position;    
    glm::vec3 target;      
};

TemploCameraAngle temploCameraAngles[4] = {
    // Ángulo 1
    {
        glm::vec3(-2.0f, 15.0f, -50.0f),
        glm::vec3(-35.0f, 10.0f, -50.0f),
    },
    // Ángulo 2
    {
        glm::vec3(-35.0f, 15.0f, -45.0f),
        glm::vec3(-35.0f, 10.0f, -50.0f),
    },
    // Ángulo 3
    {
        glm::vec3(-60.0f, 25.0f, -30.0f),   
        glm::vec3(-35.0f, 8.0f, -50.0f),     
    },
    // Ángulo 4
  {
        glm::vec3(-35.0f, 9.0f, -75.0f),    
        glm::vec3(-35.0f, 12.0f, -50.0f),    
    }
};

//VARIABLES DE CÁMARA
float cameraYaw = 0.0f;      
float cameraPitch = -10.0f; 

// Light attributes
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


// === CICLO DÍA/NOCHE ===
float dayNightCycle = 0.0f;       
float cycleSpeed = 1.0f / 120.0f; 
glm::vec3 dayAmbient = glm::vec3(0.4f, 0.4f, 0.4f);
glm::vec3 nightAmbient = glm::vec3(0.08f, 0.08f, 0.12f);
glm::vec3 dayDiffuse = glm::vec3(1.0f, 1.0f, 0.95f);
glm::vec3 nightDiffuse = glm::vec3(0.15f, 0.15f, 0.22f); 
glm::vec3 currentAmbient = dayAmbient;
glm::vec3 currentDiffuse = dayDiffuse;
glm::vec3 currentSkyColor = glm::vec3(0.5f, 0.7f, 1.0f); 
float sunIntensity = 1.0f;     
glm::vec3 skyboxTint = glm::vec3(1.0f, 1.0f, 1.0f);

//LUCES PUNTUALES (LUMINARIAS)
glm::vec3 pointLightPositions[] = {
    //MERCADOS
    glm::vec3(25.0f, 3.0f, 26.0f),  
    glm::vec3(20.0f, 3.0f, 26.0f),   
    glm::vec3(30.0f, 3.0f, 26.0f),   
	
	//GRAN ÁRBOL DEKU Y ZELDA
	glm::vec3(-18.0f, 3.0f, 32.0f), 
	glm::vec3(-6.0f, 3.0f, 32.0f), 
	glm::vec3(-12.0f, 3.0f, 28.0f),  
	
	//CASA BILLY (Z NEGATIVO)
	glm::vec3(-12.0f, 3.0f, -46.0f), 
	glm::vec3(12.0f, 3.0f, -46.0f),  
	
	//ATLANTES DE TULA
	glm::vec3(25.0f, 3.0f, -45.0f),  
	glm::vec3(25.0f, 3.0f, -35.0f),  
	
	//TEMPLO MAYOR 
	glm::vec3(-20.0f, 3.0f, -50.0f),
	glm::vec3(-20.0f, 3.0f, -40.0f),
	glm::vec3(-20.0f, 3.0f, -60.0f)  
};


// Color blanco para las luces
glm::vec3 pointLightColor = glm::vec3(1.0f, 1.0f, 1.0f);

//LUCES TIPO REFLECTOR (SPOTLIGHTS) - Posiciones ya calculadas
glm::vec3 spotLightPositions[] = {
    glm::vec3(2.828f, 9.55f, 2.328f),    // Esquina 1
    glm::vec3(-2.828f, 9.55f, 2.328f),   // Esquina 2
    glm::vec3(-2.828f, 9.55f, -3.328f),  // Esquina 3
    glm::vec3(2.828f, 9.55f, -3.328f)    // Esquina 4
};

// Direcciones de los spotlights hacia el centro del ring
glm::vec3 spotLightDirections[] = {
    glm::vec3(-0.289f, -0.928f, -0.237f),
    glm::vec3(0.289f, -0.928f, -0.237f),
    glm::vec3(0.289f, -0.928f, 0.287f),
    glm::vec3(-0.289f, -0.928f, 0.287f)
};

// Color de los reflectores
glm::vec3 spotLightColor = glm::vec3(1.0f, 1.0f, 1.0f);

//CONFIGURACIÓN DE SPOTLIGHTS
float spotIntensityAmbient = 0.15f;
float spotIntensityDiffuse = 2.0f;
float spotIntensitySpecular = 1.5f;

// Ángulos del cono 
float spotInnerAngle = 15.0f;
float spotOuterAngle = 22.0f;

void UpdateDayNightCycle()
{
    dayNightCycle += cycleSpeed * deltaTime;
    if (dayNightCycle >= 1.0f)
        dayNightCycle -= 1.0f;

    float timeOfDay = dayNightCycle;
    // cos(0) = 1 (día), cos(π) = -1 (noche)
    sunIntensity = (cos(timeOfDay * 2.0f * 3.14159f) + 1.0f) * 0.5f;

    // Interpolar colores de luz
    currentAmbient = glm::mix(nightAmbient, dayAmbient, sunIntensity);
    currentDiffuse = glm::mix(nightDiffuse, dayDiffuse, sunIntensity);

    // Color del cielo
    glm::vec3 daySky = glm::vec3(0.53f, 0.81f, 0.92f);    // Azul 
    glm::vec3 sunsetSky = glm::vec3(1.0f, 0.5f, 0.2f);    // Naranja 
    glm::vec3 nightSky = glm::vec3(0.02f, 0.02f, 0.1f);   // Azul oscuro 

    // Tinte para el skybox
    glm::vec3 daySkyboxTint = glm::vec3(1.0f, 1.0f, 1.0f);      
    glm::vec3 sunsetSkyboxTint = glm::vec3(1.0f, 0.7f, 0.5f);     // Naranja
    glm::vec3 nightSkyboxTint = glm::vec3(0.15f, 0.15f, 0.3f);    // Azul oscuro

    // Determinar fase del día
    if (sunIntensity > 0.7f) {
        // Día pleno
        currentSkyColor = daySky;
        skyboxTint = daySkyboxTint;
    }
    else if (sunIntensity > 0.3f) {
        // Atardecer/Amanecer
        float t = (sunIntensity - 0.3f) / 0.4f;
        currentSkyColor = glm::mix(sunsetSky, daySky, t);
        skyboxTint = glm::mix(sunsetSkyboxTint, daySkyboxTint, t);
    }
    else if (sunIntensity > 0.1f) {
        // Transición a noche
        float t = (sunIntensity - 0.1f) / 0.2f;
        currentSkyColor = glm::mix(nightSky, sunsetSky, t);
        skyboxTint = glm::mix(nightSkyboxTint, sunsetSkyboxTint, t);
    }
    else {
        // Noche
        currentSkyColor = nightSky;
        skyboxTint = nightSkyboxTint;
    }
}

int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Prehispanico", nullptr, nullptr);

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

    // Setup and compile our shaders
    Shader modelShader("Shader/modelLoading.vs", "Shader/modelLoading.frag");
    Shader lampshader("Shader/lamp.vs", "Shader/lamp.frag");
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
    Shader skyboxShader("Shader/skybox.vs", "Shader/skybox.frag");

    // Rutas del skybox
    std::vector<std::string> faces{
        "images/Skybox/humble_lf.jpg",
        "images/Skybox/humble_rt.jpg",
        "images/Skybox/humble_up.jpg",
        "images/Skybox/humble_dn.jpg",
        "images/Skybox/humble_ft.jpg",
        "images/Skybox/humble_bk.jpg",
    };

    // Cargar cubemap
    unsigned int cubemapTexture = loadCubemap(faces);

    // Asignar el sampler
    skyboxShader.Use();
    glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);

    //Modelos
    //Puro_Hueso
    Model Puro_Hueso((char*)"Models/Puro_Hueso/Reaper_s_Stroll_1122085949_texture.obj");
    //Casa_Billy
    Model Casa((char*)"Models/Casa_Billy/Haunted_House_Mansion_1122090151_texture.obj");
    //Luminaria
    Model Luminaria((char*)"Models/Luminaria/Street_Lamp_Design_1122071902_texture.obj");
    //Mercado
    Model Mercado((char*)"Models/Mercado/Marketplace_Display_1122092509_texture.obj");
    //Mercado_barril
    Model Mercado_barril((char*)"Models/Mercado_barril/Market_Stall_Display_1122090300_texture.obj");
    //Mercado_ropa
    Model Mercado_ropa((char*)"Models/Mercado_ropa/Colorful_Market_Stall_1122091907_texture.obj");
    //Piramide_Luna
    Model Piramide_Luna((char*)"Models/Piramide_de_la_Luna/Pyramid_Viewpoint_1122092635_texture.obj");
    //Piramide_Sol
    Model Piramide_Sol((char*)"Models/Piramide_del_Sol/Pyramid_Serenity_1124091429_texture.obj");
    //Luchadores 
    Model Luchadores((char*)"Models/Luchadores/Luchador_Duo_1122085933_texture.obj");
    //Steven
	Model Steven((char*)"Models/Steven/Wrestler_in_the_Ring_1123110745_texture.obj");
    //Nacho Libre
    Model Nacho((char*)"Models/Nacho_Libre/nacho_libre_1123112607_texture.obj");
    //Ring
    Model Ring((char*)"Models/Ring/Wrestling_Arena_1122090032_texture.obj");
    //Aguila
    Model eagle((char*)"Models/Eagle/model.obj");
	//Atlantes de Tula
	Model Atlantes((char*)"Models/Atlantes_de_Tula/Toltec_Warriors_1123105840_texture.obj");
    //Guerrero Azteca
    Model Azteca((char*)"Models/Guerrero_Azteca/Warrior_Overlooking_R_1123114344_texture.obj");
    //Gran Arbol Deku
	Model DekuTree((char*)"Models/Gran_Arbol_Deku/Great_Deku_Tree_1123110835_texture.obj");
	//Zelda
	Model Zelda((char*)"Models/Zelda/Torchbearer__1123114315_texture.obj");
    //Reflector 
	Model Reflector((char*)"Models/Reflector/Illuminated_Focus_1124010857_texture.obj");
    //Mascaras
	Model Mascaras((char*)"Models/Mascaras/Masks_on_Display_1124011824_texture.obj");
    //Artesania 
	Model Artesania((char*)"Models/Artesania/Market_Treasures_1124011838_texture.obj");
    //xoloitzcuintle
	Model Xoloitzcuintle((char*)"Models/Xoloitzcuintle/Noble_Watcher_1124011950_texture.obj");
    //Marco Luces 
	Model MarcoLuces((char*)"Models/Marco_Luces/Frame_Stand_1124031430_texture.obj");
    //Templo Mayor
    Model TemploMayor((char*)"Models/Templo_Mayor/Tenochtitlan_Temple_M_1124051158_texture.obj");
    //Ixiptla
	Model Ixiptla((char*)"Models/Ixiptla/Ancient_Resting_Figur_1125053550_texture.obj");
    //Gradas
	Model Gradas((char*)"Models/Gradas/Stadium_Bleachers_Des_1125053634_texture.obj");
    //Tlenamacac
	Model Tlenamacac((char*)"Models/Tlenamacac/Mayan_Warrior_Statue_1125053710_texture.obj");
    //Link Head
	Model Link_Head((char*)"Models/Link/LinkHead.obj");
    //Link Body
    Model Link_Body((char*)"Models/Link/LinkBody.obj");
    //Link LA
	Model Link_LA((char*)"Models/Link/LinkLA.obj");
    //Link RA
	Model Link_RA((char*)"Models/Link/LinkRA.obj");
    //Link LL
    Model Link_LL((char*)"Models/Link/LinkLL.obj");
	//Link RL
	Model Link_RL((char*)"Models/Link/LinkRL.obj");

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    // Skybox setup
    float skyboxVertices[] = {
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

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    float floorVertices[] = {
         100.0f, -0.5f,  100.0f,   0.0f, 1.0f, 0.0f,   100.0f, 0.0f,
        -100.0f, -0.5f,  100.0f,   0.0f, 1.0f, 0.0f,   0.0f,   0.0f,
        -100.0f, -0.5f, -100.0f,   0.0f, 1.0f, 0.0f,   0.0f,   100.0f,

         100.0f, -0.5f,  100.0f,   0.0f, 1.0f, 0.0f,   100.0f, 0.0f,
        -100.0f, -0.5f, -100.0f,   0.0f, 1.0f, 0.0f,   0.0f,   100.0f,
         100.0f, -0.5f, -100.0f,   0.0f, 1.0f, 0.0f,   100.0f, 100.0f
    };

    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);

    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    unsigned int floorTexture;
    glGenTextures(1, &floorTexture);
    glBindTexture(GL_TEXTURE_2D, floorTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("images/grass.jpg", &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Error al cargar textura del piso" << std::endl;
    }

    stbi_image_free(data);

    //AUDIO
    ma_engine audioEngine;
    ma_result result;

    result = ma_engine_init(NULL, &audioEngine);
    if (result != MA_SUCCESS) {
        std::cout << "ERROR: Fallo al inicializar el motor de audio." << std::endl;
    }
    g_audioEngine = &audioEngine;

    //MÚSICA DE FONDO
    ma_sound backgroundMusic;
    result = ma_sound_init_from_file(&audioEngine, "audio/sacred-garden-10377.mp3",
        0, NULL, NULL, &backgroundMusic);
    if (result != MA_SUCCESS) {
        std::cout << "ERROR: No se pudo cargar música de fondo." << std::endl;
        g_backgroundMusic = nullptr;
    }
    else {
        ma_sound_set_spatialization_enabled(&backgroundMusic, MA_FALSE);
        ma_sound_set_looping(&backgroundMusic, MA_TRUE);
        ma_sound_set_volume(&backgroundMusic, 0.3f); 
        ma_sound_start(&backgroundMusic);
        g_backgroundMusic = &backgroundMusic;
    }

    //SONIDO DE PURO HUESO
    ma_sound puroHuesoSound;
    result = ma_sound_init_from_file(&audioEngine, "audio/puro hueso.mp3",
        MA_SOUND_FLAG_DECODE, NULL, NULL, &puroHuesoSound);
    if (result != MA_SUCCESS) {
        std::cout << "ERROR: No se pudo cargar 'puro hueso.mp3'." << std::endl;
        g_puroHuesoSound = nullptr;
    }
    else {
        ma_sound_set_spatialization_enabled(&puroHuesoSound, MA_FALSE);
        ma_sound_set_volume(&puroHuesoSound, 0.5f);
        g_puroHuesoSound = &puroHuesoSound;
    }

   //SONIDO 3D DEL RING
    ma_sound ringSound;
    result = ma_sound_init_from_file(&audioEngine, "audio/Religious Man.mp3",
        MA_SOUND_FLAG_STREAM, NULL, NULL, &ringSound);
    if (result != MA_SUCCESS) {
        std::cout << "ERROR: No se pudo cargar 'Religious Man.mp3'." << std::endl;
        g_ringSound = nullptr;
    }
    else {
        ma_sound_set_spatialization_enabled(&ringSound, MA_TRUE);
        ma_sound_set_positioning(&ringSound, ma_positioning_absolute);
        ma_sound_set_position(&ringSound, ringPosition.x, ringPosition.y, ringPosition.z);
        ma_sound_set_attenuation_model(&ringSound, ma_attenuation_model_linear);
        ma_sound_set_min_distance(&ringSound, ringMinDistance);  
        ma_sound_set_max_distance(&ringSound, ringMaxDistance);  
        ma_sound_set_rolloff(&ringSound, 1.0f);  
        ma_sound_set_cone(&ringSound, 360.0f, 360.0f, 1.0f);
        ma_sound_set_doppler_factor(&ringSound, 0.0f);
        ma_sound_set_volume(&ringSound, 0.7f); 
        g_ringSound = &ringSound;
    }

    //SONIDO DEL TEMPLO MAYOR
    ma_sound temploMayorSound;
    result = ma_sound_init_from_file(&audioEngine, "audio/Danza a Mictlantecuhtli.mp3",
        MA_SOUND_FLAG_STREAM, NULL, NULL, &temploMayorSound);
    if (result != MA_SUCCESS) {
        std::cout << "ERROR: No se pudo cargar 'Danza a Mictlantecuhtli.mp3'." << std::endl;
        g_temploMayorSound = nullptr;
    }
    else {
        ma_sound_set_looping(&temploMayorSound, MA_TRUE);
        ma_sound_set_volume(&temploMayorSound, 10.0f);  
        g_temploMayorSound = &temploMayorSound;
    }

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        DoMovement();
        UpdateDayNightCycle(); 

        //LISTENER
        if (g_audioEngine != nullptr) {
            glm::vec3 listenerPos = linkPosition + glm::vec3(0.0f, 1.5f, 0.0f);
            float linkRad = glm::radians(linkRotation);
            glm::vec3 listenerFront = glm::vec3(sin(linkRad), 0.0f, cos(linkRad));
            glm::vec3 listenerUp = glm::vec3(0.0f, 1.0f, 0.0f);
            ma_engine_listener_set_position(g_audioEngine, 0, listenerPos.x, listenerPos.y, listenerPos.z);
            ma_engine_listener_set_direction(g_audioEngine, 0, listenerFront.x, listenerFront.y, listenerFront.z);
            ma_engine_listener_set_world_up(g_audioEngine, 0, listenerUp.x, listenerUp.y, listenerUp.z);
            ma_engine_listener_set_cone(g_audioEngine, 0, 360.0f, 360.0f, 1.0f);
            if (g_ringSound != nullptr && g_backgroundMusic != nullptr) {
                float distanceToRing = glm::distance(listenerPos, ringPosition);
                
                //En el modo Templo Mayor, nunca iniciar el ring ni la música de fondo aquí
                if (temploMayorMode) {
                    if (ma_sound_is_playing(g_ringSound)) {
                        ma_sound_stop(g_ringSound);
                    }
                    if (ma_sound_is_playing(g_backgroundMusic)) {
                        ma_sound_stop(g_backgroundMusic);
                    }
                }
                else {
                    //Dentro del rango del ringiniciar ring y pausar música de fondo
                    if (distanceToRing < ringMaxDistance) {
                        if (!ma_sound_is_playing(g_ringSound)) {
                            ma_sound_start(g_ringSound);
                        }
                        if (ma_sound_is_playing(g_backgroundMusic)) {
                            ma_sound_stop(g_backgroundMusic);
                        }
                    }
                    else {
                        // Fuera del rango detener sonido del ring
                        if (ma_sound_is_playing(g_ringSound)) {
                            ma_sound_stop(g_ringSound);
                        }
                        // Reanudar música de fondo 
                        if (!ma_sound_is_playing(g_backgroundMusic)) {
                            ma_sound_start(g_backgroundMusic);
                        }
                    }
                }
             }
        }

        // Usar color del cielo dinámico
        glm::vec3 clearColor = currentSkyColor;
        if (sunIntensity < 0.15f) {
            glm::vec3 sceneLevel = currentAmbient * 0.6f + currentDiffuse * 0.05f;
            sceneLevel = glm::clamp(sceneLevel, glm::vec3(0.01f), glm::vec3(0.35f));
            clearColor = sceneLevel;
        }
        glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Intensidad fija=
        float lampIntensity = 1.0f;

        //Dibujar Piso
        lightingShader.Use();

        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", projection);

        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        lightingShader.setVec3("viewPos", camera.GetPosition());

        //Luz direccional dinámica día/noche
        lightingShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        lightingShader.setVec3("dirLight.ambient", currentAmbient);
        lightingShader.setVec3("dirLight.diffuse", currentDiffuse);
        lightingShader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        //Configurar luces puntuales
        for (int i = 0; i < 13; i++)
        {
            std::string number = std::to_string(i);
            lightingShader.setVec3("pointLights[" + number + "].position", pointLightPositions[i]);
            lightingShader.setVec3("pointLights[" + number + "].ambient", pointLightColor * 0.4f * lampIntensity);
            lightingShader.setVec3("pointLights[" + number + "].diffuse", pointLightColor * 2.0f * lampIntensity);
            lightingShader.setVec3("pointLights[" + number + "].specular", pointLightColor * 1.5f * lampIntensity);
            lightingShader.setFloat("pointLights[" + number + "].constant", 1.0f);
            lightingShader.setFloat("pointLights[" + number + "].linear", 0.22f);
            lightingShader.setFloat("pointLights[" + number + "].quadratic", 0.20f);
        }

        //Configurar spotlights
        for (int i = 0; i < 4; i++)
        {
            std::string number = std::to_string(i);
            lightingShader.setVec3("spotLights[" + number + "].position", spotLightPositions[i]);
            lightingShader.setVec3("spotLights[" + number + "].direction", spotLightDirections[i]);
            lightingShader.setVec3("spotLights[" + number + "].ambient", spotLightColor * spotIntensityAmbient * lampIntensity);
            lightingShader.setVec3("spotLights[" + number + "].diffuse", spotLightColor * spotIntensityDiffuse * lampIntensity);
            lightingShader.setVec3("spotLights[" + number + "].specular", spotLightColor * spotIntensitySpecular * lampIntensity);
            lightingShader.setFloat("spotLights[" + number + "].cutOff", glm::cos(glm::radians(spotInnerAngle)));
            lightingShader.setFloat("spotLights[" + number + "].outerCutOff", glm::cos(glm::radians(spotOuterAngle)));
            lightingShader.setFloat("spotLights[" + number + "].constant", 1.0f);
            lightingShader.setFloat("spotLights[" + number + "].linear", 0.09f);
            lightingShader.setFloat("spotLights[" + number + "].quadratic", 0.032f);
        }

        lightingShader.setVec3("material.ambient", glm::vec3(1.0f));
        lightingShader.setVec3("material.diffuse", glm::vec3(1.0f));
        lightingShader.setVec3("material.specular", glm::vec3(0.5f));
        lightingShader.setFloat("material.shininess", 32.0f);

        lightingShader.setInt("texture_diffuse", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        //Dibujar
        modelShader.Use();
        modelShader.setMat4("view", view);
        modelShader.setMat4("projection", projection);

        //Configurar luz direccional para los modelos  día/noche
        modelShader.setVec3("viewPos", camera.GetPosition());
        modelShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        modelShader.setVec3("dirLight.ambient", currentAmbient);
        modelShader.setVec3("dirLight.diffuse", currentDiffuse);
        modelShader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        //Configurar luces puntuales
        for (int i = 0; i < 13; i++)
        {
            std::string number = std::to_string(i);
            modelShader.setVec3("pointLights[" + number + "].position", pointLightPositions[i]);
            modelShader.setVec3("pointLights[" + number + "].ambient", pointLightColor * 0.3f * lampIntensity);
            modelShader.setVec3("pointLights[" + number + "].diffuse", pointLightColor * 1.2f * lampIntensity);
            modelShader.setVec3("pointLights[" + number + "].specular", pointLightColor * 0.8f * lampIntensity);
            modelShader.setFloat("pointLights[" + number + "].constant", 1.0f);
            modelShader.setFloat("pointLights[" + number + "].linear", 0.09f);
            modelShader.setFloat("pointLights[" + number + "].quadratic", 0.032f);
        }

        //spotlights
        for (int i = 0; i < 4; i++)
        {
            std::string number = std::to_string(i);
            modelShader.setVec3("spotLights[" + number + "].position", spotLightPositions[i]);
            modelShader.setVec3("spotLights[" + number + "].direction", spotLightDirections[i]);
            // Intensidad 
            modelShader.setVec3("spotLights[" + number + "].ambient", spotLightColor * spotIntensityAmbient * lampIntensity);
            modelShader.setVec3("spotLights[" + number + "].diffuse", spotLightColor * spotIntensityDiffuse * lampIntensity);
            modelShader.setVec3("spotLights[" + number + "].specular", spotLightColor * spotIntensitySpecular * lampIntensity);
            
            // Ángulos del cono
            modelShader.setFloat("spotLights[" + number + "].cutOff", glm::cos(glm::radians(spotInnerAngle)));
            modelShader.setFloat("spotLights[" + number + "].outerCutOff", glm::cos(glm::radians(spotOuterAngle)));
            
            // Atenuación
            modelShader.setFloat("spotLights[" + number + "].constant", 1.0f);
            modelShader.setFloat("spotLights[" + number + "].linear", 0.09f);
            modelShader.setFloat("spotLights[" + number + "].quadratic", 0.032f);
        }

        glm::mat4 modelTemp = glm::mat4(1.0f);

		//MODELO PRINCIPAL: LINK    
        //(MODELO JERÁRQUICO)
        glm::mat4 linkTransform = modelTemp;
        linkTransform = glm::translate(linkTransform, linkPosition);
        linkTransform = glm::rotate(linkTransform, glm::radians(linkRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        linkTransform = glm::scale(linkTransform, glm::vec3(0.7f, 0.7f, 0.7f));

        // Cuerpo de Link 
        model = linkTransform;
        modelShader.setMat4("model", model);
        Link_Body.Draw(modelShader);

        // Cabeza de Link 
        model = linkTransform; 
        model = glm::translate(model, glm::vec3(-0.002047f, 1.64328f, -0.125752f)); 
        model = glm::rotate(model, glm::radians(linkHeadRotY), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw (mouse horizontal)
        model = glm::rotate(model, glm::radians(linkHeadRotX), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch (mouse vertical)
        modelShader.setMat4("model", model);
        Link_Head.Draw(modelShader);

        // Brazo Izquierdo de Link
        model = linkTransform; 
        model = glm::translate(model, glm::vec3(0.185214f, 1.45944f, -0.141444f)); 
        model = glm::rotate(model, glm::radians(linkArmLeft), glm::vec3(1.0f, 0.0f, 0.0f)); 
        modelShader.setMat4("model", model);
        Link_LA.Draw(modelShader);

        // Brazo Derecho de Link
        model = linkTransform; 
        model = glm::translate(model, glm::vec3(-0.186242f, 1.46128f,-0.138318f)); 
        model = glm::rotate(model, glm::radians(linkArmRight), glm::vec3(1.0f, 0.0f, 0.0f));
        modelShader.setMat4("model", model);
        Link_RA.Draw(modelShader);

        // Pierna Izquierda de Link
        model = linkTransform; 
        model = glm::translate(model, glm::vec3(0.088605f, 0.92654f,-0.105877f)); 
        model = glm::rotate(model, glm::radians(linkLegLeft), glm::vec3(1.0f, 0.0f, 0.0f)); 
        modelShader.setMat4("model", model);
        Link_LL.Draw(modelShader);

        // Pierna Derecha de Link
        model = linkTransform; 
        model = glm::translate(model, glm::vec3(-0.095633f, 0.926898f,-0.109903f)); 
        model = glm::rotate(model, glm::radians(linkLegRight), glm::vec3(1.0f, 0.0f, 0.0f)); 
        modelShader.setMat4("model", model);
        Link_RL.Draw(modelShader);

		//ZONA DE BILLY Y MANDY
        // Casa de Billy
        glm::mat4 casaBilly = modelTemp;
        casaBilly = glm::translate(casaBilly, glm::vec3(0.0f, -1.0f, -45.0f));
        casaBilly = glm::rotate(casaBilly, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        casaBilly = glm::scale(casaBilly, glm::vec3(1.3f, 1.3f, 1.3f));
        modelShader.setMat4("model", casaBilly);
        Casa.Draw(modelShader);

        // Puro_Hueso 
        glm::mat4 puroHuesoTransform = modelTemp;
        puroHuesoTransform = glm::translate(puroHuesoTransform, glm::vec3(0.0f, -0.3f, -38.0f));
        puroHuesoTransform = glm::scale(puroHuesoTransform, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", puroHuesoTransform);
        Puro_Hueso.Draw(modelShader);

		//MONUMENTOS PREHISPÁNICOS
        // Pirámide del Sol
        glm::mat4 piramideSol = modelTemp;
        piramideSol = glm::translate(piramideSol, glm::vec3(-50.0f, -0.5f, 0.0f));
        piramideSol = glm::rotate(piramideSol, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        piramideSol = glm::scale(piramideSol, glm::vec3(1.7f, 1.7f, 1.7f));
        modelShader.setMat4("model", piramideSol);
        Piramide_Sol.Draw(modelShader);

        // Pirámide de la Luna
        glm::mat4 piramideLuna = modelTemp;
        piramideLuna = glm::translate(piramideLuna, glm::vec3(45.0f, -0.5f, 0.0f));
        piramideLuna = glm::rotate(piramideLuna, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        piramideLuna = glm::scale(piramideLuna, glm::vec3(1.4f, 1.4f, 1.4f));
        modelShader.setMat4("model", piramideLuna);
        Piramide_Luna.Draw(modelShader);

		//Atlantes de Tula
		glm::mat4 atlantes = modelTemp;
		atlantes = glm::translate(atlantes, glm::vec3(35.0f, -0.5f, -40.0f));
		atlantes = glm::rotate(atlantes, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		atlantes = glm::scale(atlantes, glm::vec3(1.5f, 1.5f, 1.5f));
		modelShader.setMat4("model", atlantes);
		Atlantes.Draw(modelShader);

		//Templo Mayor
		glm::mat4 temploMayorBase = modelTemp;
		temploMayorBase = glm::translate(temploMayorBase, glm::vec3(-35.0f, -0.5f, -50.0f));
	    temploMayorBase = glm::rotate(temploMayorBase, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		temploMayorBase = glm::scale(temploMayorBase, glm::vec3(1.5f, 1.5f, 1.5f));
		modelShader.setMat4("model", temploMayorBase);
		TemploMayor.Draw(modelShader);

		//MODELOS JERÁRQUICO SOBRE EL TEMPLO MAYOR
		// Ixiptla (Víctima del sacrificio)
		glm::mat4 ixiptla = temploMayorBase;
		ixiptla = glm::translate(ixiptla, glm::vec3(0.0f, 7.2f, 1.0f)); 
		ixiptla = glm::rotate(ixiptla, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ixiptla = glm::scale(ixiptla, glm::vec3(1.0f, 1.0f, 1.0f));
		modelShader.setMat4("model", ixiptla);
		Ixiptla.Draw(modelShader);

		// Tlenamacac (Sacerdote)
		glm::mat4 tlenamacac = temploMayorBase;
		tlenamacac = glm::translate(tlenamacac, glm::vec3(1.2f, 7.2f, 1.0f)); 
		tlenamacac = glm::rotate(tlenamacac, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		tlenamacac = glm::scale(tlenamacac, glm::vec3(1.0f, 1.0f, 1.0f));
		modelShader.setMat4("model", tlenamacac);
		Tlenamacac.Draw(modelShader);

        // Águila
        {
            float timeValue = glfwGetTime();
            float radius = 8.0f;  // Radio del círculo
            float centerX = -35.0f;  
            float centerZ = -50.0f;  
            float eagleX = centerX + radius * cos(timeValue * 0.5f);
            float eagleZ = centerZ + radius * sin(timeValue * 0.5f);
            float floatY = sin(timeValue * 0.8f) * 0.5f;

            glm::mat4 aguila = modelTemp;
            aguila = glm::translate(aguila, glm::vec3(eagleX, 17.0f + floatY, eagleZ));  // Altura sobre el templo
            aguila = glm::rotate(aguila, timeValue * 0.5f + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            aguila = glm::scale(aguila, glm::vec3(0.015f));
            modelShader.setMat4("model", aguila);
            eagle.Draw(modelShader);
        }

		//MODELOS DE ZELDA
        //Gran Arbol Deku 
		glm::mat4 arbolDeku = modelTemp;
		arbolDeku = glm::translate(arbolDeku, glm::vec3(-15.0f, -0.5f, 45.0f));
		arbolDeku = glm::rotate(arbolDeku, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		arbolDeku = glm::scale(arbolDeku, glm::vec3(4.0f, 4.0f, 4.0f));
		modelShader.setMat4("model", arbolDeku);
		DekuTree.Draw(modelShader);

        //Zelda 
		glm::mat4 zelda = modelTemp;
		zelda = glm::translate(zelda, glm::vec3(-12.0f, -0.5f, 35.0f));
		zelda = glm::rotate(zelda, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		zelda = glm::scale(zelda, glm::vec3(1.0f, 1.0f, 1.0f));
		modelShader.setMat4("model", zelda);
		Zelda.Draw(modelShader);

		//ZONA DE MERCADOS

        // Mercado principal
        glm::mat4 mercadoPrincipal = modelTemp;
        mercadoPrincipal = glm::translate(mercadoPrincipal, glm::vec3(25.0f, -0.5f, 35.0f));
        mercadoPrincipal = glm::rotate(mercadoPrincipal, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mercadoPrincipal = glm::scale(mercadoPrincipal, glm::vec3(1.5f, 1.5f, 1.5f));
        modelShader.setMat4("model", mercadoPrincipal);
        Mercado.Draw(modelShader);

        //Mercado de ropa
        glm::mat4 mercadoRopa = modelTemp;
        mercadoRopa = glm::translate(mercadoRopa, glm::vec3(20.0f, -0.5f, 35.0f));
        mercadoRopa = glm::rotate(mercadoRopa, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mercadoRopa = glm::scale(mercadoRopa, glm::vec3(1.5f, 1.5f, 1.5f));
        modelShader.setMat4("model", mercadoRopa);
        Mercado_ropa.Draw(modelShader);

        // Mercado con barriles 
        glm::mat4 mercadoBarril = modelTemp;
        mercadoBarril = glm::translate(mercadoBarril, glm::vec3(30.0f, -0.5f, 35.0f));
        mercadoBarril = glm::rotate(mercadoBarril, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mercadoBarril = glm::scale(mercadoBarril, glm::vec3(1.5f, 1.5f, 1.5f));
        modelShader.setMat4("model", mercadoBarril);
        Mercado_barril.Draw(modelShader);

        //Mascaras 
        glm::mat4 mascaras = modelTemp;
        mascaras = glm::translate(mascaras, glm::vec3(17.0f, -0.5f, 35.0f));
        mascaras = glm::rotate(mascaras, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mascaras = glm::scale(mascaras, glm::vec3(1.5f, 1.5f, 1.5f));
        modelShader.setMat4("model", mascaras);
        Mascaras.Draw(modelShader);

        //Artesanias 
        glm::mat4 artesania = modelTemp;
        artesania = glm::translate(artesania, glm::vec3(14.0f, -0.5f, 35.0f));
        artesania = glm::rotate(artesania, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        artesania = glm::scale(artesania, glm::vec3(1.5f, 1.5f, 1.5f));
        modelShader.setMat4("model", artesania);
        Artesania.Draw(modelShader);

        //xoloitzcuintle 
        glm::mat4 xoloitzcuintle = modelTemp;
        xoloitzcuintle = glm::translate(xoloitzcuintle, glm::vec3(25.0f, -0.5f, 32.0f));
        xoloitzcuintle = glm::rotate(xoloitzcuintle, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        xoloitzcuintle = glm::scale(xoloitzcuintle, glm::vec3(1.5f, 1.5f, 1.5f));
        modelShader.setMat4("model", xoloitzcuintle);
        Xoloitzcuintle.Draw(modelShader);

		//Guerrero Azteca
		glm::mat4 guerreroAzteca = modelTemp;
		guerreroAzteca = glm::translate(guerreroAzteca, glm::vec3(20.0f, -0.5f, 32.0f));
		guerreroAzteca = glm::rotate(guerreroAzteca, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		guerreroAzteca = glm::scale(guerreroAzteca, glm::vec3(1.0f, 1.0f, 1.0f));
		modelShader.setMat4("model", guerreroAzteca);
		Azteca.Draw(modelShader);


        //RING CENTRAL
        glm::mat4 MexicanRing = modelTemp;
        MexicanRing = glm::translate(MexicanRing, glm::vec3(0.0f, -0.5f, -0.5f));
        MexicanRing = glm::rotate(MexicanRing, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        MexicanRing = glm::scale(MexicanRing, glm::vec3(1.3f, 1.3f, 1.3f));
        modelShader.setMat4("model", MexicanRing);
        Ring.Draw(modelShader);

        //MARCO DE LUCES ALREDEDOR DEL RING
        glm::mat4 marcoLucesBase = modelTemp;
        marcoLucesBase = glm::translate(marcoLucesBase, glm::vec3(0.0f, -0.5f, -0.5f));
        marcoLucesBase = glm::rotate(marcoLucesBase, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        marcoLucesBase = glm::scale(marcoLucesBase, glm::vec3(3.0f, 3.0f, 3.0f));
        modelShader.setMat4("model", marcoLucesBase);
        MarcoLuces.Draw(modelShader);

        //REFLECTORES EN LAS 4 ESQUINAS
        float reflectorDistance = 1.53f; // Distancia desde el centro del ring
        
        // Reflector 1
        glm::mat4 reflector1 = marcoLucesBase;
        reflector1 = glm::translate(reflector1, glm::vec3(reflectorDistance, 3.35f, reflectorDistance));
        reflector1 = glm::rotate(reflector1, glm::radians(-135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        reflector1 = glm::scale(reflector1, glm::vec3(0.9f, 0.9f, 0.9f));
        modelShader.setMat4("model", reflector1);
        Reflector.Draw(modelShader);

        // Reflector 2
        glm::mat4 reflector2 = marcoLucesBase;
        reflector2 = glm::translate(reflector2, glm::vec3(-reflectorDistance, 3.35f, reflectorDistance));
        reflector2 = glm::rotate(reflector2, glm::radians(130.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        reflector2 = glm::scale(reflector2, glm::vec3(0.9f, 0.9f, 0.9f));
        modelShader.setMat4("model", reflector2);
        Reflector.Draw(modelShader);

        // Reflector 3
        glm::mat4 reflector3 = marcoLucesBase;
        reflector3 = glm::translate(reflector3, glm::vec3(-reflectorDistance, 3.35f, -reflectorDistance));
        reflector3 = glm::rotate(reflector3, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        reflector3 = glm::scale(reflector3, glm::vec3(0.9f, 0.9f, 0.9f));
        modelShader.setMat4("model", reflector3);
        Reflector.Draw(modelShader);

        // Reflector 4
        glm::mat4 reflector4 = marcoLucesBase;
        reflector4 = glm::translate(reflector4, glm::vec3(reflectorDistance, 3.35f, -reflectorDistance));
        reflector4 = glm::rotate(reflector4, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        reflector4 = glm::scale(reflector4, glm::vec3(0.9f, 0.9f, 0.9f));
        modelShader.setMat4("model", reflector4);
        Reflector.Draw(modelShader);
        
        //GRADAS ALREDEDOR DEL RING
        float gradasLocalDistance = 4.2f;  // Distanciadel ring
        
        // Gradería 1
        glm::mat4 gradas1 = marcoLucesBase;
        gradas1 = glm::translate(gradas1, glm::vec3(0.0f, 0.0f, gradasLocalDistance));
        gradas1 = glm::rotate(gradas1, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        gradas1 = glm::scale(gradas1, glm::vec3(0.30f, 0.30f, 0.30f));
        modelShader.setMat4("model", gradas1);
        Gradas.Draw(modelShader);
    
        // Gradería 2
        glm::mat4 gradas2 = marcoLucesBase;
        gradas2 = glm::translate(gradas2, glm::vec3(0.0f, 0.0f, -gradasLocalDistance));
        gradas2 = glm::rotate(gradas2, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        gradas2 = glm::scale(gradas2, glm::vec3(0.30f, 0.30f, 0.30f));
        modelShader.setMat4("model", gradas2);
        Gradas.Draw(modelShader);

        // Gradería 3
        glm::mat4 gradas3 = marcoLucesBase;
        gradas3 = glm::translate(gradas3, glm::vec3(gradasLocalDistance, 0.0f, 0.0f));
        gradas3 = glm::rotate(gradas3, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        gradas3 = glm::scale(gradas3, glm::vec3(0.30f, 0.30f, 0.30f));
        modelShader.setMat4("model", gradas3);
        Gradas.Draw(modelShader);

        // Gradería 4
        glm::mat4 gradas4 = marcoLucesBase;
        gradas4 = glm::translate(gradas4, glm::vec3(-gradasLocalDistance, 0.0f, 0.0f));
        gradas4 = glm::rotate(gradas4, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        gradas4 = glm::scale(gradas4, glm::vec3(0.30f, 0.30f, 0.30f));
        modelShader.setMat4("model", gradas4);
        Gradas.Draw(modelShader);
        
        //LUCHADORES
        //Santo y Blue Demon
        glm::mat4 luchadoresRing = modelTemp;
        luchadoresRing = glm::translate(luchadoresRing, glm::vec3(-2.0f, 0.7f, -0.5f));
        luchadoresRing = glm::rotate(luchadoresRing, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        luchadoresRing = glm::scale(luchadoresRing, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luchadoresRing);
        Luchadores.Draw(modelShader);
        
        // Steven
        glm::mat4 steven = modelTemp;
        steven = glm::translate(steven, glm::vec3(2.0f, 0.7f, -1.0f));
        steven = glm::rotate(steven, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        steven = glm::scale(steven, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", steven);
        Steven.Draw(modelShader);
       
        // Nacho Libre
        glm::mat4 nachoLibre = modelTemp;
        nachoLibre = glm::translate(nachoLibre, glm::vec3(2.0f, 0.7f, -2.0f));
        nachoLibre = glm::rotate(nachoLibre, glm::radians(-80.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        nachoLibre = glm::scale(nachoLibre, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", nachoLibre);
        Nacho.Draw(modelShader);

        //LUMINARIAS      
       // Luminaria 1 (mercado)
        glm::mat4 luminaria1Base = modelTemp;
        luminaria1Base = glm::translate(luminaria1Base, glm::vec3(25.0f, -0.5f, 26.0f));
        luminaria1Base = glm::scale(luminaria1Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria1Base);
        Luminaria.Draw(modelShader);

        // Luminaria 2 (mercado)
        glm::mat4 luminaria2Base = modelTemp;
        luminaria2Base = glm::translate(luminaria2Base, glm::vec3(20.0f, -0.5f, 26.0f));
        luminaria2Base = glm::scale(luminaria2Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria2Base);
        Luminaria.Draw(modelShader);

        // Luminaria 3 (mercado)
        glm::mat4 luminaria3Base = modelTemp;
        luminaria3Base = glm::translate(luminaria3Base, glm::vec3(30.0f, -0.5f, 26.0f));
        luminaria3Base = glm::scale(luminaria3Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria3Base);
        Luminaria.Draw(modelShader);

        // Luminaria 4 (árbol Deku)
        glm::mat4 luminaria4Base = modelTemp;
        luminaria4Base = glm::translate(luminaria4Base, glm::vec3(-18.0f, -0.5f, 32.0f));
        luminaria4Base = glm::scale(luminaria4Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria4Base);
        Luminaria.Draw(modelShader);

        // Luminaria 5 (árbol Deku)
        glm::mat4 luminaria5Base = modelTemp;
        luminaria5Base = glm::translate(luminaria5Base, glm::vec3(-6.0f, -0.5f, 32.0f));
        luminaria5Base = glm::scale(luminaria5Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria5Base);
        Luminaria.Draw(modelShader);

        // Luminaria 6 (árbol Deku)
        glm::mat4 luminaria6Base = modelTemp;
        luminaria6Base = glm::translate(luminaria6Base, glm::vec3(-12.0f, -0.5f, 28.0f));
        luminaria6Base = glm::scale(luminaria6Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria6Base);
        Luminaria.Draw(modelShader);

        // Luminaria 7 (Casa Billy)
        glm::mat4 luminaria7Base = modelTemp;
        luminaria7Base = glm::translate(luminaria7Base, glm::vec3(-12.0f, -0.5f, -46.0f));
        luminaria7Base = glm::scale(luminaria7Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria7Base);
        Luminaria.Draw(modelShader);

        // Luminaria 8 (Casa Billy)
        glm::mat4 luminaria8Base = modelTemp;
        luminaria8Base = glm::translate(luminaria8Base, glm::vec3(12.0f, -0.5f, -46.0f));
        luminaria8Base = glm::scale(luminaria8Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria8Base);
        Luminaria.Draw(modelShader);

        // Luminaria 9 (Atlantes)
        glm::mat4 luminaria9Base = modelTemp;
        luminaria9Base = glm::translate(luminaria9Base, glm::vec3(25.0f, -0.5f, -45.0f));
        luminaria9Base = glm::scale(luminaria9Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria9Base);
        Luminaria.Draw(modelShader);

        // Luminaria 10 (Atlantes)
        glm::mat4 luminaria10Base = modelTemp;
        luminaria10Base = glm::translate(luminaria10Base, glm::vec3(25.0f, -0.5f, -35.0f));
        luminaria10Base = glm::scale(luminaria10Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria10Base);
        Luminaria.Draw(modelShader);

        // Luminaria 11 (Templo Mayor)
        glm::mat4 luminaria11Base = modelTemp;
        luminaria11Base = glm::translate(luminaria11Base, glm::vec3(-20.0f, -0.5f, -50.0f));
        luminaria11Base = glm::scale(luminaria11Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria11Base);
        Luminaria.Draw(modelShader);

        // Luminaria 12 (Templo Mayor)
        glm::mat4 luminaria12Base = modelTemp;
        luminaria12Base = glm::translate(luminaria12Base, glm::vec3(-20.0f, -0.5f, -40.0f));
        luminaria12Base = glm::scale(luminaria12Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria12Base);
        Luminaria.Draw(modelShader);

        // Luminaria 13 (Templo Mayor)
        glm::mat4 luminaria13Base = modelTemp;
        luminaria13Base = glm::translate(luminaria13Base, glm::vec3(-20.0f, -0.5f, -60.0f));
        luminaria13Base = glm::scale(luminaria13Base, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", luminaria13Base);
        Luminaria.Draw(modelShader);

        //Dibujar Skybox
        glDepthFunc(GL_LEQUAL);
        skyboxShader.Use();

        glm::mat4 viewSkybox = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", viewSkybox);
        skyboxShader.setMat4("projection", projection);

        // Enviar uniforms del ciclo día/noche al skybox
        float skyboxMixMin = 0.4f; 
        float dayNightMixForShader = glm::max(sunIntensity, skyboxMixMin);
        skyboxShader.setVec3("dayNightTint", skyboxTint);
        skyboxShader.setFloat("dayNightMix", dayNightMixForShader);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
    }

    //AUDIO CLEANUP
    if (g_ringSound != nullptr) {
        ma_sound_uninit(g_ringSound);
    }
    if (g_puroHuesoSound != nullptr) {
        ma_sound_uninit(g_puroHuesoSound);
    }
    if (g_temploMayorSound != nullptr) {
        ma_sound_uninit(g_temploMayorSound);
    }
    if (g_backgroundMusic != nullptr) {
        ma_sound_uninit(g_backgroundMusic);
    }
    ma_engine_uninit(&audioEngine);

    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);

    glfwTerminate();
    return 0;
}

void DoMovement()
{
    if (temploMayorMode)
    {
        //CÁMARA DEL TEMPLO MAYOR
        TemploCameraAngle& currentAngle = temploCameraAngles[temploMayorAngle - 1];
        camera.SetPosition(currentAngle.position);
        glm::vec3 direction = glm::normalize(currentAngle.target - currentAngle.position);
        camera.SetFront(direction);
    }
    else if (aerialMode)
    {
        //MODO CÁMARA AÉREA
        //Zoom In
        if (keys[GLFW_KEY_EQUAL] || keys[GLFW_KEY_KP_ADD])
        {
            aerialHeight -= aerialZoomSpeed * deltaTime;
            if (aerialHeight < aerialMinHeight) aerialHeight = aerialMinHeight;
        }

        // Zoom Out
        if (keys[GLFW_KEY_MINUS] || keys[GLFW_KEY_KP_SUBTRACT])
        {
            aerialHeight += aerialZoomSpeed * deltaTime;
            if (aerialHeight > aerialMaxHeight) aerialHeight = aerialMaxHeight;
        }

        // Movimiento WASD de la cámara aérea
        if (keys[GLFW_KEY_A]) aerialCameraPosition.x -= aerialMoveSpeed * deltaTime;
        if (keys[GLFW_KEY_D]) aerialCameraPosition.x += aerialMoveSpeed * deltaTime;
        if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) aerialCameraPosition.z -= aerialMoveSpeed * deltaTime;
        if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) aerialCameraPosition.z += aerialMoveSpeed * deltaTime;

        // Actualizar posición Y de la cámara según el zoom
        aerialCameraPosition.y = aerialHeight;

        // Configurar cámara mirando directamente hacia abajo
        camera.SetPosition(aerialCameraPosition);
        camera.SetFront(glm::vec3(0.0f, -1.0f, 0.0f)); 
        camera.SetUp(glm::vec3(0.0f, 0.0f, -1.0f));    
    }
    else if (thirdPersonMode)
    {
        //CONTROL DE LINK EN TERCERA PERSONA
        bool isMoving = false;
        glm::vec3 movement = glm::vec3(0.0f);
        // Convertir rotación de Link a radianes para calcular dirección
        float linkRotRad = glm::radians(linkRotation);
        // Vectores de dirección basados en la rotación actual de Link
        glm::vec3 forward = glm::vec3(sin(linkRotRad), 0.0f, cos(linkRotRad));
        glm::vec3 right = glm::vec3(cos(linkRotRad), 0.0f, -sin(linkRotRad));

        // Controles de Link
        if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
        {
            movement += forward;
            isMoving = true;
        }
        if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
        {
            movement -= forward;
            isMoving = true;
        }
        if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
        {
            movement -= right;
            linkRotation += 90.0f * deltaTime;
        }
        if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
        {
            movement += right;
            linkRotation -= 90.0f * deltaTime;
        }

        // Aplicar movimiento a Link
        if (isMoving && glm::length(movement) > 0.0f)
        {
            movement = glm::normalize(movement);
            linkPosition += movement * linkSpeed * deltaTime;
            linkPosition.y = -0.5f;  
        }

        //ANIMACIÓN PROCEDURAL DE EXTREMIDADES DE LINK
        linkIsMoving = isMoving;
        float time = (float)glfwGetTime();

        if (linkIsMoving)
        {
            float limbFrequency = 10.0f;
            float armAmplitude = 25.0f;
            float legAmplitude = 30.0f;

            linkArmLeft = glm::sin(time * limbFrequency) * armAmplitude;
            linkArmRight = -glm::sin(time * limbFrequency) * armAmplitude;
            linkLegLeft = -glm::sin(time * limbFrequency) * legAmplitude;
            linkLegRight = glm::sin(time * limbFrequency) * legAmplitude;
        }
        else
        {
            // Link está quieto. volver  a la posición neutral
            float returnSpeed = 5.0f;
            linkArmLeft = glm::mix(linkArmLeft, 0.0f, returnSpeed * deltaTime);
            linkArmRight = glm::mix(linkArmRight, 0.0f, returnSpeed * deltaTime);
            linkLegLeft = glm::mix(linkLegLeft, 0.0f, returnSpeed * deltaTime);
            linkLegRight = glm::mix(linkLegRight, 0.0f, returnSpeed * deltaTime);
        }

        // Normalizar rotación entre 0 y 360
        if (linkRotation < 0.0f) linkRotation += 360.0f;
        if (linkRotation >= 360.0f) linkRotation -= 360.0f;

        // Calcular posición de cámara usando coordenadas esféricas
        float totalYaw = linkRotation + cameraYaw;
        float yawRad = glm::radians(totalYaw);
        float pitchRad = glm::radians(cameraPitch);

        // Calcular offset en coordenadas esféricas
        glm::vec3 cameraOffset;
        cameraOffset.x = cameraDistance * cos(pitchRad) * sin(yawRad);
        cameraOffset.y = cameraDistance * sin(pitchRad);
        cameraOffset.z = cameraDistance * cos(pitchRad) * cos(yawRad);

        // Punto focal: cabeza de Link
        glm::vec3 targetPos = linkPosition + glm::vec3(0.0f, 1.5f, 0.0f);

        // Posición de cámara
        glm::vec3 cameraPos = targetPos - cameraOffset;
        cameraPos.y += cameraHeight;

        // EVITAR QUE LA CÁMARA ATRAVIESE EL PISO
        float minHeight = 0.3f; // Altura mínima sobre el suelo
        if (cameraPos.y < minHeight)
        {
            cameraPos.y = minHeight;
            // mantener la dirección correcta
            glm::vec3 direction = glm::normalize(targetPos - cameraPos);
            camera.SetPosition(cameraPos);
            camera.SetFront(direction);
        }
        else
        {
            // Actualizar cámara normalmente
            camera.SetPosition(cameraPos);
            camera.SetFront(glm::normalize(targetPos - cameraPos));
        }
    }
    else
    {
        // modo libre
        if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
            camera.ProcessKeyboard(RIGHT, deltaTime);

        if (camera.GetPosition().y < 0.0f)
            camera.SetPosition(glm::vec3(camera.GetPosition().x, 0.0f, camera.GetPosition().z));
    }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
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

    // Tecla P: Reproducir sonido de Puro Hueso
    if (keys[GLFW_KEY_P] && action == GLFW_PRESS)
    {
        if (g_puroHuesoSound != nullptr && g_audioEngine != nullptr)
        {
            ma_sound_stop(g_puroHuesoSound);
            ma_sound_seek_to_pcm_frame(g_puroHuesoSound, 0);
            ma_sound_start(g_puroHuesoSound);
        }
        else
        {
            std::cout << "ERROR: Audio de Puro Hueso no disponible." << std::endl;
        }
    }

    // Tecla T: Cambiar entre modo tercera persona y modo libre
    if (keys[GLFW_KEY_T] && action == GLFW_PRESS)
    {
        thirdPersonMode = !thirdPersonMode;
    }

    // Tecla M: Cambiar a modo de cámara aérea (cenital)
    if (keys[GLFW_KEY_M] && action == GLFW_PRESS)
    {
        aerialMode = !aerialMode;
        if (aerialMode)
        {
            // Centrar la cámara aérea sobre Link al activar el modo
            aerialCameraPosition.x = linkPosition.x;
            aerialCameraPosition.z = linkPosition.z;
            aerialCameraPosition.y = aerialHeight;
        }
    }

    // Tecla I: Cambiar a modo de cámara del Templo Mayor
    if (keys[GLFW_KEY_I] && action == GLFW_PRESS)
    {
        temploMayorMode = !temploMayorMode;
        if (temploMayorMode)
        {
            //GUARDAR ESTADO DE LA CÁMARA ACTUAL
            savedCameraPosition = camera.GetPosition();
            savedCameraFront = camera.GetFront();
            savedCameraUp = camera.GetUp();
            wasCameraStateSaved = true;
            temploMayorAngle = 1;
            
            //CONTROL DE AUDIO
            // Pausar música de fondo
            if (g_backgroundMusic != nullptr)
            {
                ma_sound_stop(g_backgroundMusic);
            }
            
            // Detener audio del ring si está sonando
            if (g_ringSound != nullptr && ma_sound_is_playing(g_ringSound))
            {
                ma_sound_stop(g_ringSound);
            }
            
            // Iniciar soundtrack del Templo Mayor
            if (g_temploMayorSound != nullptr)
            {
                ma_sound_seek_to_pcm_frame(g_temploMayorSound, 0); // Reiniciar desde el inicio
                ma_sound_start(g_temploMayorSound);
            }
        }
        else
        {
            // Detener soundtrack del Templo Mayor
            if (g_temploMayorSound != nullptr)
            {
                ma_sound_stop(g_temploMayorSound);
            }
            
            // Reanudar música de fondo 
            if (g_backgroundMusic != nullptr)
            {
                ma_sound_start(g_backgroundMusic);
            }
            
            //RESTAURAR ESTADO DE LA CÁMARA
            if (wasCameraStateSaved)
            {
                camera.SetPosition(savedCameraPosition);
                camera.SetFront(savedCameraFront);
                camera.SetUp(savedCameraUp);
                wasCameraStateSaved = false;
            }
        }
    }

    // Teclas 1-4: Cambiar ángulo de cámara del Templo Mayor 
    if (temploMayorMode)
    {
        if (keys[GLFW_KEY_1] && action == GLFW_PRESS)
        {
            temploMayorAngle = 1;
        }
        if (keys[GLFW_KEY_2] && action == GLFW_PRESS)
        {
            temploMayorAngle = 2;
        }
        if (keys[GLFW_KEY_3] && action == GLFW_PRESS)
        {
            temploMayorAngle = 3;
        }
        if (keys[GLFW_KEY_4] && action == GLFW_PRESS)
        {
            temploMayorAngle = 4;
        }
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    // En modo aéreo o modo Templo Mayor, el mouse no tiene funcionalidad
    if (aerialMode || temploMayorMode)
    {
        return;
    }

    if (thirdPersonMode)
    {
        // En modo tercera persona, ajustar yaw y pitch relativos a Link
        float sensitivity = 0.1f;
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        // Actualizar ángulos de cámara
        cameraYaw -= xOffset;
        cameraPitch += yOffset;

        // Limitar pitch: desde mirar hacia el cielo (60°) hasta hacia abajo (-60°)
        if (cameraPitch > 60.0f)
            cameraPitch = 60.0f;    
        if (cameraPitch < -60.0f)
            cameraPitch = -60.0f;

        // Limitar yaw para rotación completa
        if (cameraYaw > 180.0f) cameraYaw -= 360.0f;
        if (cameraYaw < -180.0f) cameraYaw += 360.0f;

        // Link mira arriba/abajo y a los lados siguiendo exactamente a la cámara.
        float headSensitivity = 0.8f; 

        // Actualizar rotación de cabeza en Y (yaw - girar izquierda/derecha)
        linkHeadRotY = cameraYaw * headSensitivity;

        // Actualizar rotación de cabeza
        linkHeadRotX = -cameraPitch * headSensitivity;

        // Límites de rotación del cuello
        if (linkHeadRotY > 60.0f) linkHeadRotY = 60.0f;
        if (linkHeadRotY < -60.0f) linkHeadRotY = -60.0f;

        if (linkHeadRotX > 45.0f) linkHeadRotX = 45.0f;
        if (linkHeadRotX < -45.0f) linkHeadRotX = -45.0f;

    }
    else
    {
        // Modo cámara libre
        camera.ProcessMouseMovement(xOffset, yOffset);
    }
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            // === ROTACIÓN DE LAS CARAS UP y DOWN ===
            if (i == 2 || i == 3)
            {
                int channels = nrChannels;
                unsigned char* rotated = new unsigned char [width * height * channels];
                for (int y = 0; y < height; ++y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        for (int c = 0; c < channels; ++c)
                        {
                            rotated[(x * height + (height - y - 1)) * channels + c] =
                                data[(y * width + x) * channels + c];
                        }
                    }
                }
                stbi_image_free(data);
                data = rotated;
                std::swap(width, height);
            }

            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            delete[] data;
        }
        else
        {
            std::cout << "Error al cargar: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}