using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Sharp;
using BaseAddon;


namespace SourceForts
{
    public abstract class SourceFortsClass
    {
        public abstract string Name { get; }
        public abstract string Description { get; }
        public abstract int MaxHealth { get; }
        public abstract int MaxArmor { get; }
        public abstract int StartArmor { get; }
        public abstract float Speed { get; }

        public readonly Player Player;

        public SourceFortsClass(Player player)
        {
            this.Player = player;
        }

        public abstract void Spawn();
        public abstract void SetAmmo();
    }

    public class ScoutClass : SourceFortsClass
    {
        public override string Name { get { return "Scout"; } }
        public override string Description { get { return "Runs fast, kills fast"; } }
        public override int MaxHealth { get { return 90; } }
        public override int MaxArmor { get { return 30; } }
        public override int StartArmor { get { return 15; } }
        public override float Speed { get { return 1.0f; } }

        public ScoutClass(Player player)
            : base(player)
        {
        }

        public override void Spawn()
        {
            Player.Give("weapon_crowbar");
            Player.Give("weapon_pistol");
            Player.Give("weapon_smg1");
            Player.Give("weapon_frag");

            Player.SelectItem("weapon_smg1");
        }

        public override void SetAmmo()
        {
            Player.GiveAmmo("smg1", 150);
            Player.GiveAmmo("Pistol", 100);
        }
    }
}
