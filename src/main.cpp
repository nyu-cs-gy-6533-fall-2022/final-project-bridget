#include "Helpers.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <vector>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif
#include <chrono>
#include <glm/vec4.hpp>
#include <sstream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

using namespace std;

BufferObject VBO;
BufferObject VBO_Color;
BufferObject VBO_Center;
BufferObject VBO_Translations;
BufferObject VBO_Rotations;
BufferObject VBO_Scalings;
BufferObject VBO_Flat_Norm;
BufferObject VBO_Phong_Norm;

bool yes = false;
bool sphere = true;

std::vector<glm::vec4> V(1);
std::vector<glm::vec4> V_Line(1);
std::vector<glm::vec4> Color(1);
std::vector<glm::vec4> Line_Color(1);
std::vector<glm::vec3> Centres(1);
std::vector<glm::vec3> Line_Centres(1);
std::vector<glm::vec3> Translations(1);
std::vector<glm::vec3> Rotations(1);
std::vector<glm::vec3> Scalings(1);
std::vector<glm::vec3> Line_Translations(1);
std::vector<glm::vec3> Line_Rotations(1);
std::vector<glm::vec3> Line_Scalings(1);
std::vector<glm::vec3> Flat_Normals(1);
std::vector<glm::vec3> Phong_Normals(1);


std::vector<glm::vec3> Temp1(502);
std::vector<glm::vec3> Temp2(1000);


std::ifstream bunnyFile;
std::ifstream sphereFile;

std::vector<glm::vec3> Bunny_vert(1);
std::vector<glm::vec3> Bunny_Flat_store(1);
std::vector<glm::vec3> Bunny_Phong_store(1);

std::vector<glm::vec4> V_Bunny(1);
std::vector<glm::vec4> Color_bunny(1);
std::vector<glm::vec4> Lines_bunny(1);
std::vector<glm::vec4> Lines_color_bunny(1);
std::vector<glm::vec3> Bunny_Translations(1);
std::vector<glm::vec3> Bunny_Rotations(1);
std::vector<glm::vec3> Bunny_Scalings(1);
std::vector<glm::vec3> Bunny_Line_Translations(1);
std::vector<glm::vec3> Bunny_Line_Rotations(1);
std::vector<glm::vec3> Bunny_Line_Scalings(1);

std::vector<glm::vec3> Bunny_Centres(1);
std::vector<glm::vec3> Bunny_Line_Centres(1);
std::vector<glm::vec3> Bunny_Flat_Normals(1);
std::vector<glm::vec3> Bunny_Phong_Normals(1);

int state=0;
double View_Scale=1.0, X_shift=0.0, Y_shift=0.0;


glm::mat4 rotationVal(1.0);

int selection = 0, selection_Bumpy = -1, selection_Bunny = -1 ,selected_index = -1, V_select_start = -1, V_select_end = -1, Line_select_start = -1, Line_select_end = 1;
float timeGlobal;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void rotateup(glm::mat4 rotationVal){

}


std::vector<glm::vec3> Triangle_Normals(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{
    std::vector<glm::vec3> N(1);
    std::vector<glm::vec3> U2(1);
    std::vector<glm::vec3> V2(1);
    U2[0] = glm::vec3(x2-x1, y2-y1, z2-z1);
    V2[0] = glm::vec3(x3-x1, y3-y1, z3-z1);
    N[0] = glm::vec3(U2[0].y * V2[0].z - U2[0].z * V2[0].y
              , U2[0].z * V2[0].x - U2[0].x * V2[0].z
              , U2[0].x * V2[0].y - U2[0].y * V2[0].x);
    return(N);
}

bool Pt_in_Triangle(int vertex_ID, int Triangle_ID)
{
    if((V[vertex_ID].y == V[Triangle_ID].y) && (V[vertex_ID].z == V[Triangle_ID].z) && (V[vertex_ID].q == V[Triangle_ID].q))
    {
        return(true);
    }
    else if((V[vertex_ID].y == V[Triangle_ID+1].y) && (V[vertex_ID].z == V[Triangle_ID+1].z) && (V[vertex_ID].q == V[Triangle_ID+1].q))
    {
        return(true);
    }
    else if((V[vertex_ID].y == V[Triangle_ID+2].y) && (V[vertex_ID].z == V[Triangle_ID+2].z) && (V[vertex_ID].q == V[Triangle_ID+2].q))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

void Flat_Norms(int iteration, double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{
    std::vector<glm::vec3> N = Triangle_Normals(x1, y1, z1, x2, y2, z2, x3, y3, z3);
    int n = Flat_Normals.size();
    int i;

    if(n == 1) {
        Flat_Normals.resize(0);
        n = Flat_Normals.size();
    }

    for(i = n; i < n + iteration; i++)
    {
        Flat_Normals.resize(Flat_Normals.size()+1);
        Flat_Normals[i] = glm::vec3(N[0].x, N[0].y, N[0].z);
    }
}

void Phong_Norms()
{
    int i,j;
    int count = 0;
    std::vector<glm::vec3> N(1);
    std::vector<glm::vec3> Sum_norm(1);

    for(i = V_select_start; i < V_select_end; i+=1)
    {
        N = Triangle_Normals(V[i+0].y, V[i+0].z, V[i+0].q,
                             V[i+1].y, V[i+1].z, V[i+1].q,
                             V[i+2].y, V[i+2].z, V[i+2].q);
        Sum_norm[0] = glm::vec3(0.0, 0.0, 0.0);
        count = 0;
        for(j = V_select_start; j < V_select_end; j+=3)
        {
            if(Pt_in_Triangle(i,j))
            {
                N = Triangle_Normals(V[j+0].y, V[j+0].z, V[j+0].q,
                                     V[j+1].y, V[j+1].z, V[j+1].q,
                                     V[j+2].y, V[j+2].z, V[j+2].q);
                Sum_norm[0] = glm::vec3(Sum_norm[0].x + N[0].x, 
                                        Sum_norm[0].y + N[0].y, 
                                        Sum_norm[0].z + N[0].z);
                count += 1;
            }
        }

        Phong_Normals.resize(i+1);
        Phong_Normals[i] = glm::vec3((float)((float)Sum_norm[0].x/count), 
                                     (float)((float)Sum_norm[0].y/count), 
                                     (float)((float)Sum_norm[0].z/count));
    }    
}


void drawBunnyLines(double x1, double y1, double z1, double x2, double y2, double z2)
{
    int i = Lines_bunny.size();

    if(i == 1) {
        Lines_bunny.resize(0);
        Lines_color_bunny.resize(0);
        Bunny_Line_Rotations.resize(0);
        Bunny_Line_Translations.resize(0);
        Bunny_Line_Scalings.resize(0);
        i = Lines_bunny.size();
    }

    Lines_bunny.resize(i+2);
    Lines_color_bunny.resize(i+2);

    Lines_bunny[i]   = glm::vec4(i,x1,y1,z1);
    Lines_bunny[i+1] = glm::vec4(i+1,x2,y2,z2);

    Lines_color_bunny[i]   = glm::vec4(0.0, 0.0, 0.0, 1.0);
    Lines_color_bunny[i+1] = glm::vec4(0.0, 0.0, 0.0 ,1.0);

    Bunny_Line_Rotations.resize(i+2);
    Bunny_Line_Translations.resize(i+2);
    Bunny_Line_Scalings.resize(i+2);

    Bunny_Line_Rotations[i]   = glm::vec3(0.0, 0.0, 0.0);
    Bunny_Line_Rotations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    
    Bunny_Line_Translations[i]   = glm::vec3(0.0, -0.5, 0.0);
    Bunny_Line_Translations[i+1] = glm::vec3(0.0, -0.5, 0.0);
    
    Bunny_Line_Scalings[i]   = glm::vec3(1.0, 1.0, 1.0);
    Bunny_Line_Scalings[i+1] = glm::vec3(1.0, 1.0, 1.0);
}

void drawBunnyTriangle(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{
    int i = V_Bunny.size();

    if(i == 1) {
        V_Bunny.resize(0);
        Color_bunny.resize(0);
        Bunny_Rotations.resize(0);
        Bunny_Translations.resize(0);
        Bunny_Scalings.resize(0);
        i = V_Bunny.size();
    }

    V_Bunny.resize(i+3);
    Color_bunny.resize(i+3);

    V_Bunny[i]   = glm::vec4(i,x1,y1,z1);
    V_Bunny[i+1] = glm::vec4(i+1,x2,y2,z2);
    V_Bunny[i+2] = glm::vec4(i+2,x3,y3,z3);

    drawBunnyLines(x1, y1, z1, x2, y2, z2);
    drawBunnyLines(x2, y2, z2, x3, y3, z3);
    drawBunnyLines(x3, y3, z3, x1, y1, z1);

    Color_bunny[i+0] = glm::vec4(0.7, 0.7, 0.5, 0.8);
    Color_bunny[i+1] = glm::vec4(0.7, 0.7, 0.5, 0.8);
    Color_bunny[i+2] = glm::vec4(0.7, 0.7, 0.5, 0.8);

    Bunny_Rotations.resize(i+3);
    Bunny_Translations.resize(i+3);
    Bunny_Scalings.resize(i+3);

    Bunny_Rotations[i]   = glm::vec3(0.0, 0.0, 0.0);
    Bunny_Rotations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    Bunny_Rotations[i+2] = glm::vec3(0.0, 0.0, 0.0);
    
    Bunny_Translations[i]   = glm::vec3(0.0, -0.5, 0.0);
    Bunny_Translations[i+1] = glm::vec3(0.0, -0.5, 0.0);
    Bunny_Translations[i+2] = glm::vec3(0.0, -0.5, 0.0);
    
    Bunny_Scalings[i]   = glm::vec3(1.0, 1.0, 1.0);
    Bunny_Scalings[i+1] = glm::vec3(1.0, 1.0, 1.0);
    Bunny_Scalings[i+2] = glm::vec3(1.0, 1.0, 1.0);
}

void Next_sel()
{
    int i;

    if(selection != 0)
    {
        if(V.size()>0) {
            selected_index = 0;
        }
        else {
            selected_index = -1;
        }

        V_select_start = selected_index*36;
        V_select_end = (selected_index+1)*36;
        Line_select_start =  selected_index*72;
        Line_select_end = (selected_index+1)*72;
    }

    selection = 0;
    if(selection_Bunny > -1)
    {
        for(i = selection_Bunny*3000; i < (selection_Bunny+1)*3000; i++)
        {
            Color_bunny[i] = glm::vec4(0.5, 0.5, 0.0, Color_bunny[i].q);
        }
        for(i = selection_Bunny*6000; i < (selection_Bunny+1)*6000; i++)
        {
            Lines_color_bunny[i] = glm::vec4(0.0, 0.0, 0.0, Lines_color_bunny[i].q);
        }
    }
    if (V.size() == 0)
    {
        selected_index = -1;
        V_select_start = -1;
        V_select_end = -1;
        return;
    }
    else if((selected_index+1)*36 == V.size())
    {
        selected_index = 0;
    }
    else
    {
        selected_index += 1;
    }
    V_select_start = selected_index*36;
    V_select_end = (selected_index+1)*36;
    Line_select_start =  selected_index*72;
    Line_select_end = (selected_index+1)*72;
   
    if(selected_index > -1)
    {
        for(i = V_select_start; i < V_select_end; i++)
        {
            Color[i] = glm::vec4(0.0, 0.05, 0.05, Color[i].q);
        }
        if(Color[V_select_start].q < 0.1)
        {
            for(i = Line_select_start; i < Line_select_end; i++){
                Line_Color[i] = glm::vec4(1.0, 0.0, 0.0, Line_Color[i].q);
            }
        }
    }
}


void Next_selBunny()
{
    int i;
    if(selection != 1)
    {
        if(V_Bunny.size()>0){
            selection_Bunny = 0;
        }
        else{
            selection_Bunny = -1;
        }
        V_select_start = selection_Bunny*36;
        V_select_end = (selection_Bunny+1)*36;
        Line_select_start =  selection_Bunny*72;
        Line_select_end = (selection_Bunny+1)*72;
    }
    selection = 1;
    if(selection_Bunny > -1)
    {
        for(i = V_select_start; i < V_select_end; i++)
        {
            Color_bunny[i] = glm::vec4(1.0, 0.5, 0.0, Color_bunny[i].q);    ////////////
        }
        for(i = selection_Bunny*6000; i < (selection_Bunny+1)*6000; i++)
        {
            Lines_color_bunny[i] = glm::vec4(0.0, 0.0, 0.0, Lines_color_bunny[i].q);
        }
    }
    if(selected_index > -1)
    {
        for(i = selected_index*36; i < (selected_index+1)*36; i++)
        {
            Color[i] = glm::vec4(0.5, 0.0, 0.5, Color[i].q);
        }
        for(i = selected_index*72; i < (selected_index+1)*72; i++)
        {
            Line_Color[i] = glm::vec4(0.0, 0.0, 0.0, Line_Color[i].q);
        }
    }
    if (V_Bunny.size() == 0)
    {
        selected_index = -1;
        V_select_start = -1;
        V_select_end = -1;
        return;
    }
    else if((selection_Bunny+1)*3000 == V_Bunny.size())
    {
        selection_Bunny = 0;
    }
    else
    {
        selection_Bunny += 1;
    }
    V_select_start = selection_Bunny*3000;
    V_select_end = (selection_Bunny+1)*3000;
    Line_select_start =  selection_Bunny*6000;
    Line_select_end = (selection_Bunny+1)*6000;
   
    if(selection_Bunny > -1)
    {
        for(i = V_select_start; i < V_select_end; i++)
        {
            Color_bunny[i] = glm::vec4(0.0, 0.05, 0.05, Color_bunny[i].q);
        }
        if(Color_bunny[V_select_start].q < 0.1)
        {
            for(i = Line_select_start; i < Line_select_end; i++)
            {
                Lines_color_bunny[i] = glm::vec4(1.0, 0.0, 0.0, Lines_color_bunny[i].q);
            }
        }
    }
}

void Bunny_data(float x0, float Y0, float z0, float x1, float Y1, float z1, float x2, float Y2, float z2)
{
    int i = Bunny_vert.size();

    if(i == 1) {
        Bunny_vert.resize(0);
        i = Bunny_vert.size();
    }
    Bunny_vert.resize(i+3);
    Bunny_vert[i]   = glm::vec3(x0,Y0,z0);
    Bunny_vert[i+1] = glm::vec3(x1,Y1,z1);
    Bunny_vert[i+2] = glm::vec3(x2,Y2,z2);
}


void initializeBunny()
{
    Temp1.resize(0);
    Temp2.resize(0);
    Temp1.resize(502);
    Temp2.resize(1000);

    if (sphere = true){
        string name = "../data/sphere2.off";
    }
    else{
        
    }

    bunnyFile.open("../data/bunny.off");
    int i = 1;
    int j = 0;
    int count = 0;
    if(bunnyFile.is_open())
    {
        while(!bunnyFile.eof()){
            std::string line;
            getline(bunnyFile,line);
            std::istringstream ss(line);
            if(i != 1 && i != 2 && i<=504)
            {
                j = 0;
                do
                {
                    std::string num;
                    ss >> num;
                    Temp1[i-3][j] = stof(num);
                    j++;
                }while(ss && j!=3);
            }
            if(i != 1 && i != 2 && i>504 && i<=1504)
            {
                int j=0;
                do
                {
                    std::string num;
                    ss>>num;
                    if(j>0){
                        Temp2[i-505][j-1] = stoi(num);
                    }
                    j++;
                }
                while(ss && j!=4);
            }
            i++;
        }
    }
    bunnyFile.close();
    Bunny_vert.resize(0);
    
    for(j=0; j<Temp2.size(); j++)
    {
        Bunny_data(Temp1[Temp2[j][0]][0], Temp1[Temp2[j][0]][1], Temp1[Temp2[j][0]][2],
                   Temp1[Temp2[j][1]][0], Temp1[Temp2[j][1]][1], Temp1[Temp2[j][1]][2],
                   Temp1[Temp2[j][2]][0], Temp1[Temp2[j][2]][1], Temp1[Temp2[j][2]][2]);
    }

    std::vector<glm::vec3> N(1);
    int n;
    
    if(Bunny_Flat_store.size() == 0) {
        Bunny_Flat_store.resize(0);
    }

    Bunny_Flat_store.resize(Bunny_vert.size());
    for(i=0; i < Bunny_vert.size(); i+=3)
    {
        N = Triangle_Normals(Bunny_vert[i].x,   Bunny_vert[i].y,   Bunny_vert[i].z,
                                                    Bunny_vert[i+1].x, Bunny_vert[i+1].y, Bunny_vert[i+1].z,
                                                    Bunny_vert[i+2].x, Bunny_vert[i+2].y, Bunny_vert[i+2].z);

        Bunny_Flat_store[i]   = glm::vec3(N[0].x, N[0].y, N[0].z);
        Bunny_Flat_store[i+1] = glm::vec3(N[0].x, N[0].y, N[0].z);
        Bunny_Flat_store[i+2] = glm::vec3(N[0].x, N[0].y, N[0].z);
    }

    if(Bunny_Phong_store.size() == 0) {
        Bunny_Phong_store.resize(0);
    }

    Bunny_Phong_store.resize(Bunny_vert.size());
    
    std::vector<glm::vec3> Sum_norm(1);
    for(i = 0; i < Bunny_vert.size(); i+=1)
    {
        N = Triangle_Normals(Bunny_vert[i].x,   Bunny_vert[i].y,   Bunny_vert[i].z,
                             Bunny_vert[i+1].x, Bunny_vert[i+1].y, Bunny_vert[i+1].z,
                             Bunny_vert[i+2].x, Bunny_vert[i+2].y, Bunny_vert[i+2].z);
        Sum_norm[0] = glm::vec3(0.0, 0.0, 0.0);
        count = 0;
        for(j = 0; j < Bunny_vert.size(); j+=3)
        {
            if(((Bunny_vert[i].x == Bunny_vert[j].x)   && (Bunny_vert[i].y == Bunny_vert[j].y)   && (Bunny_vert[i].z == Bunny_vert[j].z)) ||
               ((Bunny_vert[i].x == Bunny_vert[j+1].x) && (Bunny_vert[i].y == Bunny_vert[j+1].y) && (Bunny_vert[i].z == Bunny_vert[j+1].z)) ||
               ((Bunny_vert[i].x == Bunny_vert[j+2].x) && (Bunny_vert[i].y == Bunny_vert[j+2].y) && (Bunny_vert[i].z == Bunny_vert[j+2].z)))
            {
                N = Triangle_Normals(Bunny_vert[j].x,   Bunny_vert[j].y,   Bunny_vert[j].z,
                                     Bunny_vert[j+1].x, Bunny_vert[j+1].y, Bunny_vert[j+1].z,
                                     Bunny_vert[j+2].x, Bunny_vert[j+2].y, Bunny_vert[j+2].z);
                Sum_norm[0] = glm::vec3(Sum_norm[0].x + N[0].x, Sum_norm[0].y + N[0].y, Sum_norm[0].z + N[0].z);
                count += 1;
            }
        }
        Bunny_Phong_store[i] =  glm::vec3((float)((float)Sum_norm[0].x/count), (float)((float)Sum_norm[0].y/count), (float)((float)Sum_norm[0].z/count));
    }
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    double xworld = ((((xpos/double(width))*2)-1)-X_shift)/View_Scale;
    double yworld = (((((height-1-ypos)/double(height))*2)-1)-Y_shift)/View_Scale; 
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {}
    VBO.update(V);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    int i, iLines, j;
    double xVals, yVals, zVals, alphaVal;
    
    if(action == GLFW_PRESS)
    {
        switch (key)
        {       
            case GLFW_KEY_3:
                for(i = 0; i < Bunny_vert.size(); i+=3)
                {
                    drawBunnyTriangle(Bunny_vert[i].x, Bunny_vert[i].y, Bunny_vert[i].z,
                                      Bunny_vert[i+1].x, Bunny_vert[i+1].y, Bunny_vert[i+1].z,
                                      Bunny_vert[i+2].x, Bunny_vert[i+2].y, Bunny_vert[i+2].z);

                    j = Bunny_Centres.size();
                    
                    if(j == 1) {
                        Bunny_Centres.resize(0);
                        Bunny_Flat_Normals.resize(0);
                        Bunny_Phong_Normals.resize(0);
                        Bunny_Line_Centres.resize(0);
                        j = Bunny_Centres.size();
                    }
                    
                    Bunny_Centres.resize(j+3);
                    Bunny_Flat_Normals.resize(j+3);
                    Bunny_Phong_Normals.resize(j+3);
                    Bunny_Line_Centres.resize(j+3);

                    Bunny_Centres[j]   = glm::vec3(0.0, 0.0, 0.0);
                    Bunny_Centres[j+1] = glm::vec3(0.0, 0.0, 0.0);
                    Bunny_Centres[j+2] = glm::vec3(0.0, 0.0, 0.0);

                    Bunny_Line_Centres[j]   = glm::vec3(0.0, 0.0, 0.0);
                    Bunny_Line_Centres[j+1] = glm::vec3(0.0, 0.0, 0.0);
                    Bunny_Line_Centres[j+2] = glm::vec3(0.0, 0.0, 0.0);

                    Bunny_Flat_Normals[j]   = glm::vec3(Bunny_Flat_store[i].x, Bunny_Flat_store[i].y, Bunny_Flat_store[i].z);
                    Bunny_Flat_Normals[j+1] = glm::vec3(Bunny_Flat_store[i+1].x, Bunny_Flat_store[i+1].y, Bunny_Flat_store[i+1].z);
                    Bunny_Flat_Normals[j+2] = glm::vec3(Bunny_Flat_store[i+2].x, Bunny_Flat_store[i+2].y, Bunny_Flat_store[i+2].z);

                    Bunny_Phong_Normals[j]   = glm::vec3(Bunny_Phong_store[i].x, Bunny_Phong_store[i].y, Bunny_Phong_store[i].z);
                    Bunny_Phong_Normals[j+1] = glm::vec3(Bunny_Phong_store[i+1].x, Bunny_Phong_store[i+1].y, Bunny_Phong_store[i+1].z);
                    Bunny_Phong_Normals[j+2] = glm::vec3(Bunny_Phong_store[i+2].x, Bunny_Phong_store[i+2].y, Bunny_Phong_store[i+2].z);
                }
                break;

            case GLFW_KEY_P:
                if(selection == 1)
                {
                    if(selection_Bunny > -1)
                    {
                        for(i = V_select_start; i < V_select_end; i++)
                        {
                            Color_bunny[i] = glm::vec4(Color_bunny[i].x, Color_bunny[i].y, Color_bunny[i].z, 0.8);
                        }
                        for(i = Line_select_start; i < Line_select_end; i++)
                        {
                            Lines_color_bunny[i] = glm::vec4(0.0, 0.0, 0.0, 1.0);
                        }
                    }
                }
                break;
            
            case GLFW_KEY_O:
                yes = true;
                if(selection == 1)
                {
                    if(selection_Bunny > -1)
                    {
                        for(i = V_select_start; i < V_select_end; i++)
                        {
                            Color_bunny[i] = glm::vec4(Color_bunny[i].x, Color_bunny[i].y, Color_bunny[i].z, 0.0);
                        }
                        for(i = Line_select_start; i < Line_select_end; i++)
                        {
                            Lines_color_bunny[i] = glm::vec4(0.0, 0.0, 0.0, 1.0);
                        }
                    }
                }
                break;

            case GLFW_KEY_I:
                yes = true;
                if(selection == 1)
                {
                    if(selection_Bunny > -1)
                    {
                        for(i = V_select_start; i < V_select_end; i++)
                        {
                            Color_bunny[i] = glm::vec4(Color_bunny[i].x, Color_bunny[i].y, Color_bunny[i].z, 0.5);
                        }
                        for(i = Line_select_start; i < Line_select_end; i++)
                        {
                            Lines_color_bunny[i] = glm::vec4(0.0, 0.0, 0.0, 0.0);
                        }
                    }
                }
                break;

            case GLFW_KEY_W:
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Rotations[i] = glm::vec3(Bunny_Rotations[i].x, Bunny_Rotations[i].y + 0.2, Bunny_Rotations[i].z);  
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Rotations[i] = glm::vec3(Bunny_Line_Rotations[i].x, Bunny_Line_Rotations[i].y + 0.2, Bunny_Line_Rotations[i].z);
                    }
                }
                break;
            case GLFW_KEY_S:
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Rotations[i] = glm::vec3(Bunny_Rotations[i].x, Bunny_Rotations[i].y - 0.2, Bunny_Rotations[i].z);  
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Rotations[i] = glm::vec3(Bunny_Line_Rotations[i].x, Bunny_Line_Rotations[i].y + 0.2, Bunny_Line_Rotations[i].z);
                    }
                }
                break;
            case GLFW_KEY_D:
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Rotations[i] = glm::vec3(Bunny_Rotations[i].x, Bunny_Rotations[i].y, Bunny_Rotations[i].z + 0.2);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Rotations[i] = glm::vec3(Bunny_Line_Rotations[i].x, Bunny_Line_Rotations[i].y, Bunny_Line_Rotations[i].z + 0.2);
                    }
                }
                break;
            case GLFW_KEY_A:
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Rotations[i] = glm::vec3(Bunny_Rotations[i].x, Bunny_Rotations[i].y, Bunny_Rotations[i].z - 0.2);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Rotations[i] = glm::vec3(Bunny_Line_Rotations[i].x, Bunny_Line_Rotations[i].y, Bunny_Line_Rotations[i].z + 0.2);
                    }
                }
                break;          
            case GLFW_KEY_UP:
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Scalings[i] = glm::vec3(Bunny_Scalings[i].x*1.2, Bunny_Scalings[i].y*1.2, Bunny_Scalings[i].z*1.2);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Scalings[i] = glm::vec3(Bunny_Line_Scalings[i].x*1.2, Bunny_Line_Scalings[i].y*1.2, Bunny_Line_Scalings[i].z*1.2);
                    }
                }
                break;

            case GLFW_KEY_DOWN:
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Scalings[i] = glm::vec3(Bunny_Scalings[i].x*0.8, Bunny_Scalings[i].y*0.8, Bunny_Scalings[i].z*0.8);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Scalings[i] = glm::vec3(Bunny_Line_Scalings[i].x*0.8, Bunny_Line_Scalings[i].y*0.8, Bunny_Line_Scalings[i].z*0.8);
                    }
                }
                break;
            case GLFW_KEY_Z:
                Next_selBunny();
                break;

            default:
               break;
        }
    }
    VBO.update(V);
    VBO_Color.update(Color);
    VBO_Center.update(Centres);
    VBO_Translations.update(Translations);
    VBO_Rotations.update(Rotations);
    VBO_Scalings.update(Scalings);
    VBO_Flat_Norm.update(Flat_Normals);
    VBO_Phong_Norm.update(Phong_Normals);

    VBO.update(V_Line);
    VBO_Color.update(Line_Color);
    VBO_Center.update(Line_Centres);
    VBO_Translations.update(Line_Translations);
    VBO_Rotations.update(Line_Rotations);
    VBO_Scalings.update(Line_Scalings);
}

int main(void)
{
    GLFWwindow* window;
  if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_SAMPLES, 8);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(800, 600, "Hello OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    #ifndef __APPLE__
      glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
       fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    #endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    std::cout << "OpenGL version recieved: " << major << "." << minor << "." << rev << std::endl;
    std::cout << "Supported OpenGL is " << (const char*)glGetString(GL_VERSION) << std::endl;
    std::cout << "Supported GLSL is " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    VBO.init();
    VBO_Color.init();
    VBO_Center.init();
    VBO_Translations.init();
    VBO_Rotations.init();
    VBO_Scalings.init();
    VBO_Flat_Norm.init();
    VBO_Phong_Norm.init();

    VBO.update(V);
    VBO_Color.update(Color);
    VBO_Center.update(Centres);
    VBO_Translations.update(Translations);
    VBO_Rotations.update(Rotations);
    VBO_Scalings.update(Scalings);
    VBO_Flat_Norm.update(Flat_Normals);
    VBO_Phong_Norm.update(Phong_Normals);

    Program program;

    const GLchar* vertex_shader =
            "#version 150 core\n"
                    "in vec4 position;"
                    "in vec4 color;"
                    "in vec3 centroids;"
                    "in vec3 Rotate_val;"
                    "in vec3 Translate_val;"
                    "in vec3 Scale_val;"
                    "in vec3 FlatNormals;"
                    "in vec3 SmoothNormals;"
                    "uniform mat4 projection;"
                    "uniform vec3 vPosition;"
                    "uniform mat4 view;"
                    "out vec4 colorOut;"

                    "mat4 positionMatrix = mat4("
                        "position[1], position[2], position[3], 1.0,"
                        "FlatNormals[0], FlatNormals[1], FlatNormals[2], 0.0,"
                        "SmoothNormals[0], SmoothNormals[1], SmoothNormals[2], 0.0,"
                        "0.0, 0.0, 0.0, 8.0"
                    ");"

                    "mat4 translation1 = mat4("
                    "-1.0, 0.0, 0.0, 0.0,"
                    "0.0, 1.0, 0.0, 0.0,"
                    "0.0, 0.0, 1.0, 0.0,"
                    "-centroids[0], -centroids[1], -centroids[2], 1.0"
                    ");"

                    "mat4 rotationXY = mat4("
                    "cos(Rotate_val[0]), -sin(Rotate_val[0]), 0.0, 0.0,"
                    "sin(Rotate_val[0]), cos(Rotate_val[0]), 0.0, 0.0,"
                    "0.0, 0.0, 1.0, 0.0,"
                    "0.0, 0.0, 0.0, 1.0"
                    ");"
                    "mat4 rotationYZ = mat4("
                    "1.0, 0.0, 0.0, 0.0,"  
                    "0.0, cos(Rotate_val[1]), -sin(Rotate_val[1]), 0.0,"
                    "0.0, sin(Rotate_val[1]), cos(Rotate_val[1]), 0.0,"      
                    "0.0, 0.0, 0.0, 1.0"                                                            
                    ");"

                    "mat4 rotationXZ = mat4("
                    "cos(Rotate_val[2]), 0.0, sin(Rotate_val[2]) ,0.0,"
                    "0.0, 1.0, 0.0, 0.0,"
                    "-sin(Rotate_val[2]), 0.0 ,cos(Rotate_val[2]), 0.0,"
                    "0.0, 0.0, 0.0, 1.0"
                    ");"

                    "mat4 scaling = mat4("
                    "Scale_val[0], 0.0, 0.0, 0.0,"
                    "0.0, Scale_val[1], 0.0, 0.0,"
                    "0.0, 0.0, Scale_val[2], 0.0,"
                    "0.0, 0.0, 0.0, 1.0"
                    ");"
                   
                    "mat4 translation2 = mat4("
                    "-6.0, 0.0, 0.0, 0.0,"
                    "0.0, 6.0, 0.0, 0.0,"
                    "0.0, 0.0, -6.0, 0.0,"
                    "centroids[0]+Translate_val[0], centroids[1]+Translate_val[1], centroids[2]+Translate_val[2], 1.0"
                    ");"

                    "mat4 resultMatrix = mat4(translation2 * scaling * rotationXZ * rotationYZ  * rotationXY * translation1 * positionMatrix);"
                   
                    "vec3 lightSource = vec3(1.0,-2.0,3.0);"
                    "float amb = 0.1;"
                    "vec4 colorShaded = vec4(0.0, 0.0, 0.0, 0.0);"

                    "void main()"
                    "{"
                    "   if(color[3] > 0.9){" 
                    "       colorShaded = vec4(color[0], color[1], color[2], 1.0);"
                    "   }"
                    "   else if(color[3] > 0.75){" 
                    "       vec3 iVec = vec3(normalize(vec3(resultMatrix[0][0], resultMatrix[0][1], resultMatrix[0][2]) - lightSource));"
                    "       vec3 nVec = vec3(normalize(vec3(resultMatrix[1][0], resultMatrix[1][1], resultMatrix[1][2])));"
                    "       float x = dot(iVec,nVec);"
                    "       colorShaded = vec4(amb + (color[0] * x) , amb + (color[1] * x), amb + (color[2] * x), 1.0);"
                    "   }"
                    "   else if(color[3] > 0.4){" 
                    "       vec3 iVec = vec3(normalize(vec3(resultMatrix[0][0], resultMatrix[0][1], resultMatrix[0][2]) - lightSource));"
                    "       vec3 nVec = vec3(normalize(vec3(resultMatrix[2][0], resultMatrix[2][1], resultMatrix[2][2])));"
                    "       float x = dot(iVec,nVec);"
                    "       colorShaded = vec4(amb + (color[0] * x) , amb + (color[1] * x), amb + (color[2] * x), 1.0);"
                    "   }"
                    "   else {" 
                    "       colorShaded = vec4(0.0, 0.0, 0.0, 0.0);"
                    "   }"
                    "   colorOut = colorShaded;"
                    "   gl_Position = vec4(resultMatrix[0][0], resultMatrix[0][1], resultMatrix[0][2], 1.0);"
                    "}";

    const GLchar* fragment_shader =
            "#version 150 core\n"
                    "in vec4 colorOut;"
                    "out vec4 outColor;"
                    "void main()"
                    "{"
                    "    if(colorOut.a < 0.1){"
                    "       discard;"
                    "    }"

                    "    outColor = colorOut;"
                    "}";

    program.init(vertex_shader,fragment_shader,"outColor");
    program.bind();
    program.bindVertexAttribArray("position",VBO);
    program.bindVertexAttribArray("color",VBO_Color);
    program.bindVertexAttribArray("centroids",VBO_Center);
    program.bindVertexAttribArray("Translate_val",VBO_Translations);
    program.bindVertexAttribArray("Rotate_val",VBO_Rotations);
    program.bindVertexAttribArray("Scale_val",VBO_Scalings);
    program.bindVertexAttribArray("FlatNormals",VBO_Flat_Norm);
    program.bindVertexAttribArray("SmoothNormals",VBO_Phong_Norm);

    auto t_start = std::chrono::high_resolution_clock::now();
    glfwSetKeyCallback(window, key_callback);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(5);

    initializeBunny();

    glm::mat4 view;

    while (!glfwWindowShouldClose(window))
    {
        auto t_now = std::chrono::high_resolution_clock::now();
        timeGlobal = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        int width, height;

        if(state!=0){
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            glfwGetWindowSize(window, &width, &height);
            double xworld = ((((xpos/double(width))*2)-1)-X_shift)/View_Scale;
            double yworld = (((((height-1-ypos)/double(height))*2)-1)-Y_shift)/View_Scale; // NOTE: y axis is flipped in glfw
            
        }
        
    
        VAO.bind();

        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);
        program.bind();

        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


       
        VBO.update(V_Bunny);
        VBO_Color.update(Color_bunny);
        VBO_Translations.update(Bunny_Translations);
        VBO_Rotations.update(Bunny_Rotations);
        VBO_Scalings.update(Bunny_Scalings);
        VBO_Center.update(Bunny_Centres);
        VBO_Flat_Norm.update(Bunny_Flat_Normals);
        VBO_Phong_Norm.update(Bunny_Phong_Normals);
        glDrawArrays(GL_TRIANGLES, 0, V_Bunny.size());

        if(yes){
            VBO.update(Lines_bunny);
            VBO_Color.update(Lines_color_bunny);
            VBO_Center.update(Bunny_Line_Centres);
            VBO_Translations.update(Bunny_Line_Translations);
            VBO_Rotations.update(Bunny_Line_Rotations);
            VBO_Scalings.update(Bunny_Line_Scalings);
            VBO_Flat_Norm.update(Bunny_Line_Centres); 
            VBO_Phong_Norm.update(Bunny_Line_Centres); 
            glDrawArrays(GL_LINES, 0, Lines_bunny.size());
        }

 
        glfwSwapBuffers(window);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    program.free();
    VAO.free();
    VBO.free();
    VBO_Color.free();
    VBO_Center.free();
    VBO_Translations.free();
    VBO_Rotations.free();
    VBO_Scalings.free();
    VBO_Flat_Norm.free();
    VBO_Phong_Norm.free();

    glfwTerminate();
    return 0;
}