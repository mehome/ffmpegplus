#include "stdafx.h"

const char* videodevice[][2] = {
	{"gdigrab","desktop"}
	,{"dshow","video=screen-capture-recorder"}
};

int main()
{
	avdevice_register_all();
	const char* filename = "recordvideoonly1.mp4";
	out_avfile file;
	if (file.create(filename) < 0)
		return -1;
	if (file.addvideo(1600, 900) < 0)
		return -1;
	if (file.open() < 0)
		return -1;

	in_videosource in;
	//in_avfile in;
	if (in.create(videodevice[0][0], videodevice[0][1]) < 0)
		return -1;

	videotransformer adapter(in.getvideo(), file.getvideo());//创建转换器并关联输入、输出
	int64_t num = 100;
	while (num--) {
		if (in.read() < 0) {
		}
		else {//采集到图片
			adapter.transform();//转换pix format
			file.writevideo();
		}
	}
	file.close();
	return 0;
}