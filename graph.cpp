#include <string.h>

#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include "ghostbusters_and_ghosts.cpp"
#include <string>

// Type definitions
typedef std::string          string;
typedef std::vector<GLfloat> vector;
typedef std::vector<string>  vectorStr;

// Function declarations
static void resize(int, int), display(void), key(unsigned char, int, int),
            idle(void), init(void);
void        specialKeys(int, int, int), loadObject(char*);
int         getNumber(string, int);

// Global variables
bool          convexHullFlag   = false;
bool          pairsFlag        = false;
GLfloat       translate        = 0.f;
GLfloat       anglex           = 0.f;
GLfloat       angle            = 0.f;
const GLfloat light_ambient[]  = { 0.0f, 0.0f,  0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f,  1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f,  1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f,  5.0f, 0.0f };
const GLfloat mat_ambient[]    = { 0.7f, 0.7f,  0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f,  0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f,  1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

vectorP busters, ghosts;

// The starting point of the program. Initialization of OpenGL occurs here, as
// well as setting up the callbacks.
int main(int argc, char** argv) {
  glutInit(&argc, argv);

  glutInitWindowSize(800,600);
  glutInitWindowPosition(10,10);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  glutCreateWindow("Ghostbusters and Ghosts");
  init();
  glutReshapeFunc(resize);
  glutDisplayFunc(display);
  glutKeyboardFunc(key);
  glutSpecialFunc(specialKeys);

  glutIdleFunc(idle);
  glutMainLoop();
  
  return 0;
}

static void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  GLfloat factor = 2.f;
  
  glTranslatef(-15.f, -10.f, 0.f);
  
  // draw the ghostbusters
  glColor3f(1.f, 1.f, 1.f);
  glPointSize(10.f);
  for (unsigned int i = 0; i < busters.size(); i++) {
    glBegin(GL_POINTS);
      glVertex2f(busters[i].x * factor, busters[i].y * factor);
    glEnd();
  }
  
  // draw the ghosts
  glColor3f(0.f, 1.f, 0.f);
  for (unsigned int i = 0; i < ghosts.size(); i++) {
    glBegin(GL_POINTS);
      glVertex2f(ghosts[i].x * factor, ghosts[i].y * factor);
    glEnd();
  }
  
  // convex hull
  if (convexHullFlag && busters.size() > 1) {
    vectorP comb;
    for (unsigned int i = 0; i < busters.size(); i++) {
      comb.push_back(busters[i]);
    }
    for (unsigned int i = 0; i < ghosts.size(); i++) {
      comb.push_back(ghosts[i]);
    }
    
    stackP ch = grahamsScan(comb);
    
    pair p0 = ch.top();
    pair pf = p0;
    ch.pop();
    pair p1 = ch.top();
    ch.pop();
    
    glColor3f(0.f, 0.f, 1.f);
  
    glBegin(GL_LINES);
      glVertex2f(p0.x * factor, p0.y * factor);
      glVertex2f(p1.x * factor, p1.y * factor);
    glEnd();
    
    while (!ch.empty()) {
      p0 = p1;
      p1 = ch.top();
      ch.pop();
    
      glBegin(GL_LINES);
        glVertex2f(p0.x * factor, p0.y * factor);
        glVertex2f(p1.x * factor, p1.y * factor);
      glEnd();
    }
    
    glBegin(GL_LINES);
      glVertex2f(p1.x * factor, p1.y * factor);
      glVertex2f(pf.x * factor, pf.y * factor);
    glEnd();
  }
  
  // draw the pairs
  if (pairsFlag && busters.size() > 0) {
    vectorP pairs = findPairs(busters, ghosts);
    
    glColor3f(1.f, 0.f, 0.f);
    
    for (unsigned int i = 0; i < pairs.size() / 2; i++) {
      glBegin(GL_LINES);
        glVertex2f(pairs[2*i+0].x * factor, pairs[2*i+0].y * factor);
        glVertex2f(pairs[2*i+1].x * factor, pairs[2*i+1].y * factor);
      glEnd();
    }
  }

  // Swap the buffers
  glutSwapBuffers();
}

// Callback function that handles resizing of the window.
static void resize(int width, int height) {
  double ratio;

  if (width <= height) {
    glViewport(0, (GLsizei) (height-width) / 2,
              (GLsizei) width, (GLsizei) width);
    ratio = height / width;
  }
  else {
    glViewport((GLsizei) (width - height) / 2, 0,
               (GLsizei) height, (GLsizei) height);
    ratio = width / height;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective (60.f, ratio, 0.1f, 50.f);
}

// Callback function to handle key inputs. Users can quit the program using the
// ESC or Q key.
static void key(unsigned char key, int x, int y) {
  switch (key) {
    // ESC or Q, quit the program
    case 27 :
    case 'q':
      exit(0);
      break;
      
    case 'c':
      convexHullFlag = !convexHullFlag;
      break;
      
    case 'p':
      pairsFlag = !pairsFlag;
      break;
      
    case '0':
      busters.clear();
      ghosts.clear();
      busters.push_back(pair(2,7));
      ghosts.push_back(pair(6,4));
      break;

    case '1':
      busters.clear();
      ghosts.clear();
      busters.push_back(pair(9,3));
      ghosts.push_back(pair(1,1));
      break;

    case '2':
      busters.clear();
      ghosts.clear();
      busters.push_back(pair(2,6));
      busters.push_back(pair(8,8));
      ghosts.push_back(pair(5,3));
      ghosts.push_back(pair(4,10));
      break;

    case '3':
      busters.clear();
      ghosts.clear();
      busters.push_back(pair(2, 6));
      busters.push_back(pair(5, 3));
      ghosts.push_back(pair(8, 8));
      ghosts.push_back(pair(4, 10));
      break;

    case '4':
      busters.clear();
      ghosts.clear();
      busters.push_back(pair(2, 6));
      busters.push_back(pair(8, 8));
      ghosts.push_back(pair(5, 3));
      ghosts.push_back(pair(6, 6));
      break;

    case '5':
      busters.clear();
      ghosts.clear();
      busters.push_back(pair(4,5));
      busters.push_back(pair(5,9));
      busters.push_back(pair(10,4));
      ghosts.push_back(pair(3,2));
      ghosts.push_back(pair(7,5));
      ghosts.push_back(pair(2,6));
      break;

    case '6':
      busters.clear();
      ghosts.clear();
      busters.push_back(pair(2, 2));
      busters.push_back(pair(10, 3));
      busters.push_back(pair(4, 10));
      ghosts.push_back(pair(5, 3));
      ghosts.push_back(pair(4, 5));
      ghosts.push_back(pair(6, 5));
      break;

    case '7':
      busters.clear();
      ghosts.clear();
      busters.push_back(pair(2,1));
      busters.push_back(pair(5,3));
      busters.push_back(pair(6,1));
      busters.push_back(pair(8,9));
      ghosts.push_back(pair(10,2));
      ghosts.push_back(pair(3,5));
      ghosts.push_back(pair(9,2));
      ghosts.push_back(pair(7,4));
      break;

    case '8':
      busters.clear();
      ghosts.clear();
      busters.push_back(pair(1,1));
      busters.push_back(pair(5,3));
      busters.push_back(pair(6,13));
      busters.push_back(pair(8,9));
      busters.push_back(pair(10,2));
      ghosts.push_back(pair(3,5));
      ghosts.push_back(pair(9,8));
      ghosts.push_back(pair(7,6));
      ghosts.push_back(pair(12,10));
      ghosts.push_back(pair(15,7));
      break;

    case '9':
      busters.clear();
      ghosts.clear();
      busters.push_back(pair(1,1));
      busters.push_back(pair(1,6));
      busters.push_back(pair(8,9));
      busters.push_back(pair(15,6));
      busters.push_back(pair(15,1));
      ghosts.push_back(pair(4,2));
      ghosts.push_back(pair(6,7));
      ghosts.push_back(pair(7,2));
      ghosts.push_back(pair(11,5));
      ghosts.push_back(pair(13,4));
      break;
  }
}

// Callback function for the idle cycles.
static void idle(void) {
  glutPostRedisplay();
}

// Callback function to initiate OpenGL.
static void init(void) {
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);

  glEnable(GL_DEPTH_TEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glShadeModel(GL_SMOOTH);

  glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);
}

// Callback function to handle special key input. These include the arrow keys.
void specialKeys(int key, int x, int y) {
  GLfloat trans = 0.5f;
  GLfloat rotx  = 5.f;
  GLfloat rot   = 5.f;
  
  switch (key) {
    // End key (zoom in)
    case GLUT_KEY_END:
      translate -= trans;
      break;
      
    // Home key (zoom out)
    case GLUT_KEY_HOME:
      translate += trans;
      break;
      
    // Up arrow key (rotate up around x-axis)
    case GLUT_KEY_UP:
      anglex = int(anglex + rotx) % 360;
      break;
    
    // Down arrow key (rotate down around x-axis)
    case GLUT_KEY_DOWN:
      anglex = int(anglex - rotx) % 360;
      break;
    
    // Left arrow key (rotate left)
    case GLUT_KEY_LEFT:
      angle = int(angle + rot) % 360;
      break;
    
    // Right arrow key (rotate right)
    case GLUT_KEY_RIGHT:
      angle = int(angle - rot) % 360;
      break;
  }
  
  glutPostRedisplay();
}
