// Bryn Mawr College, alinen, 2020
//

#define _USE_MATH_DEFINES

#include "AGL.h"
#include "AGLM.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include "mesh.h"
#include "osutils.h"

using namespace std;
using namespace glm;
using namespace agl;



// globals
Mesh theModel;
int theCurrentModel = 0;
vector<string> theModelNames;
bool shiftHeld = false;
bool mouseHeld = false;
float dist;
float Azimuth = 0;
float Elevation = 0;
float oldXPos, oldYPos = -1;

// OpenGL IDs
GLuint theVboPosId;
GLuint theVboNormalId;
GLuint theElementbuffer;

static void LoadModel(int modelId)
{
   assert(modelId >= 0 && modelId < theModelNames.size());
   theModel.loadPLY(theModelNames[theCurrentModel]);

   glBindBuffer(GL_ARRAY_BUFFER, theVboPosId);
   glBufferData(GL_ARRAY_BUFFER, theModel.numVertices() * 3 * sizeof(float), theModel.positions(), GL_DYNAMIC_DRAW);

   glBindBuffer(GL_ARRAY_BUFFER, theVboNormalId);
   glBufferData(GL_ARRAY_BUFFER, theModel.numVertices() * 3 * sizeof(float), theModel.normals(), GL_DYNAMIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theElementbuffer);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, theModel.numTriangles() * 3 * sizeof(unsigned int), theModel.indices(), GL_DYNAMIC_DRAW);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (action != GLFW_PRESS) return;

   if (key == GLFW_KEY_ESCAPE)
   {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
   }
   else if (key == 'P')
   {
      if (--theCurrentModel < 0)
      {
         theCurrentModel = theModelNames.size() - 1;
      }
      cout << "Current file: " << theModelNames[theCurrentModel] << endl;
      LoadModel(theCurrentModel);
   }
   else if (key == 'N')
   {
      theCurrentModel = (theCurrentModel + 1) % theModelNames.size(); 
      cout << "Current file: " << theModelNames[theCurrentModel] << endl;
      LoadModel(theCurrentModel);
   }

}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   // Prevent a divide by zero
   if(height == 0) height = 1;
	
   // Set Viewport to window dimensions
   glViewport(0, 0, width, height);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
   double xpos, ypos;
   glfwGetCursorPos(window, &xpos, &ypos);

   // TODO: Camera controls

   int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
   if (state == GLFW_PRESS)
   {
       shiftHeld = false;
       mouseHeld = false;
       int keyPress = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
       if (keyPress == GLFW_PRESS) {
           shiftHeld = true;
       }
       else {
           mouseHeld = true;
       }
   }
   else if (state == GLFW_RELEASE)
   {
       mouseHeld = false;
       shiftHeld = false;
   }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
   // TODO: Camera controls
    // initialize values
    if (oldYPos == -1) {
        oldXPos = xpos;
        oldYPos = ypos;
    }
    else if (shiftHeld) {
        float deltaY = ypos - oldYPos;

        dist = dist + deltaY / 200;
    }
    // rotating camera
    else if (mouseHeld) {
        float deltaX = xpos - oldXPos;
        float deltaY = ypos - oldYPos;

        Azimuth = Azimuth + deltaX / 500 * M_PI;
        Elevation = Elevation + deltaY / 500 * M_PI;

    }

    oldXPos = xpos;
    oldYPos = ypos;
}

static void PrintShaderErrors(GLuint id, const std::string label)
{
   std::cerr << label << " failed\n";
   GLint logLen;
   glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);
   if (logLen > 0)
   {
      char* log = (char*)malloc(logLen);
      GLsizei written;
      glGetShaderInfoLog(id, logLen, &written, log);
      std::cerr << "Shader log: " << log << std::endl;
      free(log);
   }
}

static std::string LoadShaderFromFile(const std::string& fileName)
{
   std::ifstream file(fileName);
   if (!file)
   {
      std::cout << "Cannot load file: " << fileName << std::endl;
      return "";
   }

   std::stringstream code;
   code << file.rdbuf();
   file.close();

   return code.str();
}

static void LoadModels(const std::string& dir)
{
   std::vector<std::string> filenames = GetFilenamesInDir(dir, "ply");
   for (int i = 0; i < filenames.size(); i++)
   {
      std::string filename = filenames[i];
      theModelNames.push_back(dir+filename);
   }
}

static GLuint LoadShader(const std::string& vertex, const std::string& fragment)
{
   GLint result;
   std::string vertexShader = LoadShaderFromFile(vertex);
   const char* vertexShaderRaw = vertexShader.c_str();
   GLuint vshaderId = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vshaderId, 1, &vertexShaderRaw, NULL);
   glCompileShader(vshaderId);
   glGetShaderiv(vshaderId, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(vshaderId, "Vertex shader");
      return -1;
   }

   std::string fragmentShader = LoadShaderFromFile(fragment);
   const char* fragmentShaderRaw = fragmentShader.c_str();
   GLuint fshaderId = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fshaderId, 1, &fragmentShaderRaw, NULL);
   glCompileShader(fshaderId);
   glGetShaderiv(fshaderId, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(fshaderId, "Fragment shader");
      return -1;
   }

   GLuint shaderId = glCreateProgram();
   glAttachShader(shaderId, vshaderId);
   glAttachShader(shaderId, fshaderId);
   glLinkProgram(shaderId);
   glGetShaderiv(shaderId, GL_LINK_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(shaderId, "Shader link");
      return -1;
   }
   return shaderId;
}


int main(int argc, char** argv)
{
   GLFWwindow* window;

   if (!glfwInit())
   {
      return -1;
   }

   // Explicitly ask for a 4.0 context 
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   /* Create a windowed mode window and its OpenGL context */
   window = glfwCreateWindow(500, 500, "Mesh Viewer", NULL, NULL);
   if (!window)
   {
      glfwTerminate();
      return -1;
   }

   // Make the window's context current 
   glfwMakeContextCurrent(window);

   glfwSetKeyCallback(window, key_callback);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
   glfwSetMouseButtonCallback(window, mouse_button_callback);
   glfwSetScrollCallback(window, scroll_callback);
   glfwSetCursorPosCallback(window, cursor_position_callback);

#ifndef APPLE
   if (glewInit() != GLEW_OK)
   {
      return -1;
   }
#endif

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glClearColor(0, 0, 0, 1);

   glGenBuffers(1, &theVboPosId);
   glGenBuffers(1, &theVboNormalId);
   glGenBuffers(1, &theElementbuffer);

   GLuint vaoId;
   glGenVertexArrays(1, &vaoId);
   glBindVertexArray(vaoId);

   glEnableVertexAttribArray(0); // 0 -> Sending VertexPositions to array #0 in the active shader
   glBindBuffer(GL_ARRAY_BUFFER, theVboPosId); // always bind before setting data
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

   glEnableVertexAttribArray(1); // 1 -> Sending Normals to array #1 in the active shader
   glBindBuffer(GL_ARRAY_BUFFER, theVboNormalId); // always bind before setting data
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

   LoadModels("../models/");
   LoadModel(0);

   GLuint shaderId = LoadShader("../shaders/phong.vs", "../shaders/phong.fs");
   glUseProgram(shaderId);

   GLuint mvpId = glGetUniformLocation(shaderId, "uMVP"); 
   GLuint mvId = glGetUniformLocation(shaderId, "uMV"); 
   GLuint nmvId = glGetUniformLocation(shaderId, "uNMV");
   //GLuint pId = glGetUniformLocation(shaderId, "uP");


   glUniform3f(glGetUniformLocation(shaderId, "Ks"), 1.0, 1.0, 1.0);
   glUniform3f(glGetUniformLocation(shaderId, "Kd"), 0.4, 0.6, 1.0);
   glUniform3f(glGetUniformLocation(shaderId, "Ks"), 0.4, 0.4, 0.4);
   glUniform1f(glGetUniformLocation(shaderId, "Shininess"), 80.0f);
   glUniform4f(glGetUniformLocation(shaderId, "LightPosition"), 100.0, 100.0, 100.0, 0.0);
   glUniform3f(glGetUniformLocation(shaderId, "LightIntensity"), 1.0, 1.0, 1.0);


   // mvp stuff
   GLuint matrixParam = glGetUniformLocation(shaderId, "mvp");
   glm::mat4 transform(1.0); // initialize to identity
   glm::mat4 projection = glm::perspective(glm::radians(60.0), 1.0, 0.1, 10.0);


   glm::vec3 cameraPos(0, 0, 3);
   glm::vec3 origin(0);
   dist = glm::length(cameraPos - origin);
   float x = dist * sin(Azimuth) * cos(Elevation) + origin.x;
   float y = dist * sin(Elevation) + origin.y;
   float z = dist * cos(Azimuth) * cos(Elevation) + origin.z;
   glm::mat4 camera = glm::lookAt(cameraPos, origin, glm::vec3(0, 1, 0));



   // Loop until the user closes the window 
   while (!glfwWindowShouldClose(window))
   {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers

      // camera animation
      x = dist * sin(Azimuth) * cos(Elevation) + origin.x;
      y = dist * sin(Elevation) + origin.y;
      z = dist * cos(Azimuth) * cos(Elevation) + origin.z;
      camera = glm::lookAt(vec3(x, y, z), origin, glm::vec3(0, 1, 0));

      mat4 mvp = projection * camera * transform;
      mat4 mv = camera * transform;
      mat3 nmv = mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]));
      glUniformMatrix3fv(nmvId, 1, GL_FALSE, &nmv[0][0]); 
      glUniformMatrix4fv(mvId, 1, GL_FALSE, &mv[0][0]); 
      glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);
      

      // Draw primitive
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theElementbuffer);
      glDrawElements(GL_TRIANGLES, theModel.numTriangles() * 3, GL_UNSIGNED_INT, (void*)0);

      // Swap front and back buffers
      glfwSwapBuffers(window);

      // Poll for and process events
      glfwPollEvents();
   }

   glfwTerminate();
   return 0;
}


