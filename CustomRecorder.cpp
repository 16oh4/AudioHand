#include "FFT.h"
namespace Audio
{
class CustomRecorder : public sf::SoundRecorder
{
    ~CustomRecorder()
    {
        // Make sure to stop the recording thread
        stop();
    }
    virtual bool onStart() // optional
    {
        // Initialize whatever has to be done before the capture starts
        
        // Return true to start playing
        return true;
    }
    virtual bool onProcessSamples(const Int16* samples, std::size_t sampleCount)
    {
        // Do something with the new chunk of samples (store them, send them, ...)
        
        // Return true to continue playing
        return true;
    }
    virtual void onStop() // optional
    {
        // Clean up whatever has to be done after the capture ends
    }
};
//// Usage
//if (CustomRecorder::isAvailable())
//{
//    CustomRecorder recorder;
//    if (!recorder.start())
//        return -1;
//    ...
//    recorder.stop();
//}
}
