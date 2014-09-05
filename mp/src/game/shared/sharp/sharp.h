#pragma once

#undef max
#undef min

#include <vector>
#include <minmax.h>

#include <typeinfo>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/exception.h>

#include "mono/metadata/threads.h"


#define ASSERT_DOMAIN() \
	g_Sharp.AssertDomain();

#define UPDATE_DOMAIN() \
	g_Sharp.UpdateDomain();

#define THREAD_UPDATE_DOMAIN() \
	g_Sharp.ThreadUpdateDomain();

#if !defined( CLIENT_DLL )
#define MONO_DOMAIN_NAME "Source.Server"
#else
#define MONO_DOMAIN_NAME "Source.Client"
#endif

#if defined( CLIENT_DLL )
#define MONO_CLASS "Client"
#else
#define MONO_CLASS "Server"
#endif

#define __OVERRIDE__ 

abstract_class IMonoAddon {
public:
	virtual void RegisterInternalCalls() = 0;
	virtual void UpdateMehods( MonoImage* image ) = 0;
};


class SharpMethodItem;

abstract_class ISharpOnLoad {
public:
	virtual void LoadFromImage( MonoImage* image ) = 0;
};

class ISharpAddons {
public:
	void AddAddon( IMonoAddon* );
	void InitializeAddons( MonoImage* image );

	void InitializeCalls();

	std::vector<SharpMethodItem*> registerMethods;
	std::vector<ISharpOnLoad*> onLoadMethods;
private:
	CUtlVector<IMonoAddon*>	m_monoAddons;
};

class Sharp {

public:
	Sharp();
	~Sharp();

	void Setup();
	void Initialize();
	void Shutdown();
	inline void UpdateDomain(){
		mono_domain_set( GetDomain(), true );
	}
	inline void ThreadUpdateDomain(){
		if( mono_domain_get() == nullptr )
			mono_thread_attach( GetDomain() );

		UpdateDomain();
	};
	void AssertDomain(){
		AssertMsg( m_pDomain == mono_domain_get(), "Sharp calling method on different domain!" );

		if( m_pDomain != mono_domain_get() ){
			mono_raise_exception ( mono_get_exception_invalid_operation("Sharp calling method on different domain! (Ex. Server calling on Client)") );
		}
	}

	inline MonoDomain* GetDomain(){ 
		AssertMsg( m_pDomain != nullptr, "Domain is null! We can not get it!");
		return m_pDomain; 
	}
	bool ValidDomain(){
		return m_pDomain != nullptr;
	}
	
	
	MonoImage* sharpImage;

	char m_szSharpPath[MAX_PATH];
	MonoDomain *m_pDomain;

private:
	bool loadedInternalCalls;
};

extern Sharp g_Sharp;
ISharpAddons *SharpAddons();

template <class T> 
struct TemplateSharpObject : public MonoObject {
	T *object;
	T *Get(){
		if( object == NULL ){
			char message[256];
			Q_snprintf( message, sizeof(message), "%s is not valid", typeid(T).name() );
			mono_raise_exception ( mono_get_exception_invalid_operation( message ) );
		}
		return object;
	};
	void Set(T *obj){
		Assert( (obj != NULL && this->object == NULL) || (obj == NULL && this->object != NULL) );
		this->object = obj;
	};
};
