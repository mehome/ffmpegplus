#include "stdafx.h"

int main()
{
	const char* outfile = "out.mp3";
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

	audiotransformer adapter(input.getaudio(), file.getaudio());
	while (true) {
		if (adapter.enough())
		{
			file.writeaudio();
			continue;
		}
		int ret = input.read();
		if (ret < 0)
			break;
		else if (ret == 2)
		{
			if (adapter.transform() >= 0)
				file.writeaudio();
		}
	}
	file.close();
	return 0;
}