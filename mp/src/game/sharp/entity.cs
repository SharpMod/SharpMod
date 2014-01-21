using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Security;


namespace Sharp
{
    [Flags]
    public enum EntityEffectFlags : int
    {
        /// <summary>Performs bone merge on client side</summary>
        BoneMerge = 0x001,
        /// <summary>DLIGHT centered at entity origin</summary>
        BrightLight = 0x002,
        /// <summary>player flashlight</summary>
        DimLight = 0x004,
        /// <summary>don't interpolate the next frame</summary>
        NoInterp = 0x008,
        /// <summary>Don't cast no shadow</summary>
        NoShadow = 0x010,
        /// <summary>don't draw entity</summary>
        NoDraw = 0x020,
        /// <summary>Don't receive no shadow</summary>
        NoReceiveShadow = 0x040,
        /// <summary>
        /// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
        /// parent and uses the parent's bbox + the MAX extents of the aiment.
        /// Otherwise, it sets up the parent's bones every frame to figure out where to place
        /// the aiment, which is inefficient because it'll setup the parent's bones even if
        /// the parent is not in the PVS.
        /// </summary>
        BoneMergeFastCull = 0x080,
        /// <summary>blink an item so that the user notices it.</summary>
        ItemBlink = 0x100,
        /// <summary>always assume that the parent entity is animating</summary>
        ParentAnimates = 0x200
    }

    public enum CollisionGroup : int
    {
        None = 0,
        Debris,			// Collides with nothing but world and static stuff
        DebrisTrigger, // Same as debris, but hits triggers
        InteractiveDebris,	// Collides with everything except other interactive debris or debris
        Interactive,	// Collides with everything except interactive debris or debris
        Player,
        BreakableGlass,
        Vehicle,
        PlayerMovement,  // For HL2, same as Player, for
        // TF2, this filters out other players and CBaseObjects
        Npc,			// Generic NPC group
        InVehicle,		// for any entity inside a vehicle
        Weapon,			// for any weapons that need collision detection
        VehicleClip,	// vehicle clip brush to restrict vehicle movement
        Projectile,		// Projectiles!
        DoorBlocker,	// Blocks entities not permitted to get near moving doors
        PassableDoor,	// Doors that the player shouldn't collide with
        Dissolving,		// Things that are dissolving are in this group
        PushAway,		// Nonsolid on client and server, pushaway in player code

        NpcActor,		// Used so NPCs in scripts ignore the player.
        NpcScripted,	// USed for NPCs in scripts that should not collide with each other
        PzClip,
    }

    public enum EntityRenderFx : byte
    {
        None = 0,
        PulseSlow,
        PulseFast,
        PulseSlowWide,
        PulseFastWide,

        FadeSlow,
        FadeFast,
        SolidSlow,
        SolidFast,
        StrobeSlow,

        StrobeFast,
        StrobeFaster,
        FlickerSlow,
        FlickerFast,
        NoDissipation,

        FadeOut,
        FadeIn,
        PulseFastWider,
        GlowShell
    }

    public enum EntityRenderMode : byte
    {
        Normal = 0,		// src
        TransColor,		// c*a+dest*(1-a)
        TransTexture,	// src*a+dest*(1-a)
        Glow,			// src*a+dest -- No Z buffer checks -- Fixed size in screen space
        TransAlpha,		// src*srca+dest*(1-srca)
        TransAdd,		// src*a+dest
        Environmental,	// not drawn, used for environmental effects
        TransAddFrameBlend, // use a fractional frame value to blend between animation frames
        TransAlphaAdd,	// src + dest*(1-a)
        WorldGlow,		// Same as Glow but not fixed size in screen space
        None			// Don't render.
    }

    public enum LifeState : int
    {
        Alive,
        Dying,
        Dead,
        Respawnable,
        DiscardBody
    }

    /// <summary>
    /// Defined in const.h:139
    /// </summary>
    [Flags]
    public enum EntityFlags : int
    {
        /// <summary>
        /// At rest / on the ground
        /// </summary>
        OnGround = (1<<0),
        /// <summary>
        /// Player flag -- Player is fully crouched
        /// </summary>
        Ducking = (1<<1),
        /// <summary>
        /// Player flag -- Player is in the process of crouching or uncrouching but could be in transition
        /// </summary>
        AnimDucking = (1<<2),
        /// <summary>
        /// player jumping out of water
        /// </summary>
        WaterJump = (1<<3),
        /// <summary>
        /// Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
        /// </summary>
        OnTrain = (1<<4),
        /// <summary>
        /// Indicates the entity is standing in rain
        /// </summary>
        InRain = (1<<5),
        /// <summary>
        /// Player is frozen for 3rd person camera
        /// </summary>
        Frozen = (1<<6),
        /// <summary>
        /// Player can't move, but keeps key inputs for controlling another entity
        /// </summary>
        AtControls = (1<<7),
        /// <summary>
        /// Is a player
        /// </summary>
        Client  = (1<<8),
        /// <summary>
        /// Fake client, simulated server side; don't send network messages to them
        /// </summary>
        FakeClient  = (1<<9),
        /// <summary>
        /// In water
        /// </summary>
        InWater  = (1<<10),
        /// <summary>
        /// Changes the SV_Movestep() behavior to not need to be on ground
        /// </summary>
        Fly  = (1<<11),
        /// <summary>
        /// Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)
        /// </summary>
        Swim = (1<<12),
        Conveyor = (1<<13),
        NPC = (1<<14),
        GodMode = (1<<15),
        NoTarget = (1<<16),
        /// <summary>
        /// set if the crosshair needs to aim onto the entity
        /// </summary>
        AimTarget = (1<<17),
        /// <summary>
        /// not all corners are valid
        /// </summary>
        PartialGround = (1<<18),
        /// <summary>
        /// Eetsa static prop!		
        /// </summary>
        StaticProp = (1<<19),
        /// <summary>
        /// worldgraph has this ent listed as something that blocks a connection
        /// </summary>
        Graphed = (1<<20),
        Grenade = (1<<21),
        /// <summary>
        /// Changes the SV_Movestep() behavior to not do any processing
        /// </summary>
        StepMovement = (1<<22),
        /// <summary>
        /// Doesn't generate touch functions, generates Untouch() for anything it was touching when this flag was set
        /// </summary>
        DontTouch = (1<<23),
        /// <summary>
        /// Base velocity has been applied this frame (used to convert base velocity into momentum)
        /// </summary>
        BaseVelocity = (1<<24),
        /// <summary>
        ///  Not moveable/removeable brush entity (really part of the world, but represented as an entity for transparency or something)
        /// </summary>
        WorldBrush = (1<<25),
        /// <summary>
        /// Terrible name. This is an object that NPCs should see. Missiles, for example.
        /// </summary>
        Object = (1<<26),
        /// <summary>
        /// This entity is marked for death -- will be freed by game DLL
        /// </summary>
        KillMe = (1<<27),
        /// <summary>
        ///  You know...
        /// </summary>
        OnFire = (1<<28),
        /// <summary>
        /// We're dissolving!
        /// </summary>
        Dissolving = (1<<29),
        /// <summary>
        /// In the process of turning into a client side ragdoll.
        /// </summary>
        Transragdoll = (1<<30),
        /// <summary>
        /// pusher that can't be blocked by the player
        /// </summary>
        UnblockableByPlayer = (1<<31),
    }


    internal interface IEntity
    {
        int GetIndex(Entity entity);
        String GetClassname(Entity entity);

        Vector GetOrigin(Entity entity);
        void SetOrigin(Entity entity, Vector origin);

        QAngle GetAngles(Entity entity);
        void SetAngles(Entity entity, QAngle angle);

        String GetModel(Entity entity);
        void SetModel(Entity entity, String model);

        String GetName(Entity entity);
        void SetName(Entity entity, String name);

        MoveType GetMoveType(Entity entity);
        void SetMoveType(Entity entity, MoveType movetype);

        CollisionGroup GetCollisionGroup(Entity entity);
        void SetCollisionGroup(Entity entity, CollisionGroup collisionGroup);

        SolidFlags GetSolidFlags(Entity entity);
        void SetSolidFlags(Entity entity, SolidFlags solidFlags);

        EntityFlags GetFlags(Entity entity);
        void SetFlags(Entity entity, EntityFlags flags);

        EntityEffectFlags GetEffectFlags(Entity entity);
        void SetEffectFlags(Entity entity, EntityEffectFlags solidFlags);

        void InitPhysics(Entity entity);
        void InitSpherePhysics(Entity entity, float radius);
        void CreateMotionController(BaseAnimating entity);
        IPhysicsObject GetPhysics(Entity entity);

        void SetSolid(Entity entity, SolidType solid);
        SolidType GetSolid(Entity entity);

        void SetOwner(Entity entity, Entity owner);
        Entity GetOwner(Entity entity);

        void SetTeam(Entity entity, int team);
        int GetTeam(Entity entity);

        void SetSkin(Entity entity, int skin);
        int GetSkin(Entity entity);

        void SetSpawnFlags(Entity entity, int flags);
        int GetSpawnFlags(Entity entity);

        void SetVelocity(Entity entity, Vector velocity);
        Vector GetVelocity(Entity entity);

        void SetParent(Entity entity, Entity parent);
        Entity GetParent(Entity entity);

        void SetColor(Entity entity, Color color);
        Color GetColor(Entity entity);

        void SetHealth(Entity entity, int health);
        int GetHealth(Entity entity);

        void SetMaxHealth(Entity entity, int health);
        int GetMaxHealth(Entity entity);

        void SetLifeState(Entity entity, LifeState state);
        LifeState GetLifeState(Entity entity);

        void SetRenderFx(Entity entity, EntityRenderFx renderFx);
        EntityRenderFx GetRenderFx(Entity entity);

        void SetRenderMode(Entity entity, EntityRenderMode renderMode);
        EntityRenderMode GetRenderMode(Entity entity);

        //OBB
        Vector GetWorldAlignMins(Entity entity);
        Vector GetWorldAlignMaxs(Entity entity);

        //AABB
        void GetAABB(Entity entity, out Vector min, out Vector max);

        void SetNextThink(Entity entity, float time);
        float GetNextThink(Entity entity);

        void SetCollisionBounds(Entity entity, Vector mins, Vector maxs);
        void GetVectors(Entity entity, ref Vector forward, ref Vector left, ref Vector up);

        void Teleport(Entity entity, Vector? newPosition, QAngle? newAngle, Vector? newVelocity);
        void Touch(Entity entity, Entity other);

        void NetworkStateChanged(Entity entity);

        //void EmitSound(Entity entity, string soundName, float soundtime = 0.0f, float? duration = null);
        void StopSound(Entity entity, string soundName);

        void Dispose(Entity entity);



        //CBaseAnimating methods
        int LookupSequence(BaseAnimating entity, string name);
        void ResetSequenceInfo(BaseAnimating entity);

        void SetCycle(BaseAnimating entity, float cycle);
        float GetCycle(BaseAnimating entity);

        void SetBodyGroup(BaseAnimating entity, int iGroup, int iValue);
        int GetBodyGroup(BaseAnimating entity, int iGroup);

        int GetSequence(BaseAnimating entity);
        void SetSequence(BaseAnimating entity, int sequence);
        void ResetSequence(BaseAnimating entity, int nSequence);

        bool IsSequenceLooping(BaseAnimating entity, int sequence);
        bool SequenceLoops(BaseAnimating entity);

        void SetPlaybackRate(BaseAnimating entity, float rate);
        float GetPlaybackRate(BaseAnimating entity);

        int LookupPoseParameter(BaseAnimating entity, string szName);
        float SetPoseParameter(BaseAnimating entity, int iParameter, float flValue);
        float GetPoseParameter(BaseAnimating entity, int iParameter);

        float FrameAdvance(BaseAnimating entity, float flInterval);

        void ForceDrop(PhysCannon entity);
        Entity GetAttached(PhysCannon entity);

        //Static methods
        int PrecacheModel(string name);

        void EmitSound(RecipientFilter filter, int entity, EmitSound ep);

        Entity[] GetEntitiesInBox(Vector min, Vector max, int maxEntities, int flagMask);

        int LookupAttachment(Entity entity, string name);
        bool GetAttachment(Entity entity, int index, out Vector position, out QAngle angles);

        int OnTakeDamage(Entity entity, TakeDamageInfo info);

        void FireNamedOutput(Entity entity, string outputName, Entity activator, Entity caller, float delay);
        //void TriggerOutput(Entity entity, string action, float delay, Entity activator, Entity caller);
    }

    internal class ServerEntity : IEntity
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetIndex(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector GetOrigin(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetOrigin(Entity entity, Vector origin);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern QAngle GetAngles(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetAngles(Entity entity, QAngle angle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern String GetClassname(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern String GetModel(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetModel(Entity entity, String model);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern String GetName(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetName(Entity entity, String name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern MoveType GetMoveType(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetMoveType(Entity entity, MoveType movetype);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern CollisionGroup GetCollisionGroup(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCollisionGroup(Entity entity, CollisionGroup collisionGroup);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern EntityEffectFlags GetEffectFlags(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetEffectFlags(Entity entity, EntityEffectFlags movetype);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern EntityFlags GetFlags(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetFlags(Entity entity, EntityFlags movetype);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void InitPhysics(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void InitSpherePhysics(Entity entity, float radius);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void CreateMotionController(BaseAnimating entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern IPhysicsObject GetPhysics(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetSolid(Entity entity, SolidType movetype);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern SolidType GetSolid(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern SolidFlags GetSolidFlags(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetSolidFlags(Entity entity, SolidFlags solidFlags);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetOwner(Entity entity, Entity owner);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Entity GetOwner(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetTeam(Entity entity, int team);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetTeam(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetSkin(Entity entity, int skin);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetSkin(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetHealth(Entity entity, int health);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetHealth(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetMaxHealth(Entity entity, int health);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetMaxHealth(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetLifeState(Entity entity, LifeState state);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern LifeState GetLifeState(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetSpawnFlags(Entity entity, int flags);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetSpawnFlags(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetVelocity(Entity entity, Vector velocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector GetVelocity(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetParent(Entity entity, Entity parent);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Entity GetParent(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Color GetColor(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetColor(Entity entity, Color color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector GetWorldAlignMins(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector GetWorldAlignMaxs(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void GetAABB(Entity entity, out Vector min, out Vector max);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetNextThink(Entity entity, float time);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float GetNextThink(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCollisionBounds(Entity entity, Vector mins, Vector maxs);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void GetVectors(Entity entity, ref Vector forward, ref Vector left, ref Vector up);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Teleport(Entity entity, Vector? newPosition, QAngle? newAngle, Vector? newVelocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Touch(Entity entity, Entity other);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void NetworkStateChanged(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int PrecacheModel(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetRenderFx(Entity entity, EntityRenderFx renderFx);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern EntityRenderFx GetRenderFx(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetRenderMode(Entity entity, EntityRenderMode renderMode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern EntityRenderMode GetRenderMode(Entity entity);

        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
       // public extern void EmitSound(Entity entity, string soundName, float soundtime = 0.0f, float? duration = null);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void StopSound(Entity entity, string soundName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Dispose(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int LookupSequence(BaseAnimating entity, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void ResetSequenceInfo(BaseAnimating entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCycle(BaseAnimating entity, float cycle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float GetCycle(BaseAnimating entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetBodyGroup(BaseAnimating entity, int iGroup, int iValue);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetBodyGroup(BaseAnimating entity, int iGroup);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetSequence(BaseAnimating entity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetSequence(BaseAnimating entity, int sequence);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void ResetSequence(BaseAnimating entity, int nSequence);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool IsSequenceLooping(BaseAnimating entity, int sequence);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool SequenceLoops(BaseAnimating entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetPlaybackRate(BaseAnimating entity, float rate);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float GetPlaybackRate(BaseAnimating entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int LookupPoseParameter(BaseAnimating entity, string szName);
        //float SetPoseParameter(BaseAnimating entity, string szName, float flValue);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float SetPoseParameter(BaseAnimating entity, int iParameter, float flValue);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float GetPoseParameter(BaseAnimating entity, int iParameter);

        public float FrameAdvance(BaseAnimating entity, float flInterval) { return 0.0f; }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void ForceDrop(PhysCannon entity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Entity GetAttached(PhysCannon entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void EmitSound(RecipientFilter filter, int entity, EmitSound ep);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Entity[] GetEntitiesInBox(Vector min, Vector max, int maxEntities, int flagMask);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void FireNamedOutput(Entity entity, string outputName, Entity activator, Entity caller, float delay);

        public int LookupAttachment(Entity entity, string name)
        {
            throw new InvalidOperationException("Server can not LookupAttachment");
        }
        public bool GetAttachment(Entity entity, int index, out Vector position, out QAngle angles)
        {
            throw new InvalidOperationException("Server can not GetAttachment");
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int OnTakeDamage(Entity entity, TakeDamageInfo info);

        /*
        public void TriggerOutput(Entity entity, string action, float delay, Entity activator, Entity caller)
        {
            EventQueue.AddEventEntity(entity, action, delay, activator, caller);
        }
         * */
    }

    internal class ClientEntity : IEntity
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetIndex(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector GetOrigin(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetOrigin(Entity entity, Vector origin);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern QAngle GetAngles(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetAngles(Entity entity, QAngle angle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern String GetClassname(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern String GetModel(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetModel(Entity entity, String model);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern String GetName(Entity entity);

        public void SetName(Entity entity, String name)
        {
            throw new InvalidOperationException("You can not set the name of an entity on the client!");
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern MoveType GetMoveType(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetMoveType(Entity entity, MoveType movetype);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern CollisionGroup GetCollisionGroup(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCollisionGroup(Entity entity, CollisionGroup collisionGroup);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern SolidFlags GetSolidFlags(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetSolidFlags(Entity entity, SolidFlags solidFlags);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern EntityFlags GetFlags(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetFlags(Entity entity, EntityFlags movetype);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern EntityEffectFlags GetEffectFlags(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetEffectFlags(Entity entity, EntityEffectFlags movetype);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern IPhysicsObject GetPhysics(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetNextThink(Entity entity, float time);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float GetNextThink(Entity entity);

        public void InitPhysics(Entity entity)
        {
            throw new InvalidOperationException("You can not init physics on the client!");
        }

        public void InitSpherePhysics(Entity entity, float radius)
        {
            throw new InvalidOperationException("You can not init physics on the client!");
        }

        public void CreateMotionController(BaseAnimating entity)
        {
            throw new InvalidOperationException("You can not init physics on the client!");
        }


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetSolid(Entity entity, SolidType movetype);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern SolidType GetSolid(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetOwner(Entity entity, Entity owner);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Entity GetOwner(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetTeam(Entity entity, int owner);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetTeam(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetSkin(Entity entity, int skin);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetSkin(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetHealth(Entity entity, int health);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetHealth(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetMaxHealth(Entity entity, int health);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetMaxHealth(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetLifeState(Entity entity, LifeState state);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern LifeState GetLifeState(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetSpawnFlags(Entity entity, int flags);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetSpawnFlags(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetVelocity(Entity entity, Vector velocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector GetVelocity(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetParent(Entity entity, Entity parent);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Entity GetParent(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Color GetColor(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetColor(Entity entity, Color color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int PrecacheModel(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector GetWorldAlignMins(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector GetWorldAlignMaxs(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void GetAABB(Entity entity, out Vector min, out Vector max);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCollisionBounds(Entity entity, Vector mins, Vector maxs);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void GetVectors(Entity entity, ref Vector forward, ref Vector left, ref Vector up);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetRenderFx(Entity entity, EntityRenderFx renderFx);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern EntityRenderFx GetRenderFx(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetRenderMode(Entity entity, EntityRenderMode renderMode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern EntityRenderMode GetRenderMode(Entity entity);

        public void Teleport(Entity entity, Vector? newPosition, QAngle? newAngle, Vector? newVelocity)
        {
            throw new InvalidOperationException("You can not teleport an entity on the client!");
        }

        public void Touch(Entity entity, Entity other)
        {
            throw new InvalidOperationException("You can not touch on the client!");
        }

        public void NetworkStateChanged(Entity entity)
        {
            throw new InvalidOperationException("You can not say that the network state changed on the client!");
        }

       // [MethodImplAttribute(MethodImplOptions.InternalCall)]
        //public extern void EmitSound(Entity entity, string soundName, float soundtime = 0.0f, float? duration = null);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void StopSound(Entity entity, string soundName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Dispose(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int LookupSequence(BaseAnimating entity, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void ResetSequenceInfo(BaseAnimating entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCycle(BaseAnimating entity, float cycle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float GetCycle(BaseAnimating entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetBodyGroup(BaseAnimating entity, int iGroup, int iValue);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetBodyGroup(BaseAnimating entity, int iGroup);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetSequence(BaseAnimating entity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetSequence(BaseAnimating entity, int sequence);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void ResetSequence(BaseAnimating entity, int nSequence);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool IsSequenceLooping(BaseAnimating entity, int sequence);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool SequenceLoops(BaseAnimating entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetPlaybackRate(BaseAnimating entity, float rate);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float GetPlaybackRate(BaseAnimating entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int LookupPoseParameter(BaseAnimating entity, string szName);
        //float SetPoseParameter(BaseAnimating entity, string szName, float flValue);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float SetPoseParameter(BaseAnimating entity, int iParameter, float flValue);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float GetPoseParameter(BaseAnimating entity, int iParameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float FrameAdvance(BaseAnimating entity, float flInterval);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void ForceDrop(PhysCannon entity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Entity GetAttached(PhysCannon entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void EmitSound(RecipientFilter filter, int entity, EmitSound ep);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Entity[] GetEntitiesInBox(Vector min, Vector max, int maxEntities, int flagMask);

        public void FireNamedOutput(Entity entity, string outputName, Entity activator, Entity caller, float delay)
        {
            throw new InvalidOperationException("Client can not FireNamedOutput!");
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int LookupAttachment(Entity entity, string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool GetAttachment(Entity entity, int index, out Vector position, out QAngle angles);

        public int OnTakeDamage(Entity entity, TakeDamageInfo info)
        {
            throw new InvalidOperationException("Client can not OnTakeDamage!");
        }

        /*
        public void TriggerOutput(Entity entity, string action, float delay, Entity activator, Entity caller)
        {
            throw new InvalidOperationException("Client can not TriggerOutput!");
        }
         * */
    }

    public delegate void EntityEvent();

    public class Entity : SharpObject
    {
        internal static IEntity _entity;
        internal static void SetInterface(IEntity entity)
        {
            _entity = entity;
        }

        public Entity()
        {
            throw new InvalidOperationException("You can not create new entitities from ingame");
        }

        public override void Dispose()
        {
            _entity.Dispose(this);
        }

        internal EntityDTArrays _NetworkedFields;

        public int GetNetworkVarCount()
        {
            return 1;
        }

        public int Index
        {
            get { return _entity.GetIndex(this); }
        }

        public Vector Up
        {
            get
            {
                Vector forward = Vector.Zero, left = Vector.Zero, up = Vector.Zero;
                this.GetVectors(ref forward, ref left, ref up);
                return up;
            }
        }

        public Vector Left
        {
            get
            {
                Vector forward = Vector.Zero, left = Vector.Zero, up = Vector.Zero;
                this.GetVectors(ref forward, ref left, ref up);
                return left;
            }
        }

        public Vector Forward
        {
            get
            {
                Vector forward = Vector.Zero, left = Vector.Zero, up = Vector.Zero;
                this.GetVectors(ref forward, ref left, ref up);
                return forward;
            }
        }

        public void GetVectors(ref Vector forward, ref Vector left, ref Vector up)
        {
            _entity.GetVectors(this, ref forward, ref left, ref up);
        }

        public Vector Origin
        {
            get { return _entity.GetOrigin(this); }
            set { _entity.SetOrigin(this, value); }
        }

        public QAngle Angles
        {
            get { return _entity.GetAngles(this); }
            set { _entity.SetAngles(this, value); }
        }

        public String Model
        {
            get { return _entity.GetModel(this); }
            set { _entity.SetModel(this, value); }
        }

        public String Name
        {
            get { return _entity.GetName(this); }
            set { _entity.SetName(this, value); }
        }

        public MoveType MoveType
        {
            get { return _entity.GetMoveType(this); }
            set { _entity.SetMoveType(this, value); }
        }

        public SolidFlags SolidFlags
        {
            get { return _entity.GetSolidFlags(this); }
            set { _entity.SetSolidFlags(this, value); }
        }

        public EntityEffectFlags EffectFlags
        {
            get { return _entity.GetEffectFlags(this); }
            set { _entity.SetEffectFlags(this, value); }
        }

        public EntityFlags Flags
        {
            get { return _entity.GetFlags(this); }
            set { _entity.SetFlags(this, value); }
        }

        public Color Color
        {
            get { return _entity.GetColor(this); }
            set { _entity.SetColor(this, value); }
        }

        public String Classname
        {
            get { return _entity.GetClassname(this); }
        }

        public void InitPhysics()
        {
            _entity.InitPhysics(this);
        }

        public void InitSpherePhysics(float radius)
        {
            _entity.InitSpherePhysics(this, radius);
        }

        public SolidType Solid
        {
            get { return _entity.GetSolid(this); }
            set { _entity.SetSolid(this, value); }
        }

        public EntityRenderFx RenderFX
        {
            get { return _entity.GetRenderFx(this); }
            set { _entity.SetRenderFx(this, value); }
        }

        public EntityRenderMode RenderMode
        {
            get { return _entity.GetRenderMode(this); }
            set { _entity.SetRenderMode(this, value); }
        }

        public CollisionGroup CollisionGroup
        {
            get { return _entity.GetCollisionGroup(this); }
            set { _entity.SetCollisionGroup(this, value); }
        }

        public Entity Owner
        {
            get { return _entity.GetOwner(this); }
            set { _entity.SetOwner(this, value); }
        }

        public Entity Parent
        {
            get { return _entity.GetParent(this); }
            set { _entity.SetParent(this, value); }
        }

        public Vector Velocity
        {
            get { return _entity.GetVelocity(this); }
            set { _entity.SetVelocity(this, value); }
        }

        public int Team
        {
            get { return _entity.GetTeam(this); }
            set { _entity.SetTeam(this, value); }
        }

        public int Skin
        {
            get { return _entity.GetSkin(this); }
            set { _entity.SetSkin(this, value); }
        }

        public int Health
        {
            get { return _entity.GetHealth(this); }
            set { _entity.SetHealth(this, value); }
        }

        public int MaxHealth
        {
            get { return _entity.GetMaxHealth(this); }
            set { _entity.SetMaxHealth(this, value); }
        }

        public LifeState LifeState
        {
            get { return _entity.GetLifeState(this); }
            set { _entity.SetLifeState(this, value); }
        }

        public int SpawnFlags
        {
            get { return _entity.GetSpawnFlags(this); }
            set { _entity.SetSpawnFlags(this, value); }
        }

        public float NextThink
        {
            get { return _entity.GetNextThink(this); }
            set { _entity.SetNextThink(this, value); }
        }

        public IPhysicsObject Physics
        {
            get { return _entity.GetPhysics(this); }
        }

        public Vector WorldAlignMins
        {
            get { return _entity.GetWorldAlignMins(this); }
        }

        public Vector WorldAlignMaxs
        {
            get { return _entity.GetWorldAlignMaxs(this); }
        }

        public void GetAABB(out Vector mins, out Vector maxs)
        {
            _entity.GetAABB(this, out mins, out maxs);
        }

        public void SetCollisionBounds(Vector mins, Vector maxs)
        {
            _entity.SetCollisionBounds(this, mins, maxs);
        }

        public void Teleport(Vector? newPosition, QAngle? newAngle, Vector? newVelocity)
        {
            _entity.Teleport(this, newPosition, newAngle, newVelocity);
        }

        public void NetworkStateChanged()
        {
            _entity.NetworkStateChanged(this);
        }

        public void EmitSound(string soundName, float soundtime = 0.0f, float? duration = null)
        {
            //_entity.EmitSound(this, soundName, soundtime, duration);
            CPASAttenuationFilter filter = new CPASAttenuationFilter(); //TODO: Correct counstructor
             
            EmitSound es = new EmitSound();
            es.SoundName = soundName;
            es.SoundTime = soundtime;
            es.SoundDuration = duration;
            es.WarnOnDirectWaveReference = true;

            EmitSound(filter, this.Index, es);
        }

        public int LookupAttachment(string name)
        {
            return _entity.LookupAttachment(this, name);
        }

        public bool GetAttachment(int index, out Vector position, out QAngle angles)
        {
            return _entity.GetAttachment(this, index, out position, out angles);
        }

        public bool GetAttachment(int index, out Vector position)
        {
            QAngle angles;
            return _entity.GetAttachment(this, index, out position, out angles);
        } 

        public void StopSound(string soundName)
        {
            _entity.StopSound(this, soundName);
        }

        public void Touch(Entity other)
        {
            _entity.Touch(this, other);
        }

        protected virtual void Initialize()
        {
        }

        /// <summary>
        /// Parse data from a map file
        /// </summary>
        /// <param name="keyName"></param>
        /// <param name="keyValue"></param>
        /// <returns></returns>
        public virtual bool KeyValue(string keyName, string keyValue)
        {
            return false;
        }

        /// <summary>
        /// Server only. Called when other entities interact with this one.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="activator"></param>
        /// <param name="caller"></param>
        /// <returns></returns>
        public virtual bool AcceptInput(String name, Entity activator, Entity caller)
        {
            return false;
        }

        public virtual void StartTouch(Entity other)
        {
        }

        //Called only on the client
        public virtual bool ShouldDraw()
        {
            return true;
        }

        public virtual void Think()
        {
        }

        public virtual void PhysicsSimulate()
        {
        }

        public virtual void PhysicsCollision(CollisionEvent collisionEvent)
        {
        }

        public virtual void Spawn() { }

        /**
         * <summary>
         * Called on the client when the server sends an usermessage to this specific entity
         * </summary>
         * **/
        public virtual void ReceiveMessage(BitRead msgData)
        {
        }

        public virtual MotionEvent Simulate(ref Vector linear, ref Vector angularImpulse, float dealtaTime, IPhysicsObject pObject)
        {
            return MotionEvent.NOTHING;
        }

        public static int PrecacheModel(string name)
        {
            return _entity.PrecacheModel(name);
        }

        public override string ToString()
        {
            return "Entity[" + this.Index + "," + this.Classname + "]";
        }

        public static void EmitSound(RecipientFilter filter, int entity, EmitSound ep)
        {
            _entity.EmitSound(filter, entity, ep);
        }

        public static Entity[] FindInBox(Vector min, Vector max, int maxEntities = 256, int flagMask = 0)
        {
            return _entity.GetEntitiesInBox(min, max, maxEntities, flagMask);
        }

        public virtual int OnTakeDamage(TakeDamageInfo info)
        {
            Console.WriteLine("Damage: {0} from {1} to {2}", info.Damage, info.Attacker, info.Inflictor);
            return _entity.OnTakeDamage(this, info);
        }

        #region EntityTriggers
        private Dictionary<string, BaseEntityOutput> Outputs;

        public void StoreOutput(string key, string value)
        {
            if (Outputs == null)
                Outputs = new Dictionary<string, BaseEntityOutput>();

            BaseEntityOutput output;

            if (!Outputs.TryGetValue(key, out output))
            {
                output = new BaseEntityOutput();
                Outputs[key] = output;
            }

            output.ParseEventAction(value);
        }

        //public void TriggerOutput(string action, float delay, Entity activator, Entity caller)
        //{
            //_entity.TriggerOutput(this, action, delay, activator, caller);
        //}

        public void FireNamedOutput(string outputName, Entity activator, Entity caller, float delay = 0.0f)
        {
            if (Outputs != null)
            {
                BaseEntityOutput output;
                if (Outputs.TryGetValue(outputName, out output))
                {
                    output.FireOutput(activator, caller, delay);
                    return;
                }
            }
            
            _entity.FireNamedOutput(this, outputName, activator, caller, delay);
        }
        #endregion

        
    }

    public class BaseAnimating : Entity
    {
        public void CreateMotionController()
        {
            Entity._entity.CreateMotionController(this);
        }

        public int LookupSequence(string name)
        {
            return _entity.LookupSequence(this, name);
        }

        public void ResetSequenceInfo()
        {
            _entity.ResetSequenceInfo(this);
        }

        public float Cycle
        {
            get { return _entity.GetCycle(this); }
            set { _entity.SetCycle(this, value); }
        }

        public void SetBodygroup(int iGroup, int iValue)
        {
            _entity.SetBodyGroup(this, iGroup, iValue);
        }
        public int GetBodyGroup(int iGroup)
        {
            return _entity.GetBodyGroup(this, iGroup);
        }

        public int Sequence
        {
            get { return _entity.GetSequence(this); }
            set { _entity.SetSequence(this, value); }
        }

        public void ResetSequence(int nSequence)
        {
            _entity.ResetSequence(this, nSequence);
        }

        public bool IsSequenceLooping(int sequence)
        {
            return _entity.IsSequenceLooping(this, sequence);
        }

        public bool SequenceLoops()
        {
            return _entity.SequenceLoops(this);
        }

        public float PlaybackRate
        {
            get { return _entity.GetPlaybackRate(this); }
            set { _entity.SetPlaybackRate(this, value); }
        }

        public int LookupPoseParameter(string szName)
        {
            return _entity.LookupPoseParameter(this, szName);
        }

        public float SetPoseParameter(int iParameter, float flValue)
        {
            return _entity.SetPoseParameter(this, iParameter, flValue);
        }

        public float GetPoseParameter(int iParameter)
        {
            return _entity.GetPoseParameter(this, iParameter);
        }

        public float SetPoseParameter(string szName, float flValue)
        {
            return SetPoseParameter(LookupPoseParameter(Name), flValue);
        }

        public float GetPoseParameter(string szName)
        {
            return GetPoseParameter(LookupPoseParameter(Name));
        }

        public float FrameAdvance(float flInterval)
        {
            return _entity.FrameAdvance(this, flInterval);
        }        
    }

    public sealed class ClientBaseAnimating : BaseAnimating
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern BaseAnimating CreateClientEntity(string modelName, bool bRenderWithViewModels);
    }

    public class FuncBrush : Entity
    {
        public override void Spawn()
        {
            base.Spawn();

            this.Solid = SolidType.BSP;
            this.MoveType = MoveType.NONE;
            this.SolidFlags |= SolidFlags.Trigger;
            this.EffectFlags |= EntityEffectFlags.NoDraw;
        }

    }

    public class BaseEntityOutput
    {
        protected EventAction ActionList;

        public void ParseEventAction(string eventData)
        {
            AddEventAction(new EventAction(eventData));
        }

        public void AddEventAction(EventAction eventAction)
        {
            eventAction.Next = ActionList;
            ActionList = eventAction;
        }

        /// <summary>
        /// Fires the event, causing a sequence of action to occur in other ents.
        /// </summary>
        /// <param name="Value"></param>
        /// <param name="Activator">Entity that initiated this sequence of actions.</param>
        /// <param name="Caller">Entity that is actually causing the event.</param>
        /// <param name="delay"></param>
        public void FireOutput(Entity activator, Entity caller, float delay)
        {
            EventAction ev = ActionList;
            EventAction prev = null;

            while (ev != null)
            {
                if (ev.Parameter.Length == 0)
                {
                    EventQueue.AddEvent(ev.Target, ev.TargetInput, ev.Delay + delay, activator, caller);
                }
                else
                {
                    //TODO
                    Console.WriteLine("TODO: Running events with parameters");
                }

                bool remove = false;
                if (ev.TimesToFire != EventAction.FireAlways)
                {
                    ev.TimesToFire--;
                    if (ev.TimesToFire == 0)
                        remove = true;
                }

                if (!remove)
                {
                    prev = ev;
                    ev = ev.Next;
                }
                else
                {
                    if (prev != null)
                        prev.Next = ev.Next;
                    else
                        ActionList = ev.Next;

                    ev = ev.Next;
                }
            }

        }

    }

    public class EventAction
    {
        public static readonly int FireAlways = -1;

        /// <summary>
        /// name of the entity(s) to cause the action in
        /// </summary>
        public string Target;

        /// <summary>
        /// the name of the action to fire
        /// </summary>
        public string TargetInput;

        /// <summary>
        /// parameter to send, 0 if none
        /// </summary>
        public string Parameter;

        /// <summary>
        /// the number of seconds to wait before firing the action
        /// </summary>
        public float Delay;

        /// <summary>
        /// The number of times to fire this event, or EVENT_FIRE_ALWAYS.
        /// </summary>
        public int TimesToFire;

        public EventAction Next;

        public EventAction(string actionData)
        {

            string[] data = actionData.Split(',');

            Target = data[0];
            TargetInput = data[1];
            Parameter = data[2];
            Delay = float.Parse(data[3]);
            TimesToFire = int.Parse(data[4]);

            if (TargetInput.Length == 0)
                TargetInput = "Use";

            if (TimesToFire == 0)
                TimesToFire = FireAlways;
        }

    }

    public static class EventQueue
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void AddEvent(string target, string action, float fireDelay, Entity activator, Entity caller);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void AddEventEntity(Entity target, string action, float fireDelay, Entity activator, Entity caller);

        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //public static extern void AddEvent(Entity target, string action, int value, float fireDelay, Entity activator, Entity caller);

    }

    public delegate void EntityChange(Entity entity);

    public class EntityManager
    {
        private static Dictionary<String, Type> registeredEntities =
            new Dictionary<string, Type>();
        public static void Register(String classname, Type type)
        {
            if (!type.IsSubclassOf(typeof(BaseAnimating)) && !type.IsSubclassOf(typeof(FuncBrush)))
                throw new InvalidOperationException(type.Name + " is not a subclass of Entity");

            registeredEntities.Add(classname, type);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern Entity CreateEntity(String name);

        public static Type GetType(String classname)
        {
            if (!registeredEntities.ContainsKey(classname))
                return null;
            return registeredEntities[classname];
        }

        public static event EntityChange OnCreate;
        public static event EntityChange OnRemove;

        private static void CallCreate(Entity entity)
        {
            if (OnCreate != null)
                OnCreate(entity);
        }

        private static void CallDelete(Entity entity)
        {
            if (OnRemove != null)
                OnRemove(entity);
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class TakeDamageInfo
    {
        public Vector DamageForce;
        public Vector DamagePosition;
        public Vector ReportedPosition;

        public Entity Inflictor;
        public Entity Attacker;
        public Entity Weapon;

        public float Damage;
        public float MaxDamage;
        public float BaseDamage;

        public int DamageType;
        public int DamageCustom;
        public int DamgeStats;
        public int AmmoType;
        public int DamageOtherPlayers;
        public int PlayerPenetrationCount;
        public float DamageBonus;
    }
}