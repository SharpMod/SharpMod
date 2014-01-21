using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Runtime.CompilerServices;

namespace Sharp
{
	public enum MaterialPrimitiveType
	{
		MATERIAL_TRIANGLES = 3,
		MATERIAL_QUADS = 7
	}

	public enum MaterialMatrixMode
	{
		View = 0,
		Projection = 1,
		Model = 10
	}

	public static class RenderContext
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static Mesh GetDynamicMesh();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static Material FindMaterial(String material);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static void Bind(Material material);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static void SetMatrixMode(MaterialMatrixMode mode);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static void GetMatrix(MaterialMatrixMode mode, ref VMatrix matrix);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static void LoadIdentity();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static void LoadMatrix(VMatrix matrix);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static void Ortho(double left, double top, double right, double bottom, double zNear, double zFar);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static void EnableUserClipTransformOverride(bool bEnable);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static void UserClipTransform(VMatrix worldToView);
	}

	public class Material : SharpObject
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern bool IsError();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern int GetMappingHeight();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern int GetMappingWidth();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern override void Dispose();
	}


	public class Mesh : SharpObject
	{
		private Mesh() { }

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern void Draw();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern override void Dispose();
	}

	public class MeshBuilder : SharpObject
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern MeshBuilder(Mesh mesh, MaterialPrimitiveType type, int vertexCount);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern void Position(Vector position);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern void Color(Color color);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern void TexCoord(float s, float t);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern void AdvanceVertex();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern void End(bool shouldDraw);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern override void Dispose();
	}


	public enum RenderableTranslucencyType
	{
		Opaque = 0,
		Translucent,
		TwoPass	// has both translucent and opaque sub-partsa
	};

	public abstract class DefaultClientRenderable : SharpObject
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern DefaultClientRenderable();

		protected Vector Origin;
		protected QAngle Angles;

		public virtual void GetRenderBounds(ref Vector mins, ref Vector maxs)
		{
			mins = new Vector(-32, -32, -32);
			maxs = new Vector(32, 32, 32);
		}

		public virtual bool ShouldDraw()
		{
			return true;
		}

		public virtual int DrawModel()
		{
			return 0;
		}

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern override void Dispose();
	}

	public static class ClientLeafSystem
	{

	}


}
