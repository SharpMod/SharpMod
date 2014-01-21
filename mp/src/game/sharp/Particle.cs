using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Sharp
{
    [StructLayout(LayoutKind.Sequential)]
    public class ParticleEffect : DefaultClientRenderable, IThinkable
    {
        protected List<Particle> particles = new List<Particle>();
        protected Material Material { get; set; }

        public ParticleEffect(Vector origin, Material material)
        {
            this.Material = material;
            this.Origin = origin;

            Game.AddThinkable(this);
        }

        public Particle AddParticle()
        {
            Particle particle = new Particle();
            particle.DieTime = 1.0f;
            particle.Position = this.Origin;

            particles.Add(particle);

            return particle;
        }

        private bool ThinkParticle(Particle particle)
        {
            particle.LifeTime += Game.FrameTime;

            if (particle.LifeTime > particle.DieTime)
                return true;

            particle.Position += particle.Velocity * Game.FrameTime;
            particle.Roll += particle.RollDelta * Game.FrameTime;

            return false;
        }

        public bool Think()
        {
            particles.RemoveAll((particle) => ThinkParticle(particle));

            return particles.Count == 0;
        }

        /// <summary>
        /// Sets the render matrix to the origin of the map, and then renders the particles. This helps the particles be drawn always facing the player
        /// </summary>
        /// <returns></returns>
        public override int DrawModel()
        {
            VMatrix tempModel = new VMatrix(), tempView = new VMatrix();

            //Store the old matrices before rendering the particles
            RenderContext.GetMatrix(MaterialMatrixMode.Model, ref tempModel);
            RenderContext.GetMatrix(MaterialMatrixMode.View, ref tempView);

            //Binds the material to render
            RenderContext.Bind(this.Material);

            //Force the user clip planes to use the old view matrix
            RenderContext.EnableUserClipTransformOverride(true);
            RenderContext.UserClipTransform(tempView);

            //Load the identity to render at Vector(0,0,0)
            RenderContext.SetMatrixMode(MaterialMatrixMode.Model);
            RenderContext.LoadIdentity();

            RenderContext.SetMatrixMode(MaterialMatrixMode.View);
            RenderContext.LoadIdentity();

            //Get a dynamic mesh, which must be rendered on this frame
            Mesh mesh = RenderContext.GetDynamicMesh();
            //Each particle has 4 sides
            MeshBuilder builder = new MeshBuilder(mesh, MaterialPrimitiveType.MATERIAL_QUADS, 800);

            foreach (Particle particle in particles)
            {
                float sa = (float)Math.Sin(particle.Roll);
                float ca = (float)Math.Cos(particle.Roll);
                float size = particle.Size;
                //Translate the particle to be in the right position in accordance with the view matrix
                Vector pos = tempView.VMul4x3(particle.Position);

                //Push top-right corner
                builder.Position(new Vector(pos.x + (-ca + sa) * size, pos.y + (-sa - ca) * size, pos.z));
                builder.Color(particle.Color);
                builder.TexCoord(0.0f, 1.0f);
                builder.AdvanceVertex();

                //Push top-left corner
                builder.Position(new Vector(pos.x + (-ca - sa) * size, pos.y + (-sa + ca) * size, pos.z));
                builder.Color(particle.Color);
                builder.TexCoord(0.0f, 0.0f);
                builder.AdvanceVertex();

                //Push bottom-left corner
                builder.Position(new Vector(pos.x + (ca - sa) * size, pos.y + (sa + ca) * size, pos.z));
                builder.Color(particle.Color);
                builder.TexCoord(1.0f, 0.0f);
                builder.AdvanceVertex();

                //Push bottom-right corner
                builder.Position(new Vector(pos.x + (ca + sa) * size, pos.y + (sa - ca) * size, pos.z));
                builder.Color(particle.Color);
                builder.TexCoord(1.0f, 1.0f);
                builder.AdvanceVertex();
            }

            //Finish the mesh and render it
            builder.End(true);

            //Return the state of rendering before we were rendering the particles
            RenderContext.EnableUserClipTransformOverride(false);

            RenderContext.SetMatrixMode(MaterialMatrixMode.Model);
            RenderContext.LoadMatrix(tempModel);

            RenderContext.SetMatrixMode(MaterialMatrixMode.View);
            RenderContext.LoadMatrix(tempView);

            return 1;
        }

        public override void Dispose()
        {
            //Remove from renderables
            base.Dispose();

            Game.RemoveThinkable(this);
        }
    }


    public class Particle
    {
        // AddSimpleParticle automatically initializes these fields.
        public Vector Position;
        public Vector Velocity;

        public float Roll;
        public float RollDelta;

        public float DieTime;	// How long the particle has to live for.
        public float LifeTime;	// How long the particle has lived.
        private Color _Color;

        public float StartAlpha;
        public float EndAlpha;

        public float StartSize;
        public float EndSize;

        public float LifeProgress
        {
            get { return LifeTime / DieTime; }
        }

        public float Alpha
        {
            get { return StartAlpha + (EndAlpha - StartAlpha) * LifeProgress; }
        }

        public float Size
        {
            get { return StartSize + (EndSize - StartSize) * LifeProgress; }
        }

        public Color Color
        {
            get
            {
                return new Color(_Color, (byte)this.Alpha);
            }
            set
            {
                _Color = value;
            }
        }


    }

    public class ParticleUtil
    {
        internal static readonly IntPtr _ParticleSystem;

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void Sparks(Vector pos, int nMagnitude, int nTrailLength, Vector pDir);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void MuzzleFlash(Vector vecOrigin, QAngle vecAngles, float flScale, int iType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void Smoke(Vector origin, int modelIndex, float scale, float framerate);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void Dust( Vector pos, Vector dir, float size, float speed);
    }

}
