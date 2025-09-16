#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Windows.h>
#include <mmsystem.h>

#include "shader.h"
#include "basic_camera.h"
#include "camera.h"
#include "pointLight.h"
#include "sphere.h"
#include "curve.h"
#include "cone.h"
#include "torus.h"
#include "cube.h"
#include "bottle.h"
#include "glass.h"
#include "cylinder.h"
#include "stb_image.h"

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#pragma comment(lib, "winmm.lib")

using namespace std;
static bool qKeyPressed = false;
bool toggleTexture = true;
//function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);
glm::mat4 transforamtion(float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz);
void drawSofa(unsigned int VAO, Shader lightingShaderWithTexture, glm::mat4 matrix, Cube c);
void drawCtable(unsigned int VAO, Shader lightingShaderWithTexture, glm::mat4 matrix, Cube c);
void drawself(unsigned int VAO, Shader lightingShaderWithTexture, glm::mat4 matrix, Cube c, Cube d);
void drawChair(unsigned int VAO, Shader lightingShaderWithTexture, glm::mat4 matrix, Cube c, Cube d);
void Tree(Cube& cube, Curve& treeshape, Curve& treebelow, Shader& lightingShader, Shader& lightingShaderTexture, glm::mat4 alTogether);
void Vase(Cube& cube, Curve& shape, Shader& lightingShader, Shader& lightingShaderTexture, glm::mat4 matrix);

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//screen
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float aspectRatio = 4.0f / 3.0f;

//camera
float eyeX = 7.5f, eyeY = 1.0f, eyeZ = 35.0f;
float lookAtX = 7.5f, lookAtY = 0.0f, lookAtZ = 0.0f;
Camera camera(glm::vec3(eyeX, eyeY, eyeZ));

//timing
float deltaTime = 0.0f;     // time between current frame and last frame
float lastFrame = 0.0f;

//fan
float r = 0.0f;
bool fanOn = false;

//bird's eye view
bool birdEyeView = false;
glm::vec3 birdEyePosition(2.0f, 3.5f, 13.5f);
glm::vec3 birdEyeTarget(2.0f, 0.0f, 7.5f);
float birdEyeSpeed = 1.0f;

//rotation around a point
float theta = 0.0f; // Angle around the Y-axis
float radius = 2.0f;

//directional light
bool directionLightOn = true;
bool directionalAmbient = true;
bool directionalDiffuse = false;
bool directionalSpecular = false;

//spot light
bool spotLightOn = false;

//point light
bool point1 = true;
bool point2 = true;
bool point3 = true;

//custom projection matrix
float fov = glm::radians(camera.Zoom);
float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
float nearvar = 0.1f;
float farvar = 100.0f;
float tanHalfFOV = tan(fov / 2.0f);

//doors and windows
bool outDoor = false;
bool indoor = false;
bool windowb = false;
bool pepsi_on = false;
bool keyGPressed = false;
bool keyPPressed = false;
bool keyLPressed = false;
bool keyKPressed = false;
bool key1Pressed = false;
bool key2Pressed = false;
bool key3Pressed = false;
bool key4Pressed = false;
bool key5Pressed = false;
bool key6Pressed = false;
bool key7Pressed = false;
bool key8Pressed = false;
float doorAngle = 90.0f;

//positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(-10.0f,  3.0f,  3.85f),
    glm::vec3(6.5f,  4.2f,  0.2f),
    glm::vec3(24.9f,  2.5f,  11.8f)
};

PointLight pointlight1(
    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,       // position
    0.2f, 0.2f, 0.2f,       //ambient
    0.8f, 0.8f, 0.8f,       //diffuse
    0.2f, 0.2f, 0.2f,       //specular
    1.0f,       //k_c
    0.09f,      //k_l
    0.032f,     //k_q
    1       //light number
);

PointLight pointlight2(
    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,
    0.2f, 0.2f, 0.2f,
    0.8f, 0.8f, 0.8f,
    0.2f, 0.2f, 0.2f,
    1.0f,
    0.09f,
    0.032f,
    2
);

PointLight pointlight3(
    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,
    0.2f, 0.2f, 0.2f,
    0.8f, 0.8f, 0.8f,
    0.2f, 0.2f, 0.2f,
    1.0f,
    0.09f,
    0.032f,
    3
);


//pointlight cube color
glm::vec4 cubeColor1(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor2(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor3(1.0f, 1.0f, 1.0f, 1.0f);

int main()
{
    //glfw initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hotel Lounge", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //glad load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //configure global opengl state
    glEnable(GL_DEPTH_TEST);

    //build and compile our shader program
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    Shader lightingShaderWithTexture("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
    //Shader lightingShader("vertexShaderForGouraudShading.vs", "fragmentShaderForGouraudShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    //Shader constantShader("vertexShader.vs", "fragmentShaderV2.fs");

    //set up vertex data (and buffer(s)) and configure vertex attributes


    float cube_vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };

    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    vector<float>tree_shape =     {
        0.0650, 2.5050, 5.1000,
            -0.0650, 2.5050, 5.1000,
            -0.1400, 2.2600, 5.1000,
            -0.2650, 2.0650, 5.1000,
            -0.2650, 2.0650, 5.1000,
            -0.4400, 1.8450, 5.1000,
            -0.6200, 1.6450, 5.1000,
            -0.6200, 1.6450, 5.1000,
            -0.9800, 1.4450, 5.1000,
    };
    vector<float>tree_below = {
        -0.5550, -0.3800, 5.1000,
            -0.4400, -0.3300, 5.1000,
            -0.2950, -0.2750, 5.1000,
            -0.2200, -0.2150, 5.1000,
            -0.1550, -0.0300, 5.1000,
            -0.1250, 0.1850, 5.1000,
            -0.1200, 0.3750, 5.1000,
            -0.1200, 0.5900, 5.1000,
            -0.1200, 0.7500, 5.1000,
    };

    vector<float>vase_shape =     {
             0.66, 2.00384, 4.90659,
             0.115, 1.74458, 4.96143,
             0.34, 1.5538, 5.00179,
             0.44, 0.14498, 5.29981,
             0.49, 1.34346, 5.04629,
             0.715, 1.05974, 5.1063,
             0.885, 0.839609, 5.15287,
             0.835, 0.531428, 5.21806,
             0.675, 0.252599, 5.27705,
             0.225, 0.0422536, 5.32154,
             0.0199997, 0.0862794, 5.31223,
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    //second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    //note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //ourShader.use();
    //constantShader.use();
    /**/
    
    // Create a sphere object
    float radius = 1.0f;
    int sectorCount = 36;
    int stackCount = 18;
    glm::vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
    float shininess = 32.0f;

    Sphere sphere(radius, sectorCount, stackCount, ambient, diffuse, specular, shininess);


    string diffuseMapPath = "floor.jpg";
    string specularMapPath = "floor.jpg";
    unsigned int diffMap = loadTexture(diffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap = loadTexture(specularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 2.0f, 2.0f);

    string ffMapPath = "floor_food.jpg";
    unsigned int ffMap = loadTexture(ffMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube ffcube = Cube(ffMap, ffMap, 32.0f, 0.0f, 0.0f, 2.0f, 2.0f);

    //roof texture
    string diffuseMapPath2 = "roof.jpg";
    string specularMapPath2 = "roof.jpg";
    unsigned int diffMap2 = loadTexture(diffuseMapPath2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap2 = loadTexture(specularMapPath2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube2 = Cube(diffMap2, specMap2, 32.0f, 0.0f, 0.0f, 2.5f, 4.5f);

    //wall texture
    string diffuseMapPath3 = "wall.jpg";
    unsigned int diffMap3 = loadTexture(diffuseMapPath3.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube3 = Cube(diffMap3, diffMap3, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    //Hotel name texture
    string hnMapPath = "hotel_name.jpg";
    unsigned int hnMap = loadTexture(hnMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube hname = Cube(hnMap, hnMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string wallMapPath = "food_wall.jpg";
    unsigned int wallMap = loadTexture(wallMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube wfcube = Cube(wallMap, wallMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string d11mappath = "door11.jpg";
    unsigned int d11map = loadTexture(d11mappath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube d11 = Cube(d11map, d11map, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string d12mappath = "door11.jpg";
    unsigned int d12map = loadTexture(d12mappath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube d12 = Cube(d12map, d12map, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);


    string diffuseMapPath4 = "door2.jpg";
    string specularMapPath4 = "door2.jpg";
    unsigned int diffMap4 = loadTexture(diffuseMapPath4.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap4 = loadTexture(specularMapPath4.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cubedoor = Cube(diffMap4, specMap4, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffuseMapPath5 = "wood2.jpg";
    string specularMapPath5 = "wood2.jpg";
    unsigned int diffMap5 = loadTexture(diffuseMapPath5.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap5 = loadTexture(specularMapPath5.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube5 = Cube(diffMap5, specMap5, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffuseMapSofa = "sofa.jpg";
    string specularMapSofa = "sofa.jpg";
    unsigned int diffSofa = loadTexture(diffuseMapSofa.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specSofa = loadTexture(specularMapSofa.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube sofa = Cube(diffSofa, specSofa, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffuseMapKey = "key_cabinet.jpg";
    string specularMapKey = "key_cabinet.jpg";
    unsigned int diffkey = loadTexture(diffuseMapKey.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int speckey = loadTexture(specularMapKey.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube keycab = Cube(diffkey, speckey, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string grassmapKey = "grass.jpg";
    unsigned int grasskey = loadTexture(grassmapKey.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube grass = Cube(grasskey, grasskey, 32.0f, 0.0f, 0.0f, 10.0f, 10.0f);

    string footmapKey = "footpath.jpg";
    unsigned int footKey = loadTexture(footmapKey.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube footpath = Cube(footKey, footKey, 32.0f, 0.0f, 0.0f, 5.0f, 2.0f);

    string roadmapKey = "road.jpg";
    unsigned int roadkey = loadTexture(roadmapKey.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube road = Cube(roadkey, roadkey, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string ctmapKey = "coffee_table.jpg";
    unsigned int ctKey = loadTexture(ctmapKey.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube coffee_table = Cube(ctKey, ctKey, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string bookmapKey = "book_self.jpg";
    unsigned int bookKey = loadTexture(bookmapKey.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube book = Cube(bookKey, bookKey, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string scenemapkey1 = "scene1.jpg";
    unsigned int scenekey1 = loadTexture(scenemapkey1.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube scene1 = Cube(scenekey1, scenekey1, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string scenemapkey2 = "scene2.jpg";
    unsigned int scenekey2 = loadTexture(scenemapkey2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube scene2 = Cube(scenekey2, scenekey2, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string scenemapkey3 = "scene3.jpg";
    unsigned int scenekey3 = loadTexture(scenemapkey3.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube scene3 = Cube(scenekey3, scenekey3, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string vend1 = "pepsiside.jpg";
    unsigned int vend11 = loadTexture(vend1.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube_vend_side = Cube(vend11, vend11, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string vend2 = "pepsifront.jpg";
    unsigned int vend22 = loadTexture(vend2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube_vend_top = Cube(vend22, vend22, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string ch1 = "chair1.jpg";
    unsigned int ch11 = loadTexture(ch1.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube chairmain = Cube(ch11, ch11, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string ch2 = "chair2.jpg";
    unsigned int ch22 = loadTexture(ch2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube chairsub = Cube(ch22, ch22, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string chrismastree = "chrismastree.jpg";
    string tree_wood = "tree.jpg";
    unsigned int chrismastree_tex = loadTexture(chrismastree.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int treewood_tex = loadTexture(tree_wood.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    Curve treeshape(tree_shape, chrismastree_tex, chrismastree_tex, 1.0f);
    Curve treebelow(tree_below, treewood_tex, treewood_tex, 1.0f);

    string vase = "vase.jpg";
    unsigned int vase_tex = loadTexture(vase.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    Curve vaseshape(vase_shape, vase_tex, vase_tex, 1.0f);

    string diffuseMapPathB = "classroom_floor.png";
    string specularMapPathB = "classroom_floor.png";
    unsigned int diffMapB = loadTexture(diffuseMapPathB.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMapB = loadTexture(specularMapPathB.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Bottle bottle = Bottle(diffMapB, specMapB, 32.0f);

    string diffuseMapPathG = "glass.jpg";
    string specularMapPathG = "glass.jpg";
    unsigned int diffMapG = loadTexture(diffuseMapPathG.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMapG = loadTexture(specularMapPathG.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Glass glass = Glass(diffMapG, specMapG, 32.0f);

    string test = "round_table.jpg";
    unsigned int cltMap = loadTexture(test.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    string defaul = "black.jpg";
    unsigned int defaulMap = loadTexture(defaul.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cylinder lobby_table = Cylinder(defaulMap, defaulMap, cltMap, cltMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string rt = "food_table.jpg";
    unsigned int rtMap = loadTexture(rt.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cylinder food_table = Cylinder(defaulMap, defaulMap, rtMap, rtMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string sil = "silver.jpg";
    unsigned int silv = loadTexture(sil.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cylinder pepsii = Cylinder(vend11, vend11, silv, silv, 32.0f, 0.0f, 0.0f, 4.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);


    string co = "cone.jpg";
    unsigned int con = loadTexture(co.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cone cone = Cone(con, con, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string to = "flower.jpg";
    unsigned int tor = loadTexture(to.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Torus torus = Torus(tor, tor, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);


    //sphere texture
    string sphmap = "dis.jpg";
    unsigned int diffMaps = loadTexture(sphmap.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMaps = loadTexture(sphmap.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    int tex = 0;
    float doorin = 0.0f;
    float doorout = 0.0f;
    float winparam = 0.0f;
    float pepsicount = 0.0f;
    float z = 0.0f;
    //render loop
    while (!glfwWindowShouldClose(window))
    {
        //per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //input
        processInput(window);

        //render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //ourShader.use();
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        //point lights set up
        pointlight1.setUpPointLight(lightingShader);
        pointlight2.setUpPointLight(lightingShader);
        pointlight3.setUpPointLight(lightingShader);

        //directional light set up
        lightingShader.setVec3("directionalLight.direction", 0.0f, -1.0f, 0.0f);
        lightingShader.setVec3("directionalLight.ambient", 0.1f, 0.1f, 0.1f);
        lightingShader.setVec3("directionalLight.diffuse", 0.5f, 0.5f, 0.5f);
        lightingShader.setVec3("directionalLight.specular", 0.0f, 0.0f, 0.0f);
        lightingShader.setBool("directionLightOn", directionLightOn);

        //spot light set up
        lightingShader.setVec3("spotLight.position", 7.5f, 2.0f, 2.0f);
        lightingShader.setVec3("spotLight.direction", 0.0f, -1.0f, 0.0f);
        lightingShader.setVec3("spotLight.ambient", 0.5f, 0.5f, 0.5f);
        lightingShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("spotLight.specular", 0.2f, 0.2f, 0.2f);
        lightingShader.setFloat("spotLight.k_c", 1.0f);
        lightingShader.setFloat("spotLight.k_l", 0.09);
        lightingShader.setFloat("spotLight.k_q", 0.032);
        lightingShader.setFloat("spotLight.inner_circle", glm::cos(glm::radians(15.0f)));
        lightingShader.setFloat("spotLight.outer_circle", glm::cos(glm::radians(30.0f)));
        lightingShader.setBool("spotLightOn", spotLightOn);

        //handle for changes in directional light directly from shedder
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            if (directionLightOn) {
                lightingShader.setBool("ambientLight", !directionalAmbient);
                directionalAmbient = !directionalAmbient;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            if (directionLightOn) {
                lightingShader.setBool("diffuseLight", !directionalDiffuse);
                directionalDiffuse = !directionalDiffuse;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
            if (directionLightOn) {
                lightingShader.setBool("specularLight", !directionalSpecular);
                directionalSpecular = !directionalSpecular;
            }
        }

        glm::mat4 projection(0.0f);
        projection[0][0] = 1.0f / (aspect * tanHalfFOV);
        projection[1][1] = 1.0f / tanHalfFOV;
        projection[2][2] = -(farvar + nearvar) / (farvar - nearvar);
        projection[2][3] = -1.0f;
        projection[3][2] = -(2.0f * farvar * nearvar) / (farvar - nearvar);
        lightingShader.setMat4("projection", projection);

        //camera view transformation
        //constantShader.setMat4("view", view);
        //ourShader.setMat4("view", view);

        glm::mat4 view;

        //check for bird's eye view and normal view
        if (birdEyeView) {
            glm::vec3 up(0.0f, 1.0f, 0.0f);
            view = glm::lookAt(birdEyePosition, birdEyeTarget, up);
        }
        else {
            view = camera.GetViewMatrix();
        }

        lightingShader.setMat4("view", view);

        //define matrices and vectors needed
        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;
        glm::vec3 color;

        //initialize all elements as non-emissive
        lightingShader.setVec3("material.emissive", glm::vec3(0.0f, 0.0f, 0.0f));


        

        //draw the lamp object(s)
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);

        for (unsigned int i = 0; i < 3; i++)
        {
            if (i == 0) {
                translateMatrix = glm::translate(identityMatrix, pointLightPositions[i]);
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
                model = translateMatrix * scaleMatrix;
                ourShader.setMat4("model", model);
                ourShader.setVec4("color", cubeColor1);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
            else if (i == 2) {
                translateMatrix = glm::translate(identityMatrix, pointLightPositions[i]);
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.0f, 0.2f));
                model = translateMatrix * scaleMatrix;
                ourShader.setMat4("model", model);
                ourShader.setVec4("color", cubeColor3);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
            else {
                translateMatrix = glm::translate(identityMatrix, pointLightPositions[i]);
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.2f, 0.2f));
                model = translateMatrix * scaleMatrix;
                ourShader.setMat4("model", model);
                ourShader.setVec4("color", cubeColor2);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
            
        }

        lightingShaderWithTexture.use();
        lightingShaderWithTexture.setMat4("projection", projection);
        lightingShaderWithTexture.setMat4("view", view);

        //point lights set up
        pointlight1.setUpPointLight(lightingShaderWithTexture);
        pointlight2.setUpPointLight(lightingShaderWithTexture);
        pointlight3.setUpPointLight(lightingShaderWithTexture);

        //directional light set up
        lightingShaderWithTexture.setVec3("directionalLight.direction", 0.0f, -1.0f, 0.0f);
        lightingShaderWithTexture.setVec3("directionalLight.ambient", 0.1f, 0.1f, 0.1f);
        lightingShaderWithTexture.setVec3("directionalLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShaderWithTexture.setVec3("directionalLight.specular", 0.0f, 0.0f, 0.0f);
        lightingShaderWithTexture.setBool("directionLightOn", directionLightOn);

        //spot light set up
        lightingShaderWithTexture.setVec3("spotLight.position", 7.5f, 2.0f, 8.0f);
        lightingShaderWithTexture.setVec3("spotLight.direction", 0.0f, -1.0f, 0.0f);
        lightingShaderWithTexture.setVec3("spotLight.ambient", 0.5f, 0.5f, 0.5f);
        lightingShaderWithTexture.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShaderWithTexture.setVec3("spotLight.specular", 0.2f, 0.2f, 0.2f);
        lightingShaderWithTexture.setFloat("spotLight.k_c", 1.0f);
        lightingShaderWithTexture.setFloat("spotLight.k_l", 0.09);
        lightingShaderWithTexture.setFloat("spotLight.k_q", 0.032);
        lightingShaderWithTexture.setFloat("spotLight.inner_circle", glm::cos(glm::radians(22.5f)));
        lightingShaderWithTexture.setFloat("spotLight.outer_circle", glm::cos(glm::radians(45.0f)));
        lightingShaderWithTexture.setBool("spotLightOn", spotLightOn);


    //Objects Start

        //grass floor
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-15.0f, 0.0f, -5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(45.0f, -0.05f, 27.0f));
        model = translateMatrix * scaleMatrix;
        grass.drawCubeWithTexture(lightingShaderWithTexture, model);

        //footpath
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-15.0f, 0.0f, 22.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(45.0f, -0.05f, 5.0f));
        model = translateMatrix * scaleMatrix;
        footpath.drawCubeWithTexture(lightingShaderWithTexture, model);

        //road
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-15.0f, 0.0f, 27.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(45.0f, -0.05f, 8.0f));
        model = translateMatrix * scaleMatrix;
        road.drawCubeWithTexture(lightingShaderWithTexture, model);

        //roof
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, 4.5f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(35.0f, 0.2f, 20.0f));
        model = translateMatrix * scaleMatrix;
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);


        //floor main
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.06f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(15.0f, -0.1f, 20.0f));
        model = translateMatrix * scaleMatrix;
        cube.drawCubeWithTexture(lightingShaderWithTexture, model);

        //floor lobby
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, 0.06f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, -0.1f, 20.0f));
        model = translateMatrix * scaleMatrix;
        cube.drawCubeWithTexture(lightingShaderWithTexture, model);

        //floor food
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.0f, 0.06f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, -0.1f, 20.0f));
        model = translateMatrix * scaleMatrix;
        ffcube.drawCubeWithTexture(lightingShaderWithTexture, model);

        //front wall main
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(15.0f, 4.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        //front wall lobby
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-10.0f, 4.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        //front wall food
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.0f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 4.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        wfcube.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left wall main
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.5f, 5.0f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 3.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 10.0f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 15.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.5f, 5.0f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left wall lobby
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.5f, 20.0f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        //right wall main
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.0f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.5f, 14.5f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.0f, 3.5f, 14.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 2.0f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.0f, 0.0f, 16.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.5f, 3.5f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left wall food
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.05f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.5f, 14.5f));
        model = translateMatrix * scaleMatrix;
        wfcube.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.05f, 3.5f, 14.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 2.0f));
        model = translateMatrix * scaleMatrix;
        wfcube.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.05f, 0.0f, 16.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.5f, 3.5f));
        model = translateMatrix * scaleMatrix;
        wfcube.drawCubeWithTexture(lightingShaderWithTexture, model);

        //door to food
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.0f, 0.0f, 14.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 3.5f, 2.0f));
        if (indoor == true && doorin < 90.0f) {
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(doorin), glm::vec3(0.0f, 1.0f, 0.0f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            cubedoor.drawCubeWithTexture(lightingShaderWithTexture, model);
            doorin += 1.0f;
        }
        else if(indoor == true && doorin == 90.0f){
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(doorin), glm::vec3(0.0f, 1.0f, 0.0f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            cubedoor.drawCubeWithTexture(lightingShaderWithTexture, model);
        }
        else if (indoor == false && doorin > 0.0f) {
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(doorin), glm::vec3(0.0f, 1.0f, 0.0f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            cubedoor.drawCubeWithTexture(lightingShaderWithTexture, model);
            doorin -= 1.0f;
        }
        else if (indoor == false && doorin == 0.0f) {
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(doorin), glm::vec3(0.0f, 1.0f, 0.0f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            cubedoor.drawCubeWithTexture(lightingShaderWithTexture, model);
        }
        

        //right wall food
        translateMatrix = glm::translate(identityMatrix, glm::vec3(25.0f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.5f, 20.0f));
        model = translateMatrix * scaleMatrix;
        wfcube.drawCubeWithTexture(lightingShaderWithTexture, model);

        //back wall main
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 4.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0f, 3.5f, 20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 1.0f, 0.1f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, 0.0f, 20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 4.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        //Main door
        float zz = camera.Position.z;
        if (abs(zz - 20.0f) <= 5.0f && doorout < 2.5) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0f - doorout, 0.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5f, 3.5f, 0.05f));
            model = translateMatrix * scaleMatrix;
            d11.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(7.5f + doorout, 0.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5f, 3.5f, 0.05f));
            model = translateMatrix * scaleMatrix;
            d12.drawCubeWithTexture(lightingShaderWithTexture, model);

            doorout += 0.01;
        }
        else if (abs(zz - 20.0f) <= 5.0f && doorout == 2.5) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0f - doorout, 0.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5f, 3.5f, 0.05f));
            model = translateMatrix * scaleMatrix;
            d11.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(7.5f + doorout, 0.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5f, 3.5f, 0.05f));
            model = translateMatrix * scaleMatrix;
            d12.drawCubeWithTexture(lightingShaderWithTexture, model);
        }
        else if (abs(zz - 20.0f) > 5.0f && doorout > 0.0) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0f - doorout, 0.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5f, 3.5f, 0.05f));
            model = translateMatrix * scaleMatrix;
            d11.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(7.5f + doorout, 0.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5f, 3.5f, 0.05f));
            model = translateMatrix * scaleMatrix;
            d12.drawCubeWithTexture(lightingShaderWithTexture, model);

            doorout -= 0.01;
        }
        else {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0f - doorout, 0.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5f, 3.5f, 0.05f));
            model = translateMatrix * scaleMatrix;
            d11.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(7.5f + doorout, 0.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5f, 3.5f, 0.05f));
            model = translateMatrix * scaleMatrix;
            d12.drawCubeWithTexture(lightingShaderWithTexture, model);
        }

        //back wall lobby
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, 0.0f, 20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 4.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 0.0f, 20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, 0.1f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 3.5f, 20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, 0.1f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 0.0f, 20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 4.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        //back wall food
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.0f, 0.0f, 20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 4.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        wfcube.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(19.0f, 0.0f, 20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, 0.1f));
        model = translateMatrix * scaleMatrix;
        wfcube.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(19.0f, 3.5f, 20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, 0.1f));
        model = translateMatrix * scaleMatrix;
        wfcube.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(21.0f, 0.0f, 20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 4.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        wfcube.drawCubeWithTexture(lightingShaderWithTexture, model);

        if (windowb == true && winparam > -90.0f) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 2.5f, 0.05f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(winparam), glm::vec3(0.0f, 1.0f, 0.0f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d11.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(19.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 2.5f, 0.05f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d11.drawCubeWithTexture(lightingShaderWithTexture, model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-winparam), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.0f, 2.5f, 0.05f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d12.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(21.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.0f, 2.5f, 0.05f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d12.drawCubeWithTexture(lightingShaderWithTexture, model);


            winparam -= 1.0f;
        }
        else if (windowb == true && winparam == -90.0f) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 2.5f, 0.05f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(winparam), glm::vec3(0.0f, 1.0f, 0.0f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d11.drawCubeWithTexture(lightingShaderWithTexture, model);
            
            translateMatrix = glm::translate(identityMatrix, glm::vec3(19.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 2.5f, 0.05f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d11.drawCubeWithTexture(lightingShaderWithTexture, model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-winparam), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.0f, 2.5f, 0.05f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d12.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(21.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.0f, 2.5f, 0.05f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d12.drawCubeWithTexture(lightingShaderWithTexture, model);


        }
        else if (windowb == false && winparam < 0.0f) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 2.5f, 0.05f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(winparam), glm::vec3(0.0f, 1.0f, 0.0f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d11.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(19.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 2.5f, 0.05f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d11.drawCubeWithTexture(lightingShaderWithTexture, model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-winparam), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.0f, 2.5f, 0.05f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d12.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(21.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.0f, 2.5f, 0.05f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d12.drawCubeWithTexture(lightingShaderWithTexture, model);



            winparam += 1.0f;
        }
        else if (windowb == false && winparam == 0.0f) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 2.5f, 0.05f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(winparam), glm::vec3(0.0f, 1.0f, 0.0f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d11.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(19.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 2.5f, 0.05f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d11.drawCubeWithTexture(lightingShaderWithTexture, model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-winparam), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.0f, 2.5f, 0.05f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d12.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(21.0f, 1.0f, 20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.0f, 2.5f, 0.05f));
            model = translateMatrix * rotateYMatrix * scaleMatrix;
            d12.drawCubeWithTexture(lightingShaderWithTexture, model);


        }


        //Keylob wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0, 1.0f, 0.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 2.0f, 0.1f));
        model = translateMatrix * scaleMatrix;
        keycab.drawCubeWithTexture(lightingShaderWithTexture, model);

        //Name wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.75, 3.0f, 0.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(7.5f, 1.2f, 0.1f));
        model = translateMatrix * scaleMatrix;
        hname.drawCubeWithTexture(lightingShaderWithTexture, model);

        //reception table1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.0f, 3.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 1.3f, 1.0f));
        model = translateMatrix * scaleMatrix;
        cube5.drawCubeWithTexture(lightingShaderWithTexture, model);

        //reception table2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.51f, 0.0f, 1.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.95f, 1.29f, 2.95f));
        model = translateMatrix * scaleMatrix;
        cube5.drawCubeWithTexture(lightingShaderWithTexture, model);

        //reception table3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(11.51f, 0.0f, 1.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.95f, 1.29f, 2.95f));
        model = translateMatrix * scaleMatrix;
        cube5.drawCubeWithTexture(lightingShaderWithTexture, model);

        //draw fans with rotations



        //Sofa main
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f, 0.0f, 8.0f));
        drawSofa(VAO, lightingShaderWithTexture, translateMatrix* rotateYMatrix, sofa);

        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f, 0.0f, 14.0f));
        drawSofa(VAO, lightingShaderWithTexture, translateMatrix* rotateYMatrix, sofa);

        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(11.5f, 0.0f, 11.5f));
        drawSofa(VAO, lightingShaderWithTexture, translateMatrix* rotateYMatrix, sofa);

        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(11.5f, 0.0f, 17.5f));
        drawSofa(VAO, lightingShaderWithTexture, translateMatrix* rotateYMatrix, sofa);

        //Coffee Table
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f, 0.0f, 11.5f));
        drawCtable(VAO, lightingShaderWithTexture, translateMatrix, coffee_table);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.5f, 0.0f, 11.7f));
        drawCtable(VAO, lightingShaderWithTexture, translateMatrix, coffee_table);

        //Bookself
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.7f, 0.0f, 0.5f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.5f, 1.0f, 1.0f));
        drawself(VAO, lightingShaderWithTexture, scaleMatrix, cube5, book);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.6f, 0.0f, 0.5f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.5f, 1.0f, 1.0f));
        drawself(VAO, lightingShaderWithTexture, scaleMatrix, cube5, book);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.2f, 0.0f, 0.5f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.5f, 1.0f, 1.0f));
        drawself(VAO, lightingShaderWithTexture, scaleMatrix, cube5, book);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.1f, 0.0f, 0.5f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.5f, 1.0f, 1.0f));
        drawself(VAO, lightingShaderWithTexture, scaleMatrix, cube5, book);

        //TV-table
        //self bottom
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.8f, 0.1f, 8.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 4.0f));
        model = translateMatrix * scaleMatrix;
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //self top
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.8f, 0.8f, 8.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 4.0f));
        model = translateMatrix * scaleMatrix;
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //self left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.8f, 0.1f, 8.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.8f, 0.1f));
        model = translateMatrix * scaleMatrix;
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //self right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.8f, 0.1f, 12.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.8f, 0.1f));
        model = translateMatrix * scaleMatrix;
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //self back
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.8f, 0.1f, 8.56f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.8f, 3.9f));
        model = translateMatrix * scaleMatrix;
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //TV
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.9f, 1.3f, 8.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 2.0f, 4.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        Cube cubetv = Cube(color, color, color, 32.0f);
        cubetv.drawCubeWithMaterialisticProperty(lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.89f, 1.4f, 8.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.8f, 3.8f));
        model = translateMatrix * scaleMatrix;
        if (tex <= 500) {
            scene1.drawCubeWithTexture(lightingShaderWithTexture, model);
            tex++;
        }
        else if (tex <= 1000) {
            scene2.drawCubeWithTexture(lightingShaderWithTexture, model);
            tex++;
        }
        else if (tex < 1500) {
            scene3.drawCubeWithTexture(lightingShaderWithTexture, model);
            tex++;
        }
        else {
            scene3.drawCubeWithTexture(lightingShaderWithTexture, model);
            tex = 0;
        }

        //Sofa lobby
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, 7.5f));
        drawSofa(VAO, lightingShaderWithTexture, translateMatrix * rotateYMatrix, sofa);

        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5f, 0.0f, 12.0f));
        drawSofa(VAO, lightingShaderWithTexture, translateMatrix * rotateYMatrix, sofa);

        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.5f, 0.0f, 15.5f));
        drawSofa(VAO, lightingShaderWithTexture, translateMatrix * rotateYMatrix, sofa);

        //vending machine
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.9f, 0.0f, 17.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 3.0f, 1.5f));
        model = translateMatrix * scaleMatrix;
        cube_vend_side.drawCubeWithTexture(lightingShaderWithTexture, model);

        // Top layer texturing
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.9f, 0.0f, 17.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.01f, 3.0f, 1.5f));
        model = translateMatrix * scaleMatrix;
        cube_vend_top.drawCubeWithTexture(lightingShaderWithTexture, model);

        //reception food
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.1f, 0.0f, 3.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 1.3f, 1.0f));
        model = translateMatrix * scaleMatrix;
        cube5.drawCubeWithTexture(lightingShaderWithTexture, model);

        //counter self bottom
        translateMatrix = glm::translate(identityMatrix, glm::vec3(18.0f, 1.8f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.1f, 1.0f));
        model = translateMatrix * scaleMatrix;
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //counter self top
        translateMatrix = glm::translate(identityMatrix, glm::vec3(18.0f, 3.8f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.1f, 1.0f));
        model = translateMatrix * scaleMatrix;
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //counter self middle
        translateMatrix = glm::translate(identityMatrix, glm::vec3(18.0f, 2.8f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.1f, 1.0f));
        model = translateMatrix * scaleMatrix;
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //counter self left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(18.0f, 1.8f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(.1f, 2.0f, 1.0f));
        model = translateMatrix * scaleMatrix;
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //counter self right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(22.0f, 1.8f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(.1f, 2.1f, 1.0f));
        model = translateMatrix * scaleMatrix;
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        

        //Chairset 1
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(18.0f, 0.0f, 8.2f));
        drawChair(VAO, lightingShaderWithTexture, translateMatrix * rotateYMatrix, chairmain, chairsub);

        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(23.0f, 0.0f, 11.5f));
        drawChair(VAO, lightingShaderWithTexture, translateMatrix * rotateYMatrix, chairmain, chairsub);

        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(23.0f, 0.0f, 6.5f));
        drawChair(VAO, lightingShaderWithTexture, translateMatrix * rotateYMatrix, chairmain, chairsub);

        //Chairset 2
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(18.0f, 0.0f, 15.2f));
        drawChair(VAO, lightingShaderWithTexture, translateMatrix * rotateYMatrix, chairmain, chairsub);

        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(23.0f, 0.0f, 19.0f));
        drawChair(VAO, lightingShaderWithTexture, translateMatrix * rotateYMatrix, chairmain, chairsub);

        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(23.0f, 0.0f, 13.5f));
        drawChair(VAO, lightingShaderWithTexture, translateMatrix * rotateYMatrix, chairmain, chairsub);

        //AC
        translateMatrix = glm::translate(identityMatrix, glm::vec3(14.2f, 3.0f, 7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, 1.0f, 3.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.937f, 0.949f, 0.89f);
        Cube cubeac1 = Cube(color, color, color, 32.0f);
        cubeac1.drawCubeWithMaterialisticProperty(lightingShader, model);

        if (fanOn == true) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(14.19f, 3.05f, 7.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.15f, 3.0f));
            model = translateMatrix * scaleMatrix;
            color = glm::vec3(0.0f, 0.0f, 0.0f);
            Cube cubeac2 = Cube(color, color, color, 32.0f);
            cubeac2.drawCubeWithMaterialisticProperty(lightingShader, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(14.19f, 3.5f, 7.2f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
            model = translateMatrix * scaleMatrix;
            color = glm::vec3(0.0f, 1.0f, 0.0f);
            Cube cubeac3 = Cube(color, color, color, 32.0f);
            cubeac3.drawCubeWithMaterialisticProperty(lightingShader, model);
        }
        else {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(14.19f, 2.99f, 7.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.15f, 3.0f));
            model = translateMatrix * scaleMatrix;
            color = glm::vec3(0.0f, 0.0f, 0.0f);
            Cube cubeac2 = Cube(color, color, color, 32.0f);
            cubeac2.drawCubeWithMaterialisticProperty(lightingShader, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(14.19f, 3.5f, 7.2f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
            model = translateMatrix * scaleMatrix;
            color = glm::vec3(1.0f, 0.0f, 0.0f);
            Cube cubeac3 = Cube(color, color, color, 32.0f);
            cubeac3.drawCubeWithMaterialisticProperty(lightingShader, model);

        }















        model = transforamtion(1.0f, 0.0f, 2.25f, 0.0f, 0.0f, 0.0f, 0.8, 1.0, 0.8);
        Vase(cube, vaseshape, lightingShader, lightingShaderWithTexture, model);
        model = transforamtion(14.0f, 0.0f, 2.25f, 0.0f, 0.0f, 0.0f, 0.8, 1.0, 0.8);
        Vase(cube, vaseshape, lightingShader, lightingShaderWithTexture, model);

        int numoftree = 6;
        for (int i = 0; i < numoftree; i++)
        {
            model = transforamtion(28.0f , 0.0f , 18.25f - i * 3.0, 0.0f, 0.0f, 0.0f, 1.2, 1.5, 1.2);
            Tree(cube, treeshape, treebelow, lightingShader, lightingShaderWithTexture, model);
        }
        for (int i = 0; i < numoftree; i++)
        {
            model = transforamtion(-13.0f, 0.0f, 18.25f - i * 3.0, 0.0f, 0.0f, 0.0f, 1.2, 1.5, 1.2);
            Tree(cube, treeshape, treebelow, lightingShader, lightingShaderWithTexture, model);
        }


        // Bottle
        translateMatrix = glm::translate(identityMatrix, glm::vec3(19.0f, 1.9f, 0.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(.035, .4, .035));
        model = translateMatrix * scaleMatrix;
        bottle.drawWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(18.5f, 1.9f, 0.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(.035, .4, .035));
        model = translateMatrix * scaleMatrix;
        bottle.drawWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(20.5f, 2.9f, 0.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(.035, .4, .035));
        model = translateMatrix * scaleMatrix;
        bottle.drawWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(21.0f, 2.9f, 0.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(.035, .4, .035));
        model = translateMatrix * scaleMatrix;
        bottle.drawWithTexture(lightingShaderWithTexture, model);

        //glass
        translateMatrix = glm::translate(identityMatrix, glm::vec3(21.5f, 1.05f, 8.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(.025, .3, .025));
        model = translateMatrix * scaleMatrix;
        glass.drawWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(21.5f, 1.05f, 15.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(.025, .3, .025));
        model = translateMatrix * scaleMatrix;
        glass.drawWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.5f, 0.6f, 10.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.8, 0.5));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        cone.drawConeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 3.0f, 0.0f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 1.0, 1.0));
        model = translateMatrix * rotateXMatrix * scaleMatrix;
        torus.drawTorusWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(13.0f, 3.0f, 0.0f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 1.0, 1.0));
        model = translateMatrix * rotateXMatrix * scaleMatrix;
        torus.drawTorusWithTexture(lightingShaderWithTexture, model);

        //sphere
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.5f, 0.31f, 13.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        sphere.drawSphereTexture(lightingShaderWithTexture, model, diffMaps, specMaps);

        //pepsi
        if (pepsi_on == true && pepsicount < 250) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.5f + (pepsicount/100), 0.2f, 18.5f));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-pepsicount), glm::vec3(0.0f, 1.0f, 0.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.4f, 0.1f));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * scaleMatrix;
            pepsii.drawCylinderWithTexture(lightingShaderWithTexture, model);
            pepsicount++;
        }
        else if (pepsi_on == true && pepsicount == 250) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.5f + (pepsicount / 100), 0.2f, 18.5f));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.4f, 0.1f));
            model = translateMatrix * rotateXMatrix * scaleMatrix;
            pepsii.drawCylinderWithTexture(lightingShaderWithTexture, model);
        }
        else if (pepsi_on == false) {
            pepsicount = 0;
        }

        //lobby table
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.2f, 0.5f, 10.5));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.8f, 0.15f, 2.0f));
        model = translateMatrix * scaleMatrix;
        lobby_table.drawCylinderWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.2f, 0.1f, 10.5));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.5f, 0.2f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.937f, 0.949f, 0.89f);
        Cylinder cyl1 = Cylinder(color, color, color, 32.0f);
        cyl1.drawCylinderWithMaterialisticProperty(lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.2f, 0.0f, 10.5));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6f, 0.1f, 0.6f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        Cylinder cyl2 = Cylinder(color, color, color, 32.0f);
        cyl2.drawCylinderWithMaterialisticProperty(lightingShader, model);

        //Food Table 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(21.0f, 0.8f, 9.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.8f, 0.2f, 2.0f));
        model = translateMatrix * scaleMatrix;
        food_table.drawCylinderWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(21.0f, 0.1f, 9.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.7f, 0.2f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.937f, 0.949f, 0.89f);
        Cylinder cyl3 = Cylinder(color, color, color, 32.0f);
        cyl3.drawCylinderWithMaterialisticProperty(lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(21.0f, 0.0f, 9.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6f, 0.1f, 0.6f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        Cylinder cyl4 = Cylinder(color, color, color, 32.0f);
        cyl4.drawCylinderWithMaterialisticProperty(lightingShader, model);

        //Food Table 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(21.0f, 0.8f, 16.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.8f, 0.2f, 2.0f));
        model = translateMatrix * scaleMatrix;
        food_table.drawCylinderWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(21.0f, 0.1f, 16.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.7f, 0.2f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.937f, 0.949f, 0.89f);
        Cylinder cyl5 = Cylinder(color, color, color, 32.0f);
        cyl5.drawCylinderWithMaterialisticProperty(lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(21.0f, 0.0f, 16.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6f, 0.1f, 0.6f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        Cylinder cyl6 = Cylinder(color, color, color, 32.0f);
        cyl6.drawCylinderWithMaterialisticProperty(lightingShader, model);



        //glfw swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //optional: de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    //glfw terminate, clearing all previously allocated GLFW resources
    glfwTerminate();
    return 0;
}

//process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(P_UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(P_DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }

    // Handle GLFW_KEY_G (Toggle fanOn)
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (!keyGPressed) {
            fanOn = !fanOn; // Toggle fan state
            keyGPressed = true; // Mark the key as pressed
            float x = camera.Position.x;
            float y = camera.Position.y;
            float z = camera.Position.z;
            if (fanOn == true) {
                if (z>=0 && z<=20 && x >=0 && x<=15 && y<=4.5 && y>=0) {
                    PlaySound(TEXT("beep.wav"), NULL, SND_FILENAME | SND_SYNC);
                }
            }
            else {
                if (z >= 0 && z <= 20 && x >= 0 && x <= 15 && y <= 4.5 && y >= 0) {
                    PlaySound(TEXT("beep.wav"), NULL, SND_FILENAME | SND_SYNC);
                }
            }

        }
    }
    else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
        keyGPressed = false; // Reset the key state when released
    }

    // Handle GLFW_KEY_P (Toggle indoor)
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (!keyPPressed) {
            indoor = !indoor; // Toggle indoor state
            keyPPressed = true; // Mark the key as pressed
            float x = camera.Position.x;
            float z = camera.Position.z;
            if (indoor == true) {
                if (abs(z - 14.5) <= 3 && abs(x - 15.0) <= 5) {
                    PlaySound(TEXT("open_door.wav"), NULL, SND_FILENAME | SND_SYNC);
                }
            }
            else {
                if (abs(z - 14.5) <= 3 && abs(x - 15.0) <= 5) {
                    PlaySound(TEXT("open_door.wav"), NULL, SND_FILENAME | SND_SYNC);
                }
            }
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
        keyPPressed = false; // Reset the key state when released
    }

    // Handle GLFW_KEY_K (Toggle pepsi_on)
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        if (!keyKPressed) {
            pepsi_on = !pepsi_on; // Toggle pepsi_on state
            keyKPressed = true; // Mark the key as pressed
            if (pepsi_on == true) {
                float x = camera.Position.x;
                float z = camera.Position.z;
                if (abs(z - 18.5) <= 3 && abs(x + 8.5) <=8 ){
                    PlaySound(TEXT("vending.wav"), NULL, SND_FILENAME | SND_SYNC);
                }
            }
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE) {
        keyKPressed = false; // Reset the key state when released
    }

    // Handle GLFW_KEY_L (Toggle windowb)
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!keyLPressed) {
            windowb = !windowb; // Toggle windowb state
            keyLPressed = true; // Mark the key as pressed
            float x = camera.Position.x;
            float z = camera.Position.z;
            if (windowb == true) {
                if (abs(z - 20.0) <= 5 && (abs(x + 4.0) <= 3 || abs(x - 21.0) <= 3)) {
                    PlaySound(TEXT("open_door.wav"), NULL, SND_FILENAME | SND_SYNC);
                }
            }
            else {
                if (abs(z - 14.5) <= 3 && abs(x - 15.0) <= 5) {
                    PlaySound(TEXT("open_door.wav"), NULL, SND_FILENAME | SND_SYNC);
                }
            }
        }
    }

    else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
        keyLPressed = false; // Reset the key state when released
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(R_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!key1Pressed) {
            directionLightOn = !directionLightOn;
            key1Pressed = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
        key1Pressed = false;
    }

    // Handle GLFW_KEY_2 (Toggle pointlight1)
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (!key2Pressed) {
            if (pointlight1.ambientOn > 0 && pointlight1.diffuseOn > 0 && pointlight1.specularOn > 0) {
                pointlight1.turnOff();
                point1 = false;
                cubeColor1 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
            }
            else {
                pointlight1.turnOn();
                point1 = true;
                cubeColor1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            key2Pressed = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
        key2Pressed = false;
    }

    // Handle GLFW_KEY_3 (Toggle pointlight2)
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if (!key3Pressed) {
            if (pointlight2.ambientOn > 0 && pointlight2.diffuseOn > 0 && pointlight2.specularOn > 0) {
                pointlight2.turnOff();
                point2 = false;
                cubeColor2 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
            }
            else {
                pointlight2.turnOn();
                point2 = true;
                cubeColor2 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            key3Pressed = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
        key3Pressed = false;
    }

    // Handle GLFW_KEY_4 (Toggle pointlight3)
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        if (!key4Pressed) {
            if (pointlight3.ambientOn > 0 && pointlight3.diffuseOn > 0 && pointlight3.specularOn > 0) {
                pointlight3.turnOff();
                point3 = false;
                cubeColor3 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
            }
            else {
                pointlight3.turnOn();
                point3 = true;
                cubeColor3 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            key4Pressed = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) {
        key4Pressed = false;
    }

    // Handle GLFW_KEY_5 (Toggle spotLightOn)
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        if (!key5Pressed) {
            spotLightOn = !spotLightOn;
            key5Pressed = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE) {
        key5Pressed = false;
    }

    // Handle GLFW_KEY_6 (Toggle ambient light for pointlight1 and pointlight2)
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        if (!key6Pressed) {
            if (pointlight1.ambientOn > 0 || pointlight2.ambientOn > 0) {
                if (point1) pointlight1.turnAmbientOff();
                if (point2) pointlight2.turnAmbientOff();
            }
            else {
                if (point1) pointlight1.turnAmbientOn();
                if (point2) pointlight2.turnAmbientOn();
            }
            key6Pressed = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_RELEASE) {
        key6Pressed = false;
    }

    // Handle GLFW_KEY_7 (Toggle diffuse light for pointlight1 and pointlight2)
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        if (!key7Pressed) {
            if (pointlight1.diffuseOn > 0 || pointlight2.diffuseOn > 0) {
                if (point1) pointlight1.turnDiffuseOff();
                if (point2) pointlight2.turnDiffuseOff();
            }
            else {
                if (point1) pointlight1.turnDiffuseOn();
                if (point2) pointlight2.turnDiffuseOn();
            }
            key7Pressed = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_RELEASE) {
        key7Pressed = false;
    }

    // Handle GLFW_KEY_8 (Toggle specular light for pointlight1 and pointlight2)
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
        if (!key8Pressed) {
            if (pointlight1.specularOn > 0 || pointlight2.specularOn > 0) {
                if (point1) pointlight1.turnSpecularOff();
                if (point2) pointlight2.turnSpecularOff();
            }
            else {
                if (point1) pointlight1.turnSpecularOn();
                if (point2) pointlight2.turnSpecularOn();
            }
            key8Pressed = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_RELEASE) {
        key8Pressed = false;
    }}

//glfw whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //make sure the viewport matches the new window dimensions; note that width and height will be significantly larger than specified on retina displays.
    float aspectRatio = 4.0f / 3.0f;
    int viewWidth, viewHeight;

    if (width / (float)height > aspectRatio) {
        //Window is too wide, fit height and adjust width
        viewHeight = height;
        viewWidth = (int)(height * aspectRatio);
    }
    else {
        //Window is too tall, fit width and adjust height
        viewWidth = width;
        viewHeight = (int)(width / aspectRatio);
    }

    //Center the viewport
    int xOffset = (width - viewWidth) / 2;
    int yOffset = (height - viewHeight) / 2;

    glViewport(xOffset, yOffset, viewWidth, viewHeight);
    //glViewport(0, 0, width, height);
}

//glfw whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;       //reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

//glfw whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void drawSofa(unsigned int VAO, Shader lightingShaderWithTexture, glm::mat4 matrix, Cube c) {
    //define matrices and vectors needed
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::vec3 color;

    //sofa
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.2f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2f, 0.5f, 3.0f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.2f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 1.2f, 3.0f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2f, 0.8f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 3.2f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2f, 0.8f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);
}

void drawCtable(unsigned int VAO, Shader lightingShaderWithTexture, glm::mat4 matrix, Cube c) {
    //define matrices and vectors needed
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::vec3 color;

    //coffee table
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.4f, 0.2f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 2.0f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.2f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.4f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 2.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.4f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);
}

void drawself(unsigned int VAO, Shader lightingShaderWithTexture, glm::mat4 matrix, Cube c, Cube d) {
    //define matrices and vectors needed
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::vec3 color;

    //book self bottom
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.1f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.1f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);

    //book self middle2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 2.1f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.1f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);

    //book self middle1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 1.1f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.1f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);

    //book self top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 3.1f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.1f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);

    //book self left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.1f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.1f, 3.0f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);

    //book self right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.9f, 0.1f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.1f, 3.0f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);

    //book self book1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, 0.1f, 0.8f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.8f, 1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    d.drawCubeWithTexture(lightingShaderWithTexture, model);

    //book self book2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, 1.1f, 0.8f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.8f, 1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    d.drawCubeWithTexture(lightingShaderWithTexture, model);

    //book self book3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, 2.1f, 0.8f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.8f, 1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    d.drawCubeWithTexture(lightingShaderWithTexture, model);
}

void drawChair(unsigned int VAO, Shader lightingShaderWithTexture, glm::mat4 matrix, Cube c, Cube d) {
    //define matrices and vectors needed
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::vec3 color;

    //main
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.4f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);

    //legs
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.85f, 0.0f, 0.85f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.4f, 0.15f));
    model = matrix * translateMatrix * scaleMatrix;
    d.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.4f, 0.15f));
    model = matrix * translateMatrix * scaleMatrix;
    d.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.85f, 0.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.4f, 0.15f));
    model = matrix * translateMatrix * scaleMatrix;
    d.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.85f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.4f, 0.15f));
    model = matrix * translateMatrix * scaleMatrix;
    d.drawCubeWithTexture(lightingShaderWithTexture, model);

    //upper legs
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.6f, 0.85f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.4f, 0.15f));
    model = matrix * translateMatrix * scaleMatrix;
    d.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.6f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.4f, 0.15f));
    model = matrix * translateMatrix * scaleMatrix;
    d.drawCubeWithTexture(lightingShaderWithTexture, model);

    //back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.5f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    c.drawCubeWithTexture(lightingShaderWithTexture, model);
}

void Vase(Cube& cube, Curve& shape, Shader& lightingShader, Shader& lightingShaderTexture, glm::mat4 matrix)
{
    glm::mat4 model = transforamtion(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
    shape.draw(lightingShaderTexture, matrix * model, glm::vec3(0.0f, 0.0f, 0.0f));
}

void Tree(Cube& cube, Curve& treeshape, Curve& treebelow, Shader& lightingShader, Shader& lightingShaderTexture, glm::mat4 alTogether)
{
    glm::mat4 model = transforamtion(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
    treeshape.draw(lightingShaderTexture, alTogether * model, glm::vec3(0.0f, 0.0f, 0.0f));


    model = transforamtion(0.0, -0.5, 0.0, 0.0, 0.0, 0.0, 1.5, 1.0, 1.5);
    treeshape.draw(lightingShaderTexture, alTogether * model, glm::vec3(0.0f, 0.0f, 0.0f));


    model = transforamtion(0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.8, 1.0, 0.8);
    treeshape.draw(lightingShaderTexture, alTogether * model, glm::vec3(0.0f, 0.0f, 0.0f));

    model = transforamtion(0.0, 1.3, 0.0, 0.0, 0.0, 0.0, 0.6, 0.8, 0.6);
    treeshape.draw(lightingShaderTexture, alTogether * model, glm::vec3(0.0f, 0.0f, 0.0f));

    model = transforamtion(0.0, 0.4, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
    treebelow.draw(lightingShaderTexture, alTogether * model, glm::vec3(0.0f, 0.0f, 0.0f));
}

glm::mat4 transforamtion(float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz) {
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    return model;
}