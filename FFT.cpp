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
#include "FFT.h"

namespace Audio
{
bool FFT::outWAV(string wavName)
{
	OutputSoundFile file;
	
	
	if (!file.openFromFile(wavName, 44100, 2)) return false;
	
	file.write(buffer.getSamples(), buffer.getSampleCount());
	return true;
}

bool FFT::outWAV(string wavName, Int16*& fsamples)
{
	OutputSoundFile file;
	
	
	if (!file.openFromFile(wavName, 44100, 2)) return false;
	
	file.write(fsamples, buffer.getSampleCount());
	
	return true;
}

bool FFT::checkRecorder()
{
	
	// first check if an input audio device is available on the system
	if (!SoundBufferRecorder::isAvailable())
	{
	    // error: audio capture is not available on this system
	    return false;
	}
	
	// create the recorder
	SoundBufferRecorder recorder;
	
	// start the capture
	recorder.start();
	
	// wait...
	
	// stop the capture
	recorder.stop();
	
	// retrieve the buffer that contains the captured audio data
	const sf::SoundBuffer& buffer = recorder.getBuffer();
	return true;
}
FFT::FFT(string const& _path,int const& _bufferSize, int fc)
{
	choiceFilter = fc;
	initFilters();

	path = _path ; //set path of file to this->
	if(!buffer.loadFromFile(path)) //check to see if we can read sound
	{
		cout << "Unable to load WAV" << endl ;
	} 
	else
	{
		cout << "LOG > WAV file loaded" << endl;
	}
	
	sound.setBuffer(buffer) ; //"points" to the buffer we declared earlier for sound data
	sound.setLoop(true); //file will play in a loop
	
	
	sound.setVolume(0.0f);
	updateFilterSound();	//build sound buffer with filtered signal
	
	fsound.setLoop(true);
	sound.play();
	fsound.play();			//play filtered signal
	
	
//	sound.play(); 			//begin to play the file
//	sound.setVolume(0.0f);
	
	/*FILTERING SECTION*/
	
//	string newfile = "test2.wav";
//	string filterfile= "filter.wav";
//	
//	if(outWAV(newfile))
//	{
//		cout << "Made new wav @ " << newfile << endl;
//	}
//	
//	if(filter())
//	{
//		cout << "Filtered signal! " << endl;
//		outWAV(filterfile, filterSamples);
//	}
//	else
//	{
//		cout << "Could not filter" << endl;
//	}
	
	/*FILTERING SECTION*/

	//time domain
	VA1.setPrimitiveType(LineStrip) ; 	///< List of connected lines, a point uses the previous point to form a line
	ftimesignal.setPrimitiveType(LineStrip);
	
	//frequency domain
	VA2.setPrimitiveType(Lines) ; 		///< List of individual lines
	filterBars.setPrimitiveType(LineStrip);
	
	//cascade background
	VA3.setPrimitiveType(LineStrip) ; 	//list of connected lines
	filterCascade.setPrimitiveType(LineStrip);

	sampleRate = buffer.getSampleRate()*buffer.getChannelCount() ; 	//ex = 44100*mono or stereo
	sampleCount = buffer.getSampleCount() ; 						//get num of samples in buffer
	
//	cout << "Samples " << sampleCount << endl;
//	cout << "Sample Rate " << sampleRate << endl;
	cout << "______________Instructions______________" << endl;
	cout << "Key    Command" << endl;
	cout << "----------------------------------------" << endl;
	cout << "A:     Toggle Filter on audio signal" << endl;
	cout << "Space: Toggle play/pause" << endl;
	cout << "Up:	Increase pitch (hold key)" << endl;
	cout << "Down:	Decrease pitch (hold down)" << endl;
	cout << "R:     Reset pitch" << endl;
	cout << "Left:	Rewind" << endl;
	cout << "Right:	Seek" << endl;
	if(_bufferSize < sampleCount)
	{
		bufferSize = _bufferSize ; //if buffer chosen fits under samples, set it, otherwise set to samplecount	
	} 
	else 
	{
		bufferSize = sampleCount ;
	}
	
	mark = 0 ; //we are time t=0
	float push = 0.0;	//temp value to store windowing value
	
	for(int i(0) ; i < bufferSize ; i++) 	//setup the hamming window buffer
	{
		push = 0.54-0.46*cos(2*PI*i/(float)bufferSize);
		window.push_back(push) ;
	}
	
	/*
	Resize vector sample and vertexarray VA1 to size of buffersize
	*/
	sample.resize(bufferSize) ; 
	VA1.resize(bufferSize) ;
	
	fsample.resize(bufferSize);
	ftimesignal.resize(bufferSize);
//	lfSamples = new Int16[bufferSize];
}

void FFT::hammingWindow()
{
	mark = sound.getPlayingOffset().asSeconds()*sampleRate ; //what sample are we at right now?
	if(mark+bufferSize < sampleCount) 						//if we still have room to analyze
	{
		for(int i(mark) ; i < mark+bufferSize ; i++) //start window at mark, go up to buffersize
		{
			sample[i-mark] = Complex(buffer.getSamples()[i]*window[i-mark],0) ;
			
			fsample[i-mark] = Complex(fbuffer.getSamples()[i]*window[i-mark],0) ;
			
			//for drawing the time domain signal
			VA1[i-mark] = 
				Vertex(Vector2f(40,250) + Vector2f((i-mark) / (float)bufferSize * 750, sample[i-mark].real() * 0.007),
				Color(0,255,0,50)) ;
			ftimesignal[i-mark] =
				Vertex(Vector2f(40,250) + Vector2f((i-mark) / (float)bufferSize * 750, fsample[i-mark].real() * 0.007),
				Color::Red) ;
		}
	}
//	out.close();
}
void FFT::fft(CArray &x, CArray &fx) 
{
	const int N = x.size();
	if(N <= 1) return; //returns to previous stack element

	CArray even = x[slice(0,N/2,2)]; //slice(offset, num elements in slice, stride between array elements)
	CArray feven = fx[slice(0,N/2,2)];
	
	CArray  odd = x[slice(1,N/2,2)];
	CArray fodd = fx[slice(1,N/2,2)];
	
	fft(even, feven);
	fft(odd, fodd);

	
	for(int k = 0 ; k < N/2 ; k++)
	{
		Complex t = polar(1.0,-2 * PI * k / N) * odd[k];
		Complex ft = polar(1.0,-2 * PI * k / N) * fodd[k];
		
		x[k] = even[k] + t;
		fx[k] = feven[k] + ft;
		
		x[k+N/2] = even[k] - t;
		fx[k+N/2] = feven[k] - ft;
	}
}

void FFT::update()
{
	hammingWindow() ;
	
	VA2.clear() ;
	filterBars.clear();
	VA3.clear() ;
	filterCascade.clear();
	
	
	//get ready to transform time (sample) into frequency domain
	bin = CArray(sample.data(),bufferSize) ; //init bin to values from sample data, buffersize large
	fbin = CArray(fsample.data(),bufferSize);
	
	fft(bin,fbin);
	
	float max = 100000000 ;
	lines(max) ;
	bars(max);
}

void FFT::bars(float const& max)
{
	VA2.setPrimitiveType(Lines) ;
	filterBars.setPrimitiveType(Lines);
	
	Vector2f position(0,800) ;
	for(float i(3) ; i < min(bufferSize/2.f,20000.f) ; i*=1.01)
	{
		Vector2f samplePosition(log(i)/log(min(bufferSize/2.f,20000.f)),abs(bin[(int)i])) ;
		Vector2f fPos(log(i)/log(min(bufferSize/2.f,20000.f)),abs(fbin[(int)i])) ;
		
		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*700),Color::White)) ;
		filterBars.append(Vertex(position+Vector2f(fPos.x*800,-fPos.y/max*700),Color::White)) ;
		
//		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*500),Color::Green)) ; //peak of bars
		
		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,0),Color::White)) ;
		filterBars.append(Vertex(position+Vector2f(fPos.x*800,0),Color::White)) ;
//		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,0),Color::Green)) ; //body of bars
		
		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,0),Color(255,255,255,100))) ; //color(r,g,b,alpha);
		filterBars.append(Vertex(position+Vector2f(fPos.x*800,0),Color(255,255,255,100))) ; //color(r,g,b,alpha);
		
//		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,0),Color::Green)) ; //body of bar reflections
		
//		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,samplePosition.y/max*500/2.f),Color(255,255,255,0))) ;
		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,samplePosition.y/max*500/2.f),Color::White)); //peak of bar reflections
		filterBars.append(Vertex(position+Vector2f(fPos.x*800,fPos.y/max*500/2.f),Color::White)); //peak of bar reflections
	}
}
void FFT::lines(float const& max)
{
	VA3.setPrimitiveType(LineStrip) ;
	filterCascade.setPrimitiveType(LineStrip);
	
	Vector2f position(0,800) ;
	Vector2f samplePosition ;
	Vector2f fpos;
	float colorDecay = 1 ;
	
	for(float i(std::max((double)0,cascade.size()-3e5)) ; i < cascade.size() ; i++)
	{
		cascade[i].position -= Vector2f(-0.8,1) ;
		vFilterCascade[i].position -= Vector2f(-0.8,1);
		
		if(cascade[i].color.a != 0)
		{
			cascade[i].color = Color(0,255,255,20) ;
			vFilterCascade[i].color = Color(0,255,255,20) ;
		} 
	}
	samplePosition = Vector2f(log(3)/log(min(bufferSize/2.f,20000.f)),abs(bin[(int)3])) ;
	fpos = Vector2f(log(3)/log(min(bufferSize/2.f,20000.f)),abs(fbin[(int)3])) ;
	
	cascade.push_back(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*500),Color::Transparent)) ;
	vFilterCascade.push_back(Vertex(position+Vector2f(fpos.x*800,-fpos.y/max*500),Color::Transparent)) ;
	
	for(float i(3) ; i < bufferSize/2.f ; i*=1.02)
	{
		samplePosition = Vector2f(log(i)/log(min(bufferSize/2.f,20000.f)),abs(bin[(int)i])) ;
		fpos = Vector2f(log(i)/log(min(bufferSize/2.f,20000.f)),abs(fbin[(int)i])) ;
		
		cascade.push_back(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*500),Color(255,255,255,20))) ;
		vFilterCascade.push_back(Vertex(position+Vector2f(fpos.x*800,-fpos.y/max*500),Color(255,255,255,20))) ;
	}
	cascade.push_back(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*500),Color::Transparent)) ;
	vFilterCascade.push_back(Vertex(position+Vector2f(fpos.x*800,-fpos.y/max*500),Color::Transparent)) ;

	VA3.clear() ;
	filterCascade.clear();
	
	for(int i(std::max((double)0,cascade.size()-3e5)) ; i < cascade.size() ; i++)
	{
		VA3.append(cascade[i]) ;
		filterCascade.append(vFilterCascade[i]);
	}
}

void FFT::draw(RenderWindow &window)
{
	window.draw(VA1) ; //time domain
	window.draw(VA3) ; //cascade
	window.draw(VA2) ; //frequency domain
}
void FFT::drawFSignal(RenderWindow &window)
{
	window.draw(ftimesignal);
	window.draw(filterBars);
	window.draw(filterCascade);
}
//pauses or plays sound when spacebar is hit
void FFT::ppSound()
{
	if(sound.getStatus() == Sound::Status::Paused)
	{
		sound.play();
		fsound.play();
	}
	else 
	{
		sound.pause();
		fsound.pause();
	}
}
//rewinds pressing LEFT key
void FFT::rewind()
{
	sound.setPlayingOffset(sound.getPlayingOffset()-sf::milliseconds(250));
	fsound.setPlayingOffset(fsound.getPlayingOffset()-sf::milliseconds(250));
//	cout << sound.getPlayingOffset().asSeconds() << endl;
}
//seek pressing RIGHT key
void FFT::seek()
{
	sound.setPlayingOffset(sound.getPlayingOffset()+sf::milliseconds(250));
	fsound.setPlayingOffset(fsound.getPlayingOffset()+sf::milliseconds(250));
//	cout << sound.getPlayingOffset().asSeconds() << endl;
}
//decrease pitch with DOWN key
void FFT::dPitch()
{
	if(sound.getPitch() >= 0.002)
	{
		sound.setPitch(sound.getPitch()-0.002);
		fsound.setPitch(fsound.getPitch()-0.002);
	}
	else
	{
		sound.setPitch(0.00);
		fsound.setPitch(0.00);
	}
}
//increase pitch with UP key
void FFT::uPitch()
{
	if(sound.getPitch() <= 0.998)
	{
		sound.setPitch(sound.getPitch()+0.002);
		fsound.setPitch(fsound.getPitch()+0.002);
	}
	else
	{
		sound.setPitch(1.00);
		fsound.setPitch(1.00);
	}
}
//reset pitch with R key
void FFT::rPitch()
{
	sound.setPitch(1.00);
	fsound.setPitch(1.00);
}

void FFT::moveSound3d(int dir)
{
	/*
	0 --> reset x,y,z
	
	-1 --> -x
	1 --> x
	
	-2 --> -y
	2 --> y
	
	-3 --> -z
	3 --> z
	
	*/
	Vector3f temp = sound.getPosition();
	
	switch(dir)
	{
		case 0:
			break;
			
		case -1:
			temp.x -= 0.01f;
			break;
		case 1:
			temp.x += 0.01f;
			break;
			
		case -2:
			temp.y -= 0.01f;
			break;
		case 2:
			temp.y += 0.01f;
			break;
			
		case -3:
			temp.z -= 0.01f;
			break;
		case 3:
			temp.z += 0.01f;
			break;
		default:
			break;
	}
	sound.setPosition(temp);
}

bool FFT::initFilters()
{
//	generalFilter.push_back(new Filter(LPF, 50, 44.1, 1.0));
//	generalFilter.push_back(new Filter(HPF, 50, 44.1, 1.0, 5.0));
//	generalFilter.push_back(new Filter(BPF, 50, 44.1, 1.0, 5.0));
	
	generalFilter.push_back(new Filter(LPF, 50, 44.1, 0.5));
	generalFilter.push_back(new Filter(HPF, 50, 44.1, 5.0));
	generalFilter.push_back(new Filter(BPF, 50, 44.1, 1.0, 4.0));
	
	for(int i=0; i< generalFilter.size(); i++)
	{
		if(generalFilter.at(i)->get_error_flag() < 0 ) return false;
	}
}
bool FFT::filter()
{
	Filter* lowpass = new Filter(LPF, 51, 44.1, 2.0);
	signed short tempsample;
	filterSamples = new Int16[buffer.getSampleCount()];
	
	if( lowpass->get_error_flag() < 0 ) return false;
	
	for(Uint64 i=0; i<buffer.getSampleCount(); i++)
	{
		tempsample = lowpass->do_sample((double)buffer.getSamples()[i]);
		filterSamples[i] = tempsample;
	}
	
	cout << "Done filtering" << endl;
	delete lowpass;
	
}


void FFT::updateFilterSound()
{
	lfSamples = new Int16[buffer.getSampleCount()];
	double tempSample;
	switch(choiceFilter)
	{
		case 0:
			for(int i=0; i<buffer.getSampleCount(); i++)
			{
				tempSample = (double)buffer.getSamples()[i];
				lfSamples[i] = lpfSample(tempSample);
			}
			cout << "LOG > Applied low pass filter!" << endl;
			break;
		case 1:
			for(int i=0; i<buffer.getSampleCount(); i++)
			{
				tempSample = (double)buffer.getSamples()[i];
				lfSamples[i] = hpfSample(tempSample);
			}
			cout << "LOG > Applied high pass filter!" << endl;
			break;
		case 2:
			for(int i=0; i<buffer.getSampleCount(); i++)
			{
				tempSample = (double)buffer.getSamples()[i];
				lfSamples[i] = bpfSample(tempSample);
			}
			cout << "LOG > Applied band pass filter!" << endl;
			break;
	}
	
	fbuffer.loadFromSamples(lfSamples, buffer.getSampleCount(), buffer.getChannelCount(), buffer.getSampleRate());
	fsound.setBuffer(fbuffer);
	
}
Int16 FFT::lpfSample(double& s)
{
	return generalFilter.at(0)->do_sample( (double) s);
}

Int16 FFT::hpfSample(double& s)
{
	return generalFilter.at(1)->do_sample( (double) s);
}

Int16 FFT::bpfSample(double& s)
{
	return generalFilter.at(2)->do_sample( (double) s);
}

void FFT::switchSource(bool s)
{
	if(!s)
	{
		fsound.setVolume(0.0f);
		sound.setVolume(100.0f);
	}
	else
	{
		sound.setVolume(0.0f);
		fsound.setVolume(100.0f);
	}
	
	
}

} //end ns
