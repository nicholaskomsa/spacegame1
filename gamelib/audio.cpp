#include "audio.h"

#include "exception.h"
#include <windows.h>
#include <fstream>
#include <algorithm>
#include <sstream>


SoundFile::SoundFile(std::string fileName)
	:mFileName(fileName) {}

SoundFile::SoundData SoundFile::getData() { return mData; }
ALenum SoundFile::getFormat() { return mFormat; }
ALsizei SoundFile::getFrequency() { return mFrequency; }
std::string SoundFile::getFileName() { return mFileName; }
ALsizei SoundFile::getSize() { return mData.size(); }
void SoundFile::clear() { mData.clear(); }



WavFile::WavFile(std::string fileName)
	:SoundFile(fileName)
{}

bool WavFile::load() {
	std::ifstream fin(mFileName, std::ifstream::ate | std::ifstream::binary);
	std::size_t fileSize = fin.tellg();
	fin.seekg(0);

	WaveFormat waveFormat;
	RiffHeader riffHeader;

	const __int32 idData = *reinterpret_cast<int*>(const_cast<char*>("data"));
	const __int32 idRiff = *reinterpret_cast<int*>(const_cast<char*>("RIFF"));
	const __int32 idWave = *reinterpret_cast<int*>(const_cast<char*>("WAVE"));
	const __int32 idFmt = *reinterpret_cast<int*>(const_cast<char*>("fmt "));


	ChunkHeader header;

	auto convertIDToStr = [&](int id) -> std::string {
		std::string s; s.resize(sizeof(int) + 1);
		for (int i = 0; i < sizeof(int); i++)
			s[i] = reinterpret_cast<char*>(&id)[i];
		return s;
	};

	fin.read(reinterpret_cast<char*>(&header), sizeof(header));
	if (header.chunkID != idRiff)
		EXCEPT << "WaveFile::load, riffID is incorrect! " << convertIDToStr(header.chunkID);

	fin.read(reinterpret_cast<char*>(&riffHeader), sizeof(riffHeader));
	if (riffHeader.waveID != idWave)
		EXCEPT << "WaveFile::load, waveID is incorrect! " << convertIDToStr(header.chunkID);

	//OK, it claims it is a valid WAVE, so proceed to load chunks
	do {
		fin.read(reinterpret_cast<char*>(&header), sizeof(header));

		if (header.chunkID == idFmt) {
			switch (header.chunkSize) {
			case 16:
				fin.read(reinterpret_cast<char*>(&waveFormat), 16);
				break;
			case 18:
				fin.read(reinterpret_cast<char*>(&waveFormat), 18);
				break;
			case 40:
				fin.read(reinterpret_cast<char*>(&waveFormat), 40);
				break;
			default:
				EXCEPT << "WaveFile::load, idFmt: chunkSize invalid! " << header.chunkSize;
			}
		}
		else if (header.chunkID == idData) {
			mData.resize(header.chunkSize);

			fin.read(&mData[0], header.chunkSize);

			//read the evenness padding byte 
			if (header.chunkSize % 2 != 0) {
				char c;
				fin.read(&c, sizeof(1));
			}
		}
		else {
			//skip this block!
			char* c = reinterpret_cast<char*>(&header.chunkID);
			//simple check to make sure we dont do anything absurd incase format is invalid
			std::size_t pos = fin.tellg();
			if (pos + header.chunkSize > fileSize || header.chunkSize == 0)
				EXCEPT << "WaveFile::load, skip: invalid chunkSize! " << convertIDToStr(header.chunkID) << " " << header.chunkSize;

			std::string data; data.resize(header.chunkSize);
			fin.read(const_cast<char*>(data.c_str()), data.size());
		}

		if (fin.peek() == EOF) break;
	} while (!fin.eof());

	fin.close();

	mFrequency = waveFormat.sampleRate;
	mChannels = waveFormat.numChannels;

	if (waveFormat.numChannels == 1) {
		if (waveFormat.bitsPerSample == 8)
			mFormat = AL_FORMAT_MONO8;
		else if (waveFormat.bitsPerSample == 16)
			mFormat = AL_FORMAT_MONO16;
	}
	else if (waveFormat.numChannels == 2) {
		if (waveFormat.bitsPerSample == 8)
			mFormat = AL_FORMAT_STEREO8;
		else if (waveFormat.bitsPerSample == 16)
			mFormat = AL_FORMAT_STEREO16;
	}
	return true;
}


void Sound::create(std::size_t maxUses, SoundFile* soundFile) {
	mMaxUses = maxUses;

	alGenBuffers(1, &mBuffer);

	alBufferData(mBuffer, soundFile->getFormat(), (void*)&soundFile->getData()[0],
		soundFile->getSize(), soundFile->getFrequency());
}
void Sound::destroy() {
	alDeleteBuffers(1, &mBuffer);                                                 //Delete the OpenAL Buffer 
}
bool Sound::available() {

	return mUseCounter < mMaxUses;
}
void Sound::use() {
	mUseCounter++;
}
void Sound::free() {
	if (mUseCounter > 0)
		mUseCounter--;
}
bool Sound::getPlayed() {
	return mPlayed;
}
void Sound::setPlayed(bool played) {
	mPlayed = played;
}
ALuint Sound::getBuffer() {
	return mBuffer;
}

bool  SoundSource::create() {

	alGetError();
	alGenSources(1, &mSource);

	if (alGetError() != AL_NO_ERROR) return false;

	alSourcei(mSource, AL_BUFFER, NULL);

	return true;
}
void  SoundSource::setSound(Sound* sound) {
	if (sound == nullptr || !sound->available()) {
		mSound = nullptr;
		return;
	}
	mSound = sound;
	mSound->use();

	alSourcei(mSource, AL_BUFFER, mSound->getBuffer());
}
void SoundSource::freeSound() {
	if (mSound) mSound->free();
}
void  SoundSource::destroy() {
	if (mSource == 0) return;
	stop();

	alGetError();
	alSourcei(mSource, AL_BUFFER, NULL);

	alGetError();
	alDeleteSources(1, &mSource);
}
ALuint  SoundSource::getSourceID() {
	return mSource;
}
void  SoundSource::play() {
	if (mSound) {
		if (mSound->getPlayed()) return;
  		mSound->setPlayed(true);
		alSourcePlay(mSource);
	}
}
void  SoundSource::stop() {
	alSourceStop(mSource);
}
void  SoundSource::wait() {
	ALint sourceState = 0;
	do {
		alGetSourcei(mSource, AL_SOURCE_STATE, &sourceState);
	} while (sourceState == AL_PLAYING);
}
bool  SoundSource::isPlaying() {
	if (mSource == 0) return false;
	ALint sourceState = 0;
	alGetError();
	alGetSourcei(mSource, AL_SOURCE_STATE, &sourceState);
	if (sourceState == AL_PLAYING) return true;
	return false;
}

void  SoundSource::setPitch(ALfloat pitch) {
	alSourcef(mSource, AL_PITCH, pitch);                                 //Set the pitch of the source 
}
void  SoundSource::setGain(ALfloat gain) {

	alSourcef(mSource, AL_GAIN, gain);     //Set the gain of the source 
}
void  SoundSource::setPosition(ALfloat x, ALfloat y, ALfloat z) {
	ALfloat pos[] = { x, y, z };
	alSourcefv(mSource, AL_POSITION, pos);                                 //Set the position of the source 
}
void  SoundSource::setVelocity(ALfloat x, ALfloat y, ALfloat z) {
	ALfloat vel[] = { x, y, z };
	alSourcefv(mSource, AL_VELOCITY, vel);
}
void  SoundSource::setLooping(bool looping) {
	alSourcei(mSource, AL_LOOPING, looping ? AL_TRUE : AL_FALSE); //Set if source is looping sound 
}

Audio::SoundSlot::SoundSlot(SoundSource& source)
	:source(source)
{}
Audio::Audio() {
	// Initialize Open AL

	mDevice = alcOpenDevice(NULL); // open default device

	if (mDevice == NULL)
		EXCEPT << "Audio::Audio mDevice is NULL";

	mContext = alcCreateContext(mDevice, NULL); // create context

	if (mContext == NULL)
		EXCEPT << "Audio::Audio mContext is NULL";


	alcMakeContextCurrent(mContext); // set active context


	createMaxSources();
	std::size_t sources = mSoundSlots.size();

	mSoundSlotsIt = mSoundSlots.end();
}

Audio::~Audio() {
	std::for_each(mSoundSlots.begin(), mSoundSlots.end(), [&](SoundSlot& src) {
		src.source.destroy();
	});
	std::for_each(mSounds.begin(), mSounds.end(), [&](auto& pair) {
		pair.second.destroy();
	});

	alcMakeContextCurrent(NULL);                                                //Make no context current 
	alcDestroyContext(mContext);                                                 //Destroy the OpenAL Context 
	alcCloseDevice(mDevice);                                                     //Close the OpenAL Device 
}

Audio::SoundSlot* Audio::requestSoundSource() {

	//loop through all sources and see if there are any that are free.
	if (mSoundSlotsIt == mSoundSlots.end())
		mSoundSlotsIt = mSoundSlots.begin();

	SoundSlot* selectedSlot = NULL;
	mSoundSlotsIt = std::find_if(mSoundSlots.begin(), mSoundSlots.end(), [&](SoundSlot& slot) {

		if (slot.active) {
			if (slot.source.isPlaying())
				return false;
			else if (!slot.deactiveOnStop)
				return false;
		}
		slot.deactivate();
		//if we get here then we are selecting this slot
		slot.activate();
		selectedSlot = &slot;
		return true;
	});

	return selectedSlot;
}
void Audio::createSound(std::string name, std::size_t maxUses, SoundFile* file) {
	file->load();
	Sound sound; sound.create(maxUses, file);
	mSounds[name] = sound;
}
void Audio::createSound(std::string name, std::size_t maxUses, std::string fileName) {

	//get the filetype
	std::string fileType;
	std::size_t r = fileName.rfind(".");
	fileType = fileName.substr(r);

	if (fileType == ".wav") {
		WavFile wavFile1(mPathPrefix + "/" + fileName);
		createSound(name, maxUses, &wavFile1);
	}
	else
		EXCEPT << "Audio::createSound; unsupported file extension!";
}
Sound* Audio::getSound(std::string name) {
	return &mSounds[name];
}
void Audio::clearPlayed() {
	std::for_each(mSounds.begin(), mSounds.end(), [&](auto& pair) {
		pair.second.setPlayed(false);
	});
}
void Audio::createMaxSources() {

	std::size_t max = 10000;
	for( std::size_t i=1; i< max; ++i){
		SoundSource source;
		if (source.create())
			mSoundSlots.push_back(SoundSlot(source));
		else
			break;
	} 
}
void Audio::setListenerPosition(ALfloat x, ALfloat y, ALfloat z) {
	ALfloat pos[] = { x,y,z };
	alGetError();
	alListenerfv(AL_POSITION, pos);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) EXCEPT << "Audio::Audio setListenerPosition error! " << error;
}
void Audio::setListeneVelocity(ALfloat x, ALfloat y, ALfloat z) {
	ALfloat vel[] = { x,y,z };
	alGetError();
	alListenerfv(AL_VELOCITY, vel);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) EXCEPT << "Audio::Audio setListeneVelocity error! " << error;

}
void Audio::setListenerOrientation(ALfloat x, ALfloat y, ALfloat z) {
	ALfloat orient[] = { x,y,z };
	alGetError();
	alListenerfv(AL_ORIENTATION, orient);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) EXCEPT << "Audio::Audio setListenerOrientation error! " << error;
}
void Audio::setPathPrefix(std::string prefix) {
	mPathPrefix = prefix;
}