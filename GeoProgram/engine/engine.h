#ifndef _ENGINE_H_
#define _ENGINE_H_
#include "shp.h"
#include <vector>


class engine {
	std::vector<shp*> shp_files;

public:
	static engine* get() {
		static engine _engine;
		return &_engine;
	}

	bool load_shp(const std::string& sFilePath, int skip_verts = 100);
	shp* get_shp(int index = 0);


};














#endif