#include "cbase.h"
#include "sharp/sharp.h"

static void SharpMsg ( MonoString* str ) {
	char* input = mono_string_to_utf8( str );
	Msg( "%s", input );
	mono_free( input );
}

static void SharpColorMsg ( Color color, MonoString* str ) {
	char* input = mono_string_to_utf8( str );
	ConColorMsg( color, input );
	mono_free( input );
}



class SharpMessage : public IMonoAddon {
public:
	SharpMessage(){
		SharpAddons()->AddAddon( this );

	}

	virtual void RegisterInternalCalls(){
		mono_add_internal_call ("Sharp.SourceWriter::_Msg", SharpMsg);
		mono_add_internal_call ("Sharp.SourceWriter::_ConColorMsg", SharpColorMsg);
	};

	virtual void UpdateMehods( MonoImage* image ){};

};

SharpMessage sharpMessage;