#ifndef FILTERTOOLS_H
#define FILTERTOOLS_H
#include <SFML/Audio.hpp>
namespace Audio
{
class SoundWriter : public sf::SoundFileWriter
{
public:
    static bool check(const std::string& filename)
    {
        // typically, check the extension
        // return true if the writer can handle the format
    }
    virtual bool open(const std::string& filename, unsigned int sampleRate, unsigned int channelCount)
    {
        // open the file 'filename' for writing,
        // write the given sample rate and channel count to the file header
        // return true on success
    }
    virtual void write(const sf::Int16* samples, sf::Uint64 count)
    {
        // write 'count' samples stored at address 'samples',
        // convert them (for example to normalized float) if the format requires it
    }
};
//sf::SoundFileFactory::registerWriter<MySoundFileWriter>();

}
#endif
