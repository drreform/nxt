
#pragma platform(NXT)
#include "../constants.c"

long nElapsedTime = 0;
float fThroughput = 0;
long nSendTotal = 0;
long nSendGood = 0;
long nSendBad = 0;
long nSendBusy1 = 0;
long nSendBusy2 = 0;

long nRcvTries = 0;
long nReadCnt = 0;
long nReadBad = 0;

long nLastXmitTimeStamp = nPgmTime;
long nLastRcvdTimeStamp = nPgmTime;
long nDeltaTime         = 0;

const int kTimestampHistogramSize = 41;
int nRcvHistogram[kTimestampHistogramSize];
int nXmitHistogram[kTimestampHistogramSize];

const int kTimeBetweenXmit = 30;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                        Send Messages Task
//
////////////////////////////////////////////////////////////////////////////////////////////////////////



const int kMaxSizeOfMessage = 6;
const TMailboxIDs kQueueID = mailbox1;

void readDataMsg();

void sendDataMsg(ubyte senderID, ubyte receiverID, ubyte type, ubyte method, ubyte data)
{
	// <messageLength> <senderID> <receiverID> <messageType> <method> <data>
	ubyte nXmitBuffer[kMaxSizeOfMessage] = {0x05, senderID, receiverID, type, method, data};
	const bool bWaitForReply = false;
	TFileIOResult nBTCmdErrorStatus;

	//nxtDisplayTextLine(1, "Send      %6d", nSendTotal);

	nDeltaTime = nPgmTime - nLastXmitTimeStamp;
	if (nDeltaTime < kTimeBetweenXmit)
	{
		//nxtDisplayTextLine(2, "Bsy%6d %6d", nSendBusy1, ++nSendBusy2);
		return;
	}

	if (bBTBusy)
	{
		//nxtDisplayTextLine(2, "Bsy%6d %6d", ++nSendBusy1, nSendBusy2);
		return;
	}


	nBTCmdErrorStatus = cCmdMessageWriteToBluetooth(nXmitBuffer, kMaxSizeOfMessage, kQueueID);
	switch (nBTCmdErrorStatus)
	{
	case ioRsltSuccess:
	case ioRsltCommPending:
		//nxtDisplayTextLine(3, "Send OK   %6d", ++nSendGood);
		nDeltaTime = nPgmTime - nLastXmitTimeStamp;
		nLastXmitTimeStamp = nPgmTime;
		if (nDeltaTime >= kTimestampHistogramSize)
			nDeltaTime = kTimestampHistogramSize - 1;
		++nXmitHistogram[nDeltaTime];
		break;

	case ioRsltCommChannelBad:
	default:
		//nxtDisplayTextLine(4, "Send Bad  %6d", ++nSendBad);
		break;
	}
	return;
}

void checkBTLinkConnected()
{
	if (nBTCurrentStreamIndex >= 0)
		return;

	PlaySound(soundLowBuzz);
	PlaySound(soundLowBuzz);
	eraseDisplay();
	nxtDisplayCenteredTextLine(3, "BT not");
	nxtDisplayCenteredTextLine(4, "Connected");
	wait1Msec(3000);
	StopAllTasks();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                        Receive Messages Task
//
////////////////////////////////////////////////////////////////////////////////////////////////////////


void readDataMsg()
{

	const bool bWaitForReply = false;
	TFileIOResult nBTCmdRdErrorStatus;
	int nSizeOfMessage;
	ubyte nRcvBuffer[kMaxSizeOfMessage * 10];

	while (true)
	{
		//nxtDisplayTextLine(5, "Rd Tries  %6d", ++nRcvTries);

		// Check to see if a message is available

		nSizeOfMessage = cCmdMessageGetSize(kQueueID);
		if (nSizeOfMessage <= 0)
		{
			wait1Msec(1);    // Give other tasks a chance to run
			break;           // No message this time
		}

		if (nSizeOfMessage > kMaxSizeOfMessage)
			nSizeOfMessage = kMaxSizeOfMessage;
		nBTCmdRdErrorStatus = cCmdMessageRead(nRcvBuffer, nSizeOfMessage, kQueueID);
		if (nBTCmdRdErrorStatus == ioRsltSuccess)
		{

			nDeltaTime = nPgmTime - nLastRcvdTimeStamp;
			nLastRcvdTimeStamp = nPgmTime;
			if (nDeltaTime >= kTimestampHistogramSize)
				nDeltaTime = kTimestampHistogramSize - 1;
			++nRcvHistogram[nDeltaTime];
			// Keep a running count of the number of messages successfully read
			//nxtDisplayTextLine(6, "Read OK   %6d", ++nReadCnt);
			int l =  nRcvBuffer[0];
			for( int i=1; i<=l; i++){
				nxtDisplayTextLine(i, "m: %x", nRcvBuffer[i]);
			}
			//nxtDisplayTextLine(7, "M:%x %x %x %x %x", nRcvBuffer[0], nRcvBuffer[1], nRcvBuffer[2], nRcvBuffer[3],  nRcvBuffer[5]);
			fThroughput = nElapsedTime / (float) nReadCnt;
		}
		else
			nxtDisplayTextLine(7, "Read Bad  %6d", ++nReadBad);
	}
	return;
}

void moveConveyor(ubyte gearTeeth)
{
	checkBTLinkConnected();
	sendDataMsg(PRINTER, CONVEYOR, REQUEST, CONVEYOR_MOVE, gearTeeth);
	nElapsedTime = nPgmTime;
	wait1Msec(1);
}

// test
//task main()
//{
//	bNxtLCDStatusDisplay = true;
//	memset(nRcvHistogram,  0, sizeof(nRcvHistogram));
//	memset(nXmitHistogram, 0, sizeof(nXmitHistogram));
//	wait1Msec(2000);
//	moveConveyor(PRINTER, CONVEYOR, REQUEST, CONVEYOR_MOVE, 0x03);
//	return;
//}
