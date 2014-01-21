using System;
using System.Text;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;


using System.Reflection;
using System.Security;
using System.Security.Permissions;
using System.Security.Policy;

namespace Sharp
{

    public class Sharp
    {
        private static bool isServer;
        private static bool isInitialized = false;

        public static bool SERVER
        {
            get { return isServer; }
        }

        public static bool CLIENT
        {
            get { return !isServer; }
        }

        static void Initialize(bool isServer)
        {
            if (isInitialized == true)
            {
                Console.WriteLine("Error! We are initializing Sharp twice!");
                return;
            }

            Sharp.isServer = isServer;
            Sharp.isInitialized = true;

            Console.SetOut(new SourceWriter(isServer ? new Color(180, 180, 255, 255) : new Color(255, 250, 205, 255)));

            Console.WriteLine("Initialized Sharp!");
            EntityManager.OnCreate += EntityCreated;
            
            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
        }

        public static void EntityCreated(Entity entity)
        {
            //Console.WriteLine("Entity created! {0} ({1})", entity.Classname, entity.Index);
        }

        static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            Console.WriteLine("Unhandled exception D:");

            Exception ex = (Exception)e.ExceptionObject;
            Console.WriteLine("Whoops! Please contact the developers with the following"
                      + " information:\n\n" + ex.Message + ex.StackTrace);
        }

        static void OnAddonLoaded(Assembly assembly)
        {
            SharpReflection.ProcessAssembly(assembly);
        }
    }

    
    [StructLayout(LayoutKind.Sequential)]
    public abstract class SharpObject : IDisposable
    {
        private readonly IntPtr _obj;

        ~SharpObject()
        {
            if (this.IsValid)
                this.Dispose();
        }

        public bool IsValid
        {
            get { return _obj != IntPtr.Zero; }
        }

        public override int GetHashCode()
        {
            return this._obj.ToInt32();
        }

        public override bool Equals(System.Object obj)
        {
            // If parameter is null return false.
            if (obj == null)
                return false;

            // If parameter cannot be cast to Point return false.
            SharpObject entity = obj as SharpObject;
            if (entity == null)
                return false;

            // Return true if the fields match:
            return this._obj != IntPtr.Zero && this._obj == entity._obj;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public virtual extern void Dispose();
    }

    class SourceWriter : System.IO.TextWriter
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static int _Msg(String ptr);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static int _ConColorMsg(Color color, String ptr);

        public override Encoding Encoding
        {
            get
            {
                return System.Text.Encoding.UTF8; 
            }
        }

        readonly Color color;

        public SourceWriter(Color color)
        {
            this.color = color;
        }

        public override void Write(string s)
        {
            _ConColorMsg(color, s);
        }

        public override void Write(char[] buffer, int index, int count)
        {
            _ConColorMsg(color, new String(buffer, index, count));
        }


    }

}