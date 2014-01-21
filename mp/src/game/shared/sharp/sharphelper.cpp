#include "cbase.h"
#include "sharphelper.h"
#include "sharp.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
MonoImage*::MonoImage*( MonoImage* monoImage ) : m_monoImage(monoImage)
{
}

SharpClass MonoImage*::GetClass( const char* name_space, const char* name )
{
	MonoClass* monoClass = mono_class_from_name( m_monoImage, name_space, name );
	Assert( monoClass );
	return SharpClass( monoClass );
}
*/

SharpClass::SharpClass( MonoClass* monoClass ) : m_monoClass(monoClass)
{
}

SharpMethod SharpClass::GetMethod( const char* name, int param_count )
{
	Assert( m_monoClass );
	MonoMethod* monoMethod = mono_class_get_method_from_name( m_monoClass, name, param_count );
	Assert( monoMethod );
	return SharpMethod( monoMethod );
}

SharpObject SharpClass::NewInstance( MonoDomain* domain )
{
	Assert( m_monoClass );
	MonoObject* obj = mono_object_new (domain, m_monoClass );
	Assert( obj );
	return SharpObject( obj );
}

SharpObject SharpClass::NewInstance()
{
	return NewInstance( g_Sharp.GetDomain() );
}



SharpMethod::SharpMethod( MonoMethod* monoMethod ) : m_monoMethod(monoMethod)
{
}

SharpObject SharpMethod::Invoke( void **params, MonoObject **exc )
{
	Assert( m_monoMethod );
	return (SharpObject) mono_runtime_invoke (m_monoMethod, NULL, params, exc );
}

SharpObject SharpMethod::Invoke( SharpObject obj, void **params, MonoObject **exc )
{
	Assert( m_monoMethod );
	return (SharpObject) mono_runtime_invoke (m_monoMethod, obj.m_monoObject, params, exc );
}

bool SharpMethod::SafeInvoke( void **params, SharpObject* retn )
{
	MonoObject* exception;

	SharpObject returnValue = Invoke ( params, &exception );

	if (exception) {
		mono_unhandled_exception( exception );
		return false;
	}

	if( retn != NULL )
		*retn = returnValue;

	return true;
}

bool SharpMethod::SafeInvoke( SharpObject obj, void **params, SharpObject* retn )
{
	MonoObject* exception;

	Assert( obj.m_monoObject );

	SharpObject returnValue = Invoke (obj.m_monoObject, params, &exception );

	if (exception) {
		mono_unhandled_exception( exception );
		return false;
	}

	if( retn != NULL )
		*retn = returnValue;

	return true;
}


SharpObject::SharpObject( MonoObject* monoObject ) : m_monoObject(monoObject)
{
}

SharpMethod SharpObject::GetVirtual( SharpMethod sharpMethod )
{
	Assert( sharpMethod.m_monoMethod );
	Assert( m_monoObject );
	
	MonoMethod* method = mono_object_get_virtual_method( m_monoObject, sharpMethod.m_monoMethod );
	Assert( method );
	return SharpMethod( method );
}

SharpHandle SharpObject::GetNewHandle()
{
	Assert( m_monoObject );
	return SharpHandle( mono_gchandle_new( m_monoObject, false ) );
}

void* SharpObject::Unbox()
{
	Assert( m_monoObject );
	return mono_object_unbox( m_monoObject );
}





SharpHandle::SharpHandle( uint32_t gcHandle ) : m_gcHandle(gcHandle)
{
	Assert(gcHandle);
}

SharpObject SharpHandle::GetObject() const
{
	Assert( m_gcHandle );
	return SharpObject( mono_gchandle_get_target( m_gcHandle ) );
}

void SharpHandle::Release()
{
	Assert( m_gcHandle );
	mono_gchandle_free( m_gcHandle );
	m_gcHandle = 0;
}

SharpClassReference::SharpClassReference(const char* name_space, const char* class_name ) :
	mClass(nullptr), mName_space(name_space), mClass_name(class_name)
{
	SharpAddons()->onLoadMethods.push_back(this);

	if(g_Sharp.sharpImage != nullptr)
		LoadFromImage(g_Sharp.sharpImage);
}

void SharpClassReference::LoadFromImage(MonoImage* image)
{
	MonoClass* monoClass = mono_class_from_name( image, mName_space, mClass_name );
	Assert( monoClass );

	mClass = monoClass;
}

MonoClass* SharpClassReference::Get()
{
	Assert( mClass != nullptr );
	return mClass;
}



SharpMethodReference::SharpMethodReference(const char* name_space, const char* class_name, const char* method_name, int param_count ) :
	mMethod(nullptr), mName_space(name_space), mClass_name(class_name), mMethod_name(method_name), mParam_count(param_count)
{
	SharpAddons()->onLoadMethods.push_back(this);

	if(g_Sharp.sharpImage != nullptr)
		LoadFromImage(g_Sharp.sharpImage);
}

void SharpMethodReference::LoadFromImage(MonoImage* image)
{
	MonoClass* monoClass = mono_class_from_name( image, mName_space, mClass_name );
	Assert( monoClass );

	MonoMethod* monoMethod = mono_class_get_method_from_name( monoClass, mMethod_name, mParam_count );
	Assert( monoMethod );

	mMethod = monoMethod;
}

MonoMethod* SharpMethodReference::Get()
{
	Assert( mMethod != nullptr );
	return mMethod;
}

MonoMethod* SharpMethodReference::GetVirtual(MonoObject* monoObject)
{
	MonoMethod* method = mono_object_get_virtual_method( monoObject, mMethod );
	Assert( method != nullptr );
	return method;
}


SharpMethodItem::SharpMethodItem(const char* name, const void* method) : mName(name), mMethod(method){
	SharpAddons()->registerMethods.push_back(this);
}

bool sharp_safe_invoke( MonoMethod* method, MonoObject* obj, void **params, MonoObject** retn )
{
	MonoObject* exception = nullptr;
	MonoObject* retnValue = mono_runtime_invoke (method, obj, params, &exception );

	if (exception != nullptr) {
		mono_unhandled_exception( exception );
		return false;
	}

	if( retn != NULL )
		*retn = retnValue;

	return true;

}