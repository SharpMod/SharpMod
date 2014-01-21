#include "cbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class SharpMath : public IMonoAddon {

public:

	SharpMath(){
		SharpAddons()->AddAddon( this );
	}

	virtual void RegisterInternalCalls();
	virtual void UpdateMehods( MonoImage* image ){};
};

SharpMath sharpMath;

static void QAngleAngleVectors( const QAngle angle, Vector* forward, Vector* right, Vector* up )
{
	AngleVectors( angle, forward, right, up );
}

static Vector VectorRandomValue()
{
	return RandomVector( -1.0f, 1.0f );
}

static void VMatrixIdentity( VMatrix* matrix )
{
	matrix->Identity();
}

static bool VMatrixIsIdentity ( VMatrix* matrix )
{
	return matrix->IsIdentity();
}

static Vector VMatrixVMul4x3 ( VMatrix* matrix, Vector vIn )
{
	return matrix->VMul4x3( vIn );
}

static float MathXSin( float val ){
	return sin( val );
}

static float MathXCos( float val ){
	return cos( val );
}

static void MathXSinCos( float val, float* s, float* c ){
	SinCos( val, s, c );
}

void SharpMath::RegisterInternalCalls(){
	
	mono_add_internal_call ("Sharp.QAngle::AngleVectors", QAngleAngleVectors );
	mono_add_internal_call ("Sharp.Vector::Random", VectorRandomValue );

	mono_add_internal_call ("Sharp.VMatrix::Identity", VMatrixIdentity );
	mono_add_internal_call ("Sharp.VMatrix::IsIdentity", VMatrixIsIdentity );
	mono_add_internal_call ("Sharp.VMatrix::VMul4x3", VMatrixVMul4x3 );

	mono_add_internal_call ("Sharp.MathX::Sin", MathXSin );
	mono_add_internal_call ("Sharp.MathX::Cos", MathXCos );
	mono_add_internal_call ("Sharp.MathX::SinCos", MathXSinCos );
}


