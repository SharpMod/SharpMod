using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.CompilerServices;
using System.Collections;

namespace Sharp
{
    public enum AudioChannel : int
    {
        Replace = -1,
        Auto = 0,
        Weapon = 1,
        Voice = 2,
        Item = 3,
        Body = 4,
        /// <summary> allocate stream channel from the static or dynamic area </summary>
        Stream = 5,
        /// <summary> allocate channel from the static area  </summary>
        Static = 6,
        /// <summary>allocate channel for network voice data </summary>
        VoiceBase = 7
    }

    public static class ClientEngine
    {
        /// <summary>
        /// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
        /// Note: Calls to this are checked against FCVAR_CLIENTCMD_CAN_EXECUTE (if that bit is not set, then this function can't change it).
        /// Call ClientCmd_Unrestricted to have access to FCVAR_CLIENTCMD_CAN_EXECUTE vars.
        /// </summary>
        /// <param name="command"></param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void ClientCmd(string command);
    }

    //Server only functions
    public static class ServerEngine
    {
        // Given the current PVS(or PAS) and origin, determine which players should hear/receive the message
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int[] Message_DetermineMulticastRecipients(Vector origin, bool usepas = false);
    }

    //I really do not know what is wrong with this
    //Looking at SourceMod source code, all it does is precache and emit the sound
    //Here, is always throws a "Failed to load sound "folder\sound3.wav", file probably missing from disk/repository" error.
    public sealed class EngineSound
    {
        private IntPtr enginesound;

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool PrecacheSound(string pSample, bool bPreload = false, bool bIsUISound = false);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool IsSoundPrecached(string pSample);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void PrefetchSound(string pSample);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool IsLoopingSound(string pSample);

        /// <summary> Just loads the file header and checks for duration (not hooked up for .mp3's yet) </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float GetSoundDuration(string pSample);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void EmitSound(RecipientFilter filter, int iEntIndex, AudioChannel iChannel, string pSample,
            float flVolume, int iSoundLevel, int iFlags, int iPitch, Vector? pOrigin = null);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void StopSound(int iEntIndex, AudioChannel iChannel, string pSample);


    }


    public static class Engine
    {
        public static readonly EngineSound Sound;

    }
}
