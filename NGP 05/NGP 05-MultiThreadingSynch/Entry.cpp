#include <Windows.h>
#include <stdio.h>

CRITICAL_SECTION cs;

DWORD __stdcall ThreadRunner( LPVOID parameter )
{
	int* argument;
	int count;

	//  루프를 3번 실행한다.
	count = 3;

	//  인자를 받아온다.
	argument = ( int* )parameter;


	::EnterCriticalSection(&cs);

	while( count-- > 0 )
		printf( "I'm %d Thread !!\n", *argument);

	::LeaveCriticalSection(&cs);


	//  이 함수와 함께 쓰레드는 종료된다.
	return 0;
}

void main()
{
	HANDLE handleThread[5];
	int array[5] = { 0, };
	int i;

	::InitializeCriticalSection(&cs);

	for( i = 0 ; i < 5 ; i++ )
	{
		array[i] = i;
		handleThread[i] = ::CreateThread( 0, 0, ThreadRunner, &array[i], 0, 0 );
	}

	::EnterCriticalSection(&cs);

	for( i = 0 ; i < 5 ; i++ )
		printf( "I'm Main thread !!\n" );

	::LeaveCriticalSection(&cs);

	::WaitForMultipleObjects( 5, handleThread, TRUE, INFINITE );

	char temp[120];
	fgets(temp, 119, stdin);
}

/*
CRITICAL_SECTION cs;

	::InitializeCriticalSection( &cs );

	::EnterCriticalSection( &cs );
	::LeaveCriticalSection( &cs );
*/