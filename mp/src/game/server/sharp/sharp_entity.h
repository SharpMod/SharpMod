#pragma once

#include "cbase.h"
#include "sharp/sharp.h"

class CSharpEntity : public CBaseAnimating, public IMotionEvent
{
public:
	DECLARE_CLASS( CSharpEntity, CBaseAnimating );
	DECLARE_SERVERCLASS();
	//DECLARE_DATADESC();

	CSharpEntity( SharpClass sharpClass ) : m_monoClass(sharpClass){
	};
	~CSharpEntity();

	virtual SharpClass GetMonoClass(){ return m_monoClass; };
	virtual	bool	IsSharpEntity( void ) const { return true; }

	virtual void PostConstructor( const char *szClassname );
	virtual void	PhysicsSimulate( void );
	virtual void Think( void );
	virtual void Precache( void );

	IPhysicsMotionController	*m_pMotionController;
	IMotionEvent::simresult_e Simulate( IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular );

	//CNetworkVar( string_t, m_iSharpClassname );

private:
	SharpClass m_monoClass;
};

#include "modelentities.h"

class CSharpBrushEntity : public CBaseEntity
{
public:
	DECLARE_CLASS( CSharpBrushEntity, CBaseEntity );
	//DECLARE_SERVERCLASS();

	CSharpBrushEntity( SharpClass sharpClass ) : m_monoClass(sharpClass){
	};

	virtual SharpClass GetMonoClass(){ return m_monoClass; };
	virtual	bool	IsSharpEntity( void ) const { return true; }

	virtual void PostConstructor( const char *szClassname );
	virtual void Spawn( void );

	//CNetworkVar( string_t, m_iSharpClassname );

private:
	SharpClass m_monoClass;
};

