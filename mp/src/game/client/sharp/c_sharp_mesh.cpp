#include "cbase.h"
#include "sharp/entitylistner.h"
#include "sharp/sharphelper.h"
#include "sharp/c_sharp_entity.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class MeshSharpObject : public TemplateSharpObject<IMesh> {};
class MeshBuilderSharpObject : public TemplateSharpObject<CMeshBuilder> {};
class MaterialSharpObject : public TemplateSharpObject<IMaterial> {};


class SharpMeshManager : public IMonoAddon {
public:
	SharpMeshManager();

	virtual void RegisterInternalCalls();
	virtual void UpdateMehods( MonoImage* image );

	SharpClass m_meshClass;
	SharpClass m_materialClass;
};

SharpMeshManager sharpMesh;


SharpMeshManager::SharpMeshManager()
{
	SharpAddons()->AddAddon( this );
}

static MonoObject* GetDynamicMesh()
{
	ASSERT_DOMAIN();
	SharpObject obj = sharpMesh.m_meshClass.NewInstance();
	MeshSharpObject* meshObj = (MeshSharpObject*) obj.m_monoObject;

	CMatRenderContextPtr pRenderContext( g_pMaterialSystem );

	IMesh* mesh = pRenderContext->GetDynamicMesh();

	meshObj->Set(mesh);

	return obj.m_monoObject;
}

static void ContextBind( MaterialSharpObject* materialObj )
{
	ASSERT_DOMAIN();
	IMaterial* mat = materialObj->Get();
	//CMatRenderContextPtr pRenderContext( g_pMaterialSystem );
	CMatRenderContextPtr pRenderContext( materials );

	pRenderContext->Bind( mat );
}


static MonoObject* ContextFindMaterial(MonoString* str )
{
	ASSERT_DOMAIN();
	char* input = mono_string_to_utf8( str );

	IMaterial* material = g_pMaterialSystem->FindMaterial( input, TEXTURE_GROUP_OTHER );

	mono_free( input );

	material->AddRef();

	MaterialSharpObject* obj = (MaterialSharpObject*) sharpMesh.m_materialClass.NewInstance().m_monoObject;
	obj->Set( material );

	return obj;
}

static void DisposeMaterial( MaterialSharpObject* materialObj )
{
	ASSERT_DOMAIN();
	materialObj->Get()->Release();
	materialObj->Set(NULL);
}

static bool MaterialIsError( MaterialSharpObject* materialObj )
{
	ASSERT_DOMAIN();
	return materialObj->Get()->IsErrorMaterial();
}

static int MaterialGetActualWidth( MaterialSharpObject* materialObj )
{
	ASSERT_DOMAIN();
	return materialObj->Get()->GetMappingWidth();
}


static int MaterialGetActualHeight( MaterialSharpObject* materialObj )
{
	ASSERT_DOMAIN();
	return materialObj->Get()->GetMappingHeight();
}


static void CreateMeshBuilder( MeshBuilderSharpObject* builderObj, MeshSharpObject* meshObj, MaterialPrimitiveType_t type, int vertexCount )
{
	ASSERT_DOMAIN();
	IMesh* mesh = meshObj->Get();
	CMeshBuilder* meshBuilder = new CMeshBuilder();

	builderObj->Set( meshBuilder );

	meshBuilder->Begin( mesh, type, vertexCount );	
}

static void MeshBuilderPosition( MeshBuilderSharpObject* builderObj, Vector position )
{
	ASSERT_DOMAIN();
	builderObj->Get()->Position3f( position.x, position.y, position.z );
}

static void MeshBuilderColor( MeshBuilderSharpObject* builderObj, Color color )
{
	ASSERT_DOMAIN();
	builderObj->Get()->Color4ub( color.r(), color.g(), color.b(), color.a() );
}

static void MeshBuilderTexCoord( MeshBuilderSharpObject* builderObj, float s, float t)
{
	ASSERT_DOMAIN();
	builderObj->Get()->TexCoord2f( 0, s, t );
}

static void MeshBuilderAdvanceVertex( MeshBuilderSharpObject* builderObj)
{
	ASSERT_DOMAIN();
	builderObj->Get()->AdvanceVertex();
}

static void MeshBuilderEnd( MeshBuilderSharpObject* builderObj, bool shouldDraw)
{
	ASSERT_DOMAIN();
	builderObj->Get()->End( false, shouldDraw );
}

static void MeshBuilderDispose( MeshBuilderSharpObject* builderObj )
{
	ASSERT_DOMAIN();
	CMeshBuilder* builder = builderObj->Get();

	delete builder;

	builderObj->Set( NULL );
}

static void MeshDraw( MeshSharpObject* meshObj )
{
	ASSERT_DOMAIN();
	meshObj->Get()->Draw();
}

static void MeshDispose( MeshSharpObject* meshObj )
{
	ASSERT_DOMAIN();
	//??? Magic ???
	//Where is the memory released? DynamicMesh does some kind of magic.
}


static void SetMatrixMode(MaterialMatrixMode_t mode)
{
	ASSERT_DOMAIN();
	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->MatrixMode( mode );
}

static void GetMatrix(MaterialMatrixMode_t mode, VMatrix* matrix )
{
	ASSERT_DOMAIN();
	CMatRenderContextPtr pRenderContext( materials );
	return pRenderContext->GetMatrix( mode, matrix );
}

static void LoadIdentity()
{
	ASSERT_DOMAIN();
	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->LoadIdentity();
}

static void LoadMatrix( VMatrix matrix )
{
	ASSERT_DOMAIN();
	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->LoadMatrix( matrix );
}

static void Ortho( double left, double top, double right, double bottom, double zNear, double zFar )
{
	ASSERT_DOMAIN();
	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->Ortho( left, top, right, bottom, zNear, zFar );
}

static void EnableUserClipTransformOverride( bool bEnable )
{
	ASSERT_DOMAIN();
	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->EnableUserClipTransformOverride( bEnable );
}

static void UserClipTransform( VMatrix worldToView )
{
	ASSERT_DOMAIN();
	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->UserClipTransform( worldToView );
}




void SharpMeshManager::RegisterInternalCalls(){
		mono_add_internal_call ("Sharp.Mesh::Draw", MeshDraw );
		mono_add_internal_call ("Sharp.Mesh::Dispose", MeshDispose );
		
		mono_add_internal_call ("Sharp.MeshBuilder::.ctor", CreateMeshBuilder );
		mono_add_internal_call ("Sharp.MeshBuilder::Position", MeshBuilderPosition );
		mono_add_internal_call ("Sharp.MeshBuilder::Color", MeshBuilderColor );
		mono_add_internal_call ("Sharp.MeshBuilder::TexCoord", MeshBuilderTexCoord );
		mono_add_internal_call ("Sharp.MeshBuilder::AdvanceVertex", MeshBuilderAdvanceVertex );
		mono_add_internal_call ("Sharp.MeshBuilder::End", MeshBuilderEnd );
		mono_add_internal_call ("Sharp.MeshBuilder::Dispose", MeshBuilderDispose );


		mono_add_internal_call ("Sharp.Material::IsError", MaterialIsError );
		mono_add_internal_call ("Sharp.Material::GetMappingHeight", MaterialGetActualHeight );
		mono_add_internal_call ("Sharp.Material::GetMappingWidth", MaterialGetActualWidth );
		mono_add_internal_call ("Sharp.Material::Dispose", DisposeMaterial );


		mono_add_internal_call ("Sharp.RenderContext::GetDynamicMesh", GetDynamicMesh );
		mono_add_internal_call ("Sharp.RenderContext::FindMaterial", ContextFindMaterial );
		mono_add_internal_call ("Sharp.RenderContext::Bind", ContextBind );

		mono_add_internal_call ("Sharp.RenderContext::SetMatrixMode", SetMatrixMode );
		mono_add_internal_call ("Sharp.RenderContext::GetMatrix", GetMatrix );
		mono_add_internal_call ("Sharp.RenderContext::LoadIdentity", LoadIdentity );
		mono_add_internal_call ("Sharp.RenderContext::LoadMatrix", LoadMatrix );
		mono_add_internal_call ("Sharp.RenderContext::Ortho", Ortho );
		mono_add_internal_call ("Sharp.RenderContext::EnableUserClipTransformOverride", EnableUserClipTransformOverride );
		mono_add_internal_call ("Sharp.RenderContext::UserClipTransform", UserClipTransform );
};

void SharpMeshManager::UpdateMehods( MonoImage* image ){
	m_meshClass = mono_class_from_name( image, "Sharp", "Mesh");
	m_materialClass = mono_class_from_name( image, "Sharp", "Material");
};