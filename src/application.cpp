#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <math.h>  

#define STB_IMAGE_IMPLEMENTATION
#include "vendors/stb_image.h"


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h> 


float xMove = 0.0f;
float zMove = 10.0f;
float prevXpos = 0.0f;
double prevYpos = 0.0f;
float xlook = 0.0f;
float ylook = 0.0f;
float zlook = 0.0f;
float curAngley = 0.0f;
float curAnglex = 0.0f;
int counter = 0;
bool isSinAnnoying = false;


struct ShaderSourceCode {
    std::string vertexSource;
    std::string fragmentSource;


};

using namespace std;

static ShaderSourceCode ParseShader(const string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    string line;
    stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else  if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }

        }
        else {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

//this was the old copy
//this was the old copy

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        glm::vec3 direction = cross(glm::vec3(xlook - xMove, 0.0f, zlook - zMove), glm::vec3(0.0f, -1.0f, 0.0f));
        direction = glm::normalize(direction);
        xMove = direction.x + xMove;
        zMove = direction.z + zMove;
        xlook = direction.x + xlook;
        zlook = direction.z + zlook;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        glm::vec3 direction = cross(glm::vec3(xlook - xMove, 0.0f, zlook - zMove), glm::vec3(0.0f, 1.0f, 0.0f));
        direction = glm::normalize(direction);
        xMove = direction.x + xMove;
        zMove = direction.z + zMove;
        xlook = direction.x + xlook;
        zlook = direction.z + zlook;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {

        glm::vec3 direction = glm::normalize(glm::vec3(xlook - xMove, 0.0f, zlook - zMove));
        xMove = direction.x + xMove;
        zMove = direction.z + zMove;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        glm::vec3 direction = glm::normalize(glm::vec3(xMove - xlook, 0.0f, zMove - zlook));   //zmove shoud ve zPos 
        cout << xlook << xMove << endl;
        cout << zlook << zMove << endl << endl;
        xMove = xMove + direction.x;
        zMove = zMove + direction.z;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) //this gets called every time cursor moves
{
    int sensitivity = 5;

    //determine if mouse movement in the x or y
    //if x movemment do this
    if (counter == 0) {
        counter++;
        prevXpos = xpos;
        prevYpos = ypos;
        return;
    }

    if (prevYpos - (float)ypos != 0.0f) {
        float offset = prevYpos - (float)ypos; //+ means right, - means left
        float deltaAngle = offset / sensitivity;  //the 
        float newAngle = curAngley + deltaAngle;

        if (newAngle < 90.0f && newAngle > -90.0f)
        {
            //delta angle is what you rotate the position looking by
            glm::mat4 trans = glm::mat4(1.0f);
            glm::vec3 perpendicularVec;
            glm::vec3 lookToStand = glm::vec3(xlook - xMove, ylook - 0.0f, zlook - zMove);
            perpendicularVec.x = 1.0f;
            perpendicularVec.y = 0.0f;
            perpendicularVec.z = (0.0f - perpendicularVec.x * lookToStand.x) / lookToStand.z;

            trans = glm::rotate(trans, glm::radians(deltaAngle), glm::normalize(perpendicularVec));
            glm::vec4 rotatedVector = trans * glm::vec4(lookToStand.x, lookToStand.y, lookToStand.z, 1.0f);
            xlook = rotatedVector.x + xMove;
            ylook = rotatedVector.y;
            zlook = rotatedVector.z + zMove;
            cout << xlook << " " << ylook << " " << zlook << endl;


            curAngley = newAngle;
        }
    }

        //if x movemment do this
      if(prevXpos - (float)xpos != 0.0f)  {
            float offset = (float)xpos - prevXpos; //+ means right, - means left
            float deltaAngle = offset / sensitivity;  //the
            float newAngle = curAnglex + deltaAngle;

            if (newAngle > 360)newAngle = newAngle - 360;
            if (newAngle < 0) newAngle = newAngle + 360;

            //calculate new x and z coordinates
            xlook = xMove + 10 * cos(glm::radians(newAngle));
            zlook = zMove + (10 * sin(glm::radians(newAngle)));
            curAnglex = newAngle;
        }

        prevXpos = xpos;
        prevYpos = ypos;


    
}



//make a function that compiles shader
static unsigned int compileShader(unsigned int type, const std::string& source, int name) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    int success;
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success)
    {

        std::cout << "specific success" << endl;
    }
    return id;
}


int main(void)
{


    float texCoords[] = {
    0.0f, 0.0f,  // lower-left corner  
    1.0f, 0.0f,  // lower-right corner
    0.5f, 1.0f   // top-center corner
    };

    float vertices[] = {
        // positions          // colors           // texture coords
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f
    };

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



    unsigned int indices[] = {  // note that we start from 0!
    0, 1, 2,   // first triangle
    0, 2, 3    // second triangle
    };

    ShaderSourceCode source = ParseShader("resources/shaders/allShader.shader");
    unsigned int VBO;
    float getValue[16];
    unsigned int VAO;
    unsigned int EBO;
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;
    int success;
    int width, height, nrChannels;
    unsigned char* data = stbi_load("resources/container.jpg", &width, &height, &nrChannels, 0);
    unsigned int texture;
    glm::mat4 trans = glm::mat4(0.5f);
    std::cout << glm::to_string(trans) << std::endl;
    GLFWwindow* window;



    // Initialize the library
    if (!glfwInit())
        return -1;


    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(1200, 800, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error" << std::endl;

    //top left corner is 0,0

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);

    //opengl require two shaders vertex and fragment shader
    vertexShader = compileShader(GL_VERTEX_SHADER, source.vertexSource, 1);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, source.fragmentSource, 0);

    glEnable(GL_DEPTH_TEST);

    //error right here
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_ATTACHED_SHADERS, &success);
    if (success) {
        std::cout << "good" << endl;
    }
    else {
        std::cout << "fail" << endl;
    }


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    //create vertex buffer place to store information
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenTextures(1, &texture);
    //storing vertex is gl-array-buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //copies vertex data into buffer memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindTexture(GL_TEXTURE_2D, texture);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);



    // Loop until the user closes the window 
    while (!glfwWindowShouldClose(window)) {

        glBindVertexArray(VAO);
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);


        // create transformations
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);


        view = glm::lookAt(glm::vec3(xMove, 0.0f, zMove),
            glm::vec3(xlook, ylook, zlook),
            glm::vec3(0.0f, 1.0f, 0.0f));


        projection = glm::perspective(glm::radians(65.0f), (float)1200 / (float)800, 0.1f, 500000.0f);
        // retrieve the matrix uniform locations
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(65.0f), glm::vec3(0.5f, 1.0f, 0.0f)); //translation matrix * rotation matrix
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // update other events like input handling
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();


    return 0;
}

