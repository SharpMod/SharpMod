#include "cbase.h"
#include "sharp.h"
#include <filesystem.h>
#include "sharp/sharphelper.h"
#include <mono/metadata/tokentype.h>
#include "sharp_native_entity.h"

Sharp g_Sharp;

#ifdef _WIN32
//Define redefinitions
#pragma warning( push )
#pragma warning( disable : 4005 )
#include <Windows.h>
#pragma warning( pop ) 
#undef GetCurrentDirectory
#endif

void GetGameDir( char* gamePath, int length ){
#if !defined( CLIENT_DLL )
	engine->GetGameDir( gamePath, length - 1 );
#else
	Q_strncpy( gamePath, engine->GetGameDirectory(), length-1 );
#endif
}

//-----------------------------------------------------------------------------
// Singleton accessor
//-----------------------------------------------------------------------------
ISharpAddons *SharpAddons()
{
	static ISharpAddons s_EntityFactory;
	return &s_EntityFactory;
}


#include <vector>
static std::vector<SharpMethodItem*> registerMethods;
static std::vector<ISharpOnLoad*> onLoadMethods;


void ISharpAddons::AddAddon( IMonoAddon* monoAddon ){
	AssertMsg( !m_monoAddons.IsValidIndex( m_monoAddons.Find( monoAddon ) ), "Addon being added twice!");

	m_monoAddons.AddToTail( monoAddon );
}


void ISharpAddons::InitializeAddons(MonoImage* image){
	int addonsSize = m_monoAddons.Count();
	for( int i = 0; i < addonsSize; i++ ){
		m_monoAddons[i]->UpdateMehods(image);
	}

	for (std::vector<ISharpOnLoad*>::iterator it = SharpAddons()->onLoadMethods.begin() ; it != SharpAddons()->onLoadMethods.end(); ++it)
	{
		(*it)->LoadFromImage(image);
	}
}

void ISharpAddons::InitializeCalls(){
	int addonsSize = m_monoAddons.Count();
	for( int i = 0; i < addonsSize; i++ ){
		m_monoAddons[i]->RegisterInternalCalls();
	}

	//FOR_EACH_VEC( g_Sharp.registerMethods, method )
	for (std::vector<SharpMethodItem*>::iterator it = SharpAddons()->registerMethods.begin() ; it != SharpAddons()->registerMethods.end(); ++it)
	{
		mono_add_internal_call ((*it)->mName, (*it)->mMethod);
	}
}


Sharp::Sharp() : sharpImage(nullptr), loadedInternalCalls(false) {
}


static mono_bool
determine_platform_image (const char *image_name)
{
	char gamePath[MAX_PATH];
	GetGameDir( gamePath, sizeof( gamePath ) );
	
	char libPath[MAX_PATH];
	Q_snprintf( libPath, sizeof(libPath), "%s\\bin\\sharp.dll", gamePath );

	if( strcmp( image_name, libPath ) == 0 ){
		Msg("Loading image: %s\n", image_name );
		return true;
	}

	Q_snprintf( libPath, sizeof(libPath), "%s\\bin\\mono\\lib\\mono\\4.5\\mscorlib.dll", gamePath );

	if( strcmp( image_name, libPath ) == 0 ){
		Msg("Loading corlib: %s\n", image_name );
		return true;
	}

	Msg("Loading UNSAFE addon: %s\n", image_name );
	return false;
}

#include <mono/metadata/mono-debug.h>
#include "engine/IEngineSound.h"
#include <signal.h>
//#include <mono/mini/mini.h>

void InitializeRootDomain(){

	mono_security_enable_core_clr ();
    mono_security_set_core_clr_platform_callback (determine_platform_image);

	char gamePath[MAX_PATH];
	GetGameDir( gamePath, sizeof( gamePath ) );

	char libPath[MAX_PATH];
	Q_snprintf( libPath, sizeof( libPath ), "%s\\bin\\mono\\lib", gamePath );
	char etcPath[MAX_PATH];
	Q_snprintf( etcPath, sizeof( etcPath ), "%s\\bin\\mono\\etc", gamePath );

	mono_set_dirs(libPath, etcPath);
	mono_config_parse (NULL);
	
	char lib2Path[MAX_PATH];
	Q_snprintf( lib2Path, sizeof(libPath), "%s\\bin\\sharp.dll", gamePath );

	if(!filesystem->FileExists(lib2Path))
	{
		Error("Unable to find: %s", lib2Path);
	}

	mono_jit_init (lib2Path);

#ifdef _WIN32
	LPTOP_LEVEL_EXCEPTION_FILTER pSehHandler = SetUnhandledExceptionFilter( NULL );
	AddVectoredExceptionHandler( 1, pSehHandler );
	SetUnhandledExceptionFilter( pSehHandler );
#endif

	Assert( sizeof(SharpHandle) == sizeof(void*) );
	Assert( sizeof(SharpMethod) == sizeof(void*) );
	Assert( sizeof(SharpObject) == sizeof(void*) );
	Assert( sizeof(SharpClass) == sizeof(void*) );
}



void Sharp::Setup(){
	if( mono_get_root_domain() == NULL ){
		InitializeRootDomain();
	}

	if( loadedInternalCalls == false ){
		SharpAddons()->InitializeCalls();
		loadedInternalCalls = true;
	}
	
	FileFindHandle_t findHandle;

	const char *pszFileName = g_pFullFileSystem->FindFirst( "addons\\*", &findHandle );
	while ( pszFileName )
	{
		if ( pszFileName[0] != '.' && g_pFullFileSystem->FindIsDirectory( findHandle ) ){
			char szFullPath[ MAX_PATH ];
			Q_snprintf( szFullPath, sizeof(szFullPath), "addons\\%s", pszFileName );
			Q_FixSlashes( szFullPath ); 

			if ( g_pFullFileSystem->IsDirectory( szFullPath ) ){
				filesystem->AddSearchPath( szFullPath, pszFileName );
			}
			
		}
		pszFileName = g_pFullFileSystem->FindNext( findHandle );
	}

	//filesystem->AddSearchPath( "C:\\Users\\Nican\\Documents\\GitHub\\source-sdk-2013\\mp\\game\\mod_hl2mp\\addons\\GMT2_BallRace", "GAME" );
	//filesystem->AddSearchPath( "C:\\Users\\Nican\\Documents\\GitHub\\source-sdk-2013\\mp\\game\\mod_hl2mp\\addons\\GMT2_Base", "GAME" );

	g_pFullFileSystem->FindClose( findHandle );

	
	char gamePath[MAX_PATH];
	GetGameDir( gamePath, sizeof( gamePath ) );
	Q_snprintf( m_szSharpPath, sizeof(m_szSharpPath), "%s\\bin\\sharp.dll", gamePath );

	MonoAssembly* assembly = mono_domain_assembly_open ( mono_get_root_domain(), m_szSharpPath );
	
	if (!assembly){
		Error("SHARP: Could not load assembly at: %s\n", m_szSharpPath);
		return;
	}

	MonoImage* image = mono_assembly_get_image (assembly);
	

	g_nativeDT.LoadSharpImage( image );
	
}

void Sharp::Initialize(){
	AssertMsg( m_pDomain == NULL, "Loading Sharp while it is already loaded!");

	char gamePath[MAX_PATH];
	GetGameDir( gamePath, sizeof( gamePath ) );
	Q_snprintf( m_szSharpPath, sizeof(m_szSharpPath), "%s\\bin\\sharp.dll", gamePath );


	mono_domain_set( mono_get_root_domain(), false );
	m_pDomain = mono_domain_create_appdomain( MONO_DOMAIN_NAME, NULL ); 

	//rescrict_domain(m_pDomain);

	MonoAssembly* assembly = mono_domain_assembly_open ( m_pDomain, m_szSharpPath );
	
	if (!assembly){
		Warning("SHARP: Could not load assembly at: %s\n", m_szSharpPath);
		return;
	}

	mono_domain_set( this->m_pDomain, false );
	MonoImage* image = mono_assembly_get_image (assembly);

	Msg("Initializing Mono methods.\n"); 
	sharpImage = image;
	SharpAddons()->InitializeAddons( image );

	MonoClass *initClass = mono_class_from_name (image, "Sharp", "Sharp");
	AssertMsg(initClass, "Unable to find Source.Sharp class!");

	MonoMethod* initMethod = mono_class_get_method_from_name( initClass, "Initialize", 1 );
	AssertMsg( initMethod, "Could not find the Sharp.Sharp::Initialize method");

	MonoObject* exception;
#ifdef CLIENT_DLL
	bool isServer = 0;
#else
	bool isServer = 1;
#endif
	void* args[1];
	args[0] = &isServer;

	mono_runtime_invoke (initMethod, NULL, args, &exception);
	//And getting the expcetion from C is just tell the programmer he needs some rethinking
	AssertMsg( exception == NULL, "Shame on you! The Sharp.Sharp::Initialize just threw an exception!");

	if( exception ){
		MonoString* str = mono_object_to_string (exception, NULL);
		if (str) {
			MonoError error;
			char* message = mono_string_to_utf8_checked (str, &error);
			if (!mono_error_ok (&error)) {
				mono_error_cleanup (&error);
			} else {
				Warning( "%s\n", message );
				mono_free (message);
			}
			return;
		}
	}

	UpdateDomain();

	char filename[MAX_PATH];
	Q_snprintf( filename, sizeof( filename ), "%s\\bin\\BaseAddon.exe", gamePath );
	assembly = mono_domain_assembly_open ( m_pDomain, filename );

	if (!assembly){
		Warning("SHARP: Could not load assembly at: %s\n", filename);
		return;
	}

	image = mono_assembly_get_image (assembly);
	uint32_t entry = mono_image_get_entry_point (image);
	
	if (!entry) {
		Msg ("Assembly '%s' doesn't have an entry point.\n", mono_image_get_filename (image));
		return;
	}

	MonoMethod *entryMethod = mono_get_method (image, entry, NULL);
	if (entryMethod == NULL){
		Msg ("The entry point method could not be loaded\n");
		return;
	}

	char* addonArgs[] = {"BaseAddon.exe"};

	mono_runtime_run_main (entryMethod, 1, addonArgs, &exception);

	/*
	MonoClass* addonLoadClass = mono_class_from_name (image, "BaseAddon", "Base");
	AssertMsg(addonLoadClass, "Unable to find BaseAddon.Base class!");
	MonoObject* obj = mono_object_new (m_pDomain, addonLoadClass);

	initMethod = mono_class_get_method_from_name( addonLoadClass, ".ctor", 0 );
	mono_runtime_invoke (initMethod, obj, args, &exception);
	*/

	if (exception) 
		mono_unhandled_exception( exception );

	MonoMethod* loadAttributesMethod = mono_class_get_method_from_name( initClass, "OnAddonLoaded", 1 );
	AssertMsg( loadAttributesMethod, "Could not find the Sharp.Sharp::OnAddonLoaded method");
	args[0] = mono_assembly_get_object( m_pDomain, assembly );

	mono_runtime_invoke (loadAttributesMethod, NULL, args, &exception);
	if (exception) 
		mono_unhandled_exception( exception );


	MonoClass *baseModClass = mono_class_from_name (image, "BaseAddon", "Base");
	AssertMsg(baseModClass, "Unable to find BaseAddon.Base class!");

	MonoMethod* baseReadFile = mono_class_get_method_from_name( baseModClass, "NicanReadFile", 1 );
	AssertMsg( baseReadFile, "Could not find the BaseAddon.Base::NicanReadFile method");

	args[0] = nullptr;
	mono_runtime_invoke (baseReadFile, NULL, args, &exception);

	if (exception) 
		mono_unhandled_exception( exception );



}

void Sharp::Shutdown(){
	AssertMsg( m_pDomain != NULL, "Shutting down while domain was not even loaded");

	Msg("Shutting MONO domain down!\n");

	MonoObject *exc = NULL;
	mono_domain_try_unload( m_pDomain, &exc );
	if( exc ){
		Assert(0);
		Msg("We got an exception!");
	}

	m_pDomain = NULL;
}

Sharp::~Sharp(){


}

