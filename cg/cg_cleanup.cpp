#include "cg_cleanup.hpp"
#include "cg_memory.hpp"

#include "utils/hook.hpp"
#include "com/com_error.hpp"

void CG_Cleanup()
{

    //delete hooks
    hooktable::for_each([](std::unique_ptr<hookbase>& ptr) { ptr->release(); });
    hooktable::release();

    //remove memory modifications
    CG_MemoryTweakCleanup();

}
void CG_CleanupHook()
{
    hooktable::find<void>(HOOK_PREFIX(__func__))->call();
    CG_Cleanup();
}

void CG_ErrorExit(const std::string& reason)
{
    CG_Cleanup();
    return Com_FatalError(reason);
}

