#pragma once

#include "cbase.h"
#include "dt_common.h"
#include "sharp/sharp.h"
#include "sharp/sharphelper.h"
#include <string>
#include <vector>
#include <memory>

struct NativeEntityData {
	std::string m_entityName;
	MonoClass* m_monoClass;

#ifdef CLIENT_DLL
	std::shared_ptr<std::vector<RecvProp>> m_props; 
	ClientClass* m_clientClass;
#else
	std::shared_ptr<std::vector<SendProp>> m_props; 
	ServerClass* m_serverClass;
#endif
};

class SharpEntityAttribute : public MonoObject {
public:
	MonoString* className;
};

class SharpNativeDT {

	MonoImage* m_sharpImage;
	SharpClass m_entityAttrClass;
	SharpClass m_entitynetworkedAttrClass;
	SharpClass m_animatingClass;

public:
	std::vector<NativeEntityData> m_entities;
	SharpNativeDT()
	{
	};

#ifndef CLIENT_DLL
	CBaseAnimating* CreateEntity( const char* szClassname );
#endif

	void LoadSharpImage(MonoImage* image);
private: 
	void RegisterClass(MonoClass *klass, SharpEntityAttribute* attr);
};

extern SharpNativeDT g_nativeDT;
