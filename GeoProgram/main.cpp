#include "./ui/ui.h"
#include "./engine/engine.h"

int main() {

	engine::get()->load_shp("C:\\Users\\xapak\\Downloads\\Panstwo\\panstwo", 100);
	engine::get()->load_shp("C:\\Users\\xapak\\Downloads\\Wojewodztwa\\wojewodztwa", 100);

	auto panstwo = engine::get()->get_shp(0);
	auto wojewodztwa = engine::get()->get_shp(1);

	if (panstwo && wojewodztwa) {
		wojewodztwa->setToParentOrigin(*panstwo);
		//wojewodztwa->rescaleToParent(*panstwo);
	}


	ui_manager::get()->initialize_main_window(L"GeoApp v0.1");
	return 0;
}