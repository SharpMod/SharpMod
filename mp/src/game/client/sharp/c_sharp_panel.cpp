#include "cbase.h"
#include "sharp/sharp.h"
#include <vgui_controls/Controls.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Label.h>
#include "sharp/sharphelper.h"
#include "hud.h"
#include "iclientmode.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"

class SharpFont : public MonoObject 
{
public:
	vgui::HFont font;
};

class SharpPanelManager : public IMonoAddon {
public:
	SharpPanelManager();

	virtual void RegisterInternalCalls();
	virtual void UpdateMehods( MonoImage* image );

	SharpMethod m_paintMethod;
	SharpMethod m_thinkMethod;
	SharpMethod m_cursorEnteredMethod;
	SharpMethod m_cursorExitedMethod;
	SharpMethod m_mousePressedMethod;
	SharpMethod m_mouseDoublePressedMethod;
	SharpMethod m_mouseReleasedMethod;
	SharpMethod m_mouseWheeledMethod;
	SharpMethod m_performLayout;
	SharpMethod m_paintBackground;
	SharpMethod m_onClick;
};

SharpPanelManager sharpPanel;

class PanelMonoObject : public TemplateSharpObject<vgui::Panel> {
};


template <class T> 
class SharpTemplatePanel : public T {
	SharpHandle m_monoHandle;

	SharpMethod m_paintMethod;
	SharpMethod m_thinkMethod;
	SharpMethod m_cursorEnteredMethod;
	SharpMethod m_cursorExitedMethod;
	SharpMethod m_mousePressedMethod;
	SharpMethod m_mouseDoublePressedMethod;
	SharpMethod m_mouseReleasedMethod;
	SharpMethod m_mouseWheeledMethod;
	SharpMethod m_performLayout;
	SharpMethod m_paintBackground;

public:
	SharpTemplatePanel(vgui::Panel* parent) : T(parent){}; //vgui::Panel(GetClientMode()->GetViewport())
	SharpTemplatePanel(vgui::Panel *parent, const char *panelName) : T(parent, panelName){};
	SharpTemplatePanel(vgui::Panel *parent, const char *panelName, const char *text) : T(parent, panelName, text){};

	virtual void InitMono( PanelMonoObject* object )
	{
		//AssertMsg( object->Get() == NULL, "Error! Setting the panel twice for a mono panel.");
		object->Set( this );

		SharpObject sharpObj( object );

		m_paintMethod = sharpObj.GetVirtual( sharpPanel.m_paintMethod );
		m_thinkMethod = sharpObj.GetVirtual( sharpPanel.m_thinkMethod );
		m_cursorEnteredMethod = sharpObj.GetVirtual( sharpPanel.m_cursorEnteredMethod );
		m_cursorExitedMethod = sharpObj.GetVirtual( sharpPanel.m_cursorExitedMethod );
		m_mousePressedMethod = sharpObj.GetVirtual( sharpPanel.m_mousePressedMethod );
		m_mouseDoublePressedMethod = sharpObj.GetVirtual( sharpPanel.m_mouseDoublePressedMethod );
		m_mouseReleasedMethod = sharpObj.GetVirtual( sharpPanel.m_mouseReleasedMethod );
		m_mouseWheeledMethod = sharpObj.GetVirtual( sharpPanel.m_mouseWheeledMethod );
		m_performLayout = sharpObj.GetVirtual( sharpPanel.m_performLayout );
		m_paintBackground = sharpObj.GetVirtual( sharpPanel.m_paintBackground );

		m_monoHandle = sharpObj.GetNewHandle(); //mono_gchandle_new( object, false );
	}

	virtual ~SharpTemplatePanel(){
		if( m_monoHandle.IsValid() ){
			PanelMonoObject* monoObject = GetMonoObject();
			monoObject->Set( NULL );
			m_monoHandle.Release();
		}
	};

	PanelMonoObject* GetMonoObject()
	{
		return (PanelMonoObject*) m_monoHandle.GetObject().m_monoObject;
	}

	virtual void Paint()
	{
		CallMethod(m_paintMethod);
	};

	void BasePaint()
	{
		T::Paint();
	}

	virtual void PaintBackground()
	{
		CallMethod(m_paintBackground);
	};

	void BasePaintBackground()
	{
		T::PaintBackground();
	}

	virtual void OnThink()
	{
		T::OnThink();
		CallMethod(m_thinkMethod);
	};

	virtual void PerformLayout()
	{
		T::PerformLayout();
		CallMethod(m_performLayout);
	};

	virtual void OnCursorEntered()
	{
		T::OnCursorEntered();
		CallMethod(m_cursorEnteredMethod);
	};

	virtual void OnCursorExited()
	{
		T::OnCursorExited();
		CallMethod(m_cursorExitedMethod);
	}

	virtual void OnMousePressed(vgui::MouseCode code)
	{
		void* args[1] = { &code };
		CallMethod(m_mousePressedMethod, args);
	}

	virtual void OnMouseDoublePressed(vgui::MouseCode code)
	{
		void* args[1] = { &code };
		CallMethod(m_mouseDoublePressedMethod, args);
	}

	virtual void OnMouseReleased(vgui::MouseCode code)
	{
		void* args[1] = { &code };
		CallMethod(m_mouseReleasedMethod, args);
	}

	void BaseOnMouseReleased(vgui::MouseCode code)
	{
		//I am probably going to shoot myself in the foot with this
		T::OnMouseReleased(code);
	}

	void BaseOnMousePressed(vgui::MouseCode code)
	{
		T::OnMousePressed(code);
	}

	virtual void OnMouseWheeled(int delta)
	{
		void* args[1] = { &delta };
		CallMethod(m_mouseWheeledMethod, args);
	}

	void CallMethod( SharpMethod method, void** params = NULL )
	{
		UPDATE_DOMAIN();

		method.SafeInvoke( m_monoHandle.GetObject(), params );
	}

	/*
	static void GetSharpParent(PanelMonoObject* monoPanel )
	{
		SharpTemplatePanel<T>* panel = dynamic_cast<SharpTemplatePanel<T>*>(monoPanel->Get());

		if( !panel.m_parentHandle.IsValid() )
			return NULL;

		return panel->m_parentHandle.GetObject().m_monoObject;
	}
	*/
};




class SharpPanel : public SharpTemplatePanel<vgui::Panel>
{
public:
	SharpPanel() : SharpTemplatePanel<vgui::Panel>(g_pClientMode->GetViewport())
	{

	}

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme)
	{
	}

};

class SharpLabelPanel : public SharpTemplatePanel<vgui::Label>
{
public:
	SharpFont* m_iFont;

	SharpLabelPanel(const char *text) : SharpTemplatePanel<vgui::Label>(g_pClientMode->GetViewport(), NULL, text), m_iFont(NULL)
	{

	}

	vgui::Label::Alignment GetContentAlignment(){
		return _contentAlignment;
	}
};

class SharpImagePanel : public SharpTemplatePanel<vgui::ImagePanel>
{
public:
	SharpImagePanel() : SharpTemplatePanel<vgui::ImagePanel>(g_pClientMode->GetViewport(), NULL)
	{
	}
};

class SharpButtonPanel : public SharpTemplatePanel<vgui::Button>
{
public:
	SharpMethod m_onClick;

	SharpButtonPanel(char* input) : SharpTemplatePanel<vgui::Button>(g_pClientMode->GetViewport(), NULL, input)
	{
	}

	virtual void InitMono( PanelMonoObject* object )
	{
		SharpTemplatePanel<vgui::Button>::InitMono(object);

		SharpObject sharpObj( object );
		m_onClick = sharpObj.GetVirtual( sharpPanel.m_onClick );
	}

	virtual void DoClick()
	{
		CallMethod(m_onClick);
	}

	void BaseDoClick()
	{
		vgui::Button::DoClick();
	}

};



static void SetColor( Color color ){
	ASSERT_DOMAIN();
	vgui::surface()->DrawSetColor( color );
}

static void DrawFilledRect(int x0, int y0, int x1, int y1){
	ASSERT_DOMAIN();
	vgui::surface()->DrawFilledRect( x0, y0, x1, y1 );
}

static void DrawOutlinedRect(int x0, int y0, int x1, int y1){
	ASSERT_DOMAIN();
	vgui::surface()->DrawOutlinedRect( x0, y0, x1, y1 );
}

static void DrawLine(int x0, int y0, int x1, int y1){
	ASSERT_DOMAIN();
	vgui::surface()->DrawLine( x0, y0, x1, y1 );
}

static void PlaySound(MonoString* str){
	ASSERT_DOMAIN();

	if (str == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("sound"));

	char* input = mono_string_to_utf8( str );
	vgui::surface()->PlaySound( input );
	mono_free( input );
}

static int CreateNewTextureID(){
	ASSERT_DOMAIN();
	return vgui::surface()->CreateNewTextureID();
}

static void DestroyTextureID(int id){
	ASSERT_DOMAIN();
	vgui::surface()->DestroyTextureID(id);
}

static void DrawSetTextureFile( int id, MonoString* monoName, bool hardwareFilter, bool forceReload ){
	ASSERT_DOMAIN();

	if (monoName == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("name"));

	char* name = mono_string_to_utf8( monoName );

	vgui::surface()->DrawSetTextureFile(id, name, hardwareFilter, forceReload );

	mono_free( name );	
}

static int GetTexture( MonoString* monoFilename )
{
	ASSERT_DOMAIN();
	char* name = mono_string_to_utf8( monoFilename );
	int id = vgui::surface()->DrawGetTextureId( name );
	mono_free( name );

	return id;
}

static void SetTexture( int id )
{
	ASSERT_DOMAIN();
	vgui::surface()->DrawSetTexture( id );
}

static void GetTextureSize( int id, int& wide, int& tall )
{
	ASSERT_DOMAIN();
	vgui::surface()->DrawGetTextureSize( id, wide, tall );
}

static void DrawTexturedRect(int x0, int y0, int x1, int y1)
{
	ASSERT_DOMAIN();
	vgui::surface()->DrawTexturedRect(x0, y0, x1, y1);
}

static void DrawTexturedPolygon(MonoArray* monoVertexes, bool ClipVertices )
{
	ASSERT_DOMAIN();

	if (monoVertexes == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("vertexes"));

	int size = mono_array_length( monoVertexes );
	vgui::Vertex_t* verticies = (vgui::Vertex_t*) stackalloc( sizeof(vgui::Vertex_t) * size );

	if(verticies == nullptr)
		mono_raise_exception (mono_get_exception_out_of_memory());

	//One can be a smartass, and just dereference the first item, and pass that into DrawTexturedPolygon
	//Take a look at: https://github.com/mono/mono/blob/ac653a9186b2724ee9f80f93a6b26005f7d79828/mono/metadata/object.c#L6538
	for( int i = 0; i < size; i++ ){
		vgui::Vertex_t* arrayItem = (vgui::Vertex_t*) mono_array_addr_with_size(monoVertexes, sizeof(vgui::Vertex_t), i);

		verticies[i] = *arrayItem;
	}

	vgui::surface()->DrawTexturedPolygon(size, verticies, ClipVertices);
	
	stackfree( verticies );
}

struct Position
{
	int x;
	int y;
};

static Position GetPanelPos ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	Position pos;
	monoPanel->Get()->GetPos( pos.x, pos.y );
	return pos;
};

static void SetPanelPos ( PanelMonoObject *monoPanel, Position pos ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->SetPos( pos.x, pos.y );
};

static int GetPanelWide ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	return monoPanel->Get()->GetWide();
};

static void SetPanelWide ( PanelMonoObject *monoPanel, int wide ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->SetWide( wide );
};

static int GetPanelTall ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	return monoPanel->Get()->GetTall();
};

static void SetPanelTall ( PanelMonoObject *monoPanel, int tall ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->SetTall( tall );
};

static int GetPanelZPos ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	return monoPanel->Get()->GetZPos();
};

static void SetPanelZPos ( PanelMonoObject *monoPanel, int z ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->SetZPos( z );
};

static void SetPanelParent ( PanelMonoObject *monoPanel, PanelMonoObject *monoParent ) {
	ASSERT_DOMAIN();
	vgui::Panel* panel = monoPanel->Get();

	if( monoParent == NULL ){
		panel->SetParent( g_pClientMode->GetViewport() );
		return;
	}

	vgui::Panel* parent = monoParent->Get();

	panel->SetParent( parent );
};

static MonoObject* GetPanelParent(PanelMonoObject* monoPanel )
{
	ASSERT_DOMAIN();
	//Kill me, but there is no better way, since I can not edit vgui::Panel
	vgui::Panel* parent = monoPanel->Get()->GetParent();
	SharpPanel* panel = dynamic_cast<SharpPanel*>(parent);

	if( panel )
		return panel->GetMonoObject();

	SharpLabelPanel* panel2 = dynamic_cast<SharpLabelPanel*>(parent);
	if( panel2 )
		return panel2->GetMonoObject();

	SharpImagePanel* panel3 = dynamic_cast<SharpImagePanel*>(parent);
	if( panel3 )
		return panel3->GetMonoObject();

	return NULL;
}

static Color GetForegroundColor ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	return monoPanel->Get()->GetFgColor();
};

static void SetForegroundColor ( PanelMonoObject *monoPanel, Color color ) {
	ASSERT_DOMAIN();
	vgui::Panel* panel = monoPanel->Get();
	panel->SetFgColor( color );
};

static Color GetBackgroundColor ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	return monoPanel->Get()->GetBgColor();
};

static void SetBackgroundColor ( PanelMonoObject *monoPanel, Color color ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->SetBgColor( color );
};

static bool GetPanelVisible ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	return monoPanel->Get()->IsVisible();
};

static void SetPanelVisible( PanelMonoObject *monoPanel, bool visible ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->SetVisible( visible );
};

static bool GetPanelEnabled(PanelMonoObject *monoPanel) {
	ASSERT_DOMAIN();
	return monoPanel->Get()->IsEnabled();
};

static void SetPanelEnabled(PanelMonoObject *monoPanel, bool visible) {
	ASSERT_DOMAIN();
	monoPanel->Get()->SetEnabled(visible);
};

static bool GetAllowMouseInput ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	return monoPanel->Get()->IsMouseInputEnabled();
};

static void SetAllowMouseInput( PanelMonoObject *monoPanel, bool state ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->SetMouseInputEnabled( state );
};

static bool GetAllowKeyboardInput ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	return monoPanel->Get()->IsKeyBoardInputEnabled();
};

static void SetAllowKeyboardInput( PanelMonoObject *monoPanel, bool state ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->SetKeyBoardInputEnabled( state );
};

static void PanelInvalidateLayout( PanelMonoObject *monoPanel, bool layoutNow, bool layout_schema ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->InvalidateLayout( layoutNow, layout_schema );
};

static void PanelMakePopup(PanelMonoObject *monoPanel) {
	ASSERT_DOMAIN();
	monoPanel->Get()->MakePopup();
};

static void PanelMoveToFront(PanelMonoObject *monoPanel) {
	ASSERT_DOMAIN();
	monoPanel->Get()->MoveToFront();
};

static bool GetAutoDelete ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	return monoPanel->Get()->IsAutoDeleteSet();
};

static void SetAutoDelete( PanelMonoObject *monoPanel, bool state ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->SetAutoDelete( state );
};

static void PanelBaseImagePaint( TemplateSharpObject<SharpImagePanel> *monoPanel )
{
	ASSERT_DOMAIN();
	monoPanel->Get()->BasePaint();
};

static void PanelBaseImagePaintBackground( TemplateSharpObject<SharpImagePanel> *monoPanel )
{
	ASSERT_DOMAIN();
	monoPanel->Get()->BasePaintBackground();
};

static void PanelBaseLabelPaint( TemplateSharpObject<SharpLabelPanel> *monoPanel )
{
	ASSERT_DOMAIN();
	monoPanel->Get()->BasePaint();
};

static void PanelBaseLabelPaintBackground( TemplateSharpObject<SharpLabelPanel> *monoPanel )
{
	ASSERT_DOMAIN();
	monoPanel->Get()->BasePaintBackground();
};


static void LabelPanelSizeToContents( TemplateSharpObject<SharpLabelPanel> *monoPanel ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->SizeToContents();
};

static vgui::Label::Alignment LabelPanelGetAlignment ( TemplateSharpObject<SharpLabelPanel> *monoPanel ) {
	ASSERT_DOMAIN();
	return monoPanel->Get()->GetContentAlignment();
};

static void LabelPanelSetAlignment ( TemplateSharpObject<SharpLabelPanel> *monoPanel, vgui::Label::Alignment alignment ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->SetContentAlignment( alignment );
};

static MonoObject* LabelPanelGetFont ( TemplateSharpObject<SharpLabelPanel> *monoPanel ) {
	ASSERT_DOMAIN();
	return monoPanel->Get()->m_iFont;
};

static void LabelPanelSetFont ( TemplateSharpObject<SharpLabelPanel> *monoPanel, SharpFont* font ) {
	ASSERT_DOMAIN();
	if (font == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("font"));

	SharpLabelPanel* labelPanel = monoPanel->Get();

	labelPanel->m_iFont = font;
	labelPanel->SetFont( font->font );
};


static MonoString* LabelPanelGetText ( TemplateSharpObject<SharpLabelPanel> *monoPanel ) {
	ASSERT_DOMAIN();
	char buffer[4096];
	
	monoPanel->Get()->GetText( buffer, sizeof(buffer) );

	return mono_string_new( mono_domain_get() ,buffer );
};

static void LabelPanelSetText ( TemplateSharpObject<SharpLabelPanel> *monoPanel, MonoString* str ) {
	ASSERT_DOMAIN();
	if (str == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("string"));

	SharpLabelPanel* labelPanel = monoPanel->Get();
	char* input = mono_string_to_utf8( str );

	labelPanel->SetText( input );
	labelPanel->SetContentAlignment( vgui::Label::a_northwest );

	mono_free( input );		
};


static MonoString* ImagePanelGetImage ( TemplateSharpObject<SharpImagePanel> *monoPanel ) {
	ASSERT_DOMAIN();
	const char* imageName = monoPanel->Get()->GetImageName();

	if( imageName == NULL )
		return NULL;

	return mono_string_new(g_Sharp.GetDomain(), imageName );
};

static void ImagePanelSetImage ( TemplateSharpObject<SharpImagePanel> *monoPanel, MonoString* panelStr ) {
	ASSERT_DOMAIN();

	if (panelStr == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("image"));

	SharpImagePanel* panel = monoPanel->Get();

	char* imageName = mono_string_to_utf8( panelStr );

	panel->SetImage( imageName );

	mono_free( imageName );
};


static void CreatePanel ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	SharpPanel* panel = new SharpPanel();
	panel->InitMono( monoPanel );
};

static void CreateLabelPanel ( PanelMonoObject *monoPanel, MonoString* str ) {
	ASSERT_DOMAIN();

	if (str == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("string"));

	char* input = mono_string_to_utf8( str );
	SharpLabelPanel* panel = new SharpLabelPanel( input );
	panel->InitMono( monoPanel );
	mono_free( input );		
	panel->SizeToContents();
};

static void CreateImagePanel ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	SharpImagePanel* panel = new SharpImagePanel();
	panel->InitMono( monoPanel );
};



static void DestroyPanel ( PanelMonoObject *monoPanel ) {
	ASSERT_DOMAIN();
	monoPanel->Get()->DeletePanel();
	//monoPanel->Set(NULL);
};


static int GetScreenWide()
{
	ASSERT_DOMAIN();
	int screenWide, screenTall;
	vgui::surface()->GetScreenSize( screenWide, screenTall );
	return screenWide;
}

static int GetScreenTall()
{
	ASSERT_DOMAIN();
	int screenWide, screenTall;
	vgui::surface()->GetScreenSize( screenWide, screenTall );
	return screenTall;
}


static void CreateSharpFont ( SharpFont *monoPanel, MonoString* fontName, int tall, int width, int blur, int scanlines, int flags ) {
	ASSERT_DOMAIN();

	if (fontName == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("font"));

	char* input = mono_string_to_utf8( fontName );

	monoPanel->font = vgui::surface()->CreateFont();
	vgui::surface()->SetFontGlyphSet( 
		monoPanel->font,
		input,
		tall,
		width,
		blur,
		scanlines,
		flags
		);

	mono_free( input );
};

static int SharpFontGetTall ( SharpFont *monoPanel ) {
	ASSERT_DOMAIN();
	return vgui::surface()->GetFontTall( monoPanel->font );
};

static bool SharpFontFontAdditive ( SharpFont *monoPanel ) {
	ASSERT_DOMAIN();
	return vgui::surface()->IsFontAdditive( monoPanel->font );
};


//#include "localize/ilocalize.h"

static void SharpFontGetTextSize ( SharpFont *monoPanel, MonoString* monoStr, int& width, int& height ) {
	ASSERT_DOMAIN();

	if (monoStr == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("string"));

	char* input = mono_string_to_utf8( monoStr );
	wchar_t szButtonConverted[2048];

	//Localize no longer exists
	//g_pLocalize->ConvertANSIToUnicode( input, szButtonConverted, sizeof( szButtonConverted ) );

	vgui::surface()->GetTextSize( monoPanel->font, szButtonConverted, width, height );
	mono_free( input );
};

static void CreateButtonPanel ( PanelMonoObject *monoPanel, MonoString* str ) {
	ASSERT_DOMAIN();

	if (str == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("string"));

	char* input = mono_string_to_utf8( str );
	SharpButtonPanel* panel = new SharpButtonPanel( input );
	panel->InitMono( monoPanel );
	mono_free( input );		
	panel->SizeToContents();
};
static SharpMethodItem SharpCtorButton("Sharp.Button::.ctor", CreateButtonPanel );

static void ButtonOnMouseReleased(TemplateSharpObject<SharpButtonPanel> *monoPanel, vgui::MouseCode button) {
	ASSERT_DOMAIN();

	monoPanel->Get()->BaseOnMouseReleased(button);
};
static SharpMethodItem ButtonOnMouseReleasedItem("Sharp.Button::OnMouseReleased", ButtonOnMouseReleased);


static void ButtonOnMousePressed(TemplateSharpObject<SharpButtonPanel> *monoPanel, vgui::MouseCode button) {
	ASSERT_DOMAIN();

	monoPanel->Get()->BaseOnMousePressed(button);
};
static SharpMethodItem ButtonOnMousePressedItem("Sharp.Button::OnMousePressed", ButtonOnMousePressed);

static void ButtonOnClick(TemplateSharpObject<SharpButtonPanel> *monoPanel) {
	ASSERT_DOMAIN();

	monoPanel->Get()->BaseDoClick();
};
static SharpMethodItem ButtonOnClickItem("Sharp.Button::OnClick", ButtonOnClick);

SharpPanelManager::SharpPanelManager(){
	SharpAddons()->AddAddon( this );
};

void SharpPanelManager::RegisterInternalCalls(){
		mono_add_internal_call ("Sharp.Surface::SetColor", SetColor);
		mono_add_internal_call ("Sharp.Surface::DrawFilledRect", DrawFilledRect);
		mono_add_internal_call ("Sharp.Surface::DrawOutlinedRect", DrawOutlinedRect);
		mono_add_internal_call ("Sharp.Surface::DrawLine", DrawLine);
		mono_add_internal_call ("Sharp.Surface::PlaySound", PlaySound);
		mono_add_internal_call ("Sharp.Surface::CreateNewTextureID", CreateNewTextureID);
		mono_add_internal_call ("Sharp.Surface::DestroyTextureID", DestroyTextureID);
		mono_add_internal_call ("Sharp.Surface::DrawSetTextureFile", DrawSetTextureFile);
		mono_add_internal_call ("Sharp.Surface::GetTextureRaw", GetTexture);
		mono_add_internal_call ("Sharp.Surface::SetTexture", SetTexture);
		mono_add_internal_call ("Sharp.Surface::GetTextureSize", GetTextureSize);
		mono_add_internal_call ("Sharp.Surface::DrawTexturedRect", DrawTexturedRect);
		mono_add_internal_call ("Sharp.Surface::DrawTexturedPolygon", DrawTexturedPolygon);
		mono_add_internal_call ("Sharp.Surface::get_ScreenWide", GetScreenWide);
		mono_add_internal_call ("Sharp.Surface::get_ScreenTall", GetScreenTall);

		mono_add_internal_call ("Sharp.Panel::.ctor", CreatePanel );
		mono_add_internal_call ("Sharp.Panel::Dispose", DestroyPanel );
		mono_add_internal_call ("Sharp.Panel::InvalidateLayout", PanelInvalidateLayout );
		mono_add_internal_call("Sharp.Panel::MakePopup", PanelMakePopup);
		mono_add_internal_call("Sharp.Panel::MoveToFront", PanelMoveToFront);

		mono_add_internal_call ("Sharp.Panel::get_Position", GetPanelPos );
		mono_add_internal_call ("Sharp.Panel::set_Position", SetPanelPos );
		mono_add_internal_call ("Sharp.Panel::get_Wide", GetPanelWide );
		mono_add_internal_call ("Sharp.Panel::set_Wide", SetPanelWide );
		mono_add_internal_call ("Sharp.Panel::get_Tall", GetPanelTall );
		mono_add_internal_call ("Sharp.Panel::set_Tall", SetPanelTall );
		mono_add_internal_call ("Sharp.Panel::get_ZPos", GetPanelZPos );
		mono_add_internal_call ("Sharp.Panel::set_ZPos", SetPanelZPos );
		mono_add_internal_call ("Sharp.Panel::get_Visible", GetPanelVisible );
		mono_add_internal_call ("Sharp.Panel::set_Visible", SetPanelVisible );
		mono_add_internal_call("Sharp.Panel::get_Enabled", GetPanelEnabled);
		mono_add_internal_call("Sharp.Panel::set_Enabled", SetPanelEnabled);
		mono_add_internal_call ("Sharp.Panel::get_Parent", GetPanelParent );
		mono_add_internal_call ("Sharp.Panel::set_Parent", SetPanelParent );
		mono_add_internal_call ("Sharp.Panel::get_AllowMouseInput", GetAllowMouseInput );
		mono_add_internal_call ("Sharp.Panel::set_AllowMouseInput", SetAllowMouseInput );
		mono_add_internal_call ("Sharp.Panel::get_AllowKeyboardInput", GetAllowKeyboardInput );
		mono_add_internal_call ("Sharp.Panel::set_AllowKeyboardInput", SetAllowKeyboardInput );
		mono_add_internal_call ("Sharp.Panel::get_AutoDelete", GetAutoDelete );
		mono_add_internal_call ("Sharp.Panel::set_AutoDelete", SetAutoDelete );
		mono_add_internal_call ("Sharp.Panel::get_ForegroundColor", GetForegroundColor );
		mono_add_internal_call ("Sharp.Panel::set_ForegroundColor", SetForegroundColor );
		mono_add_internal_call ("Sharp.Panel::get_BackgroundColor", GetBackgroundColor );
		mono_add_internal_call ("Sharp.Panel::set_BackgroundColor", SetBackgroundColor );

		mono_add_internal_call ("Sharp.Label::.ctor", CreateLabelPanel );
		mono_add_internal_call ("Sharp.Label::SizeToContents", LabelPanelSizeToContents );
		mono_add_internal_call ("Sharp.Label::Paint", PanelBaseLabelPaint );
		mono_add_internal_call ("Sharp.Label::PaintBackground", PanelBaseLabelPaintBackground );
		mono_add_internal_call ("Sharp.Label::get_Alignment", LabelPanelGetAlignment );
		mono_add_internal_call ("Sharp.Label::set_Alignment", LabelPanelSetAlignment );
		mono_add_internal_call ("Sharp.Label::get_Font", LabelPanelGetFont );
		mono_add_internal_call ("Sharp.Label::set_Font", LabelPanelSetFont );
		mono_add_internal_call ("Sharp.Label::get_Text", LabelPanelGetText );
		mono_add_internal_call ("Sharp.Label::set_Text", LabelPanelSetText );


		mono_add_internal_call ("Sharp.ImagePanel::.ctor", CreateImagePanel );
		mono_add_internal_call ("Sharp.ImagePanel::get_Image", ImagePanelGetImage );
		mono_add_internal_call ("Sharp.ImagePanel::set_Image", ImagePanelSetImage );
		mono_add_internal_call ("Sharp.ImagePanel::Paint", PanelBaseImagePaint );
		mono_add_internal_call ("Sharp.ImagePanel::PaintBackground", PanelBaseImagePaintBackground );


		mono_add_internal_call ("Sharp.Font::.ctor", CreateSharpFont );
		mono_add_internal_call ("Sharp.Font::GetTall", SharpFontGetTall );
		mono_add_internal_call ("Sharp.Font::IsFontAdditive", SharpFontFontAdditive );
		mono_add_internal_call ("Sharp.Font::GetTextSize", SharpFontGetTextSize );



		
};

void SharpPanelManager::UpdateMehods( MonoImage* image ){

	SharpClass panelClass = mono_class_from_name( image, "Sharp", "Panel");

	m_paintMethod = panelClass.GetMethod( "Paint", 0 );
	m_thinkMethod = panelClass.GetMethod( "Think", 0 );
	m_cursorEnteredMethod = panelClass.GetMethod( "OnCursorEntered", 0 );
	m_cursorExitedMethod = panelClass.GetMethod( "OnCursorExited", 0 );
	m_mousePressedMethod = panelClass.GetMethod( "OnMousePressed", 1 );
	m_mouseDoublePressedMethod = panelClass.GetMethod( "OnMouseDoublePressed", 1 );
	m_mouseReleasedMethod = panelClass.GetMethod( "OnMouseReleased", 1 );
	m_mouseWheeledMethod = panelClass.GetMethod( "OnMouseWheeled", 1 );
	m_performLayout = panelClass.GetMethod( "PerformLayout", 0 );
	m_paintBackground = panelClass.GetMethod( "PaintBackground", 0 );

	SharpClass buttonClass = mono_class_from_name( image, "Sharp", "Button");

	m_onClick = buttonClass.GetMethod( "OnClick", 0 );

};




