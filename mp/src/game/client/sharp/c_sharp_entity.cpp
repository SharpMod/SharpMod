#include "cbase.h"
#include "sharp/entity.h"
#include "c_sharp_dt.h"
#include "sharp/entitylistner.h"
#include "sharp/c_sharp_entity.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#undef CSharpEntity

static void RecvProxySharpClassname( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	RecvProxy_StringToString( pData, pStruct, pOut );
}

IMPLEMENT_CLIENTCLASS_DT(C_SharpEntity, DT_SharpEntity, CSharpEntity)
	//RecvPropString( RECVINFO( m_iSharpClassname ), 0, RecvProxySharpClassname )
	//RecvPropSharp()
END_RECV_TABLE()


void C_SharpEntity::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
}

void C_SharpEntity::PostDataUpdate( DataUpdateType_t updateType )
{
	BaseClass::PostDataUpdate( updateType );
}

void C_SharpEntity::Spawn( void )
{
	SetModelScale( 2.5f );
	BaseClass::Spawn();

	SetNextClientThink( CLIENT_THINK_ALWAYS );
	
}

void C_SharpEntity::ReceiveMessage( int classID, bf_read &msg )
{
	// BaseEntity doesn't have a base class we could relay this message to
	Assert( classID == GetClientClass()->m_ClassID );
	
	g_SharpEntity->FireReceiveMessage(this, msg);
}

SharpClass C_SharpEntity::GetMonoClass()
{
	//We have not received the classname yet from the server
	//We do not yet have a mono class
	if( strlen( m_iSharpClassname ) == 0 )
		return NULL;

	SharpClass clazz = g_SharpEntityListener->GetEntityClass( m_iSharpClassname );

	if( clazz.m_monoClass == NULL ){
		Warning("Missing clientside implementation of: %s", m_iSharpClassname );
		return BaseClass::GetMonoClass();
	}

	return clazz;
}

int C_SharpEntity::DrawModel( int flags )
{
	int retn = BaseClass::DrawModel( flags);
	return retn;
}

void C_SharpEntity::ClientThink( void )
{
	g_SharpEntity->FireThink(this);
	BaseClass::ClientThink();
}

/*
IMPLEMENT_CLIENTCLASS_DT(C_SharpBrushEntity, DT_SharpBrushEntity, CSharpBrushEntity)
	RecvPropString( RECVINFO( m_iSharpClassname ), 0, RecvProxy_Classname )
END_RECV_TABLE()

SharpClass C_SharpBrushEntity::GetMonoClass()
{
	//We have not received the classname yet from the server
	//We do not yet have a mono class
	if( strlen( m_iSharpClassname ) == 0 )
		return NULL;

	SharpClass clazz = g_SharpEntityListener->GetEntityClass( m_iSharpClassname );

	if( clazz.m_monoClass == NULL ){
		Warning("Missing clientside implementation of: %s", m_iSharpClassname );
		return BaseClass::GetMonoClass();
	}

	return clazz;
}
*/
