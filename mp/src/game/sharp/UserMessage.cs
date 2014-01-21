using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Collections;

namespace Sharp
{
    public class RecipientFilter 
    {
        protected List<Player> players = new List<Player>();

        public RecipientFilter()
        {
        }

        public RecipientFilter( Player player ) : this()
        {
            players = new List<Player>();
            this.Add(player);
        }

        public RecipientFilter(List<Player> players)
            : this()
        {
            this.players = players;
        }

        public void Add(Player player)
        {
            this.players.Add(player);
        }

        public void AddAllPlayers()
        {
            foreach( Player player in Game.GetPlayers() ){
                Add( player );
            }
        }

        public Player this[int i]
        {
            get
            {
                return players[i];
            }
        }

        /// <summary>
        /// Called internally, get the number of recipients to read
        /// </summary>
        /// <returns></returns>
        public int GetRecipientCount()
        {
            return players.Count;
        }

        /// <summary>
        /// Called internally, get the index of the client at the given slot
        /// </summary>
        /// <param name="slot"></param>
        /// <returns></returns>
        public int GetRecipientIndex( int slot )
        {
            return players[slot].Index;
        }

        /// <summary>
        /// Makes sure that the message receives on the client
        /// </summary>
        /// <returns></returns>
        public bool IsReliable()
        {
            return true;
        }

        public bool IsInitMessage()
        {
            return false;
        }

        public void AddRecipientsByPAS(Vector origin)
        {
            if (Game.MaxClients == 1)
            {
                AddAllPlayers();
            }
            else
            {
                int[] vec = ServerEngine.Message_DetermineMulticastRecipients(origin, true);
                AddPlayersFromBitMask( new BitArray(vec) );
            }
        }

        public void AddPlayersFromBitMask(BitArray playerbits)
        {

            for (int i = 0; i < Game.MaxClients; i++)
            {
                if (playerbits[i])
                {
                    Player player = Game.GetPlayerByIndex(i + 1);
                    if (player != null)
                        Add(player);

                    //Console.WriteLine("Sharp player bits: %d", i);
                }
            }
        }
    }

    public class CPASFilter : RecipientFilter
    {
        public CPASFilter()
        {
        }

        public CPASFilter(Vector origin)
        {
            AddRecipientsByPAS(origin);
        }
    }

    public class CPASAttenuationFilter : CPASFilter
    {
        public CPASAttenuationFilter()
        {
        }

        public static float SoundLevelToAttention(int a)
        {
            return ((a > 50) ? (20.0f / (float)(a - 50)) : ((a == 0) ? (0.0f) : (4.0f)));
        }

        public CPASAttenuationFilter( Vector origin, int soundLevel ) : base(origin)
        {
            Filter(origin, SoundLevelToAttention(soundLevel));
        }

        public void Filter(Vector origin, float attenuation)
        {
            if (Game.MaxClients == 1)
                return;

            if (attenuation <= 0)
            {
                AddAllPlayers();
                return;
            }

            float maxAudible = (2.0f * 1000.0f) / attenuation;

            players = players.Where((Player player) => player.EyePosition.Distance(origin) <= maxAudible).ToList();
        }


    }

    public static class UserMessage
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void Begin(RecipientFilter filter, int msg_type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void BeginEntity(Entity entity, bool reliable = false);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void WriteChar(char c);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void WriteByte(byte b);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void WriteShort(short s);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void WriteLong(long l);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void WriteFloat(float f);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void WriteVector(Vector f);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void WriteVectorNormal(Vector f);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void WriteAngles(QAngle f);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void WriteString(string f);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void WriteBool(bool f);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetNumBytesWritten();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetNumBitsWritten();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetMaxNumBits();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetNumBitsLeft();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetNumBytesLeft();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void End();
    }

    public class BitRead : SharpObject
    {
        private BitRead() { }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern char ReadChar();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern byte ReadByte();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern short ReadShort();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int ReadLong(); //So... Why is long  = 32bit?

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float ReadFloat();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector ReadVector();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector ReadVectorNormal();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern QAngle ReadAngles();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern string ReadString();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int ReadBool();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int TotalBytesAvailable();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetNumBytesLeft();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Seek(int nPosition);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int Tell();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern override void Dispose();
    }

    internal struct UserMessageEntry
    {
        public string Name;
        public UserMessageHook.ReceiveMessage ReceiveMessage;

        public UserMessageEntry(string name, UserMessageHook.ReceiveMessage receiveMessage)
        {
            this.Name = name;
            this.ReceiveMessage = receiveMessage;
        }
    }

    public static class UserMessageHook
    {
        public delegate void ReceiveMessage(BitRead message);

        internal static Dictionary<byte, UserMessageEntry> messageHooks = new Dictionary<byte, UserMessageEntry>();

        public static void AddHook(byte hash, string name, ReceiveMessage hook)
        {
            UserMessageEntry entry = new UserMessageEntry(name, hook);
            //int hash = name.GetHashCode();

            messageHooks.Add(hash, entry);
        }


        /// <summary>
        /// Called internally
        /// </summary>
        public static bool GetUserMessageInfo(byte msgType, ref string name, ref int size)
        {
            Console.WriteLine("Requesting for info: {0}", msgType);
            if (!messageHooks.ContainsKey(msgType))
                return false;

            UserMessageEntry entry = messageHooks[msgType];

            name = entry.Name;
            size = -1;

            return true;
        }

        /// <summary>
        /// Called internally
        /// </summary>
        /// <param name="msgType"></param>
        /// <param name="msgData"></param>
        /// <returns></returns>
        public static bool DispatchUserMessage(byte msgType, BitRead msgData)
        {
            Console.WriteLine("Got message of type: {0}", msgType);
            if (!messageHooks.ContainsKey(msgType))
                return false;

            UserMessageEntry entry = messageHooks[msgType];

            Console.WriteLine("Found entry: {0}", entry);

            entry.ReceiveMessage(msgData);

            return true;
        }


    }



}
