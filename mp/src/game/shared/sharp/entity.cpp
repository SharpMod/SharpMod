#include "cbase.h"
#include "sharp/sharp.h"
#include "entity.h"
#include "sharp/sharphelper.h"
#include "sharp/sharp_usermessage.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

SharpEntity sharpEntity;
SharpEntity* g_SharpEntity = &sharpEntity;



static int GetEntityIndex ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return entity->entindex();
};

static Vector GetEntityPosition ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	return entity->GetAbsOrigin();
};

static void SetEntityPosition ( MonoObject* methods, EntityMonoObject *monoEntity, Vector position ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->SetAbsOrigin( position );
};

static QAngle GetAngles ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	return entity->GetAbsAngles();
};

static void SetAngles ( MonoObject* methods, EntityMonoObject *monoEntity, QAngle position ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->SetAbsAngles( position );
};

static Vector GetVelocity( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	Vector velocity = entity->GetAbsVelocity();

	return velocity;
};

static void SetVelocity( MonoObject* methods, EntityMonoObject *monoEntity, Vector velocity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	entity->SetAbsVelocity(velocity);
};

static MonoString* GetEntityClassname ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return mono_string_new(g_Sharp.GetDomain(), entity->GetClassname() );
};

static MonoString* GetEntityModel ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	const char* model = STRING( entity->GetModelName() );

	if( model == NULL )
		return NULL;

	return mono_string_new(g_Sharp.GetDomain(), model );
};

static void SetEntityModel ( MonoObject* methods, EntityMonoObject *monoEntity, MonoString* modelStr ) {
	ASSERT_DOMAIN();

	if (modelStr == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("model"));

	CBaseEntity* entity = monoEntity->GetEntity();

	char* model = mono_string_to_utf8( modelStr );

	entity->PrecacheModel( model );
	entity->SetModel( model );

	mono_free( model );
};

static MonoString* GetEntityName ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

#ifndef CLIENT_DLL
	const char* name = entity->GetEntityName().ToCStr();
#else
	const char* name = entity->GetClassname();
#endif

	if( !name )
		return NULL;

	return mono_string_new(g_Sharp.GetDomain(), name );
};

#ifndef CLIENT_DLL
static void SetEntityName ( MonoObject* methods, EntityMonoObject *monoEntity, MonoString* nameStr ) {
	ASSERT_DOMAIN();

	if (nameStr == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("name"));

	CBaseEntity* entity = monoEntity->GetEntity();

	char* model = mono_string_to_utf8( nameStr );

	entity->SetName( AllocPooledString( model ) );

	mono_free( model );
};
#endif


static MoveType_t GetEntityMoveType ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return entity->GetMoveType();
};

static void SetEntityMoveType ( MonoObject* methods, EntityMonoObject *monoEntity, MoveType_t movetype ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->SetMoveType( movetype );
};

static int GetEntityCollisionGroup ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return entity->GetCollisionGroup();
};

static void SetEntityCollisionGroup ( MonoObject* methods, EntityMonoObject *monoEntity, int collisionGroup ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->SetCollisionGroup( collisionGroup );
};



static int GetEntitySolidFlags ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return entity->GetSolidFlags();
};

static void SetEntitySolidFlags ( MonoObject* methods, EntityMonoObject *monoEntity, int solidFlags ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->SetSolidFlags( solidFlags );
};

static int GetEntityEffectFlags ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return entity->GetEffects();
};

static void SetEntityEffectFlags ( MonoObject* methods, EntityMonoObject *monoEntity, int solidFlags ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->SetEffects( solidFlags );
};


static SolidType_t GetEntitySolid ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return entity->GetSolid();
};

static void SetEntitySolid ( MonoObject* methods, EntityMonoObject *monoEntity, SolidType_t movetype ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->SetSolid( movetype );
};

static color32 GetEntityColor ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return entity->m_clrRender;
};

static void SetEntityColor ( MonoObject* methods, EntityMonoObject *monoEntity, color32 color ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->m_clrRender.Set( color );
};

/*
static RenderFx_t GetRenderFx ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return entity->GetRenderMode();
};

static void SetRenderFx ( MonoObject* methods, EntityMonoObject *monoEntity, RenderFx_t renderFx ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->SetRenderFX( renderFx );
};
*/

static RenderMode_t GetRenderMode ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return entity->GetRenderMode();
};

static void SetRenderMode ( MonoObject* methods, EntityMonoObject *monoEntity, RenderMode_t renderMode ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->SetRenderMode( renderMode );
};

static float GetNextThink ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return entity->GetNextThink();
};

static void SetNextThink ( MonoObject* methods, EntityMonoObject *monoEntity, float thinkTime ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->SetNextThink( thinkTime );
};

static int EntityGetTeam ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return entity->GetTeamNumber();
};

static void EntitySetTeam ( MonoObject* methods, EntityMonoObject *monoEntity, int teamNum ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->ChangeTeam( teamNum );
};

static int EntityGetSkin ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();

	return entity->m_nSkin;
};

static void EntitySetSkin ( MonoObject* methods, EntityMonoObject *monoEntity, int skin ) {
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	
	entity->m_nSkin = skin;
};

static MonoObject* GetEntityOwner ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	CBaseEntity* owner = entity->GetOwnerEntity();

	if( owner == NULL || !owner->MonoHandle.IsValid() )
		return NULL;

	return owner->MonoHandle.GetObject().m_monoObject;
};

static void SetEntityOwner( MonoObject* methods, EntityMonoObject* monoEntity, EntityMonoObject* ownerEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	if( ownerEntity == NULL ){
		entity->SetOwnerEntity( NULL );
		return;
	}

	CBaseEntity* owner = ownerEntity->GetEntity();
	entity->SetOwnerEntity( owner );
};

static MonoObject* GetEntityParent ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	CBaseEntity* parent = entity->GetMoveParent();

	if( parent == NULL )
		return NULL;

	return parent->MonoHandle.GetObject().m_monoObject;
};

static void SetEntityParent( MonoObject* methods, EntityMonoObject* monoEntity, EntityMonoObject* parentEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	if( parentEntity == NULL ){
		entity->SetParent(NULL);
		return;
	}

	CBaseEntity* parent = parentEntity->GetEntity();
	entity->SetParent( parent );
};

static PhysicsMonoObject* GetEntityPhysics ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	IPhysicsObject* phys = entity->VPhysicsGetObject();

	if( phys == NULL )
		return NULL;

	PhysicsMonoObject* physObj = (PhysicsMonoObject*) g_SharpEntity->m_mPhysicsClass.NewInstance().m_monoObject;
	physObj->physics = phys;

	return physObj;
};

static Vector GetEntityWorldAlignMins ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	return entity->WorldAlignMins();
};

static Vector GetEntityWorldAlignMaxs ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	return entity->WorldAlignMaxs();
};


static void SetCollisionBounds ( MonoObject* methods, EntityMonoObject *monoEntity, Vector mins, Vector maxs ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	entity->SetCollisionBounds( mins, maxs );
};

struct NullableFloat{
	float value;
	MonoBoolean hasValue;
};

struct NullableVector{
	Vector vec;
	MonoBoolean hasValue;
};

struct NullableQAngle{
	QAngle angle;
	MonoBoolean hasValue;
};

#include "engine/IEngineSound.h"

class SharpEmitSound {
public:
	int Channel;
	MonoString* SoundName;
	float Volume;
	int SoundLevel;
	int Flags;
	int Pitch;
	int SpecialDSP;
	NullableVector Origin;
	float SoundTime;
	NullableFloat SoundDuration;
	MonoBoolean EmitCloseCaption;
	MonoBoolean WarnOnMissingCloseCaption;
	MonoBoolean WarnOnDirectWaveReference;
	int SpeakerEntity;
};

static void EntityEmitSound( MonoObject* methods, MonoObject* recipientFilter, int entityId, SharpEmitSound params ) {
	ASSERT_DOMAIN();

	if (recipientFilter == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("recipientFilter"));

	SharpRecipientFilter filter;
	filter.SetFilter(recipientFilter);

	EmitSound_t ep;
	char* name = nullptr;

	if(params.SoundDuration.hasValue)
		ep.m_pflSoundDuration = &params.SoundDuration.value;

	if(params.Origin.hasValue)
		ep.m_pOrigin = &params.Origin.vec;

	if(params.SoundName != nullptr )
		name = mono_string_to_utf8(params.SoundName);

	
	ep.m_nChannel = params.Channel;
	ep.m_flVolume = params.Volume;
	ep.m_SoundLevel = (soundlevel_t) clamp(params.SoundLevel, 0, 255);
	ep.m_nFlags = params.Flags;
	ep.m_nPitch = params.Pitch;
	ep.m_nSpecialDSP = params.SpecialDSP;
	ep.m_pSoundName = name;	
	ep.m_flSoundTime = params.SoundTime; 
	ep.m_bEmitCloseCaption = params.EmitCloseCaption != 0;
	ep.m_bWarnOnMissingCloseCaption = params.WarnOnMissingCloseCaption != 0;
	ep.m_bWarnOnDirectWaveReference = params.WarnOnDirectWaveReference != 0;
	ep.m_nSpeakerEntity = params.SpeakerEntity;

	CBaseEntity::EmitSound( filter, entityId, ep );

	if( name != nullptr )
		mono_free(name);
	
};

static void EntityStopSound( MonoObject* methods, EntityMonoObject *monoEntity, MonoString* soundStr ) {
	ASSERT_DOMAIN();

	if (soundStr == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("sound"));

	CBaseEntity* entity = monoEntity->GetEntity();
	char* sound = mono_string_to_utf8( soundStr );

	entity->StopSound( sound );

	mono_free( sound );
};

#ifndef CLIENT_DLL
static void EntityInitPhysics ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->VPhysicsInitNormal( SOLID_VPHYSICS, 0, false );
};

static void EntityInitSpherePhysics ( MonoObject* methods, EntityMonoObject *monoEntity, float radius ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->SetSolid( SOLID_BBOX );
	entity->SetCollisionBounds( -Vector(radius,radius,radius), Vector(radius,radius,radius) );
	
	objectparams_t params = g_PhysDefaultObjectParams;
	params.pGameData = static_cast<void *>(entity);
	IPhysicsObject *pPhysicsObject = physenv->CreateSphereObject( radius, 0, entity->GetAbsOrigin(), entity->GetAbsAngles(), &params, false );
	if ( pPhysicsObject )
	{
		entity->VPhysicsSetObject( pPhysicsObject );
		entity->SetMoveType( MOVETYPE_VPHYSICS );
		pPhysicsObject->Wake();
	}

};

static void EntityCreateMotionController ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	if( !entity->IsSharpEntity() ){
		mono_raise_exception ( mono_get_exception_invalid_operation("Entity is not a sharp entity") );
	}

	CSharpEntity* sharpEnt = ToBaseSharp( entity );

	if( sharpEnt->m_pMotionController != NULL ){
		mono_raise_exception ( mono_get_exception_invalid_operation("Entity already has a motion controller") );
	}

	sharpEnt->m_pMotionController = physenv->CreateMotionController( sharpEnt );
	sharpEnt->m_pMotionController->AttachObject( sharpEnt->VPhysicsGetObject(), true );


};

static void EntityTeleport ( MonoObject* methods, EntityMonoObject *monoEntity, NullableVector newPosition, NullableQAngle newAngles, NullableVector newVelocity ) {
	ASSERT_DOMAIN();
	
	CBaseEntity* entity = monoEntity->GetEntity();
	Vector *position = newPosition.hasValue ? &newPosition.vec : NULL;
	Vector *velocity = newVelocity.hasValue ? &newVelocity.vec : NULL;
	QAngle* angles = newAngles.hasValue ? &newAngles.angle : NULL;
	
	entity->Teleport( position, angles, velocity );
};

static void EntityTouch ( MonoObject* methods, EntityMonoObject *monoEntity, EntityMonoObject *monoOtherEntity ) {
	ASSERT_DOMAIN();
	
	CBaseEntity* entity = monoEntity->GetEntity();
	CBaseEntity* other = monoOtherEntity->entity == nullptr ? nullptr : monoOtherEntity->GetEntity();

	entity->Touch( other );
};





static void EntityNetworkStateChanged( MonoObject* methods, EntityMonoObject *monoEntity ) 
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	entity->NetworkStateChanged();
};

static void PlayerRemoveAllItems( MonoObject* methods, EntityMonoObject *monoEntity, bool removeSuit ) 
{
	ASSERT_DOMAIN();

	CBasePlayer* player = monoEntity->GetPlayer();

	player->RemoveAllItems( removeSuit );
};

#else

#include "gamestringpool.h" 
static MonoObject* CreateClientEntity(MonoString* modelName, bool bRenderWithViewModels)
{
	ASSERT_DOMAIN();
	C_BaseAnimating *pTemp = new C_BaseAnimating;
	char* model = mono_string_to_utf8( modelName );

	//pTemp->PrecacheModel( model );
	//const model_t* modelInfo = engine->LoadModel(model);

	if ( pTemp->InitializeAsClientEntity( model, RENDER_GROUP_OPAQUE_ENTITY ) == false )
	{	
		// we failed to initialize this model so just skip it
		pTemp->Remove();
		pTemp = NULL;
	} 
	else 
	{
		g_SharpEntity->NewEntity( pTemp, g_SharpEntity->m_mClientBaseAnimatingClass );
		Assert( pTemp->MonoHandle.IsValid() );
		pTemp->UpdateVisibility();
	}

	mono_free( model );
	return pTemp == NULL ? NULL : pTemp->GetSharpEntity();
}

#endif

static MonoObject* GetObserverTarget ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetPlayer()->GetObserverTarget();

	if( entity == NULL )
		return NULL;

	return entity->MonoHandle.GetObject().m_monoObject;
};

static void SetObserverTarget( MonoObject* methods, EntityMonoObject* monoEntity, EntityMonoObject* parentEntity ) 
{
	ASSERT_DOMAIN();
	CBasePlayer* player = monoEntity->GetPlayer();

	if( parentEntity == NULL ){
		player->SetObserverTarget( NULL );
		return;
	}

	CBaseEntity* target = parentEntity->GetEntity();
	player->SetObserverTarget( target );
};

static MonoObject* GetPlayerWeapon( MonoObject* methods, EntityMonoObject* monoEntity, int wepId )
{
	ASSERT_DOMAIN();
	if( wepId < 0 || wepId >= MAX_WEAPONS )
		mono_raise_exception( mono_get_exception_index_out_of_range() );

	CBasePlayer* player = monoEntity->GetPlayer();

	CBaseCombatWeapon* wep = player->GetWeapon(wepId);

	if( wep == nullptr )
		return nullptr;
	
	return wep->MonoHandle.GetObject().m_monoObject;
}

static void SetSpectatorMovement ( MonoObject* methods, EntityMonoObject *monoEntity, int specMovement ) {
	ASSERT_DOMAIN();
#ifndef CLIENT_DLL
	CBasePlayer* player = monoEntity->GetPlayer();

	if( specMovement == OBS_MODE_NONE )
		player->StopObserverMode();
	else
		player->StartObserverMode( specMovement );
#else
	mono_raise_exception ( mono_get_exception_invalid_operation("Can not set observer mode from the client.") );
#endif
};

static int GetSpectatorMovement ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBasePlayer* player = monoEntity->GetPlayer();

	return player->GetObserverMode();
};

static void EntityGetVectors( MonoObject* methods, EntityMonoObject *monoEntity, Vector* forward, Vector* right, Vector* up )
{
	ASSERT_DOMAIN();
	monoEntity->GetEntity()->GetVectors( forward, right, up );
}

static void EntityDispose( MonoObject* methods, EntityMonoObject *monoEntity )
{
	ASSERT_DOMAIN();
	Assert(monoEntity->entity != nullptr);
	CBaseEntity* entity = monoEntity->GetEntity();

	if(!entity->MonoHandle.IsValid() )
		return;

	entity->MonoHandle.Release();
	monoEntity->entity = NULL;

	entity->Remove();
}

static MonoString* GetPlayerName ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBasePlayer* player = monoEntity->GetPlayer();

	return mono_string_new(g_Sharp.GetDomain(), player->GetPlayerName() );
};

static QAngle GetPlayerEyeAngles ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBasePlayer* player = monoEntity->GetPlayer();

	return player->EyeAngles();
};

static int GetPlayerButtons ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBasePlayer* player = monoEntity->GetPlayer();

	return player->m_nButtons;
};

static int GetPlayerButtonsLast ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBasePlayer* player = monoEntity->GetPlayer();

	return player->m_afButtonLast;
};

static int GetPlayerButtonsPressed ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBasePlayer* player = monoEntity->GetPlayer();

	return player->m_afButtonPressed;
};

static int GetPlayerButtonsReleased ( MonoObject* methods, EntityMonoObject *monoEntity ) {
	ASSERT_DOMAIN();
	CBasePlayer* player = monoEntity->GetPlayer();

	return player->m_afButtonReleased;
};

static Vector GetPlayerEyePosition ( MonoObject* methods, EntityMonoObject *monoEntity )
{
	ASSERT_DOMAIN();
	CBasePlayer* player = monoEntity->GetPlayer();

	return player->EyePosition();
};

static int LookupSequence( MonoObject* methods, EntityMonoObject *monoEntity, MonoString* name )
{
	ASSERT_DOMAIN();
	
	if (name == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("name"));

	CBaseAnimating* entity = monoEntity->GetAnimating();
	char* seqName = mono_string_to_utf8( name );

	int seq = entity->LookupSequence(seqName);

	mono_free(seqName);
	return seq;
}

static int PrecacheModel( MonoObject* methods, MonoString* szModel ){
	ASSERT_DOMAIN();
	char* model = mono_string_to_utf8( szModel );

	int id = CBaseEntity::PrecacheModel( model );

	mono_free( model );
	return id;
}

static void ResetSequenceInfo( MonoObject* methods, EntityMonoObject *monoEntity)
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	entity->ResetSequenceInfo();
}

static void SetCycle( MonoObject* methods, EntityMonoObject *monoEntity, float cycle)
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	entity->SetCycle(cycle);
}

static float GetCycle( MonoObject* methods, EntityMonoObject *monoEntity)
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	return entity->GetCycle();
}

static void SetBodyGroup( MonoObject* methods, EntityMonoObject *monoEntity, int iGroup, int iValue)
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	entity->SetBodygroup(iGroup, iValue);
}

static int GetBodyGroup( MonoObject* methods, EntityMonoObject *monoEntity, int iGroup)
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	return entity->GetBodygroup(iGroup);
}

static int GetSequence( MonoObject* methods, EntityMonoObject *monoEntity)
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	return entity->GetSequence();
}

static void SetSequence( MonoObject* methods, EntityMonoObject *monoEntity, int sequence)
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	entity->SetSequence(sequence);
}

static void ResetSequence( MonoObject* methods, EntityMonoObject *monoEntity, int sequence)
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	entity->ResetSequence(sequence);
}


static bool IsSequenceLooping( MonoObject* methods, EntityMonoObject *monoEntity, int sequence)
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	return entity->IsSequenceLooping(sequence);
}

static bool SequenceLoops( MonoObject* methods, EntityMonoObject *monoEntity)
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	return entity->SequenceLoops();
}

static void SetPlaybackRate( MonoObject* methods, EntityMonoObject *monoEntity, float rate)
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	entity->SetPlaybackRate(rate);
}

static float GetPlaybackRate( MonoObject* methods, EntityMonoObject *monoEntity)
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	return entity->GetPlaybackRate();
}

static int LookupPoseParameter( MonoObject* methods, EntityMonoObject *monoEntity, MonoString* szName )
{
	ASSERT_DOMAIN();

	if (szName == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("name"));

	CBaseAnimating* entity = monoEntity->GetAnimating();
	char* name = mono_string_to_utf8( szName );

	int pose = entity->LookupPoseParameter(name);

	mono_free( name );

	return pose;
}

static float SetPoseParameter( MonoObject* methods, EntityMonoObject *monoEntity, int iParameter, float flValue )
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	return entity->SetPoseParameter( iParameter, flValue );
}

static float GetPoseParameter( MonoObject* methods, EntityMonoObject *monoEntity, int iParameter )
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	return entity->GetPoseParameter( iParameter );
}

#ifdef CLIENT_DLL
static float FrameAdvance( MonoObject* methods, EntityMonoObject *monoEntity, float flInterval )
{
	ASSERT_DOMAIN();
	CBaseAnimating* entity = monoEntity->GetAnimating();
	return entity->FrameAdvance( flInterval );
}
#endif

static MonoObject* GetPlayerByIndex( int index )
{
	ASSERT_DOMAIN();
	CBasePlayer* player = UTIL_PlayerByIndex( index );

	if( !player )
		return NULL;

	if( player->MonoHandle.IsValid() ){
		return player->MonoHandle.GetObject().m_monoObject;
	}

	return NULL;
}

SharpEntity::SharpEntity(){
	SharpAddons()->AddAddon( this );
}

SharpEntity::~SharpEntity(){
	
}

#ifdef CLIENT_DLL
static int SharpLookupAttachment(MonoObject* methods, EntityMonoObject *monoEntity, MonoString* monoName)
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	char* name = mono_string_to_utf8( monoName );

	int attachment = entity->LookupAttachment( name );
	mono_free(name);

	return attachment;	
}
static SharpMethodItem SharpLookupAttachmentItem("Sharp.ClientEntity::LookupAttachment", SharpLookupAttachment );

static bool SharpGetAttachment(MonoObject* methods, EntityMonoObject *monoEntity, int number, Vector &origin, QAngle &angles )
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();

	return entity->GetAttachment(number, origin, angles);	
}
static SharpMethodItem SharpGetAttachmentItem("Sharp.ClientEntity::GetAttachment", SharpGetAttachment );


#endif

#ifndef CLIENT_DLL
static int GetSpawnFlags(MonoObject* methods, EntityMonoObject *monoEntity)
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	return entity->GetSpawnFlags();	
}
static SharpMethodItem GetSpawnFlagsItem("Sharp.ServerEntity::GetSpawnFlags", GetSpawnFlags );

static void SetSpawnFlags(MonoObject* methods, EntityMonoObject *monoEntity, int flags)
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	entity->ClearSpawnFlags();
	entity->AddSpawnFlags(flags);	
}
static SharpMethodItem SetSpawnFlagstItem("Sharp.ServerEntity::SetSpawnFlags", SetSpawnFlags );

static int SharpGetArmor(MonoObject* methods, EntityMonoObject *monoEntity)
{
	ASSERT_DOMAIN();
	CBasePlayer* entity = monoEntity->GetPlayer();
	return entity->ArmorValue();	
}
static SharpMethodItem SharpGetArmorItem("Sharp."MONO_CLASS"Player::GetArmor", SharpGetArmor );

static void SharpSetArmor(MonoObject* methods, EntityMonoObject *monoEntity, int armor)
{
	ASSERT_DOMAIN();
	CBasePlayer* entity = monoEntity->GetPlayer();
	entity->SetArmorValue(armor);
}
static SharpMethodItem SharpSetArmorItem("Sharp."MONO_CLASS"Player::SetArmor", SharpSetArmor );


static void SharpFireNamedOutput(MonoObject* methods, EntityMonoObject *monoEntity, MonoString* outputName, EntityMonoObject *monoActivator, EntityMonoObject *monoCaller, float delay)
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	CBaseEntity* activator = nullptr;
	CBaseEntity* caller = nullptr;
	char* pszOutput = mono_string_to_utf8( outputName );

	if( monoActivator != nullptr )
		activator = monoActivator->GetEntity();

	if( monoCaller != nullptr )
		caller = monoCaller->GetEntity();
	
	variant_t Val;
	Val.Set( FIELD_VOID, NULL );

	entity->FireNamedOutput(pszOutput, Val, activator, caller, delay );

	mono_free( pszOutput );
}
static SharpMethodItem SharpFireNamedOutputItem("Sharp.ServerEntity::FireNamedOutput", SharpFireNamedOutput );

#include <eventqueue.h>

static void SharpTriggerOutput(MonoString* target, MonoString* outputName, float delay, EntityMonoObject *monoActivator, EntityMonoObject *monoCaller )
{
	ASSERT_DOMAIN();
	char* targetName = mono_string_to_utf8( target );
	CBaseEntity* activator = nullptr;
	CBaseEntity* caller = nullptr;
	char* pszOutput = mono_string_to_utf8( outputName );

	if( monoActivator != nullptr )
		activator = monoActivator->GetEntity();

	if( monoCaller != nullptr )
		caller = monoCaller->GetEntity();
	
	variant_t Val;
	Val.Set( FIELD_VOID, NULL );

	//entity->FireNamedOutput(pszOutput, Val, activator, caller, delay );

	//TODO: Memory leak. Stupid event system expects strings on static memory.
	char* a = strdup(targetName);
	char* b = strdup(pszOutput);

	g_EventQueue.AddEvent( a, b, Val, delay, activator, caller );

	mono_free( pszOutput );
	mono_free( targetName );
}
static SharpMethodItem SharpTriggerOutputItem("Sharp.EventQueue::AddEvent", SharpTriggerOutput );

static void SharpTriggerOutputEntity(EntityMonoObject *monoEntity, MonoString* outputName, float delay, EntityMonoObject *monoActivator, EntityMonoObject *monoCaller )
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	CBaseEntity* activator = nullptr;
	CBaseEntity* caller = nullptr;
	char* pszOutput = mono_string_to_utf8( outputName );

	if( monoActivator != nullptr )
		activator = monoActivator->GetEntity();

	if( monoCaller != nullptr )
		caller = monoCaller->GetEntity();
	
	//variant_t Val;
	//Val.Set( FIELD_VOID, NULL );

	//entity->FireNamedOutput(pszOutput, Val, activator, caller, delay );

	g_EventQueue.AddEvent( entity, pszOutput, delay, activator, caller );

	mono_free( pszOutput );
}
static SharpMethodItem SharpTriggerOutputEntityItem("Sharp.EventQueue::AddEventEntity", SharpTriggerOutputEntity );
#endif

static int SharpGetHealth(MonoObject* methods, EntityMonoObject *monoEntity)
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	return entity->GetHealth();	
}
static SharpMethodItem SharpGetHealthItem("Sharp."MONO_CLASS"Entity::GetHealth", SharpGetHealth );

static void SharpSetHealth(MonoObject* methods, EntityMonoObject *monoEntity, int health)
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	entity->SetHealth(health);
}
static SharpMethodItem SharpSetHealthItem("Sharp."MONO_CLASS"Entity::SetHealth", SharpSetHealth );

static int SharpGetMaxHealth(MonoObject* methods, EntityMonoObject *monoEntity)
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	return entity->GetMaxHealth();	
}
static SharpMethodItem SharpGetMaxHealthItem("Sharp."MONO_CLASS"Entity::GetMaxHealth", SharpGetMaxHealth );

static void SharpSetMaxHealth(MonoObject* methods, EntityMonoObject *monoEntity, int health)
{
	ASSERT_DOMAIN();
#ifndef CLIENT_DLL
	CBaseEntity* entity = monoEntity->GetEntity();
	entity->SetMaxHealth(health);
#endif
}
static SharpMethodItem SharpSetMaxHealthItem("Sharp."MONO_CLASS"Entity::SetMaxHealth", SharpSetMaxHealth );


static int SharpGetLifeState(MonoObject* methods, EntityMonoObject *monoEntity)
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	return entity->m_lifeState;	
}
static SharpMethodItem SharpGetLifeStateItem("Sharp."MONO_CLASS"Entity::GetLifeState", SharpGetLifeState );

static void SharpSetLifeState(MonoObject* methods, EntityMonoObject *monoEntity, int state)
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	entity->m_lifeState = state;
}
static SharpMethodItem SharpSetLifeStateItem("Sharp."MONO_CLASS"Entity::SetLifeState", SharpSetLifeState );


static int SharpGetEntityState(MonoObject* methods, EntityMonoObject *monoEntity)
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	return entity->m_fFlags;	
}
static SharpMethodItem SharpGetEntityStateItem("Sharp."MONO_CLASS"Entity::GetFlags", SharpGetEntityState );

static void SharpSetEntityState(MonoObject* methods, EntityMonoObject *monoEntity, int flags)
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	entity->m_fFlags = flags;
}
static SharpMethodItem SharpSetEntityStateItem("Sharp."MONO_CLASS"Entity::SetFlags", SharpSetEntityState );



static MonoString* SharpPlayerConVar(MonoObject* methods, EntityMonoObject *monoEntity, MonoString*  monoName )
{
	ASSERT_DOMAIN();
	CBasePlayer* player = monoEntity->GetPlayer();
	char* name = mono_string_to_utf8(monoName);
	const char* retn = "";

#ifndef CLIENT_DLL
	retn = engine->GetClientConVarValue( engine->IndexOfEdict( player->edict() ), name );
#else
	if( C_BasePlayer::GetLocalPlayer() == player)
	{
		ConVar* convar = cvar->FindVar(name);
		if(convar != nullptr)
			retn = convar->GetString();
	}
#endif

	mono_free(name);
	return mono_string_new(g_Sharp.GetDomain(), retn);
}
static SharpMethodItem SharpPlayerConVarValue("Sharp."MONO_CLASS"Player::GetConVarValue", SharpPlayerConVar );

static MonoObject* SharpPlayerGetActiveWeapon(MonoObject* methods, EntityMonoObject *monoEntity, MonoString*  monoName )
{
	ASSERT_DOMAIN();
	CBasePlayer* player = monoEntity->GetPlayer();
	CBaseCombatWeapon* wep = player->GetActiveWeapon();

	if( wep == nullptr)
		return nullptr;

	return wep->GetSharpEntity();
}
static SharpMethodItem SharpPlayerGetActiveWeaponItem("Sharp."MONO_CLASS"Player::GetActiveWeapon", SharpPlayerGetActiveWeapon );


static void SharpEntityAABB(MonoObject* methods, EntityMonoObject *monoEntity, Vector* min, Vector* max )
{
	ASSERT_DOMAIN();
	CBaseEntity* entity = monoEntity->GetEntity();
	
	entity->CollisionProp()->WorldSpaceAABB(min, max);

}
static SharpMethodItem SharpEntityAABBValue("Sharp."MONO_CLASS"Entity::GetAABB", SharpEntityAABB );


static MonoArray* SharpGetEntitiesInBox(MonoObject* methods, Vector mins, Vector maxs, int maxEnts, int flagMask ){
	ASSERT_DOMAIN();
	if(maxEnts < 0 || maxEnts > MAX_EDICTS)
		mono_raise_exception (mono_get_exception_index_out_of_range());

	std::vector<CBaseEntity*> list(maxEnts);
	int count = UTIL_EntitiesInBox( &list.front(), 256, mins, maxs, flagMask );

	MonoArray* arr = mono_array_new( g_Sharp.GetDomain(), g_SharpEntity->m_mEntityClass.m_monoClass, count );

	for( int i = 0 ; i < count ; i++ )
		mono_array_setref( arr, i, list[i]->GetSharpEntity() );
	
	return arr;
}
static SharpMethodItem SharpGetEntitiesInBoxItem("Sharp."MONO_CLASS"Entity::GetEntitiesInBox", SharpGetEntitiesInBox );


void SharpEntity::RegisterInternalCalls(){
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetIndex", GetEntityIndex);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetOrigin", GetEntityPosition);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetOrigin", SetEntityPosition);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetAngles", GetAngles);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetAngles", SetAngles);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetClassname", GetEntityClassname);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetModel", SetEntityModel);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetModel", GetEntityModel);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetName", GetEntityName);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetMoveType", SetEntityMoveType);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetMoveType", GetEntityMoveType);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetCollisionGroup", SetEntityCollisionGroup);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetCollisionGroup", GetEntityCollisionGroup);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetSolidFlags", SetEntitySolidFlags);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetSolidFlags", GetEntitySolidFlags);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetEffectFlags", SetEntityEffectFlags);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetEffectFlags", GetEntityEffectFlags);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetSolid", SetEntitySolid);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetSolid", GetEntitySolid);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetOwner", SetEntityOwner);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetOwner", GetEntityOwner);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetParent", SetEntityParent);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetParent", GetEntityParent);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetVelocity", SetVelocity);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetVelocity", GetVelocity);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetColor", SetEntityColor);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetColor", GetEntityColor);
	//mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetRenderFx", SetRenderFx);
	//mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetRenderFx", GetRenderFx);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetRenderMode", SetRenderMode);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetRenderMode", GetRenderMode);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetTeam", EntityGetTeam );
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetTeam", EntitySetTeam );
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetSkin", EntityGetSkin );
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetSkin", EntitySetSkin );
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetWorldAlignMins", GetEntityWorldAlignMins);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetWorldAlignMaxs", GetEntityWorldAlignMaxs);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetNextThink", SetNextThink);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetNextThink", GetNextThink);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetCollisionBounds", SetCollisionBounds);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetVectors", EntityGetVectors);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::Dispose", EntityDispose);

	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetPhysics", GetEntityPhysics);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::EmitSound", EntityEmitSound);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::StopSound", EntityStopSound);
	
#ifndef CLIENT_DLL
	mono_add_internal_call ("Sharp.ServerEntity::SetName", SetEntityName);
	mono_add_internal_call ("Sharp.ServerEntity::InitPhysics", EntityInitPhysics );
	mono_add_internal_call ("Sharp.ServerEntity::InitSpherePhysics", EntityInitSpherePhysics );
	mono_add_internal_call ("Sharp.ServerEntity::CreateMotionController", EntityCreateMotionController );
	mono_add_internal_call ("Sharp.ServerEntity::Teleport", EntityTeleport );
	mono_add_internal_call ("Sharp.ServerEntity::Touch", EntityTouch );

	mono_add_internal_call ("Sharp.ServerEntity::NetworkStateChanged", EntityNetworkStateChanged );
#endif

	mono_add_internal_call ("Sharp."MONO_CLASS"Player::GetName", GetPlayerName);	
	mono_add_internal_call ("Sharp."MONO_CLASS"Player::GetEyeAngles", GetPlayerEyeAngles);
	mono_add_internal_call ("Sharp."MONO_CLASS"Player::GetButtons", GetPlayerButtons);
	mono_add_internal_call ("Sharp."MONO_CLASS"Player::GetButtonsLast", GetPlayerButtonsLast);
	mono_add_internal_call ("Sharp."MONO_CLASS"Player::GetButtonsPressed", GetPlayerButtonsPressed);
	mono_add_internal_call ("Sharp."MONO_CLASS"Player::GetButtonsReleased", GetPlayerButtonsReleased);
	mono_add_internal_call ("Sharp."MONO_CLASS"Player::GetEyePosition", GetPlayerEyePosition);
	mono_add_internal_call ("Sharp."MONO_CLASS"Player::GetPlayerByIndex", GetPlayerByIndex );

	mono_add_internal_call ("Sharp."MONO_CLASS"Player::GetSpectatorMovement", GetSpectatorMovement);
	mono_add_internal_call ("Sharp."MONO_CLASS"Player::SetSpectatorMovement", SetSpectatorMovement );
	mono_add_internal_call ("Sharp."MONO_CLASS"Player::GetObserverTarget", GetObserverTarget);
	mono_add_internal_call ("Sharp."MONO_CLASS"Player::SetObserverTarget", SetObserverTarget );
	mono_add_internal_call ("Sharp."MONO_CLASS"Player::GetWeapon", GetPlayerWeapon);

	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::LookupSequence", LookupSequence);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::ResetSequenceInfo", ResetSequenceInfo);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetCycle", SetCycle);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetCycle", GetCycle);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetBodyGroup", SetBodyGroup);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetBodyGroup", GetBodyGroup);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetSequence", GetSequence);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetSequence", SetSequence);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::ResetSequence", ResetSequence);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::IsSequenceLooping", IsSequenceLooping);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SequenceLoops", SequenceLoops);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetPlaybackRate", SetPlaybackRate);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetPlaybackRate", GetPlaybackRate);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::LookupPoseParameter", LookupPoseParameter);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::SetPoseParameter", SetPoseParameter);
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::GetPoseParameter", GetPoseParameter);
#ifdef CLIENT_DLL
	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::FrameAdvance", FrameAdvance);
#endif

	mono_add_internal_call ("Sharp."MONO_CLASS"Entity::PrecacheModel", PrecacheModel);


#ifndef CLIENT_DLL
	mono_add_internal_call ("Sharp.ServerPlayer::RemoveAllItems", PlayerRemoveAllItems );
#else
	mono_add_internal_call ("Sharp.ClientBaseAnimating::CreateClientEntity", CreateClientEntity );
#endif
}

void SharpEntity::UpdateMehods( MonoImage* image ){

	m_mEntityClass = mono_class_from_name( image, "Sharp", "Entity");
	m_mPlayerClass = mono_class_from_name( image, "Sharp", "Player");
	m_mPhysicsClass = mono_class_from_name( image, "Sharp", "IPhysicsObject");

	m_InitializeMethod = m_mEntityClass.GetMethod( "Initialize", 0 );
	m_SpawnMethod =  m_mEntityClass.GetMethod( "Spawn", 0 );
	m_ThinkMethod = m_mEntityClass.GetMethod( "Think", 0 );
	m_PhysicsSimulateMethod = m_mEntityClass.GetMethod( "PhysicsSimulate", 0 );
	m_ReceiveMessageMethod = m_mEntityClass.GetMethod( "ReceiveMessage", 1 );
	m_CallKeyValue = m_mEntityClass.GetMethod("KeyValue", 2);
	m_StartTouch = m_mEntityClass.GetMethod( "StartTouch", 1 );
	m_CallShouldDraw = m_mEntityClass.GetMethod( "ShouldDraw", 0 );
#ifndef CLIENT_DLL
	m_PhysicsCollision = m_mEntityClass.GetMethod( "PhysicsCollision", 1 );
	m_InitialSpawnMethod = m_mPlayerClass.GetMethod( "InitialSpawn", 0 );
	m_FireAcceptInputMethod = m_mEntityClass.GetMethod( "AcceptInput", 3 );
#else
	m_mClientBaseAnimatingClass = mono_class_from_name( image, "Sharp", "ClientBaseAnimating"); 
#endif

	SharpClass entityInterfaceClass = mono_class_from_name( image, "Sharp", MONO_CLASS"Entity");
	SharpClass playerInterfaceClass = mono_class_from_name( image, "Sharp", MONO_CLASS"Player");

	SharpObject entityInterface = entityInterfaceClass.NewInstance();
	SharpObject playerInterface = playerInterfaceClass.NewInstance();


	void* args[1];
	SharpMethod setEntityInterface = m_mEntityClass.GetMethod( "SetInterface", 1 );
	SharpMethod setPlayerInterface = m_mPlayerClass.GetMethod( "SetInterface", 1 );

	args[0] = entityInterface.m_monoObject;
	setEntityInterface.SafeInvoke( args );

	args[0] = playerInterface.m_monoObject;
	setPlayerInterface.SafeInvoke( args );

	SharpClass sharpNetwork = mono_class_from_name( image, "Sharp", "EntityDT");
	m_InitializeEntityDT = sharpNetwork.GetMethod("InitializeEntity", 1);

}

SharpObject SharpEntity::NewEntity( CBaseEntity* entity, SharpClass sharpClass )
{
	UPDATE_DOMAIN();

	AssertMsg( !entity->MonoHandle.IsValid(), "Initializing Mono entity instance twice" );

	SharpObject entityInstance = sharpClass.NewInstance();
	EntityMonoObject* entityObject = (EntityMonoObject*) entityInstance.m_monoObject;
	entityObject->entity = entity;
	
	entity->MonoHandle = entityInstance.GetNewHandle();

	entityInstance.GetVirtual( m_InitializeMethod ).SafeInvoke( entityInstance, NULL );

	void* args[] = {
		entityInstance.m_monoObject
	};

	m_InitializeEntityDT.SafeInvoke( args );

	return entityInstance;
}

void SharpEntity::FireSpawn( CBaseEntity* pEntity )
{
	UPDATE_DOMAIN();

	if( pEntity->MonoHandle.IsValid() ){
		SharpObject obj = pEntity->MonoHandle.GetObject();
		obj.GetVirtual( m_SpawnMethod ).SafeInvoke( obj , NULL );
	}
}

#ifndef CLIENT_DLL
void SharpEntity::FireSharpCollisionEvent( CBaseEntity* pEntity, int index, gamevcollisionevent_t *pEvent )
{
	UPDATE_DOMAIN();

	SharpCollisionEvent pSharpEvent;
	ToSharpCollisionData( pEvent, index, pSharpEvent );

	void* args[] = {
		&pSharpEvent,
	};

	SharpObject obj = pEntity->MonoHandle.GetObject();
	obj.GetVirtual( m_PhysicsCollision ).SafeInvoke(obj , args);

}

void SharpEntity::FireInitialSpawn( CBasePlayer* pPlayer )
{
	UPDATE_DOMAIN();

	SharpObject obj = pPlayer->MonoHandle.GetObject();
	//obj.GetVirtual( m_InitialSpawnMethod ).SafeInvoke( obj , NULL );
	m_InitialSpawnMethod.SafeInvoke( obj , NULL );
}
#endif

bool SharpEntity::FireKeyValue( CBaseEntity* pEntity, const char *szKeyName, const char *szValue )
{
	UPDATE_DOMAIN();

	if( !pEntity->MonoHandle.IsValid() )
		return false;

	SharpObject obj = pEntity->MonoHandle.GetObject();
	MonoString* key = mono_string_new(g_Sharp.GetDomain(), szKeyName );
	MonoString* value = mono_string_new(g_Sharp.GetDomain(), szValue );
	SharpObject retn;

	void* args[] = {
		key,
		value
	};

	if( !obj.GetVirtual( m_CallKeyValue ).SafeInvoke( obj, args, &retn ) )
		return false;

	return *((bool*) retn.Unbox());
}


bool SharpEntity::ShouldDraw( CBaseEntity* pEntity )
{
	UPDATE_DOMAIN();

	if( !pEntity->MonoHandle.IsValid() )
		return false;

	SharpObject obj = pEntity->MonoHandle.GetObject();
	SharpObject retn;

	if( !obj.GetVirtual( m_CallShouldDraw ).SafeInvoke( obj, NULL, &retn ) )
		return false;

	return *((bool*) retn.Unbox());
}



void SharpEntity::StartTouch( CBaseEntity* pEntity, CBaseEntity* otherEntity )
{
	UPDATE_DOMAIN();

	SharpObject entity = pEntity->MonoHandle.GetObject();
	SharpObject other = otherEntity->MonoHandle.GetObject();

	void* args[] = {
		other.m_monoObject
	};

	entity.GetVirtual( m_StartTouch ).SafeInvoke( entity, args );

}

void SharpEntity::FireThink( CSharpEntity* pEntity )
{
	UPDATE_DOMAIN();
	SharpObject entity = pEntity->MonoHandle.GetObject();

	entity.GetVirtual( m_ThinkMethod ).SafeInvoke( entity, NULL );
}

void SharpEntity::FirePhysicsSimulate( CSharpEntity* pEntity )
{
	UPDATE_DOMAIN();

	SharpObject entity = pEntity->MonoHandle.GetObject();
	entity.GetVirtual( m_PhysicsSimulateMethod ).SafeInvoke( entity, NULL );
}

#ifdef CLIENT_DLL
#include "sharp/sharp_usermessage.h"

void SharpEntity::FireReceiveMessage(CBaseEntity* pEntity, bf_read &msg )
{
	UPDATE_DOMAIN();

	SharpObject entity = pEntity->MonoHandle.GetObject();

	BitReadSharpObject* bitRead = g_sharpUserMessage.NewBitReadSharpObject( msg );

	void* args[] = {
		bitRead
	};

	entity.GetVirtual( m_ReceiveMessageMethod ).SafeInvoke( entity, args );
}
#else
bool SharpEntity::FireAcceptInput( CBaseEntity* pEntity, const char *szInputName, CBaseEntity *pActivator, CBaseEntity *pCaller, variant_t Value )
{
	UPDATE_DOMAIN();

	SharpObject entity = pEntity->MonoHandle.GetObject();
	SharpObject retn;

	void* args[] = {
		mono_string_new( g_Sharp.GetDomain(), szInputName ),
		pActivator->MonoHandle.GetObject().m_monoObject,
		pCaller->MonoHandle.GetObject().m_monoObject
	};

	if( !entity.GetVirtual( m_FireAcceptInputMethod ).SafeInvoke( entity, args, &retn ) )
		return false;

	return *((bool*) retn.Unbox());
}

#endif


CBaseEntity* EntityMonoObject::GetEntity(){
	if( entity == NULL ){
		mono_raise_exception ( mono_get_exception_invalid_operation("Entity is not valid") );
	}
	return entity;
};

CBasePlayer* EntityMonoObject::GetPlayer(){
	CBasePlayer* player = ToBasePlayer( this->GetEntity() );
	if( player == NULL ){
		mono_raise_exception ( mono_get_exception_invalid_operation("Entity is not a player.") );
	}
	return player;
};

CBaseAnimating* EntityMonoObject::GetAnimating(){
	CBaseAnimating* animating = dynamic_cast<CBaseAnimating*>( this->GetEntity() );
	if( animating == NULL ){
		mono_raise_exception ( mono_get_exception_invalid_operation("Entity is not a CBaseAnimating.") );
	}
	return animating;
};

#ifndef CLIENT_DLL
void ToSharpCollisionData(gamevcollisionevent_t* pEvent, int index, SharpCollisionEvent &pEventOut)
{
	int otherIndex = !index;
	
	pEventOut.collisionSpeed = pEvent->collisionSpeed;
	pEventOut.deltaTime = pEvent->deltaCollisionTime;
	pEventOut.isCollision = pEvent->isCollision;
	pEventOut.isShadowCollision = pEvent->isShadowCollision;

	pEvent->pInternalData->GetContactPoint(pEventOut.contactPoint);
	pEvent->pInternalData->GetSurfaceNormal(pEventOut.contactNormal);
	pEvent->pInternalData->GetContactSpeed(pEventOut.contactSpeed);

	pEventOut.data.entity = pEvent->pEntities[index]->GetSharpEntity();
	pEventOut.data.surfaceProp = pEvent->surfaceProps[index];
	pEventOut.data.preVelocity = pEvent->preVelocity[index];
	pEventOut.data.postVelocity = pEvent->postVelocity[index];
	pEventOut.data.preAngularVelocity = pEvent->preAngularVelocity[index];

	pEventOut.otherData.entity = pEvent->pEntities[otherIndex]->GetSharpEntity();
	pEventOut.otherData.surfaceProp = pEvent->surfaceProps[otherIndex];
	pEventOut.otherData.preVelocity = pEvent->preVelocity[otherIndex];
	pEventOut.otherData.postVelocity = pEvent->postVelocity[otherIndex];
	pEventOut.otherData.preAngularVelocity = pEvent->preAngularVelocity[otherIndex];
}
#endif

SharpClassReference SharpDamageInfoReference("Sharp", "TakeDamageInfo");

SharpDamageInfo* SharpNewDamageInfo(const CTakeDamageInfo &info){
	MonoClass* sharpDamageClass = SharpDamageInfoReference.Get();
	SharpDamageInfo* sharpInfo = static_cast<SharpDamageInfo*>(mono_object_new( g_Sharp.GetDomain(), sharpDamageClass ));

	Assert(mono_class_instance_size( sharpDamageClass ) == sizeof(SharpDamageInfo) );

	sharpInfo->DamageForce = info.GetDamageForce();
	sharpInfo->DamagePosition = info.GetDamagePosition();
	sharpInfo->ReportedPosition = info.GetReportedPosition();

	if( info.GetInflictor() != nullptr )
		sharpInfo->Inflictor = (EntityMonoObject*)(info.GetInflictor()->GetSharpEntity());

	if( info.GetAttacker() != nullptr )
		sharpInfo->Attacker = (EntityMonoObject*)(info.GetAttacker()->GetSharpEntity());

	if( info.GetWeapon() != nullptr )
		sharpInfo->Weapon = (EntityMonoObject*)(info.GetWeapon()->GetSharpEntity());

	sharpInfo->Damage = info.GetDamage();
	sharpInfo->MaxDamage = info.GetMaxDamage();
	sharpInfo->BaseDamage = info.GetBaseDamage();

	sharpInfo->DamageType = info.GetDamageType();
	sharpInfo->DamageCustom = info.GetDamageCustom();
	sharpInfo->DamgeStats = info.GetDamageStats();
	sharpInfo->AmmoType = info.GetAmmoType();
	sharpInfo->DamageOtherPlayers = info.GetDamagedOtherPlayers();
	sharpInfo->PlayerPenetrationCount = info.GetPlayerPenetrationCount();
	sharpInfo->DamageBonus = info.GetDamageBonus();

	return sharpInfo;
}

CTakeDamageInfo SharpGetDamageInfo(const SharpDamageInfo* sharpInfo){
	
	CTakeDamageInfo info;

	info.SetDamageForce(sharpInfo->DamageForce);
	info.SetDamagePosition(sharpInfo->DamagePosition);
	info.SetReportedPosition(sharpInfo->ReportedPosition);

	if(sharpInfo->Inflictor != nullptr)
		info.SetInflictor( sharpInfo->Inflictor->GetEntity() );

	if(sharpInfo->Attacker != nullptr)
		info.SetAttacker( sharpInfo->Attacker->GetEntity() );

	if(sharpInfo->Weapon != nullptr)
		info.SetWeapon( sharpInfo->Weapon->GetEntity() );

	//info.SetDamage(sharpInfo->BaseDamage);
	//info.CopyDamageToBaseDamage();

	info.SetDamage(sharpInfo->Damage);
	info.SetMaxDamage(sharpInfo->MaxDamage);

	info.SetDamageType(sharpInfo->DamageType);
	info.SetDamageCustom(sharpInfo->DamageCustom);
	info.SetDamageStats(sharpInfo->DamgeStats);
	info.SetAmmoType(sharpInfo->AmmoType);
	info.SetDamagedOtherPlayers(sharpInfo->DamageOtherPlayers);
	info.SetPlayerPenetrationCount(sharpInfo->PlayerPenetrationCount);
	info.SetDamageBonus(sharpInfo->DamageBonus);

	return info;
}

#ifndef CLIENT_DLL
static void Getplayerinfosharp( const CCommand &args )
{
	CBasePlayer *pPlayer = UTIL_GetCommandClient();
	if ( !pPlayer )
		return;


}

static ConCommand plinfo( "sharp_plinfo", Getplayerinfosharp, "Toggle show triggers", FCVAR_CHEAT );
#endif