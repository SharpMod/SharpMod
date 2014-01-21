#include "cbase.h"
#include "entitylistner.h"
#if !defined( CLIENT_DLL )
#include "entitylist.h"
#else
#include "cliententitylist.h"
#endif

#include "datacache/imdlcache.h"
#include "sharp/entity.h"



// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

SharpEntityListener sharpEntityListener;
SharpEntityListener* g_SharpEntityListener = &sharpEntityListener;

/**
	MAJOR HACK! 
	This is definied in object-internals.h, in the mono source code
	Without this, I see no other way to cast a System.Type into a MonoClass*
*/
struct _MonoReflectionType {
	MonoObject object;
	MonoType  *type;
};

void SharpEntityListener::UpdateMehods( MonoImage* image ){
	
	SharpClass entityManager = mono_class_from_name( image, "Sharp", "EntityManager");
	m_mCreateMethod = entityManager.GetMethod( "CallCreate", 1 );
	m_mDeleteMethod = entityManager.GetMethod( "CallDelete", 1 );
	m_GetTypeMethod = entityManager.GetMethod( "GetType", 1 );

	m_baseAnimatingClass = mono_class_from_name( image, "Sharp", "BaseAnimating");
	m_funcBrushClass = mono_class_from_name( image, "Sharp", "FuncBrush");


#if !defined( CLIENT_DLL )
		//gEntList.AddListenerEntity( this );
#else
		//cl_entitylist->AddListenerEntity( this );
#endif
}

#ifndef CLIENT_DLL
static MonoObject* CreateEntity( MonoString* monoStr )
{
	ASSERT_DOMAIN();
	MDLCACHE_CRITICAL_SECTION();

	bool allowPrecache = CBaseEntity::IsPrecacheAllowed();
	CBaseEntity::SetAllowPrecache( true );

	char* classname = mono_string_to_utf8( monoStr );
	CBaseEntity* entity = CreateEntityByName( classname );
	mono_free( classname );

	if( entity != NULL ){
		AssertMsg( entity->MonoHandle.IsValid(), "Created entity but it does not have a mono handle!");

		if( entity->MonoHandle.IsValid() ){
			CBaseEntity::SetAllowPrecache( allowPrecache );
			return entity->MonoHandle.GetObject().m_monoObject;
		}

		UTIL_Remove( entity );
	}

	CBaseEntity::SetAllowPrecache( allowPrecache );
	return NULL;
};
#endif

void SharpEntityListener::RegisterInternalCalls()
{
#ifndef CLIENT_DLL
	mono_add_internal_call ("Sharp.EntityManager::CreateEntity", CreateEntity);
#endif
}

SharpClass SharpEntityListener::GetEntityClass( const char *pClassName ){
	UPDATE_DOMAIN();

	void* args[1];
	args[0] = mono_string_new( g_Sharp.GetDomain(), pClassName );

	MonoObject* exception;
	MonoReflectionType* reflection = (MonoReflectionType*) m_GetTypeMethod.Invoke( args, &exception ).m_monoObject;

	Assert( exception == NULL );
	if( exception != NULL ){
		Warning("Hey! You! How could you throw an exception at Source.EntityManager::GetType method?");
		return SharpClass();
	}

	if( reflection == NULL ){
		//Oh well, it is not a C# entity either
		return SharpClass();
	}

	return SharpClass( mono_class_from_mono_type( reflection->type ) );


}

#if !defined( CLIENT_DLL )
CBaseEntity* SharpEntityListener::Create( const char *pClassName ){
	UPDATE_DOMAIN();

	SharpClass clazz = GetEntityClass( pClassName );
	if( clazz.m_monoClass == NULL )
		return NULL;

	//const char* name = mono_class_get_name( clazz );

	CBaseEntity* entity;
	
	if( mono_class_is_subclass_of( clazz.m_monoClass, m_baseAnimatingClass.m_monoClass, false ) )
	{
		entity = new CSharpEntity( clazz );
	}
	else if ( mono_class_is_subclass_of( clazz.m_monoClass, m_funcBrushClass.m_monoClass, false ) )
	{
		entity = new CSharpBrushEntity( clazz );
	}
	else 
	{
		Error("Class %s does not have a valid subclass.", pClassName );
		return NULL;
	}
	
	entity->PostConstructor( pClassName );

	return entity;
}
#endif


//Does not work: http://stackoverflow.com/a/9088204/99966
//MonoClass *entityManager = mono_class_from_name (m_pSharp->m_pImage, "Source", "EntityManager");
//MonoEvent *spawnEvent = mono_class_get_event_from_name( entityManager, "OnSpawn" );
//MonoMethod* raiseMethod = mono_event_get_raise_method( spawnEvent );




void SharpEntityListener::OnEntityCreated( CBaseEntity *pEntity ) {
	if( pEntity->MonoHandle.IsValid() )
		return;

	UPDATE_DOMAIN();

	SharpClass clazz = pEntity->GetMonoClass();

	//TODO: Why is this null?
	Assert( clazz.m_monoClass );
	if( clazz.m_monoClass == NULL )
		return;

	SharpObject playerInstance = g_SharpEntity->NewEntity( pEntity, clazz );

	void* args[1] = { playerInstance.m_monoObject };
	m_mCreateMethod.SafeInvoke( args );
};

void SharpEntityListener::OnEntityDeleted( CBaseEntity *pEntity ) {
	UPDATE_DOMAIN()

	void* args[] = { pEntity->MonoHandle.GetObject().m_monoObject };
	m_mDeleteMethod.SafeInvoke( args );
};

