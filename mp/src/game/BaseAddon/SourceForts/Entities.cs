using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Sharp;
using BaseAddon;

namespace SourceForts
{
    [Entity("info_player_team_spawn")]
    public class InfoPlayerTeamSpawn : BaseAnimating
    {
    }

    public abstract class EnvBlockSpawnerBase : BaseAnimating
    {
        public abstract String TargetClass { get; }

        public void SpawnBlock(Player player)
        {
            SfBlockBase block = EntityManager.CreateEntity(TargetClass) as SfBlockBase;

            if (block == null)
            {
                Console.WriteLine("Unable to spawn block: {0}", TargetClass);
                return;
            }

            block.Angles = this.Angles;
            block.Origin = this.Origin;
            //block.Owner = player;
            block.Spawn();
            block.SetSpawner(player);
        }

        public bool CanSpawn(Entity entity)
        {
            return Block.AllowSpawn(entity.Team);
        }

        public override bool AcceptInput(string name, Entity activator, Entity caller)
        {
            Console.WriteLine("Got input: {0} {1}", name, activator);

            if (Base.SourceForts.State != GameState.BUILD)
                return true;

            if (name == "Spawn" && activator is Player && CanSpawn(activator))
                SpawnBlock(activator as Player);

            return true;
        }
    }

    [Entity("env_block_spawner_3d_1x2")]
    public class EnvBlockSpawner3D1x2 : EnvBlockSpawnerBase
    {
        public override String TargetClass { get { return "sf_block_3d_1x2"; } }
    }

    [Entity("env_block_spawner_3d_1x1")]
    public class EnvBlockSpawner3D1x1 : EnvBlockSpawnerBase
    {
        public override String TargetClass { get { return "sf_block_3d_1x1"; } }
    }

    [Entity("env_block_spawner_2d_5x4")]
    public class EnvBlockSpawner2D5x4 : EnvBlockSpawnerBase
    {
        public override String TargetClass { get { return "sf_block_2d_5x4"; } }
    }

    [Entity("env_block_spawner_2d_2x3")]
    public class EnvBlockSpawner2D2x3 : EnvBlockSpawnerBase
    {
        public override String TargetClass { get { return "sf_block_2d_2x3"; } }
    }

    [Entity("env_block_spawner_2d_2x2")]
    public class EnvBlockSpawner2D2x2 : EnvBlockSpawnerBase
    {
        public override String TargetClass { get { return "sf_block_2d_2x2"; } }
    }

    [Entity("env_block_spawner_2d_1x5")]
    public class EnvBlockSpawner2D1x5 : EnvBlockSpawnerBase
    {
        public override String TargetClass { get { return "sf_block_2d_1x5"; } }
    }

    [Entity("env_block_spawner_2d_1x2")]
    public class EnvBlockSpawner2D1x2 : EnvBlockSpawnerBase
    {
        public override String TargetClass { get { return "sf_block_2d_1x2"; } }
    }

    public abstract class SfBlockBase : BaseAnimating
    {
        public abstract String SpwanModel { get; }

        [Networked]
        public bool AllowMotion;

        [Networked]
        public int Life;

        [Networked]
        public int LastTocuhedTeam;

        [Networked]
        public Player LastTocuhedPlayer;

        public float HealthFrac
        {
            get { return MathX.Clamp(((float)Life) / Block.MaxHealth, 0.0f, 1.0f); }
        }

        public override void Spawn()
        {
            base.Spawn();

            this.Model = this.SpwanModel;
            if (Sharp.Sharp.SERVER)
            {
                this.InitPhysics();
                this.Physics.Mass = 100.0f;
                this.Solid = SolidType.VPHYSICS;
                this.MoveType = MoveType.VPHYSICS;
                this.Physics.Gravity = true;

                this.AllowMotion = false;
                this.ResetLife();
            }
        }

        public void UpdateSkin(Player player)
        {
            if (player != null)
            {
                LastTocuhedPlayer = player;
                LastTocuhedTeam = player.Team;
            }

            int skinId = (int)MathX.Clamp((float)Math.Floor((1.0f - HealthFrac) * 6.0f), 0.0f, 5.0f);

            if (LastTocuhedPlayer.Team == (int)Teams.Blue)
                skinId += 6;

            Skin = skinId;
        }

        public void ResetLife()
        {
            this.Life = 6000;
        }

        public bool AllowFreeze()
        {
            Vector min, max;
            GetAABB(out min, out max);

            return Entity.FindInBox(min, max).All((x) => !(x is EntityFuncNoFreeze));
        }

        internal void CheckMotion(Player player, bool motion)
        {
            if (motion == true)
            {
                SetAllowMotion(true);
                return;
            }

            if (!AllowFreeze())
                return;

            SetAllowMotion(false);
        }

        public void SetAllowMotion(bool motion)
        {
            AllowMotion = motion;

            if (Sharp.Sharp.SERVER && Physics != null)
            {
                Physics.Motion = motion;

                if (motion == true)
                    Physics.Wake();
            }
        }

        internal void SetSpawner(Player player)
        {
            UpdateSkin(player);
        }
    }

    [Entity("game_phase_control")]
    public class GamePhaseControl : BaseAnimating
    {
        public override bool KeyValue(string keyName, string keyValue)
        {
            if (keyName.Substring(0, 2).ToLower() == "on")
            {
                this.StoreOutput(keyName, keyValue);
            }

            return base.KeyValue(keyName, keyValue);
        }

        public void FireGameStateChange(GameState state)
        {
            FireNamedOutput("OnSwitchPhase", this, this);

            switch (state)
            {
                case GameState.BUILD:
                    FireNamedOutput("OnBuildPhase", this, this);
                    break;
                case GameState.FIGHT:
                    FireNamedOutput("OnCombatPhase", this, this);
                    break;
            }

        }

    }


    [Entity("sf_block_3d_1x2")]
    public class SfBlock3D1x2 : SfBlockBase
    {
        public override String SpwanModel { get { return "models/sf/3d_1x2.mdl"; } }
    }

    [Entity("sf_block_3d_1x1")]
    public class SfBlock3D1x1 : SfBlockBase
    {
        public override String SpwanModel { get { return "models/sf/3d_1x1.mdl"; } }
    }

    [Entity("sf_block_2d_5x4")]
    public class SfBlock2D5x4 : SfBlockBase
    {
        public override String SpwanModel { get { return "models/sf/2d_5x4.mdl"; } }
    }

    [Entity("sf_block_2d_2x3")]
    public class SfBlock2D2x3 : SfBlockBase
    {
        public override String SpwanModel { get { return "models/sf/2d_2x3.mdl"; } }
    }

    [Entity("sf_block_2d_2x2")]
    public class SfBlock2D2x2 : SfBlockBase
    {
        public override String SpwanModel { get { return "models/sf/2d_2x2.mdl"; } }
    }

    [Entity("sf_block_2d_1x5")]
    public class SfBlock2D1x5 : SfBlockBase
    {
        public override String SpwanModel { get { return "models/sf/2d_1x5.mdl"; } }
    }

    [Entity("sf_block_2d_1x2")]
    public class SfBlock2D1x2 : SfBlockBase
    {
        public override String SpwanModel { get { return "models/sf/2d_1x2.mdl"; } }
    }

    [Entity("prop_flag")]
    public class PropFlagStand : BaseAnimating
    {
        public override bool KeyValue(string keyName, string keyValue)
        {
            if (keyName.ToLower() == "teamnum")
            {
                int id = int.Parse(keyValue);
                this.Team = id;
                return true;
            }

            return base.KeyValue(keyName, keyValue);
        }
    }

    [Entity("func_nofreeze")]
    public class EntityFuncNoFreeze : FuncBrush
    {
        protected override void Initialize()
        {
            base.Initialize();

            this.Solid = SolidType.NONE;
            this.SolidFlags = Sharp.SolidFlags.NotSolid; 
        }

        public override void StartTouch(Entity other)
        {
            Console.WriteLine("EntityFuncNoFreeze {0}", other);
        }
    }


    //[Entity("func_flagzone")]
    public class EntityFlagZone : FuncBrush
    {
        protected override void Initialize()
        {
            base.Initialize();

            this.Solid = SolidType.NONE;
            this.SolidFlags = Sharp.SolidFlags.NotSolid; 
        }

        public override void StartTouch(Entity other)
        {
            if (other is Player)
            {
                Flag.TouchFlagZone(other as Player, this);
            }
        }

        public override bool KeyValue(string keyName, string keyValue)
        {
            if (keyName.ToLower() == "teamnum")
            {
                int id = int.Parse(keyValue);
                this.Team = id;
                return true;
            }

            return base.KeyValue(keyName, keyValue);
        }

    }

    [Entity("sf_flag")]
    public class PropFlag : BaseAnimating
    {
        public float ResetTimer;

        public Entity OriginProp;

        protected override void Initialize()
        {
            Model = "models/Roller.mdl";
            MoveType = Sharp.MoveType.VPHYSICS;

            if (Sharp.Sharp.SERVER)
            {
                InitPhysics();
                Solid = SolidType.VPHYSICS;
                Physics.Motion = false;
            }

            base.Initialize();
        }

        public override void Think()
        {
            base.Think();

            if (Sharp.Sharp.SERVER)
            {
                if (ResetTimer > 0.0f && Game.CurTime > ResetTimer)
                    this.Reset();

                return;
            }

            Entity parent = this.Parent;
            if (parent != null)
            {
                int attachment = parent.LookupAttachment("eyes");
                Vector position = parent.Origin + new Vector(0, 0, 100);

                if (parent.GetAttachment(attachment, out position))
                {
                    position.z += 42.0f;
                }
                Origin = position;
            }
        }

        public override void StartTouch(Entity other)
        {
            base.StartTouch(other);

            if (other is Player)
            {
                Flag.Pickup(other as Player, this);
            }
            else if (other is FuncDisposer)
            {
                Flag.Reset(this);
            }
        }

        public void SetOriginProp(Entity ent)
        {
            OriginProp = ent;
            Team = ent.Team;
            Skin = Team == ((int)Teams.Blue) ? 0 : 2;
        }

        public void Reset()
        {
            if (OriginProp == null)
            {
                Dispose();
                Console.WriteLine("Error! unable to find origin prop of {0}", this);
                return;
            }

            Parent = null;
            Physics.Motion = false;
            ResetTimer = 0;
            Origin = OriginProp.Origin;
            Angles = OriginProp.Angles;
        }

        public void Pickup(Player player)
        {
            Parent = player;
            Origin = PlayerLocalPos(player);
            Physics.Motion = false;
        }

        public Vector PlayerLocalPos(Entity player)
        {
            return player.Origin + new Vector(0.0f, 0.0f, 100.0f);
        }

        public void Drop()
        {
            Physics.Motion = true;

            if (Parent != null)
            {
                Origin = PlayerLocalPos(Parent);
            }
            Parent = null;
        }
    }

    [Entity("func_disposer")]
    public class FuncDisposer : FuncBrush
    {
        public override void StartTouch(Entity other)
        {
            base.StartTouch(other);

            if (other is SfBlockBase)
            {
                other.Dispose();
            }
            else if (other is Player)
            {
                TakeDamageInfo info = new TakeDamageInfo();
                info.Inflictor = this;
                info.Attacker = this;
                info.Damage = other.Health + 1.0f;
                info.DamageType = 8;


                Console.WriteLine("Setting {0} health to 0", other);
                other.OnTakeDamage(info);
                //other.Health = 0;
            }
        }
    }


}
