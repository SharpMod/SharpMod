using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Sharp;
using BaseAddon;

namespace SourceForts
{
    public enum GameState
    {
        BUILD,
        FIGHT,
        SCOREBOARD,
        WAITING_PLAYERS
    }

    public enum Teams
    {
        Unassigned = 0,
        Spectator = 1,
        Blue = 2,
        Red = 3
    }

    public delegate void GameStateChanged(GameState NewState);

    public class WeldingBlockStatus
    {
        public SfBlockBase Block;
        public float StartTime;
        public bool StartState;
        public float NextSpark;
    }

    public class RepairBlockStatus
    {
    }

    public class SourceFortsMod : Gamemode
    {
        public static readonly ConVar sf_class = new ConVar("sf_class", "Scout", "Default class when joining a game", CommandFlags.USERINFO | CommandFlags.ARCHIVE);
        public static readonly ConVar GameStateVar = new ConVar("sf_gamestate", "0", "", CommandFlags.REPLICATED);
        public static readonly ConVar GameStartTime = new ConVar("sf_roundstart", "0", "", CommandFlags.REPLICATED);
        public static readonly ConVar GameRound = new ConVar("sf_round", "1", "", CommandFlags.REPLICATED);
        public static readonly ConVar GameMaxRound = new ConVar("sf_max_round", "3", "", CommandFlags.REPLICATED);
        public static readonly ConVar BuildPhaseLength = new ConVar("sf_buildlength", "300", "", CommandFlags.REPLICATED);
        public static readonly ConVar FightPhaseLength = new ConVar("sf_fightlength", "450", "", CommandFlags.REPLICATED);

        public GameState State
        {
            get { return (GameState)GameStateVar.IntValue; }
            set
            {
                GameStateVar.IntValue = (int)value;
                foreach (GamePhaseControl control in Game.GetEntities().OfType<GamePhaseControl>())
                {
                    control.FireGameStateChange(State);
                }
            }
        }

        public int Round
        {
            get { return GameRound.IntValue; }
        }

        public int MaxRound
        {
            get { return GameMaxRound.IntValue; }
        }

        public int GameLength
        {
            get
            {
                if (State == GameState.BUILD)
                    return BuildPhaseLength.IntValue;
                if (State == GameState.SCOREBOARD)
                    return 5;
                return FightPhaseLength.IntValue;
            }
        }

        public float StartTime
        {
            get { return GameStartTime.FloatValue; }
        }

        public float TimeLeft
        {
            get { return Math.Max(0, StartTime + GameLength - Game.CurTime); }
        }

        [ConCommand("sf_next", "Brings the game to the next stage", CommandFlags.GAMEDLL)]
        public static void SfNext(CCommand command)
        {
            Base.SourceForts.StartNext();
        }

        BaseTopInfoPanel TimeLeftPanel;

        public SourceFortsMod()
        {
            //Reset the values
            GameStartTime.FloatValue = Game.CurTime;

            if (Sharp.Sharp.CLIENT)
            {
                TimeLeftPanel = new BaseTopInfoPanel(() => { return TimeSpan.FromSeconds(TimeLeft).ToString(@"mm\:ss"); });
            }
            else
            {
                // StartBuild();
                State = GameState.WAITING_PLAYERS;
            }
        }


        public override void PlayerSpawn(Player player)
        {
            if (Sharp.Sharp.CLIENT)
            {
                Console.WriteLine("Client player initial team: {0}", player.Team);

                if (player.Team == (int)Teams.Unassigned)
                {
                    var teamgui = new TeamSelectGUI();
                    teamgui.MakePopup();
                }
                return;
            }

            if (State == GameState.WAITING_PLAYERS)
                StartBuild();

            if (player.Team == (int)Teams.Unassigned)
            {
                Console.WriteLine("Player spawning with no team. {0}", player.Solid);
                player.ObserverMove = SpectatorMovement.Roaming;
                player.Solid = SolidType.NONE;
                player.MoveType = MoveType.NOCLIP;
                return;
            }

            player.Solid = SolidType.BBOX;
            Console.WriteLine("Player team: {0}", player.Team);

            List<BaseAnimating> spawnPoints = Game.GetEntitiesByClassname("info_player_team_spawn")
                .OfType<BaseAnimating>()
                .Where(t => t.Skin == player.Team) //Game uses skin id as team...?
                .ToList();

            if (spawnPoints.Count == 0)
            {
                Console.WriteLine("Could not find spawn for player...?");
                return;
            }

            Random rnd = new Random();
            Entity spawnPoint = spawnPoints[rnd.Next(spawnPoints.Count)];

            if (spawnPoint == null)
                return;

            player.Teleport(spawnPoint.Origin, null, Vector.Zero);
            player.Origin = spawnPoint.Origin + new Vector(0.0f, 0.0f, 1.0f);

            player.Give("weapon_physcannon");

            if (State == GameState.BUILD)
            {
            }
            else
            {
                SourceFortsClass plyClass = player.GetClass();
                plyClass.Spawn();
                plyClass.SetAmmo();

                player.Health = plyClass.MaxHealth;
                player.Armor = plyClass.StartArmor;

                //TODO: Set Model
            }
        }

        public override bool PhysCannonCanPickupObject(Player player, Entity other, float maxMass)
        {
            if (other is SfBlockBase)
                return other.Physics.Motion;

            return false;
        }

        public override void PhysCannonPickup(Player player, Entity entity, PhysGunPickup_t reason)
        {
            base.PhysCannonPickup(player, entity, reason);

            var block = entity as SfBlockBase;
            if (block != null)
            {
                block.UpdateSkin(player);
            }
        }

        public override void Think()
        {
            if (Sharp.Sharp.CLIENT)
                return;

            if (TimeLeft <= 0.0f)
                StartNext();

            if (State == GameState.BUILD)
                BuildCheckKeys();
            else if (State == GameState.FIGHT)
                FightCheckKeys();
        }

        private void BuildCheckKeys()
        {
            foreach (Player player in Game.GetPlayers())
            {
                if (player.ButtonsReleased.HasFlag(PlayerButtons.RELOAD))
                {
                    PhysCannon cannon = player.GetActiveWeapon() as PhysCannon;

                    if (cannon == null)
                        continue;

                    if (cannon.Attached != null)
                    {
                        var attached = cannon.Attached as SfBlockBase;
                        if (attached != null)
                            attached.CheckMotion(player, false);
                    }
                    else
                    {
                        TraceResponse response = player.GetEyeTrace();
                        var hitEntity = response.HitEntity as SfBlockBase;

                        if (hitEntity != null)
                            hitEntity.CheckMotion(player, !hitEntity.AllowMotion);
                    }

                }
            }
        }

        private void FightCheckKeys()
        {
            foreach (Player player in Game.GetPlayers())
            {
                if (player.ButtonsPressed.HasFlag(PlayerButtons.RELOAD))
                    StartWeld(player);

                var status = player.GetWeldStatus();
                if (status != null)
                {
                    if (Block.WeldThink(player, status) == false)
                        Block.StopWeld(player);

                    return;
                }

                if (player.Buttons.HasFlag(PlayerButtons.USE) && player.GetActiveWeapon() is PhysCannon)
                {
                    Block.RepairTick(player);
                }
                else
                {
                    Block.StopRepair(player);
                }

            }
        }

        private void StartWeld(Player player)
        {
            PhysCannon cannon = player.GetActiveWeapon() as PhysCannon;
            if (cannon == null)
                return;

            TraceResponse response = player.GetEyeTrace();
            var hitEntity = response.HitEntity as SfBlockBase;

            if (hitEntity == null || !Block.InRange(player, response.EndPos))
                return;

            var block = response.HitEntity as SfBlockBase;
            if (block == null)
                return;

            var status = new WeldingBlockStatus();
            status.Block = block;
            status.StartState = block.AllowMotion;
            status.StartTime = Game.CurTime;
            status.NextSpark = 0.0f;

            player.Properties["_WeldingBlockStatus"] = status;
        }



        public void StartNext()
        {
            if (State == GameState.FIGHT)
            {
                StartBuild();
            }
            else if (State == GameState.BUILD)
            {
                int nextRound = Round + 1;
                if (nextRound > MaxRound)
                {
                    //Start scoreboard
                    //return;
                }
                StartFight();
                GameRound.IntValue = nextRound;
            }
            else if (State == GameState.SCOREBOARD)
            {
                //TODO: Switch to next map
            }



            Console.WriteLine("Game state is: {0}", State);
        }

        public void StartBuild()
        {
            Console.WriteLine("Start Build phase!");
            State = GameState.BUILD;
            RestartTimer();
            RespawnPlayers();
            CleanupGame();

            Flag.RemoveAll();
        }

        public void StartFight()
        {
            Console.WriteLine("Start Fight phase!");
            State = GameState.FIGHT;
            RestartTimer();
            RespawnPlayers();
            CleanupGame();

            Flag.SpawnAll();
        }

        public void CleanupGame()
        {
            foreach (Player player in Game.GetPlayers())
            {
                Block.StopWeld(player);
                Block.StopRepair(player);
            }

            Block.ResetAllHealth();
        }

        public void RestartTimer()
        {
            GameStartTime.FloatValue = Game.CurTime;
        }

        public void RespawnPlayers()
        {
            foreach (Player player in Game.GetPlayers())
            {
                player.RemoveAllWeapons();
                player.Spawn();
            }
        }

        [ConCommand("sf_forcenext", "Force the game to change to the next stage", CommandFlags.GAMEDLL)]
        public static void StartNextRound(CCommand command)
        {
            Base.SourceForts.StartNext();
        }
    }

    public static class SourceFortsUtils
    {
        public static SourceFortsClass GetClass(this Player player)
        {
            String plyClass = player.GetConVar("sf_class");

            switch (plyClass)
            {
                case "Scout": return new ScoutClass(player);
            }

            return new ScoutClass(player);
        }

    }

    public class BaseTopInfoPanel : Panel
    {
        public delegate String GetTextDelegate();

        public int Offset;
        public float NextThink = 0.0f;

        public static readonly Font CenterFont = new Font("CenterPrintText", 28, 125, true, false, FontFlags.None);
        public Label Label = new Label("0");

        public GetTextDelegate GetText;

        public BaseTopInfoPanel(GetTextDelegate del)
        {
            this.Label.Parent = this;
            this.GetText = del;
        }

        public override void Think()
        {
            base.Think();
            if (Game.CurTime > NextThink)
            {
                this.PerformLayout();
                NextThink = Game.CurTime + 1.0f;
            }
        }

        public Color GetColor()
        {
            return new Color(255, 255, 255, 255);
        }

        public override void PerformLayout()
        {
            base.PerformLayout();
            String text = GetText();

            Label.Text = GetText();
            Label.SizeToContents();
            Label.ForegroundColor = GetColor();

            Wide = (int)Math.Max(32.0f, ((float)Label.Wide) * 1.2f); ;
            Tall = Label.Tall + 15;
            X = (int)(Surface.ScreenWide * 0.5 + Offset);
        }

    }



    public static class Block
    {
        public static float MaxHealth
        {
            get { return 6000.0f; }
        }

        public static bool RemoveProhibited
        {
            get { return true; }
        }

        public static int MaxLimit
        {
            get { return 90; }
        }

        public static float TreshHold { get { return 0.05f * MaxHealth; } }
        public static float WeldMin { get { return 10.0f; } }
        public static float WeldMax { get { return 40.0f; } }
        public static float UnweldMin { get { return 10.0f; } }
        public static float UnweldMax { get { return 30.0f; } }
        public static float UnweldEnemyMultiplier { get { return 2.0f; } }
        public static float RepairTime { get { return 200.0f; } }

        public static float RepairLife
        {
            get
            {
                float time = Math.Max(RepairTime, 0.0f);
                return MaxHealth / time * Game.FrameTime;
            }
        }

        public static int BlockCount(int team)
        {
            return GetBlocks().Where((x) => x.Team == team).Count();
        }

        public static IEnumerable<SfBlockBase> GetBlocks()
        {
            return Game.GetEntities().OfType<SfBlockBase>();
        }

        public static void ResetAllHealth()
        {
            foreach (SfBlockBase block in GetBlocks())
            {
                block.ResetLife();
                bool removeBad = RemoveProhibited;

                if (Base.SourceForts.State == GameState.FIGHT)
                {
                    if (CanFreeze(block, null))
                    {
                        block.SetAllowMotion(false);
                    }
                    else if (removeBad)
                    {
                        //block.Dispose();
                    }
                }
            }

        }

        public static bool CanFreeze(SfBlockBase block, Player player)
        {
            return block.AllowFreeze();
        }

        public static bool AllowSpawn(int team)
        {
            int max = MaxLimit;

            if (max <= 0)
                return true;

            return BlockCount(team) < max;
        }

        public static bool InRange(Player player, Vector position)
        {
            return player.EyePosition.Distance(position) <= 150.0f;
        }

        public static float CalculateWeldTime(SfBlockBase block, float min, float max)
        {
            float treshhold = TreshHold;
            float life = block.Life;

            float perc = (life - treshhold) / (Block.MaxHealth - treshhold);
            return perc * (max - min) + min;
        }

        public static float WeldTime(SfBlockBase block)
        {
            return CalculateWeldTime(block, WeldMin, WeldMax);
        }

        public static float UnweldTime(SfBlockBase block)
        {
            return CalculateWeldTime(block, UnweldMin, UnweldMax);
        }

        public static float GetWeldTime(Player player, SfBlockBase block, bool motion)
        {
            if (motion)
                return WeldTime(block);

            float unweld = UnweldTime(block);

            if (player.Team != block.Team)
                unweld *= UnweldEnemyMultiplier;

            return unweld;
        }

        public static SfBlockBase AimOnBlock(Player player)
        {
            var trace = player.GetEyeTrace();

            return trace.HitEntity as SfBlockBase;
        }

        public static bool WeldThink(Player player, WeldingBlockStatus status)
        {
            //var status = player.Properties["_WeldingBlockStatus"] as WeldingBlockStatus;

            if (!status.Block.IsValid || player.LifeState != LifeState.Alive || !player.Buttons.HasFlag(PlayerButtons.RELOAD) || status.StartState != status.Block.AllowMotion)
                return false;

            TraceResponse response = player.GetEyeTrace();

            if (response.HitEntity != status.Block || !Block.InRange(player, response.EndPos))
                return false;

            float time = Game.CurTime - status.StartTime;

            if (time > GetWeldTime(player, status.Block, status.StartState))
            {
                status.Block.SetAllowMotion(!status.StartState);
                return false;
            }

            if (Game.CurTime > status.NextSpark)
            {
                ParticleUtil.Sparks(response.EndPos, 2, 1, response.Plane.normal * 2);

                status.NextSpark = Game.CurTime + 0.2f;
            }

            return true;

        }

        public static void StopWeld(Player player)
        {
            Console.WriteLine("Stop weld");
            player.Properties["_WeldingBlockStatus"] = null;
        }

        internal static void RepairTick(Player player)
        {
            var block = AimOnBlock(player);

            if (block != null && block.Team == player.Team)
            {
                float newHealth = Math.Min(block.Health + RepairLife, MaxHealth);
                block.Health = (int)newHealth;

                RepairBlockStatus weld = player.GetRepairStatus();

                if (weld == null)
                    player.Properties["_RepairBlockStatus"] = new RepairBlockStatus();
            }
        }

        internal static void StopRepair(Player player)
        {
            player.Properties["_WeldingBlockStatus"] = null;
        }

        public static WeldingBlockStatus GetWeldStatus(this Player player)
        {
            object weld;
            player.Properties.TryGetValue("_WeldingBlockStatus", out weld);

            return weld as WeldingBlockStatus;
        }

        public static RepairBlockStatus GetRepairStatus(this Player player)
        {
            object repair;
            player.Properties.TryGetValue("_RepairBlockStatus", out repair);

            return repair as RepairBlockStatus;
        }
    }



    public static class Flag
    {

        public static void SpawnAll()
        {
            foreach (Entity entity in Game.GetEntitiesByClassname("prop_flag"))
            {
                PropFlag flag = (PropFlag)EntityManager.CreateEntity("sf_flag");
                flag.SetOriginProp(entity);
                flag.Reset();
                flag.Spawn();
            }
        }

        public static void RemoveAll()
        {
            foreach (Entity entity in Game.GetEntitiesByClassname("sf_flag"))
                entity.Dispose();

            foreach (Player player in Game.GetPlayers())
                player.Properties["_SfFlag"] = null;
        }

        public static void Pickup(Player player, PropFlag flag)
        {
            if (player.Team == flag.Team)
            {
                if (flag.Parent == null && flag.ResetTimer > 0.0f)
                    Reset(flag);

                return;
            }

            if (player.GetFlag() != null)
                return; //Player already has a flag

            Player parent = flag.Parent as Player;

            if (parent != null)
                return; //Flag is already picked up

            Set(player, flag);
        }

        public static void Set(Player player, PropFlag flag)
        {
            player.Properties["_SfFlag"] = flag;

            flag.Pickup(player);
            flag.ResetTimer = 0.0f;
        }

        public static void Drop(Player player)
        {
            PropFlag flag = player.GetFlag();

            if (flag == null)
                return;

            player.Properties["_SfFlag"] = null;
            flag.Drop();
            flag.ResetTimer = 5 * 60;
        }

        public static void Reset(PropFlag flag)
        {
            Player parent = flag.Parent as Player;

            if (parent != null)
            {
                parent.Properties["_SfFlag"] = null;
            }

            Console.WriteLine("\tRESETING FLAG {0}", flag);

            flag.Reset();
        }

        public static PropFlag GetFlag(this Player player)
        {
            object flag;
            player.Properties.TryGetValue("_SfFlag", out flag);

            return flag as PropFlag;
        }

        public static void TouchFlagZone(Player player, EntityFlagZone zone)
        {
            PropFlag flag = player.GetFlag();

            if (flag == null)
                return;

            Console.WriteLine("{0} ({1}) touched {2} ({3}) with {4} ({5})", player, player.Team, zone, zone.Team, flag, flag.Team);

            if (player.Team != zone.Team)
                return;

            //TODO: Give points to team
            Reset(flag);
        }
    }

}
