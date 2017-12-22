#Test:Global.o Lock.o Thread.o Event.o RemoteFilePlayer.o PESPacketHead.o RemotePSFile.o DVRSocket.o RemoteConfig.o PSWriter.o SpeechFifo.o SpeechSocket.o DvrSpeech.o DvsPro16.o Dvs.o StreamSocket.o StreamFifo.o DeviceStream.o AudioStream.o PSFile.o LocalFilePlayer.o libNetDVRSDK.so 
#	g++ -o Test test.cpp Global.o Lock.o Thread.o Event.o RemoteFilePlayer.o PESPacketHead.o RemotePSFile.o DVRSocket.o RemoteConfig.o PSWriter.o SpeechFifo.o SpeechSocket.o DvrSpeech.o DvsPro16.o  Dvs.o StreamSocket.o StreamFifo.o DeviceStream.o AudioStream.o PSFile.o LocalFilePlayer.o libNetDVRSDK.so
MAIN_OBJS	= Global.o Lock.o Thread.o Event.o RemoteFilePlayer.o PESPacketHead.o RemotePSFile.o DVRSocket.o RemoteConfig.o PSWriter.o SpeechFifo.o SpeechSocket.o DvrSpeech.o DvsPro16.o  Dvs.o StreamSocket.o StreamFifo.o DeviceStream.o AudioStream.o PSFile.o LocalFilePlayer.o 
	
libNetDVRSDK.a:$(MAIN_OBJS) NetDvrApi.o 
	ar rcu $@ $(MAIN_OBJS) NetDvrApi.o 

	
Global.o:Global.cpp Global.h
	g++ -g -c Global.cpp -o Global.o 

Lock.o:Lock.cpp Lock.h 
	g++ -g -c Lock.cpp -o Lock.o
	
Thread.o:Thread.cpp Thread.h 
	g++ -g -c Thread.cpp -o Thread.o
	
Event.o:Event.cpp Event.h 
	g++ -g -c Event.cpp -o Event.o

RemoteFilePlayer.o:RemoteFilePlayer.cpp RemoteFilePlayer.h
	g++ -g -c RemoteFilePlayer.cpp -o RemoteFilePlayer.o 
	
PESPacketHead.o:PESPacketHead.cpp PESPacketHead.h 
	g++ -g -c PESPacketHead.cpp -o PESPacketHead.o	
	
RemotePSFile.o:RemotePSFile.cpp RemotePSFile.h 
	g++ -g -c RemotePSFile.cpp -o RemotePSFile.o
	
DVRSocket.o:DVRSocket.cpp DVRSocket.h 
	g++ -g -c DVRSocket.cpp -o DVRSocket.o
	
RemoteConfig.o:RemoteConfig.cpp RemoteConfig.h 
	g++ -g -c RemoteConfig.cpp -o RemoteConfig.o 

StreamSocket.o:StreamSocket.cpp StreamSocket.h
	g++ -g -c StreamSocket.cpp -o StreamSocket.o
	
SpeechFifo.o:SpeechFifo.cpp SpeechFifo.h
	g++ -g -c SpeechFifo.cpp -o SpeechFifo.o

SpeechSocket.o:SpeechSocket.cpp SpeechSocket.h
	g++ -g -c SpeechSocket.cpp -o SpeechSocket.o

DvrSpeech.o:DvrSpeech.cpp DvrSpeech.h 
	g++ -g -c DvrSpeech.cpp -o DvrSpeech.o 
	
PSWriter.o:PSWriter.cpp PSWriter.h 
	g++ -g -c PSWriter.cpp -o PSWriter.o 

DvsPro16.o:DvsPro16.cpp DvsPro16.h 
	g++ -g -c DvsPro16.cpp -o DvsPro16.o	
	
Dvs.o:Dvs.cpp Dvs.h
	g++ -g -c Dvs.cpp -o Dvs.o 	

StreamFifo.o:StreamFifo.cpp StreamFifo.h
	g++ -g -c StreamFifo.cpp -o StreamFifo.o 	
	
DeviceStream.o:DeviceStream.cpp DeviceStream.h
	g++ -g -c DeviceStream.cpp -o DeviceStream.o 	
	
AudioStream.o:AudioStream.cpp AudioStream.h
	g++ -g -c AudioStream.cpp -o AudioStream.o 
			
PSFile.o:PSFile.cpp PSFile.h
	g++ -g -c PSFile.cpp -o PSFile.o 
			
LocalFilePlayer.o:LocalFilePlayer.cpp LocalFilePlayer.h
	g++ -g -c LocalFilePlayer.cpp -o LocalFilePlayer.o 
	
#libNetDVRSDK.so:NetDvrApi.o 
#	g++ -g -shared -Wl $(MAIN_OBJS) NetDvrApi.o -lc -o libNetDVRSDK.so -lpthread
    
NetDvrApi.o:NetDvrApi.cpp NetDvrApi.h
	g++ -g -c NetDvrApi.cpp -o NetDvrApi.o 
    

clean:
	rm -f *.o *.so Test
