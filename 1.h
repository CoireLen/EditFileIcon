
#include <string>
#include <vector>
#include <FL/Fl_Image.H>
namespace reg_icon {
	std::vector<std::string> LoadData(std::string filetype);
	std::string SetIconDef(std::string filetype, std::string iconpath);
	void ShowMessage(std::string &msg);
	std::string IconFilePath();
	Fl_Image* IconImg();
}