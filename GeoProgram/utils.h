#ifndef _UTILS_H_
#define _UTILS_H_
#include "ui/imgui/imgui.h"

#define M_PI 3.14159265358979323846

namespace math {

	inline float ang2rad(float ang) {
		return ang * M_PI / 180.f;
	}

	inline ImVec2 rotate_point(ImVec2 origin, float cx, float cy, float rotation) {
		float s = sin(rotation);
		float c = cos(rotation);

		// translate point back to origin:
		origin.x -= cx;
		origin.y -= cy;

		// rotate point
		float xnew = origin.x * c - origin.y * s;
		float ynew = origin.x * s + origin.y * c;

		// translate point back:
		origin.x = xnew + cx;
		origin.y = ynew + cy;
		return origin;
	}
}





#endif