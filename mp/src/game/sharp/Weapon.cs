using System;
using System.Runtime.CompilerServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Sharp
{
    public class PhysCannon : BaseAnimating
    {


        public void OnPickup(Entity pEntity, Player pOwner, PhysGunPickup_t reason)
        {
            if (Game.Gamemode != null)
                Game.Gamemode.PhysCannonPickup(pOwner, pEntity, reason);
        }

        public void OnDrop(Entity pEntity, Player pOwner, PhysGunDrop_t reason)
        {
            if (Game.Gamemode != null)
                Game.Gamemode.PhysCannonDrop(pOwner, pEntity, reason);
        }

        public bool CanPickup(Player player, Entity other, float maxMass)
        {
            if (Game.Gamemode != null)
                return Game.Gamemode.PhysCannonCanPickupObject(player, other, maxMass);

            return player.CanPickupObject(other, maxMass, 0.0f);
        }

        public void ForceDrop()
        {
            Entity._entity.ForceDrop(this);
        }

        public Entity Attached
        {
            get { return Entity._entity.GetAttached(this); }
        }

    }
}
