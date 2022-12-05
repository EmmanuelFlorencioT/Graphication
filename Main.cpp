#define _USE_MATH_DEFINES

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include "GL\glew.h"
#include "GL\glut.h" //o glut

#include <cstdlib>
#include "myParser.h"

using namespace std;

float xform[4][4]; //Matrix of Transformation
float moveOrigin[4][4]; //Translation to (0,0,0)
float moveBack[4][4]; //Translation to the original place
float moveBezier[4][4];//Translation to the bezier curve coord
float rotX[4][4], rotY[4][4], rotZ[4][4];

float ilumVector[3];
float red, green, blue;
float ilumination, ambientIlum = 1.0;

float moveMult = 0.0;

float degX_D, degY_D, degZ_D, t_D; //Increments for Driver
float degX_B, degY_B, degZ_B, t_B; //Increments for Ball

myParser::Vertex p1D, p2D, p3D, p4D; //Driver Bezier points
myParser::Vertex p1B, p2B, p3B, p4B; //Ball Bezier points

/*---Transformation Methods---*/

void changeRotX(float degX) {
	float radX = degX * M_PI / 180;
	rotX[0][0] = 1;
	rotX[0][1] = 0;
	rotX[0][2] = 0;
	rotX[0][3] = 0;
	rotX[1][0] = 0;
	rotX[1][1] = cos(radX);
	rotX[1][2] = -sin(radX);
	rotX[1][3] = 0;
	rotX[2][0] = 0;
	rotX[2][1] = sin(radX);
	rotX[2][2] = cos(radX);
	rotX[2][3] = 0;
	rotX[3][0] = 0;
	rotX[3][1] = 0;
	rotX[3][2] = 0;
	rotX[3][3] = 1;
}

void changeRotY(float degY) {
	float radY = degY * M_PI / 180;
	rotY[0][0] = cos(radY);
	rotY[0][1] = 0;
	rotY[0][2] = sin(radY);
	rotY[0][3] = 0;
	rotY[1][0] = 0;
	rotY[1][1] = 1;
	rotY[1][2] = 0;
	rotY[1][3] = 0;
	rotY[2][0] = -sin(radY);
	rotY[2][1] = 0;
	rotY[2][2] = cos(radY);
	rotY[2][3] = 0;
	rotY[3][0] = 0;
	rotY[3][1] = 0;
	rotY[3][2] = 0;
	rotY[3][3] = 1;
}

void changeRotZ(float degZ) {
	float radZ = degZ * M_PI / 180;
	rotZ[0][0] = cos(radZ);
	rotZ[0][1] = -sin(radZ);
	rotZ[0][2] = 0;
	rotZ[0][3] = 0;
	rotZ[1][0] = sin(radZ);
	rotZ[1][1] = cos(radZ);
	rotZ[1][2] = 0;
	rotZ[1][3] = 0;
	rotZ[2][0] = 0;
	rotZ[2][1] = 0;
	rotZ[2][2] = 1;
	rotZ[2][3] = 0;
	rotZ[3][0] = 0;
	rotZ[3][1] = 0;
	rotZ[3][2] = 0;
	rotZ[3][3] = 1;
}

//Calculates the matrix of Translation
void calcTransl(float mat[4][4], float movX, float movY, float movZ) {
	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = movX;

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = movY;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = movZ;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
}

void calcBezier(myParser::Vertex p1, myParser::Vertex p2, myParser::Vertex p3, myParser::Vertex p4, float t) {
	float newX, newY, newZ;

	newX = pow((1 - t), 3) * p1.x + 3 * pow((1 - t), 2) * t * p2.x + 3 * (1 - t) * pow(t, 2) * p3.x + pow(t, 3) * p4.x;
	newY = pow((1 - t), 3) * p1.y + 3 * pow((1 - t), 2) * t * p2.y + 3 * (1 - t) * pow(t, 2) * p3.y + pow(t, 3) * p4.y;
	newZ = pow((1 - t), 3) * p1.z + 3 * pow((1 - t), 2) * t * p2.z + 3 * (1 - t) * pow(t, 2) * p3.z + pow(t, 3) * p4.z;

calcTransl(moveBezier, newX, newY, newZ);
}

//Performs the matrix multiplication and stores the value in matRes parameter
void multMat(float A[4][4], float B[4][4], float matRes[4][4]) {
	matRes[0][0] = A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0] + A[0][3] * B[3][0];
	matRes[0][1] = A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1] + A[0][3] * B[3][1];
	matRes[0][2] = A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2] + A[0][3] * B[3][2];
	matRes[0][3] = A[0][0] * B[0][3] + A[0][1] * B[1][3] + A[0][2] * B[2][3] + A[0][3] * B[3][3];

	matRes[1][0] = A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0] + A[1][3] * B[3][0];
	matRes[1][1] = A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1] + A[1][3] * B[3][1];
	matRes[1][2] = A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2] + A[1][3] * B[3][2];
	matRes[1][3] = A[1][0] * B[0][3] + A[1][1] * B[1][3] + A[1][2] * B[2][3] + A[1][3] * B[3][3];

	matRes[2][0] = A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0] + A[2][3] * B[3][0];
	matRes[2][1] = A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1] + A[2][3] * B[3][1];
	matRes[2][2] = A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2] + A[2][3] * B[3][2];
	matRes[2][3] = A[2][0] * B[0][3] + A[2][1] * B[1][3] + A[2][2] * B[2][3] + A[2][3] * B[3][3];

	matRes[3][0] = A[3][0] * B[0][0] + A[3][1] * B[1][0] + A[3][2] * B[2][0] + A[3][3] * B[3][0];
	matRes[3][1] = A[3][0] * B[0][1] + A[3][1] * B[1][1] + A[3][2] * B[2][1] + A[3][3] * B[3][1];
	matRes[3][2] = A[3][0] * B[0][2] + A[3][1] * B[1][2] + A[3][2] * B[2][2] + A[3][3] * B[3][2];
	matRes[3][3] = A[3][0] * B[0][3] + A[3][1] * B[1][3] + A[3][2] * B[2][3] + A[3][3] * B[3][3];
}

void calcXform_Rot(float degX, float degY, float degZ) {
	float aux[4][4];

	changeRotX(degX);
	changeRotY(degY);
	changeRotZ(degZ);

	multMat(moveBack, rotZ, aux);
	multMat(aux, rotY, xform);
	multMat(xform, rotX, aux);
	multMat(aux, moveOrigin, xform);
}

void calcXform_RotTrans(float degX, float degY, float degZ) {
	float aux[4][4];

	changeRotX(degX);
	changeRotY(degY);
	changeRotZ(degZ);

	multMat(moveBezier, rotZ, aux);
	multMat(aux, rotY, xform);
	multMat(xform, rotX, aux);
	multMat(aux, moveOrigin, xform);
}

//Rotate in X, Y and Z and returns the point back to its "original" pivot point.
void myTransform(myParser::Vertex v, float res[4]) {
	float ver[4];

	ver[0] = v.x;
	ver[1] = v.y;
	ver[2] = v.z;
	ver[3] = 1;

	res[0] = xform[0][0] * ver[0] + xform[0][1] * ver[1] + xform[0][2] * ver[2] + xform[0][3] * ver[3];
	res[1] = xform[1][0] * ver[0] + xform[1][1] * ver[1] + xform[1][2] * ver[2] + xform[1][3] * ver[3];
	res[2] = xform[2][0] * ver[0] + xform[2][1] * ver[1] + xform[2][2] * ver[2] + xform[2][3] * ver[3];
	res[3] = xform[3][0] * ver[0] + xform[3][1] * ver[1] + xform[3][2] * ver[2] + xform[3][3] * ver[3];
}

void calcFaceNormal(myParser::Vertex a, myParser::Vertex b, myParser::Vertex c, float N[3]) {
	float aT[4], bT[4], cT[4]; //Points after transformation
	float _i, _j, _k, length; //Cross product components

	myTransform(a, aT);
	myTransform(b, bT);
	myTransform(c, cT);

	//Cross Product: AB x AC
	_i = (bT[1] - aT[1]) * (cT[2] - aT[2]) - (bT[2] - aT[2]) * (cT[1] - aT[1]);
	_j = (bT[0] - aT[0]) * (cT[2] - aT[2]) - (bT[2] - aT[2]) * (cT[0] - aT[0]);
	_k = (bT[0] - aT[0]) * (cT[1] - aT[1]) - (bT[1] - aT[1]) * (cT[0] - aT[0]);

	length = sqrt(pow(_i, 2) + pow(_j, 2) + pow(_k, 2));

	//Save the unit vector components
	N[0] = (-1.0) * _i / length;
	N[1] = (-1.0) * _j / length;
	N[2] = (-1.0) * _k / length;
}

float calcDiffIlum(float norm[3]) {
	float dotProd;

	dotProd = norm[0] * ilumVector[0] + norm[1] * ilumVector[1] + norm[2] * ilumVector[2];

	if (dotProd > 0) //Normal and Ilum have the same orientation
		return 0;

	return -dotProd;
}

void keyboardEvent(unsigned char key, int x, int y){
	switch (key) {
		case 'a':
			moveMult = 0.5;
			break;
		case 's':
			moveMult = 1.0;
			break;
		case 'q':
			moveMult = 0.0;
			break;
		case 'o':
			if (ambientIlum < 1)
				ambientIlum += 0.1;
			break;
		case 'l':
			if (ambientIlum > 0)
				ambientIlum -= 0.1;
			break;
		case 'r':
			degX_D = 55.0; degY_D = 4.04; degZ_D = -24.1; t_D = 0.0;
			degX_B = degY_B = degZ_B = 0; t_B = 0;
	}
	glutPostRedisplay();
}

/*----------------------------*/

void display(void)
{
	/*  clear all pixels  */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*  draw white polygon (rectangle) with corners at
	 *  (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)
	 */

	//glRotatef(degrees, 1, 1, 1); // No se puede utilizar para la materia

	//glColor3f(1.0, 1.0, 1.0); Color por default es Blanco

	vector<myParser::Object> objects;

	myParser::executeParsing("Golf.obj", objects);

	glBegin(GL_TRIANGLES); //GL_TRIANGLES para esperar múltiplos de 3 en Vertices
	//GL_QUADS espera entonces múltiplos de 4 en vértices

	float points[4], normal[3];
	float diffIlum; //Diffuse Ilumination

	//Cycle through objects
	for (int i = 0; i < objects.size(); i++) {
		if (i == 0) { //Rotate Golf Driver
			calcTransl(moveOrigin, -1.9315, -2.7771, -4.4132);
			calcTransl(moveBack, 1.9315, 2.7771, 4.4132);
			calcXform_Rot(degX_D, degY_D, degZ_D);
			red = 0.0f; green = 0.0f; blue = 1.0f;
		}
		if (i == 1) { //Move Golf Ball
			calcTransl(moveOrigin, -1.9315, -0.44874, -0.29689);
			calcBezier(p1B, p2B, p3B, p4B, t_B);
			calcXform_RotTrans(degX_B, degY_B, degZ_B);
			red = 1.0f; green = 0.0f; blue = 0.0f;
		}
		//Cycle through faces of the i-th object
		for (int j = 0; j < objects[i].faces.size(); j++) {
			//Take first 3 vertices to calculate face normal
			calcFaceNormal(objects[i].faces[j].ver[0], objects[i].faces[j].ver[1], objects[i].faces[j].ver[2], normal);
			diffIlum = calcDiffIlum(normal);
			ilumination = diffIlum * ambientIlum;
			glColor3f(red * ilumination, green * ilumination, blue * ilumination);
			//Cycle through vertices in the j-th face
			for (int k = 0; k < objects[i].faces[j].nVer; k++) {
				myTransform(objects[i].faces[j].ver[k], points);
				glVertex3f(points[0], points[1], points[2]);

			}
		}
	}

	//glVertex3f(x, y, z);
	glEnd();

	glBegin(GL_QUADS);
	red = 0.1f; green = 1.0f; blue = 0.3f;
	glColor3f(red * ambientIlum, green * ambientIlum, blue * ambientIlum);
	glVertex3f(40.0, 10.0, -1.0);
	glVertex3f(40.0, -80.0, -1.0);
	glVertex3f(-10.0, -80.0, -1.0);
	glVertex3f(-10.0, 10.0, -1.0);
	glEnd();


	/*Cada vez que entra a la función display, se debe de dibujar un cambio nada más,
	ya que la función display se manda a llamar varias veces.
	Quedaría:
		degrees += offset; */
	
	if (degX_D < -5) {
		degX_B += 0.05; degY_B += 0.05; degZ_B += 0.05; t_B += 0.003*moveMult; //Golf Ball Increments
	}
	else {
		degX_D -= 0.85*moveMult; degY_D -= 0.015*moveMult; degZ_D += 0.2*moveMult; //Golf Driver Increments
	}
	

	glutSwapBuffers();
	glFlush();
}

void init(void)
{
	/* Only draw the visible face, the closest to the camera */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	/*  select clearing (background) color       */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	/*Colors are handled by 0 y 1
	glClearColor(RED, GREEN, BLUE, ALPHA)
	Este es el color con el cual glClear() limpia la pantalla
	*/

	/*  initialize viewing values  */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	//gluOrtho2D(0.0, 1.0, 0.0, 1.0);
	/* Esta matriz es una de proyeccion ortogonal, la cual
	 * da el efecto de que la imagen está en 2D.*/

	gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);
	//gluPerspective(FieldOfView, aspect, zNear, zFar);

	/*Sets up to modify the view matrix*/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Back View
	//gluLookAt(-25.0, 8.0, 5.0, 17.0, -17.0, 2.0, 0.0, 0.0, 1.0);

	//Front View
	gluLookAt(-15.0, -20.0, 10.0, 5.0, 0.0, 2.0, 0.0, 0.0, 1.0);
}

/*
 *  Declare initial window size, position, and display mode
 *  (single buffer and RGBA).  Open window with "hello"
 *  in its title bar.  Call initialization routines.
 *  Register callback function to display graphics.
 *  Enter main loop and process events.
 */
int main(int argc, char** argv)
{
	//Initialize Parameters
	degX_D = 55.0; degY_D = 4.04; degZ_D = -24.1; t_D = 0.0;


	degX_B = 0.0; degY_B = 0.0; degZ_B = 0.0; t_B = 0.0;
	p1B.x = 1.9315; p1B.y = 0.44874; p1B.z = 0.29689;
	p2B.x = 1.9315; p2B.y = -17.836; p2B.z = 7.8316;
	p3B.x = 1.9315; p3B.y = -29.667; p3B.z = 7.8316;
	p4B.x = 1.9315; p4B.y = -44.928; p4B.z = 0.29689;

	//Initialize ilumination unit vector
	ilumVector[0] = -11/sqrt(185); //X component
	ilumVector[1] = 0.0; //Y component
	ilumVector[2] = 8/sqrt(185); //Z component

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(150, 50);
	glutCreateWindow("Graphication");//Nombre de la ventana
	init();
	glutKeyboardFunc(keyboardEvent);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMainLoop();
	/*
	* while(no cierre la ventana){
	*	display(); Esta funcion es la encargada de dibujar()
	*	Este ciclo es lo que le da origen a los frames.
	* }
	*/

	return 0;   /* ISO C requires main to return int. */
}