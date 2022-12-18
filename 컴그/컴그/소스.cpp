#include <gl/glut.h>
#include <math.h>
#include <string>
#include <sstream>
#include <iostream>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma comment (lib, "winmm.lib")
#include <mmsystem.h>

#define pi 3.141529
#define	PI   3.1415926
#define	N    36   	
#define	M    18   	

#define SOUND_FILE_BGM "The Final Showdown.wav"
#define SOUND_FILE_PIERCE "pierce.wav"
#define SOUND_FILE_HITROCK "hitrock.wav"


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
GLfloat fogColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

float	theta, phi;
float	delta_theta, delta_phi;
float	start_theta, start_phi;
float	Radius = 1.0;
float 	ver[N][M + 1][3];

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
const char* imagefiles[5] = { "wall.png", "roof.png", "ball.png", "ball1.jpg", "gold.png"};//가져올 이미지
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
    

    glEnable(GL_TEXTURE_2D);
    glGenTextures(5, texID); //(텍스처 개수, 텍스처 저장공간)
    PlaySound(TEXT(SOUND_FILE_BGM), NULL, SND_ASYNC);
    

    for (int i = 0; i < 5; i++)
    {
        unsigned char* bitmap;
        bitmap = LoadMeshFromFile((char*)imagefiles[i]);
        glBindTexture(GL_TEXTURE_2D, texID[i]);
        //gluBuild2DMipmaps(GL_TEXTURE_2D, channel, width, height, 4, GL_UNSIGNED_BYTE, bitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
        free(bitmap);
    }

    drawgrid = 1;
    drawaxes = 1;
    cameraHeight = 150.0;
    cameraAngle = 1.0;
    angle = 0;
    debug = 0;

    glClearColor(0, 0, 0, 0);
    glFogi(GL_FOG_MODE, GL_LINEAR); // <1>
    glFogfv(GL_FOG_COLOR, fogColor); // <2>
    glFogf(GL_FOG_DENSITY, 0.3f); // <3>
    glHint(GL_FOG_HINT, GL_NICEST); // <4>
    glFogf(GL_FOG_START, 7.0f); // <5>
    glFogf(GL_FOG_END, 500.0f); // <6>
    glEnable(GL_FOG); // <7>

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluPerspective(100, 1, 1, 1000.0);

}

void 	reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 1000);
}

void 	cameraSetting(void) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(2.0 * Radius, 2.0 * Radius, 2.0 * Radius,
        0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}

void drawAxes()
{
    glLineWidth(10.0);

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    {
        glVertex3f(0, 0, 100);
        glVertex3f(0, 0, 0);
    }
    glEnd();   
}


void drawGrid()
{
    int i;
    if (drawgrid == 1)
    {
        glColor3f(0.6, 0.6, 0.6);	
        glBegin(GL_LINES);
        {
            for (i = -8; i <= 8; i++)
            {

                if (i == 0)
                    continue;	

                glVertex3f(i * 10, -90, 0);
                glVertex3f(i * 10, 90, 0);

                glVertex3f(-90, i * 10, 0);
                glVertex3f(90, i * 10, 0);
            }
        }
        glEnd();
    }
}

void drawSquare(double a)
{

    glBegin(GL_QUADS);
    {
        glVertex3f(a, a, 0);
        glVertex3f(a, -a, 0);
        glVertex3f(-a, -a, 0);
        glVertex3f(-a, a, 0);
    }
    glEnd();
}


void drawWallGeneric(double ax, double ay, double bx, double by, double height, double width, float sz, float sx)
{

    glEnable(GL_TEXTURE_2D);

    glColor3f(1, 1, 1);

    double dis = sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
    double x1, y1, x2, y2;
    x1 = ax + width * (by - ay) / dis;
    y1 = ay + width * (ax - bx) / dis;
    x2 = bx + width * (by - ay) / dis;
    y2 = by + width * (ax - bx) / dis;
    

    glPushMatrix();
    {
        glBegin(GL_QUADS);
        {
            glTexCoord3f(0, 0, 0);
            glVertex3f(ax, ay, 0);//좌하단
            glTexCoord3f(sz, 0, 0);
            glVertex3f(bx, by, 0);//우하단
            glTexCoord3f(sz, sx, 0);
            glVertex3f(bx, by, height);//우상단
            glTexCoord3f(0, sx, 0);
            glVertex3f(ax, ay, height);//좌상단
        }
        glEnd();
    }
    glPopMatrix();

    glPushMatrix();
    {
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


void buildTheMaze()
{
    
    glPushMatrix();
    glColor3f(0.0, 0.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, texID[4]);
    drawWallGeneric(destination.a.x, destination.a.y, destination.b.x, destination.b.y, 30, 30, 1, 1);
    glPopMatrix();

    glPushMatrix();
    {
        glTranslated(temp_pos.x, temp_pos.y, temp_pos.z + 20);
        glTranslated(-temp_pos.x, -temp_pos.y, -temp_pos.z);
    }
    glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texID[0]);
    glColor3f(1.0, 0.0, 0.0);
    
    drawWallGeneric(-500, -500, -500, 500, 52, 15, 10, 1);
    drawWallGeneric(-500, -500, 500, -500, 52, 15, 10, 1);
    drawWallGeneric(500, 500, -500, 500, 52, 15, 10, 1);
    drawWallGeneric(500, 500, 500, -70, 52, 15, 10, 1);
    drawWallGeneric(500, -500, 500, -150, 52, 15, 10, 1);
    
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
float tmpZ = -50;
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
bool crash = false;


void drawSS()
{

    if (!isGameOver())
    {

        glViewport(0, 0, 720, 720);
        glColor3f(0.30, 0.20, 0.10);  

        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texID[1]);
        drawWallGeneric(-500, -500, -500, 500, 0, 1000, 10, 10);
        glPopMatrix();

        
        
        glPushMatrix();
        
        glBindTexture(GL_TEXTURE_2D, texID[0]);
        glTranslatef(0, 0, 50);
        drawWallGeneric(-501, -501, -501, 501, 0, 1000, 10, 10);
        glPopMatrix();

        
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
                rotation += 6;
            }
            if (tmpY <= -80) {
                flag = true;
            }
            if (flag == true) {
                tmpY += 1;
                rotation -= 6;
            }
            if (tmpY >= 550) {
                flag = false;
            }
            
            glTranslatef(-340, tmpY, 25);
            glRotatef(rotation, 1, 0, 0);
            glBindTexture(GL_TEXTURE_2D, texID[3]);
            gluSphere(earth, 20, 100, 100);
            if (-320 >= pos.x && -360 <= pos.x && tmpY-20 <= pos.y && tmpY+20 >= pos.y) {
                std::cout << "아야!!" << "\n";
                pos.x = 450;
                pos.y = 450;
                pos.z = 25;
                PlaySound(TEXT(SOUND_FILE_HITROCK), NULL, SND_ASYNC);
            }
        }
        glPopMatrix();
        
        //바닥에서 올라오는 가시1
        glPushMatrix();
        {
            if (tmpZ < 5 && flag_ == false) {
                tmpZ += 0.1;
            }
            if (tmpZ >= 5) {
                flag_ = true;
            }
            if (flag_ == true) {
                tmpZ -= 0.1;
            }
            if (tmpZ <= -50) {
                flag_ = false;
            }
            glTranslatef(-30, 225, tmpZ);
            glColor3d(100, 100, 100);
            glutSolidCone(5, 30, 20, 50);
            if (-15 >= pos.x && -45 <= pos.x && 210 <= pos.y && 240 >= pos.y && tmpZ + 25 >= pos.z) {
                std::cout << "아야!!" << "\n";
                PlaySound(TEXT(SOUND_FILE_PIERCE), NULL, SND_ASYNC);
                pos.x = 450;
                pos.y = 450;
                pos.z = 25;
              
            }
        }
        glPopMatrix();
        
        
        //바닥에서 올라오는 가시2
        glPushMatrix();
        {
            if (tmpZ < 5 && flag_ == false) {
                tmpZ += 0.1;
            }
            if (tmpZ >= 5) {
                flag_ = true;
            }
            if (flag_ == true) {
                tmpZ -= 0.1;
            }
            if (tmpZ <= -50) {
                flag_ = false;
            }
            glTranslatef(-90, 230, tmpZ);
            glColor3d(100, 100, 100);
            glutSolidCone(5, 30, 20, 50);
            if (-75 >= pos.x && -105 <= pos.x && 210 <= pos.y && 240 >= pos.y && tmpZ + 25 < pos.z) {
                std::cout << "아야!!" << "\n";
                PlaySound(TEXT(SOUND_FILE_PIERCE), NULL, SND_ASYNC);
                pos.x = 450;
                pos.y = 450;
                pos.z = 25;
                
            }
        }
        glPopMatrix();
        

        //바닥에서 올라오는 가시3
        glPushMatrix();
        {
            if (tmpZ < 5 && flag_ == false) {
                tmpZ += 0.1;
            }
            if (tmpZ >= 5) {
                flag_ = true;
            }
            if (flag_ == true) {
                tmpZ -= 0.1;
            }
            if (tmpZ <= -50) {
                flag_ = false;
            }
            glTranslatef(-150, 225, tmpZ);
            glColor3d(100, 100, 100);
            glutSolidCone(5, 30, 20, 50);
            if (-135 >= pos.x && -165 <= pos.x && 210 <= pos.y && 240 >= pos.y && tmpZ + 25 < pos.z) {
                std::cout << "아야!!" << "\n";
                PlaySound(TEXT(SOUND_FILE_PIERCE), NULL, SND_ASYNC);
                pos.x = 450;
                pos.y = 450;
                pos.z = 25;
                
            }
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
            gluSphere(earth, 15, 100, 100);
            if (-285 + sin(rotate_angle) >= pos.x && -315 - sin(rotate_angle) <= pos.x && -470 <= pos.y && -440 >= pos.y ) {
                std::cout << "아야!!" << "\n";
                PlaySound(TEXT(SOUND_FILE_HITROCK), NULL, SND_ASYNC);
                pos.x = 450;
                pos.y = 450;
                pos.z = 25;
            }
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
            gluSphere(earth, 15, 100, 100);
            if (-135 + sin(rotate_angle) >= pos.x && -165 - sin(rotate_angle) <= pos.x && -470 <= pos.y && -440 >= pos.y) {
                std::cout << "아야!!" << "\n";
                PlaySound(TEXT(SOUND_FILE_HITROCK), NULL, SND_ASYNC);
                pos.x = 450;
                pos.y = 450;
                pos.z = 25;
            }
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
            gluSphere(earth, 15, 100, 100);
            if (15 + sin(rotate_angle) >= pos.x && -15 - sin(rotate_angle) <= pos.x && -470 <= pos.y && -440 >= pos.y) {
                std::cout << "아야!!" << "\n";
                PlaySound(TEXT(SOUND_FILE_HITROCK), NULL, SND_ASYNC);
                pos.x = 450;
                pos.y = 450;
                pos.z = 25;
            }
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
            gluSphere(earth, 15, 100, 100);
            if (165 + sin(rotate_angle) >= pos.x && 135 - sin(rotate_angle) <= pos.x && -470 <= pos.y && -440 >= pos.y) {
                std::cout << "아야!!" << "\n";
                PlaySound(TEXT(SOUND_FILE_HITROCK), NULL, SND_ASYNC);
                pos.x = 450;
                pos.y = 450;
                pos.z = 25;
            }
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
            gluSphere(earth, 15, 100, 100);
            if (315 + sin(rotate_angle) >= pos.x && 285 - sin(rotate_angle) <= pos.x && -470 <= pos.y && -440 >= pos.y) {
                std::cout << "아야!!" << "\n";
                PlaySound(TEXT(SOUND_FILE_HITROCK), NULL, SND_ASYNC);
                pos.x = 450;
                pos.y = 450;
                pos.z = 25;
            }
        }
        glPopMatrix();
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
        std::string str1 = "Merry Christmas";
        l.x = 150;
        l.y = 150;
        gameOver = 1;
        output(0, 0, 0, 1, 1, 0, 0, 0, 400, 600, (void*)font, str);
        output(300, 250, 0, 1, 1, 0, 0, 0, 400, 600, (void*)font, str1);
        /*pos.x = 450;
        pos.y = 450;
        pos.z = 25;*/
    }



}

void keyboardListener(unsigned char key, int x, int y)
{
    double angle = 0.5;
    if (gameOver == 0)
    {
        switch (key)
        {
        case 'x':
            if (debug == 0)
                debug = 1;
            else
                debug = 0;
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

int MaxMoveX = 475;
int MaxMoveY = 475;
int MinMoveX = -475;
int MinMoveY = -475;

void specialKeyListener(int key, int x, int y)
{
    double angle = 0.5;
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
        case GLUT_KEY_DOWN:		
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
        case GLUT_KEY_UP:		
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

            angle = -0.05;
            r.x = r.x * cos(angle) + l.x * sin(angle);
            r.y = r.y * cos(angle) + l.y * sin(angle);
            r.z = r.z * cos(angle) + l.z * sin(angle);
            
            l.x = u.y * r.z - u.z * r.y;
            l.y = u.z * r.x - u.x * r.z;
            l.z = u.x * r.y - u.y * r.x;
            
            break;

        case GLUT_KEY_LEFT:

            angle = 0.05;
            r.x = r.x * cos(angle) + l.x * sin(angle);
            r.y = r.y * cos(angle) + l.y * sin(angle);
            r.z = r.z * cos(angle) + l.z * sin(angle);
            
            l.x = u.y * r.z - u.z * r.y;
            l.y = u.z * r.x - u.x * r.z;
            l.z = u.x * r.y - u.y * r.x;
            
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


void mouseListener(int button, int state, int x, int y) 	
{
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) 		
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

void Sound(int Value) {
    PlaySound(TEXT(SOUND_FILE_BGM), NULL, SND_ASYNC);

    glutTimerFunc(6400, Sound, 1);
}

void display()
{

    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);	///color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    glMatrixMode(GL_MODELVIEW);
    cameraSetting();
    glLoadIdentity();
    

    gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);
    float light_position[] = { (pos.x + l.x) , (pos.y + l.y)  , 0 , 1.0f };
    float light_ambient[] = { 1.0, 1.0, 1.0, 0.0 };
    float light_diffuse[] = { 1.0, 1.0, 1.0, 0.0 };
    float light_specular[] = { 1.0, 1.0, 1.0, 0.0 };

    glShadeModel(GL_SMOOTH);      

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glEnable(GL_DEPTH_TEST);
    

    glMatrixMode(GL_MODELVIEW);


    drawSS();


    glFlush();
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
    
    glutPostRedisplay();
}


void destroy() {
    stbi_image_free(wallImageData.data);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(720, 720);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	

    glutCreateWindow("찬호의 사원 대탐험");

    init();

    glEnable(GL_DEPTH_TEST);	

    glutDisplayFunc(display);	
    glutIdleFunc(animate);		
    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);
    glutTimerFunc(6400, Sound, 1);

    glutMainLoop();
}
