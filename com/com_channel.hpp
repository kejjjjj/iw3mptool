#pragma once

enum $B87C0110D100A68234FECCEB9075A41E : int;
enum errorParm_t : int;

void Com_Printf		 ($B87C0110D100A68234FECCEB9075A41E channel, const char* msg, ...);
void Com_Printf(const char* msg, ...);

void Com_Error(errorParm_t category, const char* msg, ...);

void iPrintLn(const char* msg);
void iPrintLnBold(const char* text, ...);