/*************************************************************************************/
// Najprostszy program wykorzystujący funkcje biblioteki OpenGL
/*************************************************************************************/
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <iostream>
#include <math.h>

#define M_PI  3.14159265358979323846
typedef float point3[3];


const int n = 20;
int model = 3;
point3 arr[n][n];
point3 colors[n][n];
point3 normalizedVector[n][n];
static GLfloat theta[] = { 0.0,0.0,0.0 };

bool all_rotation = false;
bool X_rotation = false;
bool Y_rotation = false;
bool Z_rotation = false;
bool teapotDraw = false;
bool eggDraw = true;
bool pyramidDraw = false;
static GLfloat thetta = 0.0;
static float fi = 0.0;
static GLfloat pix2angleX;
static GLfloat pix2angleY;
float radius = 15.0;
// Naciśnięty klawisz myszy
static GLint status = 0;
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };
static int x_pos_old = 0;
static int y_pos_old = 0;
static int zoom_pos_old = 0;
static int delta_x = 0;
static int delta_y = 0;
static int delta_zoom = 0;


GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{
    /*************************************************************************************/
    // Struktura dla nagłówka pliku  TGA

#pragma pack(1)
    typedef struct
    {
        GLbyte    idlength;
        GLbyte    colormaptype;
        GLbyte    datatypecode;
        unsigned short    colormapstart;
        unsigned short    colormaplength;
        unsigned char     colormapdepth;
        unsigned short    x_orgin;
        unsigned short    y_orgin;
        unsigned short    width;
        unsigned short    height;
        GLbyte    bitsperpixel;
        GLbyte    descriptor;
    }TGAHEADER;
#pragma pack(8)

    FILE* pFile;
    TGAHEADER tgaHeader;
    unsigned long lImageSize;
    short sDepth;
    GLbyte* pbitsperpixel = NULL;

    /*************************************************************************************/
    // Wartości domyślne zwracane w przypadku błędu

    *ImWidth = 0;
    *ImHeight = 0;
    *ImFormat = GL_BGR_EXT;
    *ImComponents = GL_RGB8;

#pragma warning(suppress : 4996)
    pFile = fopen(FileName, "rb");
    if (pFile == NULL)
        return NULL;
    /*************************************************************************************/
    // Przeczytanie nagłówka pliku

    fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

    /*************************************************************************************/
    // Odczytanie szerokości, wysokości i głębi obrazu

    *ImWidth = tgaHeader.width;
    *ImHeight = tgaHeader.height;
    sDepth = tgaHeader.bitsperpixel / 8;

    /*************************************************************************************/
    // Sprawdzenie, czy głębia spełnia założone warunki (8, 24, lub 32 bity)

    if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
        return NULL;

    /*************************************************************************************/
    // Obliczenie rozmiaru bufora w pamięci

    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

    /*************************************************************************************/
    // Alokacja pamięci dla danych obrazu

    pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

    if (pbitsperpixel == NULL)
        return NULL;

    if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
    {
        free(pbitsperpixel);
        return NULL;
    }

    /*************************************************************************************/
    // Ustawienie formatu OpenGL

    switch (sDepth)
    {
    case 3:
        *ImFormat = GL_BGR_EXT;
        *ImComponents = GL_RGB8;
        break;
    case 4:
        *ImFormat = GL_BGRA_EXT;
        *ImComponents = GL_RGBA8;
        break;
    case 1:
        *ImFormat = GL_LUMINANCE;
        *ImComponents = GL_LUMINANCE8;
        break;
    };

    fclose(pFile);

    return pbitsperpixel;
}

void drawEggDots()
{
    point3 dotColors[n][n];
    glBegin(GL_POINTS);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            dotColors[i][j][0] = 1.0;
            dotColors[i][j][1] = 1.0;
            dotColors[i][j][2] = 1.0;
            glColor3fv(dotColors[i][j]);
            glVertex3fv(arr[i][j]);
        }
    }
    glEnd();
}

void randomColorization()
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            colors[i][j][0] = (rand() % 1000) / 1000.0;
            colors[i][j][1] = (rand() % 1000) / 1000.0;
            colors[i][j][2] = (rand() % 1000) / 1000.0;
        }
    }
}

void drawPiramid()
{

    glBegin(GL_TRIANGLES);
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, 1.0);

    glTexCoord2f(0.0, 1.0);
    glVertex3f(1.0, -1.0, 1.0);

    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.0, 1.0, 0.0);

    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);

    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);

    glTexCoord2f(1.0, 1.0);
    glVertex3f(1.0, -1.0, -1.0);

    glNormal3f(-1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.0, 1.0, 0.0);

    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glEnd();
    glBegin(GL_POLYGON);
    glNormal3f(0.0, -1.0, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);

    glTexCoord2f(1.0, 0.0);
    glVertex3f(1.0, -1.0, -1.0);

    glTexCoord2f(1.0, 1.0);
    glVertex3f(1.0, -1.0, 1.0);

    glTexCoord2f(0.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glEnd();
}










void drawTriangles() {
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - 1; j++) {
            // Prawy gorny trójkąt na styku
            glNormal3fv(normalizedVector[i][j]);
            glTexCoord3fv(arr[i][j]);
            //            glColor3fv(colors[i][j]);
            glVertex3fv(arr[i][j]);

            glNormal3fv(normalizedVector[i][j + 1]);
            glTexCoord3fv(arr[i][j + 1]);
            //            glColor3fv(colors[i][j + 1]);
            glVertex3fv(arr[i][j + 1]);

            glNormal3fv(normalizedVector[(i + 1) % n][j]);
            //            glColor3fv(colors[(i + 1) % n][j]);
            glTexCoord3fv(arr[(i + 1) % n][j]);
            glVertex3fv(arr[(i + 1) % n][j]);

            // Lewy dolny trójkąt na styku
            glNormal3fv(normalizedVector[i][j + 1]);
            //            glColor3fv(colors[i][j + 1]);
            glTexCoord3fv(arr[i][j + 1]);
            glVertex3fv(arr[i][j + 1]);

            glNormal3fv(normalizedVector[(i + 1) % n][j]);
            //            glColor3fv(colors[(i + 1) % n][j]);
            glTexCoord3fv(arr[(i + 1) % n][j]);
            glVertex3fv(arr[(i + 1) % n][j]);

            glNormal3fv(normalizedVector[(i + 1) % n][j + 1]);
            //            glColor3fv(colors[(i + 1) % n][j + 1]);
            glTexCoord3fv(arr[(i + 1) % n][j + 1]);
            glVertex3fv(arr[(i + 1) % n][j + 1]);
        }
        // Prawy gorny trójkąt
        glNormal3fv(normalizedVector[i][n - 1]);
        //        glColor3fv(colors[i][n - 1]);
        glTexCoord3fv(arr[i][n - 1]);
        glVertex3fv(arr[i][n - 1]);

        glNormal3fv(normalizedVector[(n - i) % n][0]);
        //        glColor3fv(colors[(n - i) % n][0]);
        glTexCoord3fv(arr[(n - i) % n][0]);
        glVertex3fv(arr[(n - i) % n][0]);

        glNormal3fv(normalizedVector[(i + 1) % n][n - 1]);
        //        glColor3fv(colors[(i + 1) % n][n - 1]);
        glTexCoord3fv(arr[(i + 1) % n][n - 1]);
        glVertex3fv(arr[(i + 1) % n][n - 1]);

        // Lewy dolny trójkąt na styku
        glNormal3fv(normalizedVector[(n - i) % n][0]);
        //        glColor3fv(colors[(n - i) % n][0]);
        glTexCoord3fv(arr[(n - i) % n][0]);
        glVertex3fv(arr[(n - i) % n][0]);

        glNormal3fv(normalizedVector[(i + 1) % n][n - 1]);
        //        glColor3fv(colors[(i + 1) % n][n - 1]);
        glTexCoord3fv(arr[(i + 1) % n][n - 1]);
        glVertex3fv(arr[(i + 1) % n][n - 1]);

        glNormal3fv(normalizedVector[(n - i - 1) % n][0]);
        //        glColor3fv(colors[(n - i - 1) % n][0]);
        glTexCoord3fv(arr[(n - i - 1) % n][0]);
        glVertex3fv(arr[(n - i - 1) % n][0]);
    }

    glEnd();
}

void Egg()
{
    float u, v;
    for (int i = 0; i < n; i++)
    {
        u = float(i) / n;
        for (int j = 0; j < n; j++)
        {
            v = float(j) / n;
            arr[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v);
            arr[i][j][1] = (160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2));
            arr[i][j][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v);

            GLfloat Xu, Xv, Yu, Yv, Zu, Zv;
            Xu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v);
            Yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
            Zu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(M_PI * v);
            Xv = M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(M_PI * v);
            Yv = 0;
            Zv = -M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(M_PI * v);

            GLfloat Nx, Ny, Nz;
            Nx = Yu * Zv - Zu * Yv;
            Ny = Zu * Xv - Xu * Zv;
            Nz = Xu * Yv - Yu * Xv;

            GLfloat vectorLength = sqrt(pow(Nx, 2) + pow(Ny, 2) + pow(Nz, 2));
            if (vectorLength == 0) vectorLength = 1;

            normalizedVector[i][j][0] = Nx / vectorLength;
            normalizedVector[i][j][1] = Ny / vectorLength;
            normalizedVector[i][j][2] = Nz / vectorLength;
        }
    }

    switch (model)
    {
    case 1:drawEggDots(); break;
    case 3:drawTriangles(); break;
    }

}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    if (status == 1) {
        fi += delta_y * pix2angleY;
        thetta += delta_x * pix2angleX;
        if (thetta >= 360.0) { thetta = 0.0; }
        if (fi >= 360.0) { fi = 0.0; }
    }

    else if (status == 2) {
        radius += 0.1 * delta_zoom;
        if (radius >= 25.0) { radius = 25.0; }
        if (radius <= 10.0) { radius = 10.0; }
    }
    glRotatef(thetta, 1.0, 1.0, 1.0);

    viewer[0] = radius * cos(thetta) * cos(fi);
    viewer[1] = radius * sin(fi);
    viewer[2] = radius * sin(thetta) * cos(fi);
    gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, cos(fi), 0.0);
    glTranslated(0.0, -4.5, 0.0);
    if (eggDraw)
    {
        Egg();
    }
    if (teapotDraw)
    {
        glutSolidTeapot(3.0);
    }
    if (pyramidDraw) {
        drawPiramid();
    }
    glFlush();
    glutSwapBuffers();
}

void keys(unsigned char key, int x, int y)
{
    if (key == 'p') model = 1;
    if (key == 'l') model = 2;
    if (key == 's') model = 3;
    if (key == 'd') {
        if (all_rotation) all_rotation = false;
        else all_rotation = true;
    }
    if (key == 'x') {
        X_rotation = true;
        Y_rotation = false;
        Z_rotation = false;
    }
    if (key == 'y') {
        X_rotation = false;
        Y_rotation = true;
        Z_rotation = false;
    }
    if (key == 'z') {
        X_rotation = false;
        Z_rotation = true;
        Y_rotation = false;
    }
    if (key == 't') {
        teapotDraw = true; eggDraw = false; pyramidDraw = false;
    }
    if (key == 'e') {
        eggDraw = true; teapotDraw = false; pyramidDraw = false;
    }
    if (key == 'p') {
        pyramidDraw = true; teapotDraw = false; eggDraw = false;
    }
    RenderScene();
}

void MyInit()
{
    // Kolorowanie okna na czarno
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Zmienne przechowujące dane o teksturze
    GLbyte* pBytes;
    GLint ImWidth, ImHeight, ImComponents;
    GLenum ImFormat;
    // Definicja materiału z jakiego zrobiony jest przedmiot
    GLfloat mat_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess = 20.0f;

    GLfloat light_position[] = { 30.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

    GLfloat att_constant = 1.0f;
    GLfloat att_linear = 0.05f;
    GLfloat att_quadratic = 0.001f;

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    // Przeczytanie obrazu tekstury z pliku
    pBytes = LoadTGAImage("P5_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
    // Zdefiniowanie tekstury 2-D
    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat,
        GL_UNSIGNED_BYTE, pBytes);
    // Zwolnienie pamięci po załadowanej teksturze
    free(pBytes);
    // Włączenie mechanizmu teksturowania

    glEnable(GL_TEXTURE_2D);
    // Ustalenie trybu teksturowania
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    // Określenie sposobu nałożenia tekstury
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void changeSize(GLsizei horizontal, GLsizei vertical)
{
    pix2angleX = 360.0 * 0.01 / (float)horizontal;
    pix2angleY = 360.0 * 0.01 / (float)vertical;
    GLfloat AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    if (vertical == 0) { vertical = 1; }
    glViewport(0, 0, horizontal, vertical);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, AspectRatio, 1.0, 40.0);
    if (horizontal <= vertical)
        glViewport(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio);
    else
        glViewport(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void Mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        x_pos_old = x;
        y_pos_old = y;
        status = 1;
    }
    else if (btn = GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        zoom_pos_old = y;
        status = 2;
    }
    else status = 0;
}

void Motion(GLsizei x, GLsizei y)
{
    delta_x = x - x_pos_old;
    delta_y = y - y_pos_old;
    delta_zoom = y - zoom_pos_old;
    x_pos_old = x;
    y_pos_old = y;

    zoom_pos_old = y;
    glutPostRedisplay();
}

void printManual()
{
    std::cout << "'e' --- Wyswietlic jajko" << std::endl;
    std::cout << "'t' --- Wyswietlic czajnik" << std::endl;

    std::cout << "'p' --- Tryb rysowanie przez punkty" << std::endl;
    std::cout << "'l' --- Tryb rysowania przez linije" << std::endl;
    std::cout << "'s' --- Tryb rysowanie przez trojkonty" << std::endl;

    std::cout << "'x' --- Obrot po osi x" << std::endl;
    std::cout << "'y' --- Obrot po osi y" << std::endl;
    std::cout << "'z' --- Obrot po osi y" << std::endl;
}
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    printManual();
    randomColorization();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(300, 300);
    glutCreateWindow("EGG");
    glutReshapeFunc(changeSize);
    glutDisplayFunc(RenderScene);

    glutKeyboardFunc(keys);
    //    glutIdleFunc(spinEgg);
    MyInit();
    glEnable(GL_DEPTH_TEST);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutMainLoop();
}
