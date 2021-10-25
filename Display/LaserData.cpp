#include "LaserData.hpp"
#include "Camera.hpp"
#include "Vehicle.hpp"
#include "HUD.hpp"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <unistd.h>
#elif defined(WIN32)
#include <Windows.h>
#include <tchar.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#pragma warning(disable : 4996)
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <math.h>

#include <map>

#define DEGTORAD (3.141592765 / 180.0)

void LaserData::drawLines() {

	const double centerR = -90;
	const double startR = centerR - 50;
	const double endR = centerR + 50;

	double r = 4;

	double r1 = r;

	// this seems to control length?
	double r2 = r * 4;

	// these are the lines from the gauge
	glBegin(GL_LINES);
	for (double ang = startR; ang <= endR; ang += (endR - startR) * 0.125 * .5) {
		double x = cos(ang * DEGTORAD);
		double y = sin(ang * DEGTORAD);

		glVertex2f(r1 * x, r1 * y);
		glVertex2f(r2 * x, r2 * y);
	}
	glEnd();

	//// let's try a point?
	//glBegin(GL_POINTS);
	//glVertex2f(0, 0);
	//glEnd();

	// let's try other points
	glBegin(GL_POINTS);
	for (int i = 0; i <= 1000; i++) {
		glVertex3f(i*2, 1, i);
	}
	
	glEnd();
}
void LaserData::draw() {
	glPushMatrix();
	double x = 2;
	double y = 3;
	double r = 4;
	double min = 0.2;
	double max = 7;
	double val = 3;
	const char* label = "nice";
	const char* minLabel = "min";
	const char* maxLabel = "max";

	double r1 = r;
	double r2 = r * 1.05;

	const double centerR = -90;
	const double startR = centerR - 50;
	const double endR = centerR + 50;

	glTranslatef(x, y, 0);
	glDisable(GL_LIGHTING);

	y = sin((startR)*DEGTORAD);

	double valPos = ((val - min) / (max - min));
	valPos = (valPos * (endR - startR)) + startR;
	if (valPos < startR)
		valPos = startR;
	else if (valPos > endR)
		valPos = endR;
	//else
	//	valPos = (int) (valPos*2.0)/2.0;

	// draw the bar
	double barW = 20;
	r1 -= 6;
	r2 += 3;
	glColor3f(1, 0, 1);
	glBegin(GL_QUADS);
	double x1 = cos((valPos - barW) * DEGTORAD);
	double y1 = sin((valPos - barW) * DEGTORAD);
	double x2 = cos((valPos + barW) * DEGTORAD);
	double y2 = sin((valPos + barW) * DEGTORAD);
	glVertex2f(r1 * x1, r1 * y1);
	glVertex2f(r1 * x2, r1 * y2);
	glVertex2f(r2 * x2, r2 * y2);
	glVertex2f(r2 * x1, r2 * y1);
	glEnd();



	glPopMatrix();
}