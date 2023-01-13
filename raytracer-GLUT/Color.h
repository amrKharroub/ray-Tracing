#pragma once
class Color
{

public:
	float r, g, b;
	static const Color WHITE;
	static const Color BLACK;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color GRAY;
	static const Color CYAN;
	static const Color YELLOW;
	static const Color MAGENTA;

	Color()
		: r(1), g(1), b(1)
	{};

	Color(float rCol, float gCol, float bCol)
		: r(rCol), g(gCol), b(bCol)
	{};

	void scaleColor(float scaleFactor);

	Color combineColor(Color col);

	friend Color operator *(float scaler, const Color& c);

	void combineColor(Color col, float scaleFactor);

	friend Color operator +(const Color& c1, const Color& c2);

	Color phongLight(Color diffused, float diffuseTerm, float specularTerm);
};

//Multiplies the current colour by a scalar factor
void Color::scaleColor(float scaleFactor)
{
	r = r * scaleFactor;
	g = g * scaleFactor;
	b = b * scaleFactor;
}

//Modulates the current colour by a given colour
Color Color::combineColor(Color col)
{
	r *= col.r;
	g *= col.g;
	b *= col.b;
	return Color(r, g, b);
}

Color operator *(float scaler, const Color& c) {
	return Color(c.r * scaler, c.g * scaler, c.b * scaler);
}

//Adds a scaled version of a colour to the current colour
void Color::combineColor(Color col, float scaleFactor)
{
	r += scaleFactor * col.r;
	g += scaleFactor * col.g;
	b += scaleFactor * col.b;
}

Color operator +(const Color& c1, const Color& c2)
{
	float r = c1.r + c2.r;
	float g = c1.g + c2.g;
	float b = c1.b + c2.b;
	return Color(r, g, b);
}

const Color Color::WHITE = Color(1, 1, 1);
const Color Color::BLACK = Color(0, 0, 0);
const Color Color::RED = Color(1, 0, 0);
const Color Color::GREEN = Color(0, 1, 0);
const Color Color::BLUE = Color(0, 0, 1);
const Color Color::GRAY = Color(0.2f, 0.2f, 0.2f);
const Color Color::CYAN = Color(0, 1, 1);
const Color Color::MAGENTA = Color(1, 0, 1);
const Color Color::YELLOW = Color(1, 1, 0);