#ifndef _SHP_H_
#define _SHP_H_

#include "../lib/shapefil.h"
#include <string>
#include <vector>
#include <iostream>
#include "../ui/imgui/imgui.h"


struct segment_t {
	ImVec2* points_array;
	size_t points_array_size;

	segment_t(ImVec2* points_array, size_t points_array_size) {
		this->points_array = points_array;
		this->points_array_size = points_array_size;
	}
};


class shp {
	std::string name;
	SHPHandle fileHandle;
	std::vector<segment_t> points;

	double min_x;
	double min_y;
	double max_x;
	double max_y;

	double center_x;
	double center_y;

	double rel_center_x;
	double rel_center_y;

public:

	~shp() {
		if (fileHandle) {
			SHPClose(fileHandle);
		}
	}

	shp(const std::string& fileName, int skip_verts = 100);

	SHPHandle get_handle();
	void setToParentOrigin(const shp& parent);
	void rescaleToParent(const shp& parent);
	void drawSHP(const ImVec2& offset, ImDrawList* drawlist, const ImColor& color, float scale_up = 15.f, float rotation_deg = 0.f);
};



#endif