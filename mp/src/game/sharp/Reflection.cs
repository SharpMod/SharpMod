using System;
using System.Collections.Generic;
using System.Reflection;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace Sharp
{

    [AttributeUsage(AttributeTargets.Field)]
    public class NetworkedAttribute : Attribute
    {

    }

    [AttributeUsage(AttributeTargets.Class)]
    [StructLayout(LayoutKind.Sequential)]
    public class EntityAttribute : Attribute
    {
        private string name;

        public EntityAttribute(string name)
        {
            this.name = name;
        }

        public string Name
        {
            get
            {
                return name;
            }
        }
    }

    [AttributeUsage(AttributeTargets.Method)]
    public class ConCommandAttribute : Attribute
    {
        private string name;
        private string helpText;
        private CommandFlags flags;

        public ConCommandAttribute(string name, string helpText = "", CommandFlags flags = 0)
        {
            this.name = name;
            this.helpText = helpText;
            this.flags = flags;
        }

        public string Name
        {
            get { return name; }
        }

        public string HelpText
        {
            get { return helpText; }
        }

        public CommandFlags Flags
        {
            get { return flags; }
        }
    }


    public static class SharpReflection
    {

        internal static void ProcessAssembly(Assembly assembly)
        {
            foreach (Type type in assembly.GetTypes())
            {
                try
                {
                    ProcessType(type);
                }
                catch (Exception e)
                {
                    //TODO: Is there a way to throw an unhandled excetion?
                    Console.WriteLine("Error when processing {0}: {1} ", type.FullName, e);
                }
            }

        }

        private static void ProcessType(Type type)
        {
            FindConCommands(type);

            object[] attributes = type.GetCustomAttributes(typeof(EntityAttribute), true);
            if (attributes.Length > 0)
            {
                if (!type.IsSubclassOf(typeof(Entity)))
                {
                    Console.WriteLine("Could not register {0} as it does not a subclass of Entity.", type);
                }
                else
                {
                    EntityAttribute attribute = attributes[0] as EntityAttribute;
                    EntityManager.Register(attribute.Name, type);
                }
            }
        }

        private static void FindConCommands(Type type)
        {
            foreach (MethodInfo method in type.GetMethods())
            {
                object[] attributes = method.GetCustomAttributes(typeof(ConCommandAttribute), true);

                if (attributes.Length == 0)
                    continue;

                ConCommandAttribute attribute = attributes[0] as ConCommandAttribute;
                ParameterInfo[] typeArguments = method.GetParameters();

                if (!method.IsStatic || !method.IsPublic)
                {
                    Console.WriteLine("Can not register non-static, or non-public, ConCommand: {0}", method.Name);
                    continue;
                }

                Delegate methodDelegate = Delegate.CreateDelegate(typeof(ConCommandDelegate), method, false);

                if (methodDelegate == null)
                {
                    Console.WriteLine("{0} does not match the signature of {1}", method.Name, typeof(ConCommandDelegate));
                    continue;
                }

                if (attribute.Flags.HasFlag(CommandFlags.GAMEDLL) && Sharp.CLIENT)
                    continue;

                if (attribute.Flags.HasFlag(CommandFlags.CLIENTDLL) && Sharp.SERVER)
                    continue;

                new MethodConCommand(methodDelegate as ConCommandDelegate, attribute.Name, attribute.HelpText, attribute.Flags);
            }
        }

        public delegate void ConCommandDelegate(CCommand command);

        internal class MethodConCommand : ConCommand
        {
            private ConCommandDelegate methodDelegate;

            public MethodConCommand(ConCommandDelegate method, string command, string helpText, CommandFlags flags)
                : base(command, helpText, flags)
            {
                this.methodDelegate = method;
            }

            public override void OnCommand(CCommand command)
            {
                this.methodDelegate(command);
            }
        }



    }

}
