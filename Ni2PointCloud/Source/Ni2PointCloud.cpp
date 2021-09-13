// STL Headers
#include <iostream>
// OpenNI2 Headers
#include <OpenNI.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "Ni2PointCloud.h"

using namespace std;
using namespace openni;

struct viewerState
{
    double yaw;
    double pitch;
    double lastX;
    double lastY;
    float offsetY;
    float lookatX;
    float lookatY;
    bool mouseLeft;
    bool mouseRight;
};

struct viewerState viewerStat;
GLFWwindow* window;
GLuint helpScreenTextureID;
bool bComputeCloud, bQuit;
bool bShowHelpScreen;
bool bVisualizedRGBFrame;
int windowWidth, windowHeight;
int frameWidth, frameHeight;

void drawAxes( float axisSize = 1.f, float axisWidth = 4.f )
{
    // Triangles For X axis
    glBegin( GL_TRIANGLES );
    glColor3f( 1.0f, 0.0f, 0.0f );
    glVertex3f( axisSize * 1.1f, 0.f, 0.f );
    glVertex3f( axisSize, -axisSize * 0.05f, 0.f );
    glVertex3f( axisSize, axisSize * 0.05f, 0.f );
    glVertex3f( axisSize * 1.1f, 0.f, 0.f );
    glVertex3f( axisSize, 0.f, -axisSize * 0.05f );
    glVertex3f( axisSize, 0.f, axisSize * 0.05f );
    glEnd();

    // Triangles For Y axis
    glBegin( GL_TRIANGLES );
    glColor3f( 0.f, 1.f, 0.f );
    glVertex3f( 0.f, axisSize * 1.1f, 0.0f );
    glVertex3f( 0.f, axisSize, 0.05f * axisSize );
    glVertex3f( 0.f, axisSize, -0.05f * axisSize );
    glVertex3f( 0.f, axisSize * 1.1f, 0.0f );
    glVertex3f( 0.05f * axisSize, axisSize, 0.f );
    glVertex3f( -0.05f * axisSize, axisSize, 0.f );
    glEnd();

    // Triangles For Z axis
    glBegin( GL_TRIANGLES );
    glColor3f( 0.0f, 0.0f, 1.0f );
    glVertex3f( 0.0f, 0.0f, 1.1f * axisSize );
    glVertex3f( 0.0f, 0.05f * axisSize, 1.0f * axisSize );
    glVertex3f( 0.0f, -0.05f * axisSize, 1.0f * axisSize );
    glVertex3f( 0.0f, 0.0f, 1.1f * axisSize );
    glVertex3f( 0.05f * axisSize, 0.f, 1.0f * axisSize );
    glVertex3f( -0.05f * axisSize, 0.f, 1.0f * axisSize );
    glEnd();

    glLineWidth( axisWidth );

    // Drawing Axis
    glBegin( GL_LINES );
    // X axis - Red
    glColor3f( 1.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( axisSize, 0.0f, 0.0f );

    // Y axis - Green
    glColor3f( 0.0f, 1.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, axisSize, 0.0f );

    // Z axis - Blue
    glColor3f( 0.0f, 0.0f, 1.0f );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, axisSize );
    glEnd();
}

void drawHelpScreen()
{
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glMatrixMode ( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D ( 0, windowWidth, 0, windowHeight );

    glMatrixMode ( GL_TEXTURE );
    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    int minX = windowWidth - helpScreenWidth, maxX = windowWidth;
    int minY = windowHeight - helpScreenHeight, maxY = windowHeight;

    glBindTexture( GL_TEXTURE_2D, helpScreenTextureID );
    glBegin( GL_POLYGON );

    glTexCoord2f( 0.0, 0.0 );
    glVertex2i( minX, minY );

    glTexCoord2f( 1.0, 0.0 );
    glVertex2i( maxX, minY );

    glTexCoord2f( 1.0, 1.0 );
    glVertex2i( maxX, maxY );

    glTexCoord2f( 0.0, 1.0 );
    glVertex2i( minX, maxY );

    glEnd();
    glMatrixMode ( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode ( GL_MODELVIEW );
    glPopMatrix();
    glPopAttrib();
}

void drawPointCloud( float **colorArr, float **vertexArr, int imgSize )
{
    glLoadIdentity();
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glClearColor( 0.0, 0.0, 0.0, 1 );

    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    gluPerspective( 60, ( float )windowWidth / windowHeight, 0.01f, 100000.0f );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    gluLookAt( viewerStat.lookatX, viewerStat.lookatY, 0, viewerStat.lookatX, viewerStat.lookatY, 1, 0, 1, 0 );
    glTranslatef( 0, 0, viewerStat.offsetY );
    glRotated( viewerStat.pitch, 1, 0, 0 );
    glRotated( viewerStat.yaw, 0, 1, 0 );
    glTranslatef( 0, 0, -0.5f );
    glPointSize( windowWidth / 640.0f );
    glEnable( GL_DEPTH_TEST );

    glBegin( GL_POINTS );
    for ( int i = 0; i < imgSize; i++ )
    {
        if ( vertexArr[i][2] != 0 )
        {
            glColor3f( colorArr[i][0], colorArr[i][1], colorArr[i][2] );
            glVertex3f( vertexArr[i][0], vertexArr[i][1], vertexArr[i][2] );
        }
    }
    glEnd();

    drawAxes( 100.0 );

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glPopAttrib();
}

void cursor_position_callback( GLFWwindow* window, double xpos, double ypos )
{
    if ( viewerStat.mouseLeft )
    {
        viewerStat.yaw -= ( xpos - viewerStat.lastX );
        viewerStat.pitch += ( ypos - viewerStat.lastY );
    }

    if ( viewerStat.mouseRight )
    {
        viewerStat.lookatX += ( xpos - viewerStat.lastX );
        viewerStat.lookatY += ( ypos - viewerStat.lastY );
    }

    viewerStat.lastX = xpos;
    viewerStat.lastY = ypos;
}

void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    if ( action == GLFW_PRESS )
    {
        if ( key == GLFW_KEY_SPACE )
        {
            viewerStat.yaw = 0.0;
            viewerStat.pitch = 0.0;
            viewerStat.offsetY = 0.0;
            viewerStat.lookatX = 0.0;
            viewerStat.lookatY = 0.0;
        }
        else if ( key == GLFW_KEY_P )
        {
            bComputeCloud ^= true;
        }
        else if ( key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q )
        {
            bQuit = true;
        }
        else if ( key == GLFW_KEY_H )
        {
            bShowHelpScreen ^= true;
        }
        else if ( key == GLFW_KEY_C )
        {
            bVisualizedRGBFrame ^= true;
        }
    }
}

void mouse_button_callback( GLFWwindow* window, int button, int action, int mods )
{
    if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS )
    {
        viewerStat.mouseLeft = true;
    }
    else
    {
        viewerStat.mouseLeft = false;
    }

    if ( button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS )
    {
        viewerStat.mouseRight = true;
    }
    else
    {
        viewerStat.mouseRight = false;
    }
}

void scroll_callback( GLFWwindow* window, double xoffset, double yoffset )
{
    viewerStat.offsetY -= static_cast<float>( yoffset * 100 );
}

void colorMap( float dis, float *outputR, float *outputG, float *outputB )
{
    int r, g, b;// 0 ~ 255
    int map[5][4] =
    {
        // {distance, R, G, B}
        {0, 0, 0, 255},
        {500, 0, 255, 255},
        {1000, 255, 255, 0},
        {2000, 255, 0, 0},
        {4000, 102, 0, 0}
    };
    if ( dis >= map[4][0] )
    {
        r = map[4][1];
        g = map[4][2];
        b = map[4][3];
    }
    else
    {
        for ( int i = 0; i < 4; i++ )
        {
            if ( map[i][0] <= dis && dis < map[i + 1][0] )
            {
                float t =  ( dis - map[i][0] ) / ( map[i + 1][0] - map[i][0] );
                r = map[i][1] + ( map[i + 1][1] - map[i][1] ) * t;
                g = map[i][2] + ( map[i + 1][2] - map[i][2] ) * t;
                b = map[i][3] + ( map[i + 1][3] - map[i][3] ) * t;
                break;
            }
        }
    }
    *outputR = r / 255.0;
    *outputG = g / 255.0;
    *outputB = b / 255.0;
}

void niComputeCloud( const VideoStream &depthStream, const void* depthFrameData, const void* colorFrameData,
                     float **imgDepth, float **imgColor, float **imgColorMap )
{
    const openni::DepthPixel* pDepth = ( const openni::DepthPixel* )depthFrameData;
    const openni::RGB888Pixel* pColor = ( const openni::RGB888Pixel* )colorFrameData;
    float fX, fY, fZ;
    for ( int y = 0, i = 0; y < frameHeight; y++ )
    {
        for ( int x = 0; x < frameWidth; x++ )
        {
            fX = 0.0;
            fY = 0.0;
            fZ = 0.0;
            if ( pDepth[i] != 0 )
            {
                CoordinateConverter::convertDepthToWorld( depthStream, x, y, pDepth[i], &fX, &fY, &fZ );
                if ( pColor != nullptr )
                {
                    imgColor[i][0] = pColor[i].r / 255.0;
                    imgColor[i][1] = pColor[i].g / 255.0;
                    imgColor[i][2] = pColor[i].b / 255.0;
                }
                colorMap( pDepth[i], &imgColorMap[i][0], &imgColorMap[i][1], &imgColorMap[i][2] );
            }
            imgDepth[i][0] = fX;
            imgDepth[i][1] = fY;
            imgDepth[i][2] = fZ;
            i++;
        }
    }
}

bool initGlfwSetting()
{
    //Initialize the library
    if ( !glfwInit() )
    {
        return false;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow( windowWidth, windowHeight, "Point Cloud", NULL, NULL );
    if ( !window )
    {
        glfwTerminate();
        return false;
    }

    // Make the window's context current
    glfwMakeContextCurrent( window );

    glfwSetMouseButtonCallback( window, mouse_button_callback );
    glfwSetScrollCallback( window, scroll_callback );
    glfwSetCursorPosCallback( window, cursor_position_callback );
    glfwSetKeyCallback( window, key_callback );
    return true;
}

void initHelpScreen()
{
    glGenTextures( 1, &helpScreenTextureID );
    glBindTexture( GL_TEXTURE_2D, helpScreenTextureID );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, helpScreenWidth, helpScreenHeight, GL_RGB, GL_UNSIGNED_BYTE, helpScreenData );
}

void initViewer()
{
    windowWidth = 1280;
    windowHeight = 720;
    viewerStat.yaw = 0.0;
    viewerStat.pitch = 0.0;
    viewerStat.lastX = 0.0;
    viewerStat.lastY = 0.0;
    viewerStat.offsetY = 0.0;
    viewerStat.lookatX = 0.0;
    viewerStat.lookatY = 0.0;
    viewerStat.mouseLeft = false;
    viewerStat.mouseRight = false;
    bComputeCloud = true;
    bQuit = false;
    bShowHelpScreen = true;
    bVisualizedRGBFrame = true;
}

int main( void )
{
    bool isColorValid = false;
    int resolution = 0;
    float **imgColor;
    float **imgDepth;
    float **imgColorMap;

    initViewer();

    if ( STATUS_OK != OpenNI::initialize() )
    {
        cout << "After initialization: " << OpenNI::getExtendedError() << endl << endl;
        return 1;
    }

    Device devDevice;
    if ( STATUS_OK != devDevice.open( ANY_DEVICE ) )
    {
        cout << "ERROR: Cannot open device: " << OpenNI::getExtendedError() << endl << endl;
        return 1;
    }

    VideoMode mode;
    VideoStream vsDepth;
    VideoStream vsColor;

    // Create and setup depth stream
    if ( STATUS_OK != vsDepth.create( devDevice, SENSOR_DEPTH ) )
    {
        cout << "ERROR: Cannot create depth stream on device" << endl << endl;
        return 1;
    }
    vsDepth.setMirroringEnabled( false );

    mode = vsDepth.getVideoMode();
    cout << "Depth VideoMode: " << mode.getResolutionX() << " x " << mode.getResolutionY() << " @ " << mode.getFps() << " FPS";
    cout << ", Unit is ";
    if ( mode.getPixelFormat() == PIXEL_FORMAT_DEPTH_1_MM )
    {
        cout << "1mm";
    }
    else if ( mode.getPixelFormat() == PIXEL_FORMAT_DEPTH_100_UM )
    {
        cout << "100um";
    }
    cout << endl;

    resolution = mode.getResolutionX() * mode.getResolutionY();

    imgColor = new float* [resolution];
    imgDepth = new float* [resolution];
    imgColorMap = new float* [resolution];

    for ( int i = 0; i < resolution; i++ )
    {
        imgColor[i] = new float [3];
        imgDepth[i] = new float [3];
        imgColorMap[i] = new float [3];
    }

    frameWidth = mode.getResolutionX();
    frameHeight = mode.getResolutionY();

    // Create and setup color stream
    if ( STATUS_OK != vsColor.create( devDevice, SENSOR_COLOR ) )
    {
        cout << "ERROR: Cannot create color stream on device" << endl << endl;
    }
    else
    {
        vsColor.setMirroringEnabled( false );
    }

    // Check and enable Depth-To-Color image registration
    if ( vsColor.isValid() )
    {
        if ( devDevice.isImageRegistrationModeSupported( IMAGE_REGISTRATION_DEPTH_TO_COLOR ) )
        {
            if ( STATUS_OK == devDevice.setImageRegistrationMode( IMAGE_REGISTRATION_DEPTH_TO_COLOR ) )
            {
                isColorValid = true;
            }
            else
            {
                cout << "ERROR: Failed to set imageRegistration mode" << endl << endl;
            }
        }
        else
        {
            cout << "ERROR: ImageRegistration mode is not supported" << endl << endl;
        }
    }

    // Start streams
    if ( STATUS_OK != vsDepth.start() )
    {
        cout << "ERROR: Cannot start depth stream on device" << endl << endl;
        return 1;
    }
    if ( isColorValid )
    {
        if ( STATUS_OK != vsColor.start() )
        {
            cout << "ERROR: Cannot start color stream on device" << endl << endl;
            return 1;
        }
    }

    if ( initGlfwSetting() == false )
    {
        return -1;
    }

    initHelpScreen();

    VideoFrameRef dFrame;
    VideoFrameRef cFrame;

    // Loop until the user closes the window
    while ( !glfwWindowShouldClose( window ) )
    {
        glfwGetFramebufferSize( window, &windowWidth, &windowHeight );
        glViewport( 0, 0, windowWidth, windowHeight );
        glClear( GL_COLOR_BUFFER_BIT );

        if ( bComputeCloud )
        {
            if ( vsDepth.isValid() )
            {
                if ( STATUS_OK != vsDepth.readFrame( &dFrame ) )
                {
                    break;
                }
            }

            if ( isColorValid && vsColor.isValid() )
            {
                if ( STATUS_OK != vsColor.readFrame( &cFrame ) )
                {
                    break;
                }
                niComputeCloud( vsDepth, ( const openni::DepthPixel* )dFrame.getData(), ( const openni::RGB888Pixel* )cFrame.getData(),
                                imgDepth, imgColor, imgColorMap );
            }
            else
            {
                niComputeCloud( vsDepth, ( const openni::DepthPixel* )dFrame.getData(), nullptr,
                                imgDepth, imgColor, imgColorMap );
            }
        }

        if ( isColorValid && bVisualizedRGBFrame )
        {
            drawPointCloud( imgColor, imgDepth, resolution );
        }
        else
        {
            drawPointCloud( imgColorMap, imgDepth, resolution );
        }

        if ( bShowHelpScreen )
        {
            drawHelpScreen();
        }

        if ( bQuit )
        {
            break;
        }

        // Swap front and back buffers
        glfwSwapBuffers( window );

        // Poll for and process events
        glfwPollEvents();
    }

    vsDepth.stop();
    vsDepth.destroy();
    if ( isColorValid )
    {
        vsColor.stop();
        vsColor.destroy();
    }
    devDevice.close();
    OpenNI::shutdown();

    for ( int i = 0; i < resolution; i++ )
    {
        delete [] imgColor[i];
        delete [] imgColorMap[i];
        delete [] imgDepth[i];
    }
    delete [] imgColor;
    delete [] imgColorMap;
    delete [] imgDepth;

    glfwTerminate();
    return 0;
}