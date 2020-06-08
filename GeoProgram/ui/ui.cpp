#include "ui.h";


LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
D3DPRESENT_PARAMETERS    g_d3dpp;
LPDIRECT3D9				pD3D = NULL;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

ui_manager* ui_manager::get() {
	static ui_manager instance;
	return &instance;
}

void ui_manager::initialize_main_window(LPCWSTR window_name) {
	// Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, window_name, NULL };
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(window_name, window_name, WS_SYSMENU | WS_CAPTION /*| WS_MINIMIZEBOX*/, 100, 100, MAIN_WINDOW_SIZE_X, MAIN_WINDOW_SIZE_Y, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	LPDIRECT3D9 pD3D;
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		UnregisterClass(window_name, wc.hInstance);
		return;
	}
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // Present with vsync
															//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; // Present without vsync, maximum unthrottled framerate

															// Create the D3DDevice
	if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
	{
		pD3D->Release();
		UnregisterClass(window_name, wc.hInstance);
		return;
	}

	// Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplDX9_Init(hwnd, g_pd3dDevice);
	//io.NavFlags |= ImGuiNavFlags_EnableKeyboard;  // Enable Keyboard Controls

	// Setup style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);
	while (msg.message != WM_QUIT)
	{
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		ImGui_ImplDX9_NewFrame();

		// 1. Show a simple window.
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".


		this->draw_main_window();

		// Rendering
		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * 255.0f), (int)(clear_color.y * 255.0f), (int)(clear_color.z * 255.0f), (int)(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_pd3dDevice->Reset(&g_d3dpp);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}

	ImGui_ImplDX9_Shutdown();
	ImGui::DestroyContext();

	if (g_pd3dDevice) g_pd3dDevice->Release();
	if (pD3D) pD3D->Release();
	UnregisterClass(window_name, wc.hInstance);
}

void ui_manager::draw_main_window() {
	ImGui::Begin("main_window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);


	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(MAIN_WINDOW_SIZE_X, MAIN_WINDOW_SIZE_Y));

	if (ImGui::Button("Baza Danych", ImVec2(90, 24))) {
		this->current_sub_index = 0;
	}

	ImGui::SameLine();

	if (ImGui::Button("Mapa Polski", ImVec2(90, 24))) {
		this->current_sub_index = 1;
	}

	ImGui::SameLine();

	if (ImGui::Button("O programie", ImVec2(90, 24))) {
		this->current_sub_index = 2;
	}

	this->draw_sub_window();

	ImGui::End();
}

void ui_manager::draw_sub_window() {
	ImGui::Begin("sub_window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowPos(ImVec2(5, 30));
	ImGui::SetWindowSize(ImVec2(MAIN_WINDOW_SIZE_X - 25, MAIN_WINDOW_SIZE_Y - 30));

	ImGui::Separator();
	if (this->current_sub_index == 0) {
		ImGui::Text("Baza Danych");
		ImGui::Separator();
		this->draw_database_interface();
	}
	else if (this->current_sub_index == 1) {
		ImGui::Text("Mapa Polski");
		ImGui::Separator();
	}
	else if (this->current_sub_index == 2) {
		ImGui::Text("O programie");
		ImGui::Separator();
		this->draw_about();
	}

	ImGui::End();
}

void ui_manager::draw_about() {
	ImGui::NewLine();
	ImGui::Text("Program geoanalityczny, wykonany na");
	ImGui::Text("potrzeby procesu rekrutacyjnego.");
	ImGui::NewLine();
	ImGui::Text("Autor: Michal Juraszek");
	ImGui::Text("email1: ske1337@gmail.com");
	ImGui::Text("email2: hello.coder.michael@gmail.com");
}


void ui_manager::draw_database_interface() {
	ImGui::Begin("sub_window_left", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	ImGui::SetWindowPos(ImVec2(0, 60));
	ImGui::SetWindowSize(ImVec2(MAIN_WINDOW_SIZE_X / 3 * 1, MAIN_WINDOW_SIZE_Y - 60));

	ImGui::Button("Losuj punkty", ImVec2(120, 24));
	ImGui::Button("Test x < 3km", ImVec2(120, 24));
	ImGui::Button("Wypisz punkty", ImVec2(120, 24));

	ImGui::End();

	ImGui::Begin("sub_window_right", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowPos(ImVec2((MAIN_WINDOW_SIZE_X / 3 * 1), 60));
	ImGui::SetWindowSize(ImVec2(MAIN_WINDOW_SIZE_X / 3 * 2, MAIN_WINDOW_SIZE_Y - 60));



	ImGui::Text("Prawa strona");

	ImGui::End();
}