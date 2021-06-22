#include "1.h"
#include <Windows.h>
#include <format>
BOOL EndIs(std::string &src,const char * str) {
	return TRUE;
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
		return std::string();
	}
	Fl_Image* IconImg(std::string filepath) {
		int num = ClipThePath(filepath);
		if (EndIs(filepath,".exe")||EndIs(filepath,".dll")){
			HICON hIcon = ExtractIconA(GetModuleHandle(NULL), filepath.c_str(), num);
			//算了不重要不写了
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