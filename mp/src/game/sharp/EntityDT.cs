using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Security;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Sharp
{
    [StructLayout(LayoutKind.Explicit, Size = 4)]
    internal struct DTIntValue
    {
        [FieldOffset(0)]
        public int IntValue;

        [FieldOffset(0)]
        public float FloatValue;
    }

    [StructLayout(LayoutKind.Explicit, Size = 12)]
    internal struct DTVectorValue
    {
        [FieldOffset(0)]
        public Vector VectorValue;

        [FieldOffset(0)]
        public QAngle QAngleValue;
    }

    [Entity("banana")]
    internal class TestEntity : BaseAnimating
    {
        [Networked]
        private int points;

        private static readonly String NormalModel = "models/props/cs_italy/bananna.mdl";
        private static readonly String ExtraModel = "models/props/cs_italy/bananna_bunch.mdl";


        protected override void Initialize()
        {
            this.Solid = SolidType.BBOX;
            this.CollisionGroup = CollisionGroup.DebrisTrigger;
            this.SetCollisionBounds(new Vector(-25, -25, -25), new Vector(25, 25, 30));
            this.SolidFlags |= SolidFlags.Trigger;

            if (Sharp.SERVER)
                this.Model = (points == 5) ? ExtraModel : NormalModel;

            IPhysicsObject phys = this.Physics;
            if (phys != null)
                phys.Motion = false;

            //this.EmitSound("bananaeat.wav");
            //Engine.Sound.PrecacheSound("bananaeat.wav", true);
        }

        public override bool KeyValue(string keyName, string keyValue)
        {
            if (keyName == "points")
            {
                points = int.Parse(keyValue);
                return true;
            }

            return false;
        }
    }

    internal class EntityDTArrays
    {
        internal List<FieldInfo> IntFields = new List<FieldInfo>();
        internal List<FieldInfo> StringFields = new List<FieldInfo>();
        internal List<FieldInfo> VectorFields = new List<FieldInfo>();
    }

    internal static class EntityDT
    {
        static Type[] validIntTypes = { typeof(float), typeof(int), typeof(bool) };
        static Type[] validVectorTypes = { typeof(Vector), typeof(QAngle) };

        internal static void InitializeEntity(Entity entity)
        {
            EntityDTArrays networkFields = new EntityDTArrays();
            entity._NetworkedFields = networkFields;
            FieldInfo[] infos = entity.GetType().GetFields(BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Instance);

            foreach (var field in infos)
            {
                if (field.IsDefined(typeof(NetworkedAttribute), true))
                {
                    Type type = field.FieldType;

                    if (Array.IndexOf(validIntTypes, type) != -1 || type.IsEnum)
                        networkFields.IntFields.Add(field);
                    else if (type == typeof(string)) 
                        networkFields.StringFields.Add(field);
                    else if (Array.IndexOf(validVectorTypes, type) != -1)
                        networkFields.VectorFields.Add(field);
                    else
                    {
                        Console.WriteLine("Error: {0}.{1} is not of valid datatype ({2})", entity.GetType().Name, field.Name, type.FullName);
                    }
                }
            }

            if( Sharp.SERVER )
                SetDTArraySizes(
                    entity,
                    networkFields.IntFields.Count,
                    networkFields.StringFields.Count,
                    networkFields.VectorFields.Count);

        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void SetDTArraySizes(Entity entity, int intSize, int stringSize, int vectorSize);

        /**
         * <summary> Called by the client when a value has changed on the server. </summary>
         * */
        [SecuritySafeCritical]
        internal unsafe static void SetEntityIntVar(Entity entity, int index, DTIntValue value)
        {
            FieldInfo info = entity._NetworkedFields.IntFields[index];
            Type type = info.FieldType;

            if (type == typeof(float))
            {
                info.SetValue(entity, value.FloatValue);
            }
            else if (type == typeof(bool))
            {
                Console.WriteLine("Writing a boolean out!");
                info.SetValue(entity, Convert.ToBoolean( value.IntValue ) );
            } 
            else
            {
                info.SetValue(entity, value.IntValue); //int or enum
            }
        }

        [SecuritySafeCritical]
        internal unsafe static void SetEntityVectorVar(Entity entity, int index, DTVectorValue value)
        {
            FieldInfo info = entity._NetworkedFields.VectorFields[index];
            Type type = info.FieldType;

            if (type == typeof(Vector))
            {
                info.SetValue(entity, value.VectorValue);
            }
            else
            {
                info.SetValue(entity, value.QAngleValue);
            }
        }

        [SecuritySafeCritical]
        internal unsafe static void SetEntityStringVar(Entity entity, int index, string value)
        {
            FieldInfo info = entity._NetworkedFields.StringFields[index];
            info.SetValue(entity, value);
        }


        /**
         * <summary> Called by the server to get the value to be the sent to client.
         * (Sadly, the engine works on pooling the value for changes.)
         * (NOTE!: This method must be thread safe! The engine uses multiple theards to get all the variables faster)</summary>
         * */
        [SecuritySafeCritical]
        internal unsafe static void GetEntityIntVar(Entity entity, int index, ref DTIntValue retn)
        {
            FieldInfo info = entity._NetworkedFields.IntFields[index];
            Type type = info.FieldType;

            if (type == typeof(float))
            {
                retn.FloatValue = (float)info.GetValue(entity);
            }
            else if (type == typeof(bool))
            {
                retn.IntValue = ((bool) info.GetValue(entity)) ? 1 : 0;
            }
            else //int or enum
            {
                retn.IntValue = (int)info.GetValue(entity);
            }
        }

        [SecuritySafeCritical]
        internal unsafe static void GetEntityVectorVar(Entity entity, int index, ref DTVectorValue retn)
        {
            FieldInfo info = entity._NetworkedFields.VectorFields[index];
            Type type = info.FieldType;

            if (type == typeof(Vector))
            {
                retn.VectorValue = (Vector)info.GetValue(entity);
            }
            else
            {
                retn.QAngleValue = (QAngle)info.GetValue(entity);
            }
        }

        [SecuritySafeCritical]
        internal unsafe static void GetEntityStringVar(Entity entity, int index, out string retn)
        {
            FieldInfo info = entity._NetworkedFields.StringFields[index];
            retn = (string) info.GetValue(entity);
        }

    }




    [StructLayout(LayoutKind.Sequential)]
    internal class ServerClass
    {
        internal string NetworkName;
        internal SendTable Table;


    }

    [StructLayout(LayoutKind.Sequential)]
    internal class SendTable
    {
        List<SendProp> Props;
        string tableName;
    }

    internal enum SendPropType
    {
        Int = 0,
        Float,
        Vector,
        VectorXY,
        String,
        Array,
        DataTable,
        Int64
    }

    [Flags]
    internal enum SendPropFlags
    {
        Unsigned = (1 << 0),
        Coord = (1 << 1),
        NoScale = (1 << 2),
        RoundDown = (1 << 3),
        RoundUp = (1 << 4),
        Normal = (1 << 5),
        Exclude = (1 << 6),
        XYZE = (1 << 7),
        InsideArray = (1 << 8),
        ProxyAlwaysYes = (1 << 9),
        IsAVectorElem = (1 << 10),
        Collapsible = (1 << 11),
        ChangedOften = (1 << 12)

    }

    [StructLayout(LayoutKind.Sequential)]
    internal class SendProp
    {
        string varName;

        int Offset;
        int Bits;
        float LowValue;
        float HighValue;

        SendPropFlags Flags;

        internal SendPropType PropType;

    }

}
