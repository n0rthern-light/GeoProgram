#ifndef _DEFINES_H_
#define _DEFINES_H_

struct Vector {
	float x;
	float y;
	float z;

	Vector(float X, float Y, float Z) {
		x = X;
		y = Y;
		z = Z;
	}
};

POINT rotate_point(float cx, float cy, float angle, POINT p)
{
	float s = sin(angle);
	float c = cos(angle);

	// translate point back to origin:
	p.x -= cx;
	p.y -= cy;

	// rotate point
	float xnew = p.x * c - p.y * s;
	float ynew = p.x * s + p.y * c;

	// translate point back:
	p.x = xnew + cx;
	p.y = ynew + cy;
	return p;
}

#endif