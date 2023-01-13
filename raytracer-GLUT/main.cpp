#undef max
#undef min
#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "Color.h"
#include "Object.h"
#include "Vector.h"

using namespace std;


const float WIDTH = 26.0;
const float HEIGHT = 20.0;
const float DEPTH = 20.0;
const int PPU = 60;   
const int MAX_STEPS = 2;
const float XMIN = -WIDTH * 0.5;
const float XMAX = WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX = HEIGHT * 0.5;
const float GLOBAL_MEDIUM = 1.0;
vector<Object*> sceneObjects;

struct Light
{
	Vector l;
	Color diffused = Color::WHITE;
	Color ambient = Color(0.1f, 0.1f, 0.1f);
	Color specular = Color(0.05f, 0.05f, 0.05f);
};
Light light;
Color backgroundCol;

struct PointBundle
{
	Vector point;
	int index;
	float dist;
};

/*
* This function compares the given ray with all objects in the scene
* and computes the closest point  of intersection.
*/
PointBundle closestPt(Vector pos, Vector dir)
{
	Vector  point(0, 0, 0);
	float min = FLT_MAX;

	PointBundle out = { point, -1, 0.0 };

	for (int i = 0; i < sceneObjects.size(); i++)
	{
		float t = sceneObjects[i]->intersect(pos, dir);
		if (t > 0)        //Intersects the object
		{
			point = pos + dir * t;
			if (t < min)
			{
				out.point = point;
				out.index = i;
				out.dist = t;
				min = t;
			}
		}
	}

	return out;
}

Color trace(Vector pos, Vector dir, int step)
{
	Color colorSum;
	PointBundle q = closestPt(pos, dir);
	if (q.index == -1) return backgroundCol;

	Vector n = sceneObjects[q.index]->normal(q.point);  //normal vector

	Color col = sceneObjects[q.index]->getColor();
	Color ambient = sceneObjects[q.index]->getAmbient();
	Color specular = sceneObjects[q.index]->getSpecular();
	Color pc = ambient.combineColor(Color(0.5f, 0.5f, 0.5f));

	Vector l = light.l - q.point; //The light source vector in opposite dir

	float lightDist = l.length(); //Distance to light

	l.normalise(); //Normalise this vector, and compute the dot product l•n

	float lDotn = max(0.0f, min(l.dot(n), 1.0f));

	PointBundle s = closestPt(q.point, l);

	if ((s.index > -1 && s.dist < lightDist) || lDotn <= 0) {
		colorSum = pc + 0.5f * light.ambient.combineColor(col);
	}
	else {
		float att = 1000.0f / (1 + 2 * lightDist + 1 * lightDist * lightDist);
		att *= lDotn;
		Color AI = ambient.combineColor(light.ambient);
		Color DI = col.combineColor(light.diffused);
		Color SI = specular.combineColor(light.specular);

		colorSum = (att * (AI + DI + SI)) + pc;
	}

	if (step >= MAX_STEPS) {
		return colorSum;
	}
	float shininess = sceneObjects[q.index]->getShininess();
	if (shininess != 0.0f)
	{
		Vector rdir = dir - n * (2 * dir.dot(n));
		Color rColor = trace(q.point, rdir, step+1);
		rColor.scaleColor(shininess);
		colorSum = colorSum + rColor;
	}
	float translucency = sceneObjects[q.index]->getTranslucent();
	if (translucency != 0.0f)
	{
		float n1 = sceneObjects[q.index]->getN();
		float n2 = 1.0f;
		float n1overn2 = n1 / n2;
		float cosTheta = 1 - pow(n1overn2, 2) * (1 - pow(dir.dot(n), 2));
		Vector ndir = Vector(-dir.x, -dir.y, -dir.z);
		Vector refracted = (n1overn2 * ndir.dot(n) - sqrtf(cosTheta)) * n - n1overn2 * ndir;
		Color rColor = trace(q.point, refracted, step+1);
		rColor.scaleColor(translucency);
		colorSum = colorSum + rColor;
	}

	return colorSum;
}

void display() {
	int widthInPixels = (int)(WIDTH * PPU);
	int heightInPixels = (int)(HEIGHT * PPU);
	float pixelSize = 1.0 / PPU;
	float halfPixelSize = pixelSize / 2.0;
	float x1, y1, xc, yc;
	Vector eye(0., 0., 5.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);

	for (int i = 0; i < heightInPixels; i++)	//Scan every "pixel"
	{
		y1 = YMIN + i * pixelSize;
		yc = y1 + halfPixelSize;
		for (int j = 0; j < widthInPixels; j++)
		{
			x1 = XMIN + j * pixelSize;
			xc = x1 + halfPixelSize;

			Vector dir(xc, yc, -DEPTH);	//direction of the primary ray

			dir.normalise();			//Normalise this direction

			Color col = trace(eye, dir, 1); //Trace the primary ray and get the colour value
			glColor3f(col.r, col.g, col.b);
			glVertex2f(x1, y1);				//Draw each pixel with its color value
		}
	}

	glEnd();
	glFlush();
}

void cubiod()
{
	Plane* front = new Plane(Vector(-25, -7, -40), Vector(-17, -7, -40), Vector(-17., 0, -40), Vector(-25, 0, -40), Color::YELLOW, 0.5f, 0.0f);
	Plane* down = new Plane(Vector(-25, -7, -40), Vector(-17, -7, -40), Vector(-17., -7, -48), Vector(-25, -7, -48), Color::YELLOW, 0.5f, 0.0f);
	Plane* right = new Plane(Vector(-17, -7, -40), Vector(-17., -7, -48), Vector(-17., 0, -48), Vector(-17., 0, -40), Color::YELLOW, 0.5f, 0.0f);
	Plane* up = new Plane(Vector(-17., 0, -40), Vector(-25, 0, -40), Vector(-25., 0, -48), Vector(-17, 0, -48), Color::YELLOW, 0.5f, 0.0f);
	Plane* left = new Plane(Vector(-25, -7, -40), Vector(-25, -7, -48), Vector(-25, 0, -48), Vector(-25, 0, -40), Color::YELLOW, 0.5f, 0.0f);
	Plane* back = new Plane(Vector(-25, -7, -48), Vector(-17., -7, -48), Vector(-17., 0, -48), Vector(-25, 0, -40), Color::YELLOW, 0.5f, 0.0f);
	sceneObjects.push_back(front);
	sceneObjects.push_back(down);
	sceneObjects.push_back(right);
	sceneObjects.push_back(up);
	sceneObjects.push_back(left);
	sceneObjects.push_back(back);
}

void initialize()
{
	//Iniitialize background colour and light's position
	backgroundCol = Color::GRAY;
	light.l = Vector(10.0f, 10.0f, 5.0f);

	Sphere* sphere1 = new Sphere(Vector(10, 0.0f, -40), 4.5, Color::CYAN, 0.0f, 0.5f);
	sceneObjects.push_back(sphere1);

	Sphere* sphere2 = new Sphere(Vector(-6, 12, -50.0f), 10.0, Color::MAGENTA, 0.5f, 0.4f);
	sceneObjects.push_back(sphere2);

	Sphere* sphere4 = new Sphere(Vector(-10, -5, -40.0f), 4.5, Color::WHITE, 0.5f, 0.3f);
	sceneObjects.push_back(sphere4);


	Plane* plane = new Plane(Vector(-1000, -12, 5000), Vector(1000, -12, 5000),
		Vector(1000., -12, -5000), Vector(-1000., -12, -5000), Color::WHITE, 1.0f, 0.0f);
	sceneObjects.push_back(plane);

	Cylinder* cylinder = new Cylinder(Vector(0, -10, -30), 3.0f, 8.0f, Color::MAGENTA, 0.5f, 0.8f);
	sceneObjects.push_back(cylinder);

	Cone* cone = new Cone(Vector(15, -10.0f, -30), 3.0f, 5.0f, Color::RED, 0.5f, 0.0f);
	sceneObjects.push_back(cone);

	cubiod();
	
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0, 0, 0, 1);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("Raytracing");

	initialize();
	glutDisplayFunc(display);

	glutMainLoop();
	return 0;
}