#include "cbase.h"
#include "sharp/sharp.h"
#include "sharp/entity.h"
#include "sharp/sharphelper.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//TODO (Nican): Add a check if the size of this object is equal the size of the internal sharp structure
typedef struct TraceResponse
{
	Vector StartPos;
    Vector EndPos; 
	float Fraction;
	int Contents;
	MonoBoolean AllSolid;
	MonoBoolean StartSolid;
	float FractionLeftSolid;
	int hitgroup;
	SharpObject HitEntity;
	cplane_t		Plane;
} TraceResponse;


static TraceResponse SharpTraceRay( Vector start, Vector end, unsigned int fMask, int collisionGroup, EntityMonoObject *monoEntity )
{
	ASSERT_DOMAIN();
	IHandleEntity* entity = NULL;

	if( monoEntity != NULL ){
		entity = monoEntity->GetEntity();
	}

	trace_t tr;
	UTIL_TraceLine( start, end, fMask, entity, collisionGroup, &tr );

	TraceResponse response;
	response.StartPos = tr.startpos;
	response.EndPos = tr.endpos;
	response.Fraction = tr.fraction;
	response.Contents = tr.contents;
	response.AllSolid = tr.allsolid;
	response.StartSolid = tr.startsolid;
	response.FractionLeftSolid = tr.fractionleftsolid;
	response.hitgroup = tr.hitgroup;
	response.Plane = tr.plane;

	if( tr.m_pEnt && tr.m_pEnt->MonoHandle.IsValid() ){
		response.HitEntity = tr.m_pEnt->MonoHandle.GetObject();
	}

	return response;
}


class SharpTrace : public IMonoAddon {

public:
	SharpTrace(){
		SharpAddons()->AddAddon( this );	
	};

	virtual void RegisterInternalCalls( void );
	virtual void UpdateMehods( MonoImage* image ){};

protected:
	
};
static SharpTrace sharpTrace;


void SharpTrace::RegisterInternalCalls( void ){

	mono_add_internal_call ("Sharp.EngineTrace::TraceRay"MONO_CLASS, SharpTraceRay);

}
