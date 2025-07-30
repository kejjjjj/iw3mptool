#pragma once


union XAssetHeader;
enum class MaterialTechniqueType : int;
union XAssetEntryPoolEntry;

inline struct DxGlobals* dx = reinterpret_cast<DxGlobals*>(0x0CC9A400);
inline struct clientActive_t* clients = reinterpret_cast<clientActive_t*>(0xC5F930);
inline struct clientStatic_t* cls = reinterpret_cast<clientStatic_t*>(0x956D80);
inline struct clientConnection_t* clc = reinterpret_cast<clientConnection_t*>(0x8F4CE0);
inline struct cg_s* cgs = reinterpret_cast<cg_s*>(0x74E338);
inline struct GfxBuffers* gfxBuf = reinterpret_cast<GfxBuffers*>(0xD2B0840);
inline struct GfxScene* scene = reinterpret_cast<GfxScene*>(0xCF10280);
inline struct clientInfo_t* cinfo = reinterpret_cast<clientInfo_t*>(0x839270);
inline struct r_globals_t* rg = reinterpret_cast<r_globals_t*>(0x0CC9D150);
inline struct r_global_permanent_t* rgp = reinterpret_cast<r_global_permanent_t*>(0xCC98280);
inline struct pmoveHandler_t* pmoveHandlers = reinterpret_cast<pmoveHandler_t*>(0x729448);
inline struct CmdArgs* cmd_args = reinterpret_cast<CmdArgs*>(0x1410B40);
inline struct DpvsGlob* dpvsGlob = reinterpret_cast<DpvsGlob*>(0xD065368);
inline struct level_locals_t* level = reinterpret_cast<level_locals_t*>(0x13EB6A8);
inline struct server_t* sv = reinterpret_cast<server_t*>(0x17FC7C8);
inline struct scrMemTreePub_t* gScrMemTreePub = reinterpret_cast<scrMemTreePub_t*>(0x14E8A04);
inline struct ent_field_t* ent_fields = reinterpret_cast<ent_field_t*>(0x6BC0B8);
inline struct MantleAnimTransition* s_mantleTrans = reinterpret_cast<MantleAnimTransition*>(0x729800);


inline struct serverStatic_t* svs = reinterpret_cast<serverStatic_t*>(0x0185C480);
inline struct centity_s* centity = reinterpret_cast<centity_s*>(0x0084F2D8);

inline struct entityState_s* eState = reinterpret_cast<entityState_s*>(0x0084F2D8);
inline const char** g_entityBeginParsePoint = reinterpret_cast<const char**>(0x1113674);
inline const char** g_entityEndParsePoint = reinterpret_cast<const char**>(0x1113678);
inline int* clientActive_cmdNumber = reinterpret_cast<int*>(0xCC5FF8); // part of clientActive_t

inline extern void* dbHandle = reinterpret_cast<void*>(0x14E89A4);

inline const char** zone_code_post_gfx_mp = reinterpret_cast<const char**>(0xCC9D128);
inline const char** zone_localized_code_post_gfx_mp = reinterpret_cast<const char**>(0xCC9D134);
inline const char** zone_ui_mp = reinterpret_cast<const char**>(0xCC9D12C);
inline const char** zone_common_mp = reinterpret_cast<const char**>(0xCC9D130);
inline const char** zone_localized_common_mp = reinterpret_cast<const char**>(0xCC9D138);
inline 	const char** zone_mod = reinterpret_cast<const char**>(0xCC9D13C);

inline XAssetHeader* DB_XAssetPool = reinterpret_cast<XAssetHeader*>(0x7265E0);
inline unsigned int* g_poolSize = reinterpret_cast<unsigned int*>(0x7263A0);

inline struct GfxBackEndData* frontEndDataOut = reinterpret_cast<GfxBackEndData*>(0xCC9827C);

inline char* initStringDvarValue = reinterpret_cast<char*>(0x6BFEA7);

inline struct clipMap_t* cm = reinterpret_cast<clipMap_t*>(0x14098C0);
inline struct ComWorld* com = reinterpret_cast<ComWorld*>(0x1435CB8);

inline MaterialTechniqueType* OverflowTessTech = reinterpret_cast<MaterialTechniqueType*>(0xD540EFC);
inline struct Material* OverflowTessSurf = reinterpret_cast<Material*>(0xD540EF8);
inline struct Material* builtIn_material_unlit = reinterpret_cast<Material*>(0xCC9A2C4); // 0xCC9A2B4
inline struct Material* builtIn_material_unlit_depth = reinterpret_cast<Material*>(0xCC9A2C0);

inline struct materialCommands_t* tess = reinterpret_cast<materialCommands_t*>(0xD085EE0);

inline struct DebugGlobals* debugGlob = reinterpret_cast<DebugGlobals*>((((char*)frontEndDataOut) + 0x11E71C));
inline struct GfxBackEndData* _frontEndDataOut = reinterpret_cast<GfxBackEndData*>(0xCC9827C);
inline GfxBackEndData* _backEndData = reinterpret_cast<GfxBackEndData*>(0xD0704BC);
inline struct GfxWorld* gfxWorld = reinterpret_cast<GfxWorld*>(0xD0701E0);

// print3d // DebugStrings / Lines
inline int* clsDebugFromServer = reinterpret_cast<int*>(0xC5B020);
inline bool* ifRendererStarted = reinterpret_cast<bool*>(0x956E88);
inline struct clientDebugStringInfo_t* clsDebugSV_Strings = reinterpret_cast<clientDebugStringInfo_t*>(0xC5B044);
inline struct clientDebugStringInfo_t* clsDebugCL_Strings = reinterpret_cast<clientDebugStringInfo_t*>(0xC5B024);

// --------
// RENDERER

inline struct MaterialLoadGlob* mtlLoadGlob = reinterpret_cast<MaterialLoadGlob*>(0xD541330);

inline int* RenderTargetWidth = reinterpret_cast<int*>(0xD573EBC); //D573EBC
inline int* RenderTargetHeight = reinterpret_cast<int*>(0xD573EC0); // D573EC0
inline int* RenderTargetArray = reinterpret_cast<int*>(0xD573EB0); // D573EC0

inline int* wnd_SceneHeight = reinterpret_cast<int*>(0xCC9D0E4); // CC9D0E4
inline float* wnd_SceneAspect = reinterpret_cast<float*>(0xCC9D0FC); // CC9D0FC

//IDirect3DDevice9** dx9_device_ptr = reinterpret_cast<IDirect3DDevice9**>(0xCC9A408); // actual device

inline Material* floatz_display = reinterpret_cast<Material*>(0xFA5378);
inline struct GfxCmdBufSourceState* gfxCmdBufSourceState = reinterpret_cast<GfxCmdBufSourceState*>(0xD53F5F0);
inline struct GfxCmdBufState* gfxCmdBufState = reinterpret_cast<GfxCmdBufState*>(0xD5404F0);
inline struct clientDebugLineInfo_t* clientDebugLineInfo_client = reinterpret_cast<clientDebugLineInfo_t*>(0xC5B054);
inline struct clientDebugLineInfo_t* clientDebugLineInfo_server = reinterpret_cast<clientDebugLineInfo_t*>(0xC5B074);

// get handle using DB_FindXAssetHeader
//R_RegisterFont_t R_RegisterFont = (R_RegisterFont_t)0x5F1EC0;
//Material_RegisterHandle_t Material_RegisterHandle = (Material_RegisterHandle_t)0x5F2A80;

//RB_EndTessSurface_t RB_EndTessSurface = (RB_EndTessSurface_t)0x61A2F0;


	// ---------
// UI / MENU

inline unsigned int* ui_white_material_ptr = reinterpret_cast<unsigned int*>(0xCAF06F0);

inline int* gameTypeEnum = reinterpret_cast<int*>(0xCAF1820);
inline int* mapNameEnum = reinterpret_cast<int*>(0xCAF2330);

inline struct UiContext* _cgDC = reinterpret_cast<UiContext*>(0x746FA8);
inline struct UiContext* _uiContext = reinterpret_cast<UiContext*>(0xCAEE200);


inline struct PlayerKeyState* playerKeys = reinterpret_cast<PlayerKeyState*>(0x8F1CA0);
inline struct clientUIActive_t* clientUI = reinterpret_cast<clientUIActive_t*>(0xC5F8F4);

inline struct ScreenPlacement* scrPlace = reinterpret_cast<ScreenPlacement*>(0xE34420);
inline struct ScreenPlacement* scrPlaceFull = reinterpret_cast<ScreenPlacement*>(0xE343D8);

// ----
// IWDs

inline const char* fs_gamedir = reinterpret_cast<const char*>(0xCB19898);
inline struct searchpath_s* fs_searchpaths = reinterpret_cast<searchpath_s*>(0xD5EC4DC);

// --------
// MOVEMENT

inline struct WeaponDef** BG_WeaponNames = reinterpret_cast<WeaponDef**>(0x736DB8);
//memcpy(&BG_WeaponNames, 0x736DB8, sizeof(BG_WeaponNames));

inline struct gentity_s* g_entities = reinterpret_cast<gentity_s*>(0x12885C4);
inline struct gclient_s* g_clients = reinterpret_cast<gclient_s*>(0x13255A8);
inline int* currentTime = reinterpret_cast<int*>(0x13EB894);

inline int* CanDamageContentMask = reinterpret_cast<int*>(0x802011);

// -------
// CONSOLE 

inline float* con_matchtxtColor_currentDvar = reinterpret_cast<float*>(0x6BDF14); // 0x6BDF14
inline float* con_matchtxtColor_currentValue = reinterpret_cast<float*>(0x6BDF24); // 0x6BDF24
inline float* con_matchtxtColor_defaultValue = reinterpret_cast<float*>(0x6BDF34); // 0x6BDF34
inline float* con_matchtxtColor_dvarDescription = reinterpret_cast<float*>(0x6BDF54); // 0x6BDF54
inline float* con_matchtxtColor_domainDescription = reinterpret_cast<float*>(0x6BDF44); // 0x6BDF44


// console structs
inline struct ConDrawInputGlob* conDrawInputGlob = reinterpret_cast<ConDrawInputGlob*>(0x8CC2C8);
inline struct Console* con = reinterpret_cast<Console*>(0x8DC8C0);
inline struct field_t* g_consoleField = reinterpret_cast<field_t*>(0x8F1B88);
inline struct field_t* historyEditLines = reinterpret_cast<field_t*>(0x8F29CC);

// console variables
//Font_s* con_font = reinterpret_cast<Font_s*>(0xC5AE94));
inline bool* Key_IsCatcherActive = reinterpret_cast<bool*>(0xC5F8F8);

// con->screenMin / Max manual defines for use in asm
inline float* conScreenMin0 = reinterpret_cast<float*>(0x8ECB14); // left
inline float* conScreenMin1 = reinterpret_cast<float*>(0x8ECB18); // top
inline float* conScreenMax0 = reinterpret_cast<float*>(0x8ECB1C); // right
inline float* conScreenMax1 = reinterpret_cast<float*>(0x8ECB20); // bottom

inline bool* extvar_con_ignoreMatchPrefixOnly = reinterpret_cast<bool*>(0x736BB1);

// cmd args
inline int* argc_1410B84 = reinterpret_cast<int*>(0x1410B84);
inline 	int* argc_1410B40 = reinterpret_cast<int*>(0x1410B40);
inline 	char* argv_6BFEA7 = reinterpret_cast<char*>(0x6BFEA7);
inline int* argv_1410BA4 = reinterpret_cast<int*>(0x1410BA4);

inline CmdArgs* sv_cmd_args = reinterpret_cast<CmdArgs*>(0x1433408);


inline struct cmd_function_s* cmd_functions = reinterpret_cast<cmd_function_s*>(0x1410B3C);
inline void* Cmd_AddCommand_fnc;
//SCR_DrawSmallStringExt_t	SCR_DrawSmallStringExt = (SCR_DrawSmallStringExt_t)0x474C30;
//Sys_IsMainThread_t			Sys_IsMainThread = (Sys_IsMainThread_t)0x50B5D0;
//Con_TokenizeInput_t			Con_TokenizeInput = (Con_TokenizeInput_t)0x45F350;
//Con_CancelAutoComplete_t	Con_CancelAutoComplete = (Con_CancelAutoComplete_t)0x460A90;
//Con_DrawInputPrompt_t		Con_DrawInputPrompt = (Con_DrawInputPrompt_t)0x460510;
//Cmd_EndTokenizedString_t	Cmd_EndTokenizedString = (Cmd_EndTokenizedString_t)0x4F98C0;

//ConDrawInput_IncrMatchCounter_t		ConDrawInput_IncrMatchCounter = (ConDrawInput_IncrMatchCounter_t)0x45FA40;
//Con_AnySpaceAfterCommand_t			Con_AnySpaceAfterCommand = (Con_AnySpaceAfterCommand_t)0x45F3D0;
//ConDrawInput_DetailedDvarMatch_t	ConDrawInput_DetailedDvarMatch = (ConDrawInput_DetailedDvarMatch_t)0x45FFB0;
//ConDrawInput_DetailedCmdMatch_t		ConDrawInput_DetailedCmdMatch = (ConDrawInput_DetailedCmdMatch_t)0x460370;

//ConDrawInput_DvarMatch_t	ConDrawInput_DvarMatch = (ConDrawInput_DvarMatch_t)0x45FAB0;
//ConDrawInput_CmdMatch_t		ConDrawInput_CmdMatch = (ConDrawInput_CmdMatch_t)0x460440;
//Con_DrawOutputScrollBar_t	Con_DrawOutputScrollBar = (Con_DrawOutputScrollBar_t)0x461860;
//Con_DrawOutputText_t		Con_DrawOutputText = (Con_DrawOutputText_t)0x4619E0;

// ------
// COMMON


inline struct playerState_s* ps_loc = reinterpret_cast<playerState_s*>(0x13255A8);
inline playerState_s* ps_to = reinterpret_cast<playerState_s*>(0xD90BD0C);
inline struct pmove_t* pmove = reinterpret_cast<pmove_t*>(0x8C9C90);

struct scr_const_t;
inline size_t* cmd_id = reinterpret_cast<size_t*>(0x1410B40);
inline 	size_t* cmd_argc = reinterpret_cast<size_t*>(0x1410B84);
inline char*** cmd_argv = reinterpret_cast<char***>(0x1410BA4);
inline cmd_function_s** cmd_ptr = reinterpret_cast<cmd_function_s**>(0x1410B3C);
inline scr_const_t* scr_const = reinterpret_cast<scr_const_t*>(0x1406E90);



inline struct XZone* g_zones = reinterpret_cast<XZone*>(0xFFEFD0);
inline XAssetEntryPoolEntry* g_assetEntryPool = reinterpret_cast<XAssetEntryPoolEntry*>(0xF0D640);
inline unsigned short* db_hashTable = reinterpret_cast<unsigned short*>(0xE62A80);
inline struct infoParm_t* infoParams = reinterpret_cast<infoParm_t*>(0x71FBD0); // Count 0x1C



inline struct cg_t* cg = reinterpret_cast<cg_t*>(0x0074E338);
inline struct cgs_t* cgs_s = reinterpret_cast<cgs_t*>(0x074A908);

inline struct animScriptData_t* globalScriptData = reinterpret_cast<animScriptData_t*>(0x0D5EC414);
inline struct RE_Tags_t* tag = reinterpret_cast<RE_Tags_t*>(0x1406E90);

inline float* penetrationDepthTable = reinterpret_cast<float*>(0x7373B8);
inline struct pointtrace_t* vec3_origin = reinterpret_cast<pointtrace_t*>(0x69C69C);

inline unsigned short* clientObjMap = reinterpret_cast<unsigned short*>(0x14A9F30);
inline char** objBuf = reinterpret_cast<char**>(0x1477F30);

inline struct WinMouseVars_t* s_wmv = reinterpret_cast<WinMouseVars_t*>(0xCC147C4);

inline int* bg_lastParsedWeaponIndex = reinterpret_cast<int*>(0xD5EC418);

inline struct LoadedMenus_s* g_menus = reinterpret_cast<LoadedMenus_s*>(0xCB16310);

inline gentity_s* gentities = reinterpret_cast<gentity_s*>(0x1288500);

inline char** s_threadNames = reinterpret_cast<char**>(0x07249F4);
inline unsigned int* threadHandles = reinterpret_cast<unsigned int*>(0x14E89C0);

inline struct clientSnapshot_t* newSnap = reinterpret_cast<clientSnapshot_t*>(0xD90BCF0);
inline struct playerStatNetworkData* statData = reinterpret_cast<playerStatNetworkData*>(0xCC18C90);
inline struct kbutton_t* playersKb = reinterpret_cast<kbutton_t*>(0x8F1518);