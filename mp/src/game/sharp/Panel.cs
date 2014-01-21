using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Sharp
{
    [Flags]
    public enum FontFlags : int
    {
        None,
        Italic = 0x001,
        Underline = 0x002,
        Strikeout = 0x004,
        Symbol = 0x008,
        Antialias = 0x010,
        Gaussianblur= 0x020,
        Rotary = 0x040,
        DropShadow = 0x080,
        Additive = 0x100,
        Outline = 0x200
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Position
    {
        public int x;
        public int y;

        public Position(int x, int y)
        {
            this.x = x;
            this.y = y;
        }

        public override string ToString()
        {
            return String.Format("[Position={0},{1}]", x, y);
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Font
    {
        internal Int32 fontId;

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Font(String windowsFontName, int tall, int width, bool blur, bool scanlines, FontFlags flags);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetTall();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool IsFontAdditive();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void GetTextSize(string text, out int width, out int height);
    }

    public struct Vertex2D
    {
        public float x;
        public float y;
    }

    public struct VertexT
    {
        public Vertex2D Position;
        public Vertex2D TexCoord;
    }

    public static class Surface
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetColor(Color color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void DrawFilledRect(int x0, int y0, int x1, int y1);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void DrawOutlinedRect(int x0, int y0, int x1, int y1);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void DrawLine(int x0, int y0, int x1, int y1);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void PlaySound(string filename);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int CreateNewTextureID();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void DestroyTextureID(int id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int DrawSetTextureFile(int id, string filename, bool hardwareFilter, bool forceReload);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetTextureRaw(string filename);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetTexture(int id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void GetTextureSize(int id, ref int wide, ref int tall);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void DrawTexturedRect(int x0, int y0, int x1, int y1);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void DrawTexturedPolygon(VertexT[] vertexes, bool ClipVertices = true );

        public static int GetTexture(string filename)
        {
            int texture = GetTextureRaw(filename);

            if (texture == -1)
            {
                if (filename != null && filename.Length > 0)
                {
                    texture = CreateNewTextureID();
                    DrawSetTextureFile(texture, filename, true, false);
                }
            }

            return texture;
        }

        /// <summary>
        /// Draws a rotated texture, at the given center
        /// </summary>
        /// <param name="x">center x-position</param>
        /// <param name="y">center y-position</param>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <param name="rotation">rotation in radians</param>
        public static void DrawTexturedRectRotated(int x, int y, int width, int height, float rotation)
        {
            VertexT[] list = new VertexT[4];
            float s = (float) Math.Sin(rotation);
            float c = (float) Math.Cos(rotation);
            float halfWidth = width / 2;
            float halfHeight = height / 2;

            //2d matrix rotation http://en.wikipedia.org/wiki/Rotation_matrix
            //Top-left
            list[0] = new VertexT();
            list[0].Position.x = (c * -halfWidth) + (-s * -halfHeight) + x;
            list[0].Position.y = (s * -halfWidth) + (c * -halfHeight) + y;
            list[0].TexCoord.x = 0.0f;
            list[0].TexCoord.y = 0.0f;

            //top right
            list[1] = new VertexT();
            list[1].Position.x = (c * halfWidth) + (-s * -halfHeight) + x;
            list[1].Position.y = (s * halfWidth) + (c * -halfHeight) + y;
            list[1].TexCoord.x = 1.0f;
            list[1].TexCoord.y = 0.0f;

            //bottom right
            list[2] = new VertexT();
            list[2].Position.x = (c * halfWidth) + (-s * halfHeight) + x;
            list[2].Position.y = (s * halfWidth) + (c * halfHeight) + y;
            list[2].TexCoord.x = 1.0f;
            list[2].TexCoord.y = 1.0f;

            //bottom left
            list[3] = new VertexT();
            list[3].Position.x = (c * -halfWidth) + (-s * halfHeight) + x;
            list[3].Position.y = (s * -halfWidth) + (c * halfHeight) + y;
            list[3].TexCoord.x = 0.0f;
            list[3].TexCoord.y = 1.0f;

            Surface.DrawTexturedPolygon(list);
        }

        public static extern int ScreenWide
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        public static extern int ScreenTall
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Panel : IDisposable
    {
        private readonly IntPtr _panel;

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Panel();

        public extern Position Position
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public int X
        {
            get
            {
                return Position.x;
            }
            set
            {
                Position pos = this.Position;
                pos.x = value;
                this.Position = pos;
            }
        }

        public int Y
        {
            get
            {
                return Position.y;
            }
            set
            {
                Position pos = this.Position;
                pos.y = value;
                this.Position = pos;
            }
        }

        public extern int Wide
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern int Tall
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern int Visible
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern Panel Parent
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern bool AllowMouseInput
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern bool AllowKeyboardInput
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern int ZPos
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern bool AutoDelete
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern Color ForegroundColor
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern Color BackgroundColor
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void InvalidateLayout(bool layoutNow = false, bool layoutSchema = false);

        public void CenterHorizontally( double center = 0.5 )
        {
            Panel parent = this.Parent;

            if (parent == null)
                return;

            this.X = (int)(parent.Wide * center) - (this.Wide / 2);
        }

        public bool IsValid
        {
            get { return _panel != IntPtr.Zero; }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Dispose();

		~Panel() {
			if (IsValid)
				Dispose();
		}

        
        public virtual void Paint() { }
        public virtual void PaintBackground() { }
        public virtual void Think() { }
        public virtual void PerformLayout() { }
        public virtual void OnCursorEntered() { }
        public virtual void OnCursorExited() { }
        public virtual void OnMousePressed(ButtonCode code) { }
        public virtual void OnMouseDoublePressed(ButtonCode code) { }
        public virtual void OnMouseReleased(ButtonCode code) { }
        public virtual void OnMouseWheeled(int delta) { }
    }

    public enum Alignment
    {
        NorthWest = 0,
        North,
        NorthEast,
        West,
        Center,
        East,
        SouthWest,
        South,
        SouthEast,
    };

    public class Label : Panel
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Label(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern override void Paint();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern override void PaintBackground();

        public extern string Text
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern Alignment Alignment
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern Font Font
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SizeToContents();

    }

	public class ImagePanel : Panel
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern ImagePanel();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern override void Paint();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern override void PaintBackground();

		public extern string Image
		{
			[MethodImplAttribute(MethodImplOptions.InternalCall)]
			get;
			[MethodImplAttribute(MethodImplOptions.InternalCall)]
			set;
		}


		//TODO: Num Frames, Image scale
	}

    public enum ButtonCode
    {
        BUTTON_CODE_INVALID = -1,
        BUTTON_CODE_NONE = 0,

        KEY_FIRST = 0,

        KEY_NONE = KEY_FIRST,
        KEY_0,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,
        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z,
        KEY_PAD_0,
        KEY_PAD_1,
        KEY_PAD_2,
        KEY_PAD_3,
        KEY_PAD_4,
        KEY_PAD_5,
        KEY_PAD_6,
        KEY_PAD_7,
        KEY_PAD_8,
        KEY_PAD_9,
        KEY_PAD_DIVIDE,
        KEY_PAD_MULTIPLY,
        KEY_PAD_MINUS,
        KEY_PAD_PLUS,
        KEY_PAD_ENTER,
        KEY_PAD_DECIMAL,
        KEY_LBRACKET,
        KEY_RBRACKET,
        KEY_SEMICOLON,
        KEY_APOSTROPHE,
        KEY_BACKQUOTE,
        KEY_COMMA,
        KEY_PERIOD,
        KEY_SLASH,
        KEY_BACKSLASH,
        KEY_MINUS,
        KEY_EQUAL,
        KEY_ENTER,
        KEY_SPACE,
        KEY_BACKSPACE,
        KEY_TAB,
        KEY_CAPSLOCK,
        KEY_NUMLOCK,
        KEY_ESCAPE,
        KEY_SCROLLLOCK,
        KEY_INSERT,
        KEY_DELETE,
        KEY_HOME,
        KEY_END,
        KEY_PAGEUP,
        KEY_PAGEDOWN,
        KEY_BREAK,
        KEY_LSHIFT,
        KEY_RSHIFT,
        KEY_LALT,
        KEY_RALT,
        KEY_LCONTROL,
        KEY_RCONTROL,
        KEY_LWIN,
        KEY_RWIN,
        KEY_APP,
        KEY_UP,
        KEY_LEFT,
        KEY_DOWN,
        KEY_RIGHT,
        KEY_F1,
        KEY_F2,
        KEY_F3,
        KEY_F4,
        KEY_F5,
        KEY_F6,
        KEY_F7,
        KEY_F8,
        KEY_F9,
        KEY_F10,
        KEY_F11,
        KEY_F12,
        KEY_CAPSLOCKTOGGLE,
        KEY_NUMLOCKTOGGLE,
        KEY_SCROLLLOCKTOGGLE,

        KEY_LAST = KEY_SCROLLLOCKTOGGLE,
        KEY_COUNT = KEY_LAST - KEY_FIRST + 1,

        // Mouse
        MOUSE_FIRST = KEY_LAST + 1,

        MOUSE_LEFT = MOUSE_FIRST,
        MOUSE_RIGHT,
        MOUSE_MIDDLE,
        MOUSE_4,
        MOUSE_5,
        MOUSE_WHEEL_UP,		// A fake button which is 'pressed' and 'released' when the wheel is moved up 
        MOUSE_WHEEL_DOWN,	// A fake button which is 'pressed' and 'released' when the wheel is moved down

        MOUSE_LAST = MOUSE_WHEEL_DOWN,
        MOUSE_COUNT = MOUSE_LAST - MOUSE_FIRST + 1
    };

}
