#include "cbase.h"
#include "sharp/sharp.h"
#include "vphysics_interface.h"
#include "sharp/sharphelper.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ISoundEmitterSystemBase *soundemitterbase;

struct EntityIPhysicsObject : public TemplateSharpObject<IPhysicsObject> {

};

class SharpPhysics : public IMonoAddon {

public:
	SharpPhysics(){
		SharpAddons()->AddAddon( this );	
	};

	virtual void RegisterInternalCalls( void );
	virtual void UpdateMehods( MonoImage* image );

	SharpClass mSurfaceData;
	SharpClass mSoundParameters;

protected:
	
};
SharpPhysics sharpPhysics;

struct SharpIPhysicsSurfaceProps {
	MonoObject object;
	IPhysicsSurfaceProps* physprops;
};

struct SharpSurfaceData {
	MonoObject obj;
	surfacedata_t surfacedata;
};

struct SharpSoundEmitterBase {
	MonoObject obj;
	ISoundEmitterSystemBase* emitter;
};

struct SharpCSoundParameters
{
	MonoObject obj;
	int channel;
	float volume;
	int pitch;
	int pitchlow, pitchhigh;
	int soundlevel;

	bool play_to_owner_only;
	int count;
	MonoString* soundname;
	int delay_msec;
};


static float GetPhysicsMass ( EntityIPhysicsObject *monoPhysics ) {
	IPhysicsObject* physics = monoPhysics->Get();
	
	return physics->GetMass();
};


static void SetPhysicsMass ( EntityIPhysicsObject *monoPhysics, float mass ) {
	IPhysicsObject* physics = monoPhysics->Get();
	
	physics->SetMass( mass );
};


static void SetBuoyancyRatio ( EntityIPhysicsObject *monoPhysics, float ratio ) {
	IPhysicsObject* physics = monoPhysics->Get();
	
	physics->SetBuoyancyRatio( ratio );
};

static Vector GetPhysicsVelocity ( EntityIPhysicsObject *monoPhysics ) {
	IPhysicsObject* physics = monoPhysics->Get();
	Vector velocity;
	AngularImpulse impulse;

	physics->GetVelocity( &velocity, &impulse );

	return velocity;
};

static Vector GetPhysicsPosition ( EntityIPhysicsObject *monoPhysics ) {
	IPhysicsObject* physics = monoPhysics->Get();
	Vector velocity;
	physics->GetPosition( &velocity, NULL );
	return velocity;
};

static QAngle GetPhysicsAngles ( EntityIPhysicsObject *monoPhysics ) {
	IPhysicsObject* physics = monoPhysics->Get();
	QAngle angles;
	physics->GetPosition( NULL, &angles );
	return angles;
};


static void SetPhysicsVelocity ( EntityIPhysicsObject *monoPhysics, Vector velocity ) {
	IPhysicsObject* physics = monoPhysics->Get();
	AngularImpulse impulse;
	
	physics->SetVelocity( &velocity, &impulse );
};

static bool GetPhysicsGravity ( EntityIPhysicsObject *monoPhysics ) {
	IPhysicsObject* physics = monoPhysics->Get();
	return physics->IsGravityEnabled();
};


static void SetPhysicsGravity ( EntityIPhysicsObject *monoPhysics, bool gravity ) {
	IPhysicsObject* physics = monoPhysics->Get();
	physics->EnableGravity( gravity );
};

static bool GetPhysicsMotion ( EntityIPhysicsObject *monoPhysics ) {
	IPhysicsObject* physics = monoPhysics->Get();
	
	return physics->IsMotionEnabled();
};

static void SetPhysicsMotion ( EntityIPhysicsObject *monoPhysics, bool motion ) {
	IPhysicsObject* physics = monoPhysics->Get();
	
	physics->EnableMotion( motion );
};

static bool GetPhysicsStatic ( EntityIPhysicsObject *monoPhysics ) {
	IPhysicsObject* physics = monoPhysics->Get();
	
	return physics->IsStatic();
};


static void PhysicsWake ( EntityIPhysicsObject *monoPhysics ) {
	IPhysicsObject* physics = monoPhysics->Get();
	
	physics->Wake();
};

static void ApplyForceCenter ( EntityIPhysicsObject *monoPhysics, Vector force ) {
	IPhysicsObject* physics = monoPhysics->Get();
	
	physics->ApplyForceCenter(force);
};

static SharpSurfaceData* SurfacePropsGetSurfaceData( SharpIPhysicsSurfaceProps* props, int surfaceDataIndex )
{
	surfacedata_t* surfaceData = props->physprops->GetSurfaceData( surfaceDataIndex );

	if( surfaceData == NULL )
		return NULL;

	SharpSurfaceData *sharpProps = (SharpSurfaceData*) sharpPhysics.mSurfaceData.NewInstance().m_monoObject;

	sharpProps->surfacedata = *surfaceData;

	return sharpProps;
}

static MonoString* SurfacePropsGetString( SharpIPhysicsSurfaceProps* props, unsigned short stringTableIndex )
{
	return mono_string_new( g_Sharp.GetDomain(), props->physprops->GetString( stringTableIndex ) );
}


static void PhysicsDispose( EntityIPhysicsObject *monoPhysics ) {
};

static SharpCSoundParameters* GetParametersForSound( SharpSoundEmitterBase* emitterBase, MonoString* sharpSoundname, gender_t gender, bool isbeingemitted )
{
	char* soundname = mono_string_to_utf8( sharpSoundname );
	CSoundParameters params;

	if(!emitterBase->emitter->GetParametersForSound( soundname, params, gender, isbeingemitted ) )
	{
		mono_free( soundname );
		return NULL;
	}

	SharpCSoundParameters* soundParams = (SharpCSoundParameters*) sharpPhysics.mSoundParameters.NewInstance().m_monoObject;

	soundParams->channel = params.channel;
	soundParams->volume = params.volume;
	soundParams->pitch = params.pitch;
	soundParams->pitchlow = params.pitchlow;
	soundParams->pitchhigh = params.pitchhigh;
	soundParams->soundlevel = params.soundlevel;
	soundParams->play_to_owner_only = params.play_to_owner_only;
	soundParams->count = params.count;
	soundParams->soundname = mono_string_new( g_Sharp.GetDomain(), params.soundname );
	soundParams->delay_msec = params.delay_msec;

	mono_free( soundname );

	return soundParams;
}

static gender_t GetActorGender( SharpSoundEmitterBase* emitterBase, MonoString* actormodel )
{
	if( actormodel == NULL )
		return emitterBase->emitter->GetActorGender( NULL );

	char* actor = mono_string_to_utf8( actormodel );

	gender_t gender = emitterBase->emitter->GetActorGender( actor );

	mono_free( actor );

	return gender;
}

void SharpPhysics::RegisterInternalCalls( void ){

	mono_add_internal_call ("Sharp.SurfaceProps::GetSurfaceData", SurfacePropsGetSurfaceData );
	mono_add_internal_call ("Sharp.SurfaceProps::GetString", SurfacePropsGetString );

	mono_add_internal_call ("Sharp.SoundEmitterBase::GetParametersForSound", GetParametersForSound );
	mono_add_internal_call ("Sharp.SoundEmitterBase::GetActorGender", GetActorGender );	

	mono_add_internal_call ("Sharp.IPhysicsObject::get_Mass", GetPhysicsMass);
	mono_add_internal_call ("Sharp.IPhysicsObject::set_Mass", SetPhysicsMass);
	//mono_add_internal_call ("Sharp.IPhysicsObject::get_BuoyancyRatio", GetBuoyancyRatio);
	mono_add_internal_call ("Sharp.IPhysicsObject::set_BuoyancyRatio", SetBuoyancyRatio);
	mono_add_internal_call ("Sharp.IPhysicsObject::get_Velocity", GetPhysicsVelocity);
	mono_add_internal_call ("Sharp.IPhysicsObject::set_Velocity", SetPhysicsVelocity);
	mono_add_internal_call ("Sharp.IPhysicsObject::get_Gravity", GetPhysicsGravity);
	mono_add_internal_call ("Sharp.IPhysicsObject::set_Gravity", SetPhysicsGravity);
	mono_add_internal_call ("Sharp.IPhysicsObject::get_Motion", GetPhysicsMotion);
	mono_add_internal_call ("Sharp.IPhysicsObject::set_Motion", SetPhysicsMotion);
	mono_add_internal_call ("Sharp.IPhysicsObject::get_Static", GetPhysicsStatic);
	mono_add_internal_call ("Sharp.IPhysicsObject::Wake", PhysicsWake);
	mono_add_internal_call ("Sharp.IPhysicsObject::ApplyForceCenter", ApplyForceCenter);
	mono_add_internal_call ("Sharp.IPhysicsObject::get_Position", GetPhysicsPosition);
	mono_add_internal_call ("Sharp.IPhysicsObject::get_Angles", GetPhysicsAngles);
	mono_add_internal_call ("Sharp.IPhysicsObject::Dispose", PhysicsDispose);

}

void SharpPhysics::UpdateMehods( MonoImage* image )
{
	
	SharpClass sharpSurfacePropsClass = mono_class_from_name( image, "Sharp","SurfaceProps");
	SharpClass sharpSoundEmitterClass = mono_class_from_name( image, "Sharp","SoundEmitterBase");

	mSurfaceData = mono_class_from_name( image, "Sharp", "surfacedata_t");
	mSoundParameters = mono_class_from_name( image, "Sharp", "CSoundParameters");

	SharpObject sharpSurfaceProps = sharpSurfacePropsClass.NewInstance();
	((SharpIPhysicsSurfaceProps*) sharpSurfaceProps.m_monoObject)->physprops = physprops;

	SharpObject sharpSoundEmitter = sharpSoundEmitterClass.NewInstance();
	((SharpSoundEmitterBase*) sharpSoundEmitter.m_monoObject)->emitter = soundemitterbase;


	SharpClass physicsSharp = mono_class_from_name( image, "Sharp","Physics");

	MonoVTable* engineSharpVTable = mono_class_vtable( g_Sharp.GetDomain(), physicsSharp.m_monoClass );
	
	Assert( engineSharpVTable );

	MonoClassField* engineSharpField = mono_class_get_field_from_name(physicsSharp.m_monoClass, "SurfaceProps" );
	Assert( engineSharpField );
	mono_field_static_set_value( engineSharpVTable, engineSharpField, sharpSurfaceProps.m_monoObject );


	MonoClassField* soundParamsSharpField = mono_class_get_field_from_name(physicsSharp.m_monoClass, "SoundEmitterBase" );
	Assert( soundParamsSharpField );
	mono_field_static_set_value( engineSharpVTable, soundParamsSharpField, sharpSoundEmitter.m_monoObject );
}

