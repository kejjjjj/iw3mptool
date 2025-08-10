#pragma once

#include <string>

void hmm_f();

void CM_AddModel(const struct GfxStaticModelDrawInst* model);
struct cm_model CM_MakeModel(const GfxStaticModelDrawInst* model);

void CM_LoadAllModelsToClipMap(const std::string& filter);
