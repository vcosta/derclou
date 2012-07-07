void DemoDialog(void)
{
    static first_time = 1;
    ubyte choice = 0;
    uword briggs = 56, tony = 57, matt = 7;

    tcSetPlayerMoney(10000);

    if (first_time) {
	first_time = 0;

	SetBubbleType(THINK_BUBBLE);
	Say(DEMO_TXT, 0, matt, "Matt_Start");
	Say(DEMO_TXT, 0, briggs, "Briggs_1");
	Say(DEMO_TXT, 0, matt, "Matt_Hi");
	Say(DEMO_TXT, 0, tony, "Tony_Hi");
	Say(DEMO_TXT, 0, briggs, "Briggs_1_2");
	Say(DEMO_TXT, 0, tony, "Tony_1");

	while (choice != 4) {
	    choice = Say(DEMO_TXT, 0, matt, "Matt_1");

	    switch (choice) {
	    case 0:
		Say(DEMO_TXT, 0, tony, "Tony_Juw");
		break;
	    case 1:
		Say(DEMO_TXT, 0, tony, "Tony_Aunt");
		break;
	    case 2:
		Say(DEMO_TXT, 0, tony, "Tony_Ken");
		break;
	    case 3:
		Say(DEMO_TXT, 0, tony, "Tony_Bank");
		break;
	    default:
		break;
	    }
	}

	Say(DEMO_TXT, 0, briggs, "Briggs_2");
	Say(DEMO_TXT, 0, tony, "Tony_2");
	Say(DEMO_TXT, 0, briggs, "Briggs_3");
	Say(DEMO_TXT, 0, tony, "Tony_3");
	Say(DEMO_TXT, 0, briggs, "Briggs_4");
	Say(DEMO_TXT, 0, tony, "Tony_Bye");
	Say(DEMO_TXT, 0, matt, "Matt_Bye");

	knowsSet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
	knowsSet(Person_Matt_Stuvysunt, Person_Tony_Allen);

	joined_bySet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
	joined_bySet(Person_Matt_Stuvysunt, Person_Tony_Allen);

	joinSet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
	joinSet(Person_Matt_Stuvysunt, Person_Tony_Allen);
    }
}
