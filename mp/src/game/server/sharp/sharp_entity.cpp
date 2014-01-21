#include "cbase.h"
#include "sharp/entity.h"
#include "sharp_dt.h"
#include "sharp_entity.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_SERVERCLASS_ST( CSharpEntity, DT_SharpEntity )
	//SendPropString( SENDINFO( m_iSharpClassname ), 0, SendProxy_StringToString, (byte)1 )
	//SendPropStringT( SENDINFO( m_iSharpClassname ) ),
	//SendPropSharp()
END_SEND_TABLE()

// Start of our data description for the class
//BEGIN_DATADESC( CSharpEntity )
//END_DATADESC()


CSharpEntity::~CSharpEntity()
{
	if ( m_pMotionController != NULL )
	{
		physenv->DestroyMotionController( m_pMotionController );
		m_pMotionController = NULL;
	}
}

void CSharpEntity::PostConstructor( const char *szClassname )
{
	//m_iSharpClassname.Set( AllocPooledString( szClassname ) );
	BaseClass::PostConstructor( szClassname );

	//SetModelScale( 2.5f );
}

void CSharpEntity::Precache()
{
	//enginesound->PrecacheSound("bananaeat.wav", true);
}

void CSharpEntity::PhysicsSimulate( void )
{
	g_SharpEntity->FirePhysicsSimulate( this );
	BaseClass::PhysicsSimulate();
}


void CSharpEntity::Think( void )
{
	g_SharpEntity->FireThink( this );
	//SetNextThink( gpGlobals->curtime + 0.01f );

	BaseClass::Think();
}

IMotionEvent::simresult_e CSharpEntity::Simulate( IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular ){
	UPDATE_DOMAIN();

	MonoObject* object = GetSharpEntity();
	MonoObject* exception;
	MonoClass* clazz = mono_object_get_class(object);
	MonoMethod* simulateMethod = mono_class_get_method_from_name( clazz, "Simulate", 4 );
	MonoMethod* virtualMethod = mono_object_get_virtual_method( object, simulateMethod );

	void* args[4];
	args[0] = &linear;
	args[1] = &angular;
	args[2] = &deltaTime;
	args[3] = NULL; //TODO: Get physics object

	MonoObject* retn = mono_runtime_invoke (virtualMethod, object, args, &exception);

	if (exception) {
		mono_unhandled_exception( exception );
		return SIM_NOTHING;
	}

	int retnval = *(int*)mono_object_unbox ( retn );

	return (IMotionEvent::simresult_e) retnval;

}




//IMPLEMENT_SERVERCLASS_ST( CSharpBrushEntity, DT_SharpBrushEntity )
//	SendPropStringT( SENDINFO( m_iSharpClassname ) )
//END_SEND_TABLE()



void CSharpBrushEntity::PostConstructor( const char *szClassname )
{
	//m_iSharpClassname.Set( AllocPooledString( szClassname ) );
	BaseClass::PostConstructor( szClassname );
}

void CSharpBrushEntity::Spawn( void )
{
	SetModel( STRING( GetModelName() ) );    // set size and link into world
	BaseClass::Spawn();
}
