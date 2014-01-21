#include "cbase.h"
#include "sharp/sharp.h"
#include "sharp/entity.h"
#include "sharp/sharp_usermessage.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class SharpRecipientFilter;
SharpUserMessage g_sharpUserMessage;



void SharpRecipientFilter::SetFilter( MonoObject* recipientFilter ){
	m_recipientFilter = recipientFilter;
}

bool SharpRecipientFilter::IsReliable( void ) const
{
	SharpObject obj;
	g_sharpUserMessage.m_isReliable.SafeInvoke( m_recipientFilter, NULL, &obj );
	bool* retn = (bool*) obj.Unbox();

	return *retn;
};

bool SharpRecipientFilter::IsInitMessage( void ) const {
	SharpObject obj;
	g_sharpUserMessage.m_isInitMessage.SafeInvoke( m_recipientFilter, NULL, &obj );
	bool* retn = (bool*) obj.Unbox();

	return *retn;
};

int	SharpRecipientFilter::GetRecipientCount( void ) const {
	SharpObject obj;
	g_sharpUserMessage.m_getRecipientCount.SafeInvoke( m_recipientFilter, NULL, &obj );
	int* retn = (int*) obj.Unbox();

	return *retn;
};

int	SharpRecipientFilter::GetRecipientIndex( int slot ) const {
	SharpObject obj;
	void* args[] = { &slot };
	g_sharpUserMessage.m_getRecipientIndex.SafeInvoke( m_recipientFilter, args, &obj );
	int* retn = (int*) obj.Unbox();

	return *retn;
};

static char SharpBitReadReadChar(BitReadSharpObject* bitReadObject)
{
	ASSERT_DOMAIN();
	return bitReadObject->Get()->ReadChar();
}

static char SharpBitReadReadByte(BitReadSharpObject* bitReadObject)
{
	ASSERT_DOMAIN();
	return bitReadObject->Get()->ReadByte();
}

static short SharpBitReadReadShort(BitReadSharpObject* bitReadObject)
{
	ASSERT_DOMAIN();
	return bitReadObject->Get()->ReadShort();
}

static int SharpBitReadReadLong(BitReadSharpObject* bitReadObject)
{
	ASSERT_DOMAIN();
	int readValue = bitReadObject->Get()->ReadLong();
	return readValue;
}

static float SharpBitReadReadFloat(BitReadSharpObject* bitReadObject)
{
	ASSERT_DOMAIN();
	return bitReadObject->Get()->ReadFloat();
}


static Vector SharpBitReadReadVector(BitReadSharpObject* bitReadObject)
{
	ASSERT_DOMAIN();
	Vector vec;
	bitReadObject->Get()->ReadBitVec3Coord(vec);

	return vec;
}

static Vector SharpBitReadReadVectorNormal(BitReadSharpObject* bitReadObject)
{
	ASSERT_DOMAIN();
	Vector vec;
	bitReadObject->Get()->ReadBitVec3Normal(vec);

	return vec;
}

static QAngle SharpBitReadReadAngles(BitReadSharpObject* bitReadObject)
{
	ASSERT_DOMAIN();
	QAngle angles;
	bitReadObject->Get()->ReadBitAngles(angles);

	return angles;
}

static MonoString* SharpBitReadReadString(BitReadSharpObject* bitReadObject)
{
	ASSERT_DOMAIN();
	char str[4096];

	bitReadObject->Get()->ReadString( str, sizeof(str) );

	return mono_string_new( g_Sharp.GetDomain(), str );
}

static int SharpBitReadReadBool(BitReadSharpObject* bitReadObject)
{
	ASSERT_DOMAIN();
	return bitReadObject->Get()->ReadOneBit();
}


static int SharpBitReadTotalBytesAvailable(BitReadSharpObject* bitReadObject)
{
	ASSERT_DOMAIN();
	return bitReadObject->Get()->TotalBytesAvailable();
}


static int SharpBitReadGetNumBytesLeft(BitReadSharpObject* bitReadObject)
{
	ASSERT_DOMAIN();
	return bitReadObject->Get()->GetNumBytesLeft();
}

static void SharpBitReadSeek(BitReadSharpObject* bitReadObject, int nPosition )
{
	ASSERT_DOMAIN();
	bitReadObject->Get()->Seek( nPosition );
}

static int SharpBitReadTell(BitReadSharpObject* bitReadObject )
{
	ASSERT_DOMAIN();
	return bitReadObject->Get()->GetNumBitsRead();
}

static void SharpBitReadDispose(BitReadSharpObject* bitReadObject )
{
	ASSERT_DOMAIN();
	bf_read* msgData = bitReadObject->Get();

	delete msgData;

	bitReadObject->Set( NULL );
}

#ifdef GAME_DLL
static bf_write *g_pMsgBuffer = NULL;
static SharpHandle g_UsrMsgHandle;
static SharpRecipientFilter g_sharpRecipientFilter;

static inline void PrepareMessageBegin()
{
	if( g_pMsgBuffer != NULL )
		mono_raise_exception ( mono_get_exception_invalid_operation("Another User Message was already written.") );


}

static void SharpUserMessageBegin ( MonoObject* recipientFilter, int msg_type ) {
	ASSERT_DOMAIN();
	PrepareMessageBegin();

	g_sharpRecipientFilter.SetFilter( recipientFilter );
	g_pMsgBuffer = engine->UserMessageBegin( &g_sharpRecipientFilter, msg_type );
	g_UsrMsgHandle = SharpObject( recipientFilter ).GetNewHandle();
};

static void StartEntityUserMessage( EntityMonoObject *monoEntity, bool reliable )
{
	ASSERT_DOMAIN();
	PrepareMessageBegin();

	CBaseEntity* entity = monoEntity->GetEntity();

	g_pMsgBuffer = engine->EntityMessageBegin( entity->entindex(), entity->GetServerClass(), reliable );
}

inline void VerifyBuffer()
{
	ASSERT_DOMAIN();
	if( g_pMsgBuffer == NULL ) 
		mono_raise_exception ( mono_get_exception_invalid_operation("No active buffer for user message.") );
}

static void SharpUserMessageWriteChar( int iVal ){
	VerifyBuffer();
	g_pMsgBuffer->WriteChar( iVal );
}


static void SharpUserMessageWriteByte( int iVal ){
	VerifyBuffer();
	g_pMsgBuffer->WriteByte( iVal );
}

static void SharpUserMessageWriteShort( int iVal ){
	VerifyBuffer();
	g_pMsgBuffer->WriteShort( iVal );
}

static void SharpUserMessageWriteLong( int iVal ){
	VerifyBuffer();
	g_pMsgBuffer->WriteLong( iVal );
}

static void SharpUserMessageWriteFloat( float flValue ){
	VerifyBuffer();
	g_pMsgBuffer->WriteFloat( flValue );
}

static void SharpUserMessageWriteVector( const Vector rgflValue ){
	VerifyBuffer();
	g_pMsgBuffer->WriteBitVec3Coord( rgflValue );
}

static void SharpUserMessageWriteVectorNormal( const Vector rgflValue ){
	VerifyBuffer();
	g_pMsgBuffer->WriteBitVec3Normal( rgflValue );
}

static void SharpUserMessageWriteAngles( const QAngle rgflValue ){
	VerifyBuffer();
	g_pMsgBuffer->WriteBitAngles( rgflValue );
}

static void SharpUserMessageWriteString( MonoString* monoString ){
	VerifyBuffer();
	char* sz = mono_string_to_utf8( monoString );	
	g_pMsgBuffer->WriteString( sz );
	mono_free( sz );
}

static void SharpUserMessageWriteBool( bool bValue ){
	VerifyBuffer();
	g_pMsgBuffer->WriteOneBit( bValue );
}

static int SharpUserMessageGetNumBytesWritten(){
	VerifyBuffer();
	return g_pMsgBuffer->GetNumBytesWritten();
}

static int SharpUserMessageGetNumBitsWritten(){
	VerifyBuffer();
	return g_pMsgBuffer->GetNumBitsWritten();
}

static int SharpUserMessageGetMaxNumBits(){
	VerifyBuffer();
	return g_pMsgBuffer->GetMaxNumBits();
}

static int SharpUserMessageGetNumBitsLeft(){
	VerifyBuffer();
	return g_pMsgBuffer->GetNumBitsLeft();
}

static int SharpUserMessageGetNumBytesLeft(){
	VerifyBuffer();
	return g_pMsgBuffer->GetNumBytesLeft();
}

static void SharpUserMessageEnd(){
	VerifyBuffer();
	engine->MessageEnd();
	g_pMsgBuffer = NULL;
	g_sharpRecipientFilter.SetFilter( NULL );

	if( g_UsrMsgHandle.IsValid() )
		g_UsrMsgHandle.Release();
}
#endif

#ifndef GAME_DLL
BitReadSharpObject* SharpUserMessage::NewBitReadSharpObject( bf_read &msg_data )
{
	BitReadSharpObject* bitRead = (BitReadSharpObject*) m_bitReadClass.NewInstance().m_monoObject;
	
	bf_read* msgData = new bf_read();
	*msgData = msg_data;

	bitRead->Set( msgData );

	return bitRead;
}

bool SharpUserMessage::DispatchUserMessage( int msg_type, bf_read &msg_data )
{
	
	SharpObject retn;
	BitReadSharpObject* bitRead = NewBitReadSharpObject( msg_data );

	void* args[] = {
		&msg_type,
		bitRead
	};

	if( !m_dispatchUserMessage.SafeInvoke( args, &retn ) )
		return false;

	bool* retnBool = (bool*) retn.Unbox();
	return *retnBool;
}
#endif


void SharpUserMessage::RegisterInternalCalls( void ){

#ifdef GAME_DLL
	mono_add_internal_call ("Sharp.UserMessage::Begin", SharpUserMessageBegin );
	mono_add_internal_call ("Sharp.UserMessage::BeginEntity", StartEntityUserMessage );
	mono_add_internal_call ("Sharp.UserMessage::WriteChar", SharpUserMessageWriteChar );
	mono_add_internal_call ("Sharp.UserMessage::WriteByte", SharpUserMessageWriteByte );
	mono_add_internal_call ("Sharp.UserMessage::WriteShort", SharpUserMessageWriteShort );
	mono_add_internal_call ("Sharp.UserMessage::WriteLong", SharpUserMessageWriteLong );
	mono_add_internal_call ("Sharp.UserMessage::WriteFloat", SharpUserMessageWriteFloat );
	mono_add_internal_call ("Sharp.UserMessage::WriteVector", SharpUserMessageWriteVector );
	mono_add_internal_call ("Sharp.UserMessage::WriteVectorNormal", SharpUserMessageWriteVectorNormal );
	mono_add_internal_call ("Sharp.UserMessage::WriteAngles", SharpUserMessageWriteAngles );
	mono_add_internal_call ("Sharp.UserMessage::WriteString", SharpUserMessageWriteString );
	mono_add_internal_call ("Sharp.UserMessage::WriteBool", SharpUserMessageWriteBool );

	mono_add_internal_call ("Sharp.UserMessage::GetNumBytesWritten", SharpUserMessageGetNumBytesWritten );
	mono_add_internal_call ("Sharp.UserMessage::GetNumBitsWritten", SharpUserMessageGetNumBitsWritten );
	mono_add_internal_call ("Sharp.UserMessage::GetMaxNumBits", SharpUserMessageGetMaxNumBits );
	mono_add_internal_call ("Sharp.UserMessage::GetNumBitsLeft", SharpUserMessageGetNumBitsLeft );
	mono_add_internal_call ("Sharp.UserMessage::GetNumBytesLeft", SharpUserMessageGetNumBytesLeft );

	mono_add_internal_call ("Sharp.UserMessage::End", SharpUserMessageEnd );
#endif

	mono_add_internal_call ("Sharp.BitRead::ReadChar", SharpBitReadReadChar );
	mono_add_internal_call ("Sharp.BitRead::ReadByte", SharpBitReadReadByte );
	mono_add_internal_call ("Sharp.BitRead::ReadShort", SharpBitReadReadShort );
	mono_add_internal_call ("Sharp.BitRead::ReadLong", SharpBitReadReadLong );
	mono_add_internal_call ("Sharp.BitRead::ReadFloat", SharpBitReadReadFloat );
	mono_add_internal_call ("Sharp.BitRead::ReadVector", SharpBitReadReadVector );
	mono_add_internal_call ("Sharp.BitRead::ReadVectorNormal", SharpBitReadReadVectorNormal );
	mono_add_internal_call ("Sharp.BitRead::ReadAngles", SharpBitReadReadAngles );
	mono_add_internal_call ("Sharp.BitRead::ReadString", SharpBitReadReadString );
	mono_add_internal_call ("Sharp.BitRead::ReadBool", SharpBitReadReadBool );
	mono_add_internal_call ("Sharp.BitRead::TotalBytesAvailable", SharpBitReadTotalBytesAvailable );
	mono_add_internal_call ("Sharp.BitRead::GetNumBytesLeft", SharpBitReadGetNumBytesLeft );
	mono_add_internal_call ("Sharp.BitRead::Seek", SharpBitReadSeek );
	mono_add_internal_call ("Sharp.BitRead::Tell", SharpBitReadTell );

	mono_add_internal_call ("Sharp.BitRead::Dispose", SharpBitReadDispose );
}


void SharpUserMessage::UpdateMehods( MonoImage* image ){

	SharpClass sharpClass = mono_class_from_name( image, "Sharp","RecipientFilter");

	m_getRecipientCount = sharpClass.GetMethod("GetRecipientCount", 0 );
	m_getRecipientIndex = sharpClass.GetMethod("GetRecipientIndex", 1 );
	m_isReliable = sharpClass.GetMethod("IsReliable", 0 );
	m_isInitMessage = sharpClass.GetMethod("IsInitMessage", 0 );

#ifdef GAME_DLL
	SharpClass userMessageHookClass = mono_class_from_name( image, "Sharp","UserMessageHook");

	m_getUserMessageInfo = userMessageHookClass.GetMethod("GetUserMessageInfo", 3);
#else
	m_bitReadClass = mono_class_from_name( image, "Sharp","BitRead");

	SharpClass userMessageHook =mono_class_from_name( image, "Sharp", "UserMessageHook");
	m_dispatchUserMessage = userMessageHook.GetMethod("DispatchUserMessage", 2 );
#endif
}

#ifdef GAME_DLL
bool SharpUserMessage::GetUserMessageInfo( int msg_type, char *name, int maxnamelength, int& size )
{
	UPDATE_DOMAIN();

	MonoString* monoString;
	SharpObject returnValue;

	void* args[] = {
		&msg_type,
		&monoString,
		&size
	};

	if( !m_getUserMessageInfo.SafeInvoke( args, &returnValue ) )
		return false;

	bool retn = *((bool*) returnValue.Unbox());

	if( !retn )
		return false;

	char* pchMsgName = mono_string_to_utf8( monoString );
	Q_strncpy( name, pchMsgName, maxnamelength );
	mono_free( pchMsgName );

	return retn;
}
#endif