using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Sharp;
using System.Diagnostics;

namespace BaseAddon
{
    public enum BallraceTeam : int
    {
        PLAYERS,
        DEAD,
        COMPLETE
    }

    public enum BallraceState : int
    {
        WAITING,
        PLAYING,
        INTERMISSION
    }

    public class LivesHud : ImagePanel
    {
        Label lives = new Label("3");
        int tex = 0;

        public LivesHud()
        {
            this.Image = "gmod_tower/balls/hud_main_lives";
            this.Wide = 256;
            this.Tall = 128;
            this.Position = new Position(12, 12);

            lives.Parent = this;
            lives.Font = Ballrace.BallFont;
            lives.Position = new Position(140 - 12, 69 - 12);
            lives.SizeToContents();
            lives.ForegroundColor = new Color(255, 255, 255, 255);

            tex = Surface.GetTexture("vgui/gmod_tower/balls/hud_icon_antlion");
        }

        public override void Think()
        {
            BallEntity ball = BallEntity.GetLocalBall();

            if (ball != null)
                lives.Text = ball.Lives.ToString();
        }

        public override void Paint()
        {
            base.Paint();

            Surface.SetTexture(tex);
            Surface.SetColor(new Color(255, 255, 255, 255));

            Surface.DrawTexturedRectRotated(51, 52, 128, 128, Game.CurTime);
        }

    }
    public class BananaHud : ImagePanel
    {
        Label bananas = new Label("2");
        int bananaTexture;

        int bananaCount;
        float bananaTime;
        float bananaRotation;
        float bananaSize = 0.0f;
        float curRot;

        public BananaHud()
        {
            this.Image = "gmod_tower/balls/hud_main_bananas";
            this.Wide = 256;
            this.Tall = 128;
            this.Position = new Position(Surface.ScreenWide - 268, 12);

            bananas.Parent = this;
            bananas.Font = Ballrace.BallFont;
            bananas.Position = new Position(100, 69 - 12);
            bananas.SizeToContents();
            bananas.ForegroundColor = new Color(255, 255, 255, 255);

            bananaTexture = Surface.GetTexture("vgui/gmod_tower/balls/hud_icon_banana");
        }

        public override void Think()
        {
            BallEntity ball = BallEntity.GetLocalBall();

            if (ball != null)
            {
                if (bananaCount != ball.Bananas)
                {
                    bananaTime = Game.CurTime + 1.0f;
                    bananaCount = ball.Bananas;
                    curRot = bananaRotation;
                }

                bananas.Text = ball.Bananas.ToString();

            }
        }

        public override void Paint()
        {
            base.Paint();

            if (Game.CurTime > bananaTime)
            {
                bananaRotation = MathX.Approach(bananaRotation, 0.0f, 0.09f);
                bananaSize = MathX.Approach(bananaSize, 0.0f, 0.18f);
            }
            else
            {
                bananaRotation = curRot + ((float)Math.Sin(Game.CurTime));
                bananaSize = 30.0f * ((float)Math.Sin(Game.CurTime * 5.0f)) * 2.0f;
            }

            Surface.SetTexture(bananaTexture);
            Surface.SetColor(new Color(255, 255, 255, 255));

            Surface.DrawTexturedRectRotated(209, 52, (int)(128.0f + bananaSize), (int)(128.0f + bananaSize), bananaRotation);
        }
    }
    public class TimeLeftHud : ImagePanel
    {
        Label timeLeft = new Label("0:00");
        int ClockTexture;
        float TimerSize;

        public TimeLeftHud()
        {
            this.Image = "gmod_tower/balls/hud_main_timer";
            this.Wide = 256;
            this.Tall = 128;
            this.Position = new Position(Surface.ScreenWide / 2 - this.Wide / 2, 12);

            timeLeft.Parent = this;
            timeLeft.Font = Ballrace.BallFont;
            timeLeft.ForegroundColor = new Color(255, 255, 255, 255);
            UpdateTime();

            ClockTexture = Surface.GetTexture("vgui/gmod_tower/balls/hud_icon_clock");
        }

        public void UpdateTime()
        {
            timeLeft.Text = TimeSpan.FromSeconds(Ballrace.TimeLeft).ToString(@"mm\:ss");
            timeLeft.SizeToContents();
            CenterText();
        }

        public override void Paint()
        {
            base.Paint();

            float sway = (float)Math.Sin(Game.CurTime * 2);
            float timeleft = Ballrace.TimeLeft;

            if (timeleft <= 10.0f)
                TimerSize = (float)Math.Abs(15.0 * Math.Sin(Game.CurTime * timeleft / 60.0 * 2.0));
            else
                TimerSize = MathX.Approach(TimerSize, 0.0f, 0.09f);


            Surface.SetTexture(ClockTexture);
            Surface.SetColor(new Color(255, 255, 255, 255));
            Surface.DrawTexturedRectRotated(128, 40, (int)(128.0f + TimerSize * 2), (int)(128.0f + TimerSize * 2), sway);
        }

        public override void Think()
        {
            UpdateTime();
        }

        public void CenterText()
        {
            timeLeft.CenterHorizontally();
            timeLeft.Y = 69;
        }
    }

    public class Ballrace : Gamemode
    {
        public static Font BallFont;

        public static readonly ConVar TimeEndConvar = new ConVar("ballrace_time", "500.0", "Game time for the round to end", CommandFlags.REPLICATED);
        public static readonly ConVar GameStateConvar = new ConVar("ballrace_state", "0", "Current state (waiting/playing/intermission)", CommandFlags.REPLICATED);


        LivesHud livesPanel;
        TimeLeftHud timerPanel;
        BananaHud bananasPanel;

        public override void OnStart()
        {
            if (Sharp.Sharp.CLIENT)
            {
                BallFont = new Font("Coolvetica", 48, 200, true, false, FontFlags.None);
                livesPanel = new LivesHud();
                timerPanel = new TimeLeftHud();
                bananasPanel = new BananaHud();
            }
            else
            {
                State = BallraceState.WAITING;
                TimeLeft = 5.0f;
                Console.WriteLine("Time left: {0}", TimeEndConvar.FloatValue);
            }

        }

        static Entity playerSpawn = null;
        public static int currentLevel = 1;
        public static bool NextMap = false;

        public static float TimeLeft
        {
            get { return Math.Max(0.0f, TimeEndConvar.FloatValue - Game.CurTime); }
            set { TimeEndConvar.FloatValue = Game.CurTime + value; }
        }

        public static BallraceState State
        {
            get { return (BallraceState)GameStateConvar.IntValue; }
            set
            {
                Console.WriteLine("Switch game state to: {0} ({1})", value, Game.CurTime);
                GameStateConvar.IntValue = (int)value;
            }
        }

        public override void PlayerSpawn(Player player)
        {
            Entity spawnPoint = playerSpawn;

            if (spawnPoint == null)
            {
                spawnPoint = Game.GetEntitiesByClassname("info_player_start").FirstOrDefault();
            }

            Console.WriteLine("Player spawning");

            if (spawnPoint != null)
            {
                BallEntity ball = player.GetBall();

                if (ball == null)
                {
                    ball = EntityManager.CreateEntity("player_ball") as BallEntity;

                    if (ball == null)
                    {
                        Console.WriteLine("Could not create player ball!");
                        return;
                    }

                    ball.SetRacer(player);
                    ball.Spawn();
                }

                player.ObserverMove = SpectatorMovement.None;

                ball.RenderMode = EntityRenderMode.Normal;
                ball.MoveType = MoveType.VPHYSICS;
                ball.BallraceTeam = BallraceTeam.PLAYERS;

                player.Teleport(spawnPoint.Origin, null, Vector.Zero);
                ball.Teleport(spawnPoint.Origin, null, Vector.Zero);

                //ball.Solid = SolidType.VPHYSICS;
            }
            else
            {
                Console.WriteLine("Could not find spawn for {0}!", player);
            }

        }

        public static Entity FindSpawn()
        {
            IEnumerable<Entity> dest = Game.GetEntitiesByName("lvl" + currentLevel + "_start");
            return dest.Count() > 0 ? dest.First() : null;
        }

        public override void Think()
        {
            if (Sharp.Sharp.SERVER && TimeLeft <= 0.0f)
            {
                TimerFinish();
                Debug.Assert(TimeLeft > 0.0f);
            }

        }

        public static void TimerFinish()
        {
            switch (State)
            {
                case BallraceState.WAITING:
                    StartGame();
                    break;
                case BallraceState.INTERMISSION:
                    NextRound();
                    break;
                case BallraceState.PLAYING:
                    StartIntermission();
                    break;
            }

        }

        public static void StartIntermission()
        {
            State = BallraceState.INTERMISSION;
            TimeLeft = 5.0f;
        }

        public static void StartGame()
        {
            Console.WriteLine("Starting the game!");

            NextMap = false;
            playerSpawn = null;
            currentLevel = 1;

            StartRound();
        }

        public static void StartRound()
        {
            playerSpawn = FindSpawn();
            TimeEndConvar.FloatValue = Game.CurTime + 300.0f;
            State = BallraceState.PLAYING;

            Console.WriteLine("\t StartRound/Reset spawn point: {0} ({1})", playerSpawn, currentLevel);

            foreach (Player player in Game.GetPlayers())
            {
                player.ObserverMove = SpectatorMovement.None;

                Game.Gamemode.PlayerSpawn(player);

                BallEntity ball = player.GetBall();

                if (ball != null)
                {
                    ball.Lives = 3;
                    ball.Bananas = 0;
                    ball.BallraceTeam = BallraceTeam.PLAYERS;
                }
            }
        }

        public static void NextRound()
        {
            Console.WriteLine("Advancing to next level");

            if (NextMap)
            {

            }

            currentLevel++;
            StartRound();
        }

        public static void PlayerFinish(BallEntity ball)
        {
            //Do not leave everyone waiting!
            if (TimeLeft > 15.0f)
                TimeEndConvar.FloatValue = Game.CurTime + 15.0f;

            if (ball != null)
            {
                ball.BallraceTeam = BallraceTeam.COMPLETE;
                RoamPlayer(ball);
            }

            CheckPlayingPlayers();
        }

        public static void PlayerDied(BallEntity ball)
        {
            ball.Lives--;
            ball.NetworkStateChanged();

            Console.WriteLine("Player died: {0} / {1}", ball, ball.Lives);

            if (ball.Lives > 0)
            {
                Player player = ball.GetPlayer();
                if (player != null)
                    Game.Gamemode.PlayerSpawn(player);
            }
            else
            {
                ball.BallraceTeam = BallraceTeam.DEAD;
                RoamPlayer(ball);
            }

            CheckPlayingPlayers();
        }

        public static void RoamPlayer(BallEntity ball)
        {
            Player player = ball.GetPlayer();

            if (player == null)
                return;

            player.ObserverMove = SpectatorMovement.Roaming;
            ball.RenderMode = EntityRenderMode.None;
            ball.MoveType = MoveType.NONE;
            //ball.Solid = SolidType.NONE;
        }

        public static void CheckPlayingPlayers()
        {
            if (State != BallraceState.PLAYING)
                return;

            int playing = 0;

            foreach (Player player in Game.GetPlayers())
            {
                BallEntity ball = player.GetBall();

                if (ball != null && ball.BallraceTeam == BallraceTeam.PLAYERS)
                    playing++;
            }

            if (playing == 0)
                StartIntermission();

        }

        [ConCommand("ballrace_advance", "Advance the game to the next level", CommandFlags.CHEAT | CommandFlags.GAMEDLL)]
        public static void AdvanceLevel(CCommand command)
        {
            NextRound();
        }

        [ConCommand("ballrace_forcefinish", "Advance the game to the next level", CommandFlags.CHEAT | CommandFlags.GAMEDLL)]
        public static void ForcePlayerFinish(CCommand command)
        {
            BallEntity ball = ConCommand.GetCommandClient().GetBall();

            if (ball == null)
            {
                Console.WriteLine("Player does not have the ball as the owner!");
                return;
            }

            PlayerFinish(ball);
        }
    }

    [Entity("bumper")]
    public class BumperEntity : BaseAnimating
    {
        float lastHit = 0.0f;

        public override void Spawn()
        {
            base.Spawn();
            this.Model = "models/gmod_tower/bumper.mdl";
            this.MoveType = MoveType.VPHYSICS;

            if (Sharp.Sharp.SERVER)
            {
                this.InitPhysics();
                this.Physics.Motion = false;
            }

        }

        public override void PhysicsCollision(CollisionEvent collisisonEvent)
        {
            if (Math.Abs(lastHit - Game.CurTime) < 0.25)
                return;

            Entity otherEntity = collisisonEvent.OtherData.Entity;

            if (!(otherEntity is BallEntity))
                return;

            lastHit = Game.CurTime;

            Vector normal = collisisonEvent.Normal * -1;
            Vector velocity = collisisonEvent.OtherData.PreVelocity;
            float dot = this.Up.Dot(collisisonEvent.Normal);

            if (Math.Abs(dot) > 0.1)
                return;

            Vector force = normal * otherEntity.Physics.Mass * Math.Min(400, velocity.Length) * 2.0f;
            otherEntity.Physics.ApplyForceCenter(force);
        }


    }

    /*
    [Entity("banana")]
    public class BananaEntity : BaseAnimating
    {
        private int points;

        private static readonly String NormalModel = "models/props/cs_italy/bananna.mdl";
        private static readonly String ExtraModel = "models/props/cs_italy/bananna_bunch.mdl";

        private Vector originPos;

        protected override void Initialize()
        {
            this.Solid = SolidType.BBOX;
            this.CollisionGroup = CollisionGroup.DebrisTrigger;
            this.SetCollisionBounds(new Vector(-25, -25, -25), new Vector(25, 25, 30));
            this.SolidFlags |= Sharp.SolidFlags.Trigger;

            if (Sharp.Sharp.SERVER)
                this.Model = (points == 5) ? ExtraModel : NormalModel;

            IPhysicsObject phys = this.Physics;
            if (phys != null)
                phys.Motion = false;

            //this.EmitSound("bananaeat.wav");
            //Engine.Sound.PrecacheSound("bananaeat.wav", true);
        }

        public override void Spawn()
        {
            this.originPos = this.Origin;
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

        public override void Think()
        {
            if (Sharp.Sharp.CLIENT)
            {
                this.Origin = this.originPos + new Vector(0.0f, 0.0f, 40.0f + Game.CurTime % 4.0f);
            }
        }

        public override void StartTouch(Entity other)
        {
            if (other is BallEntity && this.Enabled)
            {
                (other as BallEntity).Bananas += points;

                this.Enabled = false;


                //Timer.NewTimer(0.1f, delegate()
                //{
                UserMessage.BeginEntity(this);
                UserMessage.WriteByte(0);
                UserMessage.End();
                // });

                //this.EmitSound("BaseCombatCharacter.ItemPickup3");
                //this.EmitSound("bananaeat.wav");
                RecipientFilter filter = new RecipientFilter();
                filter.AddAllPlayers();

                //Engine.Sound.EmitSound(filter, this.Index, Sharp.AudioChannel.Auto, "bananaeat.wav", 1.0f, 75, 0, 100, this.Origin);
            }
        }

        public bool Enabled
        {
            get { return this.RenderMode != EntityRenderMode.None; }
            set { this.RenderMode = (value ? EntityRenderMode.Normal : EntityRenderMode.None); }
        }

        public override void ReceiveMessage(BitRead msgData)
        {

            Material material = RenderContext.FindMaterial("effects/yellowflare");
            ParticleEffect effect = new ParticleEffect(this.Origin, material);
            Random random = new Random();

            for (int i = 0; i < 55; i++)
            {
                Particle particle = effect.AddParticle();

                particle.Color = new Color(255, 255, 255, 255);
                particle.Velocity = Vector.Random() * 200.0f * ((float)random.NextDouble());
                if (particle.Velocity.z < 0) particle.Velocity.z *= -1.0f;
                particle.DieTime = 2.5f;

                particle.StartSize = 25.0f;
                particle.EndSize = 6.0f;

                particle.StartAlpha = 255.0f;
                particle.EndAlpha = 0.0f;

            }

            //this.EmitSound("bananaeat.wav");
        }
    }
    */

    [Entity("player_ball")]
    public class BallEntity : BaseAnimating, IPlayerView
    {
        [Networked]
        public int Lives = 0;

        [Networked]
        public int Bananas = 0;

        [Networked]
        public BallraceTeam BallraceTeam = BallraceTeam.DEAD;

        [Networked]
        public Vector BallVelocity;

        public BaseAnimating ClientModel;
        public int IdleSeq;
        public int WalkSeq;
        public int RunSeq;

        public override void Spawn()
        {
            base.Spawn();

            this.BallraceTeam = BallraceTeam.DEAD;
            this.Model = "models/gmod_tower/ball.mdl";
            this.MoveType = MoveType.VPHYSICS;

            PrecacheModel("models/humans/group03/male_01.mdl");

            if (Sharp.Sharp.SERVER)
            {
                this.InitSpherePhysics(44.0f);
                this.Physics.Mass = 100.0f;

                this.CreateMotionController();
            }
            else
            {
                ClientModel = ClientBaseAnimating.CreateClientEntity("models/humans/group03/male_01.mdl", true);

                if (ClientModel != null)
                {
                    ClientModel.ResetSequence(0);
                    IdleSeq = ClientModel.LookupSequence("idle_all_01");
                    WalkSeq = ClientModel.LookupSequence("walk_all");
                    RunSeq = ClientModel.LookupSequence("run_all_01");

                    //clientModel.ClearPoseParameters();
                    ClientModel.ResetSequenceInfo();

                    ClientModel.ResetSequence(IdleSeq);
                    ClientModel.Cycle = 0.0f;
                    ClientModel.SetBodygroup(0, 1);

                    Console.WriteLine("Sequences: {0} {1} {2}", IdleSeq, WalkSeq, RunSeq);
                }

            }

            this.NextThink = Game.CurTime + 0.1f;
        }

        public void SetRacer(Player player)
        {
            player.RemoveAllItems(true);
            player.Owner = this;
            player.Solid = SolidType.NONE;
            player.MoveType = MoveType.NOCLIP;

            this.Owner = player;
        }

        public Player GetPlayer()
        {
            return Owner as Player;
        }

        float BodyAngle;

        public void UpdateSequence(Player player)
        {
            float velocity = this.BallVelocity.Length;
            Vector veln = this.BallVelocity.Normal;
            //float velAng = this.Velocity.a

            int seq = this.IdleSeq;
            QAngle aim = player.EyeAngles;
            float rate = 1;

            if (velocity > 200.0f)
            {
                seq = this.RunSeq;
                rate = velocity / 300.0f;
            }
            else if (velocity > 10.0f)
            {
                seq = this.WalkSeq;
                rate = velocity / 100.0f;
            }

            rate = MathX.Clamp(rate, 0.1f, 2.0f);

            if (ClientModel.Sequence != seq)
            {
                ClientModel.PlaybackRate = 1.0f;
                ClientModel.ResetSequence(seq);
                ClientModel.Cycle = 0;
            }

            if (seq != IdleSeq)
            {
                BodyAngle = aim.y;
            }
            else
            {
                float diff = MathX.NormalizeAngle(aim.y - BodyAngle);
                if (Math.Abs(diff) > 45.0f)
                {
                    float norm = MathX.Clamp(diff, -1, 1);
                    BodyAngle = MathX.NormalizeAngle(BodyAngle + (diff - 45 * norm));
                }
            }

            ClientModel.Angles = new QAngle(0, BodyAngle, 0);

            ClientModel.SetPoseParameter("head_pitch", MathX.Clamp(aim.x - 40, -19, 20));
            ClientModel.SetPoseParameter("head_yaw", MathX.NormalizeAngle(aim.y - BodyAngle));
            //clientModel.SetPoseParameter("move_yaw", MoveYaw);

            ClientModel.SetPoseParameter("breathing", 0.4f);

            float dot = veln.Dot(aim.Forward);
            float dotr = veln.Dot(aim.Right);
            float spd = MathX.Clamp(velocity / 100.0f, 0.0f, 1.0f);

            ClientModel.SetPoseParameter("body_pitch", -aim.x);

            ClientModel.SetPoseParameter("move_x", spd * dot);
            ClientModel.SetPoseParameter("move_y", spd * dotr);

            ClientModel.FrameAdvance(Game.FrameTime * rate);
        }


        public override void Think()
        {
            if (Sharp.Sharp.SERVER)
            {
                IPhysicsObject phys = this.Physics;

                if (phys != null)
                {
                    phys.Wake();
                    //So, apparently the velocity is not networked by the engine at all
                    //Where does the client prediction occour?
                    BallVelocity = phys.Velocity;
                }


            }

            this.NextThink = Game.CurTime + 0.01f;

            if (Sharp.Sharp.CLIENT && ClientModel != null)
            {
                ClientModel.Origin = this.Origin - new Vector(0, 0, 40);
                Player player = this.GetPlayer();

                if (player != null)
                    UpdateSequence(player);
            }
        }

        public override void PhysicsCollision(CollisionEvent collisionEvent)
        {
            base.PhysicsCollision(collisionEvent);

            if (collisionEvent.Speed.Length >= 100.0f && collisionEvent.DeltaTime >= 1.0f)
            {
                UserMessage.BeginEntity(this, true);
                UserMessage.WriteChar((char)1);
                UserMessage.WriteVector(collisionEvent.Point);
                UserMessage.WriteVector(collisionEvent.Normal * -1);
                UserMessage.End();
            }
        }

        public override void ReceiveMessage(BitRead msgData)
        {
            char id = msgData.ReadChar();

            if (id == 1)
            {
                Vector origin = msgData.ReadVector();
                Vector normal = msgData.ReadVector();
                SpawnCollideEffect(origin, normal);
            }
        }

        private void SpawnCollideEffect(Vector origin, Vector normal)
        {
            Material material = RenderContext.FindMaterial("sprites/star");
            ParticleEffect effect = new ParticleEffect(origin, material);
            Random random = new Random();
            QAngle angle = normal.ToAngle();

            for (int i = 0; i < 8; i++)
            {
                Particle particle = effect.AddParticle();
                Vector rand = Vector.Random() * 200.0f;

                particle.Color = random.Next(0, 1) == 0 ?
                    new Color(255, 0, 0, 255) :
                    new Color(255, 255, 0, 255);

                particle.Velocity = angle.Forward * Math.Abs(rand.x) + angle.Right * rand.y + angle.Up * rand.z;
                if (particle.Velocity.z < 0) particle.Velocity.z *= -1.0f;
                particle.DieTime = 2.5f;

                particle.StartSize = 10.0f;
                particle.EndSize = 5.0f;

                particle.StartAlpha = 255.0f;
                particle.EndAlpha = 0.0f;

                particle.Roll = (float)random.NextDouble() * 50.0f;
                particle.RollDelta = (float)random.NextDouble() * 4.0f - 2.0f;

            }
        }

        public override MotionEvent Simulate(ref Vector linear, ref Vector angularImpulse, float dealtaTime, IPhysicsObject pObject)
        {
            IPhysicsObject phys = this.Physics;
            Player player = this.Owner as Player;

            if (player == null)
                return MotionEvent.NOTHING;

            Vector velocity = phys.Velocity;
            PlayerButtons buttons = player.Buttons;
            QAngle aim = player.EyeAngles;

            linear = new Vector();

            if (buttons.HasFlag(PlayerButtons.FORWARD)) linear += aim.Forward;
            if (buttons.HasFlag(PlayerButtons.BACK)) linear -= aim.Forward;
            if (buttons.HasFlag(PlayerButtons.MOVELEFT)) linear -= aim.Right;
            if (buttons.HasFlag(PlayerButtons.MOVERIGHT)) linear += aim.Right;

            linear.z = 0;

            if (linear.Length > 0)
            {
                Vector dir = linear.Normal;
                Vector dot = 1 - dir.Dot(velocity.Normal);
                linear = dir * phys.Mass * 40000 * Game.FrameTime;
                linear += linear * dot;
            }
            else if (velocity.z <= 10)
            {
                Vector length = velocity.Length;

                linear = velocity.Normal * length * -200.0f * (1.0f - Game.FrameTime);
            }

            return MotionEvent.GLOBAL_FORCE;
        }

        public void CalcPlayerView(Player player, ref Vector eyeOrigin, ref QAngle eyeAngles, ref float fov)
        {
            Vector start = this.Origin;
            Vector end = start + eyeAngles.Forward * -200.0f;

            TraceResponse response = EngineTrace.TraceRay(start, end, Mask.OPAQUE, CollisionGroup.None, null);

            eyeOrigin = start + eyeAngles.Forward * -200.0f * response.Fraction * 0.95f;
        }

        public static BallEntity GetLocalBall()
        {
            Player player = Player.GetLocalPlayer();
            return player == null ? null : player.Owner as BallEntity;
        }
    }

    [Entity("trigger_fallout")]
    public class EntityTriggerFallout : FuncBrush
    {
        public override void StartTouch(Entity other)
        {
            if (other is BallEntity)
            {
                Ballrace.PlayerDied(other as BallEntity);
            }
            Console.WriteLine("Start Touch on trigger fallout {0}", other);
        }

        public override bool KeyValue(string keyName, string keyValue)
        {
            Console.WriteLine("Trigger fallout got: {0} = {1}", keyName, keyValue);
            return true;
        }

    }

    [Entity("trigger_nextlevel")]
    public class EntityTriggerNextlevel : FuncBrush
    {
        string Destination;
        string BonusTarget;
        string ParentName;
        string Relay;

        public override void StartTouch(Entity other)
        {
            if (other is BallEntity)
            {
                Ballrace.PlayerFinish(other as BallEntity);
                //Player ply = other.Owner as Player;

                //Timer.NewTimer(0.25f, () => { Console.WriteLine("{0} has won the game!", ply); });
            }
        }

        public override bool KeyValue(string keyName, string keyValue)
        {
            Console.WriteLine("Trigger next level got: {0} = {1}", keyName, keyValue);

            if (keyName == "target")
                this.Destination = keyValue;
            else if (keyName == "bonustarget")
                this.BonusTarget = keyValue;
            else if (keyName == "parentname")
                this.ParentName = keyValue;
            else if (keyName == "relay")
                this.Relay = keyValue;
            else
                return false;

            return true;
        }

    }

    [Entity("trigger_endgame")]
    public class EntityTriggerEndGame : FuncBrush
    {
        string ParentName;

        public override void Spawn()
        {
            base.Spawn();

            if (ParentName != null)
            {
                Entity parent = Game.GetEntitiesByName(ParentName).FirstOrDefault();

                if (parent != null)
                {
                    this.Parent = parent;
                }
            }
        }

        public override void StartTouch(Entity other)
        {
            Ballrace.NextMap = true;
            if (other is BallEntity)
            {
                Ballrace.PlayerFinish(other as BallEntity);
            }
        }

        public override bool KeyValue(string keyName, string keyValue)
        {
            if (keyName == "parentname")
                this.ParentName = keyValue;
            else
                return false;

            return true;
        }

    }

    public static class BallracePlayerExtension
    {
        public static BallEntity GetBall(this Player player)
        {
            return player.Owner as BallEntity;
        }
    }
}
