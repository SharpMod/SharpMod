using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Linq;
using System.Text;

namespace Sharp
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Color
    {

        public Color(byte r, byte g, byte b)
            : this(r, g, b, 0)
        {
        }

        public Color(byte r, byte g, byte b, byte a)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = a;
        }

        public Color(Color color, byte a)
        {
            this.r = color.r;
            this.g = color.g;
            this.b = color.b;
            this.a = a;
        }

        private byte r;
        private byte g;
        private byte b;
        private byte a;

        public override string ToString()
        {
            return String.Format("Color({0},{1},{2},{4})", r, g, b, a);
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Vector
    {
        public static Vector Zero = new Vector(0, 0, 0);

        public Vector(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public float x;
        public float y;
        public float z;

        public float Length
        {
            get { return (float) Math.Sqrt(LengthSquared); }
        }

        public float LengthSquared
        {
            get { return x * x + y * y + z * z; }
        }

        public static Vector operator -(Vector a, Vector b)
        {
            return new Vector(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        public static Vector operator +(Vector a, Vector b)
        {
            return new Vector(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        public static Vector operator *(Vector a, Vector b)
        {
            return new Vector(a.x * b.x, a.y * b.y, a.z * b.z);
        }

        public static Vector operator /(Vector a, Vector b)
        {
            return new Vector(a.x / b.x, a.y / b.y, a.z / b.z);
        }

        public static Vector operator -(Vector vec)
        {
            return new Vector(-vec.x, -vec.y, -vec.z);
        }

        public static implicit operator Vector(float value)
        {
            return new Vector(value, value, value);
        }

        public Vector Normal
        {
            get{ return this / this.Length; }
        }

        public float Dot(Vector vector)
        {
            return x * vector.x + y * vector.y + z * vector.z;
        }

        public QAngle ToAngle()
        {
            float tmp, yaw, pitch;

            if (y == 0 && x == 0)
            {
                yaw = 0;
                pitch = z > 0 ? 270.0f : 90.0f;
            }
            else
            {
                yaw = (float) (Math.Atan2(y, x) * 180.0 / Math.PI);
                if (yaw < 0)
                    yaw += 360.0f;

                tmp = (float) Math.Sqrt(x * x + y * y);
                pitch = (float) (Math.Atan2(-z, tmp) * 180.0 / Math.PI);
                if (pitch < 0)
                    pitch += 360.0f;
            }

            return new QAngle(pitch, yaw, 0.0f);
        }

		/**
		 * Generates random vector with values from -1.0 to 1.0.
		 * NOTE: The vector is NOT normalized. 
		 * */
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern Vector Random();

        public override string ToString()
        {
            return String.Format("Vector({0},{1},{2})", x, y, z);
        }

        public float Distance(Vector origin)
        {
            return (this - origin).Length;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct QAngle
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void AngleVectors( QAngle angle, ref Vector forward, ref Vector right, ref Vector up ); 

        public float x, y, z;

        public QAngle( float x, float y, float z )
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public float Yaw
        {
            get { return x; }
            set { x = value; }
        }

        public float Pitch
        {
            get { return y; }
            set { y = value; }
        }

        public float Roll
        {
            get { return z; }
            set { z = value; }
        }

        public Vector Up {
            get
            {
                Vector forward = Vector.Zero, right = Vector.Zero, up = Vector.Zero;
                AngleVectors(this, ref forward, ref right, ref up);
                return up;
            }
        }

        public Vector Right
        {
            get
            {
                Vector forward = Vector.Zero, right = Vector.Zero, up = Vector.Zero;
                AngleVectors(this, ref forward, ref right, ref up);
                return right;
            }
        }

        public Vector Forward
        {
            get
            {
                Vector forward = Vector.Zero, right = Vector.Zero, up = Vector.Zero;
                AngleVectors(this, ref forward, ref right, ref up);
                return forward;
            }
        }

        public override string ToString()
        {
            return String.Format("QAngle({0},{1},{2})", x, y, z);
        }
    }

	[StructLayout(LayoutKind.Sequential)]
	public unsafe struct VMatrix
	{
		internal fixed float m[16];

		/// <summary>
		/// Resets the matrix to be the identity matrix
		/// </summary>
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern void Identity();

		/// <summary>
		/// Checks if the matrix is the identity matrix
		/// A.k.a. M*v=v (There is no change in the vector, when it is multiplied by the matrix)
		/// </summary>
		/// <returns>Returns true if the matrix is the identity.</returns>
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern bool IsIdentity();

		/// <summary>
		/// Multiply by the upper 3 rows.
		/// </summary>
		/// <param name="vIn"></param>
		/// <returns></returns>
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern Vector VMul4x3(Vector vIn);

	}

    public class MathX
    {
        public static float Approach(float current, float target, float delta)
        {
            if (current > target)
                return Math.Max(current - delta, target);
            return Math.Min(current + delta, target);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float Sin(float val);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float Cos(float val);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SinCos(float val, ref float sin, ref float cos);

        public static float Clamp(float rate, float min, float max)
        {
            if (rate < min) return min;
            if (rate > max) return max;
            return rate;
        }

        public static float NormalizeAngle(float p)
        {
            return (p + 180.0f) % 360.0f - 180.0f;
        }
    }

}
