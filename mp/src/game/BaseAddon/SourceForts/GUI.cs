using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Sharp;

namespace SourceForts
{
    class TeamSelectGUI : Panel
    {
        public Button Red = new Button("RED");
        public Button Blue = new Button("BLUE");

        public TeamSelectGUI()
        {
            Red.Parent = this;
            Blue.Parent = this;

            Wide = 200;
            Tall = 200;

            InvalidateLayout();
            PerformLayout();
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
    }
}
