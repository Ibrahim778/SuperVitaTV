/*
  MiniVitaTV
  Copyright (C) 2018, TheFloW
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <psp2/kernel/modulemgr.h>
#include <psp2/sysmodule.h>
#include <taihen.h>
#include <psp2/kernel/clib.h>
#include <psp2/vshbridge.h>

#define HOOK_NUM 3

static SceUID hooks[HOOK_NUM];
static tai_hook_ref_t refs[HOOK_NUM];

static int scePafMiscIsVitaTVPatched() 
{
  TAI_CONTINUE(int, refs[0]);
  return 1;
}

static int scePafMiscIsMultiControllerSupportedPatched() 
{
  TAI_CONTINUE(int, refs[1]);
  return 1;
}


static int sceSysmoduleLoadModuleInternalWithArgPatched(SceUInt32 id, SceSize args, void *argp, void *unk) 
{
  int res = TAI_CONTINUE(int, refs[2], id, args, argp, unk);

  if (res >= 0 && id == SCE_SYSMODULE_INTERNAL_PAF) 
  {
    hooks[0] = taiHookFunctionImport(&refs[0], "SceShell", 0x3D643CE8, 0xAF4FC3F4, scePafMiscIsVitaTVPatched);
    hooks[1] = taiHookFunctionImport(&refs[1], "SceShell", 0x3D643CE8, 0x2C227FFD, scePafMiscIsMultiControllerSupportedPatched);
  }

  return res;
}

void _start() __attribute__ ((weak, alias("module_start")));
int module_start(SceSize args, void *argp) 
{
    tai_module_info_t info;
    info.size = sizeof(tai_module_info_t);
    if (taiGetModuleInfo("ScePaf", &info) >= 0) 
    {
        hooks[0] = taiHookFunctionImport(&refs[0], "SceShell", 0x3D643CE8,
                                        0xAF4FC3F4, scePafMiscIsVitaTVPatched);
        hooks[1] = taiHookFunctionImport(&refs[1], "SceShell", 0x3D643CE8,
                                        0x2C227FFD, scePafMiscIsMultiControllerSupportedPatched);
    } 
    else 
    {
        hooks[2] = taiHookFunctionImport(&refs[2], "SceShell", 0x03FCF19D,
                                        0xC3C26339, sceSysmoduleLoadModuleInternalWithArgPatched);
    }
  return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize args, void *argp) 
{

    for(int i = 0; i < HOOK_NUM; i++)
    {
        if(hooks[i] >= 0)
            taiHookRelease(hooks[i], refs[i]);
    }

    return SCE_KERNEL_STOP_SUCCESS;
}
