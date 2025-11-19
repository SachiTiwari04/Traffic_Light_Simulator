#include <GL/glut.h>
#include <cmath> 
#include <iostream>

// --- GLOBAL CONSTANTS AND VARIABLES ---
const int WINDOW_WIDTH = 1000; 
const int WINDOW_HEIGHT = 600;

enum TrafficLightState {
    RED,
    GREEN,
    YELLOW,
    RED_YELLOW_ANT // New state for anticipation (Red + Yellow together)
};

// Traffic Light State 
TrafficLightState verticalState = RED; 

int lightTimer = 0;
// Durations in milliseconds
const int RED_DURATION = 6000; 
const int YELLOW_DURATION = 2500;
const int GREEN_DURATION = 8000;
const int RED_YELLOW_DURATION = 1000; // 1 second anticipation time

// Car movement variables
static float carX1 = WINDOW_WIDTH / 2 - 200; 
const float CAR_LENGTH = 120.0f; 
const float STOP_LINE_X = WINDOW_WIDTH / 2 + 5; 

// Variable for the car vibration effect (applied during the RED_YELLOW_ANT phase)
static float vibrationOffset = 0.0f;


// --- UTILITY FUNCTION: Draw Circle ---
void drawCircle(float x, float y, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); 
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * 3.14159f * float(i) / float(segments);
        float cx = radius * cosf(theta);
        float cy = radius * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

// --- DRAWING FUNCTIONS ---

void drawScene() {
    // 1. Background Grass (Muted, Natural Green)
    glColor3f(0.1f, 0.5f, 0.1f); 
    glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(WINDOW_WIDTH, 0.0f);
        glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
        glVertex2f(0.0f, WINDOW_HEIGHT);
    glEnd();

    // 2. Road Surface (Deep, Muted Gray)
    glColor3f(0.2f, 0.2f, 0.2f); 
    
    // Vertical Road 
    glBegin(GL_QUADS);
        glVertex2f(WINDOW_WIDTH/2 - 70, 0.0f);
        glVertex2f(WINDOW_WIDTH/2 + 70, 0.0f);
        glVertex2f(WINDOW_WIDTH/2 + 70, WINDOW_HEIGHT);
        glVertex2f(WINDOW_WIDTH/2 - 70, WINDOW_HEIGHT);
    glEnd();
    
    // Horizontal Road 
    glBegin(GL_QUADS);
        glVertex2f(0.0f, WINDOW_HEIGHT/2 - 70);
        glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT/2 - 70);
        glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT/2 + 70);
        glVertex2f(0.0f, WINDOW_HEIGHT/2 + 70);
    glEnd();

    // 3. Lane Markings (Dashed White Lines)
    glColor3f(0.8f, 0.8f, 0.8f); 
    glLineWidth(3.0f); 
    
    // Vertical Center Line 
    for(int y = 0; y < WINDOW_HEIGHT; y += 40) {
        glBegin(GL_LINES);
            glVertex2f(WINDOW_WIDTH/2, y);
            glVertex2f(WINDOW_WIDTH/2, y + 20);
        glEnd();
    }
    
    // Horizontal Center Line 
    for(int x = 0; x < WINDOW_WIDTH; x += 40) {
        glBegin(GL_LINES);
            glVertex2f(x, WINDOW_HEIGHT/2);
            glVertex2f(x + 20, WINDOW_HEIGHT/2);
        glEnd();
    }
    
    // 4. Stop Line 
    glColor3f(1.0f, 1.0f, 1.0f); 
    glLineWidth(6.0f);
    glBegin(GL_LINES);
        glVertex2f(STOP_LINE_X, WINDOW_HEIGHT/2 - 75);
        glVertex2f(STOP_LINE_X + 60, WINDOW_HEIGHT/2 - 75);
    glEnd();
}

void drawLightPost(float x, float y, TrafficLightState state) {
    const float lightRadius = 18.0f; 
    const int segments = 30;
    const float housingWidth = 50.0f;
    const float housingHeight = 110.0f;

    // Light Pole (TRUE BLACK)
    glColor3f(0.0f, 0.0f, 0.0f); 
    glBegin(GL_QUADS); 
        glVertex2f(x - 6, y - 5);
        glVertex2f(x + 6, y - 5);
        glVertex2f(x + 6, y + 70);
        glVertex2f(x - 6, y + 70);
    glEnd();
    
    // Light Housing (TRUE BLACK)
    glColor3f(0.0f, 0.0f, 0.0f); 
    glBegin(GL_QUADS); 
        glVertex2f(x - housingWidth/2, y + 70);
        glVertex2f(x + housingWidth/2, y + 70);
        glVertex2f(x + housingWidth/2, y + 70 + housingHeight);
        glVertex2f(x - housingWidth/2, y + 70 + housingHeight);
    glEnd();

    // --- DRAW LIGHTS (Aesthetic Glow Effect) ---

    // 1. Red Light (Highest)
    if (state == RED || state == RED_YELLOW_ANT) glColor3f(1.0f, 0.1f, 0.1f); 
    else glColor3f(0.2f, 0.05f, 0.05f); 
    drawCircle(x, y + 155.0f, lightRadius, segments);

    // 2. Yellow Light (Middle)
    if (state == YELLOW || state == RED_YELLOW_ANT) glColor3f(1.0f, 1.0f, 0.1f); 
    else glColor3f(0.2f, 0.2f, 0.05f);
    drawCircle(x, y + 115.0f, lightRadius, segments);

    // 3. Green Light (Lowest)
    if (state == GREEN) glColor3f(0.1f, 1.0f, 0.1f); 
    else glColor3f(0.05f, 0.2f, 0.05f);
    drawCircle(x, y + 75.0f, lightRadius, segments);
}

void drawCar(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0);

    const float CAR_W = CAR_LENGTH;
    const float CAR_H = 45.0f;
    const float WHEEL_R = 18.0f;
    const float ROOF_H = 35.0f; 

    // --- 1. Main Body (RED) ---
    glColor3f(1.0f, 0.0f, 0.0f); 
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(CAR_W, 0);
        glVertex2f(CAR_W, CAR_H);
        glVertex2f(0, CAR_H);
    glEnd();

    // --- 2. Cabin / Window Frame (WHITE) ---
    glColor3f(1.0f, 1.0f, 1.0f); 
    glBegin(GL_QUADS);
        glVertex2f(20, CAR_H);
        glVertex2f(100, CAR_H);
        glVertex2f(90, CAR_H + ROOF_H);
        glVertex2f(30, CAR_H + ROOF_H);
    glEnd();

    // --- 3. Window Glass (Darker for Contrast) ---
    glColor3f(0.3f, 0.3f, 0.4f); 
    glBegin(GL_QUADS);
        glVertex2f(25, CAR_H + 3);
        glVertex2f(95, CAR_H + 3);
        glVertex2f(87, CAR_H + 30);
        glVertex2f(33, CAR_H + 30);
    glEnd();

    // --- 4. Hood and Grill Detail ---
    
    // Side Air Vents 
    glColor3f(0.7f, 0.0f, 0.0f); 
    glBegin(GL_QUADS);
        glVertex2f(95, 25);
        glVertex2f(105, 25);
        glVertex2f(105, 35);
        glVertex2f(95, 35);
    glEnd();
    
    // --- 5. Headlights ---
    glColor3f(1.0f, 1.0f, 0.1f); 
    glBegin(GL_QUADS);
        glVertex2f(110, 35);
        glVertex2f(120, 35);
        glVertex2f(120, 40);
        glVertex2f(110, 40);
    glEnd();

    // --- 6. Wheels (Black) ---
    glColor3f(0.0f, 0.0f, 0.0f); 
    drawCircle(30, 0, WHEEL_R, 20); 
    drawCircle(90, 0, WHEEL_R, 20);
    
    glPopMatrix();
}

void display() {
    // Set Ortho projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // 1. Draw Scene/Road (Lowest layer)
    drawScene();
    
    // 2. Draw Car (Middle layer - applying vibration offset)
    drawCar(carX1 + vibrationOffset, (WINDOW_HEIGHT/2) + 5); 
    
    // 3. Draw Traffic Light (Highest layer)
    drawLightPost(WINDOW_WIDTH/2 + 50, WINDOW_HEIGHT/2 - 100, verticalState);
    
    glutSwapBuffers();
}

// --- ANIMATION AND LOGIC ---

void update(int value) {
    lightTimer += 16;
    
    // --- Light State Machine (Implements RED -> RED+YELLOW -> GREEN -> YELLOW -> RED cycle) ---
    switch (verticalState) {
        case RED:
            if (lightTimer >= RED_DURATION) {
                verticalState = RED_YELLOW_ANT; // RED -> RED+YELLOW (Anticipate Start)
                lightTimer = 0;
            }
            break;
        case RED_YELLOW_ANT:
            if (lightTimer >= RED_YELLOW_DURATION) {
                verticalState = GREEN; // RED+YELLOW -> GREEN (Go)
                lightTimer = 0;
            }
            break;
        case GREEN:
            if (lightTimer >= GREEN_DURATION) {
                verticalState = YELLOW; // GREEN -> YELLOW (Prepare to Stop)
                lightTimer = 0;
            }
            break;
        case YELLOW: 
            if (lightTimer >= YELLOW_DURATION) {
                verticalState = RED; // YELLOW -> RED (Stop)
                lightTimer = 0;
            }
            break;
    }

    // --- CAR MOVEMENT LOGIC: Stop, Prepare (Vibrate), Go ---
    const float carFront = carX1 + CAR_LENGTH;
    
    // Set vibration based on state
    if (verticalState == RED_YELLOW_ANT) {
        // INCREASED AMPLITUDE (2.5f) and FREQUENCY (0.5f) for visible vibration
        vibrationOffset = sin(lightTimer * 0.5f) * 2.5f; 
    } else {
        vibrationOffset = 0.0f;
    }

    // 1. Move when GREEN
    if (verticalState == GREEN) {
        carX1 += 1.8f; 
    } 
    // 2. ABSOLUTE STOP when RED, YELLOW, or RED_YELLOW_ANT
    else {
        if (carFront >= STOP_LINE_X) {
             // Freeze the position exactly at the stop line
             carX1 = STOP_LINE_X - CAR_LENGTH; 
        } else {
             // Roll forward until the stop line is reached
             carX1 += 0.8f;
        }
    }
    
    // Loop the car back around
    if (carX1 > WINDOW_WIDTH + 50) {
        carX1 = -200.0f; 
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void init() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); 
    glShadeModel(GL_SMOOTH); 
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("OpenGL Traffic Light Simulator - Sachu");
    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0);
    init();
    glutMainLoop();
    return 0;
}