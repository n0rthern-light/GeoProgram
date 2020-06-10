#ifndef _UI_H_
#define _UI_H_

#include "./imgui/imgui.h"
#include "./imgui/imgui_impl_dx9.h"
#include <d3d9.h>

#include "logger.h"


#define MAIN_WINDOW_SIZE_X 800
#define MAIN_WINDOW_SIZE_Y 600

extern LPDIRECT3DDEVICE9        g_pd3dDevice;
extern D3DPRESENT_PARAMETERS    g_d3dpp;
extern LPDIRECT3D9				pD3D;
extern LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ui_manager {
	HWND hwnd;
	unsigned int current_sub_index;
	logger database_tab_logger;

public:
	ui_manager() {
		this->hwnd = NULL;
		this->current_sub_index = 0;
		this->database_tab_logger = logger();
	}

	static ui_manager* get();
	void initialize_main_window(LPCWSTR window_name);
	void draw_main_window();
	void draw_sub_window();
	void draw_about();
	void draw_database_interface();
};

#endif