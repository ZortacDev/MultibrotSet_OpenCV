#include <iostream>
#include <cmath>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"

#define MAX_ITERATION 512
#define X_BOUND 2
#define X_RES 1440
#define X_ASPECT 16
#define Y_ASPECT 10
#define Y_RES ((X_RES/X_ASPECT)*Y_ASPECT)
#define MULTIBROT (3.25)
#define STD_MANDELBROT

#define RED
//#define GREEN
//#define BLUE
//#define PINK


using namespace cv;
using namespace std;

//Die Begrenzungen des Koordinatensystems und die Zoomstufe
double scaleX[2];
double scaleY[2];
double zoomlvl = 1;
int zoomed = 0;

//Die Variable in der das Bild gespeichert wird1
Mat img(Y_RES, X_RES, CV_8UC3, Scalar(255, 255, 255));

//Typ um Faben zu speichern
typedef struct {
    int r;
    int g;
    int b;
} color_t;

//Berechnungen der Begrenzungen
void calcBounds(double center_r, double center_i, double zoom) {
    scaleX[0] = center_r - (X_BOUND / zoom);
    scaleX[1] = center_r + (X_BOUND / zoom);
    
    double xscale = abs(scaleX[1] - scaleX[0]);
    double yscale = (xscale / X_ASPECT) * Y_ASPECT;
    scaleY[0] = (yscale / 2) + center_i;
    scaleY[1] = 0 - (yscale / 2) + center_i;
}

//Berechnung der Farben
color_t get_rgb_smooth(int n, int iter_max) {
    
    double t = (double)n / (double)iter_max;
    //Berechnung
    int r = (int)(9 * (1 - t)*t*t*t * 255);
    int g = (int)(15 * (1 - t)*(1 - t)*t*t * 255);
    int b = (int)(8.5*(1 - t)*(1 - t)*(1 - t)*t * 255);
    
    //Zurückgabe der Werte
    color_t color;
    
    color.r = r;
    color.g = g;
    color.b = b;
    
    return color;
}


double map(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Berechnung der Iterationen pro Pixel und Anzeige
Mat calculate(Mat img) {
    for (int i = 0; i < Y_RES; i++) {
        for (int j = 0; j < X_RES; j++) {
            double x0 = map(j, 0, (X_RES-1), scaleX[0], scaleX[1]);
            double y0 = map(i, 0, (Y_RES-1), scaleY[0], scaleY[1]);
            double x = 0.0;
            double y = 0.0;
            int iteration = 0;
            
            while (x*x + y*y <= 4 && iteration < MAX_ITERATION) {
#ifndef STD_MANDELBROT
                double xtemp = pow((x*x + y*y), (MULTIBROT/2)) * cos(MULTIBROT * atan2(y,x)) + x0;
                y = pow((x*x+y*y),(MULTIBROT/2))*sin(MULTIBROT*atan2(y,x)) + y0;
#else
                double xtemp = x*x - y*y + x0;
                y = 2*x*y + y0;
#endif
                x = xtemp;
                iteration++;
            }
            
            color_t calcColor = get_rgb_smooth(iteration, MAX_ITERATION);
#ifdef RED
            img(Rect(j, i, 1, 1)) = Scalar(calcColor.r, calcColor.g, calcColor.b);
#endif
#ifdef GREEN
            img(Rect(j, i, 1, 1)) = Scalar(calcColor.g, calcColor.b, calcColor.r);
#endif
#ifdef BLUE
            img(Rect(j, i, 1, 1)) = Scalar(calcColor.b, calcColor.g, calcColor.r);
#endif
#ifdef PINK
            img(Rect(j, i, 1, 1)) = Scalar(calcColor.b, calcColor.r, calcColor.g);
#endif
            //cout << calcColor.r << ", " << calcColor.g << ", " << calcColor.b << ", " << endl;
        }
    }
    return img;
}

//Zoom
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if (event == EVENT_LBUTTONDOWN) {
        zoomed++;
        zoomlvl += pow(10, zoomed);
        calcBounds(map(x, 0, (X_RES-1), scaleX[0], scaleX[1]), map(y, 0, (Y_RES-1), scaleY[0], scaleY[1]), zoomlvl);
        img = calculate(img);
        imshow("Mandelbrot-Menge", img);
    }
    else if (event == EVENT_RBUTTONDOWN) {
        if (zoomed > 1) {
            zoomlvl -= pow(10, zoomed);
            zoomed--;
            calcBounds(map((X_RES/2), 0, (X_RES-1), scaleX[0], scaleX[1]), map((Y_RES/2), 0, (Y_RES-1), scaleY[0], scaleY[1]), zoomlvl);
            img = calculate(img);
            imshow("Mandelbrot-Menge", img);
        }
        else {
            zoomlvl = 1;
            calcBounds(0, 0, zoomlvl);
            img = calculate(img);
            imshow("Mandelbrot-Menge", img);
        }
    }
}


//Hauptfunktion
int main(int argc, char* argv[]) {

    
    calcBounds(0, 0, zoomlvl);
    
    namedWindow("Mandelbrot-Menge", CV_WINDOW_NORMAL);
    //cvSetWindowProperty("Mandelbrot-Menge", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    cvSetMouseCallback("Mandelbrot-Menge", CallBackFunc, NULL);
    
    img = calculate(img);
    
    imshow("Mandelbrot-Menge", img);
    
    waitKey(0);
    destroyWindow("Mandelbrot-Menge");
    
    return 0;
}