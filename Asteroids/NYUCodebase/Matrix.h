//#include "Helpers.h"
#include "math.h"
#include <cfloat>

class Vector {
public:
	Vector();
	Vector(float x, float y, float z);
	float length();
	void normalize();

	float operator* (const Vector& v) const;
	Vector operator* (float k) const;

	float x;
	float y;
	float z;
};

//Column-major matrix for transformations
class Matrix {
public:
	Matrix();
	union {
		float m[4][4];
		float ml[16];
	};
	void identity();
	Matrix inverse();
	
	Matrix operator* (const Matrix& other) const;
	Vector operator* (const Vector& v) const;
};

void calcAxis(Vector& axis, Vector* box, size_t i);

void calcExtrema(float* min, float* max, const Vector& axis, Vector* box);

float calcP(const Vector& axis, Vector* box1, Vector* box2);

//Satisfying axis thm
//Return how far to move box1, in world coordinates
Vector sat(const Matrix& m1, float hw1, float hh1, const Matrix& m2, float hw2, float hh2);