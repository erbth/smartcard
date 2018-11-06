#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winscard.h>

#pragma comment(lib, "winscard.lib")


char* getStateString(int ret) {
	static char buf[2][2000];
	char* err;
	static i = 0;
	
	err = buf[i];
	i++;
	
	if(i > 1)
		i = 0;
	
	*err = 0;
	
	if(ret & SCARD_STATE_UNAWARE)
		strcat(err, "SCARD_STATE_UNAWARE ");
	
	if(ret & SCARD_STATE_IGNORE)
		strcat(err, "SCARD_STATE_IGNORE ");
	
	if(ret & SCARD_STATE_CHANGED)
		strcat(err, "SCARD_STATE_CHANGED ");
	
	if(ret & SCARD_STATE_UNKNOWN)
		strcat(err, "SCARD_STATE_UNKNOWN ");
	
	if(ret & SCARD_STATE_UNAVAILABLE)
		strcat(err, "SCARD_STATE_UNAVAILABLE ");
	
	if(ret & SCARD_STATE_EMPTY)
		strcat(err, "SCARD_STATE_EMPTY ");
	
	if(ret & SCARD_STATE_PRESENT)
		strcat(err, "SCARD_STATE_PRESENT ");
	
	if(ret & SCARD_STATE_EXCLUSIVE)
		strcat(err, "SCARD_STATE_EXCLUSIVE ");
	
	if(ret & SCARD_STATE_INUSE)
		strcat(err, "SCARD_STATE_INUSE ");
	
	if(ret & SCARD_STATE_MUTE)
		strcat(err, "SCARD_STATE_MUTE ");
	
	return err;
}


char* getErrorString(LONG ret) {
	if(ret == SCARD_S_SUCCESS)
		return "SCARD_S_SUCCESS";
	
	if(ret == SCARD_E_NO_SERVICE)
		return "SCARD_E_NO_SERVICE";
	
	if(ret == SCARD_E_INVALID_PARAMETER)
		return "SCARD_E_INVALID_PARAMETER";
	
	if(ret == SCARD_E_INVALID_VALUE)
		return "SCARD_E_INVALID_VALUE";
	
	if(ret == SCARD_E_INVALID_HANDLE)
		return "SCARD_E_INVALID_HANDLE";
	
	if(ret == SCARD_E_READER_UNAVAILABLE)
		return "SCARD_E_READER_UNAVAILABLE";
	
	if(ret == SCARD_E_TIMEOUT)
		return "SCARD_E_TIMEOUT";
	
	return "--";
}


int main(int argc, char** argv) {
	LONG ret;
	int i;
	
	SCARDCONTEXT hContext;
	LPSTR mszReaders;
	DWORD dwReaders;
	SCARD_READERSTATE rgReaderState;
	
	printf("\tINFINITE: %08X (%d)\n", INFINITE, INFINITE);
	
	ret = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
	if(ret != SCARD_S_SUCCESS) {
		fprintf(stderr, "couldn't establish context\n");
		return EXIT_FAILURE;
	}
	
	ret = SCardListReadersA(hContext, NULL, NULL, &dwReaders);
	if(ret != SCARD_S_SUCCESS) {
		fprintf(stderr, "error listing readers: %s\n", getErrorString(ret));
		goto EXCEPTION;
	}
	
	mszReaders = calloc(dwReaders, sizeof(char));
	if(!mszReaders) {
		fprintf(stderr, "didn't get free memory anymore ...\n");
		goto EXCEPTION;
	}
	
	ret = SCardListReadersA(hContext, NULL, mszReaders, &dwReaders);
	if(ret != SCARD_S_SUCCESS) {
		fprintf(stderr, "error listing readers\n");
		free(mszReaders);
		goto EXCEPTION;
	}
	
	printf("first Reader: %s (%d/%d)\n\n\n", mszReaders, strlen(mszReaders), dwReaders);
	
	
	//bzero(&rgReaderState, sizeof(rgReaderState));
	for(i = 0; i < sizeof(rgReaderState); i++) {
		((BYTE *) &rgReaderState)[i] = 0;
	}
	
	rgReaderState.szReader = mszReaders;
	rgReaderState.dwEventState = SCARD_STATE_UNKNOWN;
	rgReaderState.dwCurrentState = SCARD_STATE_UNKNOWN;
	
	for(i = 0; i < 2; i++) {
		printf("%s (%X)\nszReader: %s\ndwCurrentState: %s (%X)\ndwEventState: %s (%X)\ncbAtr: %d\n\n", getErrorString(ret), ret,
			rgReaderState.szReader, getStateString(rgReaderState.dwCurrentState), rgReaderState.dwCurrentState, getStateString(rgReaderState.dwEventState),
			rgReaderState.dwEventState, rgReaderState.cbAtr);
		
		ret = SCardGetStatusChangeA(hContext, INFINITE, &rgReaderState, 1);
		
		printf("\t%s (%X)\n\tszReader: %s\n\tdwCurrentState: %s (%X)\n\tdwEventState: %s (%X)\n\tcbAtr: %d\n\n", getErrorString(ret), ret,
			rgReaderState.szReader, getStateString(rgReaderState.dwCurrentState), rgReaderState.dwCurrentState, getStateString(rgReaderState.dwEventState),
			rgReaderState.dwEventState, rgReaderState.cbAtr);
		
		//rgReaderState.dwEventState &= ~SCARD_STATE_CHANGED;
		rgReaderState.dwCurrentState = rgReaderState.dwEventState;
		//rgReaderState.dwEventState = SCARD_STATE_UNKNOWN;
	}
	
	
EXCEPTION:
	ret = SCardReleaseContext(hContext);
	if(ret != SCARD_S_SUCCESS) {
		fprintf(stderr, "couldn't finally release context\n");
		return EXIT_FAILURE;
	}
	
	printf("everything all right!\n");
	return EXIT_SUCCESS;
}
