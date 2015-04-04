#include "Matrix.h"

Matrix::Matrix(){ identity(); }

void Matrix::identity(){
	for (int col = 0; col < 4; col++){
		for (int row = 0; row < 4; row++){
			m[col][row] = (col == row ? 1.0f : 0);
		}
	}
}

Matrix Matrix::inverse() {
	float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
	float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
	float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
	float m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];
	float v0 = m20 * m31 - m21 * m30;
	float v1 = m20 * m32 - m22 * m30;
	float v2 = m20 * m33 - m23 * m30;
	float v3 = m21 * m32 - m22 * m31;
	float v4 = m21 * m33 - m23 * m31;
	float v5 = m22 * m33 - m23 * m32;
	float t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
	float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
	float t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
	float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);
	float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
	float d00 = t00 * invDet;
	float d10 = t10 * invDet;
	float d20 = t20 * invDet;
	float d30 = t30 * invDet;
	float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;
	v0 = m10 * m31 - m11 * m30;
	v1 = m10 * m32 - m12 * m30;
	v2 = m10 * m33 - m13 * m30;
	v3 = m11 * m32 - m12 * m31;
	v4 = m11 * m33 - m13 * m31;
	v5 = m12 * m33 - m13 * m32;
	float d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;
	v0 = m21 * m10 - m20 * m11;
	v1 = m22 * m10 - m20 * m12;
	v2 = m23 * m10 - m20 * m13;
	v3 = m22 * m11 - m21 * m12;
	v4 = m23 * m11 - m21 * m13;
	v5 = m23 * m12 - m22 * m13;
	float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;
	Matrix m2;
	m2.m[0][0] = d00;
	m2.m[0][1] = d01;
	m2.m[0][2] = d02;
	m2.m[0][3] = d03;
	m2.m[1][0] = d10;
	m2.m[1][1] = d11;
	m2.m[1][2] = d12;
	m2.m[1][3] = d13;
	m2.m[2][0] = d20;
	m2.m[2][1] = d21;
	m2.m[2][2] = d22;
	m2.m[2][3] = d23;
	m2.m[3][0] = d30;
	m2.m[3][1] = d31;
	m2.m[3][2] = d32;
	m2.m[3][3] = d33;
	return m2;
}

Vector::Vector(){ x = y = z = 0; }
Vector::Vector(float x, float y, float z):x(x),y(y),z(z) {}
float Vector::length(){ return sqrt(x*x + y*y + z*z); }
void Vector::normalize(){
	float l = length();
	x /= l; y /= l; z /= l;
}

float Vector::operator* (const Vector& v) const{ return x*v.x + y*v.y + z*v.z; }

Vector Vector::operator* (float k) const{ return Vector(x*k,y*k,z*k); }

Matrix Matrix::operator* (const Matrix& other) const {
	Matrix ans;
	for (int row = 0; row < 4; row++){
		for (int col = 0; col < 4; col++){
			ans.m[col][row] = 0;
			for (int i = 0; i < 4; i++){
				ans.m[col][row] += m[i][row] * other.m[col][i];
			}
		}
	}

	return ans;
}

Vector Matrix::operator* (const Vector& v) const {
	Vector ans;
	ans.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0];
	ans.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1];
	ans.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2];
	return ans;
}

void calcAxis(Vector& axis, Vector* box, size_t i){
	axis.x = box[i + 1].x - box[i].x;
	axis.y = box[i + 1].y - box[i].y;
	axis.normalize();
}

void calcExtrema(float* min, float* max, const Vector& axis, Vector* box){
	float x = -(FLT_MAX - 1);
	float n = FLT_MAX;
	for (size_t j = 0; j < 4; j++){
		float proj = box[j] * axis;
		if (proj < n) { n = proj; }
		if (proj > x) { x = proj; }
	}
	*min = n; *max = x;
}

float calcP(const Vector& axis, Vector* box1, Vector* box2){
	float min1, max1, min2, max2;
	calcExtrema(&min1, &max1, axis, box1);
	calcExtrema(&min2, &max2, axis, box2);
	float w1 = max1 - min1; float w2 = max2 - min2;
	float c1 = (max1 + min1) / 2; float c2 = (max2 + min2) / 2;
	return fabs(c1 - c2) - (w1 + w2) / 2;
}

Vector sat(const Matrix& m1, float hw1, float hh1, const Matrix& m2, float hw2, float hh2){
	/*
	a--d
	|  |
	b--c
	*/

	//Get all points
	Vector box1[4] = {Vector(-hw1,hh1,0),Vector(-hw1,-hh1,0),
					Vector(hw1,-hh1,0),Vector(hw1,hh1,0)};
	Vector box2[4] = { Vector(-hw2, hh2, 0), Vector(-hw2, -hh2, 0),
					Vector(hw2, -hh2, 0), Vector(hw2, hh2, 0) };

	//Convert them to world coordinates
	for (size_t i = 0; i < 4; i++){
		box1[i] = m1 * box1[i];
		box2[i] = m2 * box2[i];
	}

	Vector axis;
	Vector minAxis; float minP = 0; float p;
	for (size_t i = 0; i < 2; i++){
		calcAxis(axis, box1, i);
		p = calcP(axis, box1, box2);
		if (p < minP) { minAxis = axis; minP = p; }
		
		calcAxis(axis, box2, i);
		p = calcP(axis, box1, box2);
		if (p < minP) { minAxis = axis; minP = p; }
	}

	return minAxis * minP;
}