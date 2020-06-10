#include "shp.h"
#include <algorithm>
#include "../utils.h"
#include "../ui/logger.h"


shp::shp(const std::string& fileName, int skip_verts) {
	this->name = fileName.substr(fileName.find_last_of("/\\") + 1);

	fileHandle = SHPOpen(fileName.c_str(), "rb");

	if (!fileHandle) {
		this->~shp();
		return;
	}

	min_x = fileHandle->adBoundsMin[0];
	min_y = fileHandle->adBoundsMin[1];
	max_x = fileHandle->adBoundsMax[0];
	max_y = fileHandle->adBoundsMax[1];

	center_x = (min_x + max_x) / 2.0f;
	center_y = (min_y + max_y) / 2.0f;

	rel_center_x = center_x - min_x;
	rel_center_y = center_y - min_y;

	

	for (unsigned int i = 0; i < fileHandle->nRecords; i++) {
		auto object = SHPReadObject(fileHandle, i);
		if (object) {
			ImVec2* points = 0;
			unsigned int pi = 0;

			unsigned int size_points = 0;
			for (unsigned int vi = 0; vi < object->nVertices; vi += skip_verts) {
				size_points++;
			}
			points = new ImVec2[size_points];

			for (unsigned int vi = 0; vi < object->nVertices; vi += skip_verts) {
				float _X = (float)(object->padfX[vi] - min_x);
				//idk why but padfY seems be inverted relatively to padfX so im inverting it Here using -max_y and fabs method
				float _Y = (float)(fabs(object->padfY[vi] - max_y));

				points[pi] = ImVec2(_X, _Y);
				pi++;
			}

			auto cur_pair = segment_t(points, size_points);
			this->points.push_back(cur_pair);
		}
	}

	logger::get()->add_log("[Loaded SHP]: " + this->name);
	std::cout << "[Loaded SHP]: " << this->name << std::endl;
	std::cout << "min_x: " << min_x << std::endl;
	std::cout << "min_y: " << min_y << std::endl;
	std::cout << "max_x: " << max_x << std::endl;
	std::cout << "max_y: " << max_y << std::endl;
	std::cout << "center_x: " << center_x << std::endl;
	std::cout << "center_y: " << center_y << std::endl;
	std::cout << "========================" << std::endl;

}


SHPHandle shp::get_handle() {
	return this->fileHandle;
}


void shp::drawSHP(const ImVec2& offset, ImDrawList* drawlist, const ImColor& color, float scale_up, float rotation_deg) {
	for (auto& segment : this->points) {
		for (size_t vi = 0; vi < segment.points_array_size; vi++) {
			ImVec2 v1 = segment.points_array[vi];

			//if its last point connect drawed line between last point and first point of array
			ImVec2 v2 = (vi < segment.points_array_size - 1 ? segment.points_array[vi + 1] : segment.points_array[0]);

			if (rotation_deg > 0 || rotation_deg < 0) {
				v1 = math::rotate_point(v1, this->rel_center_x, this->rel_center_y, rotation_deg);
				v2 = math::rotate_point(v2, this->rel_center_x, this->rel_center_y, rotation_deg);
			}

			v1.x = v1.x * scale_up + offset.x;
			v1.y = v1.y * scale_up + offset.y;
			v2.x = v2.x * scale_up + offset.x;
			v2.y = v2.y * scale_up + offset.y;


			drawlist->AddLine(v1, v2, color, 1.5f);
		}
	}
}

void shp::setToParentOrigin(const shp& parent) {
	double delta_x = parent.center_x - this->center_x;
	double delta_y = parent.center_y - this->center_y;

	this->min_x += delta_x;
	this->max_x += delta_y;
	this->min_y += delta_y;
	this->max_y += delta_y;

	this->center_x += delta_x;
	this->center_y += delta_y;

	for (auto& point : this->points) {
		for (size_t i = 0; i < point.points_array_size; i++) {
			point.points_array[i].x += delta_x;
			point.points_array[i].y += delta_y;
		}
	}

	logger::get()->add_log("[Origin]: " + this->name + " to " + parent.name);
}


void shp::rescaleToParent(const shp& parent) {
	double scale_up_x = (parent.max_x - parent.min_x) / (this->max_x - this->min_x);
	double scale_up_y = (parent.max_y - parent.min_y) / (this->max_y - this->min_y);

	this->max_x *= scale_up_x;
	this->max_y *= scale_up_y;

	this->center_x *= scale_up_x;
	this->center_y *= scale_up_y;

	for (auto& point : this->points) {
		for (size_t i = 0; i < point.points_array_size; i++) {
			point.points_array[i].x *= scale_up_x;
			point.points_array[i].y *= scale_up_y;
		}
	}

	logger::get()->add_log("[Rescale X & Y]: " + this->name + " to " + parent.name);
}