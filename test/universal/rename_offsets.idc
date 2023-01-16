#include <idc.idc>

static main()
{
	auto fp;
	auto lineContents;
	auto subString;
	auto offset;
	auto function;
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

		subString = substr(lineContents, 8, 11);

		if (subString == "0x0")
		{
			offset = substr(lineContents, 8, 18);
			function = substr(lineContents, 27, strlen(lineContents) - 3);
			MakeName(offset, function);
			count++;
		}
	}

	Message("Success! Renamed total of %i offsets.\n", count);
}