// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <atlimage.h>
#include <vector>
#include <string>

bool	Composite(const char* imageFile, const char* alphaFile, const char* outputFile) {
	CImage finalImg, alphaImg;
	if(FAILED(finalImg.Load(imageFile)))
		return false;
	if (FAILED(alphaImg.Load(alphaFile)))
		return false;
	CImage img;
	img.Create(finalImg.GetWidth(), finalImg.GetHeight(), 32, CImage::createAlphaChannel);
	printf("转换(final:%d, alpha:%d)保存到:%s\n", finalImg.GetBPP(), alphaImg.GetBPP(), outputFile);
	int nWidth = finalImg.GetWidth();
	int nHeight = finalImg.GetHeight();
	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			unsigned char* finalRGB = (unsigned char*)finalImg.GetBits() + finalImg.GetPitch()*y + x * (finalImg.GetBPP()/8);
			unsigned char* alphaRGB = (unsigned char*)alphaImg.GetBits() + alphaImg.GetPitch()*y + x * (alphaImg.GetBPP()/8);
			unsigned char* imgRGB = (unsigned char*)img.GetBits() + img.GetPitch()*y + x * 4;
			imgRGB[0] = finalRGB[0];
			imgRGB[1] = finalRGB[1];
			imgRGB[2] = finalRGB[2];
			imgRGB[3] = alphaRGB[0];
		}
	}
	img.Save(outputFile);
}
struct ImageFile {
	std::string	finalImage;
	std::string	depthImage;
};
int main(int argc, char** argv)
{
	if (argc < 2)return 0;
	char path[512];
	char dir[512];
	strcpy(dir, argv[1]);
	strcat(dir, "\\");
	WIN32_FIND_DATAA fd;
	memset(&fd, 0, sizeof(fd));
	sprintf(path, "%s*.png", dir);
	HANDLE h = FindFirstFile(path, &fd);
	if (h == INVALID_HANDLE_VALUE) {
		printf("找不到任何文件");
		return 0;
	}
	const char* finalImage = "finalimage";
	const char* customdepth = "customdepth";
	std::vector<ImageFile> Images;
	std::vector<std::string> Depths;
	while(true){
		sprintf(path, "%s%s", dir, fd.cFileName);
		strlwr(path);
		if (strstr(path, finalImage)) {
			ImageFile imf;
			imf.finalImage = path;
			Images.push_back(imf);
		}
		else if (strstr(path, customdepth)) {
			Depths.push_back(path);
		}
		if (!FindNextFile(h, &fd))
			break;
	}
	FindClose(h);
	for (int i = 0; i < Images.size(); i++) {
		std::string ef = Images[i].finalImage;
		ef = ef.replace(ef.find(finalImage), strlen(finalImage), "");
		for (int j = 0; j < Depths.size(); j++) {
			std::string ed = Depths[i];
			ed = ed.replace(ed.find(customdepth), strlen(customdepth), "");
			if (ef == ed) {
				Images[i].depthImage = Depths[i];
				break;
			}
		}
	}
	//
	char outname[512];
	char fileName[256];
	int nFail = 0;
	sprintf(outname, "%sOutput", dir);
	CreateDirectory(outname, 0);
	for (int i = 0; i < Images.size(); i++) {
		std::string ef = Images[i].finalImage;
		ef = ef.replace(ef.find(finalImage), strlen(finalImage), "");
		_splitpath(ef.c_str(), 0, 0, fileName, 0);
		sprintf(outname, "%sOutput\\%s.%s", dir, fileName, ".png");
		Composite(Images[i].finalImage.c_str(), Images[i].depthImage.c_str(), outname);
	}
    std::cout << "Complete!\n"; 
}
