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


FILE DIRECTIONS:
-Sounds are in "Wavs/" folder (hardcoded in line 61)

-Feel free to use any 44.1khz, 16 bit file, 2 or 1 channel
*/

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "FFT.h"

using namespace std ;
using namespace sf ;
using namespace Audio;

int main()
{
	RenderWindow window(VideoMode(900,900,32),"Source Signal");
	
	RenderWindow window2(VideoMode(900,900,32),"Filtered Signal");
	
	string path ;
	int bufferSize ;
	int myfilter;
	bool source = false;
	
	cout << "Choose a filter to use from the list with 50 taps" << endl;
	cout << "0: Low pass filter with upper bound @ 0.5khz\n" <<
			"1: High pass filter with lower bound @ 5khz\n" <<
			"2: Band pass filter with bounds between 1khz and 4khz\n";
	cin >> myfilter;
	
	FFT fft("Wavs/bird.wav", 16384, myfilter);

	Event event ; //for capturing key presses, mouse presses

	while(window.isOpen() && window2.isOpen() )
//	while(window.isOpen())
	{
		while(window.pollEvent(event) || window2.pollEvent(event)) 
		{
			if (event.type == Event::Closed)
            {
            	window.close();
			}
			//PAUSE function
			if(event.type == sf::Event::KeyPressed)
			{
				if(event.key.code == sf::Keyboard::Space)
	        	{
//	        		cout << "Pressed " << event.key.code << endl;
	        		fft.ppSound();
				}
				//REWIND function
				else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				{
					fft.rewind();
				}
				//ADVANCE function
				else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				{
					fft.seek();
				}
				
				
				//PITCH function
				else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				{
					fft.uPitch();
				}
				else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				{
					fft.dPitch();
				}
				else if(sf::Keyboard::isKeyPressed(sf::Keyboard::R))
				{
					fft.rPitch();
				}
				
				else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				{
					fft.switchSource(source);
					source = !source;
				}
			}
        	
			
		} //wait for an event to happen (key press, etc)

		fft.update() ;
		
		window.clear() ;
		window2.clear();
		
		fft.draw(window) ;
		fft.drawFSignal(window2);
		
		window.display() ;
		window2.display();
	}

	return 0;
}
