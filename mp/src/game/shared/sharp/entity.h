#pragma once

#include "sharp/sharp.h"

#if !defined( CLIENT_DLL )
#include "sharp/sharp_entity.h"
#else
#include "sharp/c_sharp_entity.h"
#endif
#include <mono/metadata/class.h>
#include "sharp/sharphelper.h"
#include "takedamageinfo.h"

#if defined( CLIENT_DLL )
#define CSharpEntity C_SharpEntity
#endif

struct EntityMonoObject {
	MonoObject object;
	CBaseEntity *entity;

public:
	CBaseEntity* GetEntity();
	CBaseAnimating* GetAnimating();
	CBasePlayer* GetPlayer();
};

struct PhysicsMonoObject {
	MonoObject object;
	IPhysicsObject *physics;
};

struct SharpDamageInfo : public MonoObject {
public:
	Vector DamageForce;
	Vector DamagePosition;
    Vector ReportedPosition;

    EntityMonoObject* Inflictor;
    EntityMonoObject* Attacker;
    EntityMonoObject* Weapon;

	float Damage;
	float MaxDamage;
	float BaseDamage;

	int DamageType;
	int DamageCustom;
	int DamgeStats;
	int AmmoType;
	int DamageOtherPlayers;
	int PlayerPenetrationCount;
	float DamageBonus;
};

class SharpEntity : public IMonoAddon {

public:
	SharpEntity();
	~SharpEntity();

	virtual void RegisterInternalCalls();
	virtual void UpdateMehods( MonoImage* image );

	SharpObject NewEntity( CBaseEntity* entity, SharpClass sharpClass );

	void FireSpawn( CBaseEntity* pEntity );
	bool FireKeyValue( CBaseEntity* pEntity, const char *szKeyName, const char *szValue );
	void FireThink( CSharpEntity* pEntity );
	void FirePhysicsSimulate( CSharpEntity* pEntity );
#ifdef CLIENT_DLL
	void FireReceiveMessage(CBaseEntity* pEntity, bf_read &msg );
#else
	bool FireAcceptInput( CBaseEntity* pEntity, const char *szInputName, CBaseEntity *pActivator, CBaseEntity *pCaller, variant_t Value );
#endif

	bool ShouldDraw( CBaseEntity* pEntity );
	void StartTouch( CBaseEntity* pEntity, CBaseEntity* otherEntity );
#ifndef CLIENT_DLL
	void FireSharpCollisionEvent( CBaseEntity* pEntity, int index, gamevcollisionevent_t *pEvent );
	void FireInitialSpawn( CBasePlayer* player );
#else
	SharpClass m_mClientBaseAnimatingClass;
#endif
	SharpClass m_mEntityClass;
	SharpClass m_mPlayerClass;
	SharpClass m_mPhysicsClass;

private:
	SharpMethod m_InitializeMethod;
	SharpMethod m_SpawnMethod;
	SharpMethod m_ThinkMethod;
	SharpMethod m_PhysicsSimulateMethod;
	SharpMethod m_ReceiveMessageMethod;
	SharpMethod m_InitializeEntityDT;
	SharpMethod m_CallKeyValue;
	SharpMethod m_StartTouch;
	SharpMethod m_CallShouldDraw;
#ifndef CLIENT_DLL
	SharpMethod m_PhysicsCollision;
	SharpMethod m_InitialSpawnMethod;
	SharpMethod m_FireAcceptInputMethod;	
#endif

};

extern SharpEntity* g_SharpEntity;

#ifndef CLIENT_DLL
struct SharpEntityCollissionData
{
	int surfaceProp;
	Vector preVelocity;
	Vector postVelocity;
	Vector preAngularVelocity;
	MonoObject* entity;
};

struct SharpCollisionEvent
{
	SharpEntityCollissionData data;
	SharpEntityCollissionData otherData;
	float deltaTime;
	bool isCollision;
	bool isShadowCollision;
	float collisionSpeed;
	Vector contactNormal;
	Vector contactPoint;
	Vector contactSpeed;
};


void ToSharpCollisionData(gamevcollisionevent_t* pEvent, int index, SharpCollisionEvent &pEventOut); 
#endif

//-----------------------------------------------------------------------------
// Converts an entity to a sharp entity
//-----------------------------------------------------------------------------
inline CSharpEntity *ToBaseSharp( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsSharpEntity() )
		return NULL;
#if _DEBUG
	return dynamic_cast<CSharpEntity *>( pEntity );
#else
	return static_cast<CSharpEntity *>( pEntity );
#endif
}

SharpDamageInfo* SharpNewDamageInfo(const CTakeDamageInfo &info);
CTakeDamageInfo SharpGetDamageInfo(const SharpDamageInfo* sharpInfo);