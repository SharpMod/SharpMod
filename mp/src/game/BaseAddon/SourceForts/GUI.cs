using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Sharp;

namespace SourceForts
{
    public class TeamButton : Button
    {
        public TeamButton(string text) : base(text)
        {
        }

        public override void OnClick()
        {
            base.OnClick();
            Console.WriteLine("GOT ON CLICK");
        }

    }

    public class TeamSelectGUI : Panel
    {
        public Button Red = new TeamButton("RED");
        public Button Blue = new TeamButton("BLUE");

        public TeamSelectGUI()
        {
            Red.Parent = this;
            Blue.Parent = this;

            Wide = 200;
            Tall = 200;

            InvalidateLayout();
            PerformLayout();
            MoveToFront();

            this.AllowMouseInput = true;
            Red.AllowMouseInput = true;
            Blue.AllowMouseInput = true;
            this.Enabled = true;
            Blue.Enabled = true;
            Red.Enabled = true;
        }

        public override void PerformLayout()
        {
            base.PerformLayout();
            CenterHorizontally();
            Y = 300;

            Red.Y = 5;
            Red.X = 5;
            Red.Wide = 190;
            Red.Tall = 90;

            Blue.Y = 105;
            Blue.X = 5;
            Blue.Wide = 190;
            Blue.Tall = 90;

            Console.WriteLine("LINE!");
        }

        public static TeamSelectGUI TeamSelectGui;

        [ConCommand("+teamchoose", "Show the GUI", CommandFlags.CLIENTDLL)]
        public static void ShowTeamChoose(CCommand command)
        {
            if (TeamSelectGui != null)
            {
                TeamSelectGui.Dispose();
                TeamSelectGui = null;
            }

            TeamSelectGui = new TeamSelectGUI();
            TeamSelectGui.MakePopup();
        }

        [ConCommand("-teamchoose", "Hide the GUI", CommandFlags.CLIENTDLL)]
        public static void HideTeamChoose(CCommand command)
        {
            if (TeamSelectGui != null)
            {
                TeamSelectGui.Dispose();
                TeamSelectGui = null;
            }
        }
    }
}
