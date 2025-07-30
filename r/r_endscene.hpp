#pragma once

long __stdcall R_EndScene(struct IDirect3DDevice9* device);
void RB_DrawDebug(GfxViewParms* viewParms);

void R_RecoverLostDevice();
void CL_ShutdownRenderer();
int __cdecl R_CreateDeviceInternal(struct HWND__* hwnd, int BehaviorFlags, struct _D3DPRESENT_PARAMETERS_* d3dpp);