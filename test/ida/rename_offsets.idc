#include <idc.idc>

static main()
{
	auto fp;
	auto lineContents;
	auto subString;
	auto offset;
	auto function;
	auto variable;
	auto count = 0;

	fp = fopen("offset_database.txt", "r");

	if (!fp)
	{
		Message("Cannot open offset list file.");
		return;
	}

	while (1)
	{
		lineContents = readstr(fp);

		if (lineContents == -1)
			break;

		if (strlen(lineContents) < 2)
			continue;

		// Functions
		subString = substr(lineContents, 8, 11);

		if (subString == "0x0")
		{
			subString = substr(lineContents, 0, 7);

			if (subString == "SetJump")
			{
				offset = substr(lineContents, 8, 18);
				function = substr(lineContents, 27, strlen(lineContents) - 3);

				if (MakeName(offset, function))
					count++;
				else
					Message("Failed to rename offset '%s' function '%s'\n", offset, function);
			}
		}

		// Variables
		subString = substr(lineContents, 9, 12);

		if (subString == "0x0")
		{
			subString = substr(lineContents, 0, 8);

			if (subString == "SetDword")
			{
				offset = substr(lineContents, 9, 19);
				variable = substr(lineContents, 28, strlen(lineContents) - 3);

				if (MakeName(offset, variable))
					count++;
				else
					Message("Failed to rename offset '%s' variable '%s'\n", offset, variable);
			}
		}
	}

	Message("Success! Renamed total of %i offsets.\n", count);
}