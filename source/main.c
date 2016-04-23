#include <string.h>
#include <stdio.h>
#include <3ds.h>

int main()
{
	Handle rsrc;
	bool failed = true;

	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	printf("\x1b[10;10HFetching DSP component...\x1b[12;10H");

	rsrc = envGetHandle("hb:ndsp");
	if (!rsrc)
	{
		printf("\x1b[31;1mFailed\x1b[0m: Need to run using *hax 2.0+");
	}
	else do
	{
		Result rc;
		u32 len;
		void* bin;
		extern u32 fake_heap_end;
		char* filename = "sdmc:/3ds/dspfirm.cdc";

		u32 mapAddr = (fake_heap_end+0xFFF) &~ 0xFFF;
		rc = svcMapMemoryBlock(rsrc, mapAddr, 0x3, 0x3);
		if (R_FAILED(rc)) break;

		len = *(u32*)(mapAddr + 0x104);
		bin = malloc(len);
		if (bin)
			memcpy(bin, (void*)mapAddr, len);
		svcUnmapMemoryBlock(rsrc, mapAddr);
		if (!bin) break;

		FILE* file = fopen(filename, "wb");
		if (!file) break;
		fwrite(bin, 1, len, file);
		fclose(file);

		failed = false;
		printf("\x1b[32;1mDone\x1b[0m: No further steps needed!\n");
		free(bin);
	} while (0);

	if (rsrc && failed)
		printf("\x1b[31;1mFailed\x1b[0m: Unknown error. Try again.");

	printf("\x1b[28;15HPress START to exit.");

	while (aptMainLoop())
	{
		hidScanInput();
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START)
			break;

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}
