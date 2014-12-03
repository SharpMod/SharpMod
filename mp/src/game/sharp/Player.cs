using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Sharp
{
    [Flags]
    public enum PlayerButtons
    {
        NONE = 0,
        ATTACK = (1 << 0),
        JUMP = (1 << 1),
        DUCK = (1 << 2),
        FORWARD = (1 << 3),
        BACK = (1 << 4),
        USE = (1 << 5),
        CANCEL = (1 << 6),
        LEFT = (1 << 7),
        RIGHT = (1 << 8),
        MOVELEFT = (1 << 9),
        MOVERIGHT = (1 << 10),
        ATTACK2 = (1 << 11),
        RUN = (1 << 12),
        RELOAD = (1 << 13),
        ALT1 = (1 << 14),
        ALT2 = (1 << 15),
        SCORE = (1 << 16),   // Used by client.dll for when scoreboard is held down
        SPEED = (1 << 17),	// Player is holding the speed key
        WALK = (1 << 18),	// Player holding walk key
        ZOOM = (1 << 19),	// Zoom key for HUD zoom
        WEAPON1 = (1 << 20),	// weapon defines these bits
        WEAPON2 = (1 << 21),	// weapon defines these bits
        BULLRUSH = (1 << 22),
        GRENADE1 = (1 << 23),	// grenade 1
        GRENADE2 = (1 << 24),	// grenade 2
        LOOKSPIN = (1 << 25)
    }

    public enum SpectatorMovement : int
    {
        /// <summary> not in spectator mode </summary>
        None = 0,
        /// <summary> special mode for death cam animation </summary>
        DeathCam,
        /// <summary> zooms to a target, and freeze-frames on them </summary>
        FreezeCam,
        /// <summary> view from a fixed camera position </summary>
        Fixed,
        /// <summary> follow a player in first person view </summary>
        InEye,
        /// <summary> follow a player in third person view </summary>
        Chase,
        /// <summary> free roaming </summary>
        Roaming
    }

    internal interface IPlayer
    {
        String GetName(Player player);
        QAngle GetEyeAngles(Player player);
        PlayerButtons GetButtons(Player player);
        PlayerButtons GetButtonsLast(Player player);
        PlayerButtons GetButtonsPressed(Player player);
        PlayerButtons GetButtonsReleased(Player player);
        void RemoveAllItems(Player player, bool removeSuit);
        void RemoveAllWeapons(Player player);
        Vector GetEyePosition(Player player);

        SpectatorMovement GetSpectatorMovement(Player player);
        void SetSpectatorMovement(Player player, SpectatorMovement movement);

        Entity GetObserverTarget(Player player);
        void SetObserverTarget(Player player, Entity entity);

        Entity GetWeapon(Player player, int id);
        void SelectItem(Player player, string name, int iSubType);

        int GetAmmo(Player player, string szName);
        void GiveAmmo(Player player, string szName, int iCount, bool bSuppressSound);
        void RemoveAllAmmo(Player player);

        void SetArmor(Entity entity, int armor);
        int GetArmor(Entity entity);

        string GetConVarValue(Player player, string name);
        bool CanPickupObject(Player player, Entity other, float massLimit, float sizeLimit);

        BaseAnimating GetActiveWeapon(Player player);
        
    }

    internal class ServerPlayer : IPlayer
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern String GetName(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern QAngle GetEyeAngles(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern PlayerButtons GetButtons(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern PlayerButtons GetButtonsPressed(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern PlayerButtons GetButtonsLast(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern PlayerButtons GetButtonsReleased(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void RemoveAllItems(Player player, bool removeSuit);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector GetEyePosition(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern Player GetPlayerByIndex(int index);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern SpectatorMovement GetSpectatorMovement(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetSpectatorMovement(Player player, SpectatorMovement movement);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Entity GetObserverTarget(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetObserverTarget(Player player, Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Entity GetWeapon(Player player, int id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SelectItem(Player player, string name, int iSubType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetAmmo(Player player, string szName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void GiveAmmo(Player player, string szName, int iCount, bool bSuppressSound);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void RemoveAllAmmo(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern string GetConVarValue(Player player, string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetArmor(Entity entity, int armor);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetArmor(Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void RemoveAllWeapons(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool CanPickupObject(Player player, Entity other, float massLimit, float sizeLimit);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern BaseAnimating GetActiveWeapon(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void ChangeTeam(Player player, int iTeam);
    }

    internal class ClientPlayer : IPlayer
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern String GetName(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern QAngle GetEyeAngles(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern PlayerButtons GetButtons(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern PlayerButtons GetButtonsPressed(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern PlayerButtons GetButtonsLast(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern PlayerButtons GetButtonsReleased(Player player);

        public void RemoveAllItems(Player player, bool removeSuit)
        {
            throw new InvalidOperationException("The client-side instance can not strip the player weapons.");
        }

        public void RemoveAllWeapons(Player player)
        {
            throw new InvalidOperationException("The client-side instance can not strip the player weapons.");
        }

        public void SetArmor(Entity entity, int armor)
        {
            throw new InvalidOperationException("Can not set armor from the client.");
        }

        public int GetArmor(Entity entity)
        {
            throw new InvalidOperationException("Can not get armor from the client.");
        }

        public bool CanPickupObject(Player player, Entity other, float massLimit, float sizeLimit)
        {
            throw new InvalidOperationException("CanPickupObject can not be called from client.");
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector GetEyePosition(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern Player GetPlayerByIndex(int index);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern SpectatorMovement GetSpectatorMovement(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetSpectatorMovement(Player player, SpectatorMovement movement);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Entity GetObserverTarget(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetObserverTarget(Player player, Entity entity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Entity GetWeapon(Player player, int id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SelectItem(Player player, string name, int iSubType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetAmmo(Player player, string szName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void GiveAmmo(Player player, string szName, int iCount, bool bSuppressSound);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void RemoveAllAmmo(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern string GetConVarValue(Player player, string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetArmor(Player player, int armor);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetArmor(Player player);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern BaseAnimating GetActiveWeapon(Player player);
    }

    public delegate void PlayerEvent(Player player);

    public class Player : BaseAnimating
    {
        private static IPlayer _player;
        internal static void SetInterface(IPlayer player)
        {
            _player = player;
        }

        public static int MAX_WEAPONS = 48; //Hard coded internally
        public static int SF_NORESPAWN = 1 << 30; //Do not ask; Spawn Flags are hard coded all over the engine.

        public event PlayerEvent PlayerInitialSpawn;

        public Dictionary<string, object> Properties;

        public String PlayerName
        {
            get { return _player.GetName(this); }
        }

        public int Armor
        {
            get { return _player.GetArmor(this); }
            set { _player.SetArmor(this, value); }
        }

        public QAngle EyeAngles
        {
            get { return _player.GetEyeAngles(this); }
        }

        public Vector AimVector
        {
            get { return EyeAngles.Forward; }
        }

        public PlayerButtons Buttons
        {
            get { return _player.GetButtons(this); }
        }

        public PlayerButtons ButtonsPressed
        {
            get { return _player.GetButtonsPressed(this); }
        }

        public PlayerButtons ButtonsReleased
        {
            get { return _player.GetButtonsReleased(this); }
        }

        public PlayerButtons ButtonsLast
        {
            get { return _player.GetButtonsLast(this); }
        }

        public void RemoveAllItems(bool removeSuit)
        {
            _player.RemoveAllItems(this, removeSuit);
        }

        public void RemoveAllWeapons()
        {
            _player.RemoveAllWeapons(this);
        }

        public BaseAnimating GetActiveWeapon()
        {
            return _player.GetActiveWeapon(this);
        }

        public Vector EyePosition
        {
            get { return _player.GetEyePosition(this); }
        }

        public SpectatorMovement ObserverMove
        {
            get { return _player.GetSpectatorMovement(this); }
            set { _player.SetSpectatorMovement(this, value); }
        }

        public Entity ObserverTarget
        {
            get { return _player.GetObserverTarget(this); }
            set { _player.SetObserverTarget(this, value); }
        }

        public virtual void SelectItem(string name, int iSubType = 0)
        {
            _player.SelectItem(this, name, iSubType);
        }
       
        public int GetAmmo(string szName)
        {
            return _player.GetAmmo(this, szName);
        }

        public void GiveAmmo(string szName, int iCount, bool bSuppressSound = false)
        {
            _player.GiveAmmo(this, szName, iCount, bSuppressSound);
        }

        public void RemoveAllAmmo()
        {
            _player.RemoveAllAmmo(this);
        }

        public string GetConVar(string name)
        {
            return _player.GetConVarValue(this, name);
        }

        public bool CanPickupObject(Entity other, float massLimit, float sizeLimit)
        {
            return _player.CanPickupObject(this, other, massLimit, sizeLimit);
        }

        public override void Spawn()
        {
            base.Spawn();
            Properties = new Dictionary<string, object>();

            Flags |= EntityFlags.OnGround;
            Flags &= ~EntityFlags.Frozen;

            //The internal implementation of setting the player position is removed and must be set manually
            if (Game.Gamemode != null)
            {
                Game.Gamemode.PlayerSpawn(this);
            }
        }

        //Called by the server when the player spawns the first time.
        public void InitialSpawn()
        {
            if (PlayerInitialSpawn != null)
                PlayerInitialSpawn.Invoke(this);
        }

        public TraceResponse GetEyeTrace()
        {
            return EngineTrace.TraceRay(EyePosition, EyePosition + EyeAngles.Forward * 20000.0f, Mask.PLAYERSOLID, CollisionGroup.Weapon, null);
        }

        public virtual void ChangeTeam(int iTeam)
        {
            ServerPlayer.ChangeTeam(this, iTeam);
            Spawn();
        }

        /*
        public override void Touch(Entity other)
        {
            if (other == EntityManager.GetGroundEntity())
                return;

            if (other.MoveType != MoveType.VPHYSICS || other.Solid != SolidType.VPHYSICS || other.SolidFlags.HasFlag(SolidFlags.Trigger))
                return;

            IPhysicsObject phys = other.Physics;
            if (phys == null || !phys.Movable)
                return;

            TouchedPhysics = true;
        }
        */

        //Server call
        //CBaseEntity	*CBasePlayer::GiveNamedItem( const char *pszName, int iSubType )
        //Player.cpp: 5658
        public Entity Give(String pszName, int subType = 0)
        {
            //Check if player already has weapon
            //TODO: Also check for weapon subtype
            if (GetWeapons().Any(w => w.Classname == pszName))
                return null;

            Entity ent = EntityManager.CreateEntity(pszName);
            Console.WriteLine("Created item: {0} {1}", pszName, ent);
            if (ent == null)
                return null;

            ent.Origin = Origin;
            ent.SpawnFlags |= SF_NORESPAWN;

            //TODO: Create weapons and subtypes
            //if (ent is Weapon)
            //    ent.SetSubType(subType);

            //ent.Spawn();
            Game.DispatchSpawn(ent);

            if (ent != null) //if (!ent.IsMarkedForDeletion())
                ent.Touch(this);

            return ent;
        }

        public IEnumerable<Entity> GetWeapons()
        {
            for (int i = 0; i < MAX_WEAPONS; i++)
            {
                Entity weapon = _player.GetWeapon(this, i);

                if (weapon != null)
                    yield return weapon;
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern Player GetLocalPlayer();
    }

}
