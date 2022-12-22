// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#else
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#endif

// OpenGL Mathematics Library
#include <glm/glm.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>

// Timer
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

float FOV = 35.0f;
float distance= 5.0;
int select = 0;

BufferObject VBO; BufferObject NBO; BufferObject Index; //head
BufferObject VBO2; BufferObject NBO2; BufferObject Index2; //neck
BufferObject VBO3; BufferObject NBO3; BufferObject Index3; //hair
BufferObject VBO4; BufferObject NBO4; BufferObject Index4; //hair
BufferObject VBO5; BufferObject NBO5; BufferObject Index5; //hair
BufferObject VBO6; BufferObject NBO6; BufferObject Index6; //hair
BufferObject VBO7; BufferObject NBO7; BufferObject Index7; //hair
BufferObject modelO;
BufferObject UVBuffer;

//vertex vector, normals vector, triangles vector * 3 vertexes
std::vector<glm::vec3> V(3); std::vector<glm::vec3> N(3); std::vector<glm::ivec3> T(3);//head
std::vector<glm::vec3> V2(3); std::vector<glm::vec3> N2(3); std::vector<glm::ivec3> T2(3);//neck
std::vector<glm::vec3> V3(3); std::vector<glm::vec3> N3(3); std::vector<glm::ivec3> T3(3);//hair?
std::vector<glm::vec3> V4(3); std::vector<glm::vec3> N4(3); std::vector<glm::ivec3> T4(3);//hair?
std::vector<glm::vec3> V5(3); std::vector<glm::vec3> N5(3); std::vector<glm::ivec3> T5(3);//hair?
std::vector<glm::vec3> V6(3); std::vector<glm::vec3> N6(3); std::vector<glm::ivec3> T6(3);//hair?
std::vector<glm::vec3> V7(3); std::vector<glm::vec3> N7(3); std::vector<glm::ivec3> T7(3);//hair?

std::vector<glm::vec2> UV(0);


//CAMERA
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

//LIGHT SOURCE
glm::vec3 lightPos = glm::normalize(glm::vec3(-1.0f, 2.0f, 3.0f));
int rotateLight = 3;
float ambient = 0.1f;
float specularExponent = 50.0f;


//view matrix
glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, up);

//projection matrix
glm::mat4 projection = glm::perspective(glm::radians(FOV), 800.0f/600.0f, 0.1f, 100.0f);

//model matrix
glm::mat4 model(1.0f);

//Positions:
glm::vec3 modelpos5 = glm::vec3(0.0f, 0.0f, 0.0f); //right lid
glm::vec3 modelpos7 = glm::vec3(0.0f, 0.0f, 0.0f); //left lid


class RGB
{
public:
	unsigned char R;
	unsigned char G;
	unsigned char B;

	RGB(unsigned char r, unsigned char g, unsigned char b)
	{
		R = r;
		G = g;
		B = b;
	}

	bool Equals(RGB rgb)
	{
		return (R == rgb.R) && (G == rgb.G) && (B == rgb.B);
	}
};

static float HueToRGB(float v1, float v2, float vH) {
	if (vH < 0)
		vH += 1;

	if (vH > 1)
		vH -= 1;

	if ((6 * vH) < 1)
		return (v1 + (v2 - v1) * 6 * vH);

	if ((2 * vH) < 1)
		return v2;

	if ((3 * vH) < 2)
		return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

	return v1;
}

static RGB HSLToRGB(int H, float S, float L) {
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;

	if (S == 0)
	{
		r = g = b = (unsigned char)(L * 255);
	}
	else
	{
		float v1, v2;
		float hue = (float)H / 360;

		v2 = (L < 0.5) ? (L * (1 + S)) : ((L + S) - (L * S));
		v1 = 2 * L - v2;

		r = (unsigned char)(255 * HueToRGB(v1, v2, hue + (1.0f / 3)));
		g = (unsigned char)(255 * HueToRGB(v1, v2, hue));
		b = (unsigned char)(255 * HueToRGB(v1, v2, hue - (1.0f / 3)));
	}
	return RGB(r, g, b);
}

//colors
int hairHue = 25; int bodyHue = 25;
float hairS = 0.75f; float bodyS = 1.0f;
float hairL = 0.30f; float bodyL = 0.75f;

float BBX; float BBY; float BBZ; float B1X; float B1Y; float B1Z; float B2X; float B2Y; float B2Z;
float B3X; float B3Y; float B3Z; float B4X; float B4Y; float B4Z; float B5X; float B5Y; float B5Z;
float BDX; float BDY; float BDZ;

float HBX; float HBY; float HBZ; float H1X; float H1Y; float H1Z; float H2X; float H2Y; float H2Z;
float H3X; float H3Y; float H3Z; float H4X; float H4Y; float H4Z; float H5X; float H5Y; float H5Z;
float HDX; float HDY; float HDZ;

void updateBodyCol(int& H, float& S, float& L, float& basex, float& basey, float& basez, float& ONEX, float& ONEY, float& ONEZ, float& TWOX, float& TWOY, float& TWOZ,
float& THREEX, float& THREEY, float& THREEZ, float& FOURX, float& FOURY, float& FOURZ, float& FIVEX, float& FIVEY, float& FIVEZ,
float& DARKX, float& DARKY, float& DARKZ){
    RGB bodyBase = HSLToRGB(H, S, L);
    basex = float((int)bodyBase.R); basey = float((int)bodyBase.G); basez = float((int)bodyBase.B); 
    RGB body1 = HSLToRGB(H, (S * (0.885f)), (L * (0.885f))); 
    ONEX = float((int)body1.R); ONEY = float((int)body1.G); ONEZ = float((int)body1.B);
    RGB body2 = HSLToRGB(H, (S * (0.625f)), (L * (0.625f)));
    TWOX = float((int)body2.R); TWOY = float((int)body2.G); TWOZ = float((int)body2.B);
    RGB body3 = HSLToRGB(H, (S * (0.795f)), (L * (0.295f)));
    THREEX = float((int)body3.R); THREEY = float((int)body3.G); THREEZ = float((int)body3.B);
    RGB body4 = HSLToRGB(H, (1.0f), (L * (0.175)));
    FOURX = float((int)body4.R); FOURY = float((int)body4.G); FOURZ = float((int)body4.B);
    RGB body5 = HSLToRGB(H, (1.0f), (L * (0.13)));
    FIVEX = float((int)body4.R); FIVEY = float((int)body4.G); FIVEZ = float((int)body4.B);
    RGB bodyDark = HSLToRGB(bodyHue, (1.0f), (L * (0.06)));
    DARKX = float((int)body4.R); DARKY = float((int)body4.G); DARKZ = float((int)body4.B);
}



float camRadius = 5.0f;

// PPM Reader code from http://josiahmanson.com/prose/optimize_ppm/

struct color {
    unsigned char r, g, b;
};

struct ImageRGB {
    int w, h;
    std::vector<color> data;
};

void eat_comment(std::ifstream& f) {
    char linebuf[1024];
    char ppp;
    while (ppp = f.peek(), ppp == '\n' || ppp == '\r')
        f.get();
    if (ppp == '#')
        f.getline(linebuf, 1023);
}

bool loadPPM(ImageRGB& img, const std::string& name) {
    std::ifstream f(name.c_str(), std::ios::binary);
    if (f.fail()) {
        std::cout << "Could not open file: " << name << std::endl;
        return false;
    }

    // get type of file
    eat_comment(f);
    int mode = 0;
    std::string s;
    f >> s;
    if (s == "P3")
        mode = 3;
    else if (s == "P6")
        mode = 6;

    // get w
    eat_comment(f);
    f >> img.w;

    // get h
    eat_comment(f);
    f >> img.h;

    // get bits
    eat_comment(f);
    int bits = 0;
    f >> bits;

    // error checking
    if (mode != 3 && mode != 6) {
        std::cout << "Unsupported magic number" << std::endl;
        f.close();
        return false;
    }
    if (img.w < 1) {
        std::cout << "Unsupported width: " << img.w << std::endl;
        f.close();
        return false;
    }
    if (img.h < 1) {
        std::cout << "Unsupported height: " << img.h << std::endl;
        f.close();
        return false;
    }
    if (bits < 1 || bits > 255) {
        std::cout << "Unsupported number of bits: " << bits << std::    endl;
        f.close();
        return false;
    }

    // load image data
    img.data.resize(img.w * img.h);

    if (mode == 6) {
        f.get();
        f.read((char*)&img.data[0], img.data.size() * 3);
    }
    else if (mode == 3) {
        for (int i = 0; i < img.data.size(); i++) {
            int v;
            f >> v;
            img.data[i].r = v;
            f >> v;
            img.data[i].g = v;
            f >> v;
            img.data[i].b = v;
        }
    }

    // close file
    f.close();
    return true;
}


void update_camera(glm::vec3 newPos)
{
    cameraPos = newPos;
    cameraDirection = glm::normalize(cameraPos-cameraTarget);
    cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    cameraUp = glm::cross(cameraDirection, cameraRight);
    view = glm::lookAt(cameraPos, cameraTarget, up);
}

void calculateNorms(std::vector<glm::vec3>& V, std::vector<glm::vec3>& N, std::vector<glm::ivec3>& T){
    std::vector<glm::vec3> faceN(3);
    faceN.resize(T.size());

    for (unsigned int i = 0; i < faceN.size(); i++) {
        faceN[i] = glm::normalize(glm::cross(V[T[i].y] - V[T[i].x], V[T[i].z] - V[T[i].x]));
    }

    N.resize(V.size());
    for (unsigned int i = 0; i < N.size(); i++) {
        N[i] = glm::vec3(0.0f);
    }
    for (unsigned int j = 0; j < T.size(); j++) {
        N[T[j].x] += faceN[j];
        N[T[j].y] += faceN[j];
        N[T[j].z] += faceN[j];
    }
    for (unsigned int i = 0; i < N.size(); i++) {
        N[i] = glm::normalize(N[i]);
    }
}

bool loadOFFFile(std::string filename, std::vector<glm::vec3>& vertex, std::vector<glm::ivec3>& tria, glm::vec3& pos)
{

    try {
        std::ifstream ofs(filename, std::ios::in | std::ios_base::binary);
        std::string line, data;
        int Vert = 0;
        int Face = 0;
        int side = 0;
        // first line must be OFF
        getline(ofs, line);
        if (line.rfind("OFF", 0) == 0)
            getline(ofs, line);
        std::stringstream read(line);
        getline(read, data, ' ');
        Vert = std::stoi(data);
        getline(read, data, ' ');
        Face = std::stoi(data);
        getline(read, data, ' ');
        side = std::stoi(data);

        vertex.resize(Vert);
        for (int i = 0; i < Vert; i++) {
            getline(ofs, line);
            read.clear();
            read.str(line);
            getline(read, data, ' ');
            vertex[i].x = std::stof(data) + pos.x;
            getline(read, data, ' ');
            vertex[i].y = std::stof(data)+ pos.y;
            getline(read, data, ' ');
            vertex[i].z = std::stof(data)+ pos.z;
        }

        tria.resize(Face);
        for (int i = 0; i < Face; i++) {
            getline(ofs, line);
            read.clear();
            read.str(line);
            getline(read, data, ' ');
            getline(read, data, ' ');
            tria[i].x = std::stoi(data);
            getline(read, data, ' ');
            tria[i].y = std::stoi(data);
            getline(read, data, ' ');
            tria[i].z = std::stoi(data);
        }

        ofs.close();
        return true;
    }
    catch (const std::exception& e) {
        // return false if an exception occurred
        std::cerr << e.what() << std::endl;
        return false;
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Get the position of the mouse in the window
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    double xworld = ((xpos/double(width))*2)-1;
    double yworld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw

    // Update the position of the first vertex if the left button is pressed

    // Upload the change to the GPU
    VBO.update(V);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Update the position of the first vertex if the keys 1,2, or 3 are pressed
    switch (key)
    {

        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_A:
            update_camera(glm::normalize(cameraPos-0.5f*cameraRight));
            break;
        case GLFW_KEY_D:
            update_camera(glm::normalize(cameraPos+0.5f*cameraRight));
            break;
        case GLFW_KEY_S:
            update_camera(glm::normalize(cameraPos-0.5f*cameraUp));
            break;
        case GLFW_KEY_W:
            update_camera(glm::normalize(cameraPos+0.5f*cameraUp));
            break;
        case GLFW_KEY_UP:
            distance = distance+0.25;
            update_camera(cameraPos-glm::normalize(cameraDirection)*0.25f);
            break;
        case GLFW_KEY_DOWN:
            update_camera(cameraPos+glm::normalize(cameraDirection)*0.25f);
            distance = distance-0.25;
            break;
        case GLFW_KEY_R:
            update_camera(glm::vec3(0.0f, 0.0f, 5.0f));
            distance = distance+0.25;
            break;
         case GLFW_KEY_H:
            select = 1;
            break;
        case GLFW_KEY_B:
            select = 2;
            break;
        case GLFW_KEY_2:
            if(select == 1){ //hair
                hairHue += 5;
            }
            if(select == 2){ //body
                bodyHue += 5;
            }
            break;
        case GLFW_KEY_1:
            if(select == 1){ //hair
                hairHue -= 5;
            }
            if(select == 2){ //body
                bodyHue -= 5;
            }
            break;
        case GLFW_KEY_3:
            if(select == 1){ //hair
                if(hairS >0){hairS -= 0.01f;}
            }
            if(select == 2){ //body
                if(bodyS >0){bodyS -= 0.01f;}
            }
            break;
        case GLFW_KEY_4:
            if(select == 1){ //hair
                if(hairS < 1.0 ){hairS += 0.01f;}
            }
            if(select == 2){ //body
                if(bodyS < 1.0 ){bodyS += 0.01f;}
            }
            break;
        case GLFW_KEY_5:
            if(select == 1){ //hair
                if(hairL >0){hairL -= 0.01f;}
            }
            if(select == 2){ //body
                if(bodyL >0){bodyL -= 0.01f;}
            }
            break;
        case GLFW_KEY_6:
            if(select == 1){ //hair
                if(hairL < 1.0 ){hairL += 0.01f;}
            }
            if(select == 2){ //body
                if(bodyL < 1.0 ){bodyL += 0.01f;}
            }
            break;
        case GLFW_KEY_C:
            rotateLight=1;
            break;
        case GLFW_KEY_V:
            rotateLight=2;
            break;
        case GLFW_KEY_SPACE:
            rotateLight=3;
            break;
        case GLFW_KEY_I:
            ambient+=0.05f;
            ambient = std::min(ambient, 1.0f);
            specularExponent+=50.0f;
            specularExponent = std::min(specularExponent, 2000.0f);
            break;
        case GLFW_KEY_L:
            ambient-=0.05f;
            ambient = std::max(ambient, 0.2f);
            specularExponent-=50.0f;
            specularExponent = std::max(specularExponent, 200.0f);
            break;
        default:
            break;
    }
    VBO.update(V);
    updateBodyCol(bodyHue, bodyS, bodyL, BBX, BBY, BBZ, B1X, B1Y, B1Z, B2X, B2Y, B2Z, B3X, B3Y, B3Z, B4X, B4Y, B4Z, B5X, B5Y, B5Z, BDX, BDY, BDZ);
    updateBodyCol(hairHue, hairS, hairL, HBX, HBY, HBZ, H1X, H1Y, H1Z, H2X, H2Y, H2Z, H3X, H3Y, H3Z, H4X, H4Y, H4Z, H5X, H5Y, H5Z, HDX, HDY, HDZ);

}

void drawObject(Program prog, BufferObject vbo, BufferObject nbo, BufferObject index, std::vector<glm::ivec3> tri){
    int perV = 3;
    prog.bindVertexAttribArray("position", vbo);
    prog.bindVertexAttribArray("normal", nbo);
    index.bind();
    glDrawElements(GL_TRIANGLES, tri.size() * perV, GL_UNSIGNED_INT, 0);
}



int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "Cartoon Style", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    #ifndef __APPLE__
      glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
       std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    #endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    std::cout << "OpenGL version recieved: " << major << "." << minor << "." << rev << std::endl;
    std::cout << "Supported OpenGL is " << (const char*)glGetString(GL_VERSION) << std::endl;
    std::cout << "Supported GLSL is " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    updateBodyCol(bodyHue, bodyS, bodyL, BBX, BBY, BBZ, B1X, B1Y, B1Z, B2X, B2Y, B2Z, B3X, B3Y, B3Z, B4X, B4Y, B4Z, B5X, B5Y, B5Z, BDX, BDY, BDZ);
    updateBodyCol(hairHue, hairS, hairL, HBX, HBY, HBZ, H1X, H1Y, H1Z, H2X, H2Y, H2Z, H3X, H3Y, H3Z, H4X, H4Y, H4Z, H5X, H5Y, H5Z, HDX, HDY, HDZ);

    // Initialize the VAO
    // A Vertex Array Object (or VAO) is an object that describes how the vertex
    // attributes are stored in a Vertex Buffer Object (or VBO). This means that
    // the VAO is not the actual object storing the vertex data,
    // but the descriptor of the vertex data.
    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();
//head
    VBO.init();
    NBO.init();
    Index.init(GL_ELEMENT_ARRAY_BUFFER);
//neck
    VBO2.init();
    NBO2.init();
    Index2.init(GL_ELEMENT_ARRAY_BUFFER);
//hair
    VBO3.init();
    NBO3.init();
    Index3.init(GL_ELEMENT_ARRAY_BUFFER);
//right eye
    VBO4.init();
    NBO4.init();
    Index4.init(GL_ELEMENT_ARRAY_BUFFER);
//right lid
    VBO5.init();
    NBO5.init();
    Index5.init(GL_ELEMENT_ARRAY_BUFFER);
//left eye
    VBO6.init();
    NBO6.init();
    Index6.init(GL_ELEMENT_ARRAY_BUFFER);
//left lid
    VBO7.init();
    NBO7.init();
    Index7.init(GL_ELEMENT_ARRAY_BUFFER);
//UV
    UVBuffer.init();


    //CREATE MODELS

//head
    glm::vec3 modelpos = glm::vec3(0.0f, 0.0f, 0.0f);
    loadOFFFile("../data/headcentered.off", V, T, modelpos);
    VBO.update(V);
    Index.update(T);
    calculateNorms(V,N,T);
    NBO.init();
    NBO.update(N);
//neck
    glm::vec3 modelpos2 = glm::vec3(0.0f, 0.0f, 0.0f);
    loadOFFFile("../data/neckcentered.off", V2, T2, modelpos2);
    VBO2.update(V2);
    Index2.update(T2);
    calculateNorms(V2,N2,T2);
    NBO2.init();
    NBO2.update(N2);
//hair
    glm::vec3 modelpos3 = glm::vec3(0.0f, 0.0f, 0.0f);
    loadOFFFile("../data/hair.off", V3, T3, modelpos3);
    VBO3.update(V3);
    Index3.update(T3);
    calculateNorms(V3,N3,T3);
    NBO3.init();
    NBO3.update(N3);
//right eye
    glm::vec3 modelpos4 = glm::vec3(0.0f, 0.0f, 0.0f);
    loadOFFFile("../data/righteye.off", V4, T4, modelpos4);
    VBO4.update(V4);
    Index4.update(T4);
    calculateNorms(V4,N4,T4);
    NBO4.init();
    NBO4.update(N4);
//right lid
    loadOFFFile("../data/rightlid.off", V5, T5, modelpos5);
    VBO5.update(V5);
    Index5.update(T5);
    calculateNorms(V5,N5,T5);
    NBO5.init();
    NBO5.update(N5);
//left eye
    glm::vec3 modelpos6 = glm::vec3(0.0f, 0.0f, 0.0f);
    loadOFFFile("../data/lefteye.off", V6, T6, modelpos6);
    VBO6.update(V6);
    Index6.update(T6);
    calculateNorms(V6,N6,T6);
    NBO6.init();
    NBO6.update(N6);
//left lid
    loadOFFFile("../data/leftlid.off", V7, T7, modelpos7);
    VBO7.update(V7);
    Index7.update(T7);
    calculateNorms(V7,N7,T7);
    NBO7.init();
    NBO7.update(N7);

//image


    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid
    Program program;
    // load fragment shader file
    const GLchar* vertex_shader =
            "#version 150 core\n"
                    "uniform mat4 modelMatrix;"
                    "uniform mat4 viewMatrix;"
                    "uniform mat4 projMatrix;"

                    "uniform vec3 BaseB;"
                    "uniform vec3 BA;"
                    "uniform vec3 BB;"
                    "uniform vec3 BC;"
                    "uniform vec3 BD;"
                    "uniform vec3 BE;"
                    "uniform vec3 BDark;"

                    "uniform vec3 BaseH;"
                    "uniform vec3 HA;"
                    "uniform vec3 HB;"
                    "uniform vec3 HC;"
                    "uniform vec3 HD;"
                    "uniform vec3 HE;"


                    "uniform vec3 lightPos;"
                    "uniform vec3 lightParams;"
                    "uniform float camX;"
                    "uniform float camY;"
                    "uniform float camZ;"
                    "uniform float Ambient;"
                    "uniform float specEx;"
                    "uniform float objectnum;"

                    "in vec3 position;"
                    "in vec3 normal;"

                    "out vec3 n;"
                    "out vec3 pos;"
                    "out float object;"

                    "out vec3 basehair;"
                    "out vec3 baseskin;"

                    "out vec3 hairA;"
                    "out vec3 hairB;"
                    "out vec3 hairC;"
                    "out vec3 hairD;"
                    "out vec3 hairE;"

                    "out vec3 skinA;"
                    "out vec3 skinB;"
                    "out vec3 skinC;"
                    "out vec3 skinD;"
                    "out vec3 skinE;"
                    "out vec3 skinDark;"

                    "void main()"
                    "{"
                        "vec3 camera = vec3(camX, camY, camZ);"
                        "n = mat3(transpose(inverse(modelMatrix))) * normal;"
                        "pos = vec3(modelMatrix * vec4(position, 1.0));"
                        "object = objectnum;"

                        "basehair = BaseH;"
                        "baseskin = BaseB;"

                        "hairA = HA;"
                        "hairB = HB;"
                        "hairC = HC;"
                        "hairD = HD;"
                        "hairE = HE;"

                        "skinA = BA;"
                        "skinB = BB;"
                        "skinC = BC;"
                        "skinD = BD;"
                        "skinE = BE;"
                        "skinDark = BDark;"

                        "gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);"
                    "}";
    const GLchar* fragment_shader =
            "#version 150 core\n"
                    "out vec4 outColor;"
                    "in vec3 n;"
                    "in vec3 pos;"
                    "in float object;"

                    "in vec3 basehair;"
                    "in vec3 baseskin;"

                    "in vec3 hairA;"
                    "in vec3 hairB;"
                    "in vec3 hairC;"
                    "in vec3 hairD;"
                    "in vec3 hairE;"

                    "in vec3 skinA;"
                    "in vec3 skinB;"
                    "in vec3 skinC;"
                    "in vec3 skinD;"
                    "in vec3 skinE;"
                    "in vec3 skinDark;"

                    "uniform vec3 lightPos;"
                    "uniform vec3 lightParams;"
                    "uniform float camX;"
                    "uniform float camY;"
                    "uniform float camZ;"
                    "uniform float Ambient;"
                    "uniform float specEx;"
                    "uniform float objectnum;"
                    "float intensity;"

                    "void main()"
                    "{"
                        "vec3 camera = vec3(camX, camY, camZ);"

                        "vec3 lightDir = normalize(lightPos - pos);"
                        "intensity = dot(lightDir,normalize(n));"

                        "if (object == 1.0f){"
                            "if (intensity > 0.95){" 
                                "outColor = vec4(skinDark, 1.0);"
                            "}"
                            "if (intensity > 0.91){"
                                "outColor = vec4(baseskin, 1.0);"
                            "}"
                            "else if (intensity > 0.75){"
                                "outColor = vec4(skinA, 1.0);"
                            "}"
                            "else if (intensity > 0.5){" 
                                "outColor = vec4(skinB, 1.0);"
                            "}"
                            "else if (intensity > 0.25){" 
                                "outColor = vec4(skinC, 1.0);"
                            "}"
                            "else if (intensity > 0.15){"
                                "outColor = vec4(skinD, 1.0);"
                            "}"
                            "else if (intensity > 0.09){"
                                "outColor = vec4(skinE, 1.0);"
                            "}"
                        "}"

                        "if (object == 2.0f){"
                            "if (intensity > 0.95){"
                                "outColor = vec4(0.0, 0.0, 0.0, 1.0);"
                            "}"
                            "if (intensity > 0.85){" 
                                "outColor = vec4(basehair, 1.0);"
                            "}"
                            "else if (intensity > 0.75){"
                                "outColor = vec4(hairA, 1.0);"
                            "}"
                            "else if (intensity > 0.5){"
                                "outColor = vec4(hairB, 1.0);"
                            "}"
                            "else if (intensity > 0.25){"
                                "outColor = vec4(hairC, 1.0);"
                            "}"
                            "else if (intensity > 0.15){"
                                "outColor = vec4(hairD, 1.0);"
                            "}"
                            "else if (intensity > 0.09){"
                                "outColor = vec4(hairE, 1.0);"
                            "}"
                        "}"
                    "}\n";
    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertex_shader, fragment_shader, "outColor");
    program.bind();

    // The vertex shader wants the position of the vertices as an input.
    // The following line connects the VBO we defined above with the position "slot"
    program.bindVertexAttribArray("position", VBO);
    program.bindVertexAttribArray("normal", NBO);

    // Save the current time --- it will be used to dynamically change the triangle color
    auto t_start = std::chrono::high_resolution_clock::now();

    // Register the keyboard callback
    glfwSetKeyCallback(window, key_callback);

    // Register the mouse callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Update viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    GLfloat halfScreenWidth = 800.0f / 2;
    GLfloat halfScreenHeight = 600.0f / 2;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Get the size of the window
        int width, height;
        glfwGetWindowSize(window, &width, &height);

    //color calculations
        //base color

     
        VAO.bind();         
        Index.bind();
        program.bind();


    //body colors
        glUniform3f(program.uniform("BaseB"), BBX/255, BBY/255, BBZ/255);
        glUniform3f(program.uniform("BA"), B1X/255, B1Y/255, B1Z/255);
        glUniform3f(program.uniform("BB"), B2X/255, B2Y/255, B2Z/255);
        glUniform3f(program.uniform("BC"), B3X/255, B3Y/255, B3Z/255);
        glUniform3f(program.uniform("BD"), B4X/255, B4Y/255, B4Z/255);
        glUniform3f(program.uniform("BE"), B5X/255, B5Y/255, B5Z/255);
        glUniform3f(program.uniform("BDark"), BDX/255, BDY/255, BDZ/255);

        glUniform3f(program.uniform("BaseH"), HBX/255, HBY/255, HBZ/255);
        glUniform3f(program.uniform("HA"), H1X/255, H1Y/255, H1Z/255);
        glUniform3f(program.uniform("HB"), H2X/255, H2Y/255, H2Z/255);
        glUniform3f(program.uniform("HC"), H3X/255, H3Y/255, H3Z/255);
        glUniform3f(program.uniform("HD"), H4X/255, H4Y/255, H4Z/255);
        glUniform3f(program.uniform("HE"), H5X/255, H5Y/255, H5Z/255);

        glUniform1f(program.uniform("camX"), cameraPos.x);
        glUniform1f(program.uniform("camY"), cameraPos.y);
        glUniform1f(program.uniform("camZ"), cameraPos.z);


        glUniform1f(program.uniform("Ambient"), ambient);
        glUniform1f(program.uniform("specEx"), specularExponent);

        glUniformMatrix4fv(program.uniform("modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(program.uniform("viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(program.uniform("projMatrix"), 1, GL_FALSE, glm::value_ptr(projection));

        if(rotateLight == 1)
        {
            auto t_now = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
            lightPos = glm::vec3(sin(time/2), lightPos[1], sin(time/2));
            glUniform3fv(program.uniform("lightPos"), 1, glm::value_ptr(lightPos));
            
        }
        if (rotateLight == 2)
        {
            glm::vec3 lightPosition = glm::vec3(lightPos[0], lightPos[1], lightPos[2]);
            glUniform3fv(program.uniform("lightPos"), 1, glm::value_ptr(lightPosition));
        }
        if (rotateLight == 3)
        {
            glUniform3fv(program.uniform("lightPos"), 1, glm::value_ptr(glm::vec3(-1.0f, 2.0f, 3.0f)));
        }


        // Clear the framebuffer
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        
    // head
        glUniform1f(program.uniform("objectnum"), 1.0f);
        drawObject(program, VBO, NBO, Index, T);

    // neck
        glUniform1f(program.uniform("objectnum"), 1.0f);
        drawObject(program, VBO2, NBO2, Index2, T2);
    //right lid
        glUniform1f(program.uniform("objectnum"), 1.0f);
        drawObject(program, VBO5, NBO5, Index5, T5);
    //left lid
        glUniform1f(program.uniform("objectnum"), 1.0f);
        drawObject(program, VBO7, NBO7, Index7, T7);
    //hair
        glUniform1f(program.uniform("objectnum"), 2.0f);
        drawObject(program, VBO3, NBO3, Index3, T3);
    //right eye
        glUniform1f(program.uniform("objectnum"), 1.0f);
        drawObject(program, VBO4, NBO4, Index4, T4);
    //left eye
        glUniform1f(program.uniform("objectnum"), 1.0f);
        drawObject(program, VBO6, NBO6, Index6, T6);


        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    VAO.free();
    VBO.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
