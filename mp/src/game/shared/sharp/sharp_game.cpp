#include "cbase.h"
#include "sharp/sharp_game.h"
#include "sharp/sharp.h"
#include "sharp/entity.h"
#include "IEffects.h"
#include "sharp/sharp_usermessage.h"

#if !defined( CLIENT_DLL )
#include "entitylist.h"
#include "world.h"
#else
#include "cliententitylist.h"
#include "c_world.h"
#endif


static float GetGameCurTime()
{
	return gpGlobals->curtime;
}

static float GetGameFrameTime()
{
	return gpGlobals->frametime;
}

static int GetGameMaxClients()
{
	return gpGlobals->maxClients;
}

static MonoObject* GetWorldSharpEntity()
{
	ASSERT_DOMAIN();
#if !defined( CLIENT_DLL )
	CBaseEntity* world = GetWorldEntity();
#else
	CBaseEntity* world = GetClientWorldEntity();
#endif

	return world->MonoHandle.GetObject().m_monoObject;
}

#if !defined( CLIENT_DLL )
static MonoObject* GetNextEntity( EntityMonoObject* monoEntity ){
	ASSERT_DOMAIN();
	CBaseEntity* entity = NULL;

	if( monoEntity != NULL )
	{
		entity = monoEntity->entity;
		if( entity == NULL ){
			mono_raise_exception ( mono_get_exception_invalid_operation("Entity is not valid") );
		}
	}

	while( (entity = gEntList.NextEnt( entity )) != NULL ){
		if( entity->MonoHandle.IsValid() )
			return entity->MonoHandle.GetObject().m_monoObject;
	}

	return NULL;
}

static void ChangeLevel( MonoString* monoLevel )
{
	ASSERT_DOMAIN();

	char* level = mono_string_to_utf8( monoLevel );

	engine->ChangeLevel( level, NULL );

	mono_free( level );
}

#else

static MonoObject* GetLocalPlayer()
{
	ASSERT_DOMAIN();
	C_BasePlayer* player = C_BasePlayer::GetLocalPlayer();

	if( player == NULL || !player->MonoHandle.IsValid() )
		return NULL;

	return player->GetSharpEntity();
}

#endif

SharpGame sharpGame;
SharpGame* g_SharpGame = &sharpGame;

void SharpGame::RegisterInternalCalls( void ){


	mono_add_internal_call ("Sharp.Game::get_CurTime", GetGameCurTime );
	mono_add_internal_call ("Sharp.Game::get_FrameTime", GetGameFrameTime );
	mono_add_internal_call ("Sharp.Game::get_MaxClients", GetGameMaxClients );

	mono_add_internal_call ("Sharp.Game::GetWorldEntity", GetWorldSharpEntity );
#if !defined( CLIENT_DLL )
	mono_add_internal_call ("Sharp.Game::GetNextEntity", GetNextEntity );
	mono_add_internal_call ("Sharp.Game::ChangeLevel", ChangeLevel );
#else
	mono_add_internal_call ("Sharp.Player::GetLocalPlayer", GetLocalPlayer );
#endif

}


void SharpGame::UpdateMehods( MonoImage* image )
{
	m_gameClass = mono_class_from_name( image, "Sharp", "Game");

	m_CalcViewMethod =  m_gameClass.GetMethod( "CalcPlayerView", 4 );
	m_thinkMethod = m_gameClass.GetMethod( "GameThink", 0 );
}

void SharpGame::GameThink()
{
	UPDATE_DOMAIN();

	m_thinkMethod.SafeInvoke( NULL );
}

void SharpGame::CalcPlayerView( CBasePlayer* player, Vector& eyeOrigin, QAngle& eyeAngles, float& fov )
{
	UPDATE_DOMAIN();

	if( !player->MonoHandle.IsValid() )
		return;

	EntityMonoObject* ply = (EntityMonoObject*) player->MonoHandle.GetObject().m_monoObject;

	void* vars[4] = { ply, &eyeOrigin, &eyeAngles, &fov };

	m_CalcViewMethod.SafeInvoke( vars );

}

//TODO: Look at CTempEntsSystem for more particle effects
class SharpParticleInternal : public SharpInternalVariable<IEffects> {
public:
	SharpParticleInternal() : SharpInternalVariable<IEffects>("Sharp","ParticleUtil","_ParticleSystem"){
	}

	virtual IEffects* LoadValue() override {
		return g_pEffects;
	}
};

static SharpParticleInternal SharpParticle;


static void SharpSparks( Vector pos, int nMagnitude, int nTrailLength, Vector pDir )
{
	IEffects* tempSystem = SharpParticle.Get();
	tempSystem->Sparks( pos, nMagnitude, nTrailLength, &pDir );
}
static SharpMethodItem SharpSparksItem("Sharp.ParticleUtil::Sparks", SharpSparks );

static void SharpMuzzleFlash( const Vector vecOrigin, const QAngle vecAngles, float flScale, int iType  )
{
	IEffects* tempSystem = SharpParticle.Get();
	tempSystem->MuzzleFlash( vecOrigin, vecAngles, flScale, iType );
}
static SharpMethodItem SharpMuzzleFlashItem("Sharp.ParticleUtil::MuzzleFlash", SharpMuzzleFlash );


static void SharpSmoke( const Vector origin, int modelIndex, float scale, float framerate  )
{
	IEffects* tempSystem = SharpParticle.Get();
	tempSystem->Smoke( origin, modelIndex, scale, framerate );
}
static SharpMethodItem SharpSmokeItem("Sharp.ParticleUtil::Smoke", SharpSmoke );

static void SharpDust( const Vector pos, const Vector dir, float size, float speed  )
{
	IEffects* tempSystem = SharpParticle.Get();
	tempSystem->Dust( pos, dir, size, speed );
}
static SharpMethodItem SharpDustItem("Sharp.ParticleUtil::Dust", SharpDust );