#include "1.h"
#include <Windows.h>
#include <format>
BOOL EndIs(std::string &src,const char * str) {
	BOOL ret = TRUE;
	for (int i = 0; i < strlen(str); i++) {
		ret &= src[src.length() - 1 - i] == str[strlen(str) - 1 - i];
	}
	return ret;
}
int ClipThePath(std::string& src) {
	if (EndIs(src, ".exe") || EndIs(src, ".dll")) {
		return 0;
	}
	else
	{
		int num = src.rfind(',');
		auto clip = src.substr(num + 1);
		if (clip.size() == 0)
			return 0;
		src = src.substr(0, num);
		return atoi(clip.c_str());
	}
}
namespace reg_icon {
	std::vector<std::string> LoadData(std::string filetype) {
		std::vector<std::string> vstr;
		vstr.resize(2);
		HKEY key1;
		int err = RegOpenKeyA(HKEY_CLASSES_ROOT, filetype.c_str(), &key1);
		if (err != ERROR_SUCCESS) {
			//if (err == 2) {
			//	err=RegCreateKeyA(HKEY_CLASSES_ROOT, filetype.c_str(), &key1);
			//	if (err != ERROR_SUCCESS) {
			//		show_error("CrateKey",filetype.c_str(), err);
			//		return 0;
			//	}
			//}
			vstr.clear();
			vstr.push_back(std::format("{} Error:{}", "OpenKey", err));
			return vstr;

		}
		char date1[1024]; DWORD sl = sizeof(date1);
		err = RegGetValueA(key1, NULL, NULL, RRF_RT_REG_SZ, NULL, date1, &sl);
		if (err != ERROR_SUCCESS) {
			vstr.clear();
			vstr.push_back(std::format("{} Error:{}", "GetValue", err));
			return vstr;
		}
		RegCloseKey(key1);
		vstr[0]=(std::string(date1));

		err = RegOpenKeyA(HKEY_CLASSES_ROOT, std::format("{}\\DefaultIcon", date1).c_str(), &key1);
		if (err != ERROR_SUCCESS) {
			vstr.clear();
			vstr.push_back(std::format("{} Error:{}", "OpenKey", err));
			return vstr;
		}
		sl = sizeof(date1);
		err = RegGetValueA(key1, NULL, NULL, RRF_RT_REG_SZ, NULL, date1, &sl);
		if (err != ERROR_SUCCESS) {
			vstr.clear();
			vstr.push_back(std::format("{} Error:{}", "GetValue", err));
			return vstr;
		}
		RegCloseKey(key1);
		vstr[1]=(std::string(date1));

		return vstr;
	}
	void ShowMessage(std::string &msg) {
		MessageBoxA(NULL, msg.c_str(), "ERROR", MB_OK);
	}
	std::string IconFilePath() {
		//打开窗口的代码
		OPENFILENAMEA ofn;       // common dialog box structure
		char szFile[260];       // buffer for file name
		HWND hwnd=0;              // owner window
		HANDLE hf;              // file handle

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = szFile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "All\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Display the Open dialog box. 

		if (GetOpenFileNameA(&ofn) == TRUE)
			return std::string(szFile);
		return std::string();
	}
	Fl_RGB_Image* Icon_To_Flrgb(HICON hIcon)
	{
		BITMAP bm;
		ICONINFO iconInfo;
		GetIconInfo(hIcon, &iconInfo);
		GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);
		int width = bm.bmWidth;
		int height = bm.bmHeight;
		int bytesPerScanLine = (width * 3 + 3) & 0xFFFFFFFC;
		int size = bytesPerScanLine * height;
		BITMAPINFO infoheader = { 0 };
		infoheader.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		infoheader.bmiHeader.biWidth = width;
		infoheader.bmiHeader.biHeight = height;
		infoheader.bmiHeader.biPlanes = 1;
		infoheader.bmiHeader.biBitCount = 24;
		infoheader.bmiHeader.biCompression = BI_RGB;
		infoheader.bmiHeader.biSizeImage = size;
		// allocate Memory for Icon RGB data plus Icon mask plus ARGB buffer for the resulting image
		unsigned char* pixelsIconRGB = new unsigned char[height * width * 4];
		if (pixelsIconRGB == NULL)
		{
			return NULL;
		}
		unsigned char* alphaPixels = new unsigned char[height * width * 4];
		if (alphaPixels == NULL)
		{
			delete[] pixelsIconRGB;
			return NULL;
		}
		unsigned char* imagePixels = new unsigned char[height * width * 4];
		if (imagePixels == NULL)
		{
			delete[] pixelsIconRGB;
			delete[] alphaPixels;
			return NULL;
		}
		HDC hDC = CreateCompatibleDC(NULL);
		if (hDC == NULL)
		{
			delete[] pixelsIconRGB;
			delete[] alphaPixels;
			return NULL;
		}
		HBITMAP hBmpOld = (HBITMAP)SelectObject(hDC, (HGDIOBJ)iconInfo.hbmColor);
		if (GetDIBits(hDC, iconInfo.hbmColor, 0, height, (LPVOID)pixelsIconRGB, &infoheader, DIB_RGB_COLORS) == 0)
		{
			DeleteDC(hDC);
			delete[] pixelsIconRGB;
			delete[] alphaPixels;
			delete[] imagePixels;
			return NULL;
		}
		SelectObject(hDC, hBmpOld);
		// now get the mask
		if (GetDIBits(hDC, iconInfo.hbmMask, 0, height, (LPVOID)alphaPixels, &infoheader, DIB_RGB_COLORS) == 0)
		{
			DeleteDC(hDC);
			delete[] pixelsIconRGB;
			delete[] alphaPixels;
			delete[] imagePixels;
			return NULL;
		}
		DeleteDC(hDC);
		int x = 0;
		int currentSrcPos = 0;
		int currentDestPos = 0;
		int linePosSrc = 0;
		int linePosDest = 0;
		int vsDest = height - 1;
		for (int y = 0; y < height; y++)
		{
			linePosSrc = (vsDest - y) * (width * 3);
			linePosDest = y * width * 4;
			for (x = 0; x < width; x++)
			{
				currentDestPos = linePosDest + (x * 4);
				currentSrcPos = linePosSrc + (x * 3);
				imagePixels[currentDestPos + 0] = pixelsIconRGB[currentSrcPos + 2];
				imagePixels[currentDestPos + 1] = pixelsIconRGB[currentSrcPos + 1];
				imagePixels[currentDestPos + 2] = pixelsIconRGB[currentSrcPos + 0];
				imagePixels[currentDestPos + 3] = 0xFF - alphaPixels[currentSrcPos];
			}
		}
		Fl_RGB_Image* pImage = new Fl_RGB_Image(imagePixels, width, height, 4);
		delete[] pixelsIconRGB;
		delete[] alphaPixels;
		return pImage;
	}
	void remove_fl_rgb_image(Fl_RGB_Image** img)
	{
		if (img != NULL)
		{
			Fl_RGB_Image* srci = (Fl_RGB_Image*)*img;
			if ((srci->array != NULL) && (srci->alloc_array == 0))
			{
				delete[] srci->array;
			}
			delete srci;
			*img = NULL;
		}
	}
	Fl_Image* IconImg(std::string filepath) {
		int num = ClipThePath(filepath);
		int max = (int)ExtractIconA(GetModuleHandle(NULL), filepath.c_str(), -1);
		if (EndIs(filepath,".exe")||EndIs(filepath,".dll")|| EndIs(filepath, ".ico")){
			HICON hIcon;
			ExtractIconExA(filepath.c_str(), num, &hIcon, NULL, max);
			if (hIcon > 0)
			{
				Fl_RGB_Image* a = Icon_To_Flrgb(hIcon);
				return a->copy();
			}
		}
		return NULL;
	}
	std::string SetIconDef(std::string filetype,std::string iconpath) {
		HKEY key1;
		DWORD n = sizeof(char) * (filetype.size() - 1 + 4);
		char date1[1024]; DWORD sl = sizeof(date1);
		int err = RegOpenKeyA(HKEY_CLASSES_ROOT, filetype.c_str(), &key1);
		if (err != ERROR_SUCCESS) {
			if (err == 2) {
				err=RegCreateKeyA(HKEY_CLASSES_ROOT, filetype.c_str(), &key1);
				if (err != ERROR_SUCCESS) {
					return std::format("{} {}Error:{}", "CreateKey",__LINE__, err);
					//show_error("CrateKey",filetype.c_str(), err);
					//return 0;
				}
			}
			return std::format("{} ()Error:{}", "OpenKey", __LINE__, err);

		}
		err = RegGetValueA(key1, NULL, NULL, RRF_RT_REG_SZ, NULL, date1, &sl);
		if (err != ERROR_SUCCESS) {
			if (err == 2) {
				err = RegSetValueA(key1, NULL, REG_SZ, filetype.substr(1).append("file").c_str(), n);
				if (err != ERROR_SUCCESS) {
					return std::format("{} {}Error:{}", "SetValue", __LINE__, err);
				}
				memcpy_s(date1, sl, filetype.substr(1).append("file").c_str(), n);
			}
			return (std::format("{} {}Error:{}", "GetValue", __LINE__, err));
		}
		RegCloseKey(key1);

		err = RegOpenKeyA(HKEY_CLASSES_ROOT, std::format("{}\\DefaultIcon",std::string(date1)).c_str(), &key1);
		if (err != ERROR_SUCCESS) {
			if (err == 2) {
				err = RegCreateKeyA(HKEY_CLASSES_ROOT, std::format("{}\\DefaultIcon", filetype.substr(1).append("file").c_str()).c_str(), &key1);
				if (err != ERROR_SUCCESS) {
					return (std::format("{} ()Error:{}", "CreateKey", __LINE__, err));
					//show_error("CrateKey",filetype.c_str(), err);
					//return 0;
				}
			}
			return(std::format("{} {}Error:{}", "OpenKey", __LINE__, err));
			//return vstr;
		}
		err = RegSetValueA(HKEY_CLASSES_ROOT, std::format("{}\\DefaultIcon", filetype.substr(1).append("file").c_str()).c_str(),
			REG_SZ, iconpath.c_str(), iconpath.size() * sizeof(char));
		if (err != ERROR_SUCCESS) {
			return (std::format("{} {}Error:{}", "SetVaule", __LINE__, err));
		}
		RegCloseKey(key1);
		return std::string("OK");
	}
}