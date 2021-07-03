#include <stdio.h>
#include<algorithm>
#include <glut.h>
#include <gl/gl.h>
#include <gl/glu.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include<vector>
#include<iostream>
using namespace std;

int						m_Mouse_Coord[2];	// previous mouse coordinates
unsigned char			m_Mouse_Event[3];	// mouse event handler
float					m_Rotate[2];		// x,y
float					m_Zoom;				// view zoom
float					m_rotate;

bool					light_on0;			// light on-off
bool					light_on1;			// light on-off
bool					doUpdate;			// update or not

float					gravity[3] = { 0.0, -9.8, 0.0 };
float					mass = 1.0;
float					time_step = 0.005f;
float					ground = -50;
float					COR = 0.9;

float					vel[20][3];
float					acc[20][3];
// 김태윤 position
GLfloat					posName[20][3];
GLfloat					radius[20] = {0.5, 1.0, 1.5, 1.0, 0.5, 1.0, 0.5, 3.0, 0.5, 0.5, 0.5, 3.5, 0.5, 3.5, 3.5, 1.5, 0.5, 0.5, 0.5, 1.5}; // 5 곱해야 함

//light position
GLfloat					light_pos[3][3];
GLfloat					light_vel[3][3] = { { 0.0, 5.0, 0.0 }, { 0.0, 5.0, 0.0 }, { 0.0, 5.0, -5.0 }};
GLfloat					light_acc[3][3] = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }};
GLfloat					light_rad = 1;

void draw_my_cube(GLfloat size);
void draw_my_Torus(double r, double c, int rSeg, int cSeg);

float dotProduct(float a[3], float b[3]) {
	float result = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	return result;
}

void updatePhysics(float* pos, float* vel, float* acc, float radius) {
	acc[0] = gravity[0] / mass;
	acc[1] = gravity[1] / mass;
	acc[2] = gravity[2] / mass;

	vel[0] += acc[0] * time_step;
	vel[1] += acc[1] * time_step;
	vel[2] += acc[2] * time_step;

	pos[0] += vel[0] * time_step;
	pos[1] += vel[1] * time_step;
	pos[2] += vel[2] * time_step;

	// collision with ground
	if (pos[1] < ground + radius) { //collision
		//bound
		vel[0] = -1 * COR * vel[0];
		vel[1] = -1 * COR * vel[1];
		vel[2] = -1 * COR * vel[2];

		pos[1] = ground + radius;
	}
}
void detectCollision(float* pos_1, float* vel_1, float* pos_2, float* vel_2, float r1, float r2) {
	float distance = sqrt(pow(pos_1[0] - pos_2[0], 2) + pow(pos_1[1] - pos_2[1], 2) + pow(pos_1[2] - pos_2[2], 2));

	if (distance <= r1 + r2) {
		//collision
		float normal1[3] = { (pos_2[0] - pos_1[0]) / distance, (pos_2[1] - pos_1[1]) / distance, (pos_2[2] - pos_1[2]) / distance };
		float normal2[3] = { (pos_1[0] - pos_2[0]) / distance, (pos_1[1] - pos_2[1]) / distance, (pos_1[2] - pos_2[2]) / distance };

		float vdotn1 = dotProduct(normal1, vel_1);
		float vdotn2 = dotProduct(normal2, vel_2);

		//Vn
		float vn1[3] = { normal1[0] * vdotn1, normal1[1] * vdotn1, normal1[2] * vdotn1 };
		float vn2[3] = { normal2[0] * vdotn2, normal2[1] * vdotn2, normal2[2] * vdotn2 };

		//Vt
		float vt1[3] = { vel_1[0] - vn1[0], vel_1[1] - vn1[1], vel_1[2] - vn1[2] };
		float vt2[3] = { vel_2[0] - vn2[0], vel_2[1] - vn2[1], vel_2[2] - vn2[2] };

		//Vn_n
		float vn1_n[3] = { vn2[0], vn2[1], vn2[2] };
		float vn2_n[3] = { vn1[0], vn1[1], vn1[2] };

		float relPos[3] = { (pos_2[0] - pos_1[0]), (pos_2[1] - pos_1[1]), (pos_2[2] - pos_1[2]) };
		float relVel[3] = { vel_2[0] - vel_1[0], vel_2[1] - vel_1[1], vel_2[2] - vel_1[2] };

		if (dotProduct(relPos, relVel) < 0) {
			//new velocities
			vel_1[0] = vn1_n[0] + vt1[0];
			vel_1[1] = vn1_n[1] + vt1[1];
			vel_1[2] = vn1_n[2] + vt1[2];

			vel_2[0] = vn2_n[0] + vt2[0];
			vel_2[1] = vn2_n[1] + vt2[1];
			vel_2[2] = vn2_n[2] + vt2[2];

			//cout << vel_1[0] << ", " << vel_1[1] << ", " << vel_1[2] << endl;

		}
	}
}

//The Function for Load Texture File
GLuint LoadMeshFromFile(const char* texFile)
{
	GLuint texture;
	glGenTextures(1, &texture);

	FILE* fp = NULL;
	if (fopen_s(&fp, texFile, "rb")) {
		printf("ERROR : No %s. \n fail to bind %d\n", texFile, texture);
		return false;
	}
	int width, height, channel;
	unsigned char* image = stbi_load_from_file(fp, &width, &height, &channel, 4);
	fclose(fp);

	//bind
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(image);

	return texture;
}
GLint myPhoto, top, bottom, back, front, leftTex, rightTex, ground_grass, jewelry; // textures

void Initialize(void)
{
	m_Zoom = 180;
	//depth test
	glEnable(GL_DEPTH_TEST);
	//Background Color Setting
	glClearColor(0.95f, 0.95, 0.95, 1.0f);

	m_Rotate[0] = 0;
	m_Rotate[1] = 0;

	m_rotate = 0;

	doUpdate = false;

	// init light
	light_on0 = true;
	light_on1 = true;
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	// Loading Texture file example:
	myPhoto = LoadMeshFromFile("textures/photo.png");
	ground_grass = LoadMeshFromFile("textures/ground-grass.jpg");
	jewelry = LoadMeshFromFile("textures/jewelry.jpg");
	top = LoadMeshFromFile("textures/skybox/top.jpg");
	bottom = LoadMeshFromFile("textures/skybox/bottom.jpg");
	back = LoadMeshFromFile("textures/skybox/back.jpg");
	front = LoadMeshFromFile("textures/skybox/front.jpg");
	leftTex = LoadMeshFromFile("textures/skybox/left.jpg");
	rightTex = LoadMeshFromFile("textures/skybox/right.jpg");
	
	// Physics
	GLfloat temp[20][3] = {
		{-55.0, 15.0, 0}, //ㄱ
		{-45.0, 7.5, 0},
		{-35.0, 10.0, 0}, //ㅣ
		{-65, -12.5, 0}, //ㅁ
		{-50, -5.0, 0},
		{-35.0, -12.5, 0},
		{-50, -15.0, 0},
		{-15.0, -2.5, 0}, //ㅌ
		{-10.0, 15.0, 0},
		{-7.5, 0.0, 0},
		{-7.5, -15.0, 0},
		{5.0, 0.0, 0}, //ㅐ
		{10.0, 0.0, 0},
		{15.0, 0.0, 0},
		{50.0, 10.0, 0}, //ㅇ
		{50, 0.0, 0}, //ㅠ
		{42.5, -5.0, 0},
		{57.5, -5.0, 0},
		{35, -10.0, 0}, //ㄴ
		{50, -15, 0}
	};
	GLfloat temp_l[3][3] = {
		{-60, 20, 50},
		{60, 20, 50},
		{0 , 0, 0}
	};

	for (int i = 0; i < 20; i++) {
		posName[i][0] = temp[i][0];
		posName[i][1] = temp[i][1];
		posName[i][2] = temp[i][2];
		vel[i][0] = 0;
		vel[i][1] = 10;
		vel[i][2] = -10;
		acc[i][0] = 0;
		acc[i][1] = 0;
		acc[i][2] = 0;
	}

	for (int i = 0; i < 3; i++) {
		light_pos[i][0] = temp_l[i][0];
		light_pos[i][1] = temp_l[i][1];
		light_pos[i][2] = temp_l[i][2];
	}
}

void kim() {
	GLfloat sizK[7][3] = {
		{2.5, 0.5, 0.5}, //ㄱ
		{0.5, 1.0, 0.5},
		{0.5, 1.5, 0.5}, //ㅣ
		{0.5, 1.0, 0.5}, //ㅁ
		{3.5, 0.5, 0.5},
		{0.5, 1.0, 0.5},
		{2.5, 0.5, 0.5}
	};

	for (int i = 0; i < 7; i++) {
		glPushMatrix();
		if (!doUpdate) glRotatef(m_rotate, 1, 0, 0);
		glTranslatef(posName[i][0], posName[i][1], posName[i][2]);
		glScalef(sizK[i][0], sizK[i][1], sizK[i][2]);
		draw_my_cube(10);	 
		glPopMatrix();
	}
}

void tae() {
	GLfloat sizT[7][3] = {
		{0.5, 3.0, 0.5}, //ㅌ
		{1.5, 0.5, 0.5},
		{1.0, 0.5, 0.5}, 
		{1.0, 0.5, 0.5}, 
		{0.5, 3.5, 0.5}, //ㅐ
		{0.5, 0.5, 0.5},
		{0.5, 3.5, 0.5}
	};

	for (int i = 0; i < 7; i++) {
		glPushMatrix();
		if (!doUpdate) glRotatef(m_rotate, 0, 1, 0);
		glTranslatef(posName[i + 7][0], posName[i + 7][1], posName[i + 7][2]);
		glScalef(sizT[i][0], sizT[i][1], sizT[i][2]);
		draw_my_cube(10);
		glPopMatrix();
	}
}

void yoon() {
	GLfloat sizY[6][3] = {
		{3.5, 1.5, 2.5}, //ㅇ
		{3.5, 0.5, 0.5}, //ㅠ
		{0.5, 0.5, 0.5},
		{0.5, 0.5, 0.5},
		{0.5, 0.5, 0.5}, //ㄴ
		{3.5, 0.5, 0.5}
	};

	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		glTranslatef(50, 0, 0);
		if (!doUpdate) glRotatef(-m_rotate, 0, 0, 1);
		glTranslatef(posName[i + 14][0] -50, posName[i + 14][1], posName[i + 14][2]);
		glScalef(sizY[i][0], sizY[i][1], sizY[i][2]);
		if (i == 0) draw_my_Torus(1, 4, 4, 10);
		else draw_my_cube(10);
		glPopMatrix();
	}
}

void light() {
	// spotlight0
	GLfloat light_pos0[] = { light_pos[0][0], light_pos[0][1], light_pos[0][2], 1.0 };
	GLfloat light_dir0[] = { 10, -10, -20 };
	GLfloat light_ambient0[] = { 0.0, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse0[] = { 0.5, 0.0, 0.5, 1.0 };
	GLfloat light_specular0[] = { 1, 1, 0, 1.0 };

	// spotlight1
	GLfloat light_pos1[] = { light_pos[1][0], light_pos[1][1], light_pos[1][2], 1.0 };
	GLfloat light_dir1[] = { -10, -10, -20 };
	GLfloat light_ambient1[] = { 0.5, 0.0, 0.5, 1.0 };
	GLfloat light_diffuse1[] = { 0.5, 0.5, 0.0, 1.0 };
	GLfloat light_specular1[] = { 0, 1, 1, 1.0 };

	// directional light
	/*GLfloat light_pos2[] = { -70, 100, -5, 0.0 };
	GLfloat light_ambient2[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse2[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_specular2[] = { 1, 1, 1, 1.0 };*/

	// point light
	GLfloat light_pos2[] = { light_pos[2][0], light_pos[2][1], light_pos[2][2], 1.0 };
	GLfloat light_ambient2[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse2[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_specular2[] = { 1, 0, 1, 1.0 };

	// 광원 속성 전달(광원, 속성, 배열)
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_dir0);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 20);

	glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_dir1);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 20);

	/*glLightfv(GL_LIGHT2, GL_POSITION, light_pos2);
	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);*/

	glLightfv(GL_LIGHT2, GL_POSITION, light_pos2);
	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);

	float noMat[] = { 0, 0, 0, 1 };
	float matAmb[] = { 0.3, 0.3, 0.3, 1 };
	float matDif[] = { 0.5, 0.5, 0.5, 1 };
	float matSpec[] = { 0.5, 0.5, 0.5, 1 };
	float matShininess = 76.8;

	glMaterialfv(GL_FRONT, GL_EMISSION, noMat);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, matShininess);

	// spotlight 위치
	glPushMatrix();
	glTranslatef(light_pos0[0], light_pos0[1], light_pos0[2]);
	glColor3f(1.0, 1.0, 0.0);
	glutSolidSphere(1, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(light_pos1[0], light_pos1[1], light_pos1[2]);
	glColor3f(0.0, 1.0, 1.0);
	glutSolidSphere(1, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(light_pos2[0], light_pos2[1], light_pos2[2]);
	glColor3f(1.0, 0.0, 0.0);
	glutSolidSphere(1, 10, 10);
	glPopMatrix();
}

void drawBG(float size) {
	//Vertices Position. Size for Scaling the cube
	GLfloat v[8][3] = {
		{size, ground, size},
		{-size, ground, size},
		{-size, size, size},
		{size, size, size},
		{size, ground, -size},
		{-size, ground, -size},
		{-size, size, -size},
		{size, size, -size}
	};

	// Face Element index
	GLint faces[6][4] = {
		{0,1,2,3},
		{5,4,7,6},
		{3,2,6,7},
		{4,5,1,0},
		{4,0,3,7},
		{1,5,6,2}
	};

	GLint textures[6] = {front, back, top, bottom, rightTex, leftTex};

	//Drawing Cube
	for (int i = 0; i < 6; i++) {
		// One Cube has 6 quads
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); //uv coordinate
		glVertex3fv(&v[faces[i][0]][0]);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv(&v[faces[i][1]][0]);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(&v[faces[i][2]][0]);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
		glPopMatrix();
	}
}

void starLight() {
	float noMat[] = { 1, 1, 1, 1 };
	float matShiness = 100;

	// 빛나는 sphere를 랜덤하게 생성해서 별 같은 효과
	for (int i = 0; i < 2; i++) {
		int x = rand() % 400 - 200; // -200 ~ 200
		int y = rand() % 100 + 75; // 75~
		int z = rand() % 400 - 200; // -200 ~ 200
		glPushMatrix();
		glTranslatef(x, y, z);
		glColor3f(1.0, 1.0, 1.0);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noMat); // make sphere glow
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShiness);
		glutSolidSphere(1.5, 10, 10);
		glPopMatrix();
	}
}

void display()
{
	//scene buffer clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Transformation matrix initialize 

	//according to "translation" and "rotation" adjust the  position and angle
	glTranslatef(0.0, 0.0, -m_Zoom);
	//value m_Rotate changing by mouse (ref: mouse() and motion() function)
	glRotatef(m_Rotate[0], 1.0, 0.0, 0.0);
	glRotatef(m_Rotate[1], 0.0, 1.0, 0.0);

	// star effect
	starLight();
	
	glEnable(GL_TEXTURE_2D);

	//light 0, 1, 2
	light();
	
	//*******************Drawing your characters*******************	
	glBindTexture(GL_TEXTURE_2D, myPhoto);
	kim();
	glBindTexture(GL_TEXTURE_2D, ground_grass);
	tae();
	glBindTexture(GL_TEXTURE_2D, jewelry);
	yoon();
	
	// background
	drawBG(200);

	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w / h, 0.1, 500); // define view frustum
	glViewport(0, 0, w, h);    //w: is width, h: is height 
}

void Mouse(int mouse_event, int state, int x, int y)
{
	//mouse x,y coordinate
	m_Mouse_Coord[0] = x;
	m_Mouse_Coord[1] = y;

	switch (mouse_event)
	{
	case GLUT_LEFT_BUTTON:
		m_Mouse_Event[0] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_MIDDLE_BUTTON:
		m_Mouse_Event[1] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_RIGHT_BUTTON:
		m_Mouse_Event[2] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void MouseMotion(int x, int y)
{
	//difference of mouse' previous coordinate and current coordinate
	int diffx = x - m_Mouse_Coord[0];
	int diffy = y - m_Mouse_Coord[1];

	m_Mouse_Coord[0] = x;
	m_Mouse_Coord[1] = y;

	if (m_Mouse_Event[0])
	{
		m_Rotate[0] += (float)0.1 * diffy;
		m_Rotate[1] += (float)0.1 * diffx;
	}
	else if (m_Mouse_Event[2]) //right button for zoom in/zoom out
	{
		m_Zoom += (float)0.1 * diffy;
	}

	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	//press"q" for exit program
	switch (key){
		case 'q':
		case 'Q':
		case 27: //esc
			exit(0);
			break;
		
		// light on-off
		case '1':
			if (light_on0 == false) {
				light_on0 = true;
				glEnable(GL_LIGHT0);
			}
			else {
				light_on0 = false;
				glDisable(GL_LIGHT0);
			}
			break;
		case '2':
			if (light_on1 == false) {
				light_on1 = true;
				glEnable(GL_LIGHT1);
			}
			else {
				light_on1 = false;
				glDisable(GL_LIGHT1);
			}
			break;
		// physics animation start
		case 'a':
		case 'A':
			if (!doUpdate) {
				doUpdate = true;
			}
			break;
		//reset all
		case 'r':
		case 'R':
			Initialize();
			break;
	}
	glutPostRedisplay();
}

void draw_my_cube(GLfloat size) {
	//Vertices Position. Size for Scaling the cube
	GLfloat v[8][3] = {
		{0.500000 * size, -0.500000 * size, 0.500000 * size},
		{-0.500000 * size, -0.500000 * size, 0.500000 * size},
		{-0.500000 * size, 0.500000 * size, 0.500000 * size},
		{0.500000 * size, 0.500000 * size, 0.500000 * size},
		{0.500000 * size, -0.500000 * size, -0.500000 * size},
		{-0.500000 * size, -0.500000 * size, -0.500000 * size},
		{-0.500000 * size, 0.500000 * size, -0.500000 * size},
		{0.500000 * size, 0.500000 * size, -0.500000 * size}
	};
	// Face Normal
	GLfloat n[6][3] = {
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f}
	};
	// Face Element index
	GLint faces[6][4] = {
		{0,1,2,3},
		{7,6,5,4},
		{3,2,6,7},
		{1,0,4,5},
		{0,4,7,3},
		{2,6,5,1}
	};

	// Vertex Normal -> 나누기3해야함
	GLfloat vn[8][3] = {
		{1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f}
	};

	//Drawing Cube
	for (int i = 0; i < 6; i++) {
		// One Cube has 6 quads
		glBegin(GL_QUADS);
		glNormal3fv(&n[i][0]); //face normal
		glTexCoord2f(1.0f, 1.0f); //uv coordinate
		glVertex3fv(&v[faces[i][0]][0]);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv(&v[faces[i][1]][0]);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(&v[faces[i][2]][0]);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
	
	// 모양 이상
	//for (int i = 0; i < 6; i++){
	//	glBegin(GL_QUAD_STRIP);
	//	
	//	for (int j = 0; j < 4; j++)
	//	{
	//		//uv coordinate
	//		//glTexCoord2f(1.0f, 1.0f);
	//		//Vertex normal  
	//		glNormal3f(vn[faces[i][j]][0], vn[faces[i][j]][1], vn[faces[i][j]][2]);
	//		glVertex3f(v[faces[i][j]][0], v[faces[i][j]][1], v[faces[i][j]][2]);
	//	}
	//	glEnd();
	//}
}

void draw_my_Torus(double r, double c, int rSeg, int cSeg) {
	// r: inner radius, c:outer radius
	// rSeg and cSeg: number of side and ring's segment
	const double PI = 3.1415926;
	const double TAU = 2 * PI;
	vector<float*> torus_v;		//torus's vertex (position)
	vector<float*> torus_v_n;	//torus's vertex  normal
	vector<int*> torus_f;		//torus's face  (0,1,2,3), ...
	vector<float*> torus_n;		//torus's face normal
	vector<float*> uv;			//torus's uv coordinate 

	// Calculte Torus Postion
	for (int i = 0; i < rSeg; i++) {
		for (int j = 0; j <= cSeg; j++) {
			for (int k = 0; k <= 1; k++) {
				float s = (i + k) % rSeg + 0.5;
				float t = j % (cSeg + 1);
				float x = (c + r * cos(s * TAU / rSeg)) * cos(t * TAU / cSeg);
				float y = (c + r * cos(s * TAU / rSeg)) * sin(t * TAU / cSeg);
				float z = r * sin(s * TAU / rSeg);
				float* new_ver = new float[3];
				new_ver[0] = (float)x, new_ver[1] = (float)y, new_ver[2] = (float)z;
				torus_v.push_back(new_ver); //save vertex position

				float u = (i + k) / (float)rSeg;
				float v = j / (float)cSeg;
				float* uv_coord = new float[2];
				uv_coord[0] = u, uv_coord[1] = v;
				uv.push_back(uv_coord);  //save uv coordinate 
			}
		}
	}

	// save the face information(vertices index)
	for (int i = 0; i < torus_v.size() - 3; i += 2)
	{	
		if ((i / 2 + 1) % (cSeg + 1) == 0) continue; // 예외처리
		int* new_ver = new int[4];
		new_ver[0] = i, new_ver[1] = i + 1, new_ver[2] = i + 2, new_ver[3] = i + 3;
		torus_f.push_back(new_ver);
	}

	// *******************do something here!!******************* 
	// calculate face normal
	for (int i = 0; i < torus_v.size() - 3; i += 2)
	{
		if ((i / 2 + 1) % (cSeg + 1) == 0) continue;
		float* normal = new float[3];

		//three position in one face
		float p1[3] = { torus_v[i][0],torus_v[i][1],torus_v[i][2] };
		float p2[3] = { torus_v[i + 1][0],torus_v[i + 1][1],torus_v[i + 1][2] };
		float p3[3] = { torus_v[i + 2][0],torus_v[i + 2][1],torus_v[i + 2][2] };

		//*************calculate the face normal in there************* 
		//hint: using the cross product   
		float u[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
		float v[3] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
		//calcute the normal value(normal[0]:x,normal[1]:y,normal[2]:z) 
		normal[0] = u[1] * v[2] - u[2] * v[1];
		normal[1] = u[2] * v[0] - u[0] * v[2];
		normal[2] = u[0] * v[1] - u[1] * v[0];
		//save the face normal
		torus_n.push_back(normal);

		//cout << i << ", " << normal[0] << ", " << normal[1] << ", " << normal[2] << "입니다." << endl;
	}

	// initialize the size of vertex normal(torus_v_n), the size same as vertex
	for (int i = 0; i < torus_v.size(); i++){ // 0~87
		float* new_ver = new float[3];
		int v_index[4] = { 0, };
		int face[4] = { 0, };
		int num_vi = 0;

		if ((i / 2 + 1) % (cSeg + 1) == 0) {
			new_ver[0] = torus_v_n[i - cSeg * 2][0];
			new_ver[1] = torus_v_n[i - cSeg * 2][1];
			new_ver[2] = torus_v_n[i - cSeg * 2][2];

			torus_v_n.push_back(new_ver);
			continue;
		}

		// 현재 index와 같은 position의 vertex index 찾기
		for (int j = 0; j < torus_v.size(); j++) {
			if (torus_v[i][0] == torus_v[j][0] && torus_v[i][1] == torus_v[j][1] && torus_v[i][2] == torus_v[j][2]) {
				v_index[num_vi] = j;
				num_vi++;
				//cout << j << endl;
			}
		}
		//cout << endl;
		// 그 vertex index들이 포함된 face index다 찾기
		int num_fi = 0;
		for (int r = 0; r < num_vi; r++) {
			for (int j = 0; j < torus_f.size(); j++) {
				if (torus_f[j][0] == v_index[r] || torus_f[j][1] == v_index[r] || torus_f[j][2] == v_index[r] || torus_f[j][3] == v_index[r]) {
					face[num_fi] = j;
					num_fi++;
					//cout << j << endl;
				}
			}
		}
		//cout << endl;
		if (num_fi < 4) { // 2개만 있는 경우 - torus ring의 끝끼리 만나는 부분의 vertex
			//cout << i << endl;
			//cout << num_fi << endl;
			face[2] = face[0] + cSeg - 1;
			face[3] = face[1] + cSeg - 1;
		} 
		
		// 구한 face index의 face normal을 이용해서 vertex normal구하기
		float n1[3] = { torus_n[face[0]][0], torus_n[face[0]][1], torus_n[face[0]][2] };
		float n2[3] = { torus_n[face[1]][0], torus_n[face[1]][1], torus_n[face[1]][2] };
		float n3[3] = { torus_n[face[2]][0], torus_n[face[2]][1], torus_n[face[2]][2] };
		float n4[3] = { torus_n[face[3]][0], torus_n[face[3]][1], torus_n[face[3]][2] };

		new_ver[0] = (n1[0] + n2[0] + n2[0] + n3[0]) / 4;
		new_ver[1] = (n1[1] + n2[1] + n2[1] + n3[1]) / 4;
		new_ver[2] = (n1[2] + n2[2] + n2[2] + n3[2]) / 4;
		
		/*new_ver[0] = (float)0;
		new_ver[1] = (float)0;
		new_ver[2] = (float)0;*/
		torus_v_n.push_back(new_ver);
	}

	//*************calculate the vertex normal in there*************
	//Hint 1: The vertex normal can be calculated using the average of the adjacent face normals
	//Hint 2: In there one vertex(same id) has two adjacent faces   
	//Hint 3: Also need finding the adjacent faces of vertex which in the same position but has a different Id 

	//cout << torus_f.size() << ", " << torus_n.size() << ", " << uv.size() << ", " << torus_v_n.size() << ", " << torus_v.size() << "입니다." << endl; //40, 88, 88, 88
	// Drawing Torus
	for (int i = 0; i < torus_f.size(); i++) // 0~39
	{
		glBegin(GL_QUAD_STRIP);
		//face normal 
		//glNormal3f(-1 * torus_n[i][0], -1 * torus_n[i][1], -1 * torus_n[i][2]);
		for (int j = 0; j < 4; j++)
		{
			//uv coordinate
			glTexCoord2d(uv[torus_f[i][j]][0], uv[torus_f[i][j]][1]);
			//Vertex normal  
			glNormal3f(-1 * torus_v_n[torus_f[i][j]][0], -1 * torus_v_n[torus_f[i][j]][1], -1 * torus_v_n[torus_f[i][j]][2]);   
			glVertex3f(torus_v[torus_f[i][j]][0], torus_v[torus_f[i][j]][1], torus_v[torus_f[i][j]][2]);
		}
		glEnd();
	}
}

void Update() {
	//Code your motion:
	//1.: update the position by physical based animation
	//2. Collision detection and response

	if (doUpdate) {
		updatePhysics(light_pos[0], light_vel[0], light_acc[0], light_rad);
		updatePhysics(light_pos[1], light_vel[1], light_acc[1], light_rad);
		updatePhysics(light_pos[2], light_vel[2], light_acc[2], light_rad);
		for (int i = 0; i < 20; i++) {
			detectCollision(posName[i], vel[i], light_pos[0], light_vel[0], 5 * radius[i], light_rad);
			detectCollision(posName[i], vel[i], light_pos[1], light_vel[1], 5 * radius[i], light_rad);
			detectCollision(posName[i], vel[i], light_pos[2], light_vel[2], 5 * radius[i], light_rad);
			updatePhysics(posName[i], vel[i], acc[i], 5 * radius[i]);
			for (int j = i; j < 20; j++) { //collision with others
				detectCollision(posName[i], vel[i], posName[j], vel[j], 5 * radius[i], 5 * radius[j]);
			}
		}
	}
	else {
		// 글자 각각 돌아가는 애니메이션
		m_rotate += 0.3;
		if (m_rotate >= 360) m_rotate = 0;
	}

	glutPostRedisplay();
}

int main(int argc, char* argv[]) {

	//Window generation
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(400, 100); //Window position
	glutInitWindowSize(1024, 576); //Window size
	glutCreateWindow("OpenGL"); //Window name 
	//Initialize function
	Initialize();
	//callback function 
	glutDisplayFunc(display);
	glutIdleFunc(Update);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutKeyboardFunc(Keyboard);

	glutMainLoop();
	return 0;
}

