using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Sharp;

namespace BaseAddon
{

    public static class Base
    {
        public static Ballrace ballrace;
        public static SourceForts.SourceForts SourceForts;

        static void Main(string[] args)
        {
            /*
            UserMessageHook.AddHook(0, "lol", ReadMessage );
            UserMessageHook.AddHook(1, "lol2", ReadMessage );
             */
            SourceForts = new SourceForts.SourceForts();
            Game.Gamemode = SourceForts;
           
        }

        public static void ReadMessage(BitRead message)
        {
            Console.WriteLine("Got message! {0}", message.GetNumBytesLeft());
            Console.WriteLine("\t Reading bool:\t {0}", message.ReadBool());
            Console.WriteLine("\t Reading byte:\t {0}", message.ReadByte());
            Console.WriteLine("\t Reading byte:\t {0}", message.ReadChar());
            Console.WriteLine("\t Reading short:\t {0}", message.ReadShort());
            Console.WriteLine("\t Reading long:\t {0}", message.ReadLong());
            Console.WriteLine("\t Reading float:\t {0}", message.ReadFloat()); 
            Console.WriteLine("\t Reading string:\t {0}", message.ReadString());
            Console.WriteLine("\t Reading vector:\t {0}", message.ReadVector());
            Console.WriteLine("\t Reading angle:\t {0}", message.ReadAngles()); 
        }

        [ConCommand("nican_readfile", "Attempt to read a file, and hopefully throw an exception!", CommandFlags.CLIENTDLL)]
        public static void NicanReadFile(CCommand command)
        {
            string[] lines = File.ReadAllLines("C:/LJCP1215.log");
            Console.WriteLine("Read a total of: {0} lines", lines.Length );
        }

        [ConCommand("testprint", "Print the first C# message!", CommandFlags.CLIENTDLL )]
        public static void TestPrint(CCommand command)
        {
            Console.WriteLine("WHAT?! {0} It works! :D ", command.command );
        }

        [ConCommand("sharp_testnull", "Test null pointer exceptions!", CommandFlags.GAMEDLL)]
        public static void TestNullPrint(CCommand command)
        {
            String test = null;
            Console.WriteLine("Testing null: {0} ", test.Length);
        }

        [ConCommand("testnican", "Spawn a bunch of particles!", CommandFlags.CLIENTDLL)]
        public static void SpawnParticles(CCommand command)
        {
            Material material = RenderContext.FindMaterial("effects/yellowflare");
            ParticleEffect effect = new ParticleEffect(new Vector(0, 0, 8), material);

            for (int i = 0; i < 200; i++)
            {
                Particle particle = effect.AddParticle();

                particle.Color = new Color(255, 255, 255, 255);
                particle.Velocity = Vector.Random() * 200.0f;
                particle.DieTime = 1.5f;
                particle.StartSize = 150.0f;
                particle.EndSize = 0.0f;
                particle.StartAlpha = 255;
                particle.EndAlpha = 100;

            }
        }

        [ConCommand("sharp_msg", "Send a lol user message to the client!", CommandFlags.CHEAT | CommandFlags.GAMEDLL)]
        public static void SendUserMessage(CCommand command)
        {
            Player ply = Game.GetEntities().First(ent => ent is Player) as Player;
            RecipientFilter rp = new RecipientFilter(ply);

            UserMessage.Begin(rp, 0);
            UserMessage.WriteBool(true);
            UserMessage.WriteByte(55);
            UserMessage.WriteChar('c');
            UserMessage.WriteShort(555);
            UserMessage.WriteLong(12341234);
            UserMessage.WriteFloat(1234.1234f);
            UserMessage.WriteString("Meow :3");
            UserMessage.WriteVector(new Vector(31.4f, 15.92f, 65.35f));
            UserMessage.WriteAngles(new QAngle(27.18f, 28.18f, 28.46f));

            Console.WriteLine("Message max size: {0}", UserMessage.GetNumBytesLeft());

            UserMessage.End();
        }

        [ConCommand("ent_create", "Create entity", CommandFlags.CHEAT | CommandFlags.GAMEDLL)]
        public static void CreateEntity(CCommand command)
        {
            Entity entity = EntityManager.CreateEntity(command[1]);
            Player player = ConCommand.GetCommandClient();

            if (entity == null)
                return;

            if (entity is BallEntity)
            {
                (entity as BallEntity).SetRacer(player);
            }

            Console.WriteLine("Created: {0}", entity);
            //TODO: Look into int DispatchSpawn( CBaseEntity *pEntity, bool bRunVScripts )
            entity.Spawn();

            TraceResponse response = EngineTrace.TraceRay(
                player.EyePosition,
                player.EyePosition + player.EyeAngles.Forward * EngineTrace.MaxTraceLength,
                Mask.SOLID,
                CollisionGroup.None,
                player);

            if (response.Fraction < 1.0)
            {
                entity.Teleport(response.EndPos - new Vector(0, 0, entity.WorldAlignMins.z - 8), null, null);
                //UTIL_DropToFloor( entity, MASK_SOLID );
            }
        }

    }

}
