#include "cbase.h"
#include "sharp/concommand.h"
#include "sharphelper.h"
#include "sharp/sharp.h"
#include "sharp/sharphelper.h"
#include "convar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

SharpCommand sharpCommand;
SharpCommand* g_sharpCommand = &sharpCommand;


SharpObject ToMonoObject( const CCommand& command )
{
	MonoClass* stringClass =  mono_get_string_class();
	MonoArray *strArray = mono_array_new( g_Sharp.GetDomain(), stringClass,command.ArgC() );

	for( int i = 0; i < command.ArgC(); i++ ){
		mono_array_setref( strArray, i, mono_string_new(g_Sharp.GetDomain(), command[i]) );
	}

	SharpObject CCommandInstance = sharpCommand.m_CCommandClass.NewInstance();
	
	void* args[] = { mono_string_new(g_Sharp.GetDomain(), command.GetCommandString() ), strArray };
	sharpCommand.m_CCommandConstructor.Invoke(CCommandInstance, args, NULL);

	return CCommandInstance;
}

/**
	SharpConCommand
*/
SharpConCommand::SharpConCommand( MonoObject* object, const char *pName, const char *pHelpString, int flags, MonoDomain* domain )
	: ConCommand( pName, (ICommandCallback*)0, pHelpString, flags | FCVAR_UNREGISTERED )
{
	m_monoDomain = domain;
	m_monoHandle = mono_gchandle_new( object, false );

	//We have to register it manually (with FCVAR_UNREGISTERED) so we can unregister it later
	cvar->RegisterConCommand( this );
}

SharpConCommand::~SharpConCommand()
{
	cvar->UnregisterConCommand( this );
	GetSharpObject()->Set( NULL );
}

//-----------------------------------------------------------------------------
// Purpose: Invoke the function if there is one
//-----------------------------------------------------------------------------
void SharpConCommand::Dispatch( const CCommand &command )
{
	mono_domain_set( m_monoDomain, true );
	ConCommandSharpObject* object = GetSharpObject();
	m_CommandCallbackMethod = mono_object_get_virtual_method( object, sharpCommand.m_CommandCallbackMethod.m_monoMethod );

	MonoObject* exception;
	void* params[1] = { ToMonoObject(command).m_monoObject };
	mono_runtime_invoke (m_CommandCallbackMethod, object, params, &exception);
		
	if (exception) {
		mono_unhandled_exception( exception );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Calls the autocompletion method to get autocompletion suggestions
//-----------------------------------------------------------------------------
int	SharpConCommand::AutoCompleteSuggest( const char *partial, CUtlVector< CUtlString > &commands )
{
	ConCommandSharpObject* object = GetSharpObject();
	m_CompleteCallbackMethod = mono_object_get_virtual_method( object, sharpCommand.m_CompleteCallbackMethod.m_monoMethod );
	mono_domain_set( m_monoDomain, true );
	//TODO
	return 0;
}

//-----------------------------------------------------------------------------
// Returns true if the console command can autocomplete 
//-----------------------------------------------------------------------------
bool SharpConCommand::CanAutoComplete( void )
{
	return false; //TODO
}

ConCommandSharpObject* SharpConCommand::GetSharpObject()
{
	return (ConCommandSharpObject*) mono_gchandle_get_target( m_monoHandle );
}

static void ConVarChange( IConVar *var, const char *pOldValue, float flOldValue )
{
	MonoDomain* oldDomain = mono_domain_get();
	
	SharpConVar* sharpConVar = static_cast<SharpConVar*>(var);

	mono_domain_set( sharpConVar->m_monoDomain, true );
	ConVarSharpObject* object = sharpConVar->GetSharpObject();
	
	void* params[] = {
		mono_string_new(sharpConVar->m_monoDomain, pOldValue ),
		&flOldValue
	};

	Assert( object != NULL );
	Assert( sharpConVar->m_ChangeCallbackMethod.m_monoMethod != NULL );

	sharpConVar->m_ChangeCallbackMethod.SafeInvoke(SharpObject(object), params );

	mono_domain_set( oldDomain, true );
}

/**
	SharpConVar
*/
SharpConVar::SharpConVar( ConVarSharpObject* object, const char *pName, const char *pDefaultValue, const char *pHelpString, int flags, MonoDomain* domain )
	: m_monoDomain(domain), m_ChangeCallbackMethod(), ConVar( pName, pDefaultValue, flags | FCVAR_UNREGISTERED, pHelpString )
{
	m_monoHandle = mono_gchandle_new( object, false );

	SharpObject sharpObj( object );

	m_ChangeCallbackMethod = sharpObj.GetVirtual( g_sharpCommand->m_ConvarCallbackMethod );

	//We have to register it manually (with FCVAR_UNREGISTERED) so we can unregister it later
	cvar->RegisterConCommand( this );

	//Callback is called immediatly after registration

	InstallChangeCallback( ConVarChange );
}

SharpConVar::~SharpConVar()
{
	cvar->UnregisterConCommand( this );
	GetSharpObject()->Set( NULL );
}

ConVarSharpObject* SharpConVar::GetSharpObject()
{
	return (ConVarSharpObject*) mono_gchandle_get_target( m_monoHandle );
}

static void CreateConCommandObject ( ConCommandSharpObject *monoConCommand, MonoString* commandObj, MonoString* helpTextObj, int flags ) {
	ASSERT_DOMAIN();

	if (commandObj == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("command"));

	if (helpTextObj == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("helperText"));

	char* command =  mono_string_to_utf8( commandObj );
	char* helpText = mono_string_to_utf8( helpTextObj );
	MonoDomain* domain = mono_domain_get();

	//TODO:
	//There might be a memory leak here because of the strdup, and the ConCommand class is not freeing those objects
	//But it must be duplicated, since internally, the memory is used.
	//Sadly, there is no way for me to know for sure.
	SharpConCommand* cCommand = new SharpConCommand( monoConCommand, strdup(command), strdup(helpText), flags, domain );
	monoConCommand->Set( cCommand );

	mono_free( command );
	mono_free( helpText );
};

static void DestroyConCommandObject ( ConCommandSharpObject *monoConCommand ) {
	ASSERT_DOMAIN();
	SharpConCommand* command = monoConCommand->Get();

	delete command;
};



static void CreateConVarObject ( 
	ConVarSharpObject *monoConVar, 
	MonoString* commandObj, 
	MonoString* defaultValueObj, 
	MonoString* helpTextObj, 
	int flags ) 
{
	ASSERT_DOMAIN();

	if (commandObj == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("command"));

	if (defaultValueObj == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("defaultValue"));

	if (helpTextObj == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("helpText"));

	char* command =  mono_string_to_utf8( commandObj );
	char* helpText = mono_string_to_utf8( helpTextObj );
	char* defaultValue = mono_string_to_utf8( defaultValueObj );
	MonoDomain* domain = mono_domain_get();

	//TODO:
	//There might be a memory leak here because of the strdup, and the ConCommand class is not freeing those objects
	//But it must be duplicated, since internally, the memory is used.
	//Sadly, there is no way for me to know for sure.
	SharpConVar* cCommand = new SharpConVar( monoConVar, strdup(command), strdup(defaultValue), strdup(helpText), flags, domain );
	monoConVar->Set( cCommand );

	mono_free( command );
	mono_free( helpText );
};

static void DestroyConVarObject ( ConVarSharpObject *monoConCommand ) {
	ASSERT_DOMAIN();
	SharpConVar* command = monoConCommand->Get();

	delete command;
};

static float  ConVarGetFloat(ConVarSharpObject *monoConCommand){
	return monoConCommand->Get()->GetFloat();
}

static int ConVarGetInt(ConVarSharpObject *monoConCommand){
	return monoConCommand->Get()->GetInt();
}

static MonoString* ConVarGetValue(ConVarSharpObject *monoConCommand){
	SharpConVar* command = monoConCommand->Get();

	return mono_string_new(command->m_monoDomain, command->GetString() );
}

static void ConVarSetFloat(ConVarSharpObject *monoConCommand, float value ){
	monoConCommand->Get()->SetValue( value );
}

static void ConVarSetInt(ConVarSharpObject *monoConCommand, int value ){
	monoConCommand->Get()->SetValue( value );
}

static void ConVarSetValue(ConVarSharpObject *monoConCommand, MonoString* valStr ){
	SharpConVar* command = monoConCommand->Get();
	char* value = mono_string_to_utf8( valStr );

	command->SetValue( value );

	mono_free( value );
}

static MonoObject* GetCommandClient()
{
#if !defined( CLIENT_DLL )
	CBasePlayer* player = UTIL_GetCommandClient();

	if( player == NULL )
		return NULL;

	SharpHandle monoHandle = UTIL_GetCommandClient()->MonoHandle;	
#else
	SharpHandle monoHandle = C_BasePlayer::GetLocalPlayer()->MonoHandle;	
#endif
	return monoHandle.GetObject().m_monoObject;
};

#if defined( CLIENT_DLL )
static void ClientEngineClientCmd(MonoString* monoStr) {
	ASSERT_DOMAIN();

	if (monoStr == NULL)
		mono_raise_exception(mono_get_exception_argument_null("command"));

	char* input = mono_string_to_utf8(monoStr);

	if (strlen(input) > 512)
	{
		mono_free(input);
		mono_raise_exception(mono_get_exception_overflow());
	}

	engine->ClientCmd(input);
	mono_free(input);
};
static SharpMethodItem ButtonOnClickItem("Sharp.ClientEngine::ClientCmd", ClientEngineClientCmd);
#endif

void SharpCommand::RegisterInternalCalls(){
	mono_add_internal_call ("Sharp.ConCommand::Create"MONO_CLASS, CreateConCommandObject );
	mono_add_internal_call ("Sharp.ConCommand::Dispose"MONO_CLASS, DestroyConCommandObject );

	mono_add_internal_call ("Sharp.ConVar::Create"MONO_CLASS, CreateConVarObject );
	mono_add_internal_call ("Sharp.ConVar::Dispose"MONO_CLASS, DestroyConVarObject );

	mono_add_internal_call ("Sharp.ConCommand::GetCommandClient"MONO_CLASS, GetCommandClient );
	
	mono_add_internal_call ("Sharp.ConVar::get_FloatValue", ConVarGetFloat );
	mono_add_internal_call ("Sharp.ConVar::get_IntValue", ConVarGetInt );
	mono_add_internal_call ("Sharp.ConVar::get_Value", ConVarGetValue );
	mono_add_internal_call ("Sharp.ConVar::set_FloatValue", ConVarSetFloat );
	mono_add_internal_call ("Sharp.ConVar::set_IntValue", ConVarSetInt );
	mono_add_internal_call ("Sharp.ConVar::set_Value", ConVarSetValue );
}


bool SharpCommand::ClientCommand( CBasePlayer* pPlayer, const CCommand &command )
{
	UPDATE_DOMAIN()
	void* args[2] = { 
		pPlayer->MonoHandle.GetObject().m_monoObject,
		ToMonoObject( command ).m_monoObject
	};

	MonoObject* exception;
	SharpObject retn = m_mCommandMethod.Invoke( args, &exception );

	if (exception) {
		mono_unhandled_exception( exception );
		return false;
	}

	return *((bool*) retn.Unbox());
}

void SharpCommand::UpdateMehods( MonoImage* image ){
	SharpClass concommandManager = mono_class_from_name( image, "Sharp", "Command");
	m_mCommandMethod = concommandManager.GetMethod( "_OnClientCommand", 2 );

	m_CCommandClass = mono_class_from_name( image, "Sharp", "CCommand");
	m_CCommandConstructor = m_CCommandClass.GetMethod(".ctor", 2 );

	SharpClass conCommandClass = mono_class_from_name( image, "Sharp", "ConCommand");
	m_CommandCallbackMethod = conCommandClass.GetMethod( "OnCommand", 1 );
	m_CompleteCallbackMethod = conCommandClass.GetMethod( "AutoCompleteSuggest", 1 );

	SharpClass conVarClass = mono_class_from_name( image, "Sharp", "ConVar");
	m_ConvarCallbackMethod = conVarClass.GetMethod( "OnChange", 2);
}