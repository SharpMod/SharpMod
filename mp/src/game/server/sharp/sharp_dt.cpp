#include "cbase.h"
#include "sharp/sharp.h"
#include "sharp/entity.h"
#include <mono/metadata/tokentype.h>
#include "sharp/sharphelper.h"

#include <cstring>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class SharpDT : public IMonoAddon {

public:
	SharpDT(){
		SharpAddons()->AddAddon( this );
	}

	virtual void RegisterInternalCalls();
	virtual void UpdateMehods( MonoImage* image );

	SharpMethod m_getEntityIntVarMethod;
	SharpMethod m_getEntityVectorVarMethod;
	SharpMethod m_getEntityStringVarMethod;
};

SharpDT sharpDT;

void SharpDT::UpdateMehods( MonoImage* image )
{
	SharpClass entityDTClass = mono_class_from_name( image, "Sharp", "EntityDT");
	
	m_getEntityIntVarMethod = entityDTClass.GetMethod("GetEntityIntVar", 3 );
	m_getEntityVectorVarMethod = entityDTClass.GetMethod("GetEntityVectorVar", 3 );
	m_getEntityStringVarMethod = entityDTClass.GetMethod("GetEntityStringVar", 3 );
}


static void SetDTArraySizes( EntityMonoObject *monoEntity, int intSize, int stringSize, int vectorSize )
{
	ASSERT_DOMAIN();

	CBaseEntity* entity = monoEntity->GetEntity();

	entity->m_sharpDT.intArraySize = intSize;
	entity->m_sharpDT.stringArraySize= stringSize;
	entity->m_sharpDT.vectorArraySize = vectorSize;

	entity->m_sharpDT.m_bufferedStr.SetSize( stringSize );
}


void SharpDT::RegisterInternalCalls(){
	mono_add_internal_call ("Sharp.EntityDT::SetDTArraySizes", SetDTArraySizes );
}

static bool SendProxy_SharpList( const void *pData, void *pOut, int iElement, SharpMethod method ){
	THREAD_UPDATE_DOMAIN();
	CBaseEntity *pEntity = (CBaseEntity*) pData;

	Assert( pEntity->MonoHandle.IsValid() );

	void* params[] = {
		pEntity->GetSharpEntity(),
		&iElement,
		pOut
	};

	return method.SafeInvoke( params );
}

static void SendProxy_SharpListInt( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID )
{
	if( !SendProxy_SharpList( pData, &pOut->m_Int, iElement, sharpDT.m_getEntityIntVarMethod ) ){
		pOut->m_Int = 0;
	}
}

//Oh god, this is so horrible
//We are pasting a string to the local buffer all the time
static void SendProxy_SharpListString( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID )
{
	MonoString* str = NULL;
	CBaseEntity *pEntity = (CBaseEntity*) pData;

	pOut->m_pString = "";

	if( SendProxy_SharpList( pData, &str, iElement, sharpDT.m_getEntityStringVarMethod ) && str != NULL )
	{
		char* monoStr = mono_string_to_utf8( str );

		Q_strncpy( pEntity->m_sharpDT.m_bufferedStr[iElement].buf, monoStr, DT_MAX_STRING_BUFFERSIZE );
		
		pOut->m_pString = pEntity->m_sharpDT.m_bufferedStr[iElement].buf;

		mono_free( monoStr );
	}
}

static void SendProxy_SharpListVector( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID )
{
	if( !SendProxy_SharpList( pData, &pOut->m_Vector, iElement, sharpDT.m_getEntityVectorVarMethod ) )
	{
		pOut->m_Vector[0] = 0.0f;
		pOut->m_Vector[1] = 0.0f;
		pOut->m_Vector[2] = 0.0f;
	}
}

static int SendProxyArrayLength_SharpArrayInt( const void *pStruct, int objectID )
{
	CBaseEntity *pEntity = (CBaseEntity*) pStruct;
	return pEntity->m_sharpDT.intArraySize;
}

static int SendProxyArrayLength_SharpArrayString( const void *pStruct, int objectID )
{
	CBaseEntity *pEntity = (CBaseEntity*) pStruct;
	return pEntity->m_sharpDT.stringArraySize;
}


static int SendProxyArrayLength_SharpArrayVector( const void *pStruct, int objectID )
{
	CBaseEntity *pEntity = (CBaseEntity*) pStruct;
	return pEntity->m_sharpDT.vectorArraySize;
}



static void SendProxy_ClassnameString( const SendProp *pProp, const void *pStruct, const void *pVarData, DVariant *pOut, int iElement, int objectID )
{
	CBaseEntity *pEntity = (CBaseEntity*) pVarData;
	Assert( pEntity->m_iClassname );
	pOut->m_pString = (char*)STRING( pEntity->m_iClassname );
}


SendProp SendPropSharp(){
	SendProp ret;

	ret.m_Type = DPT_DataTable;
	ret.m_pVarName = "sharpDT";
	ret.SetDataTableProxyFn( SendProxy_DataTableToDataTable );
	ret.SetFlags( SPROP_PROXY_ALWAYS_YES );

	SendProp *pProps = new SendProp[7]; // TODO free that again

	pProps[0] = SendPropString("SharpDTClassname", 0, DT_MAX_STRING_BUFFERSIZE, 0, SendProxy_ClassnameString );
	pProps[1] = SendPropInt("sharpDTInt", 0, sizeof(int), -1, SPROP_UNSIGNED, SendProxy_SharpListInt );
	pProps[2] = InternalSendPropArray( 1024, 0, "sharpDTArrayInt", SendProxyArrayLength_SharpArrayInt );
	pProps[3] = SendPropString("sharpDTString", 0, DT_MAX_STRING_BUFFERSIZE, 0, SendProxy_SharpListString );
	pProps[4] = InternalSendPropArray( 1024, 0, "sharpDTArrayString", SendProxyArrayLength_SharpArrayString );
	pProps[5] = SendPropVector("sharpDTVector", 0, sizeof(Vector), 32, SPROP_NOSCALE, 0.0f, HIGH_DEFAULT, SendProxy_SharpListVector );
	pProps[6] = InternalSendPropArray( 1024, 0, "sharpDTArrayVector", SendProxyArrayLength_SharpArrayVector );

	SendTable *pTable = new SendTable( pProps, 7, ret.m_pVarName ); // TODO free that again

	ret.SetDataTable( pTable );

	return ret;
}


