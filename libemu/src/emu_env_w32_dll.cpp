/********************************************************************************
 *                               libemu
 *
 *                    - x86 shellcode emulation -
 *
 *
 * Copyright (C) 2007  Paul Baecher & Markus Koetter
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * 
 *             contact nepenthesdev@users.sourceforge.net  
 *
 *******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "emu.h"
#include "emu_env.h"
#include "emu_env_w32_dll.h"
#include "emu_env_w32_dll_export.h"
extern "C"{
	#include "emu_hashtable.h"
}

struct emu_env_w32_dll *emu_env_w32_dll_new(void)
{
	struct emu_env_w32_dll *dll = (struct emu_env_w32_dll *)malloc(sizeof(struct emu_env_w32_dll));
	memset(dll,0,sizeof(struct emu_env_w32_dll));
    return dll;
}

void emu_env_w32_dll_free(struct emu_env_w32_dll *dll)
{
	emu_hashtable_free(dll->exports_by_fnptr);
	emu_hashtable_free(dll->exports_by_fnname);
	emu_hashtable_free(dll->exports_by_ordinal);
	free(dll->exportx);
//	free(dll->hooks);
	free(dll->dllname);
	free(dll);
}

struct emu_env_w32_dll_export *emu_env_w32_dll_export_new(void)
{
	struct emu_env_w32_dll_export *exp = (struct emu_env_w32_dll_export *)malloc(sizeof(struct emu_env_w32_dll_export));
	memset(exp,0,sizeof(struct emu_env_w32_dll_export));
	return exp;
}

void emu_env_w32_dll_export_free(struct emu_env_w32_dll_export *exp)
{
	free(exp);
}

void emu_env_w32_dll_exports_copy(struct emu_env_w32_dll *to,struct emu_env_w32_dll_export *from)
{
	uint32_t size;
	uint32_t i;
	for (i=0;from[i].fnname != 0; i++);

	size = i+1; //we want the last null element too dz 5.21.11

	to->exportx = (struct emu_env_w32_dll_export*)malloc(sizeof(struct emu_env_w32_dll_export) * size);
//	to->hooks = (struct emu_env_hook*)malloc(sizeof(struct emu_env_hook) * size);
	memcpy(to->exportx, from, sizeof(struct emu_env_w32_dll_export) * size);

	to->exports_by_ordinal = emu_hashtable_new(size, emu_hashtable_ptr_hash,    emu_hashtable_ptr_cmp);
	to->exports_by_fnptr   = emu_hashtable_new(size, emu_hashtable_ptr_hash,    emu_hashtable_ptr_cmp);
	to->exports_by_fnname  = emu_hashtable_new(size, emu_hashtable_string_hash, emu_hashtable_string_cmp);

	for (i=0;from[i].fnname != 0; i++)
	{
		struct emu_env_w32_dll_export *ex = &to->exportx[i];
		//struct emu_env_hook *hook = &to->hooks[i];
		//hook->type = emu_env_type_win32;
		//hook->hook.win = ex;

		//emu_hashtable_insert(to->exports_by_fnptr, (void *)(uintptr_t)from[i].virtualaddr, hook);
		//emu_hashtable_insert(to->exports_by_fnname, (void *)(uintptr_t)from[i].fnname, hook);
		emu_hashtable_insert(to->exports_by_fnptr, (void *)(uintptr_t)from[i].virtualaddr, ex);
		emu_hashtable_insert(to->exports_by_fnname, (void *)(uintptr_t)from[i].fnname, ex);
		emu_hashtable_insert(to->exports_by_ordinal, (void *)(uintptr_t)from[i].ordinal, ex);
	}
}
