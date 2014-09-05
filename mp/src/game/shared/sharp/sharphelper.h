#pragma once

#include "sharp.h"
#include <mono/metadata/image.h>
#include <mono/metadata/metadata.h>
#include <mono/metadata/object.h>

class SharpMethod;
class SharpObject;
class SharpClass;
class SharpHandle;
class SharpObjectHandle;


class SharpClass {
public:
	MonoClass* m_monoClass;

	SharpClass() : m_monoClass(NULL){};
	SharpClass( MonoClass* monoClass );

	SharpMethod GetMethod( const char* name, int param_count );

	SharpObject NewInstance( MonoDomain* domain );
	SharpObject NewInstance();
};


class SharpMethod {
public:
	MonoMethod* m_monoMethod;

	SharpMethod() : m_monoMethod(NULL) {};
	SharpMethod( MonoMethod* monoMethod );
	
	SharpObject Invoke( void **params, MonoObject **exc );
	bool SafeInvoke( void **params, SharpObject* retn = NULL );

	SharpObject Invoke( SharpObject obj, void **params, MonoObject **exc );
	bool SafeInvoke( SharpObject obj, void **params, SharpObject* retn = NULL );
};

class SharpObject {
public:
	SharpObject() : m_monoObject(NULL) {};
	SharpObject( MonoObject* monoObject );
	SharpMethod GetVirtual( SharpMethod sharpMethod );
	SharpHandle GetNewHandle();

	void* Unbox();

	MonoObject* m_monoObject;
};

class SharpHandle {
public:
	SharpHandle() : m_gcHandle(0) {};
	SharpHandle( uint32_t gcHandle );
	SharpObject GetObject() const;
	void Release();
	inline bool IsValid(){ return m_gcHandle != 0; }

	uint32_t m_gcHandle;
};

class SharpClassReference : ISharpOnLoad {
	MonoClass* mClass;
	const char* mName_space;
	const char* mClass_name;

public:
	SharpClassReference(const char* name_space, const char* class_name );

	virtual void LoadFromImage( MonoImage* image ) OVERRIDE;
	MonoClass* Get();

};

class SharpMethodReference : ISharpOnLoad  {
	MonoMethod* mMethod;
	const char* mName_space;
	const char* mClass_name;
	const char* mMethod_name;
	int mParam_count;

public:
	SharpMethodReference(const char* name_space, const char* class_name, const char* method_name, int param_count );

	virtual void LoadFromImage( MonoImage* image ) OVERRIDE;
	MonoMethod* Get();
	MonoMethod* GetVirtual(MonoObject* obj);

};

template<class T>
abstract_class SharpInternalVariable : public ISharpOnLoad {
	T* value;
	const char* mName_space;
	const char* mClass_name;
	const char* mInstance_name;

	MonoClass* m_monoClass;
	MonoClassField* m_field;

public:
	SharpInternalVariable(const char* name_space, const char* class_name, const char* instance_name ) :
	mName_space(name_space),  mClass_name(class_name), mInstance_name(instance_name)
	{
		SharpAddons()->onLoadMethods.push_back(this);
	};

	void LoadFromImage( MonoImage* image ) OVERRIDE {
		m_monoClass = mono_class_from_name( image, mName_space, mClass_name );
		Assert( m_monoClass );

		MonoVTable* vTable = mono_class_vtable( g_Sharp.GetDomain(), m_monoClass );
		Assert( vTable != nullptr );
		m_field = mono_class_get_field_from_name(m_monoClass, mInstance_name );
		Assert( m_field != nullptr );

		T* loadValue = LoadValue();
		mono_field_static_set_value( vTable, m_field, &loadValue );	
	};

	virtual T* LoadValue() = 0;
	
	T* Get(){
		Assert( m_field != nullptr );

		void* value;
		MonoVTable* vTable = mono_class_vtable( mono_domain_get(), m_monoClass );
		mono_field_static_get_value( vTable, m_field, &value );
		T* casted_val = static_cast<T*>(value);

		Assert( casted_val != nullptr );

		return casted_val;
	}
};

class SharpMethodItem {
public:
	const char* mName;
	const void* mMethod;

	SharpMethodItem(const char* name, const void* method);
};

bool sharp_safe_invoke( MonoMethod* method, MonoObject* obj, void **params, MonoObject** retn = nullptr );