#include <AL/al.h>
#include <AL/alc.h>

#include <vector>
#include <map>
#include <chrono>

#pragma once

class SoundFile {
public:
	typedef char SoundDataType;
	typedef std::vector<SoundDataType> SoundData;

protected:
	ALsizei mFrequency;
	ALenum mFormat;
	ALsizei mChannels;

	SoundData mData;
	std::string mFileName;
public:
	SoundFile(std::string fileName);
	virtual bool load() = 0;


	SoundData getData();
	ALenum getFormat();
	ALsizei getFrequency();
	std::string getFileName();
	ALsizei getSize();
	void clear();
};


struct ChunkHeader {
	__int32 chunkID;
	unsigned __int32 chunkSize;
};

struct RiffHeader {
	__int32 waveID;
};

struct WaveFormat {
	unsigned __int16 audioFormat;
	unsigned __int16 numChannels;
	unsigned __int32 sampleRate;
	unsigned __int32 byteRate;
	unsigned __int16 blockAlign;
	unsigned __int16 bitsPerSample;
	//wave 18
	unsigned __int16 cbSize{ 0 };
	//wave 40
	unsigned __int16 validBitsPerSample{ 0 };
	unsigned __int32 channelMask{ 0 };
	char subFormat[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
};


class WavFile :public SoundFile {
public:
	WavFile(std::string fileName);
	bool load();
};



class Sound {
protected:

	ALuint mBuffer{ 0 };
	std::size_t mUseCounter{ 0 };
	std::size_t mMaxUses{ 0 };

	bool mPlayed{ false };
public:

	void create(std::size_t maxUses, SoundFile* soundFile);
	void destroy();

	bool available();
	void use();
	void free();

	ALuint getBuffer();

	bool getPlayed();
	void setPlayed(bool played = true);
};


class SoundSource {
	ALuint mSource{ 0 };
	Sound* mSound{ nullptr };
public:
	bool create();
	void setSound(Sound* sound);
	void freeSound();

	void destroy();
	ALuint getSourceID();
	void play();
	void stop();
	void wait();
	bool isPlaying();
	void setPitch(ALfloat pitch = 1.0f);
	void setGain(ALfloat gain = 1.0f);
	void setPosition(ALfloat x = 0, ALfloat y = 0, ALfloat z = 0);
	void setVelocity(ALfloat x = 0, ALfloat y = 0, ALfloat z = 0);
	void setLooping(bool looping);
};


class Audio {
public:
	class SoundSlot {
		friend class Audio;

		bool active{ false };


		void activate() {
			active = true;
			deactiveOnStop = false;
			source.setGain();
			source.setPitch();
			source.setPosition();
			source.setVelocity();
			source.setLooping(false);
		}
	public:
		SoundSlot(SoundSource& source);

		SoundSource source;
		bool deactiveOnStop{ false };

		void deactivate() {
			active = false;
			source.stop();
			source.freeSound();
		}
	};
private:

	ALCdevice * mDevice;
	ALCcontext* mContext;

	ALCint mMaxSlots;

	std::map< std::string, Sound > mSounds;

	std::string mPathPrefix;

	std::vector<SoundSlot> mSoundSlots;
	std::vector<SoundSlot>::iterator mSoundSlotsIt;
public:

	Audio();
	~Audio();

	SoundSlot* requestSoundSource();
	void createSound(std::string name, std::size_t maxUses, SoundFile* file);
	void createSound(std::string name, std::size_t maxUses, std::string fileName);

	Sound* getSound(std::string name);

	void clearPlayed();


	void createMaxSources();
	void setListenerPosition(ALfloat x = 0, ALfloat y = 0, ALfloat z = 0);
	void setListeneVelocity(ALfloat x = 0, ALfloat y = 0, ALfloat z = 0);
	void setListenerOrientation(ALfloat x = 0, ALfloat y = 0, ALfloat z = 0);
	void setPathPrefix(std::string prefix);

};

