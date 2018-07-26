/*
###################################

Sound Manipulation in C++
(An Honors Contract Project) by

Bruno Gracia V.

EE 3223

under guidance by
Professor Prevost

4/27/2018

###################################

CREDITS:

Visualization and FFT Made, by Muzkaw
https://www.youtube.com/watch?v=LqUuMqfW1PE
https://drive.google.com/file/d/0B2voedb-erQsR09PYnpaRnZKbGM/view

FIR filter class, by Mike Perkins

Filter implementation, by Bruno Gracia V.

*/

#ifndef FFT_H
#define FFT_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
//#include <DSPFilters/DSP.h>
#include <Synchapi.h>

#include <iostream>
#include <complex>
#include <valarray>
#include <math.h>

#include <fstream>

//filter includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "filt.h"


using namespace std ;
using namespace sf ;
//using namespace Dsp;

namespace Audio
{
const double PI = 3.141592653589793238460 ;
typedef complex<double> Complex;	//typedef for storing each sample's complex representation
typedef valarray<Complex> CArray; 	//used to perform math operations on every element in the array

class FFT
{
	public:
		FFT(string const& _path, int const& _bufferSize, int fc);
		~FFT()
		{
			delete samples;
			delete filterSamples;
			delete lfSamples;
			for(int i=0; i < generalFilter.size(); i++)
			{
				delete generalFilter.at(i);
			}
		}
		void hammingWindow() ;		//windowing function to focus on a sample of data from the entire signal
		void fft(CArray &x, CArray &fx) ;		//perform fft on sample data
		void update() ;				//for continously running an fft on the sound data and displaying on screen
	
		void bars(float const& max) ;	//for drawing the frequency domain signal
		void lines(float const& max) ;  //for drawing the time dowmain signal
	
		void draw(RenderWindow &window) ;	//draw on the screen both freq. and time representations
		void drawFSignal(RenderWindow &window);
		
		/*Bruno's functions*/
		int choiceFilter;
		void switchSource(bool s);
		void ppSound();		//toggle play/pause on sound
		void rewind();		//rewind the playing sound
		void seek();		//seek on the playing sound
		
		void uPitch();		//increase the pitch of the sound
		void dPitch();		//decrease the pitch of the sound
		void rPitch();		//reset the pitch of the sound to original state (1.00)
		
		void moveSound3d(int dir);
		void updateFilterSound();
		
		bool initFilters();
		bool filter();
		bool checkRecorder();	
		bool outWAV(string wavName);	
		bool outWAV(string wavName, Int16*& fsamples);
		
		Int16 lpfSample(double& s);
		Int16 hpfSample(double& s);
		Int16 bpfSample(double& s);
		/*Bruno's functions*/
		
	private:
		string path ;
		SoundBuffer buffer ;
		SoundBuffer fbuffer;
		
		Sound sound ;
		Sound fsound;
	
		vector<Complex> sample ;
		vector<Complex> fsample; //filter sample for fft
		
		vector<float> window ;
		
		CArray bin ;
		CArray fbin; //filter bin
		
		VertexArray VA1 ;
		VertexArray VA2 ;
		VertexArray VA3 ;
		
		VertexArray ftimesignal; //va1 time signal
		VertexArray filterBars; //va2- filter bars
		VertexArray filterCascade; //va3 - cascade frequency
		
		vector<Vertex> cascade ;
		vector<Vertex> vFilterCascade;
		
		int sampleRate ;
		int sampleCount ;
		int bufferSize ;
		int mark ;
		
		
		Int16* samples;			//signed short size for sample data
		Int16* filterSamples;
		Int16* lfSamples;
		SoundBuffer fBuffer;
		vector<Filter*> generalFilter;
};
}
#endif
