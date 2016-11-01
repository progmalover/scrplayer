#ifndef _MESSAGE_DEFINE_H_2014_08_20_LC_
#define _MESSAGE_DEFINE_H_2014_08_20_LC_

enum   PROGRASSSTATE
{
	COMPRESS,
	DECOMPRESS,
	TRANSFER
};
/*
enum   Classification{
	INIT,
	ADDMEMBER,
	IMAGE,
	AUDIO,
    DOCUMENT,
    SELECTED,
    INFO,
    HOMEWORKQUEST,
	UNCONNECTED,
	MONITOR,
	RECIEVORNOTIFY,
	CALLSIGN,
	BLACKLOCK,
	REMOTECOMMAND,
	MP3STOP,
	VOLUMNCONTROL,
	MESSAGE,
	RAISE,
	VIDEOTECH,
	STUDENTDEMO,
	MOUSE,
	LOCK,
	MAC,
	SURFINTERNET,
	SENDFILE,
	FILETRANSFER,
	TRANSFERPAUSELOAD,
	TRANSFERSTOPLOAD
};*/

enum   Classification{
	INIT,
	ADDMEMBER,
	IMAGE,
	AUDIO,
    	DOCUMENT,
    	SELECTED,
    	INFO,
    	HOMEWORKQUEST,
	UNCONNECTED,
	MONITOR,
	RECIEVORNOTIFY,
	CALLSIGN,
	BLACKLOCK,
	REMOTECOMMAND,
	MP3STOP,
	VOLUMNCONTROL,
	MESSAGE,
	RAISE,
	VIDEOTECH,
	STUDENTDEMO,
	MOUSE,
	LOCK,
	MAC,
	SURFINTERNET,
	SENDFILE,
	FILETRANSFER,
	TRANSFERPAUSELOAD,
	TRANSFERSTOPLOAD,
	STUDENTCONFIG,
	MULTICASTSCREEN,
	MULTICASTSTUDENTDEMOSENDER,
	MULTICASTSTUDENTDEMORECIEVOR,
	MULTICASTSTUDENTDEMOCAPTURE,
	TESTSTUUI,
	TESTSTUSTART,
	TESTSTUEND
};


struct  Header
{
	Classification id;
	int  operation;
	int  deliver;
	int  externeds;
};

struct  ImageConfig
{
	int width;
	int heigth;
	int iFullScreen;
	int ilock;
};

struct  Config
{
	int  width;
	int  heigth;
};

struct  Mouse
{
	int message;
	float  dx;
	float  dy;
};

struct  MultiCaseImageDataHeader
{
	int width;
	int heigth;
};

struct  AudioConfig
{
	int  itags;
	int  ichannel;
};
#endif
