//Names: Mark Joya (z5104126), Daniel Lau (z3376221)
//Tutor: William Archinal, Thu12Kora
//Date: Started: 19/04/2016; Edited: 22/04/2016
//Purpose: Serves a coloured .bmp of the Mandelbrot set to a server,
//         given x, y coords and zoom.

//With thanks to code from:
//    -Richard Buckland:
//        -Server Functions and main (28/01/11) 
//        -serveBMPHeader (14/04/11)
//        -chessboard.c BMP Header (14/04/11)
//    -Tim Lambert:
//        -serveBMPHeader (02/04/12)
//    -COMP1917 Coordinators:
//        -serveHTML function

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

#include "pixelColor.h"
#include "mandelbrot.h"


#define MAX_ITERATIONS 256
#define DIST_SQUARED 4
#define TRUE 1
#define FALSE 0
#define HEADER_SIZE 54

#define SIMPLE_SERVER_VERSION 1.0
#define REQUEST_BUFFER_SIZE 1000
#define DEFAULT_PORT 1914
#define NUMBER_OF_PAGES_TO_SERVE 100
// after serving this many pages the server will halt

#define BYTES_PER_PIXEL 3 
#define SIZE 512

typedef struct _color {
    int red;
    int blue;
    int green;
} Color;

typedef struct _Requestdata {
    double xVal;
    double yVal;
    double zVal;
    
    char xCheck;
    char yCheck;
    char zCheck;
} Requestdata;
 
int escapeSteps (double x, double y);
static int inMandelbrot (double x, double y);
static void testinMandelbrot (void);
static Requestdata extract(char* request);
static void serveBMPData (int socket, Requestdata coord);
static void testpower (void);
static double power (double base, double exponent);
static void serveBMPHeader (int socket);

int makeServerSocket (int portno);
int waitForConnection (int serverSocket);

static void serveHTML (int socket);
 
int main (int argc, char *argv[]) {

    testinMandelbrot();
    testpower();
   
    //Server Code Template by Richard Buckland
    
    printf ("************************************\n");
    printf ("Starting simple server %f\n", SIMPLE_SERVER_VERSION);
    printf ("Serving bmps since 2012\n");   
   
    int serverSocket = makeServerSocket (DEFAULT_PORT);   
    printf ("Access this server at http://localhost:%d/\n", DEFAULT_PORT);
    printf ("************************************\n");
   
    char request[REQUEST_BUFFER_SIZE];
   
    int numberServed = 0;
    while (numberServed < NUMBER_OF_PAGES_TO_SERVE) {
      
        printf ("*** So far served %d pages ***\n", numberServed);
      
        int connectionSocket = waitForConnection (serverSocket);
        // wait for a request to be sent from a web browser, open a new
        // connection for this conversation
      
        // read the first line of the request sent by the browser  
        int bytesRead;
        bytesRead = read (connectionSocket, request, (sizeof request)-1);
        assert (bytesRead >= 0); 
        // were we able to read any data from the connection?
            
        // print entire request to the console 
        printf (" *** Received http request ***\n %s\n", request);
      
        //send the browser BMP Header and data using http
        printf (" *** Sending http response ***\n");
        
        //**Our functions here ** VVV
       
        Requestdata coord = extract(request);
        
        if (coord.xCheck == 'x' && coord.yCheck == 'y' &&
            coord.zCheck == 'z') {    
            serveBMPHeader(connectionSocket);
            serveBMPData(connectionSocket, coord);                
        } else {     
            serveHTML(connectionSocket);
        }
      
        //**Our functions here ** ^^^    
              
        // close the connection after sending the page- keep aust beautiful
        close(connectionSocket);
      
        numberServed++;
    } 
   
    // close the server connection after we are done- keep aust beautiful
    printf ("** shutting down the server **\n");
    close (serverSocket);
      
   
    return EXIT_SUCCESS;
}

//Purpose: Calculates the amount of escape steps for a complex number
//         to escape the mandelbrot set. Iterations stop at 256

int escapeSteps (double x, double y) {

    double xr = 0, xi = 0;
    double xr2 = 0, xi2 = 0;
    int i = 0;

    while(((xr * xr + xi * xi) < DIST_SQUARED) &&
           (i < MAX_ITERATIONS)) {
        xr2 = xr * xr;
        xi2 = xi * xi;
        xi = 2 * xr * xi + y;
        xr = xr2 - xi2 + x;
        i++;
    }

    return i;
}

//Purpose: Checks whether the point x, y is strictly in the mandelbrot
//         set (escapeSteps == MAX_ITERATIONS)

static int inMandelbrot (double x, double y) {
    int status;
    int steps = (escapeSteps(x, y));
    
    if (steps == MAX_ITERATIONS) {
        status = TRUE;
    } else {
        status = FALSE;    
      
    }
    return status;
}

//Purpose: Tests the inMandelbrot function with some points to check
//         if working correctly

static void testinMandelbrot (void) {

    assert (inMandelbrot(100,100) == FALSE);
    assert (inMandelbrot(0,0) == TRUE);
    assert (inMandelbrot(-1.50, -1.50) == FALSE);
    
    printf("inMandelbrot tests passed!\n");
    
    return;
}

//Purpose: Extracts data from the HTTP Request string:
//         - The coordinates of x,y and zoom level
//         - The x, y and z chars 

static Requestdata extract(char* request) {

    Requestdata point;

    sscanf( request, "GET /tile_%c%lf_%c%lf_%c%lf.bmp",
           &point.xCheck, &point.xVal, &point.yCheck,&point.yVal,
           &point.zCheck, &point.zVal);

    return point;
}

//Purpose: Iterates over an area of the mandelbrot set and writes rgb
//         colors (3 bytes) to the server depending on escape steps of 
//         each x, y coordinate

static void serveBMPData (int socket, Requestdata coord) {
    
    Color pixel;
    
    double xcentre = coord.xVal;
    double ycentre = coord.yVal;
    int zoom = coord.zVal;
    
    double pixelWidth = power (2, -zoom);
        
    double halfSideLength = (SIZE * pixelWidth) / 2;
    
    double posHeight = ycentre + halfSideLength;
    double negHeight = ycentre - halfSideLength;
    double posWidth = xcentre + halfSideLength;
    double negWidth = xcentre - halfSideLength;
    
    double row = negHeight;
    double col = negWidth;
       
    char bmpArray[SIZE * SIZE * BYTES_PER_PIXEL];
    int arrayCounter = 0;
    
    while (row < posHeight) {
        //Reset to beginning of row
        col = negWidth;
        while (col < posWidth) {
            pixel.blue = stepsToBlue(escapeSteps(col, row));
            pixel.green = stepsToGreen(escapeSteps(col, row));
            pixel.red = stepsToRed(escapeSteps(col, row));
            
            bmpArray[arrayCounter] = pixel.blue;
            bmpArray[arrayCounter + 1] = pixel.green;
            bmpArray[arrayCounter + 2] = pixel.red;
            
            arrayCounter += BYTES_PER_PIXEL;
            
            col = col + pixelWidth;
        }
        row = row + pixelWidth;        
    }
    
    write (socket, bmpArray, sizeof(bmpArray));
    
    return;
}

//Purpose: Tests the power function with some base and exponent values
//         to check if working correctly

static void testpower (void) {

    assert (power(2, 3) == 8);
    assert (power(0, 3) == 0);    
    assert ((power(3, -2) - (double) (1/9)) > -0.05);
    assert (power(5, 0) == 1);
    assert (power(-2, 2) == 4);
    assert (power(-2, 3) == -8);
    assert ((power(-2, -2) - (double) (1/4)) > -0.05);                
}

//Purpose: Given a base number and exponent number, performs the
//         math operation (base^power)

static double power (double base, double exponent) {
    
    double result = 0;
    
    int counter = 1;
    
    if (exponent > 0) {
        result = base;
        while (counter < exponent) {
            result = result * base;
            counter++;
        }
    } else if (exponent == 0) {
        result = 1;
    } else if (exponent < 0) {
          result = base;
          while (counter < (-1 * exponent)) {
              result = result * base;
              counter++;          
          }
          result = 1 / result;
    }
    
    return result;
}


//**Function TEMPLATE below by Richard Buckland / Tim Lambert**
//Purpose: Sends the BMP DIB Header to the server 

static void serveBMPHeader (int socket) {
    char* message;
   
    // first send the http response header
   
    // (if you write stings one after another like this on separate
    // lines the c compiler kindly joins them togther for you into
    // one long string)
    message = "HTTP/1.0 200 OK\r\n"
                 "Content-Type: image/bmp\r\n"
                 "\r\n";
    printf ("about to send=> %s\n", message);
    write (socket, message, strlen (message));
   
    // now send the BMP
    // DIB Header from chessboard.c - Richard Buckland
    unsigned char bmp[HEADER_SIZE] = {
     0x42,0x4d,0x36,0x00,0x0c,0x00,0x00,0x00,
     0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
     0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x02,
     0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,0x0c,0x00,0x13,0x0b,
     0x00,0x00,0x13,0x0b,0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,0x00,0x00};


    write (socket, bmp, sizeof(bmp));
}


//**Function below by Richard Buckland**
// Purpose: Starts the server listening on the specified port number

int makeServerSocket (int portNumber) { 
   
    // create socket
    int serverSocket = socket (AF_INET, SOCK_STREAM, 0);
    assert (serverSocket >= 0);   
    // error opening socket
   
    // bind socket to listening port
    struct sockaddr_in serverAddress;
    memset ((char *) &serverAddress, 0,sizeof (serverAddress));
   
    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port        = htons (portNumber);
   
    // let the server start immediately after a previous shutdown
    int optionValue = 1;
    setsockopt (
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &optionValue, 
        sizeof(int)
    );

    int bindSuccess = 
        bind (
            serverSocket, 
            (struct sockaddr *) &serverAddress,
            sizeof (serverAddress)
        );
   
    assert (bindSuccess >= 0);
    // if this assert fails wait a short while to let the operating 
    // system clear the port before trying again
   
    return serverSocket;
}

//**Function below by Richard Buckland**
// Purpose: Waits for a browser to request a connection, and returns
//          the socket on which the conversation will take place

int waitForConnection (int serverSocket) {
    // listen for a connection
    const int serverMaxBacklog = 10;
    listen (serverSocket, serverMaxBacklog);
   
    // accept the connection
    struct sockaddr_in clientAddress;
    socklen_t clientLen = sizeof (clientAddress);
    int connectionSocket = 
        accept (
            serverSocket, 
            (struct sockaddr *) &clientAddress, 
            &clientLen
        );
   
    assert (connectionSocket >= 0);
    // error on accept
   
    return (connectionSocket);
}

//**Function TEMPLATE by COMP1917 course coordinators
// Purpose: Serves a HTML page to the almond bread viewer

static void serveHTML (int socket) {
   char* message;
 
   // first send the http response header
   message =
      "HTTP/1.0 200 Found\n"
      "Content-Type: text/html\n"
      "\n";
   printf ("about to send=> %s\n", message);
   write (socket, message, strlen (message));
 
   message =
      "<!DOCTYPE html>\n"
      "<script src=\"http://almondbread.cse.unsw.edu.au/tiles.js\"></script>"
      "\n";      
   write (socket, message, strlen (message));
}

