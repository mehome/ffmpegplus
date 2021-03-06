#include "stdafx.h"
#include <thread>

int main()
{
	avdevice_register_all();
	const char* filename = "record1.mp4";
	out_avfile file;
	if (file.create(filename) < 0)
		return -1;
	if (file.addvideo(1600, 900) < 0)
		return -1;
	if (file.addaudio() < 0)
		return -1;
	if (file.open() < 0)
		return -1;
	in_avfile in;
	if (in.create("gdigrab", "desktop") < 0)
		return -1;
	in_audiosource inputa;
	if (inputa.create("dshow", "audio=virtual-audio-capturer") < 0)
		return -1;
	videotransformer vtrans(in.getvideo(), file.getvideo());//创建转换器并关联输入、输出
	audiotransformer atrans(inputa.getaudio(), file.getaudio());
	packectqueue videos(30);
	packectqueue audios(110);
	std::thread readvideo([&] {
		while (true)
		{
			if (1 == in.read())
			{
				vtrans.transform();
				avpacket pkt;
				if (file.getvideo().readpacket(pkt) < 0)
					continue;
				if (-1 == videos.push(pkt, vtrans.getcurrentpts()))
					break;
			}
		}
	});
	std::thread readaudio([&] {
		while (true)
		{
			if (!atrans.enough())
			{
				if (inputa.read() < 0)
					continue;
				if (atrans.transform() < 0)
					continue;
			}
			avpacket pkt;
			if (file.getaudio().readpacket(pkt) < 0)
				continue;
			if (-1 == audios.push(pkt, atrans.getcurrentpts()))
				break;
		}
	});
	int64_t vpts = 0;
	int64_t apts = 0;
	int64_t vduration = vtrans.getduration();
	int64_t aduration = atrans.getduration();
	int getnum = 300;
	while (getnum > 0)
	{
		avpacket pkt;
		if (vpts <= apts && !videos.empty())
		{
			vpts = videos.pop(pkt);
			//printf_s("视频帧写入时间戳:%lld\n", vpts);
			vpts += vduration;
			getnum--;
		}
		else if (!audios.empty())
		{
			apts = audios.pop(pkt);
			//printf_s("音频帧写入时间戳:%lld\n", apts);
			apts += aduration;
		}
		else
		{
			av_usleep(30);
			continue;
		}
		file.getformat().write(pkt);
	}
	videos.stop();
	audios.stop();
	readvideo.join();
	readaudio.join();
	file.close();
	return 0;
}