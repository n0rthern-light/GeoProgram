#include "engine.h"


bool engine::load_shp(const std::string& sFilePath, int skip_verts) {
	shp* new_shp = new shp(sFilePath, skip_verts);
	if (new_shp) {
		this->shp_files.push_back(new_shp);
		return true;
	}
	
	return false;
}

shp* engine::get_shp(int index) {
	return this->shp_files.at(index);
}