#include "stdafx.h"
#include "audioFilterTrans.h"

int main()
{
	const char* outfile = "filter_out.mp3";
	const char* infile = "in.mp4";
	//const char* infile = "in.aac";
	out_avfile file;
	if (file.create(outfile) < 0)
		return -1;
	if (file.addaudio() < 0)
		return -1;
	if (file.open() < 0)
		return -1;

	in_avfile input;
	if (input.create(infile) < 0)
		return -1;

	audioFilterTrans trans(input.getaudio(), file.getaudio());
	if (trans.init() < 0)
		return -1;
	while (true) {
		int ret = input.read();
		if (ret < 0)
			break;
		else if (ret == 2)
		{
			if (trans.transform() >= 0)
				file.writeaudio();
		}
	}
	file.close();
	return 0;
}