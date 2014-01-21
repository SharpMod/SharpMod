#pragma once

#include "cbase.h"
#include "sharp/sharp.h"


class C_SharpEntity : public C_BaseAnimating
{	
public:
	DECLARE_CLASS( C_SharpEntity, C_BaseAnimating );
	DECLARE_CLIENTCLASS();

	virtual void Spawn( void ) override;
	void ClientThink( void ) override;
	int DrawModel( int flags ) override;

	virtual void					OnDataChanged( DataUpdateType_t updateType ) override;
	virtual void			PostDataUpdate( DataUpdateType_t updateType ) override;

	virtual	bool					IsSharpEntity( void ) const { return true; }

	// Server to client message received
	virtual void					ReceiveMessage( int classID, bf_read &msg ) override;

	virtual SharpClass GetMonoClass() override;

	//virtual char const				*GetClassname( void ){
	//	return STRING( m_iSharpClassname );
	//}

	//char							m_iSharpClassname[MAX_PATH];

private:
};

/*
#include "c_func_brush.h"

class C_SharpBrushEntity : public C_FuncBrush
{
public:
	DECLARE_CLASS( C_SharpBrushEntity, C_FuncBrush );
	DECLARE_CLIENTCLASS();

	virtual	bool					IsSharpEntity( void ) const { return true; }

	virtual SharpClass GetMonoClass();

	virtual char const				*GetClassname( void ){
		return STRING( m_iSharpClassname );
	}
	char							m_iSharpClassname[MAX_PATH];

private:
};
*/

