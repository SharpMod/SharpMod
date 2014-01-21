#pragma once

#include "sharp/sharp.h"

class SharpGame : public IMonoAddon {

public:
	SharpGame(){
		SharpAddons()->AddAddon( this );	
	};

	void CalcPlayerView( CBasePlayer* player, Vector& eyeOrigin, QAngle& eyeAngles, float& fov );

	void GameThink();

	virtual void RegisterInternalCalls( void );
	virtual void UpdateMehods( MonoImage* image );

protected:
	
	SharpClass m_gameClass;
	SharpMethod m_CalcViewMethod;
	SharpMethod m_thinkMethod;

};

extern SharpGame* g_SharpGame;
