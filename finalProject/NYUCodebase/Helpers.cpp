#include "Helpers.h"

float dist(float x1, float y1, float x2, float y2){
	float delX = fabs(x1 - x2);
	float delY = fabs(y1 - y2);
	return sqrt(delX*delX + delY*delY);
}

float lerp(float v0, float v1, float t) {
	return (1.0f - t)*v0 + t*v1;
}

float depenetrate(float c1, float h1, float c2, float h2){
	float p = fabs(fabs(c1 - c2) - h1 - h2);
	float d = p + 0.001f;
	if (c1 < c2){ d *= -1; }
	return c1 + d;
}

bool isSolid(int t, const char* mapName){
	if (strcmp(mapName,"mfTRO.png")==0){
		
		for (int i = 144; i <= 208; i += 16){
			if (t >= i && t <= i + 4){ return true; }
		}
		for (int i = 267; i <= 379; i += 16){ if (t == i){ return true; } }

		if ((t >= 96 && t <= 103) || (t >= 112 && t <= 119) || (t >= 128 && t <= 134) ||
			(t >= 320 && t <= 328) || (t >= 336 && t <= 338) ||
			t == 368 || t == 266 || t == 282 || t == 298) {
			return true;
		}

		return false;
	}

	return false;
}