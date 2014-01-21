#pragma once

#include "sharp/sharp.h"


class ConCommandSharpObject;
class ConVarSharpObject;

class SharpCommand : public IMonoAddon {

public:
	SharpCommand(){
		SharpAddons()->AddAddon( this );
	}

	bool ClientCommand( CBasePlayer* player, const CCommand &args );

	virtual void RegisterInternalCalls();
	virtual void UpdateMehods( MonoImage* image );

	SharpMethod m_mCommandMethod;

	SharpClass m_CCommandClass;
	SharpMethod m_CCommandConstructor;

	SharpMethod m_CommandCallbackMethod;
	SharpMethod m_CompleteCallbackMethod;
	SharpMethod m_ConvarCallbackMethod;
};

class SharpConCommand : public ConCommand 
{

public:

	SharpConCommand( MonoObject* object, const char *pName, const char *pHelpString, int flags, MonoDomain* domain );
	virtual ~SharpConCommand();

	virtual int AutoCompleteSuggest( const char *partial, CUtlVector< CUtlString > &commands );
	virtual bool CanAutoComplete( void );

	// Invoke the function
	virtual void Dispatch( const CCommand &command );

	ConCommandSharpObject* GetSharpObject();

protected:
	MonoMethod* m_CommandCallbackMethod;
	MonoMethod* m_CompleteCallbackMethod;
	uint32_t m_monoHandle;
	MonoDomain* m_monoDomain;

};

class ConCommandSharpObject : public TemplateSharpObject<SharpConCommand> 
{
};

class SharpConVar : public ConVar 
{
public:
	SharpConVar( ConVarSharpObject* object, const char *pName, const char *defaultValue, const char *pHelpString, int flags, MonoDomain* domain );
	virtual ~SharpConVar();

	ConVarSharpObject* GetSharpObject();

	uint32_t m_monoHandle;
	MonoDomain* m_monoDomain;

	SharpMethod m_ChangeCallbackMethod;
};

class ConVarSharpObject : public TemplateSharpObject<SharpConVar> 
{
};


extern SharpCommand* g_sharpCommand;


