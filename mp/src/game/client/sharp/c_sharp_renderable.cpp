#include "cbase.h"
#include "sharp/entitylistner.h"
#include "sharp/c_sharp_entity.h"
#include "sharp/sharphelper.h"
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


class SharpMesRenderable : public IMonoAddon {
public:
	SharpMesRenderable();

	virtual void RegisterInternalCalls();
	virtual void UpdateMehods( MonoImage* image );

	SharpMethod m_renderBoundsMethod;
	SharpMethod m_shouldDrawMethod;
	SharpMethod m_drawModelMethod;

};

SharpMesRenderable sharpRenderable;


SharpMesRenderable::SharpMesRenderable()
{
	SharpAddons()->AddAddon( this );
}


class RenderableSharpObject;


class CSharpRenderable: public CDefaultClientRenderable
{

public:
	CSharpRenderable( SharpObject object );


// IClientRenderable overrides.
public:		

	virtual const Vector&			GetRenderOrigin( void );
	virtual const QAngle&			GetRenderAngles( void );
	virtual const matrix3x4_t &		RenderableToWorldTransform();
	virtual void					GetRenderBounds( Vector& mins, Vector& maxs );
	virtual bool					ShouldDraw( void );
	virtual int						DrawModel( int flags ) OVERRIDE;
	bool					IsTransparent( void ) OVERRIDE;

//Methods to call in sharp code
public:
	RenderableSharpObject* GetSharpObject();
	SharpHandle m_monoHandle;

	SharpMethod m_renderOriginMethod;
	SharpMethod m_renderAnglesMethod;
	SharpMethod m_renderBoundsMethod;
	SharpMethod m_shouldDrawMethod;
	SharpMethod m_drawModelMethod;


};

class RenderableSharpObject : public TemplateSharpObject<CSharpRenderable> {
public:
	Vector m_Origin;
	QAngle m_Angles;
};

CSharpRenderable::CSharpRenderable( SharpObject object )
{
	m_renderBoundsMethod = object.GetVirtual( sharpRenderable.m_renderBoundsMethod );
	m_shouldDrawMethod = object.GetVirtual( sharpRenderable.m_shouldDrawMethod );
	m_drawModelMethod = object.GetVirtual( sharpRenderable.m_drawModelMethod );

	m_monoHandle = object.GetNewHandle();
}

RenderableSharpObject* CSharpRenderable::GetSharpObject()
{
	return (RenderableSharpObject*) m_monoHandle.GetObject().m_monoObject;
}

const matrix3x4_t &	CSharpRenderable::RenderableToWorldTransform()
{
	static matrix3x4_t mat;
	SetIdentityMatrix( mat );
	PositionMatrix( GetRenderOrigin(), mat );
	return mat;
}

const Vector& CSharpRenderable::GetRenderOrigin( void )
{
	return GetSharpObject()->m_Origin;
}

const QAngle& CSharpRenderable::GetRenderAngles( void )
{
	return GetSharpObject()->m_Angles;
}

bool CSharpRenderable::IsTransparent( void )
{
	return false;
}



void CSharpRenderable::GetRenderBounds( Vector& mins, Vector& maxs )
{
	UPDATE_DOMAIN();

	void* params[2] = { &mins, &maxs };
	m_renderBoundsMethod.SafeInvoke( GetSharpObject(), params );
}

bool CSharpRenderable::ShouldDraw( void )
{
	UPDATE_DOMAIN();

	MonoObject* exception;
	SharpObject retn = m_shouldDrawMethod.Invoke( GetSharpObject(), NULL, &exception);
		
	if (exception) {
		mono_unhandled_exception( exception );
		return false;
	}

	return  *((bool*) retn.Unbox());
}

int	CSharpRenderable::DrawModel( int flags )
{
	UPDATE_DOMAIN();

	MonoObject* exception;
	SharpObject retn = m_drawModelMethod.Invoke( GetSharpObject(), NULL, &exception );  // mono_runtime_invoke (m_drawModelMethod, GetSharpObject(), NULL, &exception);
		
	if (exception) {
		mono_unhandled_exception( exception );
		return false;
	}

	int* retint = (int*) (retn.Unbox());
	return *retint;
}

static void CreateRenderableObject ( RenderableSharpObject *monoRenderable ) {
	ASSERT_DOMAIN();
	CSharpRenderable* renderable = new CSharpRenderable( monoRenderable );
	
	//TODO: Allow user to change render type
	ClientLeafSystem()->AddRenderable( renderable, RENDER_GROUP_OPAQUE_ENTITY );
};


static void DestroyRenderableObject ( RenderableSharpObject *monoRenderable ) {
	ASSERT_DOMAIN();
	CSharpRenderable* renderable = monoRenderable->Get();

	ClientLeafSystem()->RemoveRenderable( renderable->m_hRenderHandle );
	monoRenderable->Set( NULL );

	delete renderable;

};


void SharpMesRenderable::UpdateMehods( MonoImage* image ){

	SharpClass renderableClass = mono_class_from_name( image, "Sharp", "DefaultClientRenderable");

	m_renderBoundsMethod = renderableClass.GetMethod( "GetRenderBounds", 2 );
	m_shouldDrawMethod = renderableClass.GetMethod( "ShouldDraw", 0 );
	m_drawModelMethod = renderableClass.GetMethod( "DrawModel", 0 );

};

void SharpMesRenderable::RegisterInternalCalls(){
		mono_add_internal_call ("Sharp.DefaultClientRenderable::.ctor", CreateRenderableObject );

		mono_add_internal_call ("Sharp.DefaultClientRenderable::Dispose", DestroyRenderableObject );

};