using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Sharp;

namespace SourceForts
{
    public delegate void TeamButtonHandler();

    public class TeamButton : Button
    {
        public Teams TeamNumber { get; private set; }

        public event TeamButtonHandler Clicked;

        public TeamButton(string text, Teams team)
            : base(text)
        {
            TeamNumber = team;


            Wide = 190;
            Tall = 90;
        }

        public override void OnClick()
        {
            base.OnClick();

            ClientEngine.ClientCmd(string.Format("jointeam {0}", (int)TeamNumber));

            if (Clicked != null)
                Clicked.Invoke();
        }

    }

    public class TeamSelectGUI : Panel
    {
        public TeamButton Red = new TeamButton("RED", Teams.Red);
        public TeamButton Blue = new TeamButton("BLUE", Teams.Blue);

        public TeamSelectGUI()
        {
            Red.Parent = this;
            Blue.Parent = this;

            Wide = 200;
            Tall = 200;

            InvalidateLayout();
            PerformLayout();
            MoveToFront();

            AllowMouseInput = true;
            Red.AllowMouseInput = true;
            Blue.AllowMouseInput = true;

            Red.Clicked += RegisterForDispose;
            Blue.Clicked += RegisterForDispose;
        }

        public void RegisterForDispose()
        {
            Timer.NewTimer(0.0f, () =>
            {
                this.Dispose();
            });
        }

        public override void PerformLayout()
        {
            base.PerformLayout();
            CenterHorizontally();
            Y = 300;

            Red.Y = 5;
            Red.X = 5;

            Blue.Y = 105;
            Blue.X = 5;
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
