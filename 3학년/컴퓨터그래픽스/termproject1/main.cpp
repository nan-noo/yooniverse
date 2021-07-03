#include <stdio.h>
#include <stdlib.h>
#include <glut.h>

int						m_Mouse_Coord[2];	// previous mouse coordinates
unsigned char			m_Mouse_Event[3];	// mouse event handler
float					m_Rotate[2];		// x,y
float					m_Zoom;				// view zoom
float					m_rotate;

void Initialize(void)
{
	m_Zoom = 200;
	//depth test
	glEnable(GL_DEPTH_TEST);
	//Background Color Setting
	glClearColor(0.9f, 0.9, 0.9, 1.0f);

	m_Rotate[0] = 0;
	m_Rotate[1] = 0;

	m_rotate = 0;
}

void kim() {
	glLoadIdentity();

	glTranslatef(-50, 0.0, -m_Zoom);
	glRotatef(m_rotate, 0.0, 1.0, 0.0);
	glTranslatef(50, 0.0, +m_Zoom);

	glTranslatef(0, 0.0, -m_Zoom);
	glRotatef(m_Rotate[0], 1.0, 0.0, 0.0);
	glRotatef(m_Rotate[1], 0.0, 1.0, 0.0);
	glTranslatef(0, 0.0, +m_Zoom);
	
	/*±è*/
	//¤¡
	glPushMatrix();
	glTranslatef(-55, 15.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(2.5, 0.5, 0.5);	//scaling 
	glutWireCube(10);			//draw wire cube with size ten  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-45.0, 10.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(0.5, 1.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();
	//¤Ó
	glPushMatrix();
	glTranslatef(-35.0, 10.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(0.5, 1.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();
	//¤±
	glPushMatrix();
	glTranslatef(-65, -10.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(0.5, 1.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-50, -5.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(3.5, 0.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-35.0, -10.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(0.5, 1.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-50, -15.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(3.5, 0.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();
}

void tae() {
	glLoadIdentity();

	glTranslatef(0, 0.0, -m_Zoom);
	glRotatef(m_rotate, 1.0, 0.0, 0.0);
	glTranslatef(0, 0.0, +m_Zoom);

	glTranslatef(0, 0.0, -m_Zoom);
	glRotatef(m_Rotate[0], 1.0, 0.0, 0.0);
	glRotatef(m_Rotate[1], 0.0, 1.0, 0.0);
	glTranslatef(0, 0.0, +m_Zoom);
	/*ÅÂ*/
	//¤¼
	glPushMatrix();
	glTranslatef(-15.0, 0.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(0.5, 3.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-10.0, 15.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(1.5, 0.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-10.0, 0.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(1.5, 0.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-10.0, -15.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(1.5, 0.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();
	//¤À
	glPushMatrix();
	glTranslatef(5.0, 0.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(0.5, 3.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(10.0, 0.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(1.5, 0.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(15.0, 0.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(0.5, 3.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();
}

void yoon() {
	glLoadIdentity();

	glTranslatef(50.0, 0.0, -m_Zoom);
	glRotatef(m_rotate, 0.0, 0.0, 1.0);
	glTranslatef(-50, 0.0, +m_Zoom);

	glTranslatef(0, 0.0, -m_Zoom);
	glRotatef(m_Rotate[0], 1.0, 0.0, 0.0);
	glRotatef(m_Rotate[1], 0.0, 1.0, 0.0);
	glTranslatef(0, 0.0, +m_Zoom);

	/*À±*/
	//¤·
	glPushMatrix();
	glTranslatef(50.0, 10.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(3.5, 1.5, 2.5);	//scaling 
	glutWireTorus(1, 4, 4, 10);
	glPopMatrix();
	//¤Ð
	glPushMatrix();
	glTranslatef(50, 0.0, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(3.5, 0.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(42.5, -2.5, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(0.5, 1, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(57.5, -2.5, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(0.5, 1, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();
	//¤¤
	glPushMatrix();
	glTranslatef(35, -12.5, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(0.5, 1, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(50, -15, -m_Zoom);
	glColor3f(1.0, 0.0, 0.0);   //color (red color in there) 
	glScalef(3.5, 0.5, 0.5);	//scaling 
	glutWireCube(10);
	glPopMatrix();
}

void display()
{
	//scene buffer clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	//¿øÁ¡
	glLoadIdentity();
	glPointSize(10.0);
	glBegin(GL_POINTS);
	glVertex3f(0, 0, -m_Zoom);
	glEnd();
	
	//±èÅÂÀ±
	kim();
	tae();
	yoon();
	
	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);    //w: is width, h: is height
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w / h, 0.1, 500); // define view frustum
}

void Mouse(int mouse_event, int state, int x, int y)
{
	//mouse x,y coordinate
	m_Mouse_Coord[0] = x;
	m_Mouse_Coord[1] = y;

	switch (mouse_event)
	{
	case GLUT_LEFT_BUTTON:   //check if pressing mouse left button
		m_Mouse_Event[0] = ((GLUT_DOWN == state) ? 1 : 0);
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

	//press mouse left button and drag mouse for changing the rotation value
	if (m_Mouse_Event[0])
	{
		m_Rotate[0] += (float)0.1 * diffy;
		m_Rotate[1] += (float)0.1 * diffx;
	}

	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{	
	//press"q" for exit program
	switch (key)
	{
	case 'q':
	case 'Q':
	case 27: //esc
		exit(0);
		break;
		
	// rotation
	case 'a':
	case 'A':
		m_rotate += 5;
		break;

	//reset all
	case 'r':
	case 'R':
		Initialize();
		break;
	}

	glutPostRedisplay();
}

void KeySpecial(int key, int x, int y) { // arrow key¿¡ µû¶ó rotate
	float delta = 5.0;

	switch (key) {
	case GLUT_KEY_LEFT:
		m_Rotate[1] -= delta;
		break;
	case GLUT_KEY_RIGHT:
		m_Rotate[1] += delta;
		break;
	case GLUT_KEY_UP:
		m_Rotate[0] -= delta;
		break;
	case GLUT_KEY_DOWN:
		m_Rotate[0] += delta;
		break;
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
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(KeySpecial);

	glutMainLoop();

	return 0;
}