/*
COSC 3P98
Assignment 1 - Q1 - Kaleidoscope
Julian Ellis Geronimo (6756597)
Due: February 17th, 2023
*/

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <freeglut.h>
#include <FreeImage.h>

// Mutable
bool drawing = false;	// inital state
float red = 1.0, green = 0.0, blue = 0.0;	// starting paint color is always red
int counter = 0;		// counter for later determining an RGB pattern
int brush_size = 8;	// initial size of the brush
int rate = 255;			// determines how many inputs are needed to make one full transition from one RGB color to another. The lower it gets, the less (but mixed) colors. 
bool frozen = false;	// toggleable boolean for freezing the selected color
int size = 800;			// put into its own variable as it will be used often throughout this program; size is used for determining window size and calculating the kaleidoscope symmetry

// Glut function for detecting mouse clicks: left click only
void mouse(int button, int state, int x, int y) {
	// left mouse button is pressed, start drawing
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		drawing = true;
	}
	// left mouse button is released, end drawing
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		drawing = false;
	}
}

// Function with my algorithm for creating an RGB (gamer like) pattern
void colorSequence(float counter) {
	// color values are ++/-- by a very small number to make color shifting much slower/gradual
	if (0 <= counter && counter <= rate) {				  // R --> RG
		green += (float)1 / rate;
	}
	else if (rate < counter && counter <= rate * 2) {	  // RG --> G
		red -= (float)1 / rate;
	}
	else if (rate * 2 < counter && counter <= rate * 3) { // G --> GB
		blue += (float)1 / rate;
	}
	else if (rate * 3 < counter && counter <= rate * 4) { // GB --> B
		green -= (float)1 / rate;
	}
	else if (rate * 4 < counter && counter <= rate * 5) { // B --> R B
		red += (float)1 / rate;
	}
	else if (rate * 5 < counter && counter <= rate * 6) { // R B --> R
		blue -= (float)1 / rate;
	}

	glColor3f(red, green, blue);	// updates the current color in used for the next point drawn
}

// Glut function for detecting mouse movements: only when left click is being held down.
void motion(int x, int y) {

	// checks if: 1) mouse input is within the given window dimensions and 2) if left-click is being pressed
	if ((x > 0 && x < size && y > 0 && y < size) && drawing == true) {
		glPointSize(brush_size);	// sets current brush size 
		glBegin(GL_POINTS);

		if (!frozen) {	// if frozen, we disable the ability to move onto the next color
			colorSequence(counter);
		}

		// method for drawing 8 symmetrical points to the original input of [x, y]
		glVertex2f(x, y);	// the original input made by the user
		glVertex2f(size - x, y);
		glVertex2f(x, size - y);
		glVertex2f(size - x, size - y);
		glVertex2f(y, x);
		glVertex2f(size - y, x);
		glVertex2f(y, size - x);
		glVertex2f(size - y, size - x);

		if (!frozen) {
			if (counter == (rate * 6) + 1) {	// the number '6' is used because there are 6 stages in one RGB wheel sequence, and each sequence is 255 loops long
				counter = 0;	// reset to 0
			}
			else {				// if frozen, we increment counter by 1 (aids in RGB shifting)
				counter++;
			}
		}

		glEnd();
		glFlush();
	}
}

// Function used to screencapture the current window/canvas and save to a PNG. Referemced a thread to understand the use and structure of these functions. 
// Reference: https://stackoverflow.com/questions/5844858/how-to-take-screenshot-in-opengl
void save_image(const char* filename, int x, int y) {

	BYTE* px = (BYTE*)malloc(x * y * 3); // allocate memory for image data <window size x> <window size y> <# of RGB values (3)>

	glReadPixels(0, 0, x, y, GL_RGB, GL_UNSIGNED_BYTE, px);	// read the pixels from the framebuffer

	int pitch = x * 3;	// can use y or size instead of x (doesn't affect it however)
	FIBITMAP* image = FreeImage_ConvertFromRawBits(px, x, y, size * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
	FreeImage_Save(FIF_PNG, image, filename, 0);	// save the image to a PNG file using the given filename at call

	// free memory
	FreeImage_Unload(image);
	free(px);
}

// Tell what the current brush size is
void show_brush_size() {
	printf("Current Brush Size = %i\n", brush_size);
}

// Decrease brush size
void decrease_size() {
	if (brush_size <= 2) {
		printf("Lowest size reached!\n");
		brush_size = 2;		// this is here if for whatever reason brush_size is set to any number that is 2 or below as soon as the program is launched
	}
	else {
		brush_size--;
	}
}

// Increase brush size;
void increase_size() {
	brush_size++;
}

// Function for freezing the color sequence of the brush, and selecting a random RGB value
void freeze_rand() {
	if (frozen == false) {
		// unlike in freeze(), this picks a random RGB color value, and when unfrozen, it returns to what it was previously
		frozen = true;
		glColor3f((float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX);	// the arguments consist of random numbers ranging from 0.0 ... 1.0
		printf("Frozen at Random Color!\n");	// gives feedback on what color the user is frozen on
	}
	else {
		frozen = false;
		printf("Unfrozen!\n");
	}
}

// Function for freezing the color sequence of the brush, and selecting the currently selected RGB value
void freeze() {
	if (frozen == false) {
		frozen = true;
		printf("Frozen at RGB(%f, %f, %f)\n", red, blue, green);	// gives feedback on what color the user is frozen on
	}
	else {
		frozen = false;
		printf("Unfrozen!\n");
	}
}

void reset_display() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glutPostRedisplay();
}

// Function for creating keyboard commands
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 0x1B:
	case 'q':
	case 'Q':
		exit(0);
		break;
	case 'r':
	case 'R':
		reset_display();
		break;
	case 'f':
	case 'F':
		freeze();
		break;
	case 'g':
	case 'G':
		freeze_rand();
		break;
	case 'z':
	case 'Z':
		decrease_size();
		break;
	case 'x':
	case 'X':
		increase_size();
		break;
	case 'c':
	case 'C':
		show_brush_size();
		break;
	case 's':
	case 'S':
		printf("Saving image as: kaleidoscope_image.png");
		save_image("kaleidoscope_image.png", size, size);
		break;
	}
}

// Description of keyboard commands
void show_keys() {
	printf("R: Reset Image\n");
	printf("F: Freeze, and use the CURRENTLY selected color. Press 'F' or 'G' to disable.\n");
	printf("G: Freeze, and use a RANDOMLY selected color. Press 'F' or 'G' to disable.\n");
	printf("Z: Decrease brush size by 1\n");
	printf("X: Increase brush size by 1\n");
	printf("C: View current brush size (default = %i, lowest = 2)\n", brush_size);
	printf("S: Save PNG image of the current window\n");
	printf("Q: Quit Program\n\n");
}

// Simply displays the Glut window with a black background
void display() {
	glClearColor(0.0, 0.0, 0.0, 1.0);	// RGBA values for black
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(size + 16, size + 16);	// tempfix: for some reason, when the glut window appears it subtracts the actual size of the window by 16px (this affects saving as PNG)
	glutCreateWindow("Kaleidoscope by Julian Ellis Geronimo");

	// glut functions
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glMatrixMode(GL_PROJECTION);	// allows us to arrange how the drawings are rendered onto the screen
	glLoadIdentity();
	gluOrtho2D(0, size, size, 0);	// set up an orthographic projection on the glut window
	glMatrixMode(GL_MODELVIEW);	// allows us to change the position or scale of objects on screen

	show_keys();		// show keyboard commands on console
	reset_display();	// tempfix: when making your first line, the line being drawn has a huge delay until the next point is drawn (try removing it and seeing the difference)

	glutMainLoop();

	return 0;
}