using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Linq;
using System.Text;

namespace Sharp
{
    ///<summary>
    ///contents flags are seperate bits
    ///a given brush can contribute multiple content bits
    ///multiple brushes can be in a single leaf
    ///lower bits are stronger, and will eat weaker brushes completely
    ///</summary>
    public enum Contents : uint
    {
        ///<summary>No contents</summary>
        EMPTY = 0,
        ///<summary>an eye is never valid in a solid</summary>
        SOLID = 0x1,
        ///<summary>translucent, but not watery (glass)</summary>
        WINDOW = 0x2,
        AUX = 0x4,
        ///<summary>alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't</summary>
        GRATE = 0x8,
        SLIME = 0x10,
        WATER = 0x20,
        ///<summary>block AI line of sight</summary>
        BLOCKLOS = 0x40,
        ///<summary>things that cannot be seen through (may be non-solid though)</summary>
        OPAQUE = 0x80,


        TESTFOGVOLUME = 0x100,
        UNUSED = 0x200,

        /// <summary>
        /// unused 
        /// NOTE: If it's visible, grab from the top + update LAST_VISIBLE_CONTENTS
        /// if not visible, then grab from the bottom.
        /// OPAQUE + SURF_NODRAW count as OPAQUE (shadow-casting toolsblocklight textures)
        ///</summary>
        BLOCKLIGHT = 0x400,

        ///<summary>per team contents used to differentiate collisions </summary>
        TEAM1 = 0x800,
        ///<summary>between players and objects on different teams</summary>
        TEAM2 = 0x1000,

        ///<summary>ignore OPAQUE on surfaces that have SURF_NODRAW</summary>
        IGNORE_NODRAW_OPAQUE = 0x2000,

        ///<summary>hits entities which are MOVETYPE_PUSH (doors, plats, etc.)</summary>
        MOVEABLE = 0x4000,

        ///<summary>remaining contents are non-visible, and don't eat brushes</summary>
        AREAPORTAL = 0x8000,

        PLAYERCLIP = 0x10000,
        MONSTERCLIP = 0x20000,

        ///<summary>currents can be added to any other contents, and may be mixed</summary>
        CURRENT_0 = 0x40000,
        CURRENT_90 = 0x80000,
        CURRENT_180 = 0x100000,
        CURRENT_270 = 0x200000,
        CURRENT_UP = 0x400000,
        CURRENT_DOWN = 0x800000,

        ///<summary>removed before bsping an entity</summary>
        ORIGIN = 0x1000000,

        ///<summary>should never be on a brush, only in game</summary>
        MONSTER = 0x2000000,
        DEBRIS = 0x4000000,
        ///<summary>brushes to be added after vis leafs</summary>
        DETAIL = 0x8000000,
        ///<summary>auto set if any surface has trans</summary>
        TRANSLUCENT = 0x10000000,
        LADDER = 0x20000000,
        ///<summary>use accurate hitboxes on trace</summary>
        HITBOX = 0x40000000
    }

    public enum Mask : uint
    {
        ALL = (0xFFFFFFFF),
        ///<summary> everything that is normally solid</summary>
        SOLID = (Contents.SOLID | Contents.MOVEABLE | Contents.WINDOW | Contents.MONSTER | Contents.GRATE),
        ///<summary> everything that blocks player movement</summary>
        PLAYERSOLID = (Contents.SOLID | Contents.MOVEABLE | Contents.PLAYERCLIP | Contents.WINDOW | Contents.MONSTER | Contents.GRATE),
        ///<summary> blocks npc movement</summary>
        NPCSOLID = (Contents.SOLID | Contents.MOVEABLE | Contents.MONSTERCLIP | Contents.WINDOW | Contents.MONSTER | Contents.GRATE),
        ///<summary> blocks fluid movement</summary>
        NPCFLUID = (Contents.SOLID | Contents.MOVEABLE | Contents.MONSTERCLIP | Contents.WINDOW | Contents.MONSTER),
        ///<summary> water physics in these contents</summary>
        WATER = (Contents.WATER | Contents.MOVEABLE | Contents.SLIME),
        ///<summary> everything that blocks lighting</summary>
        OPAQUE = (Contents.SOLID | Contents.MOVEABLE | Contents.OPAQUE),
        ///<summary> everything that blocks lighting, but with monsters added.</summary>
        OPAQUE_AND_NPCS = (Mask.OPAQUE | Contents.MONSTER),
        ///<summary> everything that blocks line of sight for AI</summary>
        BLOCKLOS = (Contents.SOLID | Contents.MOVEABLE | Contents.BLOCKLOS),
        ///<summary> everything that blocks line of sight for AI plus NPCs</summary>
        BLOCKLOS_AND_NPCS = (Mask.BLOCKLOS | Contents.MONSTER),
        ///<summary> everything that blocks line of sight for players</summary>
        VISIBLE = (Mask.OPAQUE | Contents.IGNORE_NODRAW_OPAQUE),
        ///<summary> everything that blocks line of sight for players, but with monsters added.</summary>
        VISIBLE_AND_NPCS = (Mask.OPAQUE_AND_NPCS | Contents.IGNORE_NODRAW_OPAQUE),
        ///<summary> bullets see these as solid</summary>
        SHOT = (Contents.SOLID | Contents.MOVEABLE | Contents.MONSTER | Contents.WINDOW | Contents.DEBRIS | Contents.HITBOX),
        ///<summary> bullets see these as solid, except monsters (world+brush only)</summary>
        SHOT_BRUSHONLY = (Contents.SOLID | Contents.MOVEABLE | Contents.WINDOW | Contents.DEBRIS),
        ///<summary> non-raycasted weapons see this as solid (includes grates)</summary>
        SHOT_HULL = (Contents.SOLID | Contents.MOVEABLE | Contents.MONSTER | Contents.WINDOW | Contents.DEBRIS | Contents.GRATE),
        ///<summary> hits solids (not grates) and passes through everything else</summary>
        SHOT_PORTAL = (Contents.SOLID | Contents.MOVEABLE | Contents.WINDOW | Contents.MONSTER),
        ///<summary> everything normally solid, except monsters (world+brush only)</summary>
        SOLID_BRUSHONLY = (Contents.SOLID | Contents.MOVEABLE | Contents.WINDOW | Contents.GRATE),
        ///<summary> everything normally solid for player movement, except monsters (world+brush only)</summary>
        PLAYERSOLID_BRUSHONLY = (Contents.SOLID | Contents.MOVEABLE | Contents.WINDOW | Contents.PLAYERCLIP | Contents.GRATE),
        ///<summary> everything normally solid for npc movement, except monsters (world+brush only)</summary>
        NPCSOLID_BRUSHONLY = (Contents.SOLID | Contents.MOVEABLE | Contents.WINDOW | Contents.MONSTERCLIP | Contents.GRATE),
        ///<summary> just the world, used for route rebuilding</summary>
        NPCWORLDSTATIC = (Contents.SOLID | Contents.WINDOW | Contents.MONSTERCLIP | Contents.GRATE),
        ///<summary> just the world, used for route rebuilding</summary>
        NPCWORLDSTATIC_FLUID = (Contents.SOLID | Contents.WINDOW | Contents.MONSTERCLIP),
        ///<summary> These are things that can split areaportals</summary>
        SPLITAREAPORTAL = (Contents.WATER | Contents.SLIME),

        ///<summary> UNDONE: This is untested, any moving water</summary>
        CURRENT = (Contents.CURRENT_0 | Contents.CURRENT_90 | Contents.CURRENT_180 | Contents.CURRENT_270 | Contents.CURRENT_UP | Contents.CURRENT_DOWN),

        ///<summary> everything that blocks corpse movement
        ///UNDONE: Not used yet / may be deleted</summary>
        DEADSOLID = (Contents.SOLID | Contents.PLAYERCLIP | Contents.WINDOW | Contents.GRATE)
    }

    public enum DispSurf
    {
        FLAG_SURFACE = (1 << 0),
        FLAG_WALKABLE = (1 << 1),
        FLAG_BUILDABLE = (1 << 2),
        FLAG_SURFPROP1 = (1 << 3),
        FLAG_SURFPROP2 = (1 << 4)
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct cplant_t
    {
        public Vector normal;
        public float dist;
        public byte type;			// for fast side tests
        public byte signbits;		// signx + (signy<<1) + (signz<<1)
        public byte pad0;
        public byte pad1;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct TraceResponse
    {
        ///<summary> start position</summary>
        public readonly Vector StartPos;
        ///<summary> final position</summary>
        public readonly Vector EndPos;
        ///<summary> time completed, 1.0 = didn't hit anything</summary>
        public readonly float Fraction;
        ///<summary> contents on other side of surface hit</summary>
        public readonly int Contents;
        ///<summary> if true, plane is not valid</summary>
        public readonly bool AllSolid;
        ///<summary> if true, the initial point was in a solid area</summary>
        public readonly bool StartSolid;
        ///<summary> time we left a solid, only valid if we started in solid</summary>
        public readonly float FractionLeftSolid;
        ///<summary>  0 == generic, non-zero is specific body part</summary>
        public readonly int hitgroup;
        public readonly Entity HitEntity;

        public readonly cplant_t Plane;

        public bool DidHitWorld()
        {
            return HitEntity == Game.GetWorldEntity();
        }
    }

    public static class EngineTrace
    {
        public static readonly float MaxTraceLength = 16384.0f * 2.0f * ((float)Math.Sqrt(3.0));

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern TraceResponse TraceRayServer(Vector start, Vector end, Mask fMask, CollisionGroup collisionGroup, Entity filter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern TraceResponse TraceRayClient(Vector start, Vector end, Mask fMask, CollisionGroup collisionGroup, Entity filter);

        public static TraceResponse TraceRay(Vector start, Vector end, Mask fMask = Mask.OPAQUE, CollisionGroup collisionGroup = CollisionGroup.None, Entity filter = null)
        {
            if (Sharp.SERVER)
                return TraceRayServer(start, end, fMask, collisionGroup, filter);
            return TraceRayClient(start, end, fMask, collisionGroup, filter);
        }
    }

}
