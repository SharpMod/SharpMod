#include "cbase.h"
#include "c_sharp_dt.h"
#include "mono/metadata/threads.h"
#include "sharp/entitylistner.h"
#include "sharp/sharphelper.h"
#include <mono/metadata/tokentype.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CSharpDT : public IMonoAddon {

public:
	CSharpDT(){
		SharpAddons()->AddAddon( this );
	}

	virtual void RegisterInternalCalls(){};
	virtual void UpdateMehods( MonoImage* image );

	SharpMethod m_setEntityVarIntMethod;
	SharpMethod m_setEntityVarStringMethod;
	SharpMethod m_setEntityVarVectorMethod;
};

CSharpDT sharpDT;

void CSharpDT::UpdateMehods( MonoImage* image ){
	SharpClass entityDTClass = mono_class_from_name( image, "Sharp", "EntityDT");
	
	m_setEntityVarIntMethod = entityDTClass.GetMethod("SetEntityIntVar", 3 );
	m_setEntityVarStringMethod = entityDTClass.GetMethod("SetEntityStringVar", 3 );
	m_setEntityVarVectorMethod = entityDTClass.GetMethod("SetEntityVectorVar", 3 );
}

static void Proxy_SetData( C_BaseEntity *pEntity, int index, void* value, SharpMethod method )
{
	if( !pEntity->MonoHandle.IsValid() )
		return;

	THREAD_UPDATE_DOMAIN();

	void* params[] = {
		pEntity->GetSharpEntity(),
		&index,
		value
	};

	method.SafeInvoke( params );

}

static void RecvProxy_SharpListInt(  const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	int val = pData->m_Value.m_Int;
	Proxy_SetData( (C_BaseEntity*) pOut, pData->m_iElement, &val, sharpDT.m_setEntityVarIntMethod );
}

static void RecvProxy_SharpListVector(  const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	//Boy this is ugly; Fix me. Easy way to convert (Other than a cast?)
	Vector val( pData->m_Value.m_Vector[0], pData->m_Value.m_Vector[1], pData->m_Value.m_Vector[2] );
	Proxy_SetData( (C_BaseEntity*) pOut, pData->m_iElement, &val, sharpDT.m_setEntityVarVectorMethod );
}

static void RecvProxy_SharpListString(  const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	MonoString* str = mono_string_new(g_Sharp.GetDomain(), pData->m_Value.m_pString );

	Proxy_SetData( (C_BaseEntity*) pOut, pData->m_iElement, str, sharpDT.m_setEntityVarStringMethod );
}


static void RecvProxyArrayLength_Sharp( void *pStruct, int objectID, int currentArrayLength )
{
	//Msg("C: Received length: %d\n", currentArrayLength );
}

static void RecvProxy_ClassNameString( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	C_BaseEntity *pEntity = (C_BaseEntity*) pOut;
	Q_strncpy( pEntity->m_iSharpClassname, pData->m_Value.m_pString, sizeof(pEntity->m_iSharpClassname) );

	g_SharpEntityListener->OnEntityCreated( pEntity );
}

RecvProp RecvPropSharp(){
	RecvProp ret;

	ret.m_pVarName = "sharpDT";
	ret.SetOffset( 0 );
	ret.m_RecvType = DPT_DataTable;
	ret.SetDataTableProxyFn( DataTableRecvProxy_StaticDataTable );
	
	RecvProp *pProps = new RecvProp[7]; // TODO free that again
	pProps[0] = RecvPropString("SharpDTClassname", 0, DT_MAX_STRING_BUFFERSIZE, 0, RecvProxy_ClassNameString);
	pProps[1] = RecvPropInt( "sharpDTInt", 0, 4, 0, RecvProxy_SharpListInt );
	pProps[2] = InternalRecvPropArray( 1024, 0, "sharpDTArrayInt", RecvProxyArrayLength_Sharp );
	pProps[3] = RecvPropString( "sharpDTString", 0, DT_MAX_STRING_BUFFERSIZE, 0, RecvProxy_SharpListString );
	pProps[4] = InternalRecvPropArray( 1024, 0, "sharpDTArrayString", RecvProxyArrayLength_Sharp );
	pProps[5] = RecvPropVector( "sharpDTVector", 0, sizeof(Vector), 0, RecvProxy_SharpListVector );
	pProps[6] = InternalRecvPropArray( 1024, 0, "sharpDTArrayVector", RecvProxyArrayLength_Sharp );

	RecvTable *pTable = new RecvTable( pProps, 7, ret.m_pVarName ); // TODO free that again

	ret.SetDataTable( pTable );

	return ret;
}

