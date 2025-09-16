//
//  main.cpp
//  triangle
//
//  Created by Nazirul Hasan on 26/8/23.
//  modified by Badiuzzaman
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float rotateAngle = 0.0;
float translate_X = -0.65;
float translate_Y = -0.8;
float scale_X = 1.3;
float scale_Y = .5;


float rotateAngle2 = 0.0;  // Rotation angle for VAO2
float translate_X2 = -0.5;
float translate_Y2 = -0.65;
float scale_X2 = 1.0;
float scale_Y2 = 1.3;


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"   gl_Position = transform * vec4(aPos, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _APPLE_
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Assignment 1", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    //triangle
    float vertices2[] = {
0.0747,0.1746,0,
0.0732,0.1746,0,
0.0732,0.1696,0,
0.0742,0.1656,0,
0.0752,0.1581,0,
0.0772,0.1521,0,
0.0772,0.1471,0,
0.0777,0.1386,0,
0.0782,0.1336,0,
0.0787,0.1281,0,
0.0787,0.1241,0,
0.0802,0.1191,0,
0.0867,0.1121,0,
0.0937,0.1076,0,
0.1027,0.1056,0,
0.1127,0.1031,0,
0.1222,0.1016,0,
0.1322,0.1001,0,
0.1422,0.0996,0,
0.1557,0.0996,0,
0.1662,0.0986,0,
0.1782,0.0986,0,
0.1907,0.0991,0,
0.1992,0.1001,0,
0.2082,0.1001,0,
0.2182,0.1016,0,
0.2292,0.1031,0,
0.2372,0.1056,0,
0.2407,0.1096,0,
0.2417,0.1156,0,
0.2432,0.1271,0,
0.2432,0.1341,0,
0.2437,0.1401,0,
0.2437,0.1466,0,
0.2477,0.1511,0,
0.2482,0.1566,0,
0.2482,0.1636,0,
0.2457,0.1706,0,
0.2402,0.1781,0,
0.2357,0.1841,0,
0.2282,0.1876,0,
0.2212,0.1901,0,
0.2192,0.1926,0,
0.2167,0.1896,0,
0.2097,0.1856,0,
0.2007,0.1816,0,
0.1917,0.1781,0,
0.1827,0.1761,0,
0.1747,0.1746,0,
0.1667,0.1746,0,
0.1602,0.1746,0,
0.1527,0.1746,0,
0.1452,0.1751,0,
0.1332,0.1746,0,
0.1272,0.1756,0,
0.1167,0.1771,0,
0.1092,0.1796,0,
0.1037,0.1816,0,
0.1022,0.1841,0,
0.0942,0.1916,0,
0.0872,0.1896,0,
0.0797,0.1881,0,
0.0767,0.1831,0,
0.0752,0.1786,0,
0.0782,0.1746,0,
0.0837,0.1716,0,
0.0927,0.1671,0,
0.1007,0.1656,0,
0.1067,0.1626,0,
0.1162,0.1616,0,
0.1282,0.1601,0,
0.1417,0.1601,0,
0.1537,0.1576,0,
0.1632,0.1571,0,
0.1737,0.1566,0,
0.1882,0.1571,0,
0.2022,0.1581,0,
0.2172,0.1606,0,
0.2267,0.1621,0,
0.2342,0.1641,0,
0.2422,0.1671,0,
0.2457,0.1711,0,


0.0777,0.1501,0,
0.0842,0.1516,0,
0.0902,0.1531,0,
0.0982,0.1421,0,
0.1072,0.1416,0,
0.1167,0.1456,0,
0.1262,0.1426,0,
0.1402,0.1456,0,
0.1487,0.1421,0,
0.1552,0.1351,0,
0.1612,0.1341,0,
0.1692,0.1376,0,
0.1747,0.1441,0,
0.1852,0.1446,0,
0.1927,0.1421,0,
0.2032,0.1471,0,
0.2137,0.1421,0,
0.2212,0.1421,0,
0.2282,0.1476,0,
0.2307,0.1531,0,
0.2377,0.1506,0,
0.2437,0.1511,0,




0.1042, 0.2291, 0,
0.1017, 0.2256, 0,
0.1002, 0.2191, 0,
0.1002, 0.2136, 0,
0.1022, 0.2086, 0,
0.1017, 0.2001, 0,
0.1012, 0.1926, 0,
0.1017, 0.1881, 0,
0.1032, 0.1851, 0,
0.1067, 0.1816, 0,
0.1122, 0.1806, 0,
0.1192, 0.1786, 0,
0.1232, 0.1781, 0,
0.1277, 0.1766, 0,
0.1327, 0.1756, 0,
0.1382, 0.1756, 0,
0.1437, 0.1746, 0,
0.1512, 0.1746, 0,
0.1582, 0.1746, 0,
0.1642, 0.1746, 0,
0.1682, 0.1746, 0,
0.1737, 0.1746, 0,
0.1597, 0.1736, 0,
0.1707, 0.1746, 0,
0.1787, 0.1746, 0,
0.1867, 0.1756, 0,
0.1962, 0.1761, 0,
0.2032, 0.1776, 0,
0.1592, 0.1736, 0,
0.2067, 0.1801, 0,
0.2142, 0.1831, 0,
0.2177, 0.1906, 0,
0.2192, 0.1946, 0,
0.2177, 0.1976, 0,
0.2167, 0.2041, 0,
0.2172, 0.2086, 0,
0.2192, 0.2151, 0,
0.2187, 0.2216, 0,
0.2167, 0.2286, 0,
0.2107, 0.2341, 0,
0.2042, 0.2371, 0,
0.1972, 0.2386, 0,
0.1862, 0.2406, 0,
0.1817, 0.2416, 0,
0.1762, 0.2416, 0,
0.1667, 0.2421, 0,
0.1502, 0.2431, 0,
0.1402, 0.2421, 0,
0.1302, 0.2416, 0,
0.1162, 0.2386, 0,
0.1107, 0.2351, 0,
0.1062, 0.2326, 0,
0.1052, 0.2256, 0,
0.1127, 0.2231, 0,
0.1207, 0.2216, 0,
0.1297, 0.2206, 0,
0.1397, 0.2201, 0,
0.1482, 0.2191, 0,
0.1572, 0.2191, 0,
0.1647, 0.2191, 0,
0.1722, 0.2191, 0,
0.1807, 0.2196, 0,
0.1887, 0.2201, 0,
0.1962, 0.2211, 0,
0.2077, 0.2226, 0,
0.2142, 0.2261, 0



    };


    float  vertices[] = {
     0,0,0



    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
   glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);



    // Second object VAO and VBO
    unsigned int VBO2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
    glBindVertexArray(0); // unbind VAO


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // create transformations
        /*glm::mat4 trans = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        trans = glm::translate(trans, glm::vec3(translate_X, translate_Y, 0.0f));
        trans = glm::rotate(trans, glm:: radians(rotateAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::scale(trans,glm::vec3(scale_X, scale_Y, 1.0));*/
        glm::mat4 translationMatrix;
        glm::mat4 rotationMatrix;
        glm::mat4 scaleMatrix;
        glm::mat4 modelMatrix;
        glm::mat4 identityMatrix = glm::mat4(1.0f);


        translationMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, 0.0f));
        rotationMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, 1.0f));
        modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
        //modelMatrix = rotationMatrix * scaleMatrix;




        // get matrix's uniform location and set matrix
        glUseProgram(shaderProgram);
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        // draw our first triangle
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_LINE_STRIP, 0, 1);
      
        glBindVertexArray(0);// no need to unbind it every time


        // Transformation matrix for VAO2 (object 2)
        glm::mat4 translationMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(translate_X2, translate_Y2, 0.0f));
        glm::mat4 rotationMatrix2 = glm::rotate(glm::mat4(1.0f), glm::radians(rotateAngle2), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 scaleMatrix2 = glm::scale(glm::mat4(1.0f), glm::vec3(scale_X2, scale_Y2, 1.0f));
        glm::mat4 modelMatrix2 = translationMatrix2 * rotationMatrix2 * scaleMatrix2;

        // Set and apply the transformation for VAO2
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix2));



        glBindVertexArray(VAO2);
        glDrawArrays(GL_LINE_STRIP, 0, 82); // Draw vertices2[] as a line loop
        glDrawArrays(GL_LINE_STRIP, 82, 22); // Draw vertices2[] as a line loop
        glDrawArrays(GL_LINE_STRIP, 104, 66); // Draw vertices2[] as a line loop
        /*glDrawArrays(GL_LINE_STRIP, 46, 6); // Draw vertices2[] as a line loop
        glDrawArrays(GL_LINE_STRIP, 52, 11); // Draw vertices2[] as a line loop
        glDrawArrays(GL_LINE_STRIP, 63, 4); // Draw vertices2[] as a line loop
        glDrawArrays(GL_LINE_STRIP, 67, 5); // Draw vertices2[] as a line loop
        glDrawArrays(GL_LINE_STRIP, 72, 7); // Draw vertices2[] as a line loop
        glDrawArrays(GL_LINE_STRIP, 79, 3); // Draw vertices2[] as a line loop
        glDrawArrays(GL_LINE_STRIP, 82, 11); // Draw vertices2[] as a line loop
        glDrawArrays(GL_LINE_STRIP, 93, 7); // Draw vertices2[] as a line loop
        glDrawArrays(GL_LINE_STRIP, 100, 11); // Draw vertices2[] as a line loop
        glDrawArrays(GL_LINE_STRIP, 111, 5); // Draw vertices2[] as a line loop
        glDrawArrays(GL_LINE_STRIP, 116, 5); // Draw vertices2[] as a line loop
        glDrawArrays(GL_TRIANGLE_STRIP, 121, 38); // Draw vertices2[] as a line loop
        glDrawArrays(GL_TRIANGLES, 159, 3); // Draw vertices2[] as a line loop
        glDrawArrays(GL_TRIANGLES, 162, 3); // Draw vertices2[] as a line loop
        glDrawArrays(GL_TRIANGLES, 165, 3); // Draw vertices2[] as a line loop
        glDrawArrays(GL_TRIANGLES, 168, 3); // Draw vertices2[] as a line loop*/



        glBindVertexArray(0); // Unbind VAO

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        translate_X -= 0.002f;  // Move both left
        translate_X2 -= 0.002f; // Move plate left
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        translate_X += 0.002f;  // Move both right
        translate_X2 += 0.002f; // Move plate right
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        translate_Y += 0.002f;  // Move both up
        translate_Y2 += 0.002f; // Move plate up
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        translate_Y -= 0.002f;  // Move both down
        translate_Y2 -= 0.002f; // Move plate down
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        translate_Y += 0.002f;  // Move up
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        translate_Y -= 0.002f;  // Move down
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        translate_X -= 0.002f;  // Move left
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        translate_X += 0.002f;  // Move right

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        translate_X2 -= 0.002f;  // Move left
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        translate_X2 += 0.002f;  // Move right
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        translate_Y2 += 0.002f;  // Move up
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        translate_Y2 -= 0.002f;  // Move down

    //Rotation controls
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        rotateAngle2 += 0.2f;   // Rotate clockwise
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        rotateAngle2 -= 0.2f;   // Rotate counterclockwise

    // Scaling controls 
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        scale_Y2 += 0.002f;  // Increase vertical scale
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        scale_Y2 = std::max(scale_Y - 0.002f, 0.01f);  // Decrease vertical scale with minimum limit
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        scale_X2 += 0.002f;  // Increase horizontal scale
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        scale_X2 = std::max(scale_X - 0.002f, 0.01f);  // Decrease horizontal scale with minimum limit


    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float scale_increment = 0.2f;

    // Increase or decrease scale based on scroll direction
    if (yoffset > 0) {
        scale_Y += scale_increment; // Zoom in
        scale_X += scale_increment;
    }
    else {
        scale_Y = std::max(scale_Y - scale_increment, 0.01f); // Zoom out 
        scale_X = std::max(scale_X - scale_increment, 0.01f);
    }
}

float rotation_angle = 0.0f;
bool rotating_cake = false;  // Indicates if rotation is active
bool rotating_plate = false;  // Indicates if rotation is active
double lastX, lastY;    // Last mouse positions


// Mouse button callback to start/stop rotation
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rotating_cake = true;
            //translating_cake = true;
            glfwGetCursorPos(window, &lastX, &lastY);
            cout << "Mouse pressed at (" << lastX << ", " << lastY << ")\n";
        }
        else if (action == GLFW_RELEASE) {
            rotating_cake = false;
            cout << "Mouse released\n";
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            rotating_plate = true;
            rotating_cake = true;

            glfwGetCursorPos(window, &lastX, &lastY);
            cout << "Mouse pressed at (" << lastX << ", " << lastY << ")\n";
        }
        else if (action == GLFW_RELEASE) {
            rotating_plate = false;
            rotating_cake = false;

            cout << "Mouse released\n";
        }
    }


}

void cursor_position_callback(GLFWwindow* window, double xPOS, double yPOS) {
    if (rotating_cake && rotating_plate) {
        // Calculate the difference in mouse position

        float dY = lastY - yPOS;

        rotateAngle += (dY * 0.2f);
        rotateAngle2 += (dY * 0.2f);

        // Keep angles Within 0 to 360 degrees
        if (rotateAngle >= 360.0f) rotateAngle -= 360.0f;
        if (rotateAngle < 0.0f) rotateAngle += 360.0f;
        if (rotateAngle2 >= 360.0f) rotateAngle2 -= 360.0f;
        if (rotateAngle2 < 0.0f) rotateAngle2 += 360.0f;

        // Update last cursor position
        lastX = xPOS;
        lastY = yPOS;
    }
    else if (rotating_cake) {
        // Calculate the difference in mouse position
        float dY = lastY - yPOS;
        rotateAngle += (dY * 0.2);

        // Keep angle within 0 to 360 degrees
        if (rotateAngle >= 360.0f) rotateAngle -= 360.0f;
        if (rotateAngle < 0.0f) rotateAngle += 360.0f;

        // Update last cursor position
        lastX = xPOS;
        lastY = yPOS;

    }

}