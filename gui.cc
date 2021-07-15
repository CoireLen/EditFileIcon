#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Image.H>
#include "1.h"
void bt_load_call(Fl_Widget*, void*); //读取载入注册表信息
void bt_open_call(Fl_Widget* w, void* v);//打开ico文件路径;
void bt_set_call(Fl_Widget* w, void* v);//设置图标位置
void LoadIconImg();//载入图片
class ToolScreen
{
public:
	ToolScreen(int argc, char** argv);
	~ToolScreen();

	Fl_Window* window;
	Fl_Input* in_file;
	Fl_Button* bt_loadfilepath;
	Fl_Input* in_icopath;
	Fl_Button* bt_open;
	Fl_Button* bt_set;
	Fl_Button* bt_save;
	Fl_Button* bt_load;
	Fl_Box* bx_icon;
	std::vector<std::string> data;
private:
	
};

ToolScreen::ToolScreen(int argc,char ** argv)
{
	window = new Fl_Window(370, 140,"Coire Icon Tool");
	in_file= new Fl_Input(100, 10, 200, 30, NULL);
	bt_loadfilepath= new Fl_Button(310, 10, 50, 30, "Load");
	in_icopath= new Fl_Input(100, 50, 200, 30, NULL);
	bt_open= new Fl_Button(310, 50, 50, 30, "Open");
	bt_set = new Fl_Button(100, 90, 60, 30, "Set Icon");
	bt_save = new Fl_Button(170, 90,80, 30, "Save to File");
	bt_load = new Fl_Button(260, 90, 100, 30, "Load from File");
	bx_icon = new Fl_Box(0, 10, 100, 80,"Icon");
	bt_loadfilepath->callback((Fl_Callback*)bt_load_call,this);
	bt_open->callback((Fl_Callback*)bt_open_call, this);
	bt_set->callback(bt_set_call, this);
	window->end();
	window->show(argc, argv);
	Fl::run();
}

ToolScreen::~ToolScreen()
{

}
void bt_load_call(Fl_Widget* w, void* v) 
{
	ToolScreen* ts = (ToolScreen*)v;
	ts->data=reg_icon::LoadData(std::string(ts->in_file->value()));
	if (ts->data.size() >= 2) {
		ts->in_icopath->value(ts->data[1].c_str());
		//载入dll exe ico图标
		Fl_Image* img=reg_icon::IconImg(ts->data[1]);
		if (img != NULL) {
			ts->bx_icon->image(img->copy());
			ts->bx_icon->label("");
		}
	}
	else {
		reg_icon::ShowMessage(ts->data[0]);//输出报错
	}
}
void bt_open_call(Fl_Widget* w, void* v) {
	ToolScreen* ts = (ToolScreen*)v;
	auto path = reg_icon::IconFilePath();
	ts->in_icopath->value(path.c_str());
	if (path.size()!=0) {
		Fl_Image* img = reg_icon::IconImg(path);
		if (img != NULL) {
			ts->bx_icon->image(img->copy());
			ts->bx_icon->label("");
		}
	}
	//载入dll exe ico图标
}
void bt_set_call(Fl_Widget* w, void* v) //设置.xxx的图标
{
	ToolScreen* ts = (ToolScreen*)v;
	auto err=reg_icon::SetIconDef(ts->in_file->value(), ts->in_icopath->value());
	reg_icon::ShowMessage(err);

}
int main(int argc, char** argv) {
	ToolScreen screen(argc, argv);
}