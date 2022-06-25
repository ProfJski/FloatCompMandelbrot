#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include <limits>

#include "raylib.h"
// Info on standard boost::mp and number class functions:
// https://www.boost.org/doc/libs/1_76_0/libs/multiprecision/doc/html/boost_multiprecision/ref/backendconc.html
#define BOOST_MP_DISABLE_DEPRECATE_03_WARNING 1 //Get rid of warning
#include "boost/multiprecision/cpp_bin_float.hpp"

using namespace std;
using namespace boost::multiprecision;
using namespace std::chrono_literals; //For 0s to pass to futures.wait_for()

//Convenience type definitions for various levels of precision for cpp_bin_float
typedef number<backends::cpp_bin_float<24, backends::digit_base_2, void, std::int16_t, -126, 127>, et_off>         cpp_bin_float_single;
typedef number<backends::cpp_bin_float<53, backends::digit_base_2, void, std::int16_t, -1022, 1023>, et_off>       cpp_bin_float_double;
typedef number<backends::cpp_bin_float<64, backends::digit_base_2, void, std::int16_t, -16382, 16383>, et_off>     cpp_bin_float_double_extended;
typedef number<backends::cpp_bin_float<113, backends::digit_base_2, void, std::int16_t, -16382, 16383>, et_off>    cpp_bin_float_quad;
typedef number<backends::cpp_bin_float<237, backends::digit_base_2, void, std::int32_t, -262142, 262143>, et_off>  cpp_bin_float_oct;
typedef number<backends::cpp_bin_float<500, backends::digit_base_2, void, std::int32_t, -262142, 262143>, et_off>  cpp_bin_float_500;
typedef number<backends::cpp_bin_float<1000, backends::digit_base_2, void, std::int32_t, -262142, 262143>, et_off> cpp_bin_float_1000;

// OPTION: Set your floating point types below.  Varies cpp_bin_float types have been defined above for convenience.
typedef double first_float;
typedef cpp_bin_float_quad second_float;
typedef cpp_bin_float_oct third_float;

//For pixel-based screen locations in RayLib
struct int_vector2 {
    int x;
    int y;
};

//Simple complex number types with only the operations that we need for the Mandelbrot set
class MyComplex_first_float {
public:
    first_float real;
    first_float imag;
    MyComplex_first_float(){
        real=0;
        imag=0;
    }
    MyComplex_first_float(first_float r, first_float i){
        real=r;
        imag=i;
    }
    MyComplex_first_float operator+(const MyComplex_first_float &rhs) {
        MyComplex_first_float r;
        r.real=this->real+rhs.real;
        r.imag=this->imag+rhs.imag;
        return r;
    }
    MyComplex_first_float operator*(const MyComplex_first_float &rhs) {
        MyComplex_first_float r;                                    // (a+bi)*(c+di) = (ac-bd)+(ad+bc)i
        r.real=(this->real)*rhs.real-(this->imag)*rhs.imag;
        r.imag=(this->real)*rhs.imag+(this->imag)*rhs.real;
        return r;
    }
    MyComplex_first_float Squared() {
        MyComplex_first_float r;
        r.real=(this->real)*(this->real)-(this->imag)*(this->imag); // a^2-b^2
        r.imag=(this->real)*(this->imag)+(this->real)*(this->imag); // 2abi
        return r;
    }
    first_float Magnitude() {
        first_float r;
        r=(this->real)*(this->real)+(this->imag)*(this->imag);
        return r;
    }
};

class MyComplex_second_float {
public:
    second_float real;
    second_float imag;
    MyComplex_second_float(){
        real=0;
        imag=0;
    }
    MyComplex_second_float(second_float r, second_float i){
        real=r;
        imag=i;
    }
    MyComplex_second_float(MyComplex_first_float f) {
        real=second_float(f.real);
        imag=second_float(f.imag);
    }
    MyComplex_second_float operator+(const MyComplex_second_float &rhs) {
        MyComplex_second_float r;
        r.real=this->real+rhs.real;
        r.imag=this->imag+rhs.imag;
        return r;
    }
    MyComplex_second_float operator*(const MyComplex_second_float &rhs) {
        MyComplex_second_float r;                                    // (a+bi)*(c+di) = (ac-bd)+(ad+bc)i
        r.real=(this->real)*rhs.real-(this->imag)*rhs.imag;
        r.imag=(this->real)*rhs.imag+(this->imag)*rhs.real;
        return r;
    }
    MyComplex_second_float Squared() {
        MyComplex_second_float r;
        r.real=(this->real)*(this->real)-(this->imag)*(this->imag); // a^2-b^2
        r.imag=(this->real)*(this->imag)+(this->real)*(this->imag); // 2abi
        return r;
    }
    second_float Magnitude() {
        second_float r;
        r=(this->real)*(this->real)+(this->imag)*(this->imag);
        return r;
    }
};

class MyComplex_third_float {
public:
    third_float real;
    third_float imag;
    MyComplex_third_float(){
        real=0;
        imag=0;
    }
    MyComplex_third_float(third_float r, third_float i){
        real=r;
        imag=i;
    }
    MyComplex_third_float(MyComplex_first_float f){
        real=third_float(f.real);
        imag=third_float(f.imag);
    }
    MyComplex_third_float operator+(const MyComplex_third_float &rhs) {
        MyComplex_third_float r;
        r.real=this->real+rhs.real;
        r.imag=this->imag+rhs.imag;
        return r;
    }
    MyComplex_third_float operator*(const MyComplex_third_float &rhs) {
        MyComplex_third_float r;                                    // (a+bi)*(c+di) = (ac-bd)+(ad+bc)i
        r.real=(this->real)*rhs.real-(this->imag)*rhs.imag;
        r.imag=(this->real)*rhs.imag+(this->imag)*rhs.real;
        return r;
    }
    MyComplex_third_float Squared() {
        MyComplex_third_float r;
        r.real=(this->real)*(this->real)-(this->imag)*(this->imag); // a^2-b^2
        r.imag=(this->real)*(this->imag)+(this->real)*(this->imag); // 2abi
        return r;
    }
    third_float Magnitude() {
        third_float r;
        r=(this->real)*(this->real)+(this->imag)*(this->imag);
        return r;
    }
};

std::ostream& operator<< (std::ostream& os, MyComplex_first_float f) {
    os<<"( "<<f.real<<" + "<<f.imag<<" i)";
    return os;
}

std::ostream& operator<< (std::ostream& os, MyComplex_second_float f) {
    os<<"( "<<f.real<<" + "<<f.imag<<" i)";
    return os;
}

std::ostream& operator<< (std::ostream& os, MyComplex_third_float f) {
    os<<"( "<<f.real<<" + "<<f.imag<<" i)";
    return os;
}

//Our fundamental class for relating int_vector2 screen coordinates to the values they represent and storing the iterations calculated for each
class PointToCalc_first_float {
public:
    int_vector2 gridPos; //Grid position for plotting
    MyComplex_first_float coord;
    int iterations;
};

class PointToCalc_second_float {
public:
    int_vector2 gridPos; //Grid position for plotting
    MyComplex_second_float coord;
    int iterations;
};

class PointToCalc_third_float {
public:
    int_vector2 gridPos; //Grid position for plotting
    MyComplex_third_float coord;
    int iterations;
};

//Pairs of iterators to the above types for indicating start and end points of batches of work for the multithreaded functions that compute the iterations
struct iterPair_first_float {
    vector<PointToCalc_first_float>::iterator startIter;
    vector<PointToCalc_first_float>::iterator endIter;
};

struct iterPair_second_float {
    vector<PointToCalc_second_float>::iterator startIter;
    vector<PointToCalc_second_float>::iterator endIter;
};

struct iterPair_third_float {
    vector<PointToCalc_third_float>::iterator startIter;
    vector<PointToCalc_third_float>::iterator endIter;
};

//TODO: Use of 1ULL will limit depth of zoom to 2^64.
//Takes a screen coordinate and transforms it to a complex type of our selected precision
MyComplex_first_float Grid2Coord_first_float(int_vector2 gc, MyComplex_first_float &coordCenter, unsigned int &zoomDenomPower) {
    MyComplex_first_float p;
    first_float d=(first_float)(1ULL<<zoomDenomPower);
    p.real=coordCenter.real+((first_float)(gc.x-512))/d;
    p.imag=coordCenter.imag+((first_float)(512-gc.y))/d;
    return p;
}

MyComplex_second_float Grid2Coord_second_float(int_vector2 gc, MyComplex_second_float &coordCenter, unsigned int &zoomDenomPower) {
    MyComplex_second_float p;
    second_float d=(second_float)(1ULL<<zoomDenomPower);
    p.real=coordCenter.real+((second_float)(gc.x-512))/d;
    p.imag=coordCenter.imag+((second_float)(512-gc.y))/d;
    return p;
}

MyComplex_third_float Grid2Coord_third_float(int_vector2 gc, MyComplex_third_float &coordCenter, unsigned int &zoomDenomPower) {
    MyComplex_third_float p;
    third_float d=(third_float)(1ULL<<zoomDenomPower);
    p.real=coordCenter.real+((third_float)(gc.x-512))/d;
    p.imag=coordCenter.imag+((third_float)(512-gc.y))/d;
    return p;
}

//Inverse function to the above.  Transforms complex type to screen coordinate.  Only used for plotting point trajectories in screen 8 view
int_vector2 Coord2Grid_first_float(MyComplex_first_float c){
    first_float xf=c.real*256.0;
    first_float yf=c.imag*256.0;
    int_vector2 gc;
    gc.x=int(xf)+512;
    gc.y=512-int(yf);
    return gc;
}

int_vector2 Coord2Grid_second_float(MyComplex_second_float c){
    second_float xf=c.real*256.0;
    second_float yf=c.imag*256.0;
    int_vector2 gc;
    gc.x=int(xf)+512;
    gc.y=512-int(yf);
    return gc;
}

int_vector2 Coord2Grid_third_float(MyComplex_third_float c){
    third_float xf=c.real*256.0;
    third_float yf=c.imag*256.0;
    int_vector2 gc;
    gc.x=int(xf)+512;
    gc.y=512-int(yf);
    return gc;
}

inline bool mouseIsInBounds(Vector2 m) {
    return ( (m.x>1)&&(m.x<1024)&&(m.y>1)&&(m.y<1024) );
}

//Core function to calculate iteration value for just one point
void msetPoint_first_float(vector<PointToCalc_first_float>::iterator pit, int maxIterations) {
    MyComplex_first_float c;
    first_float maxMagnitude=first_float(4.0);
    int iterationCounter=0;

    while ( (c.Magnitude()<maxMagnitude) && (iterationCounter<maxIterations) ) {
        c=c.Squared()+pit->coord;
        iterationCounter++;
    }

    if (iterationCounter<maxIterations) {  //We have bounded out
            pit->iterations=iterationCounter;
            return;
    }
    //Otherwise, we've hit max iter, return zero
    pit->iterations=0;
    return;
}

void msetPoint_second_float(vector<PointToCalc_second_float>::iterator pit, int maxIterations) {
    MyComplex_second_float c;
    second_float maxMagnitude=second_float(4.0);
    int iterationCounter=0;

    while ( (c.Magnitude()<maxMagnitude) && (iterationCounter<maxIterations) ) {
        c=c.Squared()+pit->coord;
        iterationCounter++;
    }

    if (iterationCounter<maxIterations) {
            pit->iterations=iterationCounter;
            return;
    }
    pit->iterations=0;
    return;
}

void msetPoint_third_float(vector<PointToCalc_third_float>::iterator pit, int maxIterations) {
    MyComplex_third_float c;
    third_float maxMagnitude=third_float(4.0);
    int iterationCounter=0;

    while ( (c.Magnitude()<maxMagnitude) && (iterationCounter<maxIterations) ) {
        c=c.Squared()+pit->coord;
        iterationCounter++;
    }

    if (iterationCounter<maxIterations) {
            pit->iterations=iterationCounter;
            return;
    }
    pit->iterations=0;
    return;
}

//Wraps the above functions to work with start-end pairs of iterators.  The same iterators are returned because with std::async execution and future.get(), work is not necessarily returned in the same order that
//we submitted it, so we need to know what batch of pixels was just completed.  We only draw newly-computed pixels to their respective screen textures, rather than plotting 3x1024x1024 pixels every frame
iterPair_first_float msetBatch_first_float(iterPair_first_float vit, int maxIterations) {
    for (vector<PointToCalc_first_float>::iterator it=vit.startIter; it!=vit.endIter; it++) {
        msetPoint_first_float(it,maxIterations);
    }
return vit;
}

iterPair_second_float msetBatch_second_float(iterPair_second_float vit, int maxIterations) {
    for (vector<PointToCalc_second_float>::iterator it=vit.startIter; it!=vit.endIter; it++) {
        msetPoint_second_float(it,maxIterations);
    }
return vit;
}

iterPair_third_float msetBatch_third_float(iterPair_third_float vit, int maxIterations) {
    for (vector<PointToCalc_third_float>::iterator it=vit.startIter; it!=vit.endIter; it++) {
        msetPoint_third_float(it,maxIterations);
    }
return vit;
}

//These functions return a list of points for plotting the trajectory of a point in screen 8 display mode.  Shows how different precision give rise to different trajectories,
//which in turn gives rise to different iteration values for different precisions
vector<int_vector2> msetPath_first_float(MyComplex_first_float c, int maxIterations) {
    MyComplex_first_float z;
    vector<int_vector2> points;
    first_float maxMagnitude=first_float(4.0);
    int iterationCounter=0;

    while ( (z.Magnitude()<maxMagnitude) && (iterationCounter<maxIterations) ) {
        z=z.Squared()+c;
        iterationCounter++;
        points.push_back(Coord2Grid_first_float(z));
    }
return points;
}

vector<int_vector2> msetPath_second_float(MyComplex_second_float c, int maxIterations) {
    MyComplex_second_float z;
    vector<int_vector2> points;
    second_float maxMagnitude=second_float(4.0);
    int iterationCounter=0;

    while ( (z.Magnitude()<maxMagnitude) && (iterationCounter<maxIterations) ) {
        z=z.Squared()+c;
        iterationCounter++;
        points.push_back(Coord2Grid_second_float(z));
    }
return points;
}

vector<int_vector2> msetPath_third_float(MyComplex_third_float c, int maxIterations) {
    MyComplex_third_float z;
    vector<int_vector2> points;
    third_float maxMagnitude=third_float(4.0);
    int iterationCounter=0;

    while ( (z.Magnitude()<maxMagnitude) && (iterationCounter<maxIterations) ) {
        z=z.Squared()+c;
        iterationCounter++;
        points.push_back(Coord2Grid_third_float(z));
    }
return points;
}

//TODO: Get a more attractive palette function
//Iterations-to-color for main image
Color colorizer(int iter) {
    if (iter<=0) return BLACK;
    Color c;
    c.a=255;
    c.r=64+12*(iter%16);
    c.g=8*((iter/16)%32);
    c.b=16*((iter/512)%32);
return c;
}

//Returns color used when subtracting two images, using only red, green and black
Color colorizePixelDiff(int i) {
    Color c=BLACK;
    if (i>0) {
        c.g=(unsigned char)(128+31*(i%4));
    }
    if (i<0) {
        i=-i;
        c.r=(unsigned char)(128+31*(i%4));;
    }
return c;
}

//For raylib DrawText function
char* PrintNum(char* c, int n) {
    snprintf(c,10,"%i",n);
return c;
}

int main()
{

    Vector2 mousePos;
    int_vector2 gridPos;
    unsigned int zoomDenomPower=8;
    MyComplex_first_float coordCenter_first_float;
    MyComplex_second_float coordCenter_second_float;
    MyComplex_third_float coordCenter_third_float;
    MyComplex_first_float oldCoordCenter_first_float;
    MyComplex_second_float oldCoordCenter_second_float;
    MyComplex_third_float oldCoordCenter_third_float;
    int maxIterations=100;
    bool doNewCalc=false;
    bool recalcSamePoints=false;
    bool pauseCalc=false;
    bool eraseScreen=true;
    bool highlightCenterOfScreen=false;
    bool showGrid=false;
    bool calcDifference=false;
    bool drawThreePaths=false;
    char smallText[40];
    unsigned int displayScreen=1;

    // OPTION: Increase or decrease numThreads for your system.  Note: numThreads*3 threads are launched initially because numThreads threads runs per type.
    unsigned int numThreads=5;

    // OPTION: Change batch size for the three types here.
    unsigned int batchSize_first_float=10240;
    unsigned int batchSize_second_float=5120;
    unsigned int batchSize_third_float=1024;

    //Initialize arrays to hold our points to calculate for each type
    vector<PointToCalc_first_float> pointsToCalc_first_float(1024*1024);
    vector<PointToCalc_second_float> pointsToCalc_second_float(1024*1024);
    vector<PointToCalc_third_float> pointsToCalc_third_float(1024*1024);

    //Initialize arrays that hold differences of iteration value between the above.  2M1 = type 2 - type 1, etc.
    vector<int>differentPixels2M1(1024*1024);
    vector<int>differentPixels3M1(1024*1024);
    vector<int>differentPixels3M2(1024*1024);

    cout<<setprecision(33);
    //Print our numeric limits for our types
    cout<<"Limits of types:"<<endl;
    cout<<"1st float type:\n\tRadix digits: "<<std::numeric_limits<first_float>::digits<<"\tBase 10 digits: "<<std::numeric_limits<first_float>::digits10<<"\n\tepsilon: "<<std::numeric_limits<first_float>::epsilon()<<"\tmin: "<<std::numeric_limits<first_float>::min()<<"\tMin exponent: "<<std::numeric_limits<first_float>::min_exponent<<"\n\n";
    cout<<"2nd float type:\n\tRadix digits: "<<std::numeric_limits<second_float>::digits<<"\tBase 10 digits: "<<std::numeric_limits<second_float>::digits10<<"\n\tepsilon: "<<std::numeric_limits<second_float>::epsilon()<<"\tmin: "<<std::numeric_limits<second_float>::min()<<"\tMin exponent: "<<std::numeric_limits<second_float>::min_exponent<<"\n\n";
    cout<<"3rd float type:\n\tRadix digits: "<<std::numeric_limits<third_float>::digits<<"\tBase 10 digits: "<<std::numeric_limits<third_float>::digits10<<"\n\tepsilon: "<<std::numeric_limits<third_float>::epsilon()<<"\tmin: "<<std::numeric_limits<third_float>::min()<<"\tMin exponent: "<<std::numeric_limits<third_float>::min_exponent<<"\n\n";
    cout<<"Iteration difference type: Base 10 digits: "<<std::numeric_limits<int>::digits10<<" Max: "<<(int)std::numeric_limits<int>::max()<<endl;

    //Initialize first display
    cout<<"Initializing point list"<<endl;
    for (gridPos.y=0;gridPos.y<1024;gridPos.y++) {
        for (gridPos.x=0;gridPos.x<1024;gridPos.x++) {
            pointsToCalc_first_float[1024*gridPos.y+gridPos.x].gridPos=gridPos;
            pointsToCalc_first_float[1024*gridPos.y+gridPos.x].coord=Grid2Coord_first_float(gridPos,coordCenter_first_float,zoomDenomPower);
            pointsToCalc_first_float[1024*gridPos.y+gridPos.x].iterations=0;

            pointsToCalc_second_float[1024*gridPos.y+gridPos.x].gridPos=gridPos;
            pointsToCalc_second_float[1024*gridPos.y+gridPos.x].iterations=0;

            pointsToCalc_third_float[1024*gridPos.y+gridPos.x].gridPos=gridPos;
            pointsToCalc_third_float[1024*gridPos.y+gridPos.x].iterations=0;

            // OPTION: Select whether higher-precision types calculate starting point C (in Z=Z^2+C) using their own precision, or whether they cast the result of the first type's calculation, retaining its more limited precision
            // Relevant when lower-precision floats lose accuracy with higher zooms.  Casting the value ensures that all three float types start with the same initial value for point C,
            // and thus any subsequent divergence in iteration value is from accumulated error due to the types themselves and not their slightly different starting values.
            // Not casting means that all three types may start with slightly different values for C depending on the depth of zoom.
            // NOTE: You must change the same set of statements in the doNewCalc routine to match!  This section of code only affects the initial image.
            pointsToCalc_second_float[1024*gridPos.y+gridPos.x].coord=Grid2Coord_second_float(gridPos,coordCenter_second_float,zoomDenomPower);  // Calculate second type's starting point using second type's precision
            //pointsToCalc_second_float[1024*gridPos.y+gridPos.x].coord=MyComplex_second_float(Grid2Coord_first_float(gridPos,coordCenter_first_float,zoomDenomPower));  // Second type's starting point casts the first type's result, keeping its more limited precision

            pointsToCalc_third_float[1024*gridPos.y+gridPos.x].coord=Grid2Coord_third_float(gridPos,coordCenter_third_float,zoomDenomPower);  // Calculate third type's starting point in third type's precision
            //pointsToCalc_third_float[1024*gridPos.y+gridPos.x].coord=MyComplex_third_float(Grid2Coord_first_float(gridPos,coordCenter_first_float,zoomDenomPower)); // Third type's starting point casts the first type's result, keeping its more limited precision
        }
    }

    //Initialize pointers for batches of work
    iterPair_first_float batchIter_first_float;
    batchIter_first_float.startIter=pointsToCalc_first_float.begin();
    batchIter_first_float.endIter=batchIter_first_float.startIter+batchSize_first_float;
    iterPair_first_float drawIter_first_float;
    drawIter_first_float.startIter=pointsToCalc_first_float.begin();
    drawIter_first_float.endIter=pointsToCalc_first_float.begin(); //set to same as above because nothing to draw initially

    iterPair_second_float batchIter_second_float;
    batchIter_second_float.startIter=pointsToCalc_second_float.begin();
    batchIter_second_float.endIter=batchIter_second_float.startIter+batchSize_second_float;
    iterPair_second_float drawIter_second_float;
    drawIter_second_float.startIter=pointsToCalc_second_float.begin();
    drawIter_second_float.endIter=pointsToCalc_second_float.begin();

    iterPair_third_float batchIter_third_float;
    batchIter_third_float.startIter=pointsToCalc_third_float.begin();
    batchIter_third_float.endIter=batchIter_third_float.startIter+batchSize_third_float;
    iterPair_third_float drawIter_third_float;
    drawIter_third_float.startIter=pointsToCalc_third_float.begin();
    drawIter_third_float.endIter=pointsToCalc_third_float.begin();

    //These vectors store pairs of pointers returned from future.get(), representing newly-computed ranges of points, which are then drawn to RenderTextures to update the image
    vector<iterPair_first_float> rangesToDraw_first_float;
    vector<iterPair_second_float> rangesToDraw_second_float;
    vector<iterPair_third_float> rangesToDraw_third_float;

    //Compute basic stats about the work remaining for on-screen progress display
    unsigned int numPointsFinished=0, numPointsUpdated=0, numPointsToCalc;
    numPointsToCalc=pointsToCalc_first_float.size()+pointsToCalc_second_float.size()+pointsToCalc_third_float.size();

    //Spin up threads
    std::future<iterPair_first_float> futures_first_float[numThreads];
    std::future<iterPair_second_float> futures_second_float[numThreads];
    std::future<iterPair_third_float> futures_third_float[numThreads];


    for (unsigned int t=0;t<numThreads;t++) {
        futures_first_float[t]=std::async(msetBatch_first_float,batchIter_first_float,maxIterations);
    }

    for (unsigned int t=0;t<numThreads;t++) {
        futures_second_float[t]=std::async(msetBatch_second_float,batchIter_second_float,maxIterations);
    }

    for (unsigned int t=0;t<numThreads;t++) {
        futures_third_float[t]=std::async(msetBatch_third_float,batchIter_third_float,maxIterations);
    }

    //Initialize RayLib display and RenderTextures
    const int screenWidth = 1200;
    const int screenHeight = 1024;
    InitWindow(screenWidth, screenHeight, "");
    SetTargetFPS(30);
    RenderTexture2D msetRenderTexture_first_float=LoadRenderTexture(1024,1024);
    RenderTexture2D msetRenderTexture_second_float=LoadRenderTexture(1024,1024);
    RenderTexture2D msetRenderTexture_third_float=LoadRenderTexture(1024,1024);
    RenderTexture2D diffScreen2M1=LoadRenderTexture(1024,1024);
    RenderTexture2D diffScreen3M1=LoadRenderTexture(1024,1024);
    RenderTexture2D diffScreen3M2=LoadRenderTexture(1024,1024);
    RenderTexture2D pathDrawings=LoadRenderTexture(1024,1024);
    RenderTexture2D msetCanvas=LoadRenderTexture(1024,1024);

    //Pre-draw Mandelbrot set on (-2,2) to (2,2) on msetCanvas to make a background for pathDrawings.
    //The addition of this code makes the first pass on the first float type redundant
    //Only used on display screen mode 7.  For screen mode 8, it's easier to find interesting points with the zoomed-in image as background.
    //TODO: Perhaps omit this and screen mode 7 entirely?
    BeginTextureMode(msetCanvas);
    for (vector<PointToCalc_first_float>::iterator vit=pointsToCalc_first_float.begin();vit!=pointsToCalc_first_float.end();vit++){
        msetPoint_first_float(vit,maxIterations);
        DrawPixel(vit->gridPos.x,vit->gridPos.y,colorizer(vit->iterations));
    }
    EndTextureMode();

    //Initialize vectors which hold paths to plot for screen modes 7 and 8 where the trajectory of a point is plotted
    vector<int_vector2> pathPoints_first_float;
    vector<int_vector2> pathPoints_second_float;
    vector<int_vector2> pathPoints_third_float;
    MyComplex_first_float firstPathPoint_first_float;
    MyComplex_second_float firstPathPoint_second_float;
    MyComplex_third_float firstPathPoint_third_float;

    //For drawing textures to screen.  FlipRec has a final value that is negative to flip the Y axis from screen coordinates (+Y is down) to Cartesian (+Y is up)
    Rectangle flipRec=(Rectangle){0.0f,0.0f,1024.0f,-1024.0f};  //Flips Y-axis
    Vector2 origin=(Vector2){0.0f,0.0f};

// Begin main loop
while (!WindowShouldClose())    // Detect window close button or ESC key
{
    // Detect user input
    mousePos=GetMousePosition();
    if (mouseIsInBounds(mousePos)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) ) {
                oldCoordCenter_first_float=coordCenter_first_float;
                oldCoordCenter_second_float=coordCenter_second_float;
                oldCoordCenter_third_float=coordCenter_third_float;
                coordCenter_first_float=Grid2Coord_first_float({(int)mousePos.x,(int)mousePos.y},coordCenter_first_float,zoomDenomPower);
                coordCenter_second_float=Grid2Coord_second_float({(int)mousePos.x,(int)mousePos.y},coordCenter_second_float,zoomDenomPower);
                coordCenter_third_float=Grid2Coord_third_float({(int)mousePos.x,(int)mousePos.y},coordCenter_third_float,zoomDenomPower);
                cout<<"Mouse x:"<<mousePos.x<<" y:"<<mousePos.y<<" New center coord x:"<<coordCenter_first_float.real<<" y:"<<coordCenter_first_float.imag<<endl;
                doNewCalc=true;
            }
        if (IsKeyPressed(KEY_SLASH)) {
            MyComplex_first_float queriedPoint=Grid2Coord_first_float({(int)mousePos.x,(int)mousePos.y},coordCenter_first_float,zoomDenomPower);
            //cout<<setprecision(33);
            cout<<"Location as type 1: "<<queriedPoint<<endl;
            cout<<"Recast as type 2: "<<MyComplex_second_float(queriedPoint)<<endl;
            cout<<"Recast as type 3: "<<MyComplex_third_float(queriedPoint)<<endl;
            //cout<<setprecision(16);
        }
        if (IsKeyDown(KEY_SEVEN)) {
                calcDifference=true; pauseCalc=true; displayScreen=7; drawThreePaths=true;
                firstPathPoint_first_float=Grid2Coord_first_float({(int)mousePos.x,(int)mousePos.y},coordCenter_first_float,zoomDenomPower);
                firstPathPoint_second_float=Grid2Coord_second_float({(int)mousePos.x,(int)mousePos.y},coordCenter_second_float,zoomDenomPower);
                firstPathPoint_third_float=Grid2Coord_third_float({(int)mousePos.x,(int)mousePos.y},coordCenter_third_float,zoomDenomPower);
        }
        if (IsKeyDown(KEY_EIGHT)) {
                calcDifference=true; pauseCalc=true; displayScreen=8; drawThreePaths=true;
                firstPathPoint_first_float=Grid2Coord_first_float({(int)mousePos.x,(int)mousePos.y},coordCenter_first_float,zoomDenomPower);
                firstPathPoint_second_float=Grid2Coord_second_float({(int)mousePos.x,(int)mousePos.y},coordCenter_second_float,zoomDenomPower);
                firstPathPoint_third_float=Grid2Coord_third_float({(int)mousePos.x,(int)mousePos.y},coordCenter_third_float,zoomDenomPower);
        }
    }

    //TODO:  1<<zoomDenomPower needs to be CPP_INT(1)<<zDP or else overflows when zDP>64
    if (IsKeyDown(KEY_PERIOD)) { zoomDenomPower++; doNewCalc=true; cout<<"Increasing denom power to "<<zoomDenomPower<<" = 1/"<<(1ULL<<zoomDenomPower)<<endl;}
    if (IsKeyDown(KEY_SEMICOLON)) { zoomDenomPower+=3; doNewCalc=true; cout<<"Increasing denom power to "<<zoomDenomPower<<" = 1/"<<(1ULL<<zoomDenomPower)<<endl;}
    if (IsKeyDown(KEY_COMMA)) { zoomDenomPower--; doNewCalc=true; if (zoomDenomPower<6) zoomDenomPower=6; cout<<"Decreasing denom power to "<<zoomDenomPower<<" = 1/"<<(1ULL<<zoomDenomPower)<<endl;}
    if (IsKeyDown(KEY_M)) {maxIterations++; recalcSamePoints=true; cout<<"Increasing Max Iter to "<<maxIterations<<endl;}
    if (IsKeyDown(KEY_N)) {maxIterations--; if (maxIterations<2) maxIterations=2; recalcSamePoints=true; cout<<"Decreasing Max Iter to "<<maxIterations<<endl;}
    if (IsKeyDown(KEY_K)) {maxIterations+=10; recalcSamePoints=true; cout<<"Increasing Max Iter to "<<maxIterations<<endl;}
    if (IsKeyDown(KEY_J)) {maxIterations-=10; if (maxIterations<2) maxIterations=2; recalcSamePoints=true; cout<<"Decreasing Max Iter to "<<maxIterations<<endl;}
    if (IsKeyDown(KEY_O)) {
        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
            maxIterations=((maxIterations/1000)+1)*1000; recalcSamePoints=true; cout<<"Increasing Max Iter to "<<maxIterations<<endl; //Increase to next even thousand
        }
        else {
            maxIterations+=100; recalcSamePoints=true; cout<<"Increasing Max Iter to "<<maxIterations<<endl;
        }
    }
    if (IsKeyDown(KEY_I)) {
        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
            maxIterations=((maxIterations/1000)-1)*1000; if (maxIterations<2) maxIterations=2;
            recalcSamePoints=true; cout<<"Increasing Max Iter to "<<maxIterations<<endl; //Increase to next even thousand
        }
        else {
            maxIterations-=100; if (maxIterations<2) maxIterations=2; recalcSamePoints=true; cout<<"Decreasing Max Iter to "<<maxIterations<<endl;
        }
    }
    if (IsKeyPressed(KEY_P)) pauseCalc=!pauseCalc;
    if (IsKeyPressed(KEY_C)) highlightCenterOfScreen=!highlightCenterOfScreen;
    if (IsKeyPressed(KEY_G)) showGrid=!showGrid;
    if (IsKeyDown(KEY_R)) {
        cout<<"Reverting to previous center of x:"<<oldCoordCenter_first_float.real<<" y:"<<oldCoordCenter_first_float.imag<<endl;
        coordCenter_first_float=oldCoordCenter_first_float;
        coordCenter_second_float=oldCoordCenter_second_float;
        coordCenter_third_float=oldCoordCenter_third_float;
        doNewCalc=true;
    }
    if (IsKeyDown(KEY_ONE)) displayScreen=1;
    if (IsKeyDown(KEY_TWO)) displayScreen=2;
    if (IsKeyDown(KEY_THREE)) displayScreen=3;
    if (IsKeyDown(KEY_FOUR)) { calcDifference=true; pauseCalc=true; displayScreen=4; } // Second - first
    if (IsKeyDown(KEY_FIVE)) { calcDifference=true; pauseCalc=true; displayScreen=5; } // Third - first
    if (IsKeyDown(KEY_SIX)) { calcDifference=true; pauseCalc=true; displayScreen=6; }  // Third - second
    if (IsKeyDown(KEY_U)) { pauseCalc=false; displayScreen=1; }  //Unpause
    if (IsKeyDown(KEY_D)) { //Reprint type data
        cout<<"Limits of types:"<<endl;
        cout<<"1st float type:\n\tRadix digits: "<<std::numeric_limits<first_float>::digits<<"\tBase 10 digits: "<<std::numeric_limits<first_float>::digits10<<"\n\tepsilon: "<<std::numeric_limits<first_float>::epsilon()<<"\tmin: "<<std::numeric_limits<first_float>::min()<<"\tMin exponent: "<<std::numeric_limits<first_float>::min_exponent<<"\n\n";
        cout<<"2nd float type:\n\tRadix digits: "<<std::numeric_limits<second_float>::digits<<"\tBase 10 digits: "<<std::numeric_limits<second_float>::digits10<<"\n\tepsilon: "<<std::numeric_limits<second_float>::epsilon()<<"\tmin: "<<std::numeric_limits<second_float>::min()<<"\tMin exponent: "<<std::numeric_limits<second_float>::min_exponent<<"\n\n";
        cout<<"3rd float type:\n\tRadix digits: "<<std::numeric_limits<third_float>::digits<<"\tBase 10 digits: "<<std::numeric_limits<third_float>::digits10<<"\n\tepsilon: "<<std::numeric_limits<third_float>::epsilon()<<"\tmin: "<<std::numeric_limits<third_float>::min()<<"\tMin exponent: "<<std::numeric_limits<third_float>::min_exponent<<"\n\n";
        cout<<"Iteration difference type: Base 10 digits: "<<std::numeric_limits<int>::digits10<<" Max: "<<(int)std::numeric_limits<int>::max()<<endl;
    }

    //If we have changed the center point or zoomed in, recalculate PointToCalc for all 3x1024x1024 pixels, which is slow.  We also reset all pointers and stats.
    if (doNewCalc) {
        cout<<"Waiting for old threads to finish..."<<std::flush;
        for (unsigned int t=0;t<numThreads;t++) {
            if (futures_first_float[t].valid()) futures_first_float[t].get();
            if (futures_second_float[t].valid()) futures_second_float[t].get();
            if (futures_third_float[t].valid()) futures_third_float[t].get();
        }
        cout<<"Threads terminated."<<endl;
        cout<<"Resetting points to plot... "<<std::flush;
        eraseScreen=true;
        for (gridPos.y=0;gridPos.y<1024;gridPos.y++) {
            for (gridPos.x=0;gridPos.x<1024;gridPos.x++) {
                pointsToCalc_first_float[1024*gridPos.y+gridPos.x].gridPos=gridPos;
                pointsToCalc_first_float[1024*gridPos.y+gridPos.x].coord=Grid2Coord_first_float(gridPos,coordCenter_first_float,zoomDenomPower);
                pointsToCalc_first_float[1024*gridPos.y+gridPos.x].iterations=0;

                pointsToCalc_second_float[1024*gridPos.y+gridPos.x].gridPos=gridPos;
                pointsToCalc_second_float[1024*gridPos.y+gridPos.x].iterations=0;
                pointsToCalc_third_float[1024*gridPos.y+gridPos.x].iterations=0;
                pointsToCalc_third_float[1024*gridPos.y+gridPos.x].gridPos=gridPos;

                // OPTION: Change these sets of statements to match initialization routine! (See note above)
                pointsToCalc_second_float[1024*gridPos.y+gridPos.x].coord=Grid2Coord_second_float(gridPos,coordCenter_second_float,zoomDenomPower);  // Calculate second type's starting point using second type's precision
                //pointsToCalc_second_float[1024*gridPos.y+gridPos.x].coord=MyComplex_second_float(Grid2Coord_first_float(gridPos,coordCenter_first_float,zoomDenomPower));  // Second type's starting point casts first type's result
                pointsToCalc_third_float[1024*gridPos.y+gridPos.x].coord=Grid2Coord_third_float(gridPos,coordCenter_third_float,zoomDenomPower);  // Calculate third type's starting point using third type's precision
                //pointsToCalc_third_float[1024*gridPos.y+gridPos.x].coord=MyComplex_third_float(Grid2Coord_first_float(gridPos,coordCenter_first_float,zoomDenomPower));  // Third type's starting point casts first type's result
            }
        }
        doNewCalc=false;
        cout<<"done."<<endl;
        numPointsToCalc=pointsToCalc_first_float.size()+pointsToCalc_second_float.size()+pointsToCalc_third_float.size();
        numPointsFinished=0;
        numPointsUpdated=0;
        batchIter_first_float.startIter=pointsToCalc_first_float.begin();
        batchIter_first_float.endIter=pointsToCalc_first_float.begin(); //Thus because the next available future will start at last batchIter.endIter and go batchSize further
        drawIter_first_float.startIter=pointsToCalc_first_float.begin();
        drawIter_first_float.endIter=pointsToCalc_first_float.begin();

        batchIter_second_float.startIter=pointsToCalc_second_float.begin();
        batchIter_second_float.endIter=pointsToCalc_second_float.begin();
        drawIter_second_float.startIter=pointsToCalc_second_float.begin();
        drawIter_second_float.endIter=pointsToCalc_second_float.begin();

        batchIter_third_float.startIter=pointsToCalc_third_float.begin();
        batchIter_third_float.endIter=pointsToCalc_third_float.begin();
        drawIter_third_float.startIter=pointsToCalc_third_float.begin();
        drawIter_third_float.endIter=pointsToCalc_third_float.begin();

        rangesToDraw_first_float.clear();
        rangesToDraw_second_float.clear();
        rangesToDraw_third_float.clear();
    } //End-if doNewCalc

    //When we simply increase or decrease iterations, just overwrite the iteration values of existing PointToCalc without recomputing the complex plane coordinates associated with each pixel, since these have not changed
    //Resets our pointers and stats but saves the time-consuming calculation of C for 3x1024x1024 pixels
    if (recalcSamePoints) {
        cout<<"Waiting for old threads to finish..."<<std::flush;
        for (unsigned int t=0;t<numThreads;t++) {
            if (futures_first_float[t].valid()) futures_first_float[t].get();
            if (futures_second_float[t].valid()) futures_second_float[t].get();
            if (futures_third_float[t].valid()) futures_third_float[t].get();
        }
        cout<<"Threads terminated."<<endl;
        cout<<"Points to plot have been reset."<<std::endl;

        recalcSamePoints=false;
        numPointsToCalc=pointsToCalc_first_float.size()+pointsToCalc_second_float.size()+pointsToCalc_third_float.size();
        numPointsFinished=0;
        numPointsUpdated=0;
        batchIter_first_float.startIter=pointsToCalc_first_float.begin();
        batchIter_first_float.endIter=pointsToCalc_first_float.begin();
        drawIter_first_float.startIter=pointsToCalc_first_float.begin();
        drawIter_first_float.endIter=pointsToCalc_first_float.begin();

        batchIter_second_float.startIter=pointsToCalc_second_float.begin();
        batchIter_second_float.endIter=pointsToCalc_second_float.begin();
        drawIter_second_float.startIter=pointsToCalc_second_float.begin();
        drawIter_second_float.endIter=pointsToCalc_second_float.begin();

        batchIter_third_float.startIter=pointsToCalc_third_float.begin();
        batchIter_third_float.endIter=pointsToCalc_third_float.begin();
        drawIter_third_float.startIter=pointsToCalc_third_float.begin();
        drawIter_third_float.endIter=pointsToCalc_third_float.begin();

        rangesToDraw_first_float.clear();
        rangesToDraw_second_float.clear();
        rangesToDraw_third_float.clear();
    } //End-if recalcSamePoints

    //Plots trajectories for a single point using three different float types.  For display screen modes 7 and 8.
    if (drawThreePaths) {
        pathPoints_first_float.clear();
        pathPoints_second_float.clear();
        pathPoints_third_float.clear();
        pathPoints_first_float=msetPath_first_float(firstPathPoint_first_float, maxIterations);
        pathPoints_second_float=msetPath_second_float(firstPathPoint_second_float, maxIterations);
        pathPoints_third_float=msetPath_third_float(firstPathPoint_third_float, maxIterations);

        cout<<"Coordinates:\nFirst float:"<<firstPathPoint_first_float<<endl;
        cout<<"Second float:"<<firstPathPoint_second_float<<endl;
        cout<<"Third float:"<<firstPathPoint_third_float<<endl;
        cout<<"Iterations: 1st: "<<pathPoints_first_float.size()<<" 2nd: "<<pathPoints_second_float.size()<<" 3rd: "<<pathPoints_third_float.size()<<endl;

        BeginTextureMode(pathDrawings);
        ClearBackground(BLANK);
        unsigned int pathsize=pathPoints_first_float.size();
        for (unsigned int i=0;i<pathsize-1;i++) {
            DrawLine(pathPoints_first_float[i].x,pathPoints_first_float[i].y,pathPoints_first_float[i+1].x,pathPoints_first_float[i+1].y,BLUE);
            DrawLine(pathPoints_second_float[i].x,pathPoints_second_float[i].y,pathPoints_second_float[i+1].x,pathPoints_second_float[i+1].y,RED);
            DrawLine(pathPoints_third_float[i].x,pathPoints_third_float[i].y,pathPoints_third_float[i+1].x,pathPoints_third_float[i+1].y,YELLOW);
        }
        DrawLine(pathPoints_first_float[pathsize-2].x,pathPoints_first_float[pathsize-2].y,pathPoints_first_float[pathsize-1].x,pathPoints_first_float[pathsize-1].y,BLUE);
        DrawLine(pathPoints_second_float[pathsize-2].x,pathPoints_second_float[pathsize-2].y,pathPoints_second_float[pathsize-1].x,pathPoints_second_float[pathsize-1].y,RED);
        DrawLine(pathPoints_third_float[pathsize-2].x,pathPoints_third_float[pathsize-2].y,pathPoints_third_float[pathsize-1].x,pathPoints_third_float[pathsize-1].y,YELLOW);
        EndTextureMode();
        drawThreePaths=false;
    } //End-if drawThreePaths

    //Calculates the difference between iteration values obtained for each floating point type.  Calculates basic stats about the differences.
    if (calcDifference) {
        int pixeldiff=0;
        int pixeldiff_min=0;
        int pixeldiff_max=0;
        int pixeldiffCounter=0;

        if ( (displayScreen==4) || (displayScreen==7) ) {
            BeginTextureMode(diffScreen2M1);
            for (unsigned int i=0;i<1024*1024;i++) {
                pixeldiff=pointsToCalc_second_float[i].iterations-pointsToCalc_first_float[i].iterations;
                if (pixeldiff==0) {
                    differentPixels2M1[i]=0;
                    DrawPixel(i%1024,i/1024,BLACK);
                }
                else {
                    pixeldiffCounter++;
                    if (pixeldiff>pixeldiff_max) pixeldiff_max=pixeldiff;
                    if (pixeldiff<pixeldiff_min) pixeldiff_min=pixeldiff;
                    differentPixels2M1[i]=pixeldiff;
                    DrawPixel(i%1024,i/1024,colorizePixelDiff(pixeldiff));
                }
            }
            EndTextureMode();
            calcDifference=false;
            cout<<"Difference stats of Second type - First type:\nDifferent pixels: "<<pixeldiffCounter<<" ("<<(float)pixeldiffCounter/(10.24f*1024.0f)<<"%)\nMax diff: "<<pixeldiff_max<<" Min diff: "<<pixeldiff_min<<endl;
        }

        else if (displayScreen==5) {
            BeginTextureMode(diffScreen3M1);
            for (unsigned int i=0;i<1024*1024;i++) {
                pixeldiff=pointsToCalc_third_float[i].iterations-pointsToCalc_first_float[i].iterations;
                if (pixeldiff==0) {
                    differentPixels3M1[i]=0;
                    DrawPixel(i%1024,i/1024,BLACK);
                }
                else {
                    pixeldiffCounter++;
                    if (pixeldiff>pixeldiff_max) pixeldiff_max=pixeldiff;
                    if (pixeldiff<pixeldiff_min) pixeldiff_min=pixeldiff;
                    differentPixels3M1[i]=pixeldiff;
                    DrawPixel(i%1024,i/1024,colorizePixelDiff(pixeldiff));
                }
            }
            EndTextureMode();
            calcDifference=false;
            cout<<"Difference stats of Third type - First type:\nDifferent pixels: "<<pixeldiffCounter<<" ("<<(float)pixeldiffCounter/(10.24f*1024.0f)<<"%)\nMax diff: "<<pixeldiff_max<<" Min diff: "<<pixeldiff_min<<endl;
        }

        else if (displayScreen==6) {
            BeginTextureMode(diffScreen3M2);
            for (unsigned int i=0;i<1024*1024;i++) {
                pixeldiff=pointsToCalc_third_float[i].iterations-pointsToCalc_second_float[i].iterations;
                if (pixeldiff==0) {
                    differentPixels3M2[i]=0;
                    DrawPixel(i%1024,i/1024,BLACK);
                }
                else {
                    pixeldiffCounter++;
                    if (pixeldiff>pixeldiff_max) pixeldiff_max=pixeldiff;
                    if (pixeldiff<pixeldiff_min) pixeldiff_min=pixeldiff;
                    differentPixels3M2[i]=pixeldiff;
                    DrawPixel(i%1024,i/1024,colorizePixelDiff(pixeldiff));
                }
            }
            EndTextureMode();
            calcDifference=false;
            cout<<"Difference stats of Third type - Second type:\nDifferent pixels: "<<pixeldiffCounter<<" ("<<(float)pixeldiffCounter/(10.24f*1024.0f)<<"%)\nMax diff: "<<pixeldiff_max<<" Min diff: "<<pixeldiff_min<<endl;
        }
        else if ((displayScreen==1) || (displayScreen==7)||(displayScreen==8)) {
            //Do nothing
        }
        else {
            //Error -- how did we get here?
            cerr<<"Error in difference screen logic.  Display screen mode="<<displayScreen<<endl;
            calcDifference=false;
            displayScreen=1;
        }
    } // End-if calcDifference

    // The main routine which assigns new work to threads and fetches completed work
    if (!pauseCalc) {
        //For our first type's numThreads threads:
        for (unsigned int t=0; t<numThreads; t++) {  //For each thread
            if (futures_first_float[t].valid()) {  //If there's a valid future, that means work has been assigned
                if (futures_first_float[t].wait_for(0s)==future_status::ready) {  //See if it's ready yet
                    drawIter_first_float=futures_first_float[t].get();  // If ready, get the range of points that has been completed.  (Iterations values have been updated directly since this operation is thread-safe.)
                    rangesToDraw_first_float.push_back(drawIter_first_float);  // Push the range of newly-computed points to the vector to draw them in the next update
                    if (batchIter_first_float.endIter<pointsToCalc_first_float.end()) {  // If there is more work to do, assign it to this now-idle thread
                        batchIter_first_float.startIter=batchIter_first_float.endIter;
                        batchIter_first_float.endIter=(batchIter_first_float.startIter+batchSize_first_float>pointsToCalc_first_float.end())?pointsToCalc_first_float.end():batchIter_first_float.startIter+batchSize_first_float;
                        futures_first_float[t]=std::async(msetBatch_first_float,batchIter_first_float,maxIterations);
                    }
                }
            }
            else {  // If there isn't a valid future, that's because we are either out of new work to assign, or we have just reset our points to calculate and terminated all previously-running threads
                if (batchIter_first_float.endIter<pointsToCalc_first_float.end()) {  //If the latter, assign new work as above
                    batchIter_first_float.startIter=batchIter_first_float.endIter;
                    batchIter_first_float.endIter=(batchIter_first_float.startIter+batchSize_first_float>pointsToCalc_first_float.end())?pointsToCalc_first_float.end():batchIter_first_float.startIter+batchSize_first_float;
                    futures_first_float[t]=std::async(msetBatch_first_float,batchIter_first_float,maxIterations);
                }
            }
        }

        //Do the same thing as above for our second type's threads
        for (unsigned int t=0; t<numThreads; t++) {
            if (futures_second_float[t].valid()) {
                if (futures_second_float[t].wait_for(0s)==future_status::ready) {
                    drawIter_second_float=futures_second_float[t].get();
                    rangesToDraw_second_float.push_back(drawIter_second_float);
                    if (batchIter_second_float.endIter<pointsToCalc_second_float.end()) {
                        batchIter_second_float.startIter=batchIter_second_float.endIter;
                        batchIter_second_float.endIter=(batchIter_second_float.startIter+batchSize_second_float>pointsToCalc_second_float.end())?pointsToCalc_second_float.end():batchIter_second_float.startIter+batchSize_second_float;
                        futures_second_float[t]=std::async(msetBatch_second_float,batchIter_second_float,maxIterations);
                    }
                }
            }
            else {
                if (batchIter_second_float.endIter<pointsToCalc_second_float.end()) {
                    batchIter_second_float.startIter=batchIter_second_float.endIter;
                    batchIter_second_float.endIter=(batchIter_second_float.startIter+batchSize_second_float>pointsToCalc_second_float.end())?pointsToCalc_second_float.end():batchIter_second_float.startIter+batchSize_second_float;
                    futures_second_float[t]=std::async(msetBatch_second_float,batchIter_second_float,maxIterations);
                }
            }
        }

        //Do the same thing for our third type's threads
        for (unsigned int t=0; t<numThreads; t++) {
            if (futures_third_float[t].valid()) {
                if (futures_third_float[t].wait_for(0s)==future_status::ready) {
                    drawIter_third_float=futures_third_float[t].get();
                    rangesToDraw_third_float.push_back(drawIter_third_float);
                    if (batchIter_third_float.endIter<pointsToCalc_third_float.end()) {
                        batchIter_third_float.startIter=batchIter_third_float.endIter;
                        batchIter_third_float.endIter=(batchIter_third_float.startIter+batchSize_third_float>pointsToCalc_third_float.end())?pointsToCalc_third_float.end():batchIter_third_float.startIter+batchSize_third_float;
                        futures_third_float[t]=std::async(msetBatch_third_float,batchIter_third_float,maxIterations);
                    }
                }
            }
            else {
                if (batchIter_third_float.endIter<pointsToCalc_third_float.end()) {
                    batchIter_third_float.startIter=batchIter_third_float.endIter;
                    batchIter_third_float.endIter=(batchIter_third_float.startIter+batchSize_third_float>pointsToCalc_third_float.end())?pointsToCalc_third_float.end():batchIter_third_float.startIter+batchSize_third_float;
                    futures_third_float[t]=std::async(msetBatch_third_float,batchIter_third_float,maxIterations);
                }
            }
        }

    } //End-if !pauseCalc


    //Update RenderTextures.  We also update our progress stats here.
    numPointsUpdated=0;

    //For our first type
    BeginTextureMode(msetRenderTexture_first_float); //Update image for first type
    if (eraseScreen) {
        ClearBackground(BLACK); //We only toggle eraseScreen after erasing the third type's screen further below
    }
    for (unsigned int i=0;i<rangesToDraw_first_float.size();i++) {  //Plot points for ranges of iterators returned above in (!pauseCalc) routine
        for (vector<PointToCalc_first_float>::iterator it=rangesToDraw_first_float[i].startIter;it!=rangesToDraw_first_float[i].endIter;it++) {
            DrawPixel(it->gridPos.x,it->gridPos.y,colorizer(it->iterations));
        }
        numPointsUpdated+=(unsigned int)(rangesToDraw_first_float[i].endIter-rangesToDraw_first_float[i].startIter);  //Update stats
    }
    EndTextureMode();
    rangesToDraw_first_float.clear(); // Clear vector since these points have been plotted now

    //Same for our second type
    BeginTextureMode(msetRenderTexture_second_float);
    if (eraseScreen) {
        ClearBackground(BLACK);
    }
    for (unsigned int i=0;i<rangesToDraw_second_float.size();i++) {
        for (vector<PointToCalc_second_float>::iterator it=rangesToDraw_second_float[i].startIter;it!=rangesToDraw_second_float[i].endIter;it++) {
            DrawPixel(it->gridPos.x,it->gridPos.y,colorizer(it->iterations));
        }
        numPointsUpdated+=(unsigned int)(rangesToDraw_second_float[i].endIter-rangesToDraw_second_float[i].startIter);
    }
    EndTextureMode();
    rangesToDraw_second_float.clear();

    //Same for our third type
    BeginTextureMode(msetRenderTexture_third_float);
    if (eraseScreen) {
        ClearBackground(BLACK);
        eraseScreen=false;
    }
    for (unsigned int i=0;i<rangesToDraw_third_float.size();i++) {
        for (vector<PointToCalc_third_float>::iterator it=rangesToDraw_third_float[i].startIter;it!=rangesToDraw_third_float[i].endIter;it++) {
            DrawPixel(it->gridPos.x,it->gridPos.y,colorizer(it->iterations));
        }
        numPointsUpdated+=(unsigned int)(rangesToDraw_third_float[i].endIter-rangesToDraw_third_float[i].startIter);
    }
    EndTextureMode();
    rangesToDraw_third_float.clear();

    //Update the other stats now that we know how many points we have plotted for all three types
    numPointsFinished+=numPointsUpdated;
    numPointsToCalc-=numPointsUpdated;

    //Draw everything to the screen
    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(1030,5);

    //Compose what we draw according to the display screen mode selected
    if (displayScreen==1) {
        DrawTextureRec(msetRenderTexture_first_float.texture,flipRec,origin,WHITE);
        //Draw small rectangle to indicate progress
        if (batchIter_first_float.endIter!=pointsToCalc_first_float.end()) {
            DrawRectangle(batchIter_first_float.startIter->gridPos.x,batchIter_first_float.startIter->gridPos.y,3,3,Fade(WHITE,0.8));
        }
        DrawText("First type",1030,50,20,WHITE);
    }
    if (displayScreen==2) {
        DrawTextureRec(msetRenderTexture_second_float.texture,flipRec,origin,WHITE);
        //Draw small rectangle to indicate progress
        if (batchIter_second_float.endIter!=pointsToCalc_second_float.end()) {
            DrawRectangle(batchIter_second_float.startIter->gridPos.x,batchIter_second_float.startIter->gridPos.y,3,3,Fade(WHITE,0.8));
        }
        DrawText("Second type",1030,50,20,WHITE);
    }
    if (displayScreen==3) {
        DrawTextureRec(msetRenderTexture_third_float.texture,flipRec,origin,WHITE);
        //Draw small rectangle to indicate progress
        if (batchIter_third_float.endIter!=pointsToCalc_third_float.end()) {
            DrawRectangle(batchIter_third_float.startIter->gridPos.x,batchIter_third_float.startIter->gridPos.y,3,3,Fade(WHITE,0.8));
        }
        DrawText("Third type",1030,50,20,WHITE);
    }
    if (displayScreen==4) {
        DrawTextureRec(diffScreen2M1.texture,flipRec,origin,WHITE);
        DrawText("Difference",1030,50,20,WHITE);
        DrawText("Type 2 - Type 1",1035,80,16,WHITE);
    }
    if (displayScreen==5) {
        DrawTextureRec(diffScreen3M1.texture,flipRec,origin,WHITE);
        DrawText("Difference",1030,50,20,WHITE);
        DrawText("Type 3 - Type 1",1035,80,16,WHITE);
    }
    if (displayScreen==6) {
        DrawTextureRec(diffScreen3M2.texture,flipRec,origin,WHITE);
        DrawText("Difference",1030,50,20,WHITE);
        DrawText("Type 3 - Type 2",1035,80,16,WHITE);
    }
    if (displayScreen==7) {
        DrawTexturePro(msetRenderTexture_first_float.texture,flipRec,{0.0,0.0,512.0f,512.0f},origin,0,WHITE);
        DrawTexturePro(msetRenderTexture_second_float.texture,flipRec,{512.0,0.0,512.0f,512.0f},origin,0,WHITE);
        DrawTexturePro(diffScreen2M1.texture,flipRec,{0.0,512.0,512.0f,512.0f},origin,0,WHITE);
        DrawTexturePro(msetCanvas.texture,flipRec,{512.0,512.0,512.0f,512.0f},origin,0,Fade(WHITE,0.5));
        DrawTexturePro(pathDrawings.texture,flipRec,{512.0,512.0,512.0f,512.0f},origin,0,WHITE);
    }
    if (displayScreen==8) {
        //DrawTextureRec(msetCanvas.texture,flipRec,origin,Fade(WHITE,0.5));
        DrawTextureRec(msetRenderTexture_first_float.texture,flipRec,origin,WHITE);
        DrawTextureRec(pathDrawings.texture,flipRec,origin,WHITE);
    }

    //Draws a small rectangle to indicate the exact center of the screen.  Useful for zooming.
    if (highlightCenterOfScreen) DrawRectangle(511,511,3,3,Fade(WHITE,0.8));

    if (showGrid) { // To help estimate orbit positions on the screen and for estimating zoom by factors of two or eight
        DrawCircleLines(512,512,64,Fade(GRAY,0.5));  //Circles at radius 1/4, 1/2, 3/4 and 1
        DrawCircleLines(512,512,128,Fade(GRAY,0.5));
        DrawCircleLines(512,512,192,Fade(GRAY,0.5));
        DrawCircleLines(512,512,256,Fade(LIGHTGRAY,0.5));
        DrawRectangleLines(256,256,512,512,Fade(LIGHTGRAY,0.5)); // Rectangle from (-1,-1) to (1,1)
        DrawCircleLines(512,512,512,Fade(GRAY,0.5));
        DrawLine(0,0,1023,1023,Fade(GRAY,0.5));
        DrawLine(0,1023,1023,0,Fade(GRAY,0.5));
    }

    //Update sidebar info
    DrawText("Max iterations:",1030,650,20,WHITE);
    DrawText(PrintNum(smallText,maxIterations),1035,680,20,WHITE);

    DrawText("Pixel delta is",1030,710,20,WHITE);
    snprintf(smallText,40,"2^%i",zoomDenomPower);
    DrawText(smallText,1035,740,20,WHITE);

    if (pauseCalc) DrawText("Paused.",1030,770,20,YELLOW);

    DrawText("Points Finished:",1030,800,20,GREEN);
    DrawText(PrintNum(smallText,numPointsFinished),1035,830,20,GREEN);
    DrawText("Updated:",1030,860,20,GREEN);
    DrawText(PrintNum(smallText,numPointsUpdated),1035,890,20,GREEN);
    DrawText("Remaining:",1030,920,20,GREEN);
    DrawText(PrintNum(smallText,numPointsToCalc),1035,950,20,GREEN);

    EndDrawing();

    } //End main loop white

    //Clean-up.  RayLib requires that we unload all textures.
    UnloadRenderTexture(msetRenderTexture_first_float);
    UnloadRenderTexture(msetRenderTexture_second_float);
    UnloadRenderTexture(msetRenderTexture_third_float);
    UnloadRenderTexture(diffScreen2M1);
    UnloadRenderTexture(diffScreen3M1);
    UnloadRenderTexture(diffScreen3M2);
    UnloadRenderTexture(pathDrawings);
    UnloadRenderTexture(msetCanvas);
    CloseWindow();
    return 0;
}
