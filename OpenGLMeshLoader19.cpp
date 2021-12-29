#include <math.h>
#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <iostream>
using namespace std;

int WIDTH = 1280;
int HEIGHT = 720;
double moveX = 0;
double moveZ = 0;
double playerRotate = 0;
#define DEG2RAD(a) (a * 0.0174532925)
float x = 0;
float m = 0.005;
float z = 0;
GLuint tex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;
class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};
Vector Eye(20, 5, 20);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);
class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};
class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 12.085f, float eyeY = 7.59875f, float eyeZ = 17.0245f, float centerX = 11.7239f, float centerY = 7.42656f, float centerZ = 16.108f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	/*Camera(float eyeX = 0.02f, float eyeY = 0.02f, float eyeZ = 0.02f, float centerX = 2.4f, float centerY = 12.8166f, float centerZ = 38.4664f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}*/
	//Camera(float eyeX = 0.0f, float eyeY = 2.0f, float eyeZ = 8.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
	//	eye = Vector3f(eyeX, eyeY, eyeZ);
	//	center = Vector3f(centerX, centerY, centerZ);
	//	up = Vector3f(upX, upY, upZ);
	//}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};
Camera camera;
int cameraZoom = 0;
// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_women;
Model_3DS model_sink;
Model_3DS model_oven;
Model_3DS model_kitchen;
//Model_3DS model_table;

// Textures
GLTexture tex_ground;
GLTexture tex_girl;
GLTexture tex_sink;

void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture


	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}
void drawPlayer() {
	model_women.Draw();
}
void myDisplay(void)
{
	setupCamera();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glPushMatrix();


	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	// Draw Ground
	RenderGround();

	// Draw Tree Model
	glPushMatrix();
	glTranslatef(10, 0, 0);
	glScalef(0.7, 0.7, 0.7);
	model_tree.Draw();
	glPopMatrix();

	// Draw house Model
	glPushMatrix();
	glRotatef(90.f, 1, 0, 0);
	//glRotatef(90.f, 0, 0, 1);
	glScalef(3, 3, 3);
	model_house.Draw();
	glPopMatrix();

	//glPushMatrix();
	//glTranslated(-7.56008, 3.5, 0);
	//glRotatef(90.f, 0, 1, 0);
	//glScalef(0.003, 0.003, 0.003);
	////model_sink.Draw();
	//glPopMatrix();

	//glPushMatrix();
	//glTranslated(-0.16, 0, -4.17);
	////glRotatef(90.f, 0, 1, 0);
	//glScalef(0.035, 0.035, 0.035);
	////model_oven.Draw();
	//glPopMatrix();

	glPushMatrix();
	glTranslated(-6.27005, 3.5, 0.0999996);
	glRotatef(-90.f, 1, 0, 0);
	glScalef(0.0003, 0.0003, 0.0003);
	model_kitchen.Draw();
	glPopMatrix();


	// woman
	glPushMatrix();
	glTranslatef(moveX, 0, moveZ);
    glRotatef(playerRotate, 0, 1, 0);
	glScalef(0.02, 0.02, 0.02);
	drawPlayer();
	glPopMatrix();

	

	//sky box
	glPushMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);


	glPopMatrix();

//	glPopMatrix();

	glutSwapBuffers();
}
void myKeyboard(unsigned char button, int x1, int y)
{
	float d = 0.2;
	switch (button)
	{		
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;

	/*case 't':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;*/
	case 'j':
		moveX -= 0.03;
		//camera.eye.x -= 0.03;
		playerRotate = 180;
		//camera.rotateX(180);
		/*camera.eye.x = 2.77893;
		camera.eye.y = 4.78448;
		camera.eye.z = -1.01958;
		camera.center.x = 1.82297;
		camera.center.y = 4.70029;
		camera.center.z = -0.738441;*/
		break;
	case 'l':
		moveX += 0.03;
		//camera.eye.x += 0.03;
		playerRotate = 0;
		break;
	case 'i':
		moveZ -= 0.03;
		playerRotate = 90;
		/*camera.eye.z = 0;
		camera.eye.x = 0;
		camera.eye.y = 0;*/
		camera.rotateY(90);
		//camera.eye.z -= 0.03;
		/*camera.eye.x = -2.46162;
		camera.eye.y = 4.31299;
		camera.eye.z = 0.938554;*/
		/*camera.eye.x = 0.934596;
		camera.eye.y = 4.99004;
		camera.eye.z = 3.65498;*/
		/*camera.center.x = 0.704178;
		camera.center.y = 4.90582;
		camera.center.z = 2.68554;*/
		//camera.rotateX(90);
		break;
	case 'k':
		moveZ += 0.03;
		playerRotate = -90;
		//camera.eye.z += 0.03;
		//camera.rotateX(-90);
		/*camera.eye.x = -1.16953;
		camera.eye.y = 4.78448;
		camera.eye.z = -3.05119;
		camera.center.x = -0.933608;
		camera.center.y = 4.70156;
		camera.center.z = -2.08296;*/
		break;
	case 'f': //first-person perspective
		camera.eye.x = 0.302043;
		camera.eye.y = 4.1616;
		camera.eye.z = -0.245676;
		camera.center.x = 1.25181;
		camera.center.y = 4.1079;
		camera.center.z = -0.553977;
		break;
	case 't': //third-person perspective
		camera.eye.x = -2.46162;
		camera.eye.y = 4.31299;
		camera.eye.z = 0.938554;
		camera.center.x = -1.53194;
		camera.center.y = 4.22641;
		camera.center.z = 0.58052;
		break;
	case 'n':
		x+=0.01;
		cout << "x" << x << endl;
		break;
	case 'm':
		z += 0.01;
		cout << "z"  << z << endl;
		break;
	case 'b':
		x -= 0.01;
		cout << "x" << x << endl;
		break;
	case 'v':
		z -= 0.01;
		cout << "z" << z << endl;
		break;
	/*case 'b':
		m -= 0.001;
		cout << m << endl;
		break;*/
	case 27:
		exit(0);
		break;
	default:
		break;
	}

	cout << "camera.eye.x = " << camera.eye.x << ";" << endl;
	cout << "camera.eye.y = " << camera.eye.y << ";" << endl;
	cout << "camera.eye.z = " << camera.eye.z << ";" << endl;

	cout << "camera.center.x = " << camera.center.x << ";" << endl;
	cout << "camera.center.y = " << camera.center.y << ";" << endl;
	cout << "camera.center.z = " << camera.center.z << ";" << endl;

	glutPostRedisplay();
}
void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}
//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}
//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}
//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}
//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_women.Load("Models/women2/girl.3ds");
	model_kitchen.Load("Models/kitchen/kitchen.3ds");
	//model_sink.Load("Models/kitchensink/sink.3ds");
	//model_oven.Load("Models/oven/gasstove.3DS");
	//model_table.Load("Models/table/Table.3ds");
	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
	//tex_girl.Load("Textures/BWom0005.bmp");
	tex_sink.Load("Textures/sink.bmp");
	//loadBMP()
}
//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(Special);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}