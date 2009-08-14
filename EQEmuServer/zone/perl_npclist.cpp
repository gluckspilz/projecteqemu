/*  EQEMu:  Everquest Server Emulator
	Copyright (C) 2001-2009  EQEMu Development Team (http://eqemulator.net)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
		are required to give you total support for your newly bought product;
		without even the implied warranty of MERCHANTABILITY or FITNESS FOR
		A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "features.h"
#include "client.h"
#ifdef EMBPERL_XS_CLASSES
#include "../common/debug.h"
#include "embperl.h"

#include "../common/linked_list.h"

#ifdef THIS		/* this macro seems to leak out on some systems */
#undef THIS
#endif

XS(XS_NPCList_GetData); /* prototype to pass -Wmissing-prototypes */
XS(XS_NPCList_GetData)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: NPCList::GetData(THIS)");
	{
		ListElement<NPC*> * THIS;
		NPC * RETVAL;

		if (sv_derived_from(ST(0), "NPCList")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(ListElement<NPC*> *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type ListElement<NPC*>");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = (NPC*)THIS->GetData();
		ST(0) = sv_newmortal();
		sv_setref_pv(ST(0), "NPC", (void*)RETVAL);
	}
	XSRETURN(1);
}

XS(XS_NPCList_GetNext); /* prototype to pass -Wmissing-prototypes */
XS(XS_NPCList_GetNext)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: NPCList::GetNext(THIS)");
	{
		ListElement<NPC*> * THIS;
		ListElement<NPC*> * RETVAL;

		if (sv_derived_from(ST(0), "NPCList")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(ListElement<NPC*> *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type ListElement<NPC*>");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetNext();
		ST(0) = sv_newmortal();
		sv_setref_pv(ST(0), "NPCList", (void*)RETVAL);
	}
	XSRETURN(1);
}

XS(XS_NPCList_GetPrev); /* prototype to pass -Wmissing-prototypes */
XS(XS_NPCList_GetPrev)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: NPCList::GetPrev(THIS)");
	{
		ListElement<NPC*> * THIS;
		ListElement<NPC*> * RETVAL;

		if (sv_derived_from(ST(0), "NPCList")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(ListElement<NPC*> *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type ListElement<NPC*>");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetPrev();
		ST(0) = sv_newmortal();
		sv_setref_pv(ST(0), "NPCList", (void*)RETVAL);
	}
	XSRETURN(1);
}

#ifdef __cplusplus
extern "C"
#endif

XS(boot_NPCList);
XS(boot_NPCList) 
{
	dXSARGS;
	char file[256];
	strncpy(file, __FILE__, 256);
	file[255] = 0;

	if(items != 1)
		fprintf(stderr, "boot_quest does not take any arguments.");
	char buf[128];

	//add the strcpy stuff to get rid of const warnings....

	XS_VERSION_BOOTCHECK ;

		newXSproto(strcpy(buf, "GetData"), XS_NPCList_GetData, file, "$");
		newXSproto(strcpy(buf, "GetNext"), XS_NPCList_GetNext, file, "$");
		newXSproto(strcpy(buf, "GetPrev"), XS_NPCList_GetPrev, file, "$");

	XSRETURN_YES;
}

#endif //EMBPERL_XS_CLASSES