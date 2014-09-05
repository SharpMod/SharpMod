#include <cstring>
#include "sharp_native_entity.h"
#include "cbase.h"
#include "mono/metadata/threads.h"
#include "sharp/entitylistner.h"
#include "sharp/sharphelper.h"
#include <mono/metadata/tokentype.h>
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"




class CSharpNativeEntity : public CBaseAnimating
{
public:
	DECLARE_CLASS( CSharpNativeEntity, CBaseAnimating );
	//DECLARE_SERVERCLASS();
	//DECLARE_DATADESC();

	CSharpNativeEntity( const NativeEntityData* entityData ) : m_monoEntityData(entityData){
	};

	virtual	bool	IsSharpEntity( void ) const { return false; }

#ifdef CLIENT_DLL
	virtual ClientClass* GetClientClass() OVERRIDE {
		return m_monoEntityData->m_clientClass;	
	};
#else
	virtual ServerClass* GetServerClass() OVERRIDE {
		return m_monoEntityData->m_serverClass;	
	};

#endif

	virtual SharpClass GetMonoClass() OVERRIDE { 
		return m_monoEntityData->m_monoClass; 
	};

private:
	const NativeEntityData* m_monoEntityData;

};

SharpNativeDT g_nativeDT;

#ifndef CLIENT_DLL
CBaseAnimating* SharpNativeDT::CreateEntity( const char *szClassname ){

	for(auto &elem : m_entities ){

		if( std::strcmp( elem.m_entityName.c_str(), szClassname ) != 0 )
			continue;
		
		CSharpNativeEntity* native = new CSharpNativeEntity(&elem);
		native->PostConstructor(szClassname);
		return native;
	}

	return nullptr;
}
#endif


void SharpNativeDT::LoadSharpImage(MonoImage* image)
{
	m_sharpImage = image;
	m_entityAttrClass = mono_class_from_name( image, "Sharp", "EntityAttribute");
	m_entitynetworkedAttrClass = mono_class_from_name( image, "Sharp", "NetworkedAttribute");
	m_animatingClass = mono_class_from_name( image, "Sharp", "BaseAnimating");
	

	MonoClass *klass;
	int i;
	int rows = mono_image_get_table_rows(image, MONO_TABLE_TYPEDEF );

	for (i = 1; i <= rows; ++i) {
		klass = mono_class_get (image, MONO_TOKEN_TYPE_DEF | i);
		auto attrInfo = mono_custom_attrs_from_class(klass);

		if( attrInfo != nullptr ) {
			if( mono_custom_attrs_has_attr( attrInfo, m_entityAttrClass.m_monoClass ) ){
				//auto name = mono_class_get_name(klass);
				MonoObject* attr = mono_custom_attrs_get_attr( attrInfo, m_entityAttrClass.m_monoClass );

				RegisterClass(klass, static_cast<SharpEntityAttribute*>( attr ) );
			}
			mono_custom_attrs_free(attrInfo);
		}

		
	}


}

#ifdef CLIENT_DLL
static IClientNetworkable* CreateNativeSharpEntity( int entnum, int serialNum )
{
	auto ent = new CSharpNativeEntity(&g_nativeDT.m_entities.front());

	if(ent == nullptr)
		return nullptr;

	ent->Init(entnum, serialNum);
	return ent;
}

static void SharpNativeRecvPropInt( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	auto entity = static_cast<const CSharpNativeEntity*>(pStruct);
	auto monoObject = entity->GetSharpEntity();
	auto field = static_cast<MonoClassField*>(const_cast<void*>(pData->m_pRecvProp->GetExtraData()));
	int value = pData->m_Value.m_Int;

	mono_field_set_value(monoObject, field, &value );
}

static RecvProp RegisterIntProp(const char* fieldName, MonoClassField * field, MonoObject* attribute){
	return RecvPropInt(fieldName, 0, 4, 0, SharpNativeRecvPropInt );
}

#else

static void SharpNativeSendPropInt( const SendProp *pProp, const void *pStructBase, const void *pData, DVariant *pOut, int iElement, int objectID )
{
	auto entity = static_cast<const CSharpNativeEntity*>(pStructBase);
	auto monoObject = entity->GetSharpEntity();
	auto field = static_cast<MonoClassField*>(const_cast<void*>(pProp->GetExtraData()));

	mono_field_get_value(monoObject, field, &pOut->m_Int);
}

static SendProp RegisterIntProp(const char* fieldName, MonoClassField * field, MonoObject* attribute){
	return SendPropInt(fieldName, 0, 4, -1, 0, SharpNativeSendPropInt );
}


#endif

void SharpNativeDT::RegisterClass(MonoClass *klass, SharpEntityAttribute* attr)
{

	if(!mono_class_is_subclass_of(klass, m_animatingClass.m_monoClass, false) )
		return;

	m_entities.emplace_back(NativeEntityData());
	NativeEntityData &data = m_entities.back();	

#ifdef CLIENT_DLL
	RecvTable* table = new RecvTable();
	data.m_props.reset( new std::vector<RecvProp>() ); 
#else
	SendTable* table = new SendTable();
	data.m_props.reset( new std::vector<SendProp>() ); 
#endif
	
	MonoClassField * field;
	void* iter = nullptr;
	char* entityName = mono_string_to_utf8(attr->className); //like prop_physics

	

#ifdef CLIENT_DLL
	data.m_props->push_back(RecvPropDataTable("baseclass", 0, 0, CBaseAnimating::m_pClassRecvTable));
#else
	data.m_props->push_back(SendPropDataTable("baseclass", 0, CBaseAnimating::m_pClassSendTable, SendProxy_DataTableToDataTable));
#endif

	while ((field = mono_class_get_fields (klass, &iter)) != nullptr) {
		if( mono_field_get_flags(field) & 0x0010 ) // FIELD_ATTRIBUTE_STATIC
			continue;

		Msg("Looking at field: %s\n", mono_field_get_name(field));

		//TODO: This item is not collected in all paths
		auto fieldAttr = mono_custom_attrs_from_field(klass, field);

		if(fieldAttr == nullptr || !mono_custom_attrs_has_attr( fieldAttr, m_entitynetworkedAttrClass.m_monoClass ) )
			continue;

		const char* fieldName = mono_field_get_name(field);
		MonoObject* attrs = mono_custom_attrs_get_attr( fieldAttr, m_entitynetworkedAttrClass.m_monoClass );
		auto fieldClass = mono_class_from_mono_type( mono_field_get_type(field) );


#ifdef CLIENT_DLL
		RecvProp prop;
#else
		SendProp prop;
#endif

		if( fieldClass == mono_get_int32_class() || fieldClass == mono_get_uint32_class() ){
			prop = RegisterIntProp(fieldName, field, attrs);
		} else {
			Warning("Unable to register %s::%s type %s\n", entityName, fieldName, mono_class_get_name( fieldClass ) );
			continue;
		}


		prop.SetExtraData(field);
		data.m_props->push_back(prop);

		mono_custom_attrs_free(fieldAttr);

	}

	auto className = mono_class_get_name(klass);
	table->Construct(data.m_props->data(), data.m_props->size(), className );
	
	data.m_monoClass = klass;
	data.m_entityName = std::string(entityName);

#ifdef CLIENT_DLL
	data.m_clientClass = new ClientClass( className, CreateNativeSharpEntity, nullptr, table );
#else
	data.m_serverClass = new ServerClass( className, table );
#endif

	mono_free(entityName);
}