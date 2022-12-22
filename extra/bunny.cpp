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

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3


std::__1::string frames[]= {
    "../data/7.off", "../data/6.off", "../data/5.off", 
    "../data/4.off", "../data/3.off", "../data/2.off", "../data/1.off", "../data/0.off" };

float FOV = 35.0f;
float distance= 5.0;

int frameNum = 2;

int select = 2;
bool hair = false; bool lids = false; bool body = false;

int hairSat; int lidSat; int skinSat;

//buffer object wrapper
BufferObject VBO; BufferObject NBO; BufferObject Index; //head
BufferObject VBO2; BufferObject NBO2; BufferObject Index2; //neck
BufferObject VBO3; BufferObject NBO3; BufferObject Index3; //hair
BufferObject VBO4; BufferObject NBO4; BufferObject Index4; //hair
BufferObject VBO5; BufferObject NBO5; BufferObject Index5; //hair
BufferObject VBO6; BufferObject NBO6; BufferObject Index6; //hair
BufferObject VBO7; BufferObject NBO7; BufferObject Index7; //hair
BufferObject modelO;

//vertex vector, normals vector, triangles vector * 3 vertexes
std::vector<glm::vec3> V(3); std::vector<glm::vec3> N(3); std::vector<glm::ivec3> T(3);//head
std::vector<glm::vec3> V2(3); std::vector<glm::vec3> N2(3); std::vector<glm::ivec3> T2(3);//neck
std::vector<glm::vec3> V3(3); std::vector<glm::vec3> N3(3); std::vector<glm::ivec3> T3(3);//hair?
std::vector<glm::vec3> V4(3); std::vector<glm::vec3> N4(3); std::vector<glm::ivec3> T4(3);//hair?
std::vector<glm::vec3> V5(3); std::vector<glm::vec3> N5(3); std::vector<glm::ivec3> T5(3);//hair?
std::vector<glm::vec3> V6(3); std::vector<glm::vec3> N6(3); std::vector<glm::ivec3> T6(3);//hair?
std::vector<glm::vec3> V7(3); std::vector<glm::vec3> N7(3); std::vector<glm::ivec3> T7(3);//hair?

//CAMERA
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
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
glm::mat4 model = glm::mat4(1.0f);


//Positions:
glm::vec3 modelpos5 = glm::vec3(0.0f, 0.0f, 0.0f); //right lid
glm::vec3 modelpos7 = glm::vec3(0.0f, 0.0f, 0.0f); //left lid

//color
float xskin = 242.0f; float yskin = 176.0f; float zskin = 138.0f;
int skinHue = 22; float skinS = 0.80f; float skinL = 0.75f;
float xhair = 111.0f; float yhair = 50.0f; float zhair = 9.0f;
int hairHue = 24; float hairS = 0.85f; float hairL = 0.25f;
float xlids = 255.0f; float ylids = 148.0f; float zlids = 132.0f;
int lidHue = 8; float lidS = 1.0f; float lidL = 0.75f;


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

bool loadOBJ( const char * path, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec2> & out_uvs, std::vector<glm::vec3> & out_normals){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	
	}
	fclose(file);
	return true;
}

GLuint loadDDS(const char * imagepath){

	unsigned char header[124];

	FILE *fp; 
 
	/* try to open the file */ 
	fp = fopen(imagepath, "rb"); 
	if (fp == NULL){
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); 
		return 0;
	}
   
	/* verify the type of file */ 
	char filecode[4]; 
	fread(filecode, 1, 4, fp); 
	if (strncmp(filecode, "DDS ", 4) != 0) { 
		fclose(fp); 
		return 0; 
	}
	
	/* get the surface desc */ 
	fread(&header, 124, 1, fp); 

	unsigned int height      = *(unsigned int*)&(header[8 ]);
	unsigned int width	     = *(unsigned int*)&(header[12]);
	unsigned int linearSize	 = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC      = *(unsigned int*)&(header[80]);

 
	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */ 
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize; 
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char)); 
	fread(buffer, 1, bufsize, fp); 
	/* close the file pointer */ 
	fclose(fp);

	unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
	unsigned int format;
	switch(fourCC) 
	{ 
	case FOURCC_DXT1: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
		break; 
	case FOURCC_DXT3: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
		break; 
	case FOURCC_DXT5: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
		break; 
	default: 
		free(buffer); 
		return 0; 
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
	
	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
	unsigned int offset = 0;

	/* load the mipmaps */ 
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
	{ 
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,  
			0, size, buffer + offset); 
	 
		offset += size; 
		width  /= 2; 
		height /= 2; 

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if(width < 1) width = 1;
		if(height < 1) height = 1;

	} 

	free(buffer); 

	return textureID;


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
    if (hairS == 1.0f){ int hairSat = -1;}; if (hairS == 0.0f){ int hairSat = 1;};
    if (lidS == 1.0f){ int lidSat = -1;}; if (lidS == 0.0f){ int lidSat = 1;};
    if (skinS == 1.0f){ int skinSat = -1;}; if (skinS == 0.0f){ int skinSat = 1;};
    // Update the position of the first vertex if the keys 1,2, or 3 are pressed
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_A:
            update_camera(glm::normalize((cameraPos)-0.5f*cameraRight));
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
        case GLFW_KEY_E:
            select = 3;
            break;
        case GLFW_KEY_2:
            if(select == 1){ //hair
                hairHue += 5;
            }
            if(select == 2){ //body
                skinHue += 5;
            }
            if(select == 3){ //lids
                lidHue += 5;
            }
            break;
        case GLFW_KEY_1:
            if(select == 1){ //hair
                hairHue -= 5;
            }
            if(select == 2){ //body
                skinHue -= 5;
            }
            if(select == 3){ //lids
                lidHue -= 5;
            }
            break;
        case GLFW_KEY_3:
            if(select == 1){ //hair
                if(hairS >0){hairS -= 0.01f;}
            }
            if(select == 2){ //body
                if(skinS >0){skinS -= 0.01f;}
            }
            if(select == 3){ //lids
                if(lidS >0){lidS -= 0.01f;}
            }
            break;
        case GLFW_KEY_4:
            if(select == 1){ //hair
                if(hairS < 1.0 ){hairS += 0.01f;}
            }
            if(select == 2){ //body
                if(skinS < 1.0 ){skinS += 0.01f;}
            }
            if(select == 3){ //lids
                if(lidS < 1.0 ){lidS += 0.01f;}
            }
            break;
        case GLFW_KEY_5:
            if(select == 1){ //hair
                if(hairL >0){hairL -= 0.01f;}
            }
            if(select == 2){ //body
                if(skinL >0){skinL -= 0.01f;}
            }
            if(select == 3){ //lids
                if(lidL >0){lidL -= 0.01f;}
            }
            break;
        case GLFW_KEY_6:
            if(select == 1){ //hair
                if(hairL < 1.0 ){hairL += 0.01f;}
            }
            if(select == 2){ //body
                if(skinL < 1.0 ){skinL += 0.01f;}
            }
            if(select == 3){ //lids
                if(lidL < 1.0 ){lidL += 0.01f;}
            }
            break;
        case GLFW_KEY_0:
            if(frameNum < 7){ //hair
                frameNum += 1;
            }
            break;
        case GLFW_KEY_9:
            if(frameNum > 0){ //hair
                frameNum -= 1;
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
    RGB skinColor = HSLToRGB(skinHue, skinS, skinL);
    xskin = float((int)skinColor.R); yskin = float((int)skinColor.G); zskin = float((int)skinColor.B);
    RGB lidColor = HSLToRGB(lidHue, lidS, lidL);
    xlids = float((int)lidColor.R); ylids = float((int)lidColor.G); zlids = float((int)lidColor.B);
    RGB hairColor = HSLToRGB(hairHue, hairS, hairL);
    xhair = float((int)hairColor.R); yhair = float((int)hairColor.G); zhair = float((int)hairColor.B);
    
    //right lid
    loadOFFFile(frames[frameNum], V5, T5, modelpos5);
    VBO5.update(V5);
    Index5.update(T5);
    calculateNorms(V5,N5,T5);
    NBO5.init();
    NBO5.update(N5);
//left lid
    loadOFFFile(frames[frameNum], V7, T7, modelpos7);
    VBO7.update(V7);
    Index7.update(T7);
    calculateNorms(V7,N7,T7);
    NBO7.init();
    NBO7.update(N7);
    
    
    VBO.update(V);
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
    window = glfwCreateWindow(800, 600, "Final", NULL, NULL);
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

    // Initialize the VAO
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
    loadOFFFile(frames[frameNum], V5, T5, modelpos5);
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
    loadOFFFile(frames[frameNum], V7, T7, modelpos7);
    VBO7.update(V7);
    Index7.update(T7);
    calculateNorms(V7,N7,T7);
    NBO7.init();
    NBO7.update(N7);


    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid
    Program program;
    // load fragment shader file
    const GLchar* vertex_shader =
            "#version 150 core\n"

                    "in vec3 position;"
                    "in vec3 normal;"

                    "uniform mat4 modelMatrix;"
                    "uniform mat4 viewMatrix;"
                    "uniform mat4 projMatrix;"

                    "uniform vec3 triangleColor;"
                    "uniform vec3 lightPos;"
                    "uniform vec3 lightParams;"
                    "uniform float camX;"
                    "uniform float camY;"
                    "uniform float camZ;"
                    "uniform float Ambient;"
                    "uniform float specEx;"
                    "uniform float objectnum;"


                    "out vec3 n;"
                    "out float object;"
                    "out vec3 color;"
                    "out vec3 pos;"
                    "void main()"
                    "{"
                        "n = mat3(transpose(inverse(modelMatrix))) * normal;"
                        "pos = vec3(modelMatrix * vec4(position, 1.0));"
                        "color = triangleColor;"
                        "object = objectnum;"
                        "gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);"
                    "}";
    const GLchar* fragment_shader =
            "#version 150 core\n"
                    "out vec4 outColor;"

                    "in vec3 n;"
                    "in vec3 color;"
                    "in vec3 pos;"
                    "in float object;"

                    "uniform vec3 triangleColor;"
                    "uniform vec3 lightPos;"
                    "uniform vec3 lightParams;"
                    "uniform float camX;"
                    "uniform float camY;"
                    "uniform float camZ;"
                    "uniform float Ambient;"
                    "uniform float specEx;"

                    "void main()"
                    "{"
                        "float roughness = 0.02;"
                        "vec3 camera = vec3(camX, camY, camZ);"
                        "vec3 col = color;"

                        "if (object == 1.0f){"
                            "vec3 normal = normalize(n);" 
                            "vec3 lightDir = normalize(lightPos - pos);" 
                            "vec3 amb = triangleColor * Ambient;" 
                            "vec3 diff = triangleColor * max(0.0, dot(normal, lightDir));" 
                            "vec3 spec = vec3(1.0) * pow(max(0.0, dot( normalize(camera - pos), normalize( reflect(-lightDir, normal)))), specEx);"
                            "col = clamp( (amb + diff + spec) , 0.0, 1.0);"
                            "outColor = vec4(col, 1.0);"
                        "}"

                        "if (object == 2.0f){"
                            "vec3 col = color;"
                            "vec3 normal = normalize(n);" 
                            "vec3 lightDir = normalize(lightPos - pos);" 
                            "vec3 eye_vector0 = normalize(camera - pos);"
                            "vec3 amb = triangleColor * Ambient;" 
                            "vec3 diff = triangleColor * max(0.0, dot(normal, lightDir));" 

                            "vec3 light_color = vec3(0.3,0.5,0.5);"
                            "float diffuse_intensity = 1.0;"
                            "float DiffuseFactor = dot(normal, -lightDir);"

                            "float normalDotLight = dot(normal, lightDir);" 
	                        "vec3 light_projected = normalize(lightDir - (normal * normalDotLight));"
                            
                            "float rough_sq = roughness * roughness;"

                            "float A = 1.0 - 0.5 * (rough_sq/(rough_sq + 0.57));" 
                            "float B = 0.45 * (rough_sq / (rough_sq + 0.09));" 

                            "float normalDotEye = dot(normal,eye_vector0);"
	                        "vec3 eye_projected = normalize(eye_vector0 - (normal * normalDotEye));"

                            "float angle_eye = acos(normalDotEye);"
                            "float angle_light = acos(normalDotLight);"

                            "float alpha = max(angle_eye,angle_light);"
	                        "float beta =  min(angle_eye,angle_light);"

                            "float CX = dot(light_projected,eye_projected);"
                            "float DX = sin(alpha) * tan(beta);"

                            "vec4 DiffuseColor = vec4(light_color * diffuse_intensity * DiffuseFactor, 1.0f);"

                            "float component = max(0.0, normalDotLight)*(A + (B * max(0.0,CX) * DX));"

                            "col = clamp( (amb + diff) , 0.0, 1.0);"
                            "outColor = vec4(col, 1.0);"
                        "}"

                    "}";
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

    //GLuint Texture = loadDDS("uvmap.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	//GLuint TextureID  = glGetUniformLocation(program, "myTextureSampler");

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Get the size of the window
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Bind your VAO (not necessary if you have only one)
        VAO.bind();        
    

        // Bind your program
        program.bind();



        glUniform3f(program.uniform("triangleColor"), (xskin/255), (yskin/255),(zskin/255));

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
        glUniform1f(program.uniform("objectnum"), 2.0f);
        drawObject(program, VBO, NBO, Index, T);

    // neck
        glUniform1f(program.uniform("objectnum"), 2.0f);
        drawObject(program, VBO2, NBO2, Index2, T2);
    //right lid
        glUniform3f(program.uniform("triangleColor"), (xlids/255), (ylids/255),(zlids/255));
        glUniform1f(program.uniform("objectnum"), 2.0f);
        drawObject(program, VBO5, NBO5, Index5, T5);
    //left lid
        glUniform1f(program.uniform("objectnum"), 2.0f);
        drawObject(program, VBO7, NBO7, Index7, T7);
    //hair
        glUniform3f(program.uniform("triangleColor"), (xhair/255), (yhair/255),(zhair/255));
        glUniform1f(program.uniform("objectnum"), 1.0f);
        drawObject(program, VBO3, NBO3, Index3, T3);
    //right eye
        glUniform3f(program.uniform("triangleColor"), 0.90, 0.89, 0.89);
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
