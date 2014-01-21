//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef VPHYSICS_SOUND_H
#define VPHYSICS_SOUND_H
#ifdef _WIN32
#pragma once
#endif

#include "SoundEmitterSystem/isoundemittersystembase.h"
#include "sharp/sharphelper.h"

class SharpPhysicsSound : public IMonoAddon {
public:
	SharpPhysicsSound(){
		SharpAddons()->AddAddon( this );
	}

	virtual void RegisterInternalCalls(){};
	virtual void UpdateMehods( MonoImage* image );

	SharpClass m_impactClass;
	SharpMethod m_impactConstructor;
	SharpMethod m_addImpact;
	SharpMethod m_playImpact;

	SharpClass m_breakClass;
	SharpMethod m_breakConstructor;
	SharpMethod m_addBreak;
	SharpMethod m_playBreak;
};

void SharpPhysicsSound::UpdateMehods( MonoImage* image )
{

	//MonoImage* corLib( mono_get_corlib() );
	//m_listClass = corLib.GetClass("System.Collections.Generic","List`1");


	m_impactClass = mono_class_from_name(image, "Sharp","PhysicsImpactSound");
	m_impactConstructor = m_impactClass.GetMethod(".ctor", 0);
	m_addImpact = m_impactClass.GetMethod("AddImpactSound", 7);
	m_playImpact = m_impactClass.GetMethod("PlayImpactSounds", 0);

	m_breakClass = mono_class_from_name(image, "Sharp","PhysicsBreakSound");
	m_breakConstructor = m_breakClass.GetMethod(".ctor", 0);
	m_addBreak = m_breakClass.GetMethod("AddBreakSound", 2);
	m_playBreak = m_breakClass.GetMethod("PlayBreakSounds", 0);

	int size = mono_class_instance_size(  mono_class_from_name(image, "Sharp", "surfacedata_t") ) - sizeof(MonoObject);
	int size2 = sizeof(surfacedata_t);

	Assert(size == size2);
}

SharpPhysicsSound g_sharpPhysicsSound;

namespace physicssound
{
	struct impactsound_t
	{
		void			*pGameData;
		int				entityIndex;
		int				soundChannel;
		float			volume;
		float			impactSpeed;
		unsigned short	surfaceProps;
		unsigned short	surfacePropsHit;
		Vector			origin;
	};

	// UNDONE: Use a sorted container and sort by volume/distance?
	struct soundlist_t
	{
		SharpHandle m_handle;

		void InitSharp(){
			Assert( !m_handle.IsValid() );

			SharpObject obj = g_sharpPhysicsSound.m_impactClass.NewInstance();
			
			if( !g_sharpPhysicsSound.m_impactConstructor.SafeInvoke( obj, nullptr ) )
				return;

			m_handle = obj.GetNewHandle();

		};
		void ShutdownSharp(){
			if( m_handle.IsValid() )
				m_handle.Release();
		};

		/*
		CUtlVector<impactsound_t>	elements;
		impactsound_t	&GetElement(int index) { return elements[index]; }
		impactsound_t	&AddElement() { return elements[elements.AddToTail()]; }
		int Count() { return elements.Count(); }
		void RemoveAll() { elements.RemoveAll(); }
		*/
	};

	void PlayImpactSounds( soundlist_t &list )
	{
		if( list.m_handle.IsValid() ){
			g_sharpPhysicsSound.m_playImpact.SafeInvoke( list.m_handle.GetObject(), NULL, NULL );
		}

		
		/*
		for ( int i = list.Count()-1; i >= 0; --i )
		{
			impactsound_t &sound = list.GetElement(i);
			const surfacedata_t *psurf = physprops->GetSurfaceData( sound.surfaceProps );
			if ( psurf->sounds.impactHard )
			{
				const surfacedata_t *pHit = physprops->GetSurfaceData( sound.surfacePropsHit );
				unsigned short soundName = psurf->sounds.impactHard;
				if ( pHit && psurf->sounds.impactSoft )
				{
					if ( pHit->audio.hardnessFactor < psurf->audio.hardThreshold || 
						(psurf->audio.hardVelocityThreshold > 0 && psurf->audio.hardVelocityThreshold > sound.impactSpeed) )
					{
						soundName = psurf->sounds.impactSoft;
					}
				}
				const char *pSound = physprops->GetString( soundName );

				CSoundParameters params;
				if ( !CBaseEntity::GetParametersForSound( pSound, params, NULL ) )
					break;

				if ( sound.volume > 1 )
					sound.volume = 1;
				CPASAttenuationFilter filter( sound.origin, params.soundlevel );
				// JAY: If this entity gets deleted, the sound comes out at the world origin
				// this sounds bad!  Play on ent 0 for now.
				EmitSound_t ep;
				ep.m_nChannel = sound.soundChannel;
				ep.m_pSoundName = params.soundname;
				ep.m_flVolume = params.volume * sound.volume;
				ep.m_SoundLevel = params.soundlevel;
				ep.m_nPitch = params.pitch;
				ep.m_pOrigin = &sound.origin;

				CBaseEntity::EmitSound( filter, 0 /*sound.entityIndex*//*, ep );
			}
		}
		*/
		//list.RemoveAll();
	}

	void AddImpactSound( soundlist_t &list, void *pGameData, int entityIndex, int soundChannel, IPhysicsObject *pObject, int surfaceProps, int surfacePropsHit, float volume, float impactSpeed )
	{
		if( !list.m_handle.IsValid() )
			return;

		PhysicsMonoObject* physObj = NULL;
		
		if( pObject != NULL ){
			physObj = (PhysicsMonoObject*) g_SharpEntity->m_mPhysicsClass.NewInstance().m_monoObject;
			physObj->physics = pObject;
		}

		void* args[] = {
			&entityIndex,
			&soundChannel,
			physObj,
			&surfaceProps, 
			&surfacePropsHit, 
			&volume, 
			&impactSpeed
		};

		g_sharpPhysicsSound.m_addImpact.SafeInvoke( list.m_handle.GetObject(), args );

		/*
		impactSpeed += 1e-4;
		for ( int i = list.Count()-1; i >= 0; --i )
		{
			impactsound_t &sound = list.GetElement(i);
			// UNDONE: Compare entity or channel somehow?
			// UNDONE: Doing one slot per entity is too noisy.  So now we use one slot per material

			// heuristic - after 4 impacts sounds in one frame, start merging everything
			if ( surfaceProps == sound.surfaceProps || list.Count() > 4 )
			{
				// UNDONE: Store instance volume separate from aggregate volume and compare that?
				if ( volume > sound.volume )
				{
					pObject->GetPosition( &sound.origin, NULL );
					sound.pGameData = pGameData;
					sound.entityIndex = entityIndex;
					sound.soundChannel = soundChannel;
					sound.surfacePropsHit = surfacePropsHit;
				}
				sound.volume += volume;
				sound.impactSpeed = MAX(impactSpeed,sound.impactSpeed);
				return;
			}
		}

		impactsound_t &sound = list.AddElement();
		sound.pGameData = pGameData;
		sound.entityIndex = entityIndex;
		sound.soundChannel = soundChannel;
		pObject->GetPosition( &sound.origin, NULL );
		sound.surfaceProps = surfaceProps;
		sound.surfacePropsHit = surfacePropsHit;
		sound.volume = volume;
		sound.impactSpeed = impactSpeed;
		*/
	}

	struct breaksoundlist_t
	{
		SharpHandle m_handle;

		void InitSharp(){
			Assert( !m_handle.IsValid() );

			SharpObject obj = g_sharpPhysicsSound.m_breakClass.NewInstance();
			
			if( !g_sharpPhysicsSound.m_breakConstructor.SafeInvoke( obj, NULL, NULL ) )
				return;

			m_handle = obj.GetNewHandle();

		};

		void ShutdownSharp(){
			if( m_handle.IsValid() )
				m_handle.Release();
		};

		/*
		CUtlVector<impactsound_t>	elements;
		impactsound_t	&GetElement(int index) { return elements[index]; }
		impactsound_t	&AddElement() { return elements[elements.AddToTail()]; }
		int Count() { return elements.Count(); }
		void RemoveAll() { elements.RemoveAll(); }
		*/
	};

	
	void AddBreakSound( breaksoundlist_t &list, const Vector &origin, unsigned short surfaceProps )
	{
		if( !list.m_handle.IsValid() )
			return;

		Vector pos( origin );

		void* args[] = {
			&pos,
			&surfaceProps
		};

		g_sharpPhysicsSound.m_addBreak.SafeInvoke( list.m_handle.GetObject(), args );
	}

	void PlayBreakSounds( breaksoundlist_t &list )
	{
		if( list.m_handle.IsValid() ){
			g_sharpPhysicsSound.m_playBreak.SafeInvoke( list.m_handle.GetObject(), NULL, NULL );
		}
	}

	/*

	struct breaksound_t
	{
		Vector			origin;
		int				surfacePropsBreak;
	};

	void AddBreakSound( CUtlVector<breaksound_t> &list, const Vector &origin, unsigned short surfaceProps )
	{
		const surfacedata_t *psurf = physprops->GetSurfaceData( surfaceProps );
		if ( !psurf->sounds.breakSound )
			return;

		for ( int i = list.Count()-1; i >= 0; --i )
		{
			breaksound_t &sound = list.Element(i);
			// Allow 3 break sounds before you start merging anything.
			if ( list.Count() > 2 && surfaceProps == sound.surfacePropsBreak )
			{
				sound.origin = (sound.origin + origin) * 0.5f;
				return;
			}
		}
		breaksound_t sound;
		sound.origin = origin;
		sound.surfacePropsBreak = surfaceProps;
		list.AddToTail(sound);

	}

	void PlayBreakSounds( CUtlVector<breaksound_t> &list )
	{
		for ( int i = list.Count()-1; i >= 0; --i )
		{
			breaksound_t &sound = list.Element(i);

			const surfacedata_t *psurf = physprops->GetSurfaceData( sound.surfacePropsBreak );
			const char *pSound = physprops->GetString( psurf->sounds.breakSound );
			CSoundParameters params;
			if ( !CBaseEntity::GetParametersForSound( pSound, params, NULL ) )
				return;

			// Play from the world, because the entity is breaking, so it'll be destroyed soon
			CPASAttenuationFilter filter( sound.origin, params.soundlevel );
			EmitSound_t ep;
			ep.m_nChannel = CHAN_STATIC;
			ep.m_pSoundName = params.soundname;
			ep.m_flVolume = params.volume;
			ep.m_SoundLevel = params.soundlevel;
			ep.m_nPitch = params.pitch;
			ep.m_pOrigin = &sound.origin;
			CBaseEntity::EmitSound( filter, 0 /*sound.entityIndex*//*, ep );
		}
		list.RemoveAll();
	}
	*/
};

#endif // VPHYSICS_SOUND_H
