using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Linq;
using System.Text;

namespace Sharp
{
    public enum MoveType
    {
        NONE = 0,	        // never moves
        ISOMETRIC,			// For players -- in TF2 commander view, etc.
        WALK,				// Player only - moving on the ground
        STEP,				// gravity, special edge handling -- monsters use this
        FLY,				// No gravity, but still collides with stuff
        FLYGRAVITY,		    // flies through the air + is affected by gravity
        VPHYSICS,			// uses VPHYSICS for simulation
        PUSH,				// no clip to world, push and crush
        NOCLIP,			    // No gravity, no collisions, still do velocity/avelocity
        LADDER,			    // Used by players only when going onto a ladder
        OBSERVER,			// Observer movement, depends on player's observer mode
        CUSTOM
    }

    public enum SolidType
    {
        NONE = 0,	// no solid model
        BSP = 1,	// a BSP tree
        BBOX = 2,	// an AABB
        OBB = 3,	// an OBB (not implemented yet)
        OBB_YAW = 4,	// an OBB, constrained so that it can only yaw
        CUSTOM = 5,	// Always call into the entity for tests
        VPHYSICS = 6,	// solid vphysics object, get vcollide from the model and collide with that
    }

    [Flags]
    public enum SolidFlags
    {
        CustomRayTest = 0x0001,	// Ignore solid type + always call into the entity for ray tests
        CustomBoxTest = 0x0002,	// Ignore solid type + always call into the entity for swept box tests
        NotSolid = 0x0004,	// Are we currently not solid?
        Trigger = 0x0008,	// This is something may be collideable but fires touch functions
        // even when it's not collideable (when the FSOLID_NOT_SOLID flag is set)
        NotStandable = 0x0010,	// You can't stand on this
        VouleContents = 0x0020,	// Contains volumetric contents (like water)
        ForceWorldAligned = 0x0040,	// Forces the collision rep to be world-aligned even if it's SOLID_BSP or SOLID_VPHYSICS
        UseTriggerBounds = 0x0080,	// Uses a special trigger bounds separate from the normal OBB
        RootParentAligned = 0x0100,	// Collisions are defined in root parent's local coordinate space
        TriggerTouchDebris = 0x0200,	// This trigger will touch debris objects
        TriggerTouchPlayer = 0x0400,	// This trigger will touch only players
        NotMoveable = 0x0800	// Assume this object will not move
    }

    public enum MotionEvent
    {
        NOTHING = 0,
        LOCAL_ACCELERATION,
        LOCAL_FORCE,
        GLOBAL_ACCELERATION,
        GLOBAL_FORCE
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct EntityCollissionData
    {
        public readonly int SurfaceProp;
        public readonly Vector PreVelocity;
        public readonly Vector PostVelocity;
        public readonly Vector PreAngularVelocity;
        public readonly Entity Entity;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct CollisionEvent
    {
        public readonly EntityCollissionData Data;
        public readonly EntityCollissionData OtherData;
        public readonly float DeltaTime;
        public readonly bool IsCollision;
        public readonly bool IsShadowCollision;
        public readonly float CollisionSpeed;
        public readonly Vector Normal;
        public readonly Vector Point;
        public readonly Vector Speed;
    }

    public sealed class IPhysicsObject : SharpObject
    {

        extern public float Mass
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        extern public float BuoyancyRatio
        {
            //For some reason, IPhysicsObject::GetBuoyancyRatio does not exist
            //[MethodImplAttribute(MethodImplOptions.InternalCall)]
            //get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        extern public Vector Velocity
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        extern public Vector Position
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        extern public QAngle Angles
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        extern public bool Gravity
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        extern public bool Motion
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        extern public bool Static
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        public bool Moveable
        {
            get { return Motion && !Static; }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Wake();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void ApplyForceCenter(Vector force);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public override extern void Dispose();
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct surfacephysicsparams_t
    {
        // vphysics physical properties
        public float friction;
        public float elasticity;				// collision elasticity - used to compute coefficient of restitution
        public float density;				// physical density (in kg / m^3)
        public float thickness;				// material thickness if not solid (sheet materials) in inches
        public float dampening;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct surfaceaudioparams_t
    {
        // sounds / audio data
        public float reflectivity;		// like elasticity, but how much sound should be reflected by this surface
        public float hardnessFactor;	// like elasticity, but only affects impact sound choices
        public float roughnessFactor;	// like friction, but only affects scrape sound choices

        // audio thresholds
        public float roughThreshold;	// surface roughness > this causes "rough" scrapes, < this causes "smooth" scrapes
        public float hardThreshold;	// surface hardness > this causes "hard" impacts, < this causes "soft" impacts
        public float hardVelocityThreshold;	// collision velocity > this causes "hard" impacts, < this causes "soft" impacts
        // NOTE: Hard impacts must meet both hardnessFactor AND velocity thresholds
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct surfacesoundnames_t
    {
        public ushort walkStepLeft;
        public ushort walkStepRight;
        //public ushort runStepLeft;
        //public ushort runStepRight;

        public ushort impactSoft;
        public ushort impactHard;

        public ushort scrapeSmooth;
        public ushort scrapeRough;

        public ushort bulletImpact;
        public ushort rolling;

        public ushort breakSound;
        public ushort strainSound;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct surfacegameprops_t
    {
        // game movement data
        public float maxSpeedFactor;			// Modulates player MAX speed when walking on this surface
        public float jumpFactor;				// Indicates how much higher the player should jump when on the surface
        // Game-specific data
        public ushort material;
        // Indicates whether or not the player is on a ladder.
        public byte climbable;
        public byte pad;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct surfacesoundhandles_t
    {
        public short walkStepLeft;
        public short walkStepRight;
        //public short runStepLeft;
        //public short runStepRight;

        public short impactSoft;
        public short impactHard;

        public short scrapeSmooth;
        public short scrapeRough;

        public short bulletImpact;
        public short rolling;

        public short breakSound;
        public short strainSound;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class surfacedata_t
    {
        public surfacephysicsparams_t physics;	// physics parameters
        public surfaceaudioparams_t audio;		// audio parameters
        public surfacesoundnames_t sounds;		// names of linked sounds
        public surfacegameprops_t game;		// Game data / properties
        public surfacesoundhandles_t soundhandles;
    }

    [StructLayout(LayoutKind.Sequential)]
    public sealed class SurfaceProps
    {
        private IntPtr _surface;

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern surfacedata_t GetSurfaceData(int surfaceDataIndex);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern string GetString(ushort stringTableIndex);

    }

    [StructLayout(LayoutKind.Sequential)]
    public sealed class SoundEmitterBase
    {
        private IntPtr _emitter;

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetActorGender(string actormodel);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern CSoundParameters GetParametersForSound(string soundname, int gender, bool isbeingemitted = false);
    }

    public static class Physics
    {
        public static SurfaceProps SurfaceProps;

        public static SoundEmitterBase SoundEmitterBase;
    }

    [StructLayout(LayoutKind.Sequential)]
    struct breaksound_t
    {
        public Vector origin;
        public int surfacePropsBreak;
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct impactsound_t
    {
        internal IntPtr pGameData;
        public int entityIndex;
        public int soundChannel;
        public float volume;
        public float impactSpeed;
        public ushort surfaceProps;
        public ushort surfacePropsHit;
        public Vector origin;
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct EmitSound
    {
        public int Channel;
        public String SoundName;
        public float Volume; //Default: 1.0
        public int SoundLevel;
        public int Flags;
        public int Pitch;   //Default: 100
        public int SpecialDSP;
        public Vector? Origin;
        /// <summary>
        /// ///< NOT DURATION, but rather, some absolute time in the future until which this sound should be delayed
        /// </summary>
        public float SoundTime;
        public float? SoundDuration;
        public bool EmitCloseCaption; //Default true
        public bool WarnOnMissingCloseCaption;
        public bool WarnOnDirectWaveReference;
        public int SpeakerEntity;
        //mutable CUtlVector< Vector >	m_UtlVecSoundOrigin;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class CSoundParameters
    {
        public int channel;
        public float volume;
        public int pitch;
        public int pitchlow, pitchhigh;
        public int soundlevel;

        public bool play_to_owner_only;
        public int count;
        public string soundname;
        public int delay_msec;
    }

    public class PhysicsImpactSound
    {
        List<impactsound_t> list = new List<impactsound_t>();

        public void PlayImpactSounds()
        {
            try
            {
                foreach (impactsound_t sound in list)
                {
                    surfacedata_t psurf = Physics.SurfaceProps.GetSurfaceData(sound.surfaceProps);
                    if (psurf.sounds.impactHard == 0)
                        continue;

                    surfacedata_t pHit = Physics.SurfaceProps.GetSurfaceData(sound.surfacePropsHit);
                    ushort soundName = psurf.sounds.impactHard;
                    if (pHit != null && psurf.sounds.impactSoft != 0)
                    {
                        if (pHit.audio.hardnessFactor < psurf.audio.hardThreshold ||
                            (psurf.audio.hardVelocityThreshold > 0 && psurf.audio.hardVelocityThreshold > sound.impactSpeed))
                        {
                            soundName = psurf.sounds.impactSoft;
                        }
                    }

                    string pSound = Physics.SurfaceProps.GetString(soundName);

                    CSoundParameters parms = Physics.SoundEmitterBase.GetParametersForSound(pSound, Physics.SoundEmitterBase.GetActorGender(null));
                    if (parms == null)
                        break;

                    float volume = Math.Min(sound.volume, 1.0f);

                    CPASAttenuationFilter filter = new CPASAttenuationFilter(sound.origin, parms.soundlevel);

                    EmitSound ep = new EmitSound();
                    ep.Channel = sound.soundChannel;
                    ep.SoundName = parms.soundname;
                    ep.Volume = parms.volume * volume;
                    ep.SoundLevel = parms.soundlevel;
                    ep.Pitch = parms.pitch;
                    ep.Origin = sound.origin;

                    Entity.EmitSound(filter, 0, ep);
                    /*
                    Engine.Sound.EmitSound(
                        filter,
                        0,
                        (AudioChannel)sound.soundChannel,
                        parms.soundname,
                        parms.volume * sound.volume,
                        parms.soundlevel,
                        0,
                        parms.pitch,
                        sound.origin);
                     * */

                }

            }
            catch (Exception e)
            {
                throw e;
            }
            finally
            {
                list.Clear();
            }
        }

        public void AddImpactSound(int entityIndex, int soundChannel, IPhysicsObject pObject, int surfaceProps, int surfacePropsHit, float volume, float impactSpeed)
        {
            impactsound_t sound = new impactsound_t();
            impactSpeed += 1e-4f;

            for (int i = 0; i < list.Count; i++)
            {
                sound = list[i];

                if (surfaceProps == sound.surfaceProps || list.Count > 4)
                {
                    if (volume > sound.volume)
                    {
                        sound.origin = pObject.Position;
                        sound.entityIndex = entityIndex;
                        sound.soundChannel = soundChannel;
                        sound.surfacePropsHit = (ushort)surfacePropsHit;
                    }

                    sound.volume += volume;
                    sound.impactSpeed = Math.Max(impactSpeed, sound.impactSpeed);

                    list[i] = sound;
                    return;
                }
            }

            sound.entityIndex = entityIndex;
            sound.soundChannel = soundChannel;
            sound.origin = pObject.Position;
            sound.surfaceProps = (ushort)surfaceProps;
            sound.surfacePropsHit = (ushort)surfacePropsHit;
            sound.volume = volume;
            sound.impactSpeed = impactSpeed;

            list.Add(sound);
        }


    }

    public class PhysicsBreakSound
    {
        List<breaksound_t> list = new List<breaksound_t>();

        public void AddBreakSound(Vector origin, ushort surfaceProps)
        {
            surfacedata_t psurf = Physics.SurfaceProps.GetSurfaceData(surfaceProps);
            if (psurf.sounds.breakSound == 0)
                return;

            breaksound_t sound;

            for (int i = list.Count() - 1; i >= 0; --i)
            {
                sound = list[i];
                // Allow 3 break sounds before you start merging anything.
                if (list.Count() > 2 && surfaceProps == sound.surfacePropsBreak)
                {
                    sound.origin = (sound.origin + origin) * 0.5f;
                    return;
                }
            }

            sound.origin = origin;
            sound.surfacePropsBreak = surfaceProps;
            list.Add(sound);
        }

        public void PlayBreakSounds()
        {
            foreach (breaksound_t sound in list)
            {

                surfacedata_t psurf = Physics.SurfaceProps.GetSurfaceData(sound.surfacePropsBreak);
                string pSound = Physics.SurfaceProps.GetString(psurf.sounds.breakSound);

                CSoundParameters parms = Physics.SoundEmitterBase.GetParametersForSound(pSound, Physics.SoundEmitterBase.GetActorGender(null));
                if (parms == null)
                    break;

                CPASAttenuationFilter filter = new CPASAttenuationFilter(sound.origin, parms.soundlevel);

                /*
                Engine.Sound.EmitSound(
                    filter,
                    0,
                    AudioChannel.Static,
                    parms.soundname,
                    parms.volume,
                    parms.soundlevel,
                    0,
                    parms.pitch,
                    sound.origin); 
                 * */
            }

            list.Clear();
        }

    }
}
