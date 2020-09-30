/* (c) DDNet developers. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.  */

#include <base/math.h>
#include <base/system.h>
#include <engine/graphics.h>
#include <engine/shared/datafile.h>
#include <engine/storage.h>
#include <game/mapitems.h>

#include <pnglite.h>
/*
	Usage: map_optimize <source map filepath> <dest map filepath>
*/

CDataFileReader g_DataReader;
CDataFileWriter g_DataWriter;

int main(int argc, const char **argv)
{
	dbg_logger_stdout();

	IStorage *pStorage = CreateStorage("Teeworlds", IStorage::STORAGETYPE_BASIC, argc, argv);

	if(argc < 2 || argc > 3)
	{
		dbg_msg("map_optimize", "Invalid arguments");
		dbg_msg("map_optimize", "Usage: map_optimize <source map filepath> [<dest map filepath>]");
		return -1;
	}

	if(!pStorage)
	{
		dbg_msg("map_optimize", "error loading storage");
		return -1;
	}

	const char *pSourceFileName = argv[1];

	const char *pDestFileName;
	char aDestFileName[MAX_PATH_LENGTH];

	if(argc == 3)
	{
		pDestFileName = argv[2];
	}
	else
	{
		char aBuf[MAX_PATH_LENGTH];
		IStorage::StripPathAndExtension(pSourceFileName, aBuf, sizeof(aBuf));
		str_format(aDestFileName, sizeof(aDestFileName), "out/maps/%s.map", aBuf);
		pDestFileName = aDestFileName;
		if(fs_makedir("out") != 0)
		{
			dbg_msg("map_optimize", "failed to create data directory");
			return -1;
		}

		if(fs_makedir("out/maps") != 0)
		{
			dbg_msg("map_optimize", "failed to create out/maps directory");
			return -1;
		}
	}

	int ID = 0;
	int Type = 0;
	int Size = 0;
	void *pItem = 0;
	void *pData = 0;

	if(!g_DataReader.Open(pStorage, pSourceFileName, IStorage::TYPE_ABSOLUTE))
	{
		dbg_msg("map_optimize", "failed to open source map. filename='%s'", pSourceFileName);
		return -1;
	}

	if(!g_DataWriter.Open(pStorage, pDestFileName, IStorage::TYPE_ABSOLUTE))
	{
		dbg_msg("map_optimize", "failed to open destination map. filename='%s'", pDestFileName);
		return -1;
	}

	bool Success = true;

	// add all items
	for(int Index = 0; Index < g_DataReader.NumItems(); Index++)
	{
		pItem = g_DataReader.GetItem(Index, &Type, &ID);
		Size = g_DataReader.GetItemSize(Index);

		g_DataWriter.AddItem(Type, ID, Size, pItem);
	}

	// add all data
	for(int Index = 0; Index < g_DataReader.NumData(); Index++)
	{
		pData = g_DataReader.GetData(Index);
		Size = g_DataReader.GetDataSize(Index);
		g_DataWriter.AddDataZopfli(Size, pData);
	}

	g_DataReader.Close();
	g_DataWriter.Finish();
	return Success ? 0 : -1;
}
