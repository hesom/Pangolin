#include <iostream>

#include <pangolin/pangolin.h>

struct CustomType
{
  CustomType()
    : x(0), y(0.0f) {}

  CustomType(int x, float y, std::string z)
    : x(x), y(y), z(z) {}

  int x;
  float y;
  std::string z;
};

std::ostream& operator<< (std::ostream& os, const CustomType& o){
  os << o.x << " " << o.y << " " << o.z;
  return os;
}

std::istream& operator>> (std::istream& is, CustomType& o){
  is >> o.x;
  is >> o.y;
  is >> o.z;
  return is;
}

void SampleMethod()
{
    std::cout << "You typed ctrl-r or pushed reset" << std::endl;
}

int main( int /*argc*/, char* argv[] )
{  
  // Load configuration data
  pangolin::ParseVarsFile("app.cfg");

  // Create OpenGL window in single line thanks to GLUT
  pangolin::CreateWindowAndBind("Main",640,480);
  
  // 3D Mouse handler requires depth testing to be enabled
  glEnable(GL_DEPTH_TEST);
  
  // Issue specific OpenGl we might need
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

  // Define Camera Render Object (for view / scene browsing)
  pangolin::OpenGlRenderState s_cam(
    pangolin::ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
    pangolin::ModelViewLookAt(-0,0.5,-3, 0,0,0, pangolin::AxisY)
  );

  const int UI_WIDTH = 180;

  // Add named OpenGL viewport to window and provide 3D Handler
  pangolin::View& d_cam = pangolin::CreateDisplay()
    .SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0, -640.0f/480.0f)
    .SetHandler(new pangolin::Handler3D(s_cam));

  // Add named Panel and bind to variables beginning 'ui'
  // A Panel is just a View with a default layout and input handling
  pangolin::CreatePanel("ui")
      .SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(UI_WIDTH));

  // Safe and efficient binding of named variables.
  // Specialisations mean no conversions take place for exact types
  // and conversions between scalar types are cheap.
  pangolin::Var<bool> a_button("ui.A Button",false,false);
  pangolin::Var<double> a_double("ui.A Double",3,0,5);
  pangolin::Var<int> an_int("ui.An Int",2,0,5);
  pangolin::Var<double> a_double_log("ui.Log scale var",3,1,1E4, true);
  pangolin::Var<bool> a_checkbox("ui.A Checkbox",false,true);
  pangolin::Var<int> an_int_no_input("ui.An Int No Input",2);
  pangolin::Var<CustomType> any_type("ui.Some Type", CustomType(0,1.2,"Hello") );

  pangolin::Var<bool> save_window("ui.Save Window",false,false);
  pangolin::Var<bool> save_teapot("ui.Save Teapot",false,false);

  pangolin::Var<bool> record_teapot("ui.Record Teapot",false,false);

#ifdef CPP11_NO_BOOST
  // boost::function / std::function objects can be used for Var's too.
  // In C++11, these work great with closures.
  pangolin::Var<std::function<void(void)> > reset("ui.Reset", SampleMethod);
#endif

  // Demonstration of how we can register a keyboard hook to alter a Var
  pangolin::RegisterKeyPressCallback(pangolin::PANGO_CTRL + 'b', pangolin::SetVarFunctor<double>("ui.A Double", 3.5));

  // Demonstration of how we can register a keyboard hook to trigger a method
  pangolin::RegisterKeyPressCallback(pangolin::PANGO_CTRL + 'r', SampleMethod);

  // Default hooks for exiting (Esc) and fullscreen (tab).
  while( !pangolin::ShouldQuit() )
  {
    // Clear entire screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

    if( pangolin::Pushed(a_button) )
      std::cout << "You Pushed a button!" << std::endl;

    // Overloading of Var<T> operators allows us to treat them like
    // their wrapped types, eg:
    if( a_checkbox )
      an_int = a_double;

    if( !any_type->z.compare("robot"))
        any_type = CustomType(1,2.3,"Boogie");

    an_int_no_input = an_int;

    if( pangolin::Pushed(save_window) )
        pangolin::SaveWindowOnRender("window");

    if( pangolin::Pushed(save_teapot) )
        d_cam.SaveOnRender("teapot");
    
    if( pangolin::Pushed(record_teapot) )
        pangolin::DisplayBase().RecordOnRender("ffmpeg:[fps=50,bps=8388608,unique_filename]//screencap.avi");

    // Activate efficiently by object
    d_cam.Activate(s_cam);

    // Render some stuff
    glColor3f(1.0,1.0,1.0);
    glutWireTeapot(1.0);

    // Swap frames and Process Events
    pangolin::FinishFrame();
  }

  return 0;
}
