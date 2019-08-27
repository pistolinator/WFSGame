//
//  main.cpp
//
//  Created by Fletcher Wells on 12/7/18.
//  Copyright © 2018 Fletcher Wells. All rights reserved.
//

#include "GFWLCD.hpp"
#include "GFWUtility.hpp"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <vector>
using namespace std;

#include <cstdlib>                      // standard definitions
#include <iostream>                     // C++ I/O
#include <cstdio>                       // C I/O (for sprintf)
#include <cmath>                        // standard definitions



#define ZOOM_SCALE 3
void Init ();
void mainLoop ();
void DrawEverything ();
bool leftKeyInp;
bool upKeyInp;
bool rightKeyInp;
bool downKeyInp;

#define CALLBACK_DELAY .0001 // in seconds
// GLint TIMER_DELAY = CALLBACK_DELAY * 1000; // in milleseconds
GLint TIMER_DELAY = 5; // in milleseconds

void Reshape(int w, int h) {
    glViewport (0, 0, w, h);                    // update the viewport
    glMatrixMode(GL_PROJECTION);                // update projection
    glLoadIdentity();
    gluOrtho2D(0.0, 320, 0.0, 240);             // map unit square to viewport
    glMatrixMode(GL_MODELVIEW);
    // glutPostRedisplay ();                       // request redisplay
}

void Display (void) {                          // display callback
    DrawEverything ();
    glFlush();
}

void PassiveMouseV2 (int x, int y) {             // updates upon mouse movement when there are mouse clicks/inputs
    if (LCD.touchState) {
        LCD.touchX = x/ZOOM_SCALE;
        LCD.touchY = y/ZOOM_SCALE;
    }
}

void Mouse (int buttonState, int clickState, int x, int y) {      // mouse click callback ... only updates upon mouse click
    if (clickState == GLUT_DOWN && buttonState == GLUT_LEFT_BUTTON) { // was (button == GLUT_LEFT_BUTTON)
        LCD.touchState = true;
        LCD.touchX = x/ZOOM_SCALE; // re-enable these statements if you delete the passive mouse function
        LCD.touchY = y/ZOOM_SCALE;
        // cout << "Touch Y: " << LCD.touchY;
    } if ((buttonState == GLUT_LEFT_BUTTON ) && (clickState == GLUT_UP)) {
        LCD.touchState = false;
        LCD.touchX = -1;
        LCD.touchY = -1;
    }
}

// keyboard callback
void Keyboard (unsigned char c, int x, int y) {
    switch (c) {                                // c is the key that is hit
        case 'a':
            leftKeyInp = true;
            break;
        case 'w':
            upKeyInp = true;
            break;
        case 'd':
            rightKeyInp = true;
            break;
        case 's':
            downKeyInp = true;
            break;
        case 27:                               // 'q' means quit
            exit (0);
            break;
        default:
            break;
    }
}

void KeyboardUp (unsigned char c, int x, int y) {
    switch (c) {                                // c is the key that is hit
        case 'a':
            leftKeyInp = false;
            break;
        case 'w':
            upKeyInp = false;
            break;
        case 'd':
            rightKeyInp = false;
            break;
        case 's':
            downKeyInp = false;
            break;
        default:
            break;
    }
}

void Timer (int id) {
    mainLoop ();
    
    glutTimerFunc(TIMER_DELAY, Timer, 0);
}
    
int main (int argc, char** argv) {
    
    glutInit(&argc, argv);                      // OpenGL initializations
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);// double buffering and RGB // was GLUT_DOUBLE
    glutInitWindowSize(320*ZOOM_SCALE, 240*ZOOM_SCALE);               // create a 400x400 window
    glutInitWindowPosition(0, 0);               // ...in the upper left
    glutCreateWindow("Game");                  // create the window
    
    glutDisplayFunc(Display);                 // setup callbacks
    glutReshapeFunc(Reshape);
    glutMotionFunc(PassiveMouseV2);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(Keyboard);
    glutKeyboardUpFunc(KeyboardUp);
    glutTimerFunc(TIMER_DELAY, Timer, 0);
    
    Init ();
    
    glutMainLoop();                             // start it running
    
    return 0;                                   // ANSI C expects this
}







/*
 
 
 
 NON GL STUFF
 
 
 
*/






/************************************************/
/* Name: Fletcher Wells & Doug Van Arsdale  Date: 11/27/18          */
/* Instructor: Paul Clingan  10:20                                  */
/************************************************/

#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <string.h>
using namespace std;

// define scalers for values that depend on the frame rate
#define FRS_0 125 // framerate scale for accelAmt (the player's acceleration) // .001
#define FRS_1 .01 // framerate scale for KINF (coefficient of kinetic friction)
#define FRS_2 .3 // framerate scale for AIRF (amount of air friction)
#define FRS_3 250 // framerate scale for velScale (bullet velocity)

/************************************************************************************************ DECLERATIONS ************************************************************************************************/




/************************************************************************ CLASS DECLERATIONS ************************************************************************/



/************************************************ MISCELLANEOUS CLASSES ************************************************/


class Vector2 {
public:
    Vector2 (float x0, float y0) {
        x = x0;
        y = y0;
    }
    Vector2 () {
        x = 0;
        y = 0;
    }
    // returns the magnitude of the vector
    float getMagnitude () {
        return sqrt (  pow (x, 2.0)  +  pow (y, 2.0)  );
    }
    // returns the vector converted into a unit vector
    Vector2 getUnitVector () {
        float magnitude = getMagnitude();
        return Vector2 (x / magnitude, y / magnitude);
    }
    // rotates vector counter-clockwise 90 degrees
    void RotateLeft () {
        int tempX = x;
        x = -y;
        y = tempX;
    }
    // rotates vector clockwise 90 degrees
    void RotateRight () {
        int tempX = x;
        x = y;
        y = -tempX;
    }
    float x;
    float y;
};

class IntVector2 {
public:
    IntVector2 (int x0, int y0) {
        x = x0;
        y = y0;
    }
    IntVector2 () {
        x = 0;
        y = 0;
    }
    // rotates vector counter-clockwise 90 degrees
    void RotateLeft () {
        int tempX = x;
        x = -y;
        y = tempX;
    }
    // rotates vector clockwise 90 degrees
    void RotateRight () {
        int tempX = x;
        x = y;
        y = -tempX;
    }
    // returns whether the given vector equals this vector
    bool Equals (IntVector2 vect) {
        return (x == vect.x && y == vect.y);
    }
    int x;
    int y;
};

class Edge {
public:
    // creates edge object, taking as its parameters: its start point, its end point, its direction
    // where its direction is used exclusively for collision and a direction of 0 is down (collides with an object traveling upwards), 1 is left, 2 is up, 3 is right
    // (the start point should generally be above and to the left of the end point in order for the edge to work properly with the physics system)
    Edge (Vector2 v0, Vector2 v1, int direction) {
        verts [0] = v0;
        verts [1] = v1;
        dir = direction;
        edgeType = 'e';
        switch (dir) {
            case 0:
                norm = IntVector2 (0, -1);
                break;
            case 1:
                norm = IntVector2 (-1, 0);
                break;
            case 2:
                norm = IntVector2 (0, 1);
                break;
            case 3:
                norm = IntVector2 (1, 0);
                break;
            default:
                break;
        }
    }
    Edge (Vector2 v0, Vector2 v1, IntVector2 normal) {
        verts [0] = v0;
        verts [1] = v1;
        norm = normal;
        edgeType = 'e';
        if (normal.x == 0) {
            if (normal.y == 1) {
                dir = 2;
            } else if (normal.y == -1) {
                dir = 0;
            }
        } else if (normal.y == 0) {
            if (normal.x == 1) {
                dir = 3;
            } else if (normal.x == -1) {
                dir = 1;
            }
        } else {
            dir = -1;
        }
    }
    Edge () {
        
    }
    Vector2 verts[2];
    int dir;
    IntVector2 norm;
    // for game mechanics only
    float closestEdge; // distance to closest edge (if horizontal line)
    bool absoluteEdge; // if the edge is the upper or bottom most edge
    bool isIsland; // if the edge makes up a shape that is surrounded on all sides by tunnels
    char edgeType;
};

IntVector2 DirToIntVector2 (int dir) {
    IntVector2 norm;
    switch (dir) {
        case 0:
            norm = IntVector2 (0, -1);
            break;
        case 1:
            norm = IntVector2 (-1, 0);
            break;
        case 2:
            norm = IntVector2 (0, 1);
            break;
        case 3:
            norm = IntVector2 (1, 0);
            break;
        default:
            norm = IntVector2 (0, 0);
            break;
    }
    return norm;
}

class Box { // notably, the screen is in the fourth quadrant
public:
    // creates box object, taking four verticies of type Vector2 as its parameters
    // where v0 is top left vertex, v1 is top right vertex, v2 is bottom left vertex, v3 is bottom right vertex // should be where v0 is top left vertex, v1 is top right vertex, v2 is bottom right vertex, v3 is bottom left vertex
    Box (Vector2 v0, Vector2 v1, Vector2 v2, Vector2 v3) {
        verts [0] = v0;
        verts [1] = v1;
        verts [2] = v2;
        verts [3] = v3;
        width = verts [1].x - verts [0].x;
        height = verts [2].y - verts [0].y;
    }
    // creates box object, taking as its parameters: its top left vertex, a width, a height
    Box (Vector2 v0, float wdth, float hght) {
        width = wdth;
        height = hght;
        verts [0] = v0;
        verts [1] = Vector2 (v0.x + width, v0.y);
        verts [2] = Vector2 (v0.x, v0.y - height); // verts [2] = Vector2 (v0.x + width, v0.y - height);
        verts [3] = Vector2 (v0.x + width, v0.y - height); // verts [3] = Vector2 (v0.x, v0.y - height);
    }
    Box () {
        
    }
    Vector2 verts[4];
    float width;
    float height;
    void UpdatePosition (Vector2 pos) {
        verts [0] = pos;
        UpdateVerts ();
    }
    void UpdateVerts () {
        verts [1] = Vector2 (verts [0].x + width, verts [0].y);
        verts [2] = Vector2 (verts [0].x, verts [0].y - height); // verts [2] = Vector2 (v0.x + width, v0.y - height);
        verts [3] = Vector2 (verts [0].x + width, verts [0].y - height); // verts [3] = Vector2 (v0.x, v0.y - height);
    }
    bool IsWithinBounds (Vector2 point) {
        if (point.x >= verts [0].x  &&  point.x <= verts [0].x + width  &&  point.y <= verts [0].y  &&  point.y >= verts [0].y - height) {
            return true;
        } else {
            return false;
        }
    }
    bool IsWithinBounds (Box box) {
        // if the box is within this.box's X bounds  &&  Y bounds
        if (  box.verts [0].x <= verts [1].x  &&  box.verts [1].x >= verts [0].x   &&   box.verts [3].y <= verts [0].y  &&  box.verts [0].y >= verts [3].y) {
            return true;
        } else {
            return false;
        }
    }
    Vector2 getCenter () {
        return Vector2 (  verts [0].x + width/2.0,  verts [0].y - height/2.0  );
    }
    Edge getSouthEdge () {
        Edge southEdge = Edge (verts[2], verts[3], 0);
        return southEdge;
    }
    Edge getWestEdge () {
        Edge westEdge = Edge (verts[0], verts[2], 1);
        return westEdge;
    }
    Edge getNorthEdge () {
        Edge northEdge = Edge (verts[0], verts[1], 2);
        return northEdge;
    }
    Edge getEastEdge () {
        Edge eastEdge = Edge (verts[1], verts[3], 3);
        return eastEdge;
    }
};

class Raycast {
public:
    // Raycast (Vector2 origin, Vector2 direction, float maxDistance = 99999, char type = 'X');
    Raycast (Vector2 origin, Vector2 direction, float maxDistance, char type);
    Raycast () {
        
    }
    bool hit; // whether the ray hit or not
    char colliderType; // type of collider hit the ray
    // Shape collider; // the collider hit by the ray
    Vector2 point; // the point in which the ray hit the collider's surface
    float distance; // the distance from the origin to where the ray hit the collider's surface
private:
    float stepSize; // the step size per iteration of the raycast
    Vector2 pos; // current position of the particle
    Vector2 dir; // standarized direction of the raycast
    bool CheckBoxCollision (Box obj);
    bool CheckEdgeCollision (Edge obj);
};

struct RGB {
    float r;
    float g;
    float b;
};
typedef struct RGB RGB;
int ConvertColor (float r, float g, float b);
int ConvertColor (RGB rgbColor);
RGB ConvertColor (int hexColor);
int ConvertToShadow (int hexColor, int shadowColor, float shadowAlpha);



/************************************************ GUI CLASSES ************************************************/


class Button {
public:
    // creates a box object, taking as its parameters a box object representing its boundary, a character pointer to an array indicating the text
    // to be displayed on the object, and an an integer representing the type of button it is, where 0 is default menu button and 5 is arrow pad button
    char text[100]; // may not be right
    Button (Box bounds, char txt[], int buttonType) {
        box = bounds;
        //text = txt;
        strcpy (text, txt);
        type = buttonType;
        BUTT_MARGIN = 5;
        pressStartedWithinBound = false;
    }
    Button (Box bounds) {
        box = bounds;
        type = 0;
        strcpy (text, "");
        BUTT_MARGIN = 5;
    }
    Button () {
        
    }
    // updates the text of the button
    void UpdateText (char txt[]) {
        strcpy (text, txt);
    }
    // note that this function is not used
    // returns whether the button had been pressed and updates the pressed-or-unpressed state of the button
    bool WasPressed (Vector2 touchPos, bool touchStatus) {
        bool result = false;
        if (touchStatus == false) { // if the screen is not currently touched
            if (isPressed == true && pressStartedWithinBound) { // check if the button had been touched the last time the method was called and that the initial touch was within the bounds of the button
                result = true; // if it was, then return that the button was pressed
                // *****
                //       this bit could not be working because LCD.Touch only updates the returned mouse position when the screen is first touched
                //       I can fix that in this 'emulation' but I don't know what the real behavior of LCD.Touch is on the actual proteus
                //       so if I fix this now it may cause problems with button input in the future
                // *****
                isPressed = false; // reset the button so no weird behaviors happen
                pressStartedWithinBound = false;
                noPressLastTime = true;
            } else { // if the screen had never been touched or if when the screen was no longer being touched the touch input was not within the bounds of the button
                result = false;
                noPressLastTime = true; // take note that there was no press during this iteration of the method's call
                pressStartedWithinBound = false;
            }
        } else { // if the screen is touched
            isPressed = IsWithinBounds (touchPos); // check if the touch input was in the bounds of the button
            // pressStartedWithinBound = true; // un-comment this line to make it so that it doesn't matter if the touch input initially started within the bounds of the button
            if (noPressLastTime && isPressed) { // if this is the first time there is a touch input on the screen and the input is within the bounds of the button
                pressStartedWithinBound = true; // take note of it
            } if (pressStartedWithinBound == false) { // if the initial press was not within the bounds of the button
                isPressed = false; // make it be so that the button is not pressed
            }
            result = false;
            noPressLastTime = false;
        }
        return result;
    }
    // returns whether the button is being pressed and updates the pressed-or-unpressed state of the button
    bool IsBeingPressed (Vector2 touchPos) {
        isPressed = IsWithinBounds (touchPos);
        return isPressed;
    }
    bool IsBeingPressed (Vector2 touchPos, bool touchStatus) {
        isPressed = IsWithinBounds (touchPos);
        return isPressed;
    }
    // returns true if the touch position is within the position of the button boundaries and false otherwise
    bool IsWithinBounds (Vector2 touchPos) {
        if (touchPos.x > box.verts [0].x  &&  touchPos.x < box.verts [0].x + box.width  &&  touchPos.y < box.verts [0].y  &&  touchPos.y > box.verts [0].y - box.height) { // may have to adjust this based on where the origin is set
            return true;
        } else {
            return false;
        }
    }
    // draws pressed or unpressed button depending on the button's state
    void DrawButton () {
        if (isPressed) { // if the button is pressed
            DrawPressedButton ();
        } else {
            DrawUnpressedButton ();
        }
    }
    Box box;
    int type;
    constexpr static float dec = 5; // amount pixels to shrink the button by when its pressed
    bool isPressed; // button's state of being pressed or unpressed
    bool noPressLastTime;
    bool pressStartedWithinBound;
    int BUTT_MARGIN;
    constexpr static int TEXT_HEIGHT = 10;
private:
    // draws unpressed button
    void DrawUnpressedButton () {
        switch (type) {
            case 5:
                LCD.SetFontColor (WHITE);
                LCD.WriteAt ( text, (int)(box.verts[0].x+BUTT_MARGIN), -(int)(box.verts[0].y - box.height / 2.0 + TEXT_HEIGHT / 2.0) );
                LCD.DrawRectangle ( (int)(box.verts[0].x), -(int)(box.verts[0].y), box.width, box.height );
                break;
            case 6:
                LCD.SetFontColor (ABYSS);
                LCD.SetBackgroundColor (ABYSS);
                LCD.FillRectangle ( (int)(box.verts[0].x), -(int)(box.verts[0].y), box.width, box.height );
                LCD.SetFontColor (WHITE);
                LCD.WriteAt ( text, (int)(box.verts[0].x+BUTT_MARGIN), -(int)(box.verts[0].y - box.height / 2.0 + TEXT_HEIGHT / 2.0) );
                LCD.DrawRectangle ( (int)(box.verts[0].x), -(int)(box.verts[0].y), box.width, box.height );
                break;
            case 0:
            default:
                //LCD.SetFontColor(CORNFLOWERBLUE);
                //LCD.FillRectangle ( (int)(box.verts[0].x), -(int)(box.verts[0].y), box.width, box.height);
                // effectively erase the button before drawing the un-pressed version
                LCD.SetFontColor (WALLCOLOR);
                LCD.FillRectangle ( (int)(box.verts[0].x), -(int)(box.verts[0].y) - 1, box.width + 1, box.height + 1 );
                // draw the actual button
                LCD.SetFontColor (WHITE);
                LCD.WriteAt ( text, (int)(box.verts[0].x+BUTT_MARGIN), -(int)(box.verts[0].y - box.height / 2.0 + TEXT_HEIGHT / 2.0) );
                LCD.DrawRectangle ( (int)(box.verts[0].x), -(int)(box.verts[0].y), box.width, box.height );
                break;
        }
    }
    // draws pressed version of button
    static const int ABYSS = 0x1a1a1a;
    // static const int WALLCOLOR = 0x2f4f4f;
    static const int WALLCOLOR = ABYSS;
    static const int BUTTHIGHLIGHT = INDIANRED;
    void DrawPressedButton () {
        switch (type) {
            case 5:
                LCD.SetFontColor (RED);
                LCD.WriteAt ( text, (int)(box.verts[0].x+BUTT_MARGIN), -(int)(box.verts[0].y - box.height / 2.0 + TEXT_HEIGHT / 2.0) );
                // LCD.DrawRectangle ( (int)(box.verts[0].x + dec / 2.0), -(int)(box.verts[0].y - dec / 2.0), box.width - dec, box.height - dec );
                LCD.DrawRectangle ( (int)(box.verts[0].x), -(int)(box.verts[0].y), box.width, box.height );
                break;
            case 6:
                LCD.SetFontColor (RED);
                LCD.WriteAt ( text, (int)(box.verts[0].x+BUTT_MARGIN), -(int)(box.verts[0].y - box.height / 2.0 + TEXT_HEIGHT / 2.0) );
                break;
            case 0:
            default:
                // effectively erase the button before drawing the pressed version
                LCD.SetFontColor (WALLCOLOR);
                LCD.FillRectangle ( (int)(box.verts[0].x), -(int)(box.verts[0].y) - 1, box.width + 1, box.height + 1 ); // erase button
                LCD.SetFontColor (BUTTHIGHLIGHT);
                LCD.SetBackgroundColor (BUTTHIGHLIGHT);
                //cout << "oioioiioio";
                LCD.FillRectangle ( (int)(box.verts[0].x + dec / 2.0), -(int)(box.verts[0].y - dec / 2.0), box.width - dec, box.height - dec );
                LCD.SetFontColor (WHITE);
                LCD.WriteAt ( text, (int)(box.verts[0].x+BUTT_MARGIN), -(int)(box.verts[0].y - box.height / 2.0 + TEXT_HEIGHT / 2.0));
                LCD.DrawRectangle ( (int)(box.verts[0].x + dec / 2.0), -(int)(box.verts[0].y - dec / 2.0), box.width - dec, box.height - dec );
                LCD.SetBackgroundColor (BLACK);
                break;
        }
    }
};



/************************************************ IN-GAME CLASSES ************************************************/


class Cop { // note that this class requires custom definitions for colors
public:
    Cop (Box box) {
        bounds = box;
        pos = box.verts[0]; // initialize position vector (for the cop object)
        int SPRITESCALE2 = 2;
        shootPoint = Vector2 (pos.x - 3 * SPRITESCALE2, pos.y - 5 * SPRITESCALE2);
        eyePoint = Vector2 (pos.x + 4 * SPRITESCALE2, pos.y - 4 * SPRITESCALE2); // center of the cops eyes
        isShooting = false;
        bulletPos = shootPoint;
        velScale = .003 * FRS_3;
        timerStarted = false;
        elapsedTime = 0;
        totalElapsedTime = 0;
        alertState = false;
        didBreak = false;
        ricoCount = 0;
        explodePos.x = -420;
        explodeIterations = 0;
        coolDownTimer = -1;
    }
    Cop () {
        
    }
    Vector2 pos;
    Box bounds;
    Raycast raycast;
    Vector2 eyePoint;
    Vector2 shootPoint; // point at which the bullet exits the cops gun
    Vector2 bulletPos; // position of the bullet
    Vector2 bulletDir; // direction bullet is traveling
    float velScale; // velocity scalar of the bullet
    bool isShooting; // true if the bullet should be displayed
    float timer; // point in time in which the cop noticed the player's criminal activities
    bool timerStarted;
    float elapsedTime;
    float totalElapsedTime;
    constexpr static int COOL_DOWN_AMT = 3.5;
    float coolDownTimer;
    int alertState; // where 0 is not alerted, 1 is alerted, 2 is shoot on sight
    bool bulleting; // whether the bullet currently being fired / is traveling through the air
    bool didBreak;
    int ricoCount; // the amount of times the bullet has ricocheted
    Vector2 explodePos;
    int explodeIterations;
    constexpr static int MAX_EXPLODE_FRAMES = 15;
    constexpr static int MAXRICO = 1; // the maximum amount of times the bullet can ricochet before being destroyed
    constexpr static float WAITTIME = 1.0;
    void DrawExplosion (); // draw a small explosion at the point in which the bullet was destroyed
    void DrawCop ();
    void Patrol ();
    void DrawBullet ();
    void DrawGraphic ();
    bool CheckBoxCollision (Box obj);
    bool CheckEdgeCollision (Edge obj);
};

class WFS { // class for wet floor sign object
public:
    WFS (Box box) {
        bounds = box;
        pos = box.verts[0]; // initialize position vector (for the cop object)
        prevPos = pos;
        prevBounds = bounds;
        heldByPlayer = false;
    }
    WFS () {
        /*bounds = Box ();
         pos = box.verts[0];
         heldByPlayer = false;*/
    }
    Vector2 pos;
    Vector2 prevPos;
    Box bounds;
    Box prevBounds;
    bool heldByPlayer;
    void DrawWFS ();
};

class Arm { // class for the player's arms
public:
    constexpr static int SPRITESCALE = 2;
    Arm (char tp) {
        type = tp;
        DEFAULT_LENGTH = 3 * SPRITESCALE;
        holdLength = 0 * SPRITESCALE;
    }
    Arm (Box box) {
        bounds = box;
        type = 'X';
        DEFAULT_LENGTH = 3 * SPRITESCALE;
        holdLength = 0 * SPRITESCALE;
    }
    Arm () {
        
    }
    void DetermineLength ();
    void UpdateArmBounds ();
    void DrawArm ();
    
    Vector2 rayPos0; // the position in which the ray should be cast from
    Vector2 rayPos1; // the position in which the ray should be cast from
    Box bounds;
    Box prevBounds;
    int length;
    int holdLength; // the extra length the arm extends to hold the wetfloor sign
    int DEFAULT_LENGTH;
    char type;
    IntVector2 dir;
    Raycast ray0;
    Raycast ray1;
private:
};
void UpdateArms ();



/************************************************ PHYSICS CLASSES ************************************************/


class PhysicsObject {
public:
    // creates physics object, which is a box with simple newtonian physics, capable of colliding with static boxes and edges
    PhysicsObject (Box box) {
        bounds = box;
        pos = box.verts[0]; // initialize position vector (for the physics object)
        vel = Vector2 (); // initialize velocity vector (for the physics object)
        prevPos = pos;
        prevDir = dir;
    }
    void Translate (Vector2 accel);
    void CollideBox (Box obj);
    void CollideEdge (Edge obj);
    float ApplyFrictionalDeceleration (float velocity, float deceleration);
    Vector2 CalcAccel (Vector2 objPos, Vector2 objVel); // calculate the acceleration due to a collision with the given object (currently only being used for bullets)
    // void UpdateBounds () {} // updates the verticies of the physics object's bounds // not really necessary and probablly won't keep
    Vector2 pos, vel;
    Box bounds;
    Vector2 prevPos; // the player's position since the last instance the screen was updated
    bool IsWithinBounds (Vector2 point, float tolerance) {
        if (point.x > bounds.verts [0].x-tolerance  &&  point.x < bounds.verts [0].x + bounds.width + tolerance  &&  point.y < bounds.verts [0].y + tolerance  &&  point.y > bounds.verts [0].y - bounds.height - tolerance) {
            return true;
        } else {
            return false;
        }
    }
    float RESTI = .5; // initialize coefficiednt of restitution
    float STATF = .35; // initialize coefficient of static friction (notably currently being used as a sort of orthogonal coefficient of restituion)
    int dir = 0; // direction the player is facing / traveling
    int prevDir = dir;
private:
    //constexpr static float STATF = .35; // initialize coefficient of static friction // (in theory, static friction would have to be fully implemented if another physics object were created, or wanted to be varied)
    constexpr static float KINF = 0.3 * FRS_1; // initialize coefficient of kinetic friction
    constexpr static float AIRF = .01 * FRS_2; // initialize coefficient of air friction
    //constexpr static float RESTI = .5; // initialize coefficient of restitution
    constexpr static float ERROR_MARGIN = 7; // initialize margin of error for collision detection
    constexpr static float EDGE_TOL = 10; // initialize edge tolerance, which is essentially a pseudo width for collision edges
};




/************************************************************************ FUNCTION DECLERATIONS ************************************************************************/



/************************ GUI FUNCTIONS ************************/

void DrawSplashScreen ();
void DrawMainMenu ();
void DrawRules ();
void DrawStats ();
void DrawCredits ();


/************************ IN-GAME FUNCTIONS ************************/

void GameLoop ();
void PreDraw ();
void DrawGame ();
void DrawBoxesAndEdges ();
void DrawEdge (Edge edge);
void DrawPlayer (); // where the player's position (Vector2 pos) is ~its top left corner
void ErasePlayer ();
void DrawInGameGUI ();
void UpdateStats ();
void ResetGame ();
float Distance (Vector2 start, Vector2 end);
// void UpdateArmBounds ();


/************************ LEVEL-GENERATION FUNCTIONS ************************/

void GenerateLevel ();
void DrawLevel ();
void DrawLevelForeground ();


/************************ PHYSICS FUNCTIONS ************************/

/*void Translate (Vector2 accel);
 void CollideBox (Box obj);
 void CollideEdge (Edge obj);
 void CorrectBoxError (Box obj);
 void CorrectEdgeError (Edge obj);*/




/************************************************************************ GLOBAL VARIABLE DECLERATIONS ************************************************************************/



/************************ MISCELLANEOUS VARIABLES ************************/

int state = 0;
bool gameJustStarted = false;
Vector2 touch = Vector2 ();
bool touchState = false;
Box window = Box (   Vector2 (0, 0),  320,  240   );
#define UPDATE_INTERVAL 0.005 // define the time interval in which the screen should be updated
float timeSinceIteration;
float timeSpentRendering;
#define SPRITESCALE 2
int iterationsSinceDeath = 0; // number of frame iterations elapsed since player died


/************************ GUI VARIABLES ************************/

#define BUTT_WIDTH 150
#define BUTT_HEIGHT 30
Button startButt;
Button helpButt;
Button statsButt;
Button creditsButt;
Button backButt;
Button scoreButt;
bool stateChange;
bool firstTimeEverLoaded;
bool firstTimeEverDrawn;


/************************ IN-GAME GUI VARIABLES ************************/

#define MB_WIDTH 30 // move buttons width
#define MB_HEIGHT 30 // move buttons height
#define MB_MARGIN 5 // move buttons margin
Vector2 moveButtPos = Vector2 (MB_MARGIN, -240 + (MB_HEIGHT + MB_MARGIN) * 2);
Button leftButt;
Button upButt;
Button rightButt;
Button downButt;
Button inGameBackButt;
/*
 bool leftInp;
 bool upInp;
 bool rightInp;
 bool downInp;
*/
#define ABYSS                   0x1a1a1a
#define BACKGROUNDCOLOR         0xFDF2E9     // 0x6495ed //- 0xCDDDF9 // 0x444243 // 0xAB7D5C //
// #define WALLCOLOR               0x1a1a1a
#define WALLCOLOR               0x1a1a1a     // 0x2f4f4f
int TILECOLOR =                 0xa14e17;    // 0xffffff // 0xB5B0AF // 0x574231 // 0xa14e17
int TILECOLOR2 =                0xe1b289;    // 0x786B58 // 0xe1b289
int TILECOLOR3 =                0xA18F7B;    // 0xA18F7B // 0xA18F7B
#define WALLWALLCOLOR           0x8a715c     // 0x708090 // 0x418E88 // 0x9b704b // 0x8a715c
#define SHADOWCOLOR             0x000000
int RAILINGCOLOR =              INDIANRED;   // 0xC0C0C0 // 0x000000 // 0x4d4d4d
int RAILINGSHADOW;
int TOPTHINGCOLOR;
int TOPTHINGSHADOW;
int BACKGROUNDSHADOW;
int TILESHADOW;
int TILESHADOW2;
int TILESHADOW3;
#define SHADOW_ALPHA            0.5          // between 0 and 1
// #define WALLCOLOR               0x484848


/************************ GAME VARIABLES ************************/

#define MAX_COPS 64
int level = 1; // current level the player is on; note that the level number corresponds to the amount of cops loaded into the game
Cop cops [MAX_COPS];
WFS wfs;
bool playerIsDead;
int highScore;
Vector2 deathVel;
Vector2 bullDeathVel;
float bSR[8]; // bleed speed random


/************************ PHYSICS VARIABLES ************************/

#define BOXES_SIZE 0
#define EDGES_SIZE 2
PhysicsObject playerObj = PhysicsObject (  Box ( Vector2 (0, 0) , 16, 16)  ); // initializes physics object for player with width 32 and height 32 positioned at the origin
// Box armBounds;
Arm Larm; // left arm
Arm Rarm; // right arm
Box boxes [BOXES_SIZE]; // note that boxes will contain nothing
Edge edges [EDGES_SIZE]; // note that edges will only contain the left and right bounds of the screen
Vector2 accelBull; // the player's acceleration due to a collision with a bullet


// global variables for level generation
#define SCREEN_HEIGHT 240
#define SCREEN_LENGTH 320

// #define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320
// #define TUNNEL_RES 25
#define TILE_SIZE 10 // the width or height of a square tile in pixels              // 20
#define TUNNEL_TILE_SIZE 6 // amount of tiles for the diameter of the tunnel        // 2
#define GRID_HEIGHT 24 // should equal SCREEN_WIDTH / TILE_SIZE                     // 12
#define GRID_WIDTH 32 // should equal SCREEN_WIDTH / TILE_SIZE                      // 16
int MAX_TURNS = 10; // maximum amount of turns
int MAX_TUNNELS = 15;
int MAX_LENGTH = 10; // maximum length (in tiles) of a single tunnel
#define MAX_TUNNEL_EDGES 4096
#define TUNNEL_BOX_SIZE 2048
Edge tunnelEdges [MAX_TUNNEL_EDGES];
Box backgroundBoxes [TUNNEL_BOX_SIZE];
int numTunnelEdges = 0; // actual number or length of tunnel edges
int numTunnelBoxes = 0; // actual number or length of tunnel boxes
// bool RIGHT_ONLY = true;
#define RIGHT_ONLY true


// gen 2 variables
bool leftInp;
bool upInp;
bool rightInp;
bool downInp;

// settings
bool isComputer = true;
bool highDetail = true;
bool objectPerm = true; // determines whether or not object permanence is enabled for cops
bool goreMode = true && highDetail; // determines whether or not there should be a lot of gore
bool showFOV = false; // determines whether the cop's field of view should be displayed




/************************************************************************************************ MAIN FUNCTION ************************************************************************************************/


int tempState;
float timeSinceLastFrameUpdate;

// do all the stuff that comes before the mainLoop
void Init () {
    // array global variable initializations
    char charPtr [100];
    strcpy (charPtr, "START");
    startButt = Button (  Box ( Vector2 (30, -60), BUTT_WIDTH, BUTT_HEIGHT ), charPtr, 0  );
    strcpy (charPtr, "HELP");
    helpButt = Button (  Box ( Vector2 (30, -105), BUTT_WIDTH, BUTT_HEIGHT ), charPtr, 0  );
    strcpy (charPtr, "STATISTICS");
    statsButt = Button (  Box ( Vector2 (30, -150), BUTT_WIDTH, BUTT_HEIGHT ), charPtr, 0  );
    strcpy (charPtr, "CREDITS");
    creditsButt = Button (  Box ( Vector2 (30, -195), BUTT_WIDTH, BUTT_HEIGHT ), charPtr, 0  );
    strcpy (charPtr, "BACK");
    backButt = Button (  Box ( Vector2 (145+60, -15), 75, BUTT_HEIGHT ), charPtr, 0  );
    
    strcpy (charPtr, "<");
    leftButt = Button (  Box ( Vector2 (moveButtPos.x, moveButtPos.y - (MB_HEIGHT + MB_MARGIN)) ,  MB_WIDTH, MB_HEIGHT ), charPtr, 5  ); // may need to be declared abovedddddsawda function
    strcpy (charPtr, "^");
    upButt = Button (  Box ( Vector2 (moveButtPos.x + MB_WIDTH + MB_MARGIN, moveButtPos.y) ,  MB_WIDTH, MB_HEIGHT ), charPtr, 5  );
    strcpy (charPtr, ">");
    rightButt = Button (  Box ( Vector2 (moveButtPos.x + (MB_WIDTH + MB_MARGIN) * 2, moveButtPos.y - (MB_HEIGHT + MB_MARGIN)) ,  MB_WIDTH, MB_HEIGHT ), charPtr, 5  );
    strcpy (charPtr, "v");
    downButt = Button (  Box ( Vector2 (moveButtPos.x + MB_WIDTH + MB_MARGIN, moveButtPos.y - (MB_HEIGHT + MB_MARGIN)) ,  MB_WIDTH, MB_HEIGHT ), charPtr, 5  );
    strcpy (charPtr, "BACK");
    inGameBackButt = Button (  Box ( Vector2 (160-28, -MB_MARGIN), 56, 30 ), charPtr, 5  );
    
    strcpy (charPtr, "SCORE: ");
    // strcat (charPtr, 0);
    scoreButt = Button (  Box ( Vector2 (MB_MARGIN, -MB_MARGIN), 30, 30 ), charPtr, 6  );
    
    //edges [0] = Edge (  window.verts [0], window.verts [1], 0  ); // top edge of screen
    edges [1] = Edge (  window.verts [1], window.verts [3], 1  ); // right edge of screen   ( [1,2] if I fix box vertex order )
    //edges [2] = Edge (  window.verts [2], window.verts [3], 2  ); // bottom edge of screen   ( [3,2] if I fix box vertex order )
    edges [0] = Edge (  window.verts [0], window.verts [2], 3  ); // left edge of screen   ( [0,3] if I fix box vertex order )
    //boxes [0] = Box (   Vector2 (200, -90),  50,  100   );
    //boxes [1] = Box (   Vector2 (75, -150),  135,  20   );
    
    // cop[0] = Cop (  Box ( Vector2 (240, -180) , 16, 16)  );
    // wfs = WFS (  Box ( Vector2 (24, -18) , 14, 9)  );
    
    state = 0; // change state to main menu state
    tempState = state; // initialize tempState, which is used to avoid weird switch case behaviors that may or may not exist
    timeSinceLastFrameUpdate = TimeNow (); // initialize the time since the last screen/frame update
    
    timeSinceIteration = TimeNow ();
    timeSpentRendering = TimeNow ();
    stateChange = true;
    
    firstTimeEverLoaded = true;
    
    TILECOLOR = ConvertToShadow (TILECOLOR, WHITE, 0.5);
    TILECOLOR2 = ConvertToShadow (TILECOLOR2, WHITE, 0.5);
    TILECOLOR3 = ConvertToShadow (TILECOLOR3, WHITE, 0.5);
    
    
    BACKGROUNDSHADOW = ConvertToShadow (BACKGROUNDCOLOR, SHADOWCOLOR, SHADOW_ALPHA);
    TILESHADOW = ConvertToShadow (TILECOLOR, SHADOWCOLOR, SHADOW_ALPHA);
    TILESHADOW2 = ConvertToShadow (TILECOLOR2, SHADOWCOLOR, SHADOW_ALPHA);
    TILESHADOW3 = ConvertToShadow (TILECOLOR3, SHADOWCOLOR, SHADOW_ALPHA);
    // RAILINGCOLOR = ConvertToShadow (WALLWALLCOLOR, SHADOWCOLOR, 0.2);
    RAILINGCOLOR = ConvertToShadow (WALLWALLCOLOR, TILECOLOR, 0.75);
    // RAILINGCOLOR = ConvertToShadow (RAILINGCOLOR, SHADOWCOLOR, 0.35);
    RAILINGSHADOW = ConvertToShadow (RAILINGCOLOR, SHADOWCOLOR, 0.5);
    // RAILINGCOLOR = INDIANRED;
    
    TOPTHINGCOLOR = ConvertToShadow (WALLWALLCOLOR, WHITE, 0.5);
    // TOPTHINGSHADOW = ConvertToShadow (TOPTHINGCOLOR, SHADOWCOLOR, SHADOW_ALPHA);
    TOPTHINGSHADOW = ConvertToShadow (TOPTHINGCOLOR, SHADOWCOLOR, 0.25);
    
    // initialize arms
    Larm = Arm ('L');
    Rarm = Arm ('R');
    
    // LCD.SetBackgroundColor (BLACK);
}


void DrawEverything () {
    //if (state != 10) {
    // LCD.SetFontColor (BLACK); // idk this bit may work better than the clear command
    // LCD.FillRectangle (0,0,320,240);
    //}
    if (stateChange == true) {
        if (state == 10) {
            LCD.Clear (BACKGROUNDCOLOR); // clear the screen (for animation purposes)
            PreDraw ();
        } else {
            LCD.Clear (WALLCOLOR); // this may break the game for some reason
        }
        stateChange = false;
    }
    
    switch (tempState) {
        case 0: // splash screen state
            if (firstTimeEverDrawn) {
                LCD.Clear (WALLCOLOR);
                firstTimeEverDrawn = false;
            }
            DrawSplashScreen ();
            break;
        case 1: // main menu state
            DrawMainMenu (); // draw the main menu
            break;
        case 2: // rules and instructions state
            DrawRules (); // draw the rules/help screen
            break;
        case 3: // stats state
            DrawStats (); // draw the stats screen
            break;
        case 4: // credit state
            DrawCredits (); // draw the credits screen
            break;
        case 10: // in-game state
            if (highDetail) {
                PreDraw ();
            }
            DrawGame ();
            if (highDetail == false) {
                playerObj.prevPos = playerObj.pos; // update what the players last position was since the frame had been update (for the purpose of erasing/clearing the player)
                playerObj.prevDir = playerObj.dir;
                wfs.prevPos = wfs.pos;
                wfs.prevBounds = wfs.bounds;
                Larm.prevBounds = Larm.bounds;
                Rarm.prevBounds = Rarm.bounds;
            }
            break;
        default:
            break;
    }
}


// do all of the stuff for the main game loop that gets repeated
void mainLoop () {
    // LCD.Clear (BLACK); // clear the screen (for animation purposes)
    touchState = LCD.Touch (&touch.x, &touch.y); // get the user's touch input coordinates, and store whether or not the user is touching the screen in the variable touchState
    touch = Vector2 (touch.x, -touch.y); // standardize the touch coordinates so they're effectively in the fourth quadrant
    
    // perform all game actions (i.e. check for inputs, calculate physics, etc.) except for updating the screen
    //cout << "stuff be doin";
    switch (tempState) {
        case 0: // splash screen state
            if (firstTimeEverLoaded) {
                firstTimeEverDrawn = true;
            }
            if (touchState) {
                state = 1;
                gameJustStarted = true;
                stateChange = true;
            }
            break;
        case 1: // main menu state
            // check for touch inputs on the main menu
            if (startButt.WasPressed (touch, touchState) == true) { // was IsBeingPressed ()
                state = 10;
                gameJustStarted = true;
                stateChange = true;
            } else if (helpButt.WasPressed (touch, touchState) == true) {
                state = 2;
                stateChange = true;
            } else if (statsButt.WasPressed (touch, touchState) == true) {
                state = 3;
                stateChange = true;
            } else if (creditsButt.WasPressed (touch, touchState) == true) {
                state = 4;
                stateChange = true;
            }
            // DrawMainMenu (); // draw the main menu
            break;
        case 2: // rules and instructions state
            // check for touch inputs
            if (backButt.WasPressed (touch, touchState) == true) {
                state = 1;
                stateChange = true;
            }
            // DrawRules (); // draw the rules/help screen
            break;
        case 3: // stats state
            // check for touch inputs
            if (backButt.WasPressed (touch, touchState) == true) {
                state = 1;
                stateChange = true;
            }
            // DrawStats (); // draw the stats screen
            break;
        case 4: // credit state
            // check for touch inputs
            if (backButt.WasPressed (touch, touchState) == true) {
                state = 1;
                stateChange = true;
            }
            // DrawCredits (); // draw the credits screen
            break;
        case 10: // in-game state
            // check for in-game back-button touch inputs
            if (inGameBackButt.WasPressed (touch, touchState) == true) {
                UpdateStats ();
                ResetGame ();
                state = 1;
                stateChange = true;
            } else {
                GameLoop (); // do all the game stuff
            }
            break;
        default:
            break;
    }
    
    tempState = state;
    
    timeSpentRendering = TimeNow ();
    
    // update the screen only after a fixed time interval has passed
    if (TimeNow () - timeSinceLastFrameUpdate > UPDATE_INTERVAL) { // I've heard TimeNow is bad, so the game may break after a certain amount of time has passed
        timeSinceLastFrameUpdate = TimeNow ();
        
        glutPostRedisplay ();
    }
    
    tempState = state;
    
    timeSpentRendering = TimeNow () - timeSpentRendering; // determine the time spent rendering (later used to calculate the normalized acceleration amount for the player)
    
    // wait a really small amount of time
    // Sleep (100); // or not
    
    timeSinceIteration = TimeNow ();
}






/************************************************************************************************ ADDITIONAL FUNCTIONS ************************************************************************************************/




/************************************************************************ GUI FUNCTIONS ************************************************************************/



// draws splash screen
void DrawSplashScreen () {
    LCD.SetFontColor (WHITE);
    LCD.WriteRC ("Wet-floor Sign", 10, 5);
    LCD.WriteRC ("Nabbing", 10, 6);
    LCD.WriteRC ("Touch to begin", 10, 8);
    wfs = WFS (  Box ( Vector2 (30, -60) , 36, 56)  );
    wfs.DrawWFS ();
}


// reference for global variables for GUI
/*
 #define BUTT_WIDTH 100
 #define BUTT_HEIGHT 30
 Button startButt = Button (  Box ( Vector2 (30, -60), BUTT_WIDTH, BUTT_HEIGHT ), "START", 0  );
 Button helpButt = Button (  Box ( Vector2 (30, -105), BUTT_WIDTH, BUTT_HEIGHT ), "HELP", 0  );
 Button statsButt = Button (  Box ( Vector2 (30, -150), BUTT_WIDTH, BUTT_HEIGHT ), "STATISTICS", 0  );
 Button creditsButt = Button (  Box ( Vector2 (30, -195), BUTT_WIDTH, BUTT_HEIGHT ), "CREDITS", 0  );
 Button backButt = Button (  Box ( Vector2 (145, -30), 50, 30 ), "BACK", 0  );
 */

#define HEADER_LINE_WIDTH 150
#define HEADER_MARGIN 5

// draws main menu screen
void DrawMainMenu () {
    LCD.SetBackgroundColor (WALLCOLOR);
    LCD.SetFontColor (WHITE);
    LCD.WriteAt ( "MAIN MENU", 30, -(-30 + HEADER_MARGIN) );
    LCD.DrawLine (30, 45, HEADER_LINE_WIDTH, 45);
    
    startButt.DrawButton ();
    helpButt.DrawButton ();
    statsButt.DrawButton ();
    creditsButt.DrawButton ();
}


// draws rules screen
void DrawRules () {
    LCD.SetBackgroundColor (WALLCOLOR);
    LCD.SetFontColor (WHITE);
    LCD.WriteAt ( "HOW TO PLAY", 30, -(-30 + HEADER_MARGIN) );
    LCD.DrawLine (30, 45, HEADER_LINE_WIDTH, 45);
    
    LCD.WriteAt ( "Nab wet-floor signs.", 30, -(-60) ); // edited
    LCD.WriteAt ( "Avoid cops.", 30, -(-60-30*1) );
    LCD.WriteAt ( "Don't die.", 30, -(-60-30*2) );
    LCD.WriteAt ( "Leave.", 30, -(-60-30*3) );
    
    backButt.DrawButton ();
}


// draws stats screen
void DrawStats () {
    LCD.SetBackgroundColor (WALLCOLOR);
    LCD.SetFontColor (WHITE);
    LCD.WriteAt ( "STATISTICS", 30, -(-30 + HEADER_MARGIN) );
    LCD.DrawLine (30, 45, HEADER_LINE_WIDTH, 45);
    
    LCD.WriteAt ( "Highest Level: ", 30, -(-60) );
    LCD.WriteAt ( highScore, 210, -(-60) );
    
    backButt.DrawButton ();
}


// draws credits screen
void DrawCredits () {
    LCD.SetBackgroundColor (WALLCOLOR);
    LCD.SetFontColor (WHITE);
    LCD.WriteAt ( "CREDITS", 30, -(-30 + HEADER_MARGIN) );
    LCD.DrawLine (30, 45, HEADER_LINE_WIDTH, 45);
    
    LCD.WriteAt ( "Created by Fletcher", 30, -(-60) );
    LCD.WriteAt ( "Wells & Dough Van", 30, -(-60-30*1) ); // edited
    LCD.WriteAt ( "Arsdale using OSU's", 30, -(-60-30*2) );
    LCD.WriteAt ( "FEH software.", 30, -(-60-30*3) );
    
    backButt.DrawButton ();
}





/************************************************************************ IN-GAME FUNCTIONS ************************************************************************/





// draws given box
void DrawBox (Box box) {
    LCD.FillRectangle ( (int)(box.verts[0].x), -(int)(box.verts[0].y), box.width, box.height);
}



// draws given edge
void DrawEdge (Edge edge) {
    LCD.DrawLine (  (int)(edge.verts[0].x), -(int)(edge.verts[0].y), (int)(edge.verts[1].x), -(int)(edge.verts[1].y)  );
}



// erases the player at its position since the last time the screen was updated
void ErasePlayer () {
    LCD.SetFontColor (BACKGROUNDCOLOR);
    LCD.FillRectangle ((int)(playerObj.prevPos.x), -(int)(playerObj.prevPos.y), (int)(playerObj.bounds.width), (int)(playerObj.bounds.height));
    
    // Vector2 pos = playerObj.prevPos;
    DrawBox (Larm.prevBounds);
    DrawBox (Rarm.prevBounds);
    
    // erase the wet floor sign
    if (wfs.heldByPlayer && playerIsDead == false) {
        // Vector2 pos = Vector2 (wfs.pos.x - playerObj.pos.x + playerObj.prevPos.x,  wfs.pos.y - playerObj.pos.y + playerObj.prevPos.y);
        Vector2 pos = wfs.prevPos;
        Box bounds = wfs.prevBounds;
        LCD.FillRectangle ((int)(pos.x), -(int)(pos.y), (int)(bounds.width), (int)(bounds.height));
    }
}



// draws parts of the game scene that will only be drawn once
void PreDraw () {
    LCD.Clear (BACKGROUNDCOLOR);
    // DrawBoxesAndEdges (); // not used
    DrawLevel ();
    for (int j = 0; j < level && j < MAX_COPS; j++) {
        cops [j].DrawCop ();
    }
    if (highDetail == false) {
        wfs.DrawWFS ();
    }
}

bool leftWasInp;
bool upWasInp;
bool rightWasInp;
bool downWasInp;
bool anyInps;

float startDisplacement;
int exitIndex;
#define REMEMBER_DOORWAY_POS 1

// does all the game stuff (like a main function, except exclusively for while the state is in-game)
void GameLoop () {
    if (playerObj.pos.x >= 319 && wfs.heldByPlayer) { // check to see if the player has made it through the tunnel
        gameJustStarted = true;
        level++;
        startDisplacement = playerObj.pos.y - tunnelEdges [exitIndex - 1].verts[0].y;
    }
    if (gameJustStarted == true) { // checks if the game just started
        GenerateLevel (); // generate rooms and tunnels for level
        
        // if this is the first time the game began
        if (level == 1  ||  REMEMBER_DOORWAY_POS) {
            startDisplacement =  -(TILE_SIZE * (TUNNEL_TILE_SIZE/2.0))/2.0;
        }
        
        // (improve these later)
        playerObj.pos = Vector2 (0, tunnelEdges [0].verts[0].y + startDisplacement - playerObj.bounds.height / 2.0); // place the player at the entrance of the tunnel
        // wfs = WFS (  Box ( Vector2 ( tunnelEdges [4].verts[0].x - 4.20, tunnelEdges [4].verts[0].y - radius/2 - 14.0/2.0) , 9, 14)  ); // place the wet floor sign // - playerObj.bounds.height/2.0
        playerObj.prevPos = playerObj.pos;
        
        bool withinTunnel;
        do {
            withinTunnel = true;
            wfs = WFS (  Box ( Vector2 (rand() % ((int)(SCREEN_WIDTH * 0.35)) + SCREEN_WIDTH * 0.45, -(rand() % (SCREEN_HEIGHT-21) + 7)) , 9, 14)  );
            for (int k = 0; k < numTunnelBoxes; k++) {
                //if (backgroundBoxes [k].IsWithinBounds (wfs.bounds.getCenter())) {
                if (  backgroundBoxes [k].IsWithinBounds (wfs.bounds.verts [0]) || backgroundBoxes [k].IsWithinBounds (wfs.bounds.verts [3]) || backgroundBoxes [k].IsWithinBounds (wfs.bounds.verts [1]) || backgroundBoxes [k].IsWithinBounds (wfs.bounds.verts [2])  ) {
                    withinTunnel = false;
                }
            }
        } while (withinTunnel == false);
        
        wfs.heldByPlayer = false;
        
        for (int j = 0; j < level && j < MAX_COPS; j++) { // randomly place the cops
            bool copWithinTunnel;
            do {
                copWithinTunnel = true;
                cops [j] = Cop (  Box ( Vector2 (rand() % SCREEN_WIDTH, -(rand() % SCREEN_HEIGHT)) , 16, 16)  );
                for (int k = 0; k < numTunnelBoxes; k++) {
                    // if (backgroundBoxes [k].IsWithinBounds (cops [j].eyePoint)) {
                    if (backgroundBoxes [k].IsWithinBounds (cops [j].bounds) || backgroundBoxes [k].IsWithinBounds (cops [j].shootPoint)) {
                        copWithinTunnel = false;
                    }
                }
            } while (copWithinTunnel == false);
        }
        
        PreDraw (); // should be enabled
        gameJustStarted = false;
    }

    if (playerIsDead == false) { // check if the player is dead
        // float accelAmt = 30 * (TimeNow() - timeSinceIteration - timeSpentRendering); //initialize acceleration amount, normalized with respect to time
        float accelAmt = 0.00005 * FRS_0;
        Vector2 accel = Vector2 (); //initialize acceleration input vector
        
        // get the inputs from the user (right left up or down)
        if ( leftButt.IsBeingPressed (touch) || leftKeyInp) {
            leftInp = true;
            if (leftWasInp == false) { // check if the button was pressed down previously
                playerObj.dir = 1;
                leftWasInp = true; // and note that the button has now been pressed down if it do be that way
            }
        } else {
            leftInp = false;
            leftWasInp = false; // make the button no longer be noted as pressed
        }
        if ( upButt.IsBeingPressed (touch) || upKeyInp ) {
            upInp = true;
            if (upWasInp == false) {
                playerObj.dir = 2;
                upWasInp = true;
            }
        } else {
            upInp = false;
            upWasInp = false;
        }
        if ( rightButt.IsBeingPressed (touch) || rightKeyInp ) {
            rightInp = true;
            if (rightWasInp == false) {
                playerObj.dir = 3;
                rightWasInp = true;
            }
        } else {
            rightInp = false;
            rightWasInp = false;
        }
        if ( downButt.IsBeingPressed (touch) || downKeyInp ) {
            downInp = true;
            if (downWasInp == false) {
                playerObj.dir = 0;
                downWasInp = true;
            }
        } else {
            downInp = false;
            downWasInp = false;
        }
        
        // store whether there are any inputs given in the variable anyInps
        if (leftInp || upInp || rightInp || downInp) {
            anyInps = true;
        } else {
            anyInps = true; // this variable doesn't actually do what its suppose to do but I'm to lazy to change the rest of my code
        }
        
        // depending on input, add acceleration amount to input vector
        if ( leftInp ) {
            accel.x = accel.x - accelAmt;
        } if ( upInp ) {
            accel.y = accel.y + accelAmt;
        } if ( rightInp ) {
            accel.x = accel.x + accelAmt;
        } if ( downInp ) {
            accel.y = accel.y - accelAmt;
        }
        
        // if there is an acceleration due to a collision with a bullet, then apply it (and reset the acceleration due to the bullet)
        if (accelBull.x != 0 && accelBull.y != 0) { // this if statement may not work properly due to floating point numbers being weird and can either way be removed entirely
            accel = Vector2 (accel.x + accelBull.x, accel.y + accelBull.y); // apply the acceleration
            accelBull = Vector2 (); // reset accelBull
        }
        
        // accel = Vector2 (accel.x + (touch.x - playerObj.pos.x) / 5.0, accel.y + (touch.y - playerObj.pos.y) / 5.0); // (temporary) add acceleration based on relative touch position to the player
        // accel = Vector2 (accel.x, accel.y - 0.0); // add gravity to input vector (add a fixed negative downward acceleration)
        
        // // (optional) normalize the acceleration input vector
        playerObj.Translate (accel); // call the translate function, which calculates the translation of the object and updates its position, and takes the users inputted acceleration as a parameter
    }
    
    // update arm positions
    UpdateArms ();
    // detect if the player should pick up the wet floor sign
    if (wfs.heldByPlayer == false) {
        playerObj.bounds.UpdatePosition (playerObj.pos); // update the player's bounds position
        if (Distance (playerObj.pos, wfs.pos) < wfs.bounds.height*2) {
            if (playerObj.bounds.IsWithinBounds (wfs.bounds) || wfs.bounds.IsWithinBounds (Larm.bounds) || wfs.bounds.IsWithinBounds (Rarm.bounds)) {
                wfs.heldByPlayer = true;
            }
        }
    }
    
    // update position of the wet floor sign if the player is holding it
    if (wfs.heldByPlayer) {
        float dist;
        dist = Larm.length;
        switch (playerObj.dir) { // depending on which direction the player is facing
            case (0): // down
                // wfs.pos = Vector2 (playerObj.pos.x + 1 * (SPRITESCALE/2.0), playerObj.pos.y - 22 * (SPRITESCALE/2.0));
                wfs.pos = Vector2 (playerObj.pos.x + 1 * (SPRITESCALE/2.0), playerObj.pos.y - 16 * (SPRITESCALE/2.0) - dist);
                wfs.bounds = Box (wfs.pos, 14, 9);
                break;
            case (1): // left
                // wfs.pos = Vector2 (playerObj.pos.x + (-15 * SPRITESCALE/2.0), playerObj.pos.y - 1 * (SPRITESCALE/2.0));
                wfs.pos = Vector2 (playerObj.pos.x + (-9 * SPRITESCALE/2.0) - dist, playerObj.pos.y - 1 * (SPRITESCALE/2.0));
                wfs.bounds = Box (wfs.pos, 9, 14);
                break;
            case (2): // up
                // wfs.pos = Vector2 (playerObj.pos.x + 1 * (SPRITESCALE/2.0), playerObj.pos.y + 15 * (SPRITESCALE/2.0));
                wfs.pos = Vector2 (playerObj.pos.x + 1 * (SPRITESCALE/2.0), playerObj.pos.y + 9 * (SPRITESCALE/2.0) + dist);
                wfs.bounds = Box (wfs.pos, 14, 9);
                break;
            case (3): // right
                // wfs.pos = Vector2 (playerObj.pos.x + (11 * SPRITESCALE), playerObj.pos.y - 1 * (SPRITESCALE/2.0));
                wfs.pos = Vector2 (playerObj.pos.x + 16 * (SPRITESCALE/2.0) + dist, playerObj.pos.y - 1 * (SPRITESCALE/2.0));
                wfs.bounds = Box (wfs.pos, 9, 14);
                break;
            default:
                wfs.pos = Vector2 (playerObj.pos.x + (SPRITESCALE/2.0), playerObj.pos.y - 1 * (SPRITESCALE));
                break;
        }
    }
    
    // make the cops do cop things and check if the cop has killed the player
    for (int j = 0; j < level && j < MAX_COPS; j++) {
        // wfs.bounds.UpdateVerts();
        cops [j].Patrol ();
        if (cops[j].isShooting) {
            // bool shot = playerObj.IsWithinBounds (cops [j].bulletPos, 3); // this function just doesn't work
            // cout << "Player x = " << playerObj.pos.x << ", y = " << playerObj.pos.y << endl;
            // cout << "Bullet x = " << cops [j].bulletPos.x << ", y = " << cops [j].bulletPos.y << endl;
            float dist = Distance (  Vector2 (playerObj.pos.x + playerObj.bounds.width/2.0, playerObj.pos.y - playerObj.bounds.height/2.0), cops[j].bulletPos  );
            if (dist < playerObj.bounds.width/2.0) { // || shot
                if (playerIsDead == false) { // -playerIsDead would look dank if screen wasn't cleared each time (and reset iterations since death)
                    bullDeathVel = cops[j].bulletDir;
                    deathVel = playerObj.vel;
                    float magnitude = Distance (Vector2 (), deathVel);
                    deathVel = Vector2 (2 * deathVel.x / magnitude, 2 * deathVel.y / magnitude);
                    bSR[0] = (rand() % 100)/100.0; // random num between 0 and 1
                    bSR[1] = (rand() % 100)/100.0;
                    bSR[2] = (rand() % 100)/100.0;
                    bSR[3] = (rand() % 100)/100.0;
                    bSR[4] = (rand() % 100)/100.0; // random num between 0 and 1
                    bSR[5] = (rand() % 100)/100.0;
                    bSR[6] = (rand() % 100)/100.0;
                    bSR[7] = (rand() % 100)/100.0;
                }
                playerIsDead = true;
                wfs.heldByPlayer = false;
            }
        }
    }
    
    if (iterationsSinceDeath > 1000) {
        UpdateStats ();
        ResetGame ();
        state = 1;
        stateChange = true;
    }
}



// draws the game scene
void DrawGame () {
    // DrawLevel (); // done elsewhere
    
    if (highDetail  &&  RIGHT_ONLY) {
        DrawLevelForeground ();
    }
    
    if (highDetail == false) {
        if (playerIsDead == false) {
            ErasePlayer (); // erases the player at its position since the last time the screen was updated
            if (wfs.heldByPlayer) {
                wfs.DrawWFS ();
            }
        }
    } else {
        wfs.DrawWFS ();
    }
    DrawPlayer (); // draws the player at its updated position
    
    if (highDetail) {
        for (int j = 0; j < level && j < MAX_COPS; j++) {
            cops [j].DrawBullet ();
        }
    } else {
        for (int j = 0; j < level && j < MAX_COPS; j++) {
            cops [j].DrawBullet ();
            cops [j].DrawGraphic ();
        }
    }
    
    if (highDetail  &&  !RIGHT_ONLY) {
        DrawLevelForeground ();
    }
    
    DrawInGameGUI ();
}



// definitions for custom colors
#define WHITEFLESH              0x986954
#define WHITENOSE               0x563024
#define WHITEMOUTH              0x31180b
#define WHITEIRIS               0x3f2a75
#define WHITEHAIR               0x20160c
#define BLACKFLESH              0x32160A
#define BLACKNOSE               0x100905
#define BLACKMOUTH              0x763A2F



// draws the player  (Note dimensions of written char is 12x17)
void DrawPlayer () {
    // int SPRITESCALE = 2;
    
    Vector2 pos = playerObj.pos; // create copy of playerObj position
    
    if (playerIsDead == false) {
        // draw head #986954
        LCD.SetFontColor (WHITEFLESH); // PAPAYAWHIP, cornsilk, tan, wheat, burlywood, blanchedalmond
        LCD.FillRectangle ((int)(pos.x), -(int)(pos.y), (int)(playerObj.bounds.width), (int)(playerObj.bounds.height));
        // LCD.FillRectangle ((int)(pos.x), -(int)(pos.y), 8*SCALE, 8*SCALE); // for reference
        switch (playerObj.dir) { // draw the player differently depending on which direction they are facing
            case (0): // downward
                if (anyInps || wfs.heldByPlayer) {
                    // draw arms
                    Larm.DrawArm ();
                    Rarm.DrawArm ();
                    // LCD.FillRectangle ((int)(pos.x) + 1 * SPRITESCALE, -(int)(pos.y) + 8 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE);
                    // LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) + 8 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE);
                }
                // draw nose #563024
                LCD.SetFontColor (WHITENOSE); // SADDLEBROWN
                LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) + 5 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
                // draw mouth #31180b
                LCD.SetFontColor (WHITEMOUTH); // ROSYBROWN
                LCD.FillRectangle ((int)(pos.x) + 2 * SPRITESCALE, -(int)(pos.y) + 6 * SPRITESCALE, 4 * SPRITESCALE, 1 * SPRITESCALE);
                // draw left eye-white #ffffff
                LCD.SetFontColor (WHITE);
                LCD.FillRectangle ((int)(pos.x) + 1 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
                // draw right eye-white
                LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
                // draw left pupil #3f2a75
                LCD.SetFontColor (WHITEIRIS); // BLUEVIOLET
                LCD.FillRectangle ((int)(pos.x) + 2 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
                // draw right pupil
                LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
                // draw top hair #20160c
                LCD.SetFontColor (WHITEHAIR); // SIENNA
                LCD.FillRectangle ((int)(pos.x) + 0 * SPRITESCALE, -(int)(pos.y) + 0 * SPRITESCALE, 8 * SPRITESCALE, 2 * SPRITESCALE);
                // draw left hair
                LCD.FillRectangle ((int)(pos.x) + 0 * SPRITESCALE, -(int)(pos.y) + 2 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
                // draw right hair
                LCD.FillRectangle ((int)(pos.x) + 7 * SPRITESCALE, -(int)(pos.y) + 2 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
                break;
            case (1): // leftward
                if (anyInps || wfs.heldByPlayer) {
                    // draw arm
                    Larm.DrawArm ();
                    // Rarm.DrawArm ();
                    // LCD.FillRectangle ((int)(pos.x) + -3 * SPRITESCALE, -(int)(pos.y) + 6 * SPRITESCALE, 3 * SPRITESCALE, 2 * SPRITESCALE);
                }
                // draw left eye-white #ffffff
                LCD.SetFontColor (WHITE);
                LCD.FillRectangle ((int)(pos.x) + 1 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
                // draw left pupil #3f2a75
                LCD.SetFontColor (WHITEIRIS); // BLUEVIOLET
                LCD.FillRectangle ((int)(pos.x) + 1 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
                // draw top hair #20160c
                LCD.SetFontColor (WHITEHAIR); // SIENNA
                LCD.FillRectangle ((int)(pos.x) + 0 * SPRITESCALE, -(int)(pos.y) + 0 * SPRITESCALE, 8 * SPRITESCALE, 3 * SPRITESCALE);
                // draw right hair
                LCD.FillRectangle ((int)(pos.x) + 7 * SPRITESCALE, -(int)(pos.y) + 3 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
                break;
            case (2): // upward
                if (anyInps || wfs.heldByPlayer) {
                    // draw arms
                    Larm.DrawArm ();
                    Rarm.DrawArm ();
                    // LCD.FillRectangle ((int)(pos.x) + 1 * SPRITESCALE, -(int)(pos.y) - 3 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE);
                    // LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) - 3 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE);
                }
                // draw top hair #20160c
                LCD.SetFontColor (WHITEHAIR); // SIENNA
                LCD.FillRectangle ((int)(pos.x) + 0 * SPRITESCALE, -(int)(pos.y) + 0 * SPRITESCALE, 8 * SPRITESCALE, 6 * SPRITESCALE);
                // draw bottom hair
                LCD.FillRectangle ((int)(pos.x) + 2 * SPRITESCALE, -(int)(pos.y) + 6 * SPRITESCALE, 4 * SPRITESCALE, 1 * SPRITESCALE);
                break;
            case (3): // rightward
                if (anyInps || wfs.heldByPlayer) {
                    // draw arm
                    // Larm.DrawArm ();
                    Rarm.DrawArm ();
                    // LCD.FillRectangle ((int)(pos.x) + 8 * SPRITESCALE, -(int)(pos.y) + 6 * SPRITESCALE, 3 * SPRITESCALE, 2 * SPRITESCALE);
                }
                // draw right eye-white
                LCD.SetFontColor (WHITE);
                LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
                // draw right pupil
                LCD.SetFontColor (WHITEIRIS);
                LCD.FillRectangle ((int)(pos.x) + 6 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
                // draw top hair #20160c
                LCD.SetFontColor (WHITEHAIR); // SIENNA
                LCD.FillRectangle ((int)(pos.x) + 0 * SPRITESCALE, -(int)(pos.y) + 0 * SPRITESCALE, 8 * SPRITESCALE, 3 * SPRITESCALE);
                // draw left hair
                LCD.FillRectangle ((int)(pos.x) + 0 * SPRITESCALE, -(int)(pos.y) + 3 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
                break;
        }
        
        LCD.SetFontColor (RED);
    } else {
        // bool goreMode = true;
        int bloodStartIndex = iterationsSinceDeath-1;
        // draw animated blood circles traveling outward from the players center
        // LCD.SetFontColor (DARKRED);
        LCD.SetFontColor (0.33333, 0, 0);
        // float bleedSpeed = .5f;
        float BVS = 30*4; // bullet velocity scale (the influence the velocity of the bullet has on the velocity of the blood)
        float OVS = 25*4; // outward velocity scale
        
        Vector2 center = Vector2 (  pos.x + playerObj.bounds.width/2.0,  pos.y - playerObj.bounds.height/2.0  ); // set the target to equal the center of the player
        Vector2 vel = bullDeathVel;
        // deathVel = Vector2 (deathVel.x, deathVel.y);
        // int bloodRadius = (int)(playerObj.bounds.width / (1.0 + iterationsSinceDeath/25.0));
        Box bounds = playerObj.bounds;
        
        if (goreMode) {
            LCD.FillCircle ((int)(center.x), -(int)(center.y), bounds.width/2.3 * pow (2.0, 0.5)); // center
            bloodStartIndex = 0;
        }
        bounds.width = bounds.width/3.1415926535; // modify the bounds...
        bounds.height = bounds.height/3.1415926535;
        for (int j = bloodStartIndex; j < iterationsSinceDeath; j++) {
            // float squirterations = pow (iterationsSinceDeath, .5);
            float q = pow ((float)(j), .05);
            if (j != 0) q = q - pow ((float)(1), .05);
            float colorBoi = 0.66667 * (float)(j) / (float)(iterationsSinceDeath);
            // LCD.SetFontColor (0.33333 + colorBoi, 0, 0);
            int hexValue = ConvertColor (0.33333 + colorBoi, 0, 0);
            LCD.SetFontColor (hexValue);
            int bloodRadius = (int)(playerObj.bounds.width / (4.0 + j/3.0));
            LCD.FillCircle ((int)(center.x - bounds.width * bSR[1] + q * (bSR[0] * (vel.x + deathVel.x)*BVS  -  1*OVS * bSR[4])), -(int)(center.y + bounds.height * bSR[3] + q * (bSR[0] * (vel.y + deathVel.y)*BVS  +  1*OVS * bSR[7])), bloodRadius); // left up
            LCD.FillCircle ((int)(center.x + bounds.width * bSR[2] + q * (bSR[1] * (vel.x + deathVel.x)*BVS  +  1*OVS * bSR[5])), -(int)(center.y + bounds.height * bSR[4] + q * (bSR[0] * (vel.y + deathVel.y)*BVS  +  1*OVS * bSR[6])), bloodRadius); // right up
            LCD.FillCircle ((int)(center.x + bounds.width * bSR[3] + q * (bSR[2] * (vel.x + deathVel.x)*BVS  +  1*OVS * bSR[6])), -(int)(center.y - bounds.height * bSR[5] + q * (bSR[0] * (vel.y + deathVel.y)*BVS  -  1*OVS * bSR[5])), bloodRadius); // right down
            LCD.FillCircle ((int)(center.x - bounds.width * bSR[4] + q * (bSR[3] * (vel.x + deathVel.x)*BVS  -  1*OVS * bSR[7])), -(int)(center.y - bounds.height * bSR[6] + q * (bSR[0] * (vel.y + deathVel.y)*BVS  -  1*OVS * bSR[4])), bloodRadius); // left down
            
            LCD.FillCircle ((int)(center.x - bounds.width + q * (bSR[5] * (vel.x + deathVel.x)*BVS  -  1*OVS * bSR[5])), -(int)(center.y + 0 + q * (bSR[3] * (vel.y + deathVel.y)*BVS  +  0*OVS * bSR[2])), bloodRadius); // left ... 1, 6
            LCD.FillCircle ((int)(center.x + bounds.width + q * (bSR[2] * (vel.x + deathVel.x)*BVS  +  1*OVS * bSR[5])), -(int)(center.y + 0 + q * (bSR[0] * (vel.y + deathVel.y)*BVS  +  0*OVS * bSR[1])), bloodRadius); // right ... 1, 6
            LCD.FillCircle ((int)(center.x + q * (bSR[7] * (vel.x + deathVel.x)*BVS  +  0*OVS * bSR[4])), -(int)(center.y + bounds.height + q * (bSR[1] * (vel.y + deathVel.y)*BVS  +  1*OVS * bSR[5])), bloodRadius); // up ... 0, 7
            LCD.FillCircle ((int)(center.x + q * (bSR[6] * (vel.x + deathVel.x)*BVS  +  0*OVS * bSR[4])), -(int)(center.y - bounds.height + q * (bSR[2] * (vel.y + deathVel.y)*BVS  -  1*OVS * bSR[4])), bloodRadius); // down ... 0, 7
        }
        
        /*
        // for (int j = 0; j < iterationsSinceDeath; j++) {   ?
        // int bloodRadius = (int)((rand() % 50)/10.0 + playerObj.bounds.width / (2.0 + iterationsSinceDeath))
        int bloodRadius = (int)(playerObj.bounds.width / (1.0 + iterationsSinceDeath/25.0));
        float squirterations = pow (iterationsSinceDeath, 0.5);
        Vector2 center = Vector2 (  pos.x + playerObj.bounds.width/2.0,  pos.y - playerObj.bounds.height/2.0  ); // set the target to equal the center of the player
        float centerRadius = (squirterations * bleedSpeed + playerObj.bounds.width/2.0) * pow (2.0, 0.5); // hypotenuse
        
        LCD.FillCircle ((int)(center.x), -(int)(center.y), centerRadius); // center
        LCD.FillCircle ((int)(pos.x - squirterations * bleedSpeed), -(int)(pos.y + squirterations * bleedSpeed), bloodRadius); // left up
        LCD.FillCircle ((int)(pos.x + playerObj.bounds.width + squirterations * bleedSpeed), -(int)(pos.y + squirterations * bleedSpeed), bloodRadius); // right up
        LCD.FillCircle ((int)(pos.x + playerObj.bounds.width + squirterations * bleedSpeed), -(int)(pos.y - playerObj.bounds.height - squirterations * bleedSpeed), bloodRadius); // right down
        LCD.FillCircle ((int)(pos.x - squirterations * bleedSpeed), -(int)(pos.y - playerObj.bounds.height - squirterations * bleedSpeed), bloodRadius); // left down
        */
        
        iterationsSinceDeath++;
    }
}

// determine the arm's length
void Arm::DetermineLength () {
    Vector2 pos = playerObj.pos;
    
    dir = DirToIntVector2 (playerObj.dir);
    
    // determine the position the rays should be cast from
    switch (playerObj.dir) { // draw the player differently depending on which direction they are facing
        case (0):
            if (type == 'R') {
                rayPos0 = Vector2 ((pos.x) + 1 * SPRITESCALE, pos.y - 8 * SPRITESCALE);
                rayPos1 = Vector2 ((pos.x) + 3 * SPRITESCALE, pos.y - 8 * SPRITESCALE);
                // fucko ,  nobody will notice that the wet floor sign's collision is off by one pixel
                /*
                if (wfs.heldByPlayer) {
                    rayPos1 = Vector2 ((pos.x) + 0.5 * SPRITESCALE, pos.y - 8 * SPRITESCALE); // better mimic wet floor sign
                }
                */
            } else if (type == 'L') {
                rayPos0 = Vector2 ((pos.x) + 5 * SPRITESCALE, pos.y - 8 * SPRITESCALE);
                rayPos1 = Vector2 ((pos.x) + 7 * SPRITESCALE, pos.y - 8 * SPRITESCALE);
            }
            // bounds = Box (Vector2 ((int)(pos.x) + 1 * SPRITESCALE, (int)(pos.y) - 8 * SPRITESCALE), 6 * SPRITESCALE, 3 * SPRITESCALE);
            // LCD.FillRectangle ((int)(pos.x) + 1 * SPRITESCALE, -(int)(pos.y) + 8 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE);
            // LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) + 8 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE);
            break;
        case (1):
            rayPos0 = Vector2 (pos.x, (pos.y) - 6 * SPRITESCALE);
            rayPos1 = Vector2 (pos.x, (pos.y) - 8 * SPRITESCALE);
            // because right arm isn't shown...
            if (wfs.heldByPlayer && type == 'R') {
                rayPos1 = Vector2 (pos.x, (pos.y) - 1 * SPRITESCALE); // better mimic wet floor sign
            }
            break;
        case (2):
            if (type == 'R') {
                rayPos0 = Vector2 ((pos.x) + 1 * SPRITESCALE, pos.y);
                rayPos1 = Vector2 ((pos.x) + 3 * SPRITESCALE, pos.y);
            } else if (type == 'L') {
                rayPos0 = Vector2 ((pos.x) + 5 * SPRITESCALE, pos.y);
                rayPos1 = Vector2 ((pos.x) + 7 * SPRITESCALE, pos.y);
            }
            // bounds = Box (Vector2 ((int)(pos.x) + 1 * SPRITESCALE, (int)(pos.y) + 3 * SPRITESCALE), 6 * SPRITESCALE, 3 * SPRITESCALE);
            // LCD.FillRectangle ((int)(pos.x) + 1 * SPRITESCALE, -(int)(pos.y) - 3 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE);
            // LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) - 3 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE);
            break;
        case (3):
            rayPos0 = Vector2 (pos.x + 8 * SPRITESCALE, (pos.y) - 6 * SPRITESCALE);
            rayPos1 = Vector2 (pos.x + 8 * SPRITESCALE, (pos.y) - 8 * SPRITESCALE);
            if (wfs.heldByPlayer && type == 'L') {
                rayPos1 = Vector2 (pos.x + 8 * SPRITESCALE, (pos.y) - 1 * SPRITESCALE); // better mimic wet floor sign // should be 0.5
            }
            break;
        default:
            break;
    }
    
    
    // perform raycast in order to determine the length in which the arm should be drawn
    Vector2 direction = Vector2 (dir.x, dir.y);
    float raycastLength0 = DEFAULT_LENGTH;
    float raycastLength1 = DEFAULT_LENGTH;
    // if the player is holding the wet floor sign, make the raycast length include the width of the wfs
    float wfsWidth = 0;
    if (wfs.heldByPlayer) {
        if (wfs.bounds.width < wfs.bounds.height) {
            wfsWidth = wfs.bounds.width;
        } else {
            wfsWidth = wfs.bounds.height;
        }
        raycastLength0 = DEFAULT_LENGTH + wfsWidth;
        raycastLength1 = DEFAULT_LENGTH + wfsWidth;
    }
    // raycastLength0 = 99999;
    
    // cast the actual raycast
    ray0 = Raycast (rayPos0, direction, raycastLength0, 'a');
    ray1 = Raycast (rayPos1, direction, raycastLength1, 'a');
    
    // raycastLength0 = ray0.distance;
    // raycastLength1 = ray1.distance;
    
    if (ray1.hit) {
        length = ray1.distance - wfsWidth;
    } else if (ray0.hit) {
        length = ray0.distance - wfsWidth;
    } else {
        length = DEFAULT_LENGTH;
    }
    
    if (ray1.hit || ray0.hit) {
        switch (playerObj.dir) {
            case 0:
                length = length - 0;
                break;
            case 1:
                length = length - 1;
                break;
            case 2:
                length = length - 1;
                break;
            default:
                break;
        }
    }
    
    
}

// updates the bounds of the player's arms
void Arm::UpdateArmBounds () {
    switch (playerObj.dir) { // draw the player differently depending on which direction they are facing
        case (0):
            if (type == 'R') {
                // bounds = Box (Vector2 ((int)(pos.x) + 1 * SPRITESCALE, (int)(pos.y) - 8 * SPRITESCALE), 2 * SPRITESCALE, 3 * SPRITESCALE);
                bounds = Box (Vector2 (rayPos0.x, rayPos0.y), 2 * SPRITESCALE, length);
            } else if (type == 'L') {
                bounds = Box (Vector2 (rayPos0.x, rayPos0.y), 2 * SPRITESCALE, length);
            }
            // bounds = Box (Vector2 ((int)(pos.x) + 1 * SPRITESCALE, (int)(pos.y) - 8 * SPRITESCALE), 6 * SPRITESCALE, 3 * SPRITESCALE);
            // LCD.FillRectangle ((int)(pos.x) + 1 * SPRITESCALE, -(int)(pos.y) + 8 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE);
            // LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) + 8 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE);
            break;
        case (1):
            if (wfs.heldByPlayer) {
                length = length + holdLength;
            }
            
            bounds = Box (Vector2 (rayPos0.x - length, rayPos0.y), length, 2 * SPRITESCALE);
            
            if (wfs.heldByPlayer) {
                length = length - holdLength;
            }
            // bounds = Box (Vector2 ((int)(pos.x) + -3 * SPRITESCALE, (int)(pos.y) - 6 * SPRITESCALE), 3 * SPRITESCALE, 2 * SPRITESCALE);
            break;
        case (2):
            if (type == 'L') {
                bounds = Box (Vector2 (rayPos0.x, rayPos0.y + length), 2 * SPRITESCALE, length);
            } else if (type == 'R') {
                bounds = Box (Vector2 (rayPos0.x, rayPos0.y + length), 2 * SPRITESCALE, length);
            }
            // bounds = Box (Vector2 ((int)(pos.x) + 1 * SPRITESCALE, (int)(pos.y) + 3 * SPRITESCALE), 6 * SPRITESCALE, 3 * SPRITESCALE);
            // LCD.FillRectangle ((int)(pos.x) + 1 * SPRITESCALE, -(int)(pos.y) - 3 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE);
            // LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) - 3 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE);
            break;
        case (3):
            if (wfs.heldByPlayer) {
                length = length + holdLength;
            }
            
            bounds = Box (Vector2 (rayPos0.x, rayPos0.y), length, 2 * SPRITESCALE);
            
            if (wfs.heldByPlayer) {
                length = length - holdLength;
            }
            // bounds = Box (Vector2 ((int)(pos.x) + 8 * SPRITESCALE, (int)(pos.y) - 6 * SPRITESCALE), 3 * SPRITESCALE, 2 * SPRITESCALE);
            break;
        default:
            break;
    }
    
}

// draws the players; doesn't set the color (at present at least)
void Arm::DrawArm () {
    // LCD.SetFontColor (WHITEFLESH);
    DrawBox (bounds);
}


// determine the length of the player's arms
void UpdateArms () {
    // determine the lengths of each arm
    Larm.DetermineLength ();
    Rarm.DetermineLength ();
    // if the player is holding the wet floor sign, make sure the arms are the same length
    if (Larm.length < Rarm.length) {
        // dist = Larm.length;
        Rarm.length = Larm.length;
    } else {
        // dist = Rarm.length;
        Larm.length = Rarm.length;
    }
    // update the arms bounds
    Larm.UpdateArmBounds ();
    Rarm.UpdateArmBounds ();
}


// draws the wet floor sign
void WFS::DrawWFS () {
    int SCALE = SPRITESCALE/2;
    LCD.SetFontColor (GOLD);
    int dispY = 0;
    int dispX = 12 * SCALE;
    
    if ( (int)(bounds.height) == 56 ) { // determine if the wetfloor sign should be drawn for the splash screen
        SCALE = 8; // if so, increase the scale
        LCD.FillRectangle ((int)(pos.x) + 0 * SCALE, -(int)(pos.y) + 0 * SCALE, 9 * SCALE, 15 * SCALE);
    } else if (playerObj.dir == 0 || playerObj.dir == 2) {
        if (heldByPlayer) {
            // dispY = 2;
        } else if (playerIsDead) {
            // dispY = 2;
        }
    }
    dispY = dispY * SCALE;
    
    if (heldByPlayer  &&  (playerObj.dir == 0 || playerObj.dir == 2)) { // if the wet floor sign is horizontal
        if (playerObj.dir == 0) {
            // draw outside
            LCD.FillRectangle ((int)(pos.x), -(int)(pos.y), (int)(bounds.width), (int)(bounds.height));
            // draw triangle
            LCD.SetFontColor (BLACK);
            LCD.FillRectangle ((int)(pos.x) + 2 * SCALE, -(int)(pos.y + dispY) + 4 * SCALE, 1 * SCALE, 1 * SCALE); //    .
            LCD.FillRectangle ((int)(pos.x) + 3 * SCALE, -(int)(pos.y + dispY) + 3 * SCALE, 2 * SCALE, 1 * SCALE); //   |
            LCD.FillRectangle ((int)(pos.x) + 5 * SCALE, -(int)(pos.y + dispY) + 2 * SCALE, 2 * SCALE, 1 * SCALE); //  |
            LCD.FillRectangle ((int)(pos.x) + 7 * SCALE, -(int)(pos.y + dispY) + 1 * SCALE, 2 * SCALE, 1 * SCALE); // |
            LCD.FillRectangle ((int)(pos.x) + 3 * SCALE, -(int)(pos.y + dispY) + 5 * SCALE, 2 * SCALE, 1 * SCALE); //     |
            LCD.FillRectangle ((int)(pos.x) + 5 * SCALE, -(int)(pos.y + dispY) + 6 * SCALE, 2 * SCALE, 1 * SCALE); //      |
            LCD.FillRectangle ((int)(pos.x) + 7 * SCALE, -(int)(pos.y + dispY) + 7 * SCALE, 2 * SCALE, 1 * SCALE); //       |
            LCD.FillRectangle ((int)(pos.x) + 9 * SCALE, -(int)(pos.y + dispY) + 2 * SCALE, 1 * SCALE, 5 * SCALE); //  .....
            // draw !
            LCD.SetFontColor (RED);
            LCD.FillRectangle ((int)(pos.x) + 5 * SCALE, -(int)(pos.y + dispY) + 4 * SCALE, 2 * SCALE, 1 * SCALE); // |
            LCD.FillRectangle ((int)(pos.x) + 8 * SCALE, -(int)(pos.y + dispY) + 4 * SCALE, 1 * SCALE, 1 * SCALE); // .
        } else if (playerObj.dir == 2) {
            // draw outside
            LCD.FillRectangle ((int)(pos.x), -(int)(pos.y), (int)(bounds.width), (int)(bounds.height));
            // draw triangle
            LCD.SetFontColor (BLACK);
            LCD.FillRectangle ((int)(pos.x) - (2 - dispX - 1) * SCALE, -(int)(pos.y + dispY) + 4 * SCALE, 1 * SCALE, 1 * SCALE); //    .
            LCD.FillRectangle ((int)(pos.x) - (3 - dispX) * SCALE, -(int)(pos.y + dispY) + 3 * SCALE, 2 * SCALE, 1 * SCALE); //   |
            LCD.FillRectangle ((int)(pos.x) - (5 - dispX) * SCALE, -(int)(pos.y + dispY) + 2 * SCALE, 2 * SCALE, 1 * SCALE); //  |
            LCD.FillRectangle ((int)(pos.x) - (7 - dispX) * SCALE, -(int)(pos.y + dispY) + 1 * SCALE, 2 * SCALE, 1 * SCALE); // |
            LCD.FillRectangle ((int)(pos.x) - (3 - dispX) * SCALE, -(int)(pos.y + dispY) + 5 * SCALE, 2 * SCALE, 1 * SCALE); //     |
            LCD.FillRectangle ((int)(pos.x) - (5 - dispX) * SCALE, -(int)(pos.y + dispY) + 6 * SCALE, 2 * SCALE, 1 * SCALE); //      |
            LCD.FillRectangle ((int)(pos.x) - (7 - dispX) * SCALE, -(int)(pos.y + dispY) + 7 * SCALE, 2 * SCALE, 1 * SCALE); //       |
            LCD.FillRectangle ((int)(pos.x) - (9 - dispX - 1) * SCALE, -(int)(pos.y + dispY) + 2 * SCALE, 1 * SCALE, 5 * SCALE); //  .....
            // draw !
            LCD.SetFontColor (RED);
            LCD.FillRectangle ((int)(pos.x) - (5 - dispX) * SCALE, -(int)(pos.y + dispY) + 4 * SCALE, 2 * SCALE, 1 * SCALE); // |
            LCD.FillRectangle ((int)(pos.x) - (8 - dispX - 1) * SCALE, -(int)(pos.y + dispY) + 4 * SCALE, 1 * SCALE, 1 * SCALE); // .
        }
    } else {
        // draw outside
        LCD.FillRectangle ((int)(pos.x), -(int)(pos.y), (int)(bounds.width), (int)(bounds.height));
        // draw triangle
        LCD.SetFontColor (BLACK);
        LCD.FillRectangle ((int)(pos.x) + 4 * SCALE, -(int)(pos.y + dispY) + 2 * SCALE, 1 * SCALE, 1 * SCALE); //    .
        LCD.FillRectangle ((int)(pos.x) + 3 * SCALE, -(int)(pos.y + dispY) + 3 * SCALE, 1 * SCALE, 2 * SCALE); //   |
        LCD.FillRectangle ((int)(pos.x) + 2 * SCALE, -(int)(pos.y + dispY) + 5 * SCALE, 1 * SCALE, 2 * SCALE); //  |
        LCD.FillRectangle ((int)(pos.x) + 1 * SCALE, -(int)(pos.y + dispY) + 7 * SCALE, 1 * SCALE, 2 * SCALE); // |
        LCD.FillRectangle ((int)(pos.x) + 5 * SCALE, -(int)(pos.y + dispY) + 3 * SCALE, 1 * SCALE, 2 * SCALE); //     |
        LCD.FillRectangle ((int)(pos.x) + 6 * SCALE, -(int)(pos.y + dispY) + 5 * SCALE, 1 * SCALE, 2 * SCALE); //      |
        LCD.FillRectangle ((int)(pos.x) + 7 * SCALE, -(int)(pos.y + dispY) + 7 * SCALE, 1 * SCALE, 2 * SCALE); //       |
        LCD.FillRectangle ((int)(pos.x) + 2 * SCALE, -(int)(pos.y + dispY) + 9 * SCALE, 5 * SCALE, 1 * SCALE); //  .....
        // draw !
        LCD.SetFontColor (RED);
        LCD.FillRectangle ((int)(pos.x) + 4 * SCALE, -(int)(pos.y + dispY) + 5 * SCALE, 1 * SCALE, 2 * SCALE); // |
        LCD.FillRectangle ((int)(pos.x) + 4 * SCALE, -(int)(pos.y + dispY) + 8 * SCALE, 1 * SCALE, 1 * SCALE); // .
    }
}

#define COPCOLOR 0x09487E // color of a cops hat // 0x09487E // 0x3580C4 // 0x345387

// draws the cop
void Cop::DrawCop () {
    //int SPRITESCALE = 2;
    
    // draw head #986954
    LCD.SetFontColor (BLACKFLESH); // PAPAYAWHIP, cornsilk, tan, wheat, burlywood, blanchedalmond
    LCD.FillRectangle ((int)(pos.x), -(int)(pos.y), (int)(bounds.width), (int)(bounds.height));
    // LCD.FillRectangle ((int)(pos.x), -(int)(pos.y), 8*SCALE, 8*SCALE); // for reference
    
    if (alertState == false) {
        // draw left eye-white #ffffff
        LCD.SetFontColor (WHITE);
        LCD.FillRectangle ((int)(pos.x) + 1 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
        // draw right eye-white
        LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
        // draw nose #563024
        LCD.SetFontColor (BLACKNOSE); // SADDLEBROWN
        LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) + 5 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
        // draw mouth #31180b
        LCD.SetFontColor (BLACKMOUTH); // ROSYBROWN
        LCD.FillRectangle ((int)(pos.x) + 2 * SPRITESCALE, -(int)(pos.y) + 6 * SPRITESCALE, 4 * SPRITESCALE, 1 * SPRITESCALE);
        // draw left pupil #3f2a75
        LCD.SetFontColor (BLACK); // BLUEVIOLET
        LCD.FillRectangle ((int)(pos.x) + 2 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
        // draw right pupil
        LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
        // draw top of hat
        LCD.SetFontColor (COPCOLOR); // blue
        LCD.FillRectangle ((int)(pos.x) + -2 * SPRITESCALE, -(int)(pos.y) + -1 * SPRITESCALE, 12 * SPRITESCALE, 3 * SPRITESCALE);
        // draw badge
        LCD.SetFontColor (GOLD);
        LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) + 0 * SPRITESCALE, 2 * SPRITESCALE, 2 * SPRITESCALE);
    } else if (alertState == 2) { // if the cop is alerted and shooting
        // draw left eye-white #ffffff
        LCD.SetFontColor (WHITE);
        LCD.FillRectangle ((int)(pos.x) + 1 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
        // draw right eye-white
        LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
        // draw nose #563024
        LCD.SetFontColor (BLACKNOSE); // SADDLEBROWN
        LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) + 5 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
        // draw mouth #31180b
        LCD.SetFontColor (BLACKMOUTH); // ROSYBROWN
        LCD.FillRectangle ((int)(pos.x) + 2 * SPRITESCALE, -(int)(pos.y) + 6 * SPRITESCALE, 4 * SPRITESCALE, 1 * SPRITESCALE);
        // draw left pupil #3f2a75
        LCD.SetFontColor (INDIANRED);
        LCD.FillRectangle ((int)(pos.x) + 2 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
        // draw right pupil
        LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
        // draw top of hat
        LCD.SetFontColor (COPCOLOR); // BLUE
        LCD.FillRectangle ((int)(pos.x) + -2 * SPRITESCALE, -(int)(pos.y) + -1 * SPRITESCALE, 12 * SPRITESCALE, 3 * SPRITESCALE);
        // draw badge
        LCD.SetFontColor (GOLD);
        LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) + 0 * SPRITESCALE, 2 * SPRITESCALE, 2 * SPRITESCALE);
        DrawGraphic ();
    } else if (alertState) { // if it is just the alerted state
        // draw left eye-white #ffffff
        LCD.SetFontColor (WHITE);
        LCD.FillRectangle ((int)(pos.x) + 1 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
        // draw right eye-white
        LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
        // draw nose #563024
        LCD.SetFontColor (BLACKNOSE); // SADDLEBROWN
        LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) + 5 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE);
        // draw mouth #31180b
        LCD.SetFontColor (BLACKMOUTH); // ROSYBROWN
        LCD.FillRectangle ((int)(pos.x) + 2 * SPRITESCALE, -(int)(pos.y) + 6 * SPRITESCALE, 4 * SPRITESCALE, 1 * SPRITESCALE);
        // draw left pupil #3f2a75
        // LCD.SetFontColor (BLACK);
        // make the eye color depend on the amount of time the cop has been watching the player
        // float colorBoi = elapsedTime / WAITTIME;
        float colorBoi = (totalElapsedTime + elapsedTime) / WAITTIME;
        colorBoi = ((int)(colorBoi * 10)) / 12.0;
        // LCD.SetFontColor (colorBoi, colorBoi, colorBoi);
        int hexValue = ConvertColor (colorBoi, colorBoi, colorBoi);
        LCD.SetFontColor (hexValue);
        // cout << "Colorboi: " << colorBoi << endl;
        LCD.FillRectangle ((int)(pos.x) + 2 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
        // draw right pupil
        LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) + 4 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
        // draw top of hat
        LCD.SetFontColor (COPCOLOR); // BLUE
        LCD.FillRectangle ((int)(pos.x) + -2 * SPRITESCALE, -(int)(pos.y) + -1 * SPRITESCALE, 12 * SPRITESCALE, 3 * SPRITESCALE);
        // draw badge
        LCD.SetFontColor (GOLD);
        LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) + 0 * SPRITESCALE, 2 * SPRITESCALE, 2 * SPRITESCALE);
        DrawGraphic ();
    }
    
    // draw gun
    LCD.SetFontColor (DIMGRAY);
    LCD.FillRectangle ((int)(pos.x) - 3 * SPRITESCALE, -(int)(pos.y) + 5 * SPRITESCALE, 3 * SPRITESCALE, 1 * SPRITESCALE); // gun shoot point located at (pos.x - 3 * 2, pos.y - 5 * 2)
    LCD.FillRectangle ((int)(pos.x) - 1 * SPRITESCALE, -(int)(pos.y) + 6 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE);
    
    if (showFOV && wfs.heldByPlayer) {
        // draw field of view
        LCD.SetFontColor (RED);
        LCD.DrawLine (eyePoint.x, -eyePoint.y, raycast.point.x, -raycast.point.y);
    }
    
    LCD.SetFontColor (RED);
}


void Cop::DrawGraphic () {
    int SCALE = SPRITESCALE / 2;
    LCD.SetFontColor (RED);
    if (alertState == 2) {
        /*
        // draw colon
        LCD.FillRectangle ((int)(pos.x) + 2 * SPRITESCALE, -(int)(pos.y) - 7 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE); // '
        LCD.FillRectangle ((int)(pos.x) + 2 * SPRITESCALE, -(int)(pos.y) - 4 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE); // .
        // draw paranthesis
        LCD.FillRectangle ((int)(pos.x) + 4 * SPRITESCALE, -(int)(pos.y) - 7 * SPRITESCALE, 1 * SPRITESCALE, 4 * SPRITESCALE); //   |
        LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) - 8 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE); //    '
        LCD.FillRectangle ((int)(pos.x) + 5 * SPRITESCALE, -(int)(pos.y) - 3 * SPRITESCALE, 1 * SPRITESCALE, 1 * SPRITESCALE); //    .
         */
        LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) - 7 * SPRITESCALE, 2 * SPRITESCALE, 2 * SPRITESCALE); // |
        LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) - 4 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE); // .
        // LCD.FillRectangle ((int)(pos.x) + 6 * SCALE, -(int)(pos.y) - 14 * SCALE, 4 * SCALE, 5 * SCALE); // |
        // LCD.FillRectangle ((int)(pos.x) + 6 * SCALE, -(int)(pos.y) - 8 * SCALE, 4 * SCALE, 3 * SCALE); // .
        
    } if (alertState == 1  &&  timerStarted) {
        LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) - 7 * SPRITESCALE, 2 * SPRITESCALE, 2 * SPRITESCALE); // |
        LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) - 4 * SPRITESCALE, 2 * SPRITESCALE, 1 * SPRITESCALE); // .
        // LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) - 9 * SPRITESCALE, 2 * SPRITESCALE, 3 * SPRITESCALE); // |
        // LCD.FillRectangle ((int)(pos.x) + 3 * SPRITESCALE, -(int)(pos.y) - 4 * SPRITESCALE, 2 * SPRITESCALE, 2 * SPRITESCALE); // .
    }
}



// makes the cop object do cop things (and update the bullets position)
void Cop::Patrol () {
    if (isShooting == false && wfs.heldByPlayer) {
        Vector2 viewTarget;
        if (alertState != 2 || objectPerm == false) {
            viewTarget = wfs.bounds.getCenter ();
        } else {
            viewTarget = Vector2 (  playerObj.pos.x + playerObj.bounds.width/2.0,  playerObj.pos.y - playerObj.bounds.height/2.0  ); // set the target to equal the center of the player
        }
        Vector2 viewDirection = Vector2 (  (viewTarget.x - eyePoint.x),  (viewTarget.y - eyePoint.y)  );
        
        raycast = Raycast (eyePoint, viewDirection, 300, 'c');
        // float distance = Distance (playerObj.pos, pos); // get distance between player and cop
        // raycast.hit = true; raycast.colliderType = 'p';
        // float distance = raycast.distance;
        
        if (raycast.hit && raycast.colliderType == 'w' || objectPerm && alertState == 2 && raycast.hit && raycast.colliderType == 'p') { //  && distance < 100
            if (timerStarted == false) { // if the timer hasn't been started yet
                timer = TimeNow(); // start the time
                timerStarted = true;
            } else if (totalElapsedTime + elapsedTime > WAITTIME) { // if the time since the cop noticed the player is greater than the wait time
                if (TimeNow () - coolDownTimer >= COOL_DOWN_AMT) {
                    isShooting = true;
                    bulleting = true;
                    coolDownTimer = TimeNow ();
                    
                    Vector2 target = Vector2 (  playerObj.pos.x + playerObj.bounds.width/2.0,  playerObj.pos.y - playerObj.bounds.height/2.0  ); // set the target to equal the center of the player
                    float magnitude = Distance (target, shootPoint);
                    
                    bulletDir = Vector2 (  (target.x - shootPoint.x) / magnitude,  (target.y - shootPoint.y) / magnitude  ); // create a unit vector of the direction of the bullet's velocity (which is constant)
                    bulletPos = shootPoint;
                    alertState = 2;
                }
            } else {
                // display exclamation mark animation ...
                elapsedTime = TimeNow() - timer; // elapsed time is the time since the most recent time in which the cop noticed the player
                alertState = true;
            }
        } else { // reset the time
            // timerStarted = false; // or not ... maybe pause the timer instead through some means
            // timer = (TimeNow() - timer) +
            if (timerStarted) {
                timerStarted = false;
                // elapsedTime = elapsedTime + (TimeNow() - timer); // make the elapsed time equal to the previous elapsed time plus the elapsed time since the timer was last started (the time while the cop was last watching the player)
                totalElapsedTime = totalElapsedTime + elapsedTime;
                elapsedTime = 0;
            }
            if (timerStarted && TimeNow() - timer > WAITTIME) { // if the cop noticed the player but the player ran off if time
                alertState = 2; // set the alert state to shoot on sight
            }
        }
    } else {
        // update the bullets position
        if (bulleting) {
            bulletPos = Vector2 (bulletPos.x + bulletDir.x * velScale, bulletPos.y + bulletDir.y * velScale);
            if (bulletPos.x < 0 || bulletPos.x > 319 || bulletPos.y > 0 || bulletPos.y < -239) { // determine if the bullet is out of bounds
                isShooting = false; // if it is, stop the shooting animation
                bulleting = false;
            }
            
            // check if the bullet has hit a collision object
            // first check if the bullet hit the wet floor sign
            if (playerIsDead == false) { // if the player is not dead (so the bullet passes above/through the wet floor sign when the player dies)
                if (CheckBoxCollision (wfs.bounds)) {
                    ricoCount++; // increment the amount of times the bullet has ricocheted
                }
            }
            
            if (ricoCount <= MAXRICO) { // if the bullet did not hit the wet floor sign
                for (int j = 0; j < numTunnelEdges; j++) { // check for other collisions (those with edges)
                    if (CheckEdgeCollision (tunnelEdges [j])) {
                        ricoCount++; // increment the amount of times the bullet has ricocheted
                        // if (ricoCount > MAXRICO) { // may wanna change this for multi collision / corner hits ...
                            break; // exit the loop ... this may not exit the loop entirely
                        // }
                    }
                }
            } else {
                didBreak = true;
            }
            
            if (didBreak) { // if the bullet has collided with something
                didBreak = false; // reset variables
                ricoCount = 0;
                explodePos = bulletPos; // makes a lil collision animation be played
                isShooting = false; // reset variables ... may want to change how the cop knows to shoot again / the frequency in which the cop shoots ... or else this'll be a bit weird and bad
                bulleting = false;
            }
        }
    }
}


float ApplyFrictionalDeceleration (float velocity, float deceleration) {
    if (velocity > 0) {
        velocity = velocity - deceleration;
        if (velocity < 0) {
            velocity = 0;
        }
    } else if (velocity < 0) {
        velocity = velocity + deceleration;
        if (velocity > 0) {
            velocity = 0;
        }
    }
    return velocity;
}



// checks if the bullet has collided with a box collision object
bool Cop::CheckBoxCollision (Box obj) {
    Vector2 objVel = bulletDir; // store the objects velocity before any collision occur
    // convert the box into collision edges
    Edge southEdge = Edge (obj.verts[2], obj.verts[3], 0);
    Edge westEdge = Edge (obj.verts[0], obj.verts[2], 1);
    Edge northEdge = Edge (obj.verts[0], obj.verts[1], 2);
    Edge eastEdge = Edge (obj.verts[1], obj.verts[3], 3);
    
    // perform the bullet edge collision function on each edge of the box
    bool sCol = CheckEdgeCollision (southEdge);
    bool wCol = CheckEdgeCollision (westEdge);
    bool nCol = CheckEdgeCollision (northEdge);
    bool eCol = CheckEdgeCollision (eastEdge);
    
    if (sCol || wCol || nCol || eCol) {
        // ope this part is exclusively for collision with the wet-floor sign ...
        accelBull = playerObj.CalcAccel (bulletPos, Vector2 (objVel.x * velScale, objVel.y * velScale));
        if (playerIsDead == false) {
            // if the bullet hits the side of the wet-floor sign change the players direction
            int tempDir = playerObj.dir; // temporary copy of player direction in order to prevent weird switch behavior
            switch (tempDir) {
                case (0): // down
                    if (!nCol && !sCol) {
                        if (eCol) { // hit from the east side
                            playerObj.dir = 1; // face leftward
                        } else if (wCol) { // hit from the west side
                            playerObj.dir = 3; // face rightward
                        }
                    }
                    break;
                case (1): // left
                    if (!eCol && !wCol) {
                        if (nCol) { // hit from the north side
                            playerObj.dir = 0; // face downward
                        } else if (sCol) { // hit from the south side
                            playerObj.dir = 2; // face upward
                        }
                    }
                    break;
                case (2): // up
                    if (!nCol && !sCol) {
                        if (eCol) { // hit from the east side
                            playerObj.dir = 1; // face leftward
                        } else if (wCol) { // hit from the west side
                            playerObj.dir = 3; // face rightward
                        }
                    }
                    break;
                case (3): // right
                    if (!eCol && !wCol) {
                        if (nCol) { // hit from the north side
                            playerObj.dir = 0; // face downward
                        } else if (sCol) { // hit from the south side
                            playerObj.dir = 2; // face upward
                        }
                    }
                    break;
            }
        }
        
        return true;
        
    } else {
        
        return false;
        
    }
    
    /* if (CheckEdgeCollision (southEdge) || CheckEdgeCollision (westEdge) || CheckEdgeCollision (northEdge) || CheckEdgeCollision (eastEdge)) {
        return true;
    } else {
        return false;
    } */
}



// checks if the bullet has collided with an edge collision object
bool Cop::CheckEdgeCollision (Edge obj) {
    Vector2 nextPos = bulletPos; // set nextPos to (secretely) equal the bullet's current position
    Vector2 vel = bulletDir;
    bool collided = false;
    
    float RESTI = 1; // .5 initialize coefficient of restitution
    float EDGE_TOL = 5; // initialize edge tolerance, which is essentially a pseudo width for collision edges
    // Box bounds = Box (bulletPos, 0, 0);
    
    switch (obj.dir) {
        case (1):   // case where edge normal vector is pointing leftward   (object is traveling rightwards)
            // if right side of physics object is colliding with the edge, then adjust the physics object's velocity accordingly
            if (vel.x > 0   &&   nextPos.y < obj.verts[0].y && nextPos.y > obj.verts[1].y) { // determine if the object is traveling rightward and if either (Y) points denoting the bounds of the physics object are within the (Y) bounds of the collision edge
                
                // (determine if the right point denoting the (X) bounds of the physics object are within the (X) bounds of the collision edge)
                if (nextPos.x > obj.verts[0].x && nextPos.x < obj.verts[1].x+EDGE_TOL) {
                    //float decelCol = abs (vel.x) * STATF; // calculate deceleration due to collision
                    //vel.y = ApplyFrictionalDeceleration (vel.y, decelCol); // apply the deceleration due to collision to the new y-velocity
                    vel.x = -(vel.x) * RESTI; // calculate x-velocity after the collision
                    collided = true;
                }
                
            }
            break;
        case (3):   // case where edge normal vector is pointing rightward   (object is traveling leftwards)
            // if left side of physics object is colliding with the edge, then adjust the physics object's velocity accordingly
            if (vel.x < 0   &&   nextPos.y < obj.verts[0].y && nextPos.y > obj.verts[1].y) { // determine if the object is traveling leftward and if either (Y) points denoting the bounds of the physics object are within the (Y) bounds of the collision edge
                
                // (determine if the left point denoting the (X) bounds of the physics object are within the (X) bounds of the collision edge)
                if (nextPos.x < obj.verts[1].x && nextPos.x > obj.verts[0].x-EDGE_TOL) {
                    //float decelCol = abs (vel.x) * STATF; // calculate deceleration due to collision
                    //vel.y = ApplyFrictionalDeceleration (vel.y, decelCol); // apply the deceleration due to collision to the new y-velocity
                    vel.x = -(vel.x) * RESTI; // calculate x-velocity after the collision
                    collided = true;
                }
                
            }
            break;
        case (2):   // case where edge normal vector is pointing upward   (object is traveling downwards)
            // if colliding with top side of edge, then adjust the physics object's velocity accordingly
            if (vel.y < 0   &&   nextPos.x > obj.verts[0].x && nextPos.x < obj.verts[1].x) {
                
                if (nextPos.y < obj.verts[0].y && nextPos.y > obj.verts[1].y-EDGE_TOL) {
                    //float decelCol = abs (vel.y) * STATF; // calculate deceleration due to collision
                    //vel.x = ApplyFrictionalDeceleration (vel.x, decelCol); // apply the deceleration due to collision to the new y-velocity
                    vel.y = -(vel.y) * RESTI; // calculate x-velocity after the collision
                    collided = true;
                }
                
            }
            break;
        case (0):   // case where edge normal vector is pointing downward   (object is traveling upwards)
            // if colliding with bottom side of edge, then adjust the physics object's velocity accordingly
            if (vel.y > 0   &&   nextPos.x > obj.verts[0].x && nextPos.x < obj.verts[1].x) {
                
                if (nextPos.y > obj.verts[1].y && nextPos.y < obj.verts[0].y+EDGE_TOL) {
                    //float decelCol = abs (vel.y) * STATF; // calculate deceleration due to collision
                    //vel.x = ApplyFrictionalDeceleration (vel.x, decelCol); // apply the deceleration due to collision to the new y-velocity
                    vel.y = -(vel.y) * RESTI; // calculate x-velocity after the collision
                    collided = true;
                }
                
            }
            break;
    }
    
    bulletDir = vel;
    
    return collided;
}



// draws bullet
void Cop::DrawBullet () {
    if (isShooting) { // if shooting then
        // draw trail
        /*
        LCD.SetFontColor (RED);
        float trailLength = 25;
        Vector2 bulletCenter = Vector2 ((bulletPos.x-2), (bulletPos.y+2)); // is causing weird problems
        LCD.DrawLine (bulletCenter.x, -bulletCenter.y, bulletCenter.x - bulletDir.x * trailLength, -(bulletCenter.y - bulletDir.y * trailLength)); // may be too computationally intensive for the proteus to be worth it
        */
        // draw bullet
        LCD.SetFontColor (GRAY); // DIMEGRAY
        // LCD.DrawPixel ((int)(bulletPos.x), -(int)(bulletPos.y));
        // LCD.FillRectangle ((int)(bulletCenter.x), -(int)(bulletCenter.y), 4, 4);
        //LCD.FillRectangle ((int)(bulletPos.x-2), -(int)(bulletPos.y+2), 4, 4);
        LCD.FillCircle ((int)(bulletPos.x), -(int)(bulletPos.y), 2);
    }
    DrawExplosion ();
}

// draws an explosion at the point in which the bullet was destroyed
void Cop::DrawExplosion () { // && MAX_EXPLODE_FRAMES != 0
    if (highDetail  &&  explodePos.x > -68) { // if the explosion should be played
        explodeIterations++;
        LCD.SetFontColor (YELLOW);
        LCD.FillCircle ((int)(explodePos.x), -(int)(explodePos.y), 2 + (10.0/MAX_EXPLODE_FRAMES) * explodeIterations);
        if (explodeIterations >= MAX_EXPLODE_FRAMES) {
            LCD.SetFontColor (BACKGROUNDCOLOR);
            LCD.FillCircle ((int)(explodePos.x), -(int)(explodePos.y), 2 + (10.0/MAX_EXPLODE_FRAMES) * explodeIterations);
            explodePos.x = -420;
            explodeIterations = 0;
        }
    }
    // erase the explosion if necessary
    /* if (explodePos.x < -419) {
        LCD.SetFontColor (WALLCOLOR);
        LCD.FillCircle ((int)(explodePos.x), -(int)(explodePos.y), 5)
        explodePos.x = -69; // I'm a dumb ass this wouldn't work (bc your modifying where the explosion would occur)
    } */
}



// calculates distance between two points
float Distance (Vector2 start, Vector2 end) {
    return sqrt (  pow (end.x - start.x, 2.0)  +  pow (end.y - start.y, 2.0)  );
}



// reference for global variables for in-game GUI
/*
 #define MB_WIDTH 30 // move buttons width
 #define MB_HEIGHT 30 // move buttons height
 #define MB_MARGIN 5 // move buttons margin
 Vector2 moveButtPos = Vector2 (MB_MARGIN, -240 + (MB_HEIGHT + MB_MARGIN) * 2);
 Button leftButt = Button (  Box ( Vector2 (moveButtPos.x, moveButtPos.y - (MB_HEIGHT + MB_MARGIN)) ,  MB_WIDTH, MB_HEIGHT ), "<", 5  ); // may need to be declared above gameloop function
 Button upButt = Button (  Box ( Vector2 (moveButtPos.x + MB_WIDTH + MB_MARGIN, moveButtPos.y) ,  MB_WIDTH, MB_HEIGHT ), "^", 5  );
 Button rightButt = Button (  Box ( Vector2 (moveButtPos.x + (MB_WIDTH + MB_MARGIN) * 2, moveButtPos.y - (MB_HEIGHT + MB_MARGIN)) ,  MB_WIDTH, MB_HEIGHT ), ">", 5  );
 Button downButt = Button (  Box ( Vector2 (moveButtPos.x + MB_WIDTH + MB_MARGIN, moveButtPos.y - (MB_HEIGHT + MB_MARGIN)) ,  MB_WIDTH, MB_HEIGHT ), "v", 5  );
 Button inGameBackButt = Button (  Box ( Vector2 (160-15, -MB_MARGIN), 30, 30 ), "BACK", 5  );
 */



// draws the in-game GUI
void DrawInGameGUI () {
    leftButt.DrawButton ();
    upButt.DrawButton ();
    rightButt.DrawButton ();
    downButt.DrawButton ();
    
    inGameBackButt.DrawButton ();
    
    // (move this bit elsewher to make it more efficient)
    char charPtr [100];
    char levelPtr [100];
    sprintf (levelPtr, "%d", level);
    // strcpy (charPtr, "SCORE: ");
    strcpy (charPtr, "");
    strcat (charPtr, levelPtr);
    
    scoreButt.UpdateText (charPtr);
    scoreButt.DrawButton ();
}



// updates stats
void UpdateStats () {
    if (level > highScore) {
        highScore = level;
    }
}



// resets game
void ResetGame () {
    playerObj.pos = Vector2 ();
    playerObj.vel = Vector2 ();
    // reset a bunch of other things ...
    level = 1;
    iterationsSinceDeath = 0;
    playerIsDead = false;
    wfs.heldByPlayer = false; // x
}



// reference for global variables for level generation

/*
// #define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320
// #define TUNNEL_RES 25
#define TILE_SIZE 10 // the width or height of a square tile in pixels              // 20
#define TUNNEL_TILE_SIZE 4 // amount of tiles for the diameter of the tunnel        // 2
#define GRID_HEIGHT 24 // should equal SCREEN_WIDTH / TILE_SIZE                     // 12
#define GRID_WIDTH 32 // should equal SCREEN_WIDTH / TILE_SIZE                      // 16
int MAX_TURNS = 10; // maximum amount of turns
int MAX_TUNNELS = 15;
int MAX_LENGTH = 10; // maximum length (in tiles) of a single tunnel
#define MAX_TUNNEL_EDGES 4096
Edge tunnelEdges [MAX_TUNNEL_EDGES];
int numTunnelEdges = 0; // actual number or length of tunnel edges
*/

#define ENDING_TILE_BUFFER_SIZE 2 // the size (in tiles) of the buffer between the entrance/exit and where the tunnel may turn to
int MIN_LENGTH = 6; // should at least equal TUNNEL_TILE_SIZE; should be a factor of TURN_INTERVAL
int TURN_INTERVAL = 2; // the interval of tiles in which a turn may occur; only affects the length of the tunnel; should not exceed TUNNEL_TILE_SIZE
int map[GRID_HEIGHT][GRID_WIDTH];
// bool RIGHT_ONLY = true;
// #define RIGHT_ONLY true

// returns whether there is wall at the given position or if it exceeds the bounds ... 2 if it exceeds the bounds
int IsWall (int map [][GRID_WIDTH], IntVector2 pos) {
    int result = false;
    int x = pos.x;
    int y = -pos.y;
    
    // check to see if it exceeds the bounds
    if (x < 0 || y < 0 || x >= GRID_WIDTH || y >= GRID_HEIGHT) {
        result = 2;
    } else if (map [y][x] == 0) {
        result = true;
    }
    
    return result;
}
IntVector2 GetHop (IntVector2 pos, IntVector2 dir) {
    return IntVector2 (pos.x + dir.x, pos.y + dir.y);
}
IntVector2 GetLeft (IntVector2 pos, IntVector2 dir) {
    dir.RotateLeft ();
    return IntVector2 (pos.x + dir.x, pos.y + dir.y);
}
IntVector2 GetRight (IntVector2 pos, IntVector2 dir) {
    dir.RotateRight ();
    return IntVector2 (pos.x + dir.x, pos.y + dir.y);
}

int IsGood (int map [][GRID_WIDTH], IntVector2 pos) {
    int result = true;
    int x = pos.x;
    int y = -pos.y;
    
    IntVector2 minBounds = IntVector2 (ENDING_TILE_BUFFER_SIZE, 1); // 1, 1
    IntVector2 maxBounds = IntVector2 (GRID_WIDTH - ENDING_TILE_BUFFER_SIZE + 1, GRID_HEIGHT - 1); // GRID_WIDTH, GRID_HEIGHT-1
    
    // check to see if it doesn't exceed the bounds
    if (x < minBounds.x || y < minBounds.y || x >= maxBounds.x || y >= maxBounds.y) {
        result = 0;
    }
    
    return result;
}
int IsGood (int map [][GRID_WIDTH], int x, int y) {
    int result = true;
    //int x = pos.x;
    //int y = -pos.y;
    y = -y;
    
    IntVector2 minBounds = IntVector2 (ENDING_TILE_BUFFER_SIZE, 1); // 1, 1
    IntVector2 maxBounds = IntVector2 (GRID_WIDTH - ENDING_TILE_BUFFER_SIZE + 1, GRID_HEIGHT - 1); // GRID_WIDTH, GRID_HEIGHT-1
    
    // check to see if it doesn't exceed the bounds
    if (x < minBounds.x || y < minBounds.y || x >= maxBounds.x || y >= maxBounds.y) {
        result = 0;
    }
    
    return result;
}



void GenerateLevel () {
    int tunnelAmt = 0;
    int dir = 0;
    IntVector2 dir2;
    IntVector2 norm2;
    IntVector2 prevDir2;
    IntVector2 prevNorm2;
    int currLength = 0;
    numTunnelEdges = 0;
    numTunnelBoxes = 0;
    
    // make 2D array of tiles representing the map——where 0 is a wall and 1 is a tunnel
    // int map[GRID_HEIGHT][GRID_WIDTH] = {0};
    // int tempMap[GRID_HEIGHT][GRID_WIDTH] = {0};
    // map [][GRID_WIDTH] = tempMap;
    // map = tempMap;
    for (int j = 0; j < GRID_HEIGHT; j++) {
        for (int k = 0; k < GRID_WIDTH; k++) {
            map [j][k] = 0;
        }
    }
    
    // generate random starting point
    // seed the random number generator
    // srand (TimeForRand ()); // change this in future since non-FEH
    IntVector2 currPoint = IntVector2  (   ENDING_TILE_BUFFER_SIZE,  -(rand () % (GRID_HEIGHT - TUNNEL_TILE_SIZE * 2) + TUNNEL_TILE_SIZE)   ); // -TUNNEL_TILE_SIZE
    
    // create tunnel entrance
    IntVector2 tempCurrPoint = IntVector2 (0, currPoint.y);
    for (int j = 0;   j < ENDING_TILE_BUFFER_SIZE;   j++) {
        IntVector2 tempNorm = IntVector2 (0, 0);
        for (int p = 0; p < TUNNEL_TILE_SIZE; p++) {
            // if (IsGood (map, tempCurrPoint.x + tempNorm.x, tempCurrPoint.y + tempNorm.y)) {
            map [-(tempCurrPoint.y + tempNorm.y)][tempCurrPoint.x + tempNorm.x] = 1;
            tempNorm = IntVector2 (tempNorm.x + 0, tempNorm.y + -1);
            // }
        }
        tempCurrPoint = IntVector2 (tempCurrPoint.x + 1, tempCurrPoint.y + 0);
    }
    
    while (tunnelAmt < MAX_TUNNELS) {
        // generate random length of tunnel
        // currLength = rand () % ( (MAX_LENGTH - MIN_LENGTH) / TURN_INTERVAL ) * TURN_INTERVAL + MIN_LENGTH;
        currLength = (rand () % ((MAX_LENGTH - MIN_LENGTH)) / TURN_INTERVAL) * TURN_INTERVAL + MIN_LENGTH;
        // currLength = rand () % ((MAX_LENGTH - TUNNEL_TILE_SIZE) / 3) * 3 + TUNNEL_TILE_SIZE;
        prevDir2 = dir2;
        prevNorm2 = norm2;
        
        // determine if it should end if its right only
        if (RIGHT_ONLY) {
            if (currPoint.x >= GRID_WIDTH - ENDING_TILE_BUFFER_SIZE + 1 - 1) {
                tunnelAmt = MAX_TUNNELS + 1;
            }
        }
        
        // generate random direction
        if (tunnelAmt == 0) { // unless its the first or last iteration
            dir = 3;
            
            // currLength += TUNNEL_TILE_SIZE;
            // cout << "L: " << currLength << endl;
            // correction = 1;
        } else if (tunnelAmt >= MAX_TUNNELS - 1) {
            dir = 3;
            currLength = GRID_WIDTH;
            // doingEndpoint = true;
            //- correction = TUNNEL_TILE_SIZE + 1;
        } else {
            int prevDir = dir;
            // loop until the direction is perpendicular to the previous direction
            do {
                dir = rand () % 4;
            } while (prevDir == dir || prevDir == dir+2 || prevDir == dir-2); // prevDir == dir+2 && dir <= 1 || prevDir == dir-2 && dir >= 2)
            if (RIGHT_ONLY) {
                if (dir == 1) {
                    dir = 3;
                }
            }
            // correction = 0;
        }
        
        dir2 = DirToIntVector2 (dir);
        norm2 = IntVector2 (dir2.x, dir2.y);
        norm2.RotateRight ();
        
        map [-currPoint.y][currPoint.x] = 1;
        
        // get sharp corners
        IntVector2 testDir = IntVector2 (prevDir2.x, prevDir2.y);
        testDir.RotateLeft ();
        // determine if it turned left or right in order to properly fill in sharp corners
        if (dir2.Equals (testDir)) { // if it turned left
            // (note that if its turning leftward tiles would be drwan ahead of its current position
            // if its position + the tile size (in the direction it was going) exceeds the bounds then push it back some
            if (!IsGood (map, currPoint.x + prevDir2.x*(TUNNEL_TILE_SIZE), currPoint.y + prevDir2.y*(TUNNEL_TILE_SIZE))) {
                // ****** (should actually be pushed back by (TUNNEL_TILE_SIZE-1 - the distance from closest/corresponding boundary)) ******
                cout << "Pushed back." << endl;
                currPoint = IntVector2 (currPoint.x - prevDir2.x*(TUNNEL_TILE_SIZE-1), currPoint.y - prevDir2.y*(TUNNEL_TILE_SIZE-1));
                /*
                for (int d = 0; !IsGood (map, currPoint.x + prevDir2.x*(TUNNEL_TILE_SIZE), currPoint.y + prevDir2.y*(TUNNEL_TILE_SIZE)); d++) {
                    currPoint = IntVector2 (currPoint.x - prevDir2.x, currPoint.y - prevDir2.y);
                }
                */
            }
            IntVector2 tempCurrPoint = IntVector2 (currPoint.x, currPoint.y);
            for (int j = 0;   j < TUNNEL_TILE_SIZE;   j++) { // maybe <
                IntVector2 tempNorm = IntVector2 (0, 0);
                for (int p = 0; p < TUNNEL_TILE_SIZE; p++) {
                    if (IsGood (map, tempCurrPoint.x + tempNorm.x, tempCurrPoint.y + tempNorm.y)) {
                        map [-(tempCurrPoint.y + tempNorm.y)][tempCurrPoint.x + tempNorm.x] = 1;
                        tempNorm = IntVector2 (tempNorm.x + prevNorm2.x, tempNorm.y + prevNorm2.y);
                    }
                }
                tempCurrPoint = IntVector2 (tempCurrPoint.x + prevDir2.x, tempCurrPoint.y + prevDir2.y);
            }
        }
        
        int j = 0;
        // for (j = 0;   j < currLength  &&  IsGood (map, currPoint.x + dir2.x, currPoint.y + dir2.y)  &&  IsGood (map, currPoint.x + norm2.x*(TUNNEL_TILE_SIZE-1*0), currPoint.y + norm2.y*(TUNNEL_TILE_SIZE-1*0));   j++) {
        for (j = 0;   j < currLength  &&  IsGood (map, currPoint.x + dir2.x, currPoint.y + dir2.y);   j++) {
            cout << currPoint.x << ", " << currPoint.y << " => " << map [-currPoint.y][currPoint.x] <<    "  (dir)  => "    << dir2.x << ", " << dir2.y << endl;
            IntVector2 tempNorm = IntVector2 (0, 0);
            
            for (int p = 0; p < TUNNEL_TILE_SIZE; p++) { // <=
                map [-(currPoint.y + tempNorm.y)][currPoint.x + tempNorm.x] = 1;
                tempNorm = IntVector2 (tempNorm.x + norm2.x, tempNorm.y + norm2.y);
            }
            
            currPoint = IntVector2 (currPoint.x + dir2.x, currPoint.y + dir2.y);
        }
        
        // if the new tunnel went immediately out of bounds, make it try again  (currently causing an infinite loop of sone variety
        
        //if (j == 0  &&  correction == 0) { // j == 0
        //    cout << "No progress.";
        //    tunnelAmt--;
        //}
        
        tunnelAmt++;
    }
    
    // create tunnel exit
    tempCurrPoint = IntVector2 (GRID_WIDTH-ENDING_TILE_BUFFER_SIZE, currPoint.y);
    for (int j = 0;   j < ENDING_TILE_BUFFER_SIZE;   j++) {
        IntVector2 tempNorm = IntVector2 (0, 0);
        for (int p = 0; p < TUNNEL_TILE_SIZE; p++) {
            // if (IsGood (map, tempCurrPoint.x + tempNorm.x, tempCurrPoint.y + tempNorm.y)) {
            map [-(tempCurrPoint.y + tempNorm.y)][tempCurrPoint.x + tempNorm.x] = 1;
            tempNorm = IntVector2 (tempNorm.x + 0, tempNorm.y + -1);
            // }
        }
        tempCurrPoint = IntVector2 (tempCurrPoint.x + 1, tempCurrPoint.y + 0);
    }
    
    // print the map array
    cout << endl << endl;
    cout << "     ";
    for (int k = 0; k < GRID_WIDTH; k++) {
        cout << k % 10 << " ";
    }
    cout << endl << endl;
    for (int j = 0; j < GRID_HEIGHT; j++) {
        cout << j << ":  ";
        if (j < 10) {
            cout << " ";
        }
        for (int k = 0; k < GRID_WIDTH; k++) {
            if (map [j][k] == 0) {
                cout << "." << " ";
            } else {
                cout << map [j][k] << " ";
            }
        }
        cout << endl;
    }
    
    
    /************************ IDENTIFY ISLANDS AND PENINSULAS ************************/
    
    int targetTile = 1;
    bool justSwitched = false; // whether the target tile just switched (used for the purpose of detecting 1x1 tiles)
    
    char landform [GRID_HEIGHT][GRID_WIDTH];
    for (int j = 0; j < GRID_HEIGHT; j++) {
        for (int k = 0; k < GRID_WIDTH; k++) {
            landform [j][k] = '0';
        }
    }
    
    int columnSwitches [GRID_WIDTH] = {0}; // the amount of switches from wall to tunnel / vise-versa per column
    int rowSwitches [GRID_HEIGHT] = {0}; // the amount of switches from wall to tunnel / vise-versa per row
    
    // iterate downward until a target tile is detected  (in order to identify peninsulas)
    for (int i = 0; i < GRID_WIDTH; i++) { // i < GRID_WIDTH
        int switches = 0;
        for (int j = 0; j < GRID_HEIGHT; j++) {
            // if the target tile is reached
            if (map [j][i] == targetTile) {
                // cout << i << ", " << j << " => " << map [j][i] << " " << endl;
                // if the tunnel is reached and hasn't been visited yet
                if (targetTile == 1  &&  (justSwitched || true)) {
                    switches++;
                    j--; // step back so that 1x1 tiles can be detected
                }
                // if a wall is reached and the above tunnel hasn't been visited yet
                else if (targetTile == 0  &&  (justSwitched || true)) { // also step back so that the jeroo will be along the inside of the tunnel (and so that 1x1 tiles can be detected laterr)
                    // follow the border of that tunnel (going counter-clockwise along the inside of the tunnel) until the path/border ends
                    
                    // switch target tiles
                    // targetTile = 1;
                    switches++;
                    j--;
                }
                
                // switch target tiles
                if (targetTile == 1) {
                    targetTile = 0;
                } else if (targetTile == 0) {
                    targetTile = 1;
                }
                justSwitched = true; // record that the target tile just switched
            } else {
                justSwitched = false; // record that the target tile hadn't been switched
            }
            // if its a tunnel
            if (map [j][i] == 1) {
                if (switches > 2) {
                    landform [j][i] = 'p';
                }
            }
        }
        columnSwitches [i] = switches;
        justSwitched = false; // reset justSwitched variable
    }
    
    // mark peninsulas based on the number of column switches
    for (int i = 0; i < GRID_WIDTH; i++) { // i < GRID_WIDTH
        for (int j = 0; j < GRID_HEIGHT; j++) {
            // if (columnSwitches [i] )
        }
    }
    
    targetTile = 0;
    justSwitched = false;
    
    // iterate rightward until a target tile is detected  (in order to identify islands)
    for (int j = 0; j < GRID_HEIGHT; j++) { // i < GRID_WIDTH
        int switches = 0;
        for (int i = 0; i < GRID_WIDTH; i++) {
            // if the target tile is reached
            if (map [j][i] == targetTile) {
                // cout << i << ", " << j << " => " << map [j][i] << " " << endl;
                // if the tunnel is reached and hasn't been visited yet
                if (targetTile == 1  &&  (justSwitched || true)) {
                    switches++;
                    i--; // step back so that 1x1 tiles can be detected
                }
                // if a wall is reached and the above tunnel hasn't been visited yet
                else if (targetTile == 0  &&  (justSwitched || true)) { // also step back so that the jeroo will be along the inside of the tunnel (and so that 1x1 tiles can be detected laterr)
                    // follow the border of that tunnel (going counter-clockwise along the inside of the tunnel) until the path/border ends
                    
                    // switch target tiles
                    // targetTile = 1;
                    switches++;
                    i--;
                }
                
                // switch target tiles
                if (targetTile == 1) {
                    targetTile = 0;
                } else if (targetTile == 0) {
                    targetTile = 1;
                }
                justSwitched = true; // record that the target tile just switched
            } else {
                justSwitched = false; // record that the target tile hadn't been switched
            }
            // if its a tunnel
            if (map [j][i] == 1) {
                if (switches >= 2  &&  landform [j][i] == 'p') {
                    // landform [j][i] = 'i';
                }
            }
        }
        // columnSwitches [i] = switches;
        justSwitched = false; // reset justSwitched variable
    }
    
    
    
    
    int visitedTiles[GRID_HEIGHT][GRID_WIDTH] = {0};
    targetTile = 1;
    justSwitched = false; // whether the target tile just switched (used for the purpose of detecting 1x1 tiles)
    
    
    
    /************************ CONVERT THE MAP ARRAY INTO PHYSICS EDGES ************************/
    
    
    // iterate downward until a target tile is detected (and the current tunnel hasn't been detected/visited yet)
    for (int i = 0; i == 0; i++) { // i < GRID_WIDTH
        for (int j = 0; j < GRID_HEIGHT; j++) {
            // if the target tile is reached
            if (map [j][i] == targetTile) {
                // cout << i << ", " << j << " => " << map [j][i] << " " << endl;
                // if the tunnel is reached and hasn't been visited yet
                if (targetTile == 1  &&  (justSwitched || visitedTiles [j][i] == 0)) {
                    IntVector2 jong = IntVector2 (i, -j); // store jeroo's starting position
                    IntVector2 dir = IntVector2 (1, 0); // starting direction is right
                    IntVector2 norm = IntVector2 (0, -1); // starting normal is down
                    bool lineEnded = false; // whether the line ended or not
                    Vector2 v0 = Vector2 (jong.x * SCREEN_WIDTH/((float)(GRID_WIDTH)), jong.y * SCREEN_HEIGHT/((float)(GRID_HEIGHT)));
                    Vector2 v1; // the points should equal the center of jong + the direction - the normal vector
                    
                    // follow the border of that tunnel (going clockwise along the inside of the tunnel) until the path/border ends
                    do { // do while the path ends once the edge of the map is reached (or the detector comes back to its starting point / returns to the same orientation)
                        visitedTiles [-jong.y][jong.x] = 1;
                        // if i is not 0 then mark it as an island
                        if (i > 0) {
                            landform [-jong.y][jong.x] = 'i';
                        }
                        // cout << jong.x << ", " << jong.y << " => " << map [-jong.y][jong.x] <<    "  (dir)  => "    << dir.x << ", " << dir.y << endl;
                        IntVector2 forward = GetHop (jong, dir);
                        IntVector2 leftward = GetLeft (jong, dir);
                        // if there is not a wall to the left and there is a wall ahead
                        if (IsWall (map, leftward) == false  &&  IsWall (map, forward) >= true) {
                            Vector2 tileV1 = Vector2 (jong.x + 0.5 + (-norm.x - dir.x) * 0.5, jong.y - 0.5 + (-norm.y - dir.y) * 0.5);
                            v1 = Vector2 (tileV1.x * SCREEN_WIDTH/((float)(GRID_WIDTH)), tileV1.y * SCREEN_HEIGHT/((float)(GRID_HEIGHT)));
                            tunnelEdges [numTunnelEdges] = Edge (v0, v1, norm);
                            if (i > 0) {
                                tunnelEdges [numTunnelEdges].edgeType = 'i';
                            } else if (landform [-jong.y][jong.x] == 'p') {
                                tunnelEdges [numTunnelEdges].edgeType = 'p';
                            }
                            numTunnelEdges++;
                            v0 = Vector2 (tileV1.x * SCREEN_WIDTH/((float)(GRID_WIDTH)), tileV1.y * SCREEN_HEIGHT/((float)(GRID_HEIGHT)));
                            // effectively remove the edges that block the hallway exits
                            if (tunnelEdges [numTunnelEdges-1].dir == 1) {
                                if (jong.x >= GRID_WIDTH-1) {
                                    numTunnelEdges--;
                                    exitIndex = numTunnelEdges;
                                }
                            }
                            
                            // then rotate left
                            dir.RotateLeft();
                            norm.RotateLeft();
                            // move forward one
                            jong = GetHop (jong, dir);
                            // cout << "both" << endl;
                        }
                        // else if there is not a wall to the left then rotate left
                        else if (IsWall (map, leftward) == false) {
                            Vector2 tileV1 = Vector2 (jong.x + 0.5 + (-norm.x - dir.x) * 0.5, jong.y - 0.5 + (-norm.y - dir.y) * 0.5);
                            v1 = Vector2 (tileV1.x * SCREEN_WIDTH/((float)(GRID_WIDTH)), tileV1.y * SCREEN_HEIGHT/((float)(GRID_HEIGHT)));
                            tunnelEdges [numTunnelEdges] = Edge (v0, v1, norm);
                            if (i > 0) {
                                tunnelEdges [numTunnelEdges].edgeType = 'i';
                            } else if (landform [-jong.y][jong.x] == 'p') {
                                tunnelEdges [numTunnelEdges].edgeType = 'p';
                            }
                            numTunnelEdges++;
                            v0 = Vector2 (tileV1.x * SCREEN_WIDTH/((float)(GRID_WIDTH)), tileV1.y * SCREEN_HEIGHT/((float)(GRID_HEIGHT))); // just in case deep copy weird
                            // effectively remove the edges that block the hallway exits
                            if (tunnelEdges [numTunnelEdges-1].dir == 1) {
                                if (jong.x >= GRID_WIDTH-1) {
                                    numTunnelEdges--;
                                    exitIndex = numTunnelEdges;
                                }
                            }
                            
                            dir.RotateLeft();
                            norm.RotateLeft();
                            // move forward one
                            jong = GetHop (jong, dir);
                            // cout << "left fw" << endl;
                        }
                        // else if there is a wall ahead then rotate right
                        else if (IsWall (map, forward) >= true) {
                            Vector2 tileV1 = Vector2 (jong.x + 0.5 + (-norm.x + dir.x) * 0.5, jong.y - 0.5 + (-norm.y + dir.y) * 0.5);
                            v1 = Vector2 (tileV1.x * SCREEN_WIDTH/((float)(GRID_WIDTH)), tileV1.y * SCREEN_HEIGHT/((float)(GRID_HEIGHT)));
                            tunnelEdges [numTunnelEdges] = Edge (v0, v1, norm);
                            if (i > 0) {
                                tunnelEdges [numTunnelEdges].edgeType = 'i';
                            } else if (landform [-jong.y][jong.x] == 'p') {
                                tunnelEdges [numTunnelEdges].edgeType = 'p';
                            }
                            numTunnelEdges++;
                            v0 = Vector2 (tileV1.x * SCREEN_WIDTH/((float)(GRID_WIDTH)), tileV1.y * SCREEN_HEIGHT/((float)(GRID_HEIGHT)));
                            // effectively remove the edges that block the hallway exits
                            if (tunnelEdges [numTunnelEdges-1].dir == 1) {
                                if (jong.x >= GRID_WIDTH-1) {
                                    numTunnelEdges--;
                                    exitIndex = numTunnelEdges;
                                }
                            }
                            
                            dir.RotateRight ();
                            norm.RotateRight();
                            // cout << "right" << endl;
                        }
                        // else // (there is a wall to the left and tunnel forward)
                        else {
                            // move forward
                            jong = forward;
                        }
                    } while (!(jong.Equals (IntVector2 (i, -j))  &&  dir.Equals (IntVector2 (1, 0)))); // while the jeroo's orientation/position doesn't equal its starting position
                    // may need edge creation code chunk here
                    
                    // switch target tiles
                    // targetTile = 0;
                    j--; // step back so that 1x1 tiles can be detected
                }
                // if a wall is reached and the above tunnel hasn't been visited yet
                else if (targetTile == 0  &&  (justSwitched || visitedTiles [--j][i] == 0)) { // also step back so that the jeroo will be along the inside of the tunnel (and so that 1x1 tiles can be detected laterr)
                    // follow the border of that tunnel (going counter-clockwise along the inside of the tunnel) until the path/border ends
                }
                
                // switch target tiles
                if (targetTile == 1) {
                    targetTile = 0;
                } else if (targetTile == 0) {
                    targetTile = 1;
                }
                justSwitched = true; // record that the target tile just switched
            } else {
                justSwitched = false; // record that the target tile hadn't been switched
            }
        }
        justSwitched = false; // reset justSwitched variable
    }
    
    // fix the collision edges
    
    for (int j = 0; j < numTunnelEdges; j++) {
        if (tunnelEdges [j].dir == 2) {
            // Vector2 tempV0 = Vector2 (tunnelEdges [j].x, tunnelEdges [j].y); // avoid deep copy bugs
            tunnelEdges [j] = Edge (tunnelEdges [j].verts [1],  tunnelEdges [j].verts [0], 2);
        } else if (tunnelEdges [j].dir == 3) {
            tunnelEdges [j] = Edge (tunnelEdges [j].verts [1],  tunnelEdges [j].verts [0], 3);
        }
    }
    
    
    // print out the visited tile array
    cout << endl;
    cout << "     ";
    for (int k = 0; k < GRID_WIDTH; k++) {
            cout << k % 10 << " ";
    }
    cout << endl << endl;
    for (int j = 0; j < GRID_HEIGHT; j++) {
        cout << j << ":  ";
        if (j < 10) {
            cout << " ";
        }
        for (int k = 0; k < GRID_WIDTH; k++) {
            if (visitedTiles [j][k] != 0) {
                // cout << "." << " ";
                cout << visitedTiles [j][k] << " ";
                // cout << visitedTiles [j][k] << visitedTiles [j][k];
            } else {
                // cout << visitedTiles [j][k] << "o";
                cout << "." << " ";
            }
        }
        cout << endl;
    }
    
    
    // print the landform array
    cout << endl;
    cout << "     ";
    for (int k = 0; k < GRID_WIDTH; k++) {
        cout << k % 10 << " ";
    }
    cout << endl << endl;
    for (int j = 0; j < GRID_HEIGHT; j++) {
        cout << j << ":  ";
        if (j < 10) {
            cout << " ";
        }
        for (int k = 0; k < GRID_WIDTH; k++) {
            if (landform [j][k] == '0') {
                cout << "." << " ";
            } else {
                cout << landform [j][k] << " ";
            }
        }
        cout << endl;
    }
    
    
    // generate background boxes
    numTunnelBoxes = 0;
    
    float TOLER = -0; // tolerance for determining if within boundaries
    for (int j = 0; j < numTunnelEdges; j++) {
        Edge Jedge = tunnelEdges [j];
        if (Jedge.dir == 0) {
            bool again = false;
            bool firstIteration = true;
            int minIndex = -1;
            do {
                // find closes tunnel edge that is above the current tunnel edge
                float closestDistance = 42069;
                for (int k = 0; k < numTunnelEdges; k++) {
                    Edge Kedge = tunnelEdges [k];
                    if (Kedge.dir == 2  &&  k != minIndex) {
                        // if either of the verticies denoting the k tunnel edge are within the X bounds of the j tunnel
                        // or both of the verticies denoting the k tunnel edge are within the X bounds of the j tunnel  (it will always be both if it meets exclusively this condition...)
                        if ((Kedge.verts [0].x - TOLER > Jedge.verts [0].x  &&  Kedge.verts [0].x + TOLER < Jedge.verts [1].x)     ||     (Kedge.verts [1].x + TOLER < Jedge.verts [1].x  &&  Kedge.verts [1].x - TOLER > Jedge.verts [0].x)
                            ||     (Jedge.verts [0].x - TOLER >= Kedge.verts [0].x  &&  Jedge.verts [1].x + TOLER <= Kedge.verts [1].x)) {
                            // if that tunnel edge is closer than the current closest tunnel edge
                            float currentDistance = Kedge.verts [0].y - Jedge.verts [0].y; // (note that the k tunnel edge should be above the j tunnel edge)
                            if (currentDistance > 0  &&  currentDistance < closestDistance) {
                                // then store that as the current closest tunnel edge
                                closestDistance = currentDistance;
                                minIndex = k;
                            }
                        }
                    }
                }
                if (firstIteration == true) { // if its the very first iteration...
                    tunnelEdges [j].closestEdge = closestDistance;
                }
                again = false;
                // if there are no tunnel edges above that tunnel edge, then use the upper bounds of the screen
                if (closestDistance >= 42042) { // (note that it shouldn't matter if the current edge is at the very top of the screen, though that shouldn't ever happen)
                    closestDistance = 0 - Jedge.verts [0].y;
                    if (firstIteration == true) {
                        tunnelEdges [j].absoluteEdge = true;
                    }
                    // closestDistance = 2;
                } else {
                    again = true;
                }
                Vector2 v0 = Vector2 (Jedge.verts [0].x, Jedge.verts [0].y + closestDistance);
                float width = Jedge.verts [1].x - Jedge.verts [0].x;
                backgroundBoxes [numTunnelBoxes] = Box (v0, width, closestDistance);
                if (again) {
                    Vector2 p0;
                    Vector2 p1;
                    Edge Kedge = tunnelEdges [minIndex];
                    int TOLER2 = 1;
                    if (Kedge.verts [0].x > Jedge.verts [0].x  &&  Kedge.verts [1].x < Jedge.verts [1].x) { // if both
                        p1 = Vector2 (Kedge.verts [1].x, Kedge.verts [1].y);
                        p0 = Vector2 (Kedge.verts [0].x, Kedge.verts [0].y);
                    } else if (Kedge.verts [0].x - TOLER2 > Jedge.verts [0].x) { // would equal Jedge in the other scenario
                        p1 = Vector2 (Kedge.verts [0].x, Kedge.verts [0].y);
                        p0 = backgroundBoxes [numTunnelBoxes].verts [0];
                    } else if (Kedge.verts [1].x + TOLER2 < Jedge.verts [1].x) {
                        p0 = Vector2 (Kedge.verts [1].x, Kedge.verts [1].y);
                        p1 = backgroundBoxes [numTunnelBoxes].verts [1];
                    }
                    if (p0.x < p1.x) {
                        Jedge = Edge (p0, p1, 0);
                        tunnelEdges [j].isIsland = true;
                    } else {
                        again = false;
                        cout << "Ohnoibaboi" << endl;
                    }
                }
                firstIteration = false;
                numTunnelBoxes++;
            } while (again == true);
        } else if (Jedge.dir == 2) {
            bool again = false;
            bool firstIteration = true;
            int minIndex = -1;
            do {
                // find closes tunnel edge that is above the current tunnel edge
                float closestDistance = 42069;
                for (int k = 0; k < numTunnelEdges; k++) { // should probably also compare it to other background boxes to avoid overlap and imporve efficiency
                    Edge Kedge = tunnelEdges [k];
                    if (Kedge.dir == 0  &&  k != minIndex) {
                        // if either of the verticies denoting the k tunnel edge are within the X bounds of the tunnel
                        // or both of the verticies denoting the k tunnel edge are within the X bounds of the j tunnel
                        if ((Kedge.verts [0].x - TOLER > Jedge.verts [0].x  &&  Kedge.verts [0].x + TOLER < Jedge.verts [1].x)     ||     (Kedge.verts [1].x + TOLER < Jedge.verts [1].x  &&  Kedge.verts [1].x - TOLER > Jedge.verts [0].x)
                            ||     (Jedge.verts [0].x - TOLER >= Kedge.verts [0].x  &&  Jedge.verts [1].x + TOLER <= Kedge.verts [1].x)) {
                            // if that tunnel edge is closer than the current closest tunnel edge
                            float currentDistance = Jedge.verts [0].y - Kedge.verts [0].y; // (note that the k tunnel edge should be below the j tunnel edge)
                            if (currentDistance > 0  &&  currentDistance < closestDistance) {
                                // then store that as the current closest tunnel edge
                                closestDistance = currentDistance;
                                minIndex = k;
                            }
                        }
                    }
                }
                if (firstIteration == true) { // if its the very first iteration...
                    tunnelEdges [j].closestEdge = closestDistance;
                }
                again = false;
                // if there are no tunnel edges above that tunnel edge, then use the upper bounds of the screen
                if (closestDistance >= 42042) { // (note that it shouldn't matter if the current edge is at the very top of the screen, though that shouldn't ever happen)
                    closestDistance = Jedge.verts [0].y - -SCREEN_HEIGHT;
                    if (firstIteration == true) {
                        tunnelEdges [j].absoluteEdge = true;
                    }
                    // closestDistance = 2;
                } else {
                    again = true;
                }
                Vector2 v0 = Vector2 (Jedge.verts [0].x, Jedge.verts [0].y);
                float width = Jedge.verts [1].x - Jedge.verts [0].x;
                backgroundBoxes [numTunnelBoxes] = Box (v0, width, closestDistance);
                if (again) {
                    Vector2 p0;
                    Vector2 p1;
                    Edge Kedge = tunnelEdges [minIndex];
                    int TOLER2 = 1;
                    if (Kedge.verts [0].x > Jedge.verts [0].x  &&  Kedge.verts [1].x < Jedge.verts [1].x) { // if both
                        p1 = Vector2 (Kedge.verts [1].x, Kedge.verts [1].y);
                        p0 = Vector2 (Kedge.verts [0].x, Kedge.verts [0].y);
                    } else if (Kedge.verts [0].x - TOLER2 > Jedge.verts [0].x) { // would equal Jedge in the other scenario
                        p1 = Vector2 (Kedge.verts [0].x, Kedge.verts [0].y);
                        p0 = backgroundBoxes [numTunnelBoxes].verts [2]; // bottom left vert
                    } else if (Kedge.verts [1].x + TOLER2 < Jedge.verts [1].x) {
                        p0 = Vector2 (Kedge.verts [1].x, Kedge.verts [1].y);
                        p1 = backgroundBoxes [numTunnelBoxes].verts [3]; // bottom right vert
                    }
                    if (p0.x < p1.x) {
                        Jedge = Edge (p0, p1, 0);
                    } else {
                        again = false;
                        tunnelEdges [j].isIsland = true;
                        cout << "Ohnoibaboi" << endl;
                    }
                }
                numTunnelBoxes++;
                firstIteration = false;
            } while (again == true);
        }
    }
    
}





// for level drawing function; returns true if the inputted coords are within the bounds of the map
int IsValid (int map [][GRID_WIDTH], int x, int y) {
    int result = true;
    // int x = pos.x;
    // int y = -pos.y;
    y = -y;
    
    IntVector2 minBounds = IntVector2 (0, 0); // 1, 1
    IntVector2 maxBounds = IntVector2 (GRID_WIDTH, GRID_HEIGHT); // GRID_WIDTH, GRID_HEIGHT-1
    
    // check to see if it doesn't exceed the bounds
    if (x < minBounds.x || y < minBounds.y || x >= maxBounds.x || y >= maxBounds.y) {
        result = 0;
    }
    // I might keep this part...
    //else if (map [y][x] == 1) {
    //    result = 0;
    //}
    
    return result;
}


#define REAL_WALL_HEIGHT 30
float VERT_SHADOW_SIZE = 0; // REAL_WALL_HEIGHT * 0.35; // vertical shadow size
float HORI_SHADOW_SIZE = 8; // REAL_WALL_HEIGHT * 0.25; // horizontal shadow size
int VERT_SHADOW_TILE_SIZE = (VERT_SHADOW_SIZE) / TILE_SIZE + 0.99999; // vertical shadow size
int HORI_SHADOW_TILE_SIZE = (HORI_SHADOW_SIZE) / TILE_SIZE + 0.99999; // horizontal shadow size
// VERT_SHADOW_TILE_SIZE = 2;
// HORI_SHADOW_TILE_SIZE = 1;
#define RAILING_HEIGHT 5 // 5
#define TOPTHING_HEIGHT 8
#define TOPTHING_SHADOW_HEIGHT 2
int floorPattern = 0;

// draws good level backdrop
void DrawLevel () {
    // draw tile floor
    // LCD.SetFontColor (TILECOLOR); // TILECOLOR
    // cout << HORI_SHADOW_TILE_SIZE << endl;
    if (highDetail) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            for (int k = 0; k < GRID_WIDTH; k++) {
                if (map [j][k] == 1) {
                    int primaryColor;
                    int shadowColor;
                    switch (floorPattern) {
                        case 1:
                            // determine what color the tiles should be based on whether they are odd or even
                            if (j%2 == 0 && k%2 == 1  ||  j%2 == 1 && k%2 == 0) {
                                primaryColor = TILECOLOR;
                                shadowColor = TILESHADOW;
                            } else {
                                primaryColor = BACKGROUNDCOLOR;
                                shadowColor = BACKGROUNDSHADOW;
                            }
                            break;
                        case 2:
                            if (j%4 == 0 && k%4 == 1  ||  j%4 == 1 && k%4 == 2  ||  j%4 == 2 && k%4 == 3  ||  j%4 == 3 && k%4 == 0) {
                                primaryColor = TILECOLOR2;
                                shadowColor = TILESHADOW2;
                            } else if (j%4 == 0 && k%4 == 2  ||  j%1 == 1 && k%4 == 3  ||  j%4 == 2 && k%4 == 0  ||  j%4 == 3 && k%4 == 1) {
                                primaryColor = TILECOLOR2;
                                shadowColor = TILESHADOW2;
                            } else if (j%4 == 0 && k%4 == 3  ||  j%1 == 1 && k%4 == 0  ||  j%4 == 2 && k%4 == 1  ||  j%4 == 3 && k%4 == 2) {
                                primaryColor = TILECOLOR3;
                                shadowColor = TILESHADOW3;
                            } else {
                                primaryColor = BACKGROUNDCOLOR;
                                shadowColor = BACKGROUNDSHADOW;
                            }
                            break;
                        case 3:
                            if (j%2 == 0 && k%4 == 1  ||  j%2 == 1 && k%4 == 0) {
                                primaryColor = TILECOLOR;
                                shadowColor = TILESHADOW;
                            } else if (j%2 == 0 && k%4 == 0  ||  j%2 == 1 && k%4 == 1) {
                                primaryColor = TILECOLOR2;
                                shadowColor = TILESHADOW2;
                            } else if (j%2 == 0 && k%4 == 3  ||  j%2 == 1 && k%4 == 2) {
                                primaryColor = TILECOLOR3;
                                shadowColor = TILESHADOW3;
                            } else {
                                primaryColor = BACKGROUNDCOLOR;
                                shadowColor = BACKGROUNDSHADOW;
                            }
                            break;
                        case 4:
                            if (j%4 == 0 && k%4 == 1  ||  j%4 == 1 && k%4 == 0) {
                                primaryColor = TILECOLOR;
                                shadowColor = TILESHADOW;
                            } else if (j%4 == 0 && k%4 == 0  ||  j%4 == 1 && k%4 == 1) {
                                primaryColor = TILECOLOR2;
                                shadowColor = TILESHADOW2;
                            } else if (j%4 == 0 && k%4 == 3  ||  j%4 == 1 && k%4 == 2) {
                                primaryColor = TILECOLOR3;
                                shadowColor = TILESHADOW3;
                            } else if (j%4 == 0 && k%4 == 2  ||  j%4 == 1 && k%4 == 3) {
                                primaryColor = BLACK;
                                shadowColor = BLACK;
                            } else {
                                primaryColor = BACKGROUNDCOLOR;
                                shadowColor = BACKGROUNDSHADOW;
                            }
                            break;
                        case 5:
                            if (j%2 == 0 && k%4 == 0) {
                                primaryColor = TILECOLOR;
                                shadowColor = TILESHADOW;
                            } else if (j%2 == 0 && k%4 == 1) {
                                primaryColor = TILECOLOR2;
                                shadowColor = TILESHADOW2;
                            } else if (j%2 == 0 && k%4 == 2) {
                                primaryColor = TILECOLOR3;
                                shadowColor = TILESHADOW3;
                            } else if (j%2 == 0 && k%4 == 3) {
                                primaryColor = BLACK;
                                shadowColor = BLACK;
                            } else {
                                primaryColor = BACKGROUNDCOLOR;
                                shadowColor = BACKGROUNDSHADOW;
                            }
                            break;
                        case 6: // good 2x2
                            if (j%2 == 0 && k%2 == 0) {
                                primaryColor = TILECOLOR;
                                shadowColor = TILESHADOW;
                            } else if (j%2 == 0 && k%2 == 1) {
                                primaryColor = TILECOLOR2;
                                shadowColor = TILESHADOW2;
                            } else if (j%2 == 1 && k%2 == 0) {
                                primaryColor = TILECOLOR3;
                                shadowColor = TILESHADOW3;
                            } else if (j%2 == 1 && k%2 == 1) {
                                primaryColor = BACKGROUNDCOLOR;
                                shadowColor = BACKGROUNDSHADOW;
                            } else {
                                primaryColor = BACKGROUNDCOLOR;
                                shadowColor = BACKGROUNDSHADOW;
                            }
                            break;
                        case 7:
                            if (j%2 == 0 && k%2 == 0) {
                                primaryColor = TILECOLOR2;
                                shadowColor = TILESHADOW2;
                            } else {
                                primaryColor = TILECOLOR3;
                                shadowColor = TILESHADOW3;
                            }
                            break;
                        case 8:
                            if (j%8 == 0 && k%8 == 0  ||  j%8 == 1 && k%8 == 1) {
                                primaryColor = TILECOLOR2;
                                shadowColor = TILESHADOW2;
                            } else if (j%8 == 4 && k%8 == 4  ||  j%8 == 5 && k%8 == 5) {
                                primaryColor = TILECOLOR;
                                shadowColor = TILESHADOW;
                            } else {
                                primaryColor = TILECOLOR3;
                                shadowColor = TILESHADOW3;
                            }
                            break;
                        case 0:
                            if (j%10 == 0 && k%9 == 0  ||  j%5 == 2 && k%9 == 4) {
                                primaryColor = TILECOLOR;
                                shadowColor = TILESHADOW;
                            } else if (j%5 == 1 && k%9 == 3  ||  j%5 == 1 && k%9 == 5  ||  j%5 == 2 && k%9 == 2  ||  j%5 == 2 && k%9 == 6  ||  j%5 == 3 && k%9 == 3  ||  j%5 == 3 && k%9 == 5) {
                                primaryColor = TILECOLOR2;
                                shadowColor = TILESHADOW2;
                            } else {
                                primaryColor = BACKGROUNDCOLOR;
                                shadowColor = BACKGROUNDSHADOW;
                            }
                            break;
                        case 9:
                            if (k%10 == 0 && j%9 == 0  ||  k%5 == 2 && j%9 == 4) {
                                primaryColor = TILECOLOR;
                                shadowColor = TILESHADOW;
                            } else if (k%5 == 1 && j%9 == 3  ||  k%5 == 1 && j%9 == 5  ||  k%5 == 2 && j%9 == 2  ||  k%5 == 2 && j%9 == 6  ||  k%5 == 3 && j%9 == 3  ||  k%5 == 3 && j%9 == 5) {
                                primaryColor = TILECOLOR2;
                                shadowColor = TILESHADOW2;
                            } else {
                                primaryColor = BACKGROUNDCOLOR;
                                shadowColor = BACKGROUNDSHADOW;
                            }
                            break;
                        default:
                            primaryColor = BACKGROUNDCOLOR;
                            shadowColor = BACKGROUNDSHADOW;
                            break;
                    }
                    Vector2 v0 = Vector2 (k * TILE_SIZE, -j * TILE_SIZE);
                    IntVector2 boxDimensions = IntVector2 (TILE_SIZE, TILE_SIZE);;
                    bool isShadow = false;
                    bool vertical = false;
                    bool horizontal = false;
                    bool wasLastVertical = false;
                    bool wasLastHorizontal = false;
                    bool isConvex = false; // if the tile denotes a shadow tile at a convex corner (only needs to be true at the absolute corner for it to work properly at present)
                    bool isCorner = false; // more specifically is non-convex corner
                    
                    // vertical first
                    for (int n = 1; n <= VERT_SHADOW_TILE_SIZE; n++) {
                        if (IsValid (map, k, -(j - n))) {
                            if (map [j - n][k] == 0) {
                                isShadow = true;
                                vertical = true;
                                if (n == VERT_SHADOW_TILE_SIZE) {
                                    wasLastVertical = true;
                                    // identify if it is a corner
                                    if (IsValid (map, k - 1, -(j - n))  &&  map [j - n][k - 1] == 1) { // if to the left of that wall tile is tunnel
                                        isCorner = true;
                                    }
                                }
                                break;
                            }
                        } else {
                            break;
                        }
                    }
                    // then horizontal
                    for (int p = 1; p <= HORI_SHADOW_TILE_SIZE; p++) {
                        if (IsValid (map, k - p, -j)) {
                            if (map [j][k - p] == 0) {
                                isShadow = true;
                                horizontal = true;
                                if (p == HORI_SHADOW_TILE_SIZE) {
                                    wasLastHorizontal = true;
                                    // identify if it is a corner
                                    if (IsValid (map, k - p, -(j - 1))  &&  map [j - 1][k - p] == 1) { // if above that wall tile is tunnel
                                        isCorner = true;
                                    }
                                }
                                break;
                            }
                        } else {
                            break;
                        }
                    }
                    // then both
                    if (!isShadow) {
                        // denotes the closest coors in which an adjacent tile is found
                        int minN = VERT_SHADOW_TILE_SIZE + 1;
                        int minP = HORI_SHADOW_TILE_SIZE + 1;
                        for (int n = 0; n <= VERT_SHADOW_TILE_SIZE; n++) { // n makes it iterate vertically
                            for (int p = 0; p <= HORI_SHADOW_TILE_SIZE; p++) { // p makes it iterate horizontally
                                if (IsValid (map, k - p, -(j - n))) {
                                    if (map [j - n][k - p] == 0) {
                                        if (n < minN && p < minP) {
                                            minN = n;
                                            minP = p;
                                        }
                                        // all the tiles that I would be scanning for at this point are both vertical and horizontal tiles
                                        // so mark them as so (may wanna change this in the future if u wanna combine the all the for loops or perhaps for testing purposes)
                                        // vertical = true;
                                        // horizontal = true;
                                        isShadow = true;
                                    }
                                }
                            }
                        }
                        
                        // if the tile has become a shadow tile since that for loop had occured
                        if (isShadow) {
                            if (minN == VERT_SHADOW_TILE_SIZE) {
                                wasLastVertical = true;
                                //cout << "vert" << endl;
                            }
                            if (minP == HORI_SHADOW_TILE_SIZE) {
                                wasLastHorizontal = true;
                                //cout << "hori" << endl;
                            }
                            isConvex = true;
                            // (doesn't actually bc of the way my code is set up)
                            // vertical = true;
                            // horizontal = true;
                        }
                        
                    }
                    
                    isCorner = false;  // note that corner drawing currently only works when  <  TILE_SIZE ... actually never works ... (it doesn't make realistic looking shadows
                    
                    // draw the shadow tiles that are at the edges/ends of the shadows differently
                    if (isShadow) {
                        LCD.SetFontColor (shadowColor);
                        // if it was both a horizontal and vertical edge tile
                        if (wasLastVertical && wasLastHorizontal) {
                            // if its at the concave corner (the non-artificial corner)
                            if (!isConvex) {
                                // draw shadow box
                                DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                                // modify dimensions of the non-shadow box
                                int width = (int) HORI_SHADOW_SIZE % TILE_SIZE;
                                int height = (int) VERT_SHADOW_SIZE % TILE_SIZE;
                                if (height != 0 && width != 0) {
                                    v0 = Vector2 (v0.x + width, v0.y - height);
                                    boxDimensions = IntVector2 (TILE_SIZE - width, TILE_SIZE - height);
                                    LCD.SetFontColor (primaryColor);
                                }
                                // draw modified non-shadow, normal box
                                DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                            }
                            // if its at the convex corner (the artificial corner)
                            else {
                                // draw normal box
                                LCD.SetFontColor (primaryColor);
                                DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                                // modify dimensions of the shadow box
                                int width = (int) HORI_SHADOW_SIZE % TILE_SIZE;
                                int height = (int) VERT_SHADOW_SIZE % TILE_SIZE;
                                if (height == 0) { // && VERT_SHADOW_SIZE > 0
                                    height = TILE_SIZE;
                                }
                                if (width == 0) { // && HORI_SHADOW_SIZE > 0
                                    width = TILE_SIZE;
                                }
                                // if (height != 0 && width != 0) {
                                v0 = Vector2 (v0.x, v0.y);
                                boxDimensions = IntVector2 (width, height);
                                LCD.SetFontColor (primaryColor);
                                // }
                                // draw shadow box
                                LCD.SetFontColor (shadowColor);
                                DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                            }
                        }
                        // if it was just a vertical end tile (and also not any flavor of a horizontal tile)
                        else if (wasLastVertical && !horizontal) {
                            if (!(isCorner && isComputer)) {
                                // draw shadow box
                                DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                                // modify dimensions of the non-shadow box
                                int height = (int) VERT_SHADOW_SIZE % TILE_SIZE;
                                if (height != 0) {
                                    v0 = Vector2 (v0.x, v0.y - height);
                                    boxDimensions = IntVector2 (TILE_SIZE, TILE_SIZE - height);
                                    LCD.SetFontColor (primaryColor);
                                }
                                // draw modified non-shadow, normal box
                                DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                            } else {
                                // draw normal box
                                LCD.SetFontColor (primaryColor);
                                DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                                
                                int height = (int) VERT_SHADOW_SIZE % TILE_SIZE;
                                if (height == 0) { // && VERT_SHADOW_SIZE > 0
                                    height = TILE_SIZE;
                                }
                                
                                // create triangle for shadow
                                int triangleX [3];
                                int triangleY [3];
                                // create top left point 0
                                triangleX [0] = v0.x;
                                triangleY [0] = -v0.y;
                                // create point 1
                                triangleX [1] = v0.x + height;
                                triangleY [1] = -v0.y;
                                // create point 2
                                triangleX [2] = v0.x + height;
                                triangleY [2] = -(v0.y - height);
                                // draw the shadow triangle
                                LCD.SetFontColor (shadowColor);
                                LCD.FillPolygon (triangleX, triangleY, 3);
                                
                                // draw shadow box
                                v0 = Vector2 (v0.x + height, v0.y);
                                boxDimensions = IntVector2 (TILE_SIZE - height, height);
                                DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                            }
                        }
                        // if it was just a horizontal end tile (and also not any flavor of a vertical tile)
                        else if (wasLastHorizontal && !vertical) {
                            if (!(isCorner && isComputer)) {
                                // draw shadow box
                                DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                                // modify dimensions of the non-shadow box
                                int width = (int) HORI_SHADOW_SIZE % TILE_SIZE;
                                if (width != 0) {
                                    v0 = Vector2 (v0.x + width, v0.y);
                                    boxDimensions = IntVector2 (TILE_SIZE - width, TILE_SIZE);
                                    LCD.SetFontColor (primaryColor);
                                }
                                // draw modified non-shadow, normal box
                                DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                            } else {
                                // draw normal box
                                LCD.SetFontColor (primaryColor);
                                DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                                
                                int width = (int) HORI_SHADOW_SIZE % TILE_SIZE;
                                if (width == 0) { // && VERT_SHADOW_SIZE > 0
                                    width = TILE_SIZE;
                                }
                                
                                // create triangle for shadow
                                int triangleX [3];
                                int triangleY [3];
                                // create top left point 0
                                triangleX [0] = v0.x;
                                triangleY [0] = -v0.y;
                                // create point 1
                                triangleX [1] = v0.x;
                                triangleY [1] = -(v0.y - width);
                                // create point 2
                                triangleX [2] = v0.x + width;
                                triangleY [2] = -(v0.y - width);
                                // draw the shadow triangle
                                LCD.SetFontColor (shadowColor);
                                LCD.FillPolygon (triangleX, triangleY, 3);
                                
                                // draw shadow box
                                v0 = Vector2 (v0.x, v0.y - width);
                                boxDimensions = IntVector2 (width, TILE_SIZE - width);
                                DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                            }
                        } else {
                            // draw un-modified, shadow box
                            DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                        }
                    } else {
                        LCD.SetFontColor (primaryColor);
                        // boxDimensions = IntVector2 (TILE_SIZE, TILE_SIZE);
                        // draw normal box
                        DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                    }
                    
                    // DrawBox (  Box (v0, boxDimensions.x, boxDimensions.y)  );
                }
            }
        }
    }
    
    // draw tunnel background boxes
    LCD.SetFontColor (ABYSS); // wall color
    for (int j = 0; j < numTunnelBoxes; j++) {
        DrawBox (backgroundBoxes[j]);
    }
    
    // draw background portion of scenic walls
    if (0 == 0) {
        // LCD.SetFontColor (WALLWALLCOLOR);
        for (int j = 0; j < numTunnelEdges; j++) {
            if (tunnelEdges [j].dir == 0) {
            // if (tunnelEdges [j].dir == 0  &&  tunnelEdges [j].closestEdge > REAL_WALL_HEIGHT) {
            // if (tunnelEdges [j].dir == 0  &&  tunnelEdges [j].absoluteEdge) {
                Box wall;
                if (tunnelEdges [j].absoluteEdge || tunnelEdges [j].closestEdge >= REAL_WALL_HEIGHT) {
                    Vector2 v0 = Vector2 (tunnelEdges [j].verts [0].x, tunnelEdges [j].verts [0].y + REAL_WALL_HEIGHT);
                    float width = tunnelEdges [j].verts [1].x - tunnelEdges [j].verts [0].x;
                    wall = Box (v0, width, REAL_WALL_HEIGHT);
                    
                    if (0 == 1 && tunnelEdges [j].absoluteEdge) { // hopefully change in the future
                        // draw top thing
                        Vector2 t0 = Vector2 (tunnelEdges [j].verts [0].x, tunnelEdges [j].verts [0].y + REAL_WALL_HEIGHT + TOPTHING_HEIGHT);
                        Box topThing = Box (t0, wall.width, TOPTHING_HEIGHT);
                        LCD.SetFontColor (TOPTHINGCOLOR);
                        DrawBox (topThing);
                        // draw top thing for the right side
                        int k = j+1;
                        if (k < numTunnelEdges) {
                            if (tunnelEdges [k].dir == 1) { // going down
                                Vector2 k0 = Vector2 (tunnelEdges [j].verts [1].x, tunnelEdges [j].verts [0].y + REAL_WALL_HEIGHT + TOPTHING_HEIGHT);
                                float height = k0.y - tunnelEdges [k].verts [1].y;
                                topThing = Box (k0, TOPTHING_HEIGHT, height);
                                DrawBox (topThing);
                            }
                            else if (tunnelEdges [k].dir == 3) { // going up
                                Vector2 k0 = Vector2 (tunnelEdges [j].verts [1].x - TOPTHING_HEIGHT, tunnelEdges [k].verts [0].y + REAL_WALL_HEIGHT + TOPTHING_HEIGHT);
                                float height = tunnelEdges [k].verts [0].y - tunnelEdges [j].verts [0].y;
                                topThing = Box (k0, TOPTHING_HEIGHT, height);
                                DrawBox (topThing);
                            }
                        }
                        // draw shadow of top thing
                        t0 = Vector2 (tunnelEdges [j].verts [0].x, tunnelEdges [j].verts [0].y + REAL_WALL_HEIGHT + TOPTHING_SHADOW_HEIGHT);
                        topThing = Box (t0, wall.width, TOPTHING_SHADOW_HEIGHT);
                        LCD.SetFontColor (TOPTHINGSHADOW);
                        DrawBox (topThing);
                    }
                }
                else {
                    Vector2 v0 = Vector2 (tunnelEdges [j].verts [0].x, tunnelEdges [j].verts [0].y + tunnelEdges [j].closestEdge);
                    float width = tunnelEdges [j].verts [1].x - tunnelEdges [j].verts [0].x;
                    wall = Box (v0, width, tunnelEdges [j].closestEdge);
                }
                
                LCD.SetFontColor (WALLWALLCOLOR);
                DrawBox (wall);
                
                // draw railing
                Vector2 r0 = Vector2 (tunnelEdges [j].verts [0].x, tunnelEdges [j].verts [0].y + RAILING_HEIGHT);
                Box railing = Box (r0, wall.width, RAILING_HEIGHT);
                LCD.SetFontColor (RAILINGSHADOW);
                DrawBox (railing);
                r0 = Vector2 (tunnelEdges [j].verts [0].x, tunnelEdges [j].verts [0].y + RAILING_HEIGHT + TOPTHING_SHADOW_HEIGHT);
                railing = Box (r0, wall.width, TOPTHING_SHADOW_HEIGHT);
                LCD.SetFontColor (RAILINGCOLOR);
                // DrawBox (railing);
                
                // draw top thing
                // ...
            }
        }
        if (!highDetail) {
            DrawLevelForeground ();
        }
    }
    
    // draw tunnel edges
    if (0 == 1  &&  highDetail == false) {
        LCD.SetFontColor (BLACK);
        for (int j = 0; j < numTunnelEdges; j++) {
            /*
            switch (tunnelEdges [j].dir) {
                case 0:
                    LCD.SetFontColor (RED);
                    break;
                case 1:
                    LCD.SetFontColor (YELLOW);
                    break;
                case 2:
                    LCD.SetFontColor (GREEN);
                    break;
                case 3:
                    LCD.SetFontColor (BLUE);
                    break;
                default:
                    LCD.SetFontColor (WHITE);
                    break;
            }
            */
            
            DrawEdge (tunnelEdges [j]);
        }
    }
}


// draw the foreground of the level; only performed when high detail is enabled
void DrawLevelForeground () {
    // draw foreground portion of scenic walls
    // LCD.SetFontColor (WALLWALLCOLOR);
    for (int j = 0; j < numTunnelEdges; j++) {
        if (tunnelEdges [j].dir == 0  &&  !tunnelEdges [j].absoluteEdge) { //   &&  tunnelEdges [j].closestEdge > REAL_WALL_HEIGHT
            // if (tunnelEdges [j].dir == 0  &&  tunnelEdges [j].absoluteEdge) {
            Vector2 v0 = Vector2 (tunnelEdges [j].verts [0].x, tunnelEdges [j].verts [0].y + REAL_WALL_HEIGHT);
            float width = tunnelEdges [j].verts [1].x - tunnelEdges [j].verts [0].x;
            Box wall = Box (v0, width, (REAL_WALL_HEIGHT - tunnelEdges [j].closestEdge));
            
            if (tunnelEdges [j].closestEdge < REAL_WALL_HEIGHT) {
                LCD.SetFontColor (WALLWALLCOLOR);
                DrawBox (wall);
            }
            
            // draw ceiling thing if necessary    // methinks I should definitely alter how this is done / handeled ( as in being done at all)
            // if (!tunnelEdges [j].absoluteEdge && tunnelEdges [j].closestEdge < 2 * REAL_WALL_HEIGHT) { // hmmmm...
            // if (!tunnelEdges [j].absoluteEdge  &&  tunnelEdges [j].closestEdge < 20 * REAL_WALL_HEIGHT) { // hmmmm... redundant
            if (tunnelEdges [j].edgeType == 'i') {
                // // determine the height of the thing
                Vector2 c0 = Vector2 (v0.x, v0.y + tunnelEdges [j].closestEdge);
                
                Box ceiling = Box (c0, width, tunnelEdges [j].closestEdge);
                
                // LCD.SetFontColor (WALLCOLOR);
                LCD.SetFontColor (ABYSS);
                DrawBox (ceiling);
            }
        } if (tunnelEdges [j].dir == 0) {
            Vector2 t0 = Vector2 (tunnelEdges [j].verts [0].x, tunnelEdges [j].verts [0].y + REAL_WALL_HEIGHT + TOPTHING_HEIGHT);
            float width = tunnelEdges [j].verts [1].x - tunnelEdges [j].verts [0].x;
            Box topThing = Box (t0, width, TOPTHING_HEIGHT);
            LCD.SetFontColor (TOPTHINGCOLOR);
            DrawBox (topThing);
            // draw top thing for the right side
            int k = j+1;
            if (k < numTunnelEdges) {
                if (tunnelEdges [k].dir == 1) { // going down
                    Vector2 k0 = Vector2 (tunnelEdges [j].verts [1].x, tunnelEdges [j].verts [0].y + REAL_WALL_HEIGHT + TOPTHING_HEIGHT);
                    float height = k0.y - tunnelEdges [k].verts [1].y - REAL_WALL_HEIGHT;
                    topThing = Box (k0, TOPTHING_HEIGHT, height);
                    DrawBox (topThing);
                }
                else if (tunnelEdges [k].dir == 3) { // going up
                    Vector2 k0 = Vector2 (tunnelEdges [j].verts [1].x - TOPTHING_HEIGHT, tunnelEdges [k].verts [0].y + REAL_WALL_HEIGHT + TOPTHING_HEIGHT);
                    float height = tunnelEdges [k].verts [0].y - tunnelEdges [j].verts [0].y;
                    topThing = Box (k0, TOPTHING_HEIGHT, height);
                    DrawBox (topThing);
                }
            }
            // draw shadow of top thing
            t0 = Vector2 (tunnelEdges [j].verts [0].x, tunnelEdges [j].verts [0].y + REAL_WALL_HEIGHT + TOPTHING_SHADOW_HEIGHT);
            topThing = Box (t0, width, TOPTHING_SHADOW_HEIGHT);
            LCD.SetFontColor (TOPTHINGSHADOW);
            DrawBox (topThing);
        }
    }
}






/************************************************************************ PHYSICS FUNCTIONS ************************************************************************/




// takes the users inputted acceleration as a parameter and calculates the translation of the object and updates its position
void PhysicsObject::Translate (Vector2 accel) {
    // float fair = .0025; // initialize air friction amount // may want to make this global
    
    vel = Vector2 (vel.x - vel.x * AIRF, vel.y - vel.y * AIRF); // subtract acceleration due to air friction from velocity vector
    vel = Vector2 (vel.x + accel.x, vel.y + accel.y); // add acceleration input vector to velocity vector
    
    // detect if playerObj is colliding with anything and adjust the velocity if it is
    // (collide with boxes); note that there are currently are no boxes to collide with
    for (int j = 0; j < BOXES_SIZE; j++) {
        CollideBox (boxes [j]); // detects if the physics object is colliding with inputted collision box and changes its velocity if it is
    }
    // (collide with screen border edges)
    for (int j = 0; j < EDGES_SIZE; j++) {
        // (this is purely a game mechanic and should be removed for future uses)
        if (j == 1 && wfs.heldByPlayer == true) { // if the player has the wetfloor sign, then let him pass and don't collide with the right screen boundary
            
        } else {
            CollideEdge (edges [j]); // detects if the physics object is colliding with inputted collision edge and changes its velocity if it is
        }
    }
    // (collide with tunnel edges)
    for (int j = 0; j < numTunnelEdges; j++) { // TUNNEL_EDGE_SIZE
        // CollideEdge (toptunnelEdges[j]); // detects if the physics object is colliding with inputted collision edge and changes its velocity if it is
        // CollideEdge (bottomtunnelEdges[j]); // detects if the physics object is colliding with inputted collision edge and changes its velocity if it is
        CollideEdge (tunnelEdges [j]);
    }
    
    pos = Vector2 (pos.x + vel.x, pos.y + vel.y); // add velocity vector to position vector
}



// Note that...
// STATF = .35;     // coefficient of static friction
// KINF = 0.3;      // coefficient of kinetic friction
// AIRF = .0025;    // coefficient of air friction
// RESTI = .35;     // coefficient of restitution // edited
// ERROR_MARGIN = 7;



// detects if the physics object is colliding with inputted collision box and realistically changes its velocity if it is
void PhysicsObject::CollideBox (Box obj) {
    Vector2 nextPos = Vector2 (pos.x + vel.x, pos.y + vel.y); // where the object is going to be (after the next physics iteration / frame) if it doesn't collide with anything
    
    /************************ COLLIDING WITH LEFT OR RIGHT SIDE OF BOX ************************/
    if (nextPos.y < obj.verts[0].y+bounds.height && nextPos.y > obj.verts[3].y) { // determine if either (Y) points denoting the bounds of the physics object are within the (Y) bounds of the collision box
        
        // if colliding with left side of box, then adjust the physics object's velocity accordingly
        // (determine if the x velocity is positive   and   the right point denoting the (X) bounds of the physics object are within the (X) bounds of the collision box   and   the right physics object (X) vertex is only slightly passed the left vertex of the collision box (is within the margin error))
        if (vel.x > 0   &&   nextPos.x > obj.verts[0].x-bounds.width && nextPos.x < obj.verts[1].x-bounds.width   &&   nextPos.x+(bounds.width-ERROR_MARGIN) < obj.verts[0].x) {
            
            float decelCol = abs (vel.x) * STATF; // calculate deceleration due to collision
            vel.y = ApplyFrictionalDeceleration (vel.y, decelCol); // apply the deceleration due to collision to the new y-velocity
            vel.x = -(vel.x) * RESTI; // calculate x-velocity after the collision
            
        }
        // else if colliding with right side of box, then adjust the physics object's velocity accordingly
        // (determine if the x velocity is negative   and   the left point denoting the (X) bounds of the physics object are within the (X) bounds of the collision box   and   the left physics object (X) vertex is only slightly passed the right vertex of the collision box (is within the margin error))
        else if (vel.x < 0   &&   nextPos.x < obj.verts[1].x && nextPos.x > obj.verts[0].x   &&   nextPos.x+ERROR_MARGIN > obj.verts[1].x) {
            
            float decelCol = abs (vel.x) * STATF; // calculate deceleration due to collision
            vel.y = ApplyFrictionalDeceleration (vel.y, decelCol); // apply the deceleration due to collision to the new y-velocity
            vel.x = -(vel.x) * RESTI; // calculate x-velocity after the collision
            
        }
        
        /************************ KINETIC FRICTION ************************/
        // if the physics object is touching right side of the box, then apply kinetic friction to it
        // (determine if the physics objects left vertex (X)  approximately equals  the right vertex (X) of the collision box)
        if ((int)(nextPos.x+.5) == (int)(obj.verts[1].x+.5)) {
         
         // calculate and apply kinetic friction to physics object
         float decelKin = abs (vel.x) * KINF;
         vel.y = ApplyFrictionalDeceleration (vel.y, decelKin);
         
         }
         // else if the physics object is touching left side of the box, then apply kinetic friction to it
         // (determine if the physics objects right vertex (X)  approximately equals  the left vertex (X) of the collision box)
         else if ((int)(nextPos.x + .5) == (int)((obj.verts[0].x - bounds.width) + .5)) {
         
         // calculate and apply kinetic friction to physics object
         float decelKin = abs (vel.x) * KINF;
         vel.y = ApplyFrictionalDeceleration (vel.y, decelKin);
         
        }
        
    }
    
    /************************ COLLIDING WITH TOP OR BOTTOM SIDE OF BOX ************************/
    if (nextPos.x > obj.verts[0].x-bounds.width && nextPos.x < obj.verts[1].x) { // determine if either (X) points denoting the bounds of the physics object are within the (X) bounds of the collision box
        
        // if colliding with top side of box, then adjust the physics object's velocity accordingly
        if (vel.y < 0   &&   nextPos.y < obj.verts[0].y+bounds.height && nextPos.y > obj.verts[3].y+bounds.height) {
            
            float decelCol = abs (vel.y) * STATF; // calculate deceleration due to collision
            vel.x = ApplyFrictionalDeceleration (vel.x, decelCol); // apply the deceleration due to collision to the new y-velocity
            vel.y = -(vel.y) * RESTI; // calculate x-velocity after the collision
            
        }
        // else if colliding with bottom side of box, then adjust the physics object's velocity accordingly
        else if (vel.y > 0   &&   nextPos.y > obj.verts[3].y && nextPos.y < obj.verts[0].y) {
            
            float decelCol = abs (vel.y) * STATF; // calculate deceleration due to collision
            vel.x = ApplyFrictionalDeceleration (vel.x, decelCol); // apply the deceleration due to collision to the new y-velocity
            vel.y = -(vel.y) * RESTI; // calculate x-velocity after the collision
            
        }
        
        /************************ KINETIC FRICTION ************************/
        // if the physics object is touching bottom side of the box, then apply kinetic friction to it
        // (determine if the physics objects top vertex (Y)  approximately equals  the bottom vertex (Y) of the collision box)
        if ((int)(nextPos.y+.5) == (int)(obj.verts[3].y+.5)) {
         
         // calculate and apply kinetic friction to physics object
         float decelKin = abs (vel.y) * KINF;
         vel.x = ApplyFrictionalDeceleration (vel.x, decelKin);
         
         }
         // else if the physics object is touching top side of the box, then apply kinetic friction to it
         // (determine if the physics objects bottom vertex (Y)  approximately equals  the top vertex (Y) of the collision box)
         else if ((int)(nextPos.y + .5) == (int)((obj.verts[0].y + bounds.height) + .5)) {
         
         // calculate and apply kinetic friction to physics object
         float decelKin = abs (vel.y) * KINF;
         vel.x = ApplyFrictionalDeceleration (vel.x, decelKin);
         
        }
        
    }
    
}



// detects if the physics object is colliding with inputted collision edge and realistically changes its velocity if it is
void PhysicsObject::CollideEdge (Edge obj) {
    Vector2 nextPos = Vector2 (pos.x + vel.x, pos.y + vel.y); // where the object is going to be (after the next physics iteration / frame) if it doesn't collide with anything
    // (note these vars are purely for game mechanic purposes)
    // should probably define these at somepoint in time in the constructor in the future
    float rebound0 = 0.75; // default restitution/rebound value   0.75
    float rebound1 = 1.5; // rebound value for pushing off with hands   1.65
    float rebound2 = 0.5; // rebound value for dampened collisions   .5
    float statf0 = .2; // default value for 'static' friction   .2
    float statf1 = -.50; // -.50
    float statf2 = .3;  // .33
    
    switch (obj.dir) {
        case (1):   // case where edge normal vector is pointing leftward   (object is traveling rightwards)
            // if right side of physics object is colliding with the edge, then adjust the physics object's velocity accordingly
            if (vel.x > 0   &&   nextPos.y < obj.verts[0].y+bounds.height && nextPos.y > obj.verts[1].y) { // determine if the object is traveling rightward and if either (Y) points denoting the bounds of the physics object are within the (Y) bounds of the collision edge
                
                // (determine if the right point denoting the (X) bounds of the physics object are within the (X) bounds of the collision edge)
                if (nextPos.x > obj.verts[0].x-bounds.width && nextPos.x < obj.verts[1].x+EDGE_TOL-bounds.width) {
                    
                    // (note that this chunk of code is purely for game mechanics and should be removed for future uses)
                    if (anyInps || wfs.heldByPlayer) { // if players arms are out / if coefficient of restitution should be modified
                        if (playerObj.dir == 3) { // player is facing rightwards / is facing towards the edge
                            RESTI = rebound1;
                            STATF = statf1;
                        } else if (playerObj.dir == 1) { // player is facing backwards
                            RESTI = rebound2;
                            STATF = statf2; // statf0 ?
                        } else {
                            RESTI = rebound0;
                            STATF = statf0;
                        }
                    } else {
                        RESTI = rebound0;
                        STATF = statf0;
                    }

                    float decelCol = abs (vel.x) * STATF; // calculate deceleration due to collision
                    vel.y = ApplyFrictionalDeceleration (vel.y, decelCol); // apply the deceleration due to collision to the new y-velocity
                    vel.x = -(vel.x) * RESTI; // calculate x-velocity after the collision
                    
                }
                
                // if the physics object is touching left side of the edge, then apply kinetic friction to it
                // (determine if the physics objects right vertex (X)  approximately equals  the left vertex (X) of the collision edge)
                if ((int)(nextPos.x + .5) == (int)((obj.verts[0].x - bounds.width) + .5)) {
                 // calculate and apply kinetic friction to physics object
                 float decelKin = abs (vel.x) * KINF;
                 vel.y = ApplyFrictionalDeceleration (vel.y, decelKin);
                }
                
            }
            break;
        case (3):   // case where edge normal vector is pointing rightward   (object is traveling leftwards)
            // if left side of physics object is colliding with the edge, then adjust the physics object's velocity accordingly
            if (vel.x < 0   &&   nextPos.y < obj.verts[0].y+bounds.height && nextPos.y > obj.verts[1].y) { // determine if the object is traveling leftward and if either (Y) points denoting the bounds of the physics object are within the (Y) bounds of the collision edge
                
                // (determine if the left point denoting the (X) bounds of the physics object are within the (X) bounds of the collision edge)
                if (nextPos.x < obj.verts[1].x && nextPos.x > obj.verts[0].x-EDGE_TOL) {
                    
                    // (note that this chunk of code is purely for game mechanics and should be removed for future uses)
                    if (anyInps || wfs.heldByPlayer) { // if players arms are out / if coefficient of restitution should be modified
                        if (playerObj.dir == 1) { // player is facing leftward / is facing towards the edge
                            RESTI = rebound1;
                            STATF = statf1;
                        } else if (playerObj.dir == 3) { // player is facing backwards
                            RESTI = rebound2;
                            STATF = statf2;
                        } else {
                            RESTI = rebound0;
                            STATF = statf0;
                        }
                    } else {
                        RESTI = rebound0;
                        STATF = statf0;
                    }
                    
                    float decelCol = abs (vel.x) * STATF; // calculate deceleration due to collision
                    vel.y = ApplyFrictionalDeceleration (vel.y, decelCol); // apply the deceleration due to collision to the new y-velocity
                    vel.x = -(vel.x) * RESTI; // calculate x-velocity after the collision
                }
                
                // if the physics object is touching right side of the edge, then apply kinetic friction to it
                // (determine if the physics objects left vertex (X)  approximately equals  the right vertex (X) of the collision edge)
                if ((int)(nextPos.x+.5) == (int)(obj.verts[1].x+.5)) {
                 // calculate and apply kinetic friction to physics object
                 float decelKin = abs (vel.x) * KINF;
                 vel.y = ApplyFrictionalDeceleration (vel.y, decelKin);
                }
                
            }
            break;
        case (2):   // case where edge normal vector is pointing upward   (object is traveling downwards)
            // if colliding with top side of edge, then adjust the physics object's velocity accordingly
            if (vel.y < 0   &&   nextPos.x > obj.verts[0].x-bounds.width && nextPos.x < obj.verts[1].x) {
                
                if (nextPos.y < obj.verts[0].y+bounds.height && nextPos.y > obj.verts[1].y+bounds.height-EDGE_TOL) {
                    
                    // (note that this chunk of code is purely for game mechanics and should be removed for future uses)
                    if (anyInps || wfs.heldByPlayer) { // if players arms are out / if coefficient of restitution should be modified
                        if (playerObj.dir == 0) { // player is facing downward / is facing towards the edge
                            RESTI = rebound1;
                            STATF = statf1;
                        } else if (playerObj.dir == 2) { // player is facing backwards
                            RESTI = rebound2;
                            STATF = statf2;
                        } else {
                            RESTI = rebound0;
                            STATF = statf0;
                        }
                    } else {
                        RESTI = rebound0;
                        STATF = statf0;
                    }
                    
                    float decelCol = abs (vel.y) * STATF; // calculate deceleration due to collision
                    vel.x = ApplyFrictionalDeceleration (vel.x, decelCol); // apply the deceleration due to collision to the new y-velocity
                    vel.y = -(vel.y) * RESTI; // calculate x-velocity after the collision
                }
                
                // if the physics object is touching top side of the edge, then apply kinetic friction to it
                // (determine if the physics objects bottom vertex (Y)  approximately equals  the top vertex (Y) of the collision edge)
                if ((int)(nextPos.y + .5) == (int)((obj.verts[0].y + bounds.height) + .5)) {
                 // calculate and apply kinetic friction to physics object
                 float decelKin = abs (vel.y) * KINF;
                 vel.x = ApplyFrictionalDeceleration (vel.x, decelKin);
                }
                
            }
            break;
        case (0):   // case where edge normal vector is pointing downward   (object is traveling upwards)
            // if colliding with bottom side of edge, then adjust the physics object's velocity accordingly
            if (vel.y > 0   &&   nextPos.x > obj.verts[0].x-bounds.width && nextPos.x < obj.verts[1].x) {
                
                if (nextPos.y > obj.verts[1].y && nextPos.y < obj.verts[0].y+EDGE_TOL) {
                    
                    // (note that this chunk of code is purely for game mechanics and should be removed for future uses)
                    if (anyInps || wfs.heldByPlayer) { // if players arms are out / if coefficient of restitution should be modified
                        if (playerObj.dir == 2) { // player is facing upward / is facing towards the edge
                            RESTI = rebound1;
                            STATF = statf1;
                        } else if (playerObj.dir == 0) { // player is facing backwards
                            RESTI = rebound2;
                            STATF = statf2;
                        } else {
                            RESTI = rebound0;
                            STATF = statf0;
                        }
                    } else {
                        RESTI = rebound0;
                        STATF = statf0;
                    }
                    
                    float decelCol = abs (vel.y) * STATF; // calculate deceleration due to collision
                    vel.x = ApplyFrictionalDeceleration (vel.x, decelCol); // apply the deceleration due to collision to the new y-velocity
                    vel.y = -(vel.y) * RESTI; // calculate x-velocity after the collision
                }
                
                // if the physics object is touching bottom side of the edge, then apply kinetic friction to it
                // (determine if the physics objects top vertex (Y)  approximately equals  the bottom vertex (Y) of the collision edge)
                if ((int)(nextPos.y+.5) == (int)(obj.verts[1].y+.5)) {
                 // calculate and apply kinetic friction to physics object
                 float decelKin = abs (vel.y) * KINF;
                 vel.x = ApplyFrictionalDeceleration (vel.x, decelKin);
                }
                
            }
            break;
    }
}



// applies given frictional deceleration to the given velocity and returns the new velocity (how you apply the frictional deceleration depends on the direction the velocity, so a function like this (with a bunch of if-statements) is necessary)
float PhysicsObject::ApplyFrictionalDeceleration (float velocity, float deceleration) {
    if (velocity > 0) {
        velocity = velocity - deceleration;
        if (velocity < 0) {
            velocity = 0;
        }
    } else if (velocity < 0) {
        velocity = velocity + deceleration;
        if (velocity > 0) {
            velocity = 0;
        }
    }
    return velocity;
}

Vector2 PhysicsObject::CalcAccel (Vector2 objPos, Vector2 objVel) { // calculate the acceleration due to a collision with the given object (currently only being used for bullets)
    float factor = 0.5;
    Vector2 acceleration = Vector2 (objVel.x * factor, objVel.y * factor);
    
    return acceleration;
}





Raycast::Raycast (Vector2 origin, Vector2 direction, float maxDistance, char type) {
    stepSize = 1;
    hit = false;
    colliderType = 'X';
    point = Vector2 ();
    distance = 0;
    float currentDistance = 0; // current distance of the particle
    
    pos = Vector2 (origin.x, origin.y); // deep copy the origin
    // convert direction into a unit vector * stepSize
    dir = direction.getUnitVector ();
    dir = Vector2 (dir.x * stepSize, dir.y * stepSize);
    
    // cout << "Particle bnds x = " << playerObj.bounds.verts [0].x << ", y = " << playerObj.bounds.verts [0].y;
    playerObj.bounds.UpdatePosition (playerObj.pos); // update the bounds of the player    // may already be done else where
    // cout << "; new bnds x = " << playerObj.bounds.verts [0].x << ", y = " << playerObj.bounds.verts [0].y << endl;
    
    // do stuff
    while (hit == false && currentDistance < maxDistance) {
        currentDistance = Distance (origin, pos);
        // check for collision with player
        if (type != 'a' && CheckBoxCollision (playerObj.bounds)) {
            hit = true; // note that there was a collision
            point = pos;
            distance = Distance (origin, point); // or currentDistance
            colliderType = 'p'; // collider type equals player
            // cout << "Player hit!" << endl;
        } else if (type != 'a' && CheckBoxCollision (wfs.bounds)) {
            hit = true; // note that there was a collision
            point = pos;
            distance = Distance (origin, point); // or currentDistance
            colliderType = 'w'; // collider type equals wfs
        } else {
            
            // check for collision with tunnel edges
            for (int j = 0; j < numTunnelEdges; j++) { // CollideEdge (tunnelEdges [j]);
                if (CheckEdgeCollision (tunnelEdges [j])) { // if there was a collision then
                    hit = true; // note that there was a collision
                    point = pos;
                    distance = Distance (origin, point); // or currentDistance
                    colliderType = 'e'; // collider type equals edge
                    break; // may not actually be breaking fully
                }
            }
            
            /*
            // check for closest collision with tunnel edges
            for (int j = 0; j < numTunnelEdges; j++) { // CollideEdge (tunnelEdges [j]);
                if (CheckEdgeCollision (tunnelEdges [j])) { // if there was a collision then
                    if (hit == false || distance > Distance (origin, point)) { // could be made more efficient by not check the dist multiple times...
                        point = pos;
                        distance = Distance (origin, point); // or currentDistance
                        colliderType = 'e'; // collider type equals edge
                    }
                    // break; // may not actually be breaking fully
                    hit = true; // note that there was a collision
                }
            }
            */
        }
        // increment the particle
        pos = Vector2 (pos.x + dir.x, pos.y + dir.y);
        // point = pos;
        // cout << "Particle x = " << pos.x << ", y = " << pos.y << endl;
        // LCD.SetFontColor (RED);
        // LCD.FillCircle (pos.x, -pos.y, 5);
    }
    if (hit == false && currentDistance >= maxDistance) { // if the loop exited due to the distance limit being exceeded
        point = pos;
    }
}


// checks if the bullet has collided with a box collision object
bool Raycast::CheckBoxCollision (Box obj) {
    // convert the box into collision edges
    Edge southEdge = Edge (obj.verts[2], obj.verts[3], 0);
    Edge westEdge = Edge (obj.verts[0], obj.verts[2], 1);
    Edge northEdge = Edge (obj.verts[0], obj.verts[1], 2);
    Edge eastEdge = Edge (obj.verts[1], obj.verts[3], 3);
    
    /*
    // perform the particle edge collision function on each edge of the box
    bool sCol = CheckEdgeCollision (southEdge);
    bool wCol = CheckEdgeCollision (westEdge);
    bool nCol = CheckEdgeCollision (northEdge);
    bool eCol = CheckEdgeCollision (eastEdge);
    
    if (sCol || wCol || nCol || eCol) {
        return true;
    } else {
        return false;
    }
    */
    
    if (CheckEdgeCollision (southEdge) || CheckEdgeCollision (westEdge) || CheckEdgeCollision (northEdge) || CheckEdgeCollision (eastEdge)) {
        return true;
    } else {
        return false;
    }
}


bool Raycast::CheckEdgeCollision (Edge obj) {
    Vector2 nextPos = pos; // set nextPos to (secretely) equal the particle's current position
    Vector2 vel = dir;
    bool collided = false;
    
    float EDGE_TOL = 5; // initialize edge tolerance, which is essentially a pseudo width for collision edges
    
    switch (obj.dir) {
        case (1):   // case where edge normal vector is pointing leftward   (object is traveling rightwards)
            // if right side of physics object is colliding with the edge, then adjust the physics object's velocity accordingly
            if (vel.x > 0   &&   nextPos.y < obj.verts[0].y && nextPos.y > obj.verts[1].y) { // determine if the object is traveling rightward and if either (Y) points denoting the bounds of the physics object are within the (Y) bounds of the collision edge
                
                // (determine if the right point denoting the (X) bounds of the physics object are within the (X) bounds of the collision edge)
                if (nextPos.x > obj.verts[0].x && nextPos.x < obj.verts[1].x+EDGE_TOL) {
                    collided = true;
                }
                
            }
            break;
        case (3):   // case where edge normal vector is pointing rightward   (object is traveling leftwards)
            // if left side of physics object is colliding with the edge, then adjust the physics object's velocity accordingly
            if (vel.x < 0   &&   nextPos.y < obj.verts[0].y && nextPos.y > obj.verts[1].y) { // determine if the object is traveling leftward and if either (Y) points denoting the bounds of the physics object are within the (Y) bounds of the collision edge
                
                // (determine if the left point denoting the (X) bounds of the physics object are within the (X) bounds of the collision edge)
                if (nextPos.x < obj.verts[1].x && nextPos.x > obj.verts[0].x-EDGE_TOL) {
                    collided = true;
                }
                
            }
            break;
        case (2):   // case where edge normal vector is pointing upward   (object is traveling downwards)
            // if colliding with top side of edge, then adjust the physics object's velocity accordingly
            if (vel.y < 0   &&   nextPos.x > obj.verts[0].x && nextPos.x < obj.verts[1].x) {
                
                if (nextPos.y < obj.verts[0].y && nextPos.y > obj.verts[1].y-EDGE_TOL) {
                    collided = true;
                }
                
            }
            break;
        case (0):   // case where edge normal vector is pointing downward   (object is traveling upwards)
            // if colliding with bottom side of edge, then adjust the physics object's velocity accordingly
            if (vel.y > 0   &&   nextPos.x > obj.verts[0].x && nextPos.x < obj.verts[1].x) {
                
                if (nextPos.y > obj.verts[1].y && nextPos.y < obj.verts[0].y+EDGE_TOL) {
                    collided = true;
                }
                
            }
            break;
    }
    
    return collided;
}



int ConvertColor (float rf, float gf, float bf) {
    int r = rf*255, g = gf*255, b = bf*255;
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}
int ConvertColor (RGB rgbColor) {
    int r = rgbColor.r*255, g = rgbColor.g*255, b = rgbColor.b*255;
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}
RGB ConvertColor (int hexValue) {
    RGB rgbColor;
    rgbColor.r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
    rgbColor.g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
    rgbColor.b = ((hexValue) & 0xFF) / 255.0;        // Extract the BB byte
    return rgbColor;
}



// adds a shadow to the inputted color and returns the hex value
int ConvertToShadow (int hexInput, int shadowColor, float shadowAlpha) {
    RGB rgbInput = ConvertColor (hexInput);
    RGB rgbShadow = ConvertColor (shadowColor);
    // RGB rgbOutput = RGB (    rgbInput.r + (rgbShadow.r-rgbInput.r) * shadowAlpha,   rgbInput.g + (rgbShadow.g-rgbInput.g) * shadowAlpha,   rgbInput.b + (rgbShadow.b-rgbInput.b) * shadowAlpha    );
    RGB rgbOutput;
    rgbOutput.r = rgbInput.r + (rgbShadow.r-rgbInput.r) * shadowAlpha;
    rgbOutput.g = rgbInput.g + (rgbShadow.g-rgbInput.g) * shadowAlpha;
    rgbOutput.b = rgbInput.b + (rgbShadow.b-rgbInput.b) * shadowAlpha;
    int hexOutput = ConvertColor (rgbOutput);
    return hexOutput;
}
