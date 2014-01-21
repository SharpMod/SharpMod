#pragma once

#include "sharp/sharp.h"
#include "sharp/sharphelper.h"

class SharpRecipientFilter;

class BitReadSharpObject : public TemplateSharpObject<bf_read> {

};

class SharpUserMessage : public IMonoAddon {

public:
	SharpUserMessage(){
		SharpAddons()->AddAddon( this );	
	};

	virtual void RegisterInternalCalls( void );
	virtual void UpdateMehods( MonoImage* image );

	SharpMethod m_getRecipientCount;
	SharpMethod m_getRecipientIndex;
	SharpMethod m_isReliable;
	SharpMethod m_isInitMessage;

#ifdef GAME_DLL
	bool GetUserMessageInfo( int msg_type, char *name, int maxnamelength, int& size );

	SharpMethod m_getUserMessageInfo;
#else
	bool DispatchUserMessage( int msg_type, bf_read &msg_data );
	BitReadSharpObject* NewBitReadSharpObject( bf_read &msg_data );

	SharpClass m_bitReadClass;
	SharpMethod m_dispatchUserMessage;
#endif
};

extern SharpUserMessage g_sharpUserMessage;

class SharpRecipientFilter : public IRecipientFilter
{
public:
	MonoObject* m_recipientFilter;

	SharpRecipientFilter() : m_recipientFilter(NULL)
	{
	}

	void SetFilter( MonoObject* recipientFilter );

	virtual bool	IsReliable( void ) const;

	virtual bool	IsInitMessage( void ) const;

	virtual int		GetRecipientCount( void ) const;
	virtual int		GetRecipientIndex( int slot ) const;

};

extern SharpUserMessage g_sharpUserMessage;


