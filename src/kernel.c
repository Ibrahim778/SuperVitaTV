#include <psp2/types.h>
#include <psp2kern/kernel/debug.h>
#include <taihen.h>
#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/sysmem.h>

#define HOOK_NUM 7

static SceUID hooks[HOOK_NUM];
static tai_hook_ref_t refs[HOOK_NUM];

int ksceisVitaPatched()
{
    TAI_CONTINUE(int, refs[2]);
    return 0;
}

int ksceisGenuineVitaPatched()
{
    TAI_CONTINUE(int, refs[3]);
    return 0;
}

int sceKernelIsPSVitaTVPatched(void)
{
    TAI_CONTINUE(int, refs[6]);
    return 1;
}

int ksceisDolcePatched()
{
    TAI_CONTINUE(int, refs[0]);
    return 1;
}

int getModuleForCDPatched(void)
{
    TAI_CONTINUE(int, refs[4]);
    return SCE_KERNEL_MODEL_VITATV;
}

int getModulePatched(void)
{
    TAI_CONTINUE(int, refs[5]);
    return SCE_KERNEL_MODEL_VITATV;
}

int ksceGenuineDolcePatched()
{
    TAI_CONTINUE(int, refs[1]);
    return 1;
}

void _start() __attribute__ ((weak, alias("module_start")));
int module_start(SceSize args, void *argp) 
{
    tai_module_info_t info;
    info.size = sizeof(info);
    if(taiGetModuleInfoForKernel(KERNEL_PID, "SceSysmem", &info) >= 0)
    {
        hooks[0] = taiHookFunctionExportForKernel(KERNEL_PID, &refs[0], "SceSysmem", 0xFD00C69A, 0x71608CA3, ksceisDolcePatched);
        hooks[1] = taiHookFunctionExportForKernel(KERNEL_PID, &refs[1], "SceSysmem", 0xFD00C69A, 0xC6E83F34, ksceGenuineDolcePatched);
        hooks[2] = taiHookFunctionExportForKernel(KERNEL_PID, &refs[2], "SceSysmem", 0xFD00C69A, 0x963CA644, ksceisGenuineVitaPatched);
        hooks[3] = taiHookFunctionExportForKernel(KERNEL_PID, &refs[3], "SceSysmem", 0xFD00C69A, 0x4273B97B, ksceisVitaPatched);
        hooks[4] = taiHookFunctionExportForKernel(KERNEL_PID, &refs[4], "SceSysmem", 0x37FE725A, 0xA2CB322F, getModuleForCDPatched);
        hooks[5] = taiHookFunctionExportForKernel(KERNEL_PID, &refs[5], "SceSysmem", 0x37FE725A, 0xA2CB322F, getModulePatched);
        hooks[6] = taiHookFunctionExportForKernel(KERNEL_PID, &refs[6], "SceSysmem", 0x37FE725A, 0x1453A5E5, sceKernelIsPSVitaTVPatched);
    }
    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize args, void *argp)
{
    for (int i = 0; i < HOOK_NUM; i++)
    {
        if(hooks[i] >= 0)
            taiHookReleaseForKernel(hooks[i], refs[i]);
    }
    return SCE_KERNEL_STOP_SUCCESS;
}