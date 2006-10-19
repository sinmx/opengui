
#include "../DemoAppFramework/DemoAppFrameWork.h"
//#include "../TachometerWidget/Tachometer.h"

class Demo1App : public DemoApp
{
public:
	Demo1App() : DemoApp("OpenGUI - Demo 1") {}
	virtual void preRun();
	virtual void perframeRun();
	virtual void postRun(){
		int i = 0;
	}
	virtual void mousePositionCallback(int x, int y);
private:
	//OpenGUI::Widgets::TextLabel* mLabel;
	OpenGUI::TimerPtr mTimer;
	OpenGUI::Screen* mScreen;
};

using namespace OpenGUI;

class MyWidget:public Widget{
protected:
	void onDraw(Object* sender, Draw_EventArgs& evtArgs ){
		Brush& b = evtArgs.brush;
		//b.pushRotation( Degree(12) );
		FVector2 pos(400,300);
		Font fnt("pecot",30);
		
		FRect outlineRect(pos, FVector2(pos.x + 200, pos.y + 200));
		
		b.pushColor(Color::PresetRed());
		b.Text.drawTextArea("lowery CASE text",
			outlineRect, fnt, true, TextAlignment::ALIGN_JUSTIFIED, TextAlignment::ALIGN_CENTER);
		b.pop();

		b.Primitive.drawLine(FVector2(outlineRect.min.x, outlineRect.min.y + outlineRect.getHeight()/2 ),
			FVector2(outlineRect.max.x, outlineRect.min.y + outlineRect.getHeight()/2 ), 2.0f);

		b.pushAlpha(0.25f);
		b.pushColor(Color::PresetBlue());
		b.Primitive.drawRect( outlineRect );
		b.pop();
		b.Primitive.drawOutlineRect( outlineRect, -5);
		b.pop();

		//b.pop();


		ImageryPtrList imgrylist = FontManager::getSingleton()._getFontAtlases();
		if ( imgrylist.size() > 0 ){
			//b.Image.drawImageUnscaled( imgrylist.front(), FVector2( 0, 0 ) );
			b.Image.drawImageUnscaledAndTiled(imgrylist.front(), FRect(0,0,150,400));
		}
	}
};
void Demo1App::preRun()
{
	mScreen = ScreenManager::getSingleton().createScreen("MainScreen", FVector2(800,600) );
	FontManager::getSingleton().RegisterFontSet("pecot.ttf","pecot");

	mScreen->enableCursor();

	Widget* w = new MyWidget();
	w->setName("Blah");

	mScreen->Children.add_back( w, true );
/*
	OpenGUI::XMLParser::LoadFromFile("demo1.xml");
	OpenGUI::System::getSingleton().setGUISheet(
		OpenGUI::System::getSingleton().getGUISheetByName("root")
		);
	mLabel = static_cast<OpenGUI::Widgets::TextLabel*>
		( OpenGUI::System::getSingleton().createWidget("TextLabel","OpenGUI") );
	OpenGUI::System::getSingleton().getGUISheetByName("root")->addChildElement(mLabel,"Msg");
	mLabel->setFont("pecot");
	mLabel->setFontSize(16);
	mLabel->setAlignment_Horiz(OpenGUI::TextAlignment::ALIGN_RIGHT);
	mLabel->setPos(0.0f,0.0f);
	mLabel->setSize(1.0f,1.0f);
	mLabel->setText("Press ESC to Quit");

	mLabel = static_cast<OpenGUI::Widgets::TextLabel*>
		( OpenGUI::System::getSingleton().createWidget("TextLabel","OpenGUI") );
	OpenGUI::System::getSingleton().getGUISheetByName("root")->addChildElement(mLabel,"FPS");
	mLabel->setFont("pecot");
	mLabel->setFontSize(16);
	mLabel->setPos(0.0f,0.0f);
	mLabel->setSize(1.0f,1.0f);
	mLabel->setText("FPS On the way!");

	mTimer = OpenGUI::TimerManager::getSingleton().getTimer();
	*/
}
void Demo1App::perframeRun()
{
	/*
	static int val = 0;
	static bool dirUp = true;
	if(dirUp) val++;
	else val--;
	if(val <= 0) dirUp=true;
	if(val >= 900) dirUp=false;

	if(OpenGUI::System::getSingletonPtr()){
		OpenGUI::Widgets::Widget* w = (OpenGUI::Widgets::Widget*)
			OpenGUI::System::getSingleton().getElementByName("myTach");

		OpenGUI::Widgets::Tachometer* t = static_cast<OpenGUI::Widgets::Tachometer*>(w);
		t->setNeedleValue((float) val);

		if(mTimer->getMilliseconds() > 200){
			mTimer->reset();
			std::stringstream ss;
			ss << "FPS: " << OpenGUI::System::getSingleton().statRenderFPS();
			mLabel->setText(ss.str());
		}
	}
	*/
}
void Demo1App::mousePositionCallback(int x, int y){
	mScreen->injectCursorPosition((float)x, (float)y);
}

int main( void )
{
	Demo1App app;
	
	app.run();

	return 0;
}


