#pragma once
#include "cbase.h"
#include "sharp/sharphelper.h"
#include "sharp/sharp.h"


class SharpEntityListener : 
#if !defined( CLIENT_DLL )
	public IEntityListener, 
#else
	public IClientEntityListener, 
#endif
	public IMonoAddon {
public:
	SharpEntityListener(){
		SharpAddons()->AddAddon( this );
	}
	~SharpEntityListener(){
		//gEntList.RemoveListenerEntity( this );
	}

	virtual void OnEntityCreated( CBaseEntity *pEntity );
	virtual void OnEntitySpawned( CBaseEntity *pEntity ){};
	virtual void OnEntityDeleted( CBaseEntity *pEntity );

	virtual void UpdateMehods( MonoImage* image );
	virtual void RegisterInternalCalls();

	SharpClass GetEntityClass( const char *pClassName );

#if !defined( CLIENT_DLL )
	CBaseEntity* Create( const char *pClassName );
#endif

	SharpMethod m_mCreateMethod;
	SharpMethod m_mDeleteMethod;
	SharpMethod m_GetTypeMethod;

	SharpClass m_baseAnimatingClass;
	SharpClass m_funcBrushClass;

};


extern SharpEntityListener* g_SharpEntityListener;
