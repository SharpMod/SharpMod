#include "cbase.h"
#include "sharp/sharp.h"
#include "engine/IEngineSound.h"
#include "sharp/sharphelper.h"
#ifndef CLIENT_DLL
#include "sharp/sharp_usermessage.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

struct SharpEngineSound {
	MonoObject object;
	IEngineSound* sound;
};

class SharpEngine: public IMonoAddon {

public:
	SharpEngine(){
		SharpAddons()->AddAddon( this );
	}

	virtual void RegisterInternalCalls(){};
	virtual void UpdateMehods( MonoImage* image );


};

SharpEngine sharpEngine;


void SharpEngine::UpdateMehods( MonoImage* image ){

	MonoClass* engineSoundClass = mono_class_from_name( image, "Sharp","EngineSound" );
	Assert( engineSoundClass != nullptr );
	MonoObject* sharpSound = mono_object_new (g_Sharp.GetDomain(), engineSoundClass );

	((SharpEngineSound*) sharpSound)->sound = enginesound;

	SharpClass engineSharp = mono_class_from_name( image, "Sharp","Engine");
	MonoVTable* engineSharpVTable = mono_class_vtable( g_Sharp.GetDomain(), engineSharp.m_monoClass );
	MonoClassField* engineSharpField = mono_class_get_field_from_name(engineSharp.m_monoClass, "Sound" );

	Assert( engineSharpVTable );
	Assert( engineSharpField );

	mono_field_static_set_value( engineSharpVTable, engineSharpField, sharpSound );
}


static bool PrecacheSound( SharpEngineSound* sharpSound, MonoString* pSample, bool bPreload, bool bIsUISound )
{
	if (pSample == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("sample"));

	char* sound = mono_string_to_utf8( pSample );

	bool retn = sharpSound->sound->PrecacheSound(sound, bPreload, bIsUISound );

	mono_free( sound );
	return retn;
}
static SharpMethodItem PrecacheSoundItem("Sharp.EngineSound::PrecacheSound", PrecacheSound);

static bool IsSoundPrecached( SharpEngineSound* sharpSound, MonoString* pSample )
{
	if (pSample == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("sample"));

	char* sound = mono_string_to_utf8( pSample );

	bool retn = sharpSound->sound->IsSoundPrecached(sound);

	mono_free( sound );
	return retn;
}
static SharpMethodItem IsSoundPrecachedItem("Sharp.EngineSound::IsSoundPrecached", IsSoundPrecached);

static void PrefetchSound( SharpEngineSound* sharpSound, MonoString* pSample )
{
	if (pSample == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("sample"));

	char* sound = mono_string_to_utf8( pSample );
	sharpSound->sound->PrefetchSound(sound);
	mono_free( sound );
}
static SharpMethodItem PrefetchSoundItem("Sharp.EngineSound::PrefetchSound", PrefetchSound);

static bool IsLoopingSound( SharpEngineSound* sharpSound, MonoString* pSample )
{
	if (pSample == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("sample"));

	char* sound = mono_string_to_utf8( pSample );

	bool retn = false; // sharpSound->sound->IsLoopingSound(sound);

	mono_free( sound );
	return retn;
}
static SharpMethodItem IsLoopingSoundItem("Sharp.EngineSound::IsLoopingSound", IsLoopingSound);

static float GetSoundDuration( SharpEngineSound* sharpSound, MonoString* pSample )
{
	if (pSample == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("sample"));

	char* sound = mono_string_to_utf8( pSample );

	float retn = 0; //sharpSound->sound->IsLoopingSound(sound);

	mono_free( sound );
	return retn;
}
static SharpMethodItem GetSoundDurationItem("Sharp.EngineSound::GetSoundDuration", GetSoundDuration);

struct NullableVector{
	Vector vec;
	MonoBoolean hasValue;
};

static void EmitSound( SharpEngineSound* sharpSound, MonoObject* monoFilter, int iEntIndex, int iChannel, MonoString* pSample,
            float flVolume, soundlevel_t iSoundLevel, int iFlags, int iPitch, NullableVector pOrigin )
{
	if (monoFilter == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("filter"));

#ifdef CLIENT_DLL
	CReliableBroadcastRecipientFilter filter;
#else
	SharpRecipientFilter filter;
	filter.SetFilter( monoFilter );
#endif

	char* sound = mono_string_to_utf8( pSample );
	Vector* origin = pOrigin.hasValue ? &pOrigin.vec : NULL;

	sharpSound->sound->EmitSound(filter, iEntIndex, iChannel, sound, flVolume, iSoundLevel, iFlags, iPitch, 0, origin );

	mono_free( sound );
}
static SharpMethodItem EmitSoundItem("Sharp.EngineSound::EmitSound", EmitSound);

static void StopSound( SharpEngineSound* sharpSound, int iEntIndex, int iChannel, MonoString* pSample )
{
	if (pSample == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("sample"));

	char* sound = mono_string_to_utf8( pSample );
	sharpSound->sound->StopSound(iEntIndex, iChannel, sound);
	mono_free( sound );
}
static SharpMethodItem StopSoundItem("Sharp.EngineSound::StopSound", StopSound);



#ifndef CLIENT_DLL
static MonoArray* Message_DetermineMulticastRecipients( Vector origin, bool usepas )
{
	CBitVec< ABSOLUTE_PLAYER_LIMIT > playerbits;
	engine->Message_DetermineMulticastRecipients( true, origin, playerbits );

	MonoArray* monoArray = mono_array_new( g_Sharp.GetDomain(), mono_get_int32_class(), 2 );

	for( int i = 0; i < 2; i++ ){
		 mono_array_set( monoArray, int, i, playerbits.Base()[i] );
	}

	int index = playerbits.FindNextSetBit( 0 );

	while ( index > -1 )
	{
		//Msg("Found index from playerbits:\t %d\n", index );
		index = playerbits.FindNextSetBit( index + 1 );
	}

	return monoArray;
}
static SharpMethodItem DetermineMulticastRecipientsItem("Sharp.ServerEngine::Message_DetermineMulticastRecipients", Message_DetermineMulticastRecipients);
#endif
