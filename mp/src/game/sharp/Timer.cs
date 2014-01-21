using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Sharp
{
    public delegate void TimerCallback();

    public class TimerItem
    {
        public float DueTime;
        public TimerCallback Callback;

        public TimerItem(float dueTime, TimerCallback callback)
        {
            this.DueTime = dueTime;
            this.Callback = callback;
        }
    }

    public class TimersByDueTime : IComparer<TimerItem>
    {
        public int Compare(TimerItem x, TimerItem y)
        {
            return x.DueTime.CompareTo(y.DueTime);
        }
    }

    public static class Timer
    {
        //internal static SortedSet<TimerItem> timers = new SortedSet<TimerItem>(new TimersByDueTime());
        internal static List<TimerItem> timers = new List<TimerItem>();

        public static void NewTimer(float seconds, TimerCallback callback)
        {
            TimerItem timer = new TimerItem(Game.CurTime + seconds, callback);
            timers.Add(timer);
        }

        internal static void Think()
        {
            timers.RemoveAll((timer) => ThinkTimer(timer));

            /*
             * float curTime = Game.CurTime;
            while (timers.Count > 0)
            {
                
                TimerItem item = timers.First();

                if (item.DueTime < curTime)
                    break;

                timers.Remove(item);

                try
                {
                    item.Callback();
                }
                catch (Exception e)
                {
                    Console.WriteLine("Unhandled exception on timer: {0}", e);
                }
                
            }
             *  * */

        }

        private static bool ThinkTimer(TimerItem item)
        {
            if (Game.CurTime > item.DueTime)
            {
                item.Callback();
                return true;
            }
            return false;
        }

    }


}
