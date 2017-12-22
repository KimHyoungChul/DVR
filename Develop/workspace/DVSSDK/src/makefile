Test:Global.o Lock.o Thread.o Event.o RemoteFilePlayer.o PESPacketHead.o RemotePSFile.o DVRSocket.o RemoteConfig.o PSWriter.o SpeechFifo.o SpeechSocket.o DvrSpeech.o DvsPro16.o Dvs.o StreamSocket.o StreamFifo.o DeviceStream.o AudioStream.o PSFile.o LocalFilePlayer.o libNetDVRSDK.so 
	g++ -o Test test.cpp Global.o Lock.o Thread.o Event.o RemoteFilePlayer.o PESPacketHead.o RemotePSFile.o DVRSocket.o RemoteConfig.o PSWriter.o SpeechFifo.o SpeechSocket.o DvrSpeech.o DvsPro16.o  Dvs.o StreamSocket.o StreamFifo.o DeviceStream.o AudioStream.o PSFile.o LocalFilePlayer.o libNetDVRSDK.so
	
MAIN_OBJS	= Global.o Lock.o Thread.o Event.o RemoteFilePlayer.o PESPacketHead.o RemotePSFile.o DVRSocket.o RemoteConfig.o PSWriter.o SpeechFifo.o SpeechSocket.o DvrSpeech.o DvsPro16.o  Dvs.o StreamSocket.o StreamFifo.o DeviceStream.o AudioStream.o PSFile.o LocalFilePlayer.o 
	
Global.o:Global.cpp Global.h
	g++ -g -fPIC -c Global.cpp -o Global.o 

Lock.o:Lock.cpp Lock.h 
	g++ -g -fPIC -c Lock.cpp -o Lock.o
	
Thread.o:Thread.cpp Thread.h 
	g++ -g -fPIC -c Thread.cpp -o Thread.o
	
Event.o:Event.cpp Event.h 
	g++ -g -fPIC -c Event.cpp -o Event.o

RemoteFilePlayer.o:RemoteFilePlayer.cpp RemoteFilePlayer.h
	g++ -g -fPIC -c RemoteFilePlayer.cpp -o RemoteFilePlayer.o 
	
PESPacketHead.o:PESPacketHead.cpp PESPacketHead.h 
	g++ -g -fPIC -c PESPacketHead.cpp -o PESPacketHead.o	
	
RemotePSFile.o:RemotePSFile.cpp RemotePSFile.h 
	g++ -g -fPIC -c RemotePSFile.cpp -o RemotePSFile.o
	
DVRSocket.o:DVRSocket.cpp DVRSocket.h 
	g++ -g -fPIC -c DVRSocket.cpp -o DVRSocket.o
	
RemoteConfig.o:RemoteConfig.cpp RemoteConfig.h 
	g++ -g -fPIC -c RemoteConfig.cpp -o RemoteConfig.o 

StreamSocket.o:StreamSocket.cpp StreamSocket.h
	g++ -g -fPIC -c StreamSocket.cpp -o StreamSocket.o
	
SpeechFifo.o:SpeechFifo.cpp SpeechFifo.h
	g++ -g -fPIC -c SpeechFifo.cpp -o SpeechFifo.o

SpeechSocket.o:SpeechSocket.cpp SpeechSocket.h
	g++ -g -fPIC -c SpeechSocket.cpp -o SpeechSocket.o

DvrSpeech.o:DvrSpeech.cpp DvrSpeech.h 
	g++ -g -fPIC -c DvrSpeech.cpp -o DvrSpeech.o 
	
PSWriter.o:PSWriter.cpp PSWriter.h 
	g++ -g -fPIC -c PSWriter.cpp -o PSWriter.o 

DvsPro16.o:DvsPro16.cpp DvsPro16.h 
	g++ -g -fPIC -c DvsPro16.cpp -o DvsPro16.o	
	
Dvs.o:Dvs.cpp Dvs.h
	g++ -g -fPIC -c Dvs.cpp -o Dvs.o 	

StreamFifo.o:StreamFifo.cpp StreamFifo.h
	g++ -g -fPIC -c StreamFifo.cpp -o StreamFifo.o 	
	
DeviceStream.o:DeviceStream.cpp DeviceStream.h
	g++ -g -fPIC -c DeviceStream.cpp -o DeviceStream.o 	
	
AudioStream.o:AudioStream.cpp AudioStream.h
	g++ -g -fPIC -c AudioStream.cpp -o AudioStream.o 
			
PSFile.o:PSFile.cpp PSFile.h
	g++ -g -fPIC -c PSFile.cpp -o PSFile.o 
			
LocalFilePlayer.o:LocalFilePlayer.cpp LocalFilePlayer.h
	g++ -g -fPIC -c LocalFilePlayer.cpp -o LocalFilePlayer.o 
	
libNetDVRSDK.so:NetDvrApi.o 
	g++ -g -shared -Wl $(MAIN_OBJS) NetDvrApi.o -lc -o libNetDVRSDK.so -lpthread
    
NetDvrApi.o:NetDvrApi.cpp NetDvrApi.h
	g++ -g -fPIC -c NetDvrApi.cpp -o NetDvrApi.o 
    
all:Test


clean:
	rm -f *.o *.so Test
