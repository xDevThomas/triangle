#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class simApp : public AppNative {
public:
    void prepareSettings( Settings *settings );
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();
    void resize();
};

void simApp::prepareSettings(Settings *settings)
{
    Window::Format fmt;
    fmt.setTitle( "Triangle Simulator" );
    fmt.setSize( 1280, 720 );
    settings->prepareWindow(fmt);
    settings->setFrameRate( 60.0f );
    settings->enableHighDensityDisplay();
}

void simApp::setup()
{
}

void simApp::mouseDown( MouseEvent event )
{
}

void simApp::update()
{
}

void simApp::resize()
{
}

void simApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
}

CINDER_APP_NATIVE( simApp, RendererGl )
