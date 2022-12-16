//#include<windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
//#include<bits/stdc++.h>
//#include <windows.h>
#include <math.h>
#include <string>
#include <sstream>
#include <iostream>
//#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define pi 3.141529
// const int font = (int)GLUT_BITMAP_9_BY_15;
const int font = (int)GLUT_BITMAP_9_BY_15;
double cubeLen = 35, sphereRad = 7;
double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
int debug;
int mapFlag = 0;
double playerRad = 0;
int playerRadFlag = 0;
int start = clock();
int sec, mn = 0, hour = 0;
int gameOver = 0;
std::string strSec, strMn, strHour, cout;

struct ImageData {
    int width;
    int height;
    int nrChannels;
    GLuint texture;
    unsigned char* data;
};

ImageData wallImageData;

struct Moon
{

    double x, y, z, rad;
    Moon(double a, double b, double c, double r)
    {
        x = a;
        y = b;
        z = c;
        rad = r;
    }
};
struct point
{
    double x, y, z;
};
class vec
{

public:
    double x, y, z;
    vec(double a, double b, double c)
    {
        x = a;
        y = b;
        z = c;
    }
    vec()
    {
        x = 0;
        y = 0;
        z = 0;
    }


};
class Destination
{

public:

    vec a;
    vec b;
    Destination(double ax, double ay, double bx, double by)
    {

        a.x = ax;
        a.y = ay;
        a.z = 0;
        b.x = bx;
        b.y = by;
        b.z = 0;
    }

};
vec pos(450, 450, 25);
vec l(-sqrt(0.5), -sqrt(0.5), 0);
vec r(-sqrt(0.5), sqrt(0.5), 0);
vec u(0, 0, 1);
vec temp_pos(1000, 1000, 1000);
vec temp_l(0, 0, 0);
Moon moon(-200, -200, 200, 20);
Destination destination(-250, -250, -300, -250);

GLuint g_textureID = 1;
int width, height;
int channel;
const char* imagefiles[5] = { "wall.png", "roof.png", "ball.png", "ball1.jpg", "face.jpg"};//가져올 이미지
GLuint texID[5];

unsigned char* LoadMeshFromFile(const char* texFile) //이미지 정보 읽어오는 함수
{
    GLuint texture;
    glGenTextures(1, &texture);
    FILE* fp = NULL;
    if (fopen_s(&fp, texFile, "rb")) {
        printf("ERROR : No %s. \n fail to bind %d\n", texFile, texture);
        return (unsigned char*)false;
    }

    unsigned char* image = stbi_load_from_file(fp, &width, &height, &channel, 4); //이미지 너비, 높이, 컬러채널의 수
    fclose(fp);
    return image;
}



void init()
{

    // LOAD TEXTURES

    /*wallImageData.data = stbi_load("wall.jpg", &(wallImageData.width), &(wallImageData.height), &(wallImageData.nrChannels), 0);
    glGenTextures(1, &(wallImageData.texture));*/

    glEnable(GL_TEXTURE_2D);
    glGenTextures(5, texID); //(텍스처 개수, 텍스처 저장공간)

    for (int i = 0; i < 5; i++)
    {
        unsigned char* bitmap;
        bitmap = LoadMeshFromFile((char*)imagefiles[i]);
        glBindTexture(GL_TEXTURE_2D, texID[i]);
        //gluBuild2DMipmaps(GL_TEXTURE_2D, channel, width, height, 4, GL_UNSIGNED_BYTE, bitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //2D 텍스처, 확대될 때 LINEAR필터적용(픽셀이 덜 보여 매끄러운 결과 산출)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //2D 텍스처, 축소될 때 LINEAR필터적용(픽셀이 덜 보여 매끄러운 결과 산출)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
        free(bitmap);
    }


    //codes for initialization
    drawgrid = 1;
    drawaxes = 1;
    cameraHeight = 150.0;
    cameraAngle = 1.0;
    angle = 0;
    debug = 0;
    //clear the screen
    glClearColor(0, 0, 0, 0);

    /************************
    / set-up projection here
    ************************/
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    //initialize the matrix
    glLoadIdentity();

    //give PERSPECTIVE parameters
    gluPerspective(80, 1, 1, 1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance
}

void drawAxes()
{
    if (drawaxes == 1)
    {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        {
            glVertex3f(100, 0, 0);
            glVertex3f(-100, 0, 0);

            glVertex3f(0, -100, 0);
            glVertex3f(0, 100, 0);

            glVertex3f(0, 0, 100);
            glVertex3f(0, 0, -100);
        }
        glEnd();
    }
}


void drawGrid()
{
    int i;
    if (drawgrid == 1)
    {
        glColor3f(0.6, 0.6, 0.6);	//grey
        glBegin(GL_LINES);
        {
            for (i = -8; i <= 8; i++)
            {

                if (i == 0)
                    continue;	//SKIP the MAIN axes

                //lines parallel to Y-axis
                glVertex3f(i * 10, -90, 0);
                glVertex3f(i * 10, 90, 0);

                //lines parallel to X-axis
                glVertex3f(-90, i * 10, 0);
                glVertex3f(90, i * 10, 0);
            }
        }
        glEnd();
    }
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
    glBegin(GL_QUADS);
    {
        glVertex3f(a, a, 0);
        glVertex3f(a, -a, 0);
        glVertex3f(-a, -a, 0);
        glVertex3f(-a, a, 0);
    }
    glEnd();
}



void drawSphere(double radius, int slices, int stacks)
{
    struct point points[100][100];
    int i, j;
    double h, r;
    //generate points
    for (i = 0; i <= stacks; i++)
    {
        h = radius * sin(((double)i / (double)stacks) * (pi / 2));
        r = radius * cos(((double)i / (double)stacks) * (pi / 2));
        for (j = 0; j <= slices; j++)
        {
            points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
            points[i][j].y = r * sin(((double)j / (double)slices) * 2 * pi);
            points[i][j].z = h;
        }
    }
    //draw quads using generated points
    for (i = 0; i < stacks; i++)
    {
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        for (j = 0; j < slices; j++)
        {

            glBegin(GL_QUADS);
            {
                //upper hemisphere
                
                glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                
                glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                
                glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                
                glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
                //lower hemisphere
                
                glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
                
                glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
                
                glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
                
                glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
            }
            glEnd();
        }
    }
}

void drawWallGeneric(double ax, double ay, double bx, double by, double height, double width, float sz, float sx)
{

    glEnable(GL_TEXTURE_2D);

    /*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, wallImageData.data);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glColor3f(1, 1, 1);*/
    //glBindTexture(GL_TEXTURE_2D, wallImageData.texture);

    glColor3f(1, 1, 1);

    //glBindTexture(GL_TEXTURE_2D, texID[0]);


    double dis = sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
    double x1, y1, x2, y2;
    x1 = ax + width * (by - ay) / dis;
    y1 = ay + width * (ax - bx) / dis;
    x2 = bx + width * (by - ay) / dis;
    y2 = by + width * (ax - bx) / dis;
    


    glPushMatrix();
    {
        glBegin(GL_POLYGON);
        {

            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //
            //   범진님 여길 봐요!!!!!!!!!!!!!!!!!!!1
            //
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            

            /*glTexCoord3f(0, 0, 0);
            glVertex3f(ax, ay, 0);
            glTexCoord3f(height/20.0, 0, 0);
            glVertex3f(bx, by, 0);
            glTexCoord3f(height / 20.0, width / 20.0, 0);
            glVertex3f(bx, by, height);
            glTexCoord3f(0, width / 20.0, 0);
            glVertex3f(ax, ay, height);*/

            glTexCoord3f(0, 0, 0);
            glVertex3f(ax, ay, 0);//좌하단
            glTexCoord3f(sz, 0, 0);
            glVertex3f(bx, by, 0);//우하단
            glTexCoord3f(sz, sx, 0);
            glVertex3f(bx, by, height);//우상단
            glTexCoord3f(0, sx, 0);
            glVertex3f(ax, ay, height);//좌상단

            /*glNormal3f(0.0, 1.0, 0.0);
            glTexCoord2d(1.0, 1.0);
            glVertex3f(21.0f, 0.0f, -21.0f);
            glTexCoord2d(0.0, 1.0);
            glVertex3f(-21.0f, 0.0f, -21.0f);
            glTexCoord2d(0.0, 0.0);
            glVertex3f(-21.0f, 0.0f, 21.0f);
            glTexCoord2d(1.0, 0.0);
            glVertex3f(21.0f, 0.0f, 21.0f);*/
        }
        glEnd();
    }
    glPopMatrix();

    glPushMatrix();
    {
        //glTranslatef(width,0,0);
        glBegin(GL_QUADS);
        {
            glTexCoord3f(0, 0, 0);
            glVertex3f(x1, y1, 0);
            glTexCoord3f(sz, 0, 0);
            glVertex3f(x2, y2, 0);
            glTexCoord3f(sz, sx, 0);
            glVertex3f(x2, y2, height);
            glTexCoord3f(0, sx, 0);
            glVertex3f(x1, y1, height);
        }
        glEnd();
    }
    glPopMatrix();


    //킹바갓닥
    //a.k.a 윗면
    glPushMatrix();
    {
        glTranslatef(0, 0, height);
        glBegin(GL_QUADS);
        {
            glTexCoord3f(0, 0, 0);
            glVertex3f(ax, ay, 0);
            glTexCoord3f(sz, 0, 0);
            glVertex3f(bx, by, 0);
            glTexCoord3f(sz, sx, 0);
            glVertex3f(x2, y2, 0);
            glTexCoord3f(0, sx, 0);
            glVertex3f(x1, y1, 0);
        }
        glEnd();
    }
    glPopMatrix();


    glPushMatrix();
    {

        glBegin(GL_QUADS);
        {
            glTexCoord3f(0, 0, 0);
            glVertex3f(ax, ay, 0);
            glTexCoord3f(sz, 0, 0);
            glVertex3f(bx, by, 0);
            glTexCoord3f(sz, sx, 0);
            glVertex3f(x2, y2, 0);
            glTexCoord3f(0, sx, 0);
            glVertex3f(x1, y1, 0);
        }
        glEnd();
    }
    glPopMatrix();

    glPushMatrix();
    {

        glBegin(GL_QUADS);
        {
            glTexCoord3f(0, 0, 0);
            glVertex3f(ax, ay, 0);
            glTexCoord3f(sz, 0, 0);
            glVertex3f(ax, ay, height);
            glTexCoord3f(sz, sx, 0);
            glVertex3f(x1, y1, height);
            glTexCoord3f(0, sx, 0);
            glVertex3f(x1, y1, 0);
        }
        glEnd();
    }
    glPopMatrix();

    glPushMatrix();
    {

        glBegin(GL_QUADS);
        {
            glTexCoord3f(0, 0, 0);
            glVertex3f(bx, by, 0);
            glTexCoord3f(sz, 0, 0);
            glVertex3f(bx, by, height);
            glTexCoord3f(sz, sx, 0);
            glVertex3f(x2, y2, height);
            glTexCoord3f(0, sx, 0);
            glVertex3f(x2, y2, 0);
        }
        glEnd();
    }
    glPopMatrix();






}
void drawOrion()
{

    glPopMatrix();
    glPushMatrix();
    {
        glTranslated(250, 300, 1000);
        drawSphere(6, 50, 50);
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslated(200, 300, 1000);
        drawSphere(6, 50, 50);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslated(150, 300, 1000);
        drawSphere(6, 50, 50);
    }
    glPopMatrix();

    glColor3f(1, 0.7, 0);
    glPushMatrix();
    {
        glTranslated(450, 600, 1000);
        drawSphere(6, 50, 50);
    }
    glColor3f(1, 1, 1);
    glPopMatrix();
    glPushMatrix();
    {

        glTranslated(-250, 600, 1000);
        drawSphere(6, 50, 50);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslated(500, -150, 1000);
        drawSphere(6, 50, 50);
    }

    glPopMatrix();
    glPushMatrix();
    {
        glTranslated(-100, -175, 1000);
        drawSphere(6, 50, 50);
    }
    glPopMatrix();

}
void buildTheMaze()
{
    //draw destination
    glPushMatrix();
    glColor3f(0.0, 0.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, texID[4]);
    drawWallGeneric(destination.a.x, destination.a.y, destination.b.x, destination.b.y, 50, 50, 1, 1);
    glPopMatrix();
    //draw player position
    glPushMatrix();
    {
        glTranslated(temp_pos.x, temp_pos.y, temp_pos.z + 20);
        drawSphere(playerRad, 50, 50);
        glTranslated(-temp_pos.x, -temp_pos.y, -temp_pos.z);
    }
    glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texID[0]);
    glColor3f(1.0, 0.0, 0.0);
    //border
    drawWallGeneric(-500, -500, -500, 500, 52, 15, 10, 1);
    drawWallGeneric(-500, -500, 500, -500, 52, 15, 10, 1);
    drawWallGeneric(500, 500, -500, 500, 52, 15, 10, 1);
    drawWallGeneric(500, 500, 500, -70, 52, 15, 10, 1);
    drawWallGeneric(500, -500, 500, -150, 52, 15, 10, 1);
    //main maze
    drawWallGeneric(400, 350, 400, 0, 52, 15, 7, 1);
    drawWallGeneric(400, 400, 0, 400, 52, 15, 8, 1);
    drawWallGeneric(400, 300, 300, 300, 52, 15, 1, 1);
    drawWallGeneric(300, 300, 300, 350, 52, 15, 1, 1);
    drawWallGeneric(-100, 0, 400, 0, 52, 15, 5, 1);
    drawWallGeneric(-200, -70, 500, -70, 52, 15, 6, 1);
    drawWallGeneric(-200, -70, -200, 200, 52, 15, 3, 1);
    drawWallGeneric(-200, 200, 200, 200, 52, 15, 4, 1);
    drawWallGeneric(200, 250, 400, 200, 52, 15, 6, 1);
    drawWallGeneric(-50, 500, -50, 300, 52, 15, 2, 1);
    drawWallGeneric(0, 400, 0, 250, 52, 15, 3, 1);
    drawWallGeneric(0, 250, -200, 250, 52, 15, 2, 1);
    drawWallGeneric(-200, 250, -200, 500, 52, 15, 3, 1);
    drawWallGeneric(-300, -150, 500, -150, 52, 15, 5, 1);
    drawWallGeneric(-300, -150, -300, 450, 52, 15, 6, 1);
    drawWallGeneric(300, 80, -100, 80, 52, 15, 2, 1);
    drawWallGeneric(-380, 500, -380, 150, 52, 15, 3, 1);
    drawWallGeneric(-380, 100, -380, -300, 52, 15, 2, 1);
    drawWallGeneric(-380, -300, -380, -330, 52, 15, 1, 1);
    drawWallGeneric(-380, -100, -300, -100, 52, 15, 1, 1);
    drawWallGeneric(-380, -330, 400, -330, 52, 15, 7, 1);
    drawWallGeneric(-200, -250, 300, -250, 52, 15, 2, 1);
    drawWallGeneric(-200, -250, -200, -170, 52, 15, 1, 1);
    drawWallGeneric(-300, -400, 400, -400, 52, 15, 2, 1);
    drawWallGeneric(400, -330, 400, -400, 52, 15, 2, 1);
    glPopMatrix();



}
void forceLookForward()
{

    l.z = 0;
    double d;
    d = sqrt(l.x * l.x + l.y * l.y);
    l.x = l.x / d;
    l.y = l.y / d;
    if (r.x * l.y - r.y * l.x > 0)
    {
        u.x = r.y * l.z - r.z * l.y;
        u.y = r.z * l.x - r.x * l.z;
        u.z = r.x * l.y - r.y * l.x;
    }
    else
    {
        u.x = -r.y * l.z + r.z * l.y;
        u.y = -r.z * l.x + r.x * l.z;
        u.z = -r.x * l.y + r.y * l.x;

        l.x = -l.x;
        l.y = -l.y;

    }
}
void output(int x, int y, int z, float r, float g, float b, int portX, int portY, int portWidth, int portHeight, void* font, std::string str)
{
    glViewport(portX, portY, portWidth, portHeight);
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    int len, i;
    len = (int)str.size();
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}
bool isGameOver()
{
    if (destination.a.x >= pos.x && destination.b.x <= pos.x && destination.a.y <= pos.y && destination.b.y + 50 >= pos.y)
    {
        return true;

    }
    return false;
}



float tmpY = 500;
float tmpZ = -25;
float rotate_angle = 0;
float rotate_angle1 = 60;
float rotate_angle2 = 0;
float rotate_angle3 = 30;
float rotate_angle4 = 0;
bool flag = false;
bool flag_ = false;
bool flag_angle = false;
bool flag_angle1 = false;
bool flag_angle2 = false;
bool flag_angle3 = false;
bool flag_angle4 = false;
int rotation = 0;

void drawSS()
{

    if (!isGameOver())
    {

        glViewport(0, 0, 600, 500);
        glColor3f(0.30, 0.20, 0.10);   //ground

        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texID[1]);
        drawWallGeneric(-500, -500, -500, 500, 0, 1000, 10, 10);
        glPopMatrix();

        //if (mapFlag == 0) {
        //지붕
        
        glPushMatrix();
        //glBindTexture(GL_TEXTURE_2D, texID[0]);
        glBindTexture(GL_TEXTURE_2D, texID[0]);
        glTranslatef(0, 0, 50);
        drawWallGeneric(-501, -501, -501, 501, 0, 1000, 10, 10);
        glPopMatrix();

        /*glPushMatrix();

        int ax = -501, ay = -501, bx = -501, by = 501;
        double dis = sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
        double x1, y1, x2, y2;
        x1 = ax + width * (by - ay) / dis;
        y1 = ay + width * (ax - bx) / dis;
        x2 = bx + width * (by - ay) / dis;
        y2 = by + width * (ax - bx) / dis;

        glColor3f(0.0, 0.0, 1.0);
        glTranslatef(0, 0, 51);
        
        glBegin(GL_QUADS);
        {
            
            glVertex3f(ax, ax, 0);
            
            glVertex3f(bx, by, 0);
            
            glVertex3f(x2, y2, 0);
            
            glVertex3f(x1, y1, 0);
        }
        glEnd();

        glPopMatrix();*/
        //}
        glColor3f(0.0, 0.0, 1.0);
        drawWallGeneric(500, -85, 500, -150, 52, 5, 1, 1);

        glColor3f(1, 1, 1);

        GLUquadricObj* earth;
        earth = gluNewQuadric();
        gluQuadricDrawStyle(earth, GLU_FILL);
        gluQuadricTexture(earth, GL_TRUE);

        
        //이동하는 구 장애물
        glPushMatrix();
        {   
            if (tmpY > -80 && flag == false) {
                tmpY -= 1;
                rotation += 8;
            }
            if (tmpY <= -80) {
                flag = true;
            }
            if (flag == true) {
                tmpY += 1;
                rotation -= 8;
            }
            if (tmpY >= 550) {
                flag = false;
            }
            
            glTranslatef(-340, tmpY, 25);
            glRotatef(rotation, 1, 0, 0);
            glBindTexture(GL_TEXTURE_2D, texID[3]);
            gluSphere(earth, 20, 100, 100);
        }
        glPopMatrix();
        
        //바닥에서 올라오는 가시1
        glPushMatrix();
        {
            if (tmpZ < 0 && flag_ == false) {
                tmpZ += 0.01;
            }
            if (tmpZ >= 0) {
                flag_ = true;
            }
            if (flag_ == true) {
                tmpZ -= 0.01;
            }
            if (tmpZ <= -25) {
                flag_ = false;
            }
            glTranslatef(-30, 225, tmpZ);
            glutSolidCone(10, 25, 20, 50);
        }
        glPopMatrix();
        
        //바닥에서 올라오는 가시2
        glPushMatrix();
        {
            if (tmpZ < 0 && flag_ == false) {
                tmpZ += 0.01;
            }
            if (tmpZ >= 0) {
                flag_ = true;
            }
            if (flag_ == true) {
                tmpZ -= 0.01;
            }
            if (tmpZ <= -25) {
                flag_ = false;
            }
            glTranslatef(-70, 230, tmpZ);
            glutSolidCone(10, 25, 20, 50);
        }
        glPopMatrix();

        //바닥에서 올라오는 가시3
        glPushMatrix();
        {
            if (tmpZ < 0 && flag_ == false) {
                tmpZ += 0.01;
            }
            if (tmpZ >= 0) {
                flag_ = true;
            }
            if (flag_ == true) {
                tmpZ -= 0.01;
            }
            if (tmpZ <= -25) {
                flag_ = false;
            }
            glTranslatef(-110, 225, tmpZ);
            glutSolidCone(10, 25, 20, 50);
        }
        glPopMatrix();

        //흔들리는 공1
        glPushMatrix();
        {
            if (rotate_angle < 60 && flag_angle == false) {
                rotate_angle += 1;
            }
            if (rotate_angle >= 60) {
                flag_angle = true;
            }
            if (flag_angle == true) {
                rotate_angle -= 1;
            }
            if (rotate_angle <= -60) {
                flag_angle = false;
            }
            glTranslatef(-300, -455, 50);
            glRotatef(rotate_angle, 1, 0, 0);
            glTranslatef(0, 0, -30);
            drawAxes();
            glBindTexture(GL_TEXTURE_2D, texID[2]);
            gluSphere(earth, 20, 100, 100);
        }
        glPopMatrix();

        //흔들리는 공2
        glPushMatrix();
        {
            if (rotate_angle1 < 60 && flag_angle1 == false) {
                rotate_angle1 += 1;
            }
            if (rotate_angle1 >= 60) {
                flag_angle1 = true;
            }
            if (flag_angle1 == true) {
                rotate_angle1 -= 1;
            }
            if (rotate_angle1 <= -60) {
                flag_angle1 = false;
            }
            glTranslatef(-150, -455, 50);
            glRotatef(rotate_angle1, 1, 0, 0);
            glTranslatef(0, 0, -30);
            drawAxes();
            glBindTexture(GL_TEXTURE_2D, texID[2]);
            gluSphere(earth, 20, 100, 100);
        }
        glPopMatrix();

        //흔들리는 공3
        glPushMatrix();
        {
            if (rotate_angle2 < 60 && flag_angle2 == false) {
                rotate_angle2 += 1;
            }
            if (rotate_angle2 >= 60) {
                flag_angle2 = true;
            }
            if (flag_angle2 == true) {
                rotate_angle2 -= 1;
            }
            if (rotate_angle2 <= -60) {
                flag_angle2 = false;
            }
            glTranslatef(-0, -455, 50);
            glRotatef(rotate_angle2, 1, 0, 0);
            glTranslatef(0, 0, -30);
            drawAxes();
            glBindTexture(GL_TEXTURE_2D, texID[2]);
            gluSphere(earth, 20, 100, 100);
        }
        glPopMatrix();

        //흔들리는 공4
        glPushMatrix();
        {
            if (rotate_angle3 < 60 && flag_angle3 == false) {
                rotate_angle3 += 1;
            }
            if (rotate_angle3 >= 60) {
                flag_angle3 = true;
            }
            if (flag_angle3 == true) {
                rotate_angle3 -= 1;
            }
            if (rotate_angle3 <= -60) {
                flag_angle3 = false;
            }
            glTranslatef(150, -455, 50);
            glRotatef(rotate_angle3, 1, 0, 0);
            glTranslatef(0, 0, -30);
            drawAxes();
            glBindTexture(GL_TEXTURE_2D, texID[2]);
            gluSphere(earth, 20, 100, 100);
        }
        glPopMatrix();

        //흔들리는 공5
        glPushMatrix();
        {
            if (rotate_angle4 < 60 && flag_angle4 == false) {
                rotate_angle4 += 1;
            }
            if (rotate_angle4 >= 60) {
                flag_angle4 = true;
            }
            if (flag_angle4 == true) {
                rotate_angle4 -= 1;
            }
            if (rotate_angle4 <= -60) {
                flag_angle4 = false;
            }
            glTranslatef(300, -455, 50);
            glRotatef(rotate_angle4, 1, 0, 0);
            glTranslatef(0, 0, -30);
            drawAxes();
            glBindTexture(GL_TEXTURE_2D, texID[2]);
            gluSphere(earth, 20, 100, 100);
        }
        glPopMatrix();

        drawOrion();
        buildTheMaze();
        glPushMatrix();
        {
            int n = ((clock() - start) / 1000);

            sec = n % 60;
            mn = n / 60;
            hour = n / 3600;
            std::stringstream ss;
            ss << sec;
            strSec = ss.str();
            ss.str(std::string());
            ss.clear();
            ss << mn;
            strMn = ss.str();
            ss.str(std::string());
            ss.clear();
            ss << hour;
            strHour = ss.str();
            std::string str = "time  " + strHour + "::" + strMn + "::" + strSec;
            output(0, 0, 0, 0, 0, 1, 400, 400, 10, 100, (void*)font, str);
        }
        glPopMatrix();

    }
    else
    {
        std::string str = "your TIME: " + strHour + "::" + strMn + "::" + strSec;
        l.x = 150;
        l.y = 150;
        gameOver = 1;
        output(0, 0, 0, 0, 1, 0, 0, 0, 400, 600, (void*)font, str);
    }



}

void keyboardListener(unsigned char key, int x, int y)
{
    double angle = 0.5;
    if (gameOver == 0)
    {
        switch (key)
        {

        case '1':
            angle = 0.05;
            r.x = r.x * cos(angle) + l.x * sin(angle);
            r.y = r.y * cos(angle) + l.y * sin(angle);
            r.z = r.z * cos(angle) + l.z * sin(angle);
            //now, l=u*r
            l.x = u.y * r.z - u.z * r.y;
            l.y = u.z * r.x - u.x * r.z;
            l.z = u.x * r.y - u.y * r.x;
            break;

        case '2':
            angle = -0.05;
            r.x = r.x * cos(angle) + l.x * sin(angle);
            r.y = r.y * cos(angle) + l.y * sin(angle);
            r.z = r.z * cos(angle) + l.z * sin(angle);
            //now, l=u*r
            l.x = u.y * r.z - u.z * r.y;
            l.y = u.z * r.x - u.x * r.z;
            l.z = u.x * r.y - u.y * r.x;
            break;
            //case '3'://look up
            //    angle = 0.05;
            //    l.x = l.x * cos(angle) + u.x * sin(angle);
            //    l.y = l.y * cos(angle) + u.y * sin(angle);
            //    l.z = l.z * cos(angle) + u.z * sin(angle);
            //    //now, u=r*l
            //    u.x = r.y * l.z - r.z * l.y;
            //    u.y = r.z * l.x - r.x * l.z;
            //    u.z = r.x * l.y - r.y * l.x;
            //    break;
            //case '4'://look down
            //    angle = -0.05;
            //    l.x = l.x * cos(angle) + u.x * sin(angle);
            //    l.y = l.y * cos(angle) + u.y * sin(angle);
            //    l.z = l.z * cos(angle) + u.z * sin(angle);
            //    //now, u=r*l
            //    u.x = r.y * l.z - r.z * l.y;
            //    u.y = r.z * l.x - r.x * l.z;
            //    u.z = r.x * l.y - r.y * l.x;
            //    break;


        case 'x':
            if (debug == 0)
                debug = 1;
            else
                debug = 0;
            break;
        case 'l':
            forceLookForward();
            break;
        case 'm':
            if (mapFlag == 0)
            {
                mapFlag = 1;
                temp_pos.x = pos.x;
                temp_pos.y = pos.y;
                temp_pos.z = pos.z;
                temp_l.x = l.x;
                temp_l.y = l.y;
                temp_l.z = l.z;
                pos.x = -50;
                pos.y = 100;
                pos.z = 990;
                l.x = 0;
                l.y = 0;
                l.z = -1;

                angle = 0.05;
                l.x = l.x * cos(angle) + u.x * sin(angle);
                l.y = l.y * cos(angle) + u.y * sin(angle);
                l.z = l.z * cos(angle) + u.z * sin(angle);
                //now, u=r*l
                u.x = r.y * l.z - r.z * l.y;
                u.y = r.z * l.x - r.x * l.z;
                u.z = r.x * l.y - r.y * l.x;

                moon.rad = 0;



            }
            else
            {

                mapFlag = 0;
                pos.x = temp_pos.x;
                pos.y = temp_pos.y;
                pos.z = temp_pos.z;
                l.x = temp_l.x;
                l.y = temp_l.y;
                l.z = temp_l.z;

                angle = 0.05;
                l.x = l.x * cos(angle) + u.x * sin(angle);
                l.y = l.y * cos(angle) + u.y * sin(angle);
                l.z = l.z * cos(angle) + u.z * sin(angle);
                //now, u=r*l
                u.x = r.y * l.z - r.z * l.y;
                u.y = r.z * l.x - r.x * l.z;
                u.z = r.x * l.y - r.y * l.x;
                moon.rad = 10;

            }

        default:
            break;
        }
    }
}

int MaxMoveX = 485;
int MaxMoveY = 485;
int MinMoveX = -485;
int MinMoveY = -485;

void specialKeyListener(int key, int x, int y)
{
    if (gameOver == 0)
    {
        if (pos.x >= MaxMoveX) {
            pos.x = MaxMoveX;
        }
        if (pos.x <= MinMoveX) {
            pos.x = MinMoveX;
        }
        if (pos.y >= MaxMoveY) {
            pos.y = MaxMoveY;
        }
        if (pos.y <= MinMoveY) {
            pos.y = MinMoveY;
        }
        switch (key)
        {
        case GLUT_KEY_DOWN:		//down arrow key
            double unit;
            if (pos.z == 25)
            {
                forceLookForward();

                unit = sqrt(l.x * l.x + l.y * l.y + l.z * l.z) / 10;
                pos.x = pos.x - l.x / unit;
                pos.y = pos.y - l.y / unit;
                pos.z = pos.z - l.z / unit;

            }
            break;
        case GLUT_KEY_UP:		// up arrow key
            if (pos.z == 25)
            {
                forceLookForward();
                unit = sqrt(l.x * l.x + l.y * l.y + l.z * l.z) / 10;
                pos.x = pos.x + l.x / unit;
                pos.y = pos.y + l.y / unit;
                pos.z = pos.z + l.z / unit;
            }

            break;

        case GLUT_KEY_RIGHT:

            unit = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
            pos.x = pos.x + r.x / unit;
            pos.y = pos.y + r.y / unit;
            pos.z = pos.z + r.z / unit;
            break;
        case GLUT_KEY_LEFT:

            unit = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
            pos.x = pos.x - r.x / unit;
            pos.y = pos.y - r.y / unit;
            pos.z = pos.z - r.z / unit;

            break;


        case GLUT_KEY_INSERT:
            break;

        case GLUT_KEY_HOME:


            break;
        case GLUT_KEY_END:

            break;

        default:
            break;
        }
    }
}


void mouseListener(int button, int state, int x, int y) 	//x, y is the x-y of the screen (2D)
{
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
            drawaxes = 1 - drawaxes;
        }
        break;

    case GLUT_RIGHT_BUTTON:
        //........
        break;

    case GLUT_MIDDLE_BUTTON:
        //........
        break;

    default:
        break;
    }
}



void display()
{

    ///clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);	///color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
    / set-up camera here
    ********************/
    ///load the correct matrix -- MODEL-VIEW matrix
    //glViewport(0, 0, 500, 500);
    glMatrixMode(GL_MODELVIEW);

    ///initialize the matrix
    glLoadIdentity();



    //now give three info
    //1. where is the camera (viewer)?
    //2. where is the camera looking?
    //3. Which direction is the camera's UP direction?

    //gluLookAt(100,100,100,	0,0,0,	0,0,1);
    gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);
    //gluLookAt(0,0,1000,	0,0,0,	0,1,0);


    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/
    //add objects

    //drawAxes();
    //drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

    /*glPushMatrix();
    glTranslatef(-100, 200, 0);
    glColor3f(1.0, 1.0, 0.0);
    drawSphere(30,24,20);
    glPopMatrix();*/




    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}


void animate()
{
    if (debug == 1)
    {
        std::cout << l.x << " " << l.y << " " << l.z << "\n";
        std::cout << r.x << " " << r.y << " " << r.z << "\n";
        std::cout << u.x << " " << u.y << " " << u.z << "\n";
        std::cout << "pos " << pos.x << " " << pos.y << " " << pos.z << "\n";
    }
    if (mapFlag == 1 && playerRadFlag == 0 && playerRad <= 20)
    {

        playerRad++;
        if (playerRad == 20)
        {
            playerRadFlag = 1;
        }
    }
    else if (mapFlag == 1 && playerRadFlag == 1 && playerRad > 0)
    {
        playerRad--;
        if (playerRad == 1)
        {
            playerRadFlag = 0;
        }
    }
    else if (mapFlag == 0)
    {
        playerRad = 0;
        playerRadFlag = 0;
    }
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}


void destroy() {
    stbi_image_free(wallImageData.data);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(600, 500);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("My OpenGL Program");

    init();

    glEnable(GL_DEPTH_TEST);	//enable Depth Testing

    glutDisplayFunc(display);	//display callback function
    glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    //destroy();

    glutMainLoop();
}
