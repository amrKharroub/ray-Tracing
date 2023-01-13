#pragma once
#include "Vector.h"
#include "Color.h"
#include <algorithm>
#include <math.h>
#include <cstdio>

class Object
{
protected:
	Color color;
	Color ambient = Color(0.1f, 0.1f, 0.1f);
	Color specular = Color(0.05f, 0.05f, 0.05f);
	float n;
	float shininess;
	float translucency = 0.0f;
public:
	Object() {}
	virtual float intersect(Vector pos, Vector dir) = 0;
	virtual Vector normal(Vector pos) = 0;
	virtual ~Object() {}
	Color getColor();
	Color getAmbient();
	Color getSpecular();
	float getShininess();
	float getTranslucent();
	float getN();
	void setColor(Color col);
};


class Sphere : public Object
{

private:
	Vector center;
	float radius;
	float n = 1.5f;

public:
	Sphere()
		: center(Vector()), radius(1)  //Default constructor creates a unit sphere
	{
		color = Color::WHITE;
		shininess = 0.0f;
		translucency = 0.0f;
	};

	Sphere(Vector c, float r, Color col, float shin, float tran)
		: center(c), radius(r)
	{
		color = col;
		shininess = shin;
		translucency = tran;
	};


	float intersect(Vector pos, Vector dir);

	Vector normal(Vector p);

};


class Cone : public Object
{

private:
	Vector center;
	float radius;
	float height;
	float n = 1.6f;

public:
	Cone()
		: center(Vector()), radius(1), height(1)  //Default constructor creates a unit cylinder
	{
		color = Color::WHITE;
		shininess = 0.0f;
		translucency = 0.0f;
	};

	Cone(Vector c, float r, float h, Color col, float shin, float tran)
		: center(c), radius(r), height(h)
	{
		color = col;
		shininess = shin;
		translucency = tran;
	};


	float intersect(Vector pos, Vector dir);

	Vector normal(Vector p);

};

class Plane : public Object
{
private:
	Vector a, b, c, d;      //The 4 vertices of a quad

public:
	Plane(void) = default;

	Plane(Vector pa, Vector pb, Vector pc, Vector pd, Color col, float shin, float tran)
		: a(pa), b(pb), c(pc), d(pd)
	{
		color = col;
		shininess = shin;
		translucency = tran;
		
	};


	bool isInside(Vector pos);

	float intersect(Vector pos, Vector dir);

	Vector normal(Vector pos);

};


class Cylinder : public Object
{

private:
	Vector center;
	float radius;
	float height;
	float n = 1.8f;

public:
	Cylinder()
		: center(Vector()), radius(1), height(1)  //Default constructor creates a unit cylinder
	{
		color = Color::WHITE;
		shininess = 0.0f;
		translucency = 0.0f;
	};

	Cylinder(Vector c, float r, float h, Color col, float shin, float tran)
		: center(c), radius(r), height(h)
	{
		color = col;
		shininess = shin;
		translucency = tran;
	};


	float intersect(Vector pos, Vector dir);
	Vector normal(Vector p);

};

//functions implementation

Color Object::getColor()
{
	return color;
}

Color Object::getAmbient()
{
	return ambient;
}

Color Object::getSpecular()
{
	return specular;
}

void Object::setColor(Color col)
{
	color = col;
}

float Object::getShininess()
{
	return shininess;
}

float Object::getTranslucent()
{
	return translucency;
}

float Object::getN()
{
	return n;
}

/**
* Sphere's intersection method.  The input is a ray (pos, dir).
* equation:
* |O + Dt - C| - r = 0
* |D|^2 t^2 + 2(D.CO)t + |CO|^2 - R^2 = 0
* if the ray is normalized |D|^2 = 1
*/
float Sphere::intersect(Vector pos, Vector dir)
{
	Vector vdif = pos - center;
	float b = dir.dot(vdif);
	float len = vdif.length();
	float c = len * len - radius * radius;
	float delta = b * b - c;

	//if (fabs(delta) < 0.5) return -1.0;
	if (delta < 0.0f) return -1.0f;

	float t1 = -b - sqrt(delta);
	float t2 = -b + sqrt(delta);

	if (fabs(t1) < 0.001f)
	{
		if (t2 > 0) return t2;
		else t1 = -1.0f;
	}
	if (fabs(t2) < 0.001f) t2 = -1.0f;

	return (t1 < t2) ? t1 : t2;

}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
Vector Sphere::normal(Vector p)
{
	Vector n = p - center;
	n.normalise();
	return n;
}

//Function to test if an input point is within the quad.
bool Plane::isInside(Vector q)
{
	Vector n = normal(q);
	Vector ua = b - a, ub = c - b, uc = d - c, ud = a - d;
	Vector va = q - a, vb = q - b, vc = q - c, vd = q - d;
	//Complete this function
	return  (ua.cross(va)).dot(n) > 0 && (ub.cross(vb)).dot(n) > 0 &&
		(uc.cross(vc)).dot(n) > 0 && (ud.cross(vd)).dot(n) > 0;
}

//Function to compute the paramter t at the point of intersection.
float Plane::intersect(Vector pos, Vector dir)
{
	Vector n = normal(pos);
	Vector vdif = a - pos;
	float vdotn = dir.dot(n);
	if (fabs(vdotn) < 1.e-4) return -1;
	float t = vdif.dot(n) / vdotn;
	if (fabs(t) < 0.0001) return -1;
	Vector q = pos + dir * t;
	if (isInside(q)) return t;
	else return -1;
}

// Function to compute the unit normal vector
Vector Plane::normal(Vector pos)
{
	return (b - a).cross((c - a));
}

/**
* Cone's intersection method.  The input is a ray (pos, dir).
*/
float Cone::intersect(Vector pos, Vector dir)
{

	float rOverh2 = pow((radius / height), 2);
	float a = dir.x * dir.x
		+ dir.z * dir.z
		- rOverh2 * dir.y * dir.y;

	float b = 2 * dir.x * (pos.x - center.x)
		+ 2 * dir.z * (pos.z - center.z)
		+ 2 * rOverh2 * dir.y * (height + center.y - pos.y);

	float c = pow(pos.x - center.x, 2)
		+ pow(pos.z - center.z, 2)
		- rOverh2 * pow(height + center.y - pos.y, 2);


	float discriminant = sqrt(b * b - 4.0 * a * c);

	if (fabs(discriminant) < 0.001) return -1.0;

	if (discriminant < 0.0) return -1.0;

	float t1 = (-b + discriminant) / (2 * a);
	float t2 = (-b - discriminant) / (2 * a);

	if (fabs(t1) < 0.001)
	{
		if (t2 > 0) return t2;
		else t1 = -1.0;
	}
	if (fabs(t2) < 0.001) t2 = -1.0f;
	float closet = min(t1, t2);
	float furest = max(t1, t2);
	float y1 = (pos.y + closet * dir.y) - center.y;
	float y2 = (pos.y + furest * dir.y) - center.y;
	if (!(y1 < 0 || y1 > height) && closet != -1.0) return closet;
	if (!(y2 < 0 || y2 > height) && furest != -1.0) return furest;
	return -1.0;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the Cone.
*/
Vector Cone::normal(Vector p) {
	float x = p.x - center.x;
	float z = p.z - center.z;
	float d = sqrt(pow(x, 2) + pow(z, 2));
	Vector n = Vector(x, -(radius / height) * d, z);
	n.normalise();
	return n;
}

float Cylinder::intersect(Vector pos, Vector dir)
{

	float a = pow(dir.x, 2) + pow(dir.z, 2);
	float b = 2 * (dir.x * (pos.x - center.x) + dir.z * (pos.z - center.z));
	float c = pow((pos.x - center.x), 2) + pow((pos.z - center.z), 2) - pow(radius, 2);
	float discriminant = sqrt(b * b - 4.0 * a * c);

	if (fabs(discriminant) < 0.001) return -1.0;

	if (discriminant < 0.0) return -1.0;

	float t1 = (-b + discriminant) / (2 * a);
	float t2 = (-b - discriminant) / (2 * a);

	if (fabs(t1) < 0.001)
	{
		if (t2 > 0) return t2;
		else t1 = -1.0;
	}
	if (fabs(t2) < 0.001) t2 = -1.0;

	float closest = min(t1, t2);
	float furthest = max(t1, t2);
	float y1 = (pos.y + closest * dir.y) - center.y;
	float y2 = (pos.y + furthest * dir.y) - center.y;
	if (!(y1 < 0 || y1 > height) && closest != -1.0) return closest;
	if (!(y2 < 0 || y2 > height) && furthest != -1.0) return furthest;
	return -1.0;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
Vector Cylinder::normal(Vector p)
{
	Vector n = Vector((p.x - center.x), 0.0, (p.z - center.z));
	n.normalise();
	return n;
}