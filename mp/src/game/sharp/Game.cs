using System;
using System.Runtime.CompilerServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Sharp
{
    public interface IPlayerView
    {
        void CalcPlayerView(Player player, ref Vector eyeOrigin, ref QAngle eyeAngles, ref float fov);
    }

    public enum PhysGunPickup_t
    {
        PickedUpByCannon,
        PuntedByCannon,
        PickedUpByPlayer // Picked up by +USE, not physgun.
    };

    public enum PhysGunDrop_t
    {
        DroppedByPlayer,
        ThrownByPlayer,
        DroppedByCannon,
        LaunchedByCannon
    };

    public static class Game
    {
        private static List<IThinkable> ThinkEntries = new List<IThinkable>();
        private static Gamemode _gamemode;

        public extern static float CurTime
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        public extern static float FrameTime
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        public extern static int MaxClients
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void ChangeLevel(string map);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern Entity GetNextEntity(Entity entity);

        public static IEnumerable<Entity> GetEntities()
        {
            Entity entity = null;

            while ((entity = GetNextEntity(entity)) != null)
            {
                yield return entity;
            }
        }

        public static IEnumerable<Entity> GetEntitiesByName(string name)
        {
            return GetEntities().Where((entity) => entity.Name == name);
        }

        public static IEnumerable<Entity> GetEntitiesByClassname(string name)
        {
            return GetEntities().Where((entity) => entity.Classname == name);
        }

        public static Player GetPlayerByIndex(int index)
        {
            return Sharp.SERVER ? 
                ServerPlayer.GetPlayerByIndex(index) :
                ClientPlayer.GetPlayerByIndex(index);
        }

        public static IEnumerable<Player> GetPlayers()
        {
            int maxPlayers = MaxClients;

            for (int i = 1; i <= maxPlayers; i++ )
            {
                Player player = GetPlayerByIndex(i);

                if( player != null )
                    yield return player;
            }
        }


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern Entity GetWorldEntity();

        private static void GameThink()
        {
            ThinkEntries.RemoveAll((thinkEntry) => thinkEntry.Think());

            if (Gamemode != null)
                Gamemode.Think();

            Timer.Think();
        }

        public static void CalcPlayerView(Player player, ref Vector eyeOrigin, ref QAngle eyeAngles, ref float fov)
        {
            if (Gamemode != null)
                Gamemode.CalcPlayerView(player, ref eyeOrigin,ref  eyeAngles, ref fov);
        }

        public static Gamemode Gamemode
        {
            get
            {
                return _gamemode;
            }
            set
            {
                if (_gamemode != null)
                {
                    throw new InvalidOperationException("A gamemode is already set!");
                }

                _gamemode = value;
                _gamemode.OnStart();
            }

        }

        public static void AddThinkable(IThinkable thinkEntry)
        {
            ThinkEntries.Add(thinkEntry);
        }

        public static void RemoveThinkable(IThinkable thinkEntry)
        {
            ThinkEntries.Remove(thinkEntry);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int DispatchSpawn(Entity entity);
    }

    

    public class Gamemode
    {

        public virtual void OnStart()
        {

        }

        /// <summary>
        /// Update the player position when the player spawns
        /// </summary>
        /// <param name="player"></param>
        public virtual void PlayerSpawn(Player player)
        {
            
        }

        public virtual void CalcPlayerView(Player player, ref Vector eyeOrigin, ref QAngle eyeAngles, ref float fov)
        {
            Entity owner = player.Owner;
            if (owner != null && owner is IPlayerView)
            {
                (owner as IPlayerView).CalcPlayerView(player, ref eyeOrigin, ref eyeAngles, ref fov);
            }
        }

        public virtual void Think()
        {
        }

        public virtual bool PhysCannonCanPickupObject(Player player, Entity other, float maxMass)
        {
            return player.CanPickupObject(other, maxMass, 0.0f);
        }

        public virtual void PhysCannonPickup(Player player, Entity entity, PhysGunPickup_t reason)
        {
        }

        public virtual void PhysCannonDrop(Player player, Entity entity, PhysGunDrop_t reason)
        {
        }
    }

    public interface IThinkable
    {
        bool Think();
    }
}
