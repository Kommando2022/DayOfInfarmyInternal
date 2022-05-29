#include "Include.h"

typedef HRESULT(__stdcall* TypeEndScene)(LPDIRECT3DDEVICE9 device);
TypeEndScene endscene = NULL;

HWND windowhandle = FindWindowA("Valve001", "Day of Infamy");

HRESULT __stdcall EndSceneHook(LPDIRECT3DDEVICE9 device)
{
	static bool imguiinit = false;

	if (!imguiinit)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui::StyleColorsClassic();

		ImGui_ImplWin32_Init(windowhandle);
		ImGui_ImplDX9_Init(device);

		imguiinit = true;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Test");
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return endscene(device);
}

void Init()
{
	FILE* outputfile;
	AllocConsole();
	freopen_s(&outputfile, "CONOUT$", "w", stdout);

	UINT32 shaderapidx9handle = (UINT32)GetModuleHandleA("shaderapidx9.dll");
	UINT32 device = *(UINT32*)(shaderapidx9handle + 0xDC3F0);

	if (!device)
	return;

	UINT32* vtable = *(UINT32**)device;

	if (!vtable)
	return;

	endscene = (TypeEndScene)vtable[42];
	vtable[42] = (UINT32)EndSceneHook;
}

BOOL APIENTRY DllMain(HMODULE moduelehandle, DWORD reason, LPVOID reserve)
{
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
		Init();
		break;
	}
	return TRUE;
}