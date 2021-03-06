#include <gtest/gtest.h>
#include <mimc/user.h>
#include <mimc/utils.h>
#include <mimc/error.h>
#include <test/mimc_message_handler.h>
#include <test/mimc_onlinestatus_handler.h>
#include <test/mimc_tokenfetcher.h>
#include <test/rts_call_eventhandler.h>
#include <test/rts_call_delayresponse_eventhandler.h>
#include <test/rts_call_timeoutresponse_eventhandler.h>
#include <test/rts_message_data.h>

using namespace std;

#ifndef STAGING
string appId = "2882303761517613988";
string appKey = "5361761377988";
string appSecret = "2SZbrJOAL1xHRKb7L9AiRQ==";
#else
string appId = "2882303761517479657";
string appKey = "5221747911657";
string appSecret = "PtfBeZyC+H8SIM/UXhZx1w==";
#endif

string appAccount1 = "mi153";
string appAccount2 = "mi108";
string appAccount3 = "mi110";
string appAccount4 = "mi111";
string appAccount5 = "mi112";
const int WAIT_TIME_FOR_MESSAGE = 1;
const int UDP_CONN_TIMEOUT = 5;

class RtsTest: public testing::Test
{
protected:
    void SetUp() {
        rtsUser1_r1 = new User(appAccount1, "rts_staging_resource1");
        rtsUser1_r2 = new User(appAccount1, "rts_staging_resource2");
        rtsUser2_r1 = new User(appAccount2, "rts_staging_resource1");
        rtsUser2_r2 = new User(appAccount2, "rts_staging_resource2");
        rtsUser3 = new User(appAccount3, "rts_staging_resource1");

        msgHandler1_r1 = new TestMessageHandler();
        msgHandler1_r2 = new TestMessageHandler();
        msgHandler2_r1 = new TestMessageHandler();
        msgHandler2_r2 = new TestMessageHandler();
        msgHandler3 = new TestMessageHandler();

        callEventHandler1_r1 = new TestRTSCallEventHandler();
        callEventHandler1_r2 = new TestRTSCallEventHandler();
        callEventHandler2_r1 = new TestRTSCallEventHandler("ll123456");
        callEventHandler2_r2 = new TestRTSCallEventHandler("ll123456");
        callEventHandler3 = new TestRTSCallEventHandler();
        callEventHandlerDelayResponse = new TestRTSCallDelayResponseEventHandler();
        callEventHandlerTimeoutResponse = new TestRTSCallTimeoutResponseEventHandler();

        rtsUser1_r1->registerTokenFetcher(new TestTokenFetcher(appId, appKey, appSecret, appAccount1));
        rtsUser1_r1->registerOnlineStatusHandler(new TestOnlineStatusHandler());
        rtsUser1_r1->registerMessageHandler(msgHandler1_r1);
        rtsUser1_r1->registerRTSCallEventHandler(callEventHandler1_r1);

        rtsUser1_r2->registerTokenFetcher(new TestTokenFetcher(appId, appKey, appSecret, appAccount1));
        rtsUser1_r2->registerOnlineStatusHandler(new TestOnlineStatusHandler());
        rtsUser1_r2->registerMessageHandler(msgHandler1_r2);
        rtsUser1_r2->registerRTSCallEventHandler(callEventHandler1_r2);

        rtsUser2_r1->registerTokenFetcher(new TestTokenFetcher(appId, appKey, appSecret, appAccount2));
        rtsUser2_r1->registerOnlineStatusHandler(new TestOnlineStatusHandler());
        rtsUser2_r1->registerMessageHandler(msgHandler2_r1);
        rtsUser2_r1->registerRTSCallEventHandler(callEventHandler2_r1);

        rtsUser2_r2->registerTokenFetcher(new TestTokenFetcher(appId, appKey, appSecret, appAccount2));
        rtsUser2_r2->registerOnlineStatusHandler(new TestOnlineStatusHandler());
        rtsUser2_r2->registerMessageHandler(msgHandler2_r2);
        rtsUser2_r2->registerRTSCallEventHandler(callEventHandler2_r2);

        rtsUser3->registerTokenFetcher(new TestTokenFetcher(appId, appKey, appSecret, appAccount3));
        rtsUser3->registerOnlineStatusHandler(new TestOnlineStatusHandler());
        rtsUser3->registerMessageHandler(msgHandler3);
        rtsUser3->registerRTSCallEventHandler(callEventHandler3);
    }

    void TearDown() {
        rtsUser1_r1->logout();
        rtsUser1_r2->logout();
        rtsUser2_r1->logout();
        rtsUser2_r2->logout();
        rtsUser3->logout();
        sleep(2);
        delete rtsUser1_r1;
        rtsUser1_r1 = NULL;
        delete rtsUser1_r2;
        rtsUser1_r2 = NULL;
        delete rtsUser2_r1;
        rtsUser2_r1 = NULL;
        delete rtsUser2_r2;
        rtsUser2_r2 = NULL;
        delete rtsUser3;
        rtsUser3 = NULL;
    }

    //@test
    void testCreateCall() {
        testCreateCall(rtsUser1_r1, callEventHandler1_r1, rtsUser3, callEventHandler3);
    }

    //@test
    void testCreateCallTimeout() {
        rtsUser2_r1->registerRTSCallEventHandler(callEventHandlerTimeoutResponse);
        createCallTimeout(rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandlerTimeoutResponse);
    }

    //@test
    void testP2PSendOneRtsSignalToOffline() {
        testP2PSendOneRtsSignalToOffline(rtsUser1_r1, callEventHandler1_r1, rtsUser3, callEventHandler3, callEventHandler1_r1->getAppContent());
    }

    //@test
    void testP2PSendOneRtsSignalFromOffline() {
        testP2PSendOneRtsSignalFromOffline(rtsUser1_r1, callEventHandler1_r1, rtsUser3, callEventHandler3, callEventHandler1_r1->getAppContent());
    }

    //@test
    void testCreateCallRefuse() {
        testCreateCallRefuse(rtsUser1_r1, callEventHandler1_r1, rtsUser2_r2, callEventHandler2_r2, callEventHandler1_r1->getAppContent());
    }

    //@test
    void testCreateCallWithSelf() {
        testCreateCallWithSelf(rtsUser1_r1, callEventHandler1_r1, rtsUser1_r2, callEventHandler1_r2);
    }

    //@test
    void testCreateCallFromMulResources() {
        testCreateCallFromMulResources(rtsUser1_r1, callEventHandler1_r1, rtsUser1_r2, callEventHandler1_r2, rtsUser2_r1, callEventHandler2_r1);
    }

    //@test
    void testCreateCallWithMulResources() {
        testCreateCallWithMulResources(rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandler2_r1, rtsUser2_r2, callEventHandlerDelayResponse);
    }

    //@test
    void testCreateCallWhenCalling() {
        testCreateCallWhenCalling(rtsUser1_r1, callEventHandler1_r1, rtsUser1_r2, callEventHandler1_r2, rtsUser2_r1, callEventHandler2_r1, rtsUser2_r2, callEventHandler2_r2);
    }

    //@test
    void testRecvCallWhenCalling() {
        testRecvCallWhenCalling(rtsUser1_r1, callEventHandler1_r1, rtsUser1_r2, callEventHandler1_r2, rtsUser2_r1, callEventHandler2_r1, rtsUser2_r2, callEventHandler2_r2);
    }

    //@test
    void testCreateCalls() {
        testCreateCalls(rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandler2_r1, rtsUser3, callEventHandler3);
    }

    //@test
    void testCreateCallWithOffline() {
        testCreateCallWithOffline(rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandler2_r1);
    }

    //@test
    void testCloseCall() {
        logIn(rtsUser1_r1, callEventHandler1_r1);
        logIn(rtsUser2_r1, callEventHandler2_r1);

        long chatId = createCall(rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandler2_r1, "ll123456");
        testCloseCall(chatId, rtsUser1_r1, callEventHandler1_r1, callEventHandler2_r1);
    }

    //@test
    void testCloseCallSameTime() {
        logIn(rtsUser1_r1, callEventHandler1_r1);
        logIn(rtsUser2_r1, callEventHandler2_r1);

        long chatId = createCall(rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandler2_r1, "ll123456");
        closeCallSameTime(chatId, rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandler2_r1);
    }

    //@test
    void testSendDatas() {
        logIn(rtsUser1_r1, callEventHandler1_r1);
        logIn(rtsUser2_r1, callEventHandler2_r1);

        long chatId = createCall(rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandler2_r1, "ll123456");

        sendDatas(chatId, rtsUser1_r1, callEventHandler2_r1, RELAY);

        closeCall(chatId, rtsUser1_r1, callEventHandler1_r1, callEventHandler2_r1);
    }

    //@test
    void testSendDataToEachOther() {
        logIn(rtsUser1_r1, callEventHandler1_r1);
        logIn(rtsUser2_r1, callEventHandler2_r1);

        long chatId = createCall(rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandler2_r1, "ll123456");

        sendDataToEachOther(chatId, rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandler2_r1, RELAY);

        closeCall(chatId, rtsUser1_r1, callEventHandler1_r1, callEventHandler2_r1);
    }

    //@test
    void testSendDataAfterClose() {
        sendDataAfterClose(rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandler2_r1, "ll123456");
    }

    //@test
    void testOneBrokenNetWorkRecoverBeforeConnTimeout() {
        testOneBrokenNetWorkRecoverBeforeConnTimeout(rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandler2_r1, "ll123456");
    }

/*
    //@test
    void testOneBrokenNetWorkRecoverAfterConnTimeoutBeforeServerTimeout() {
        testOneBrokenNetWorkRecoverAfterConnTimeoutBeforeServerTimeout(rtsUser1_r1, callEventHandler1_r1, rtsUser2_r1, callEventHandler2_r1, "ll123456");
    }
*/

    void logIn(User* user, TestRTSCallEventHandler* callEventHandler) {
        long loginTs = time(NULL);
        user->login();
        while (time(NULL) - loginTs < LOGIN_TIMEOUT && user->getOnlineStatus() == Offline) {
            usleep(100000);
        }
        ASSERT_EQ(Online, user->getOnlineStatus());
        if (callEventHandler != NULL) {
            XMDLoggerWrapper::instance()->info("CLEAR CALLEVENTHANDLER OF UUID:%ld", user->getUuid());
            callEventHandler->clear();
        }
    }

    void testCreateCall(User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallEventHandler* callEventHandlerTo) {
        string appContent = "appinfo";
        logIn(from, callEventHandlerFrom);
        logIn(to, callEventHandlerTo);
        //A_STREAM
        long chatId1 = from->dialCall(to->getAppAccount(), appContent);
        ASSERT_NE(chatId1, -1);
        sleep(1);

        RtsMessageData* inviteRequest = callEventHandlerTo->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId1, inviteRequest->getChatId());
        ASSERT_EQ(from->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(from->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ(appContent, inviteRequest->getAppContent());

        RtsMessageData* createResponse = callEventHandlerFrom->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId1, createResponse->getChatId());
        ASSERT_EQ(true, createResponse->isAccepted());
        ASSERT_EQ(callEventHandlerFrom->LAUNCH_OK, createResponse->getErrmsg());

        closeCall(chatId1, from, callEventHandlerFrom, callEventHandlerTo);
        //V_STREAM
        long chatId2 = from->dialCall(to->getAppAccount());
        ASSERT_NE(chatId2, -1);
        sleep(1);

        inviteRequest = callEventHandlerTo->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId2, inviteRequest->getChatId());
        ASSERT_EQ(from->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(from->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("", inviteRequest->getAppContent());

        createResponse = callEventHandlerFrom->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId2, createResponse->getChatId());
        ASSERT_EQ(true, createResponse->isAccepted());
        ASSERT_EQ(callEventHandlerFrom->LAUNCH_OK, createResponse->getErrmsg());

        closeCall(chatId2, from, callEventHandlerFrom, callEventHandlerTo);
        //AV_STREAM
        long chatId3 = from->dialCall(to->getAppAccount());
        ASSERT_NE(chatId3, -1);
        sleep(1);

        inviteRequest = callEventHandlerTo->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId3, inviteRequest->getChatId());
        ASSERT_EQ(from->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(from->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("", inviteRequest->getAppContent());

        createResponse = callEventHandlerFrom->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId3, createResponse->getChatId());
        ASSERT_EQ(true, createResponse->isAccepted());
        ASSERT_EQ(callEventHandlerFrom->LAUNCH_OK, createResponse->getErrmsg());

        closeCall(chatId3, from, callEventHandlerFrom, callEventHandlerTo);
    }

    long createCall(User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallEventHandler* callEventHandlerTo, const string& appContent) {
        long chatId = from->dialCall(to->getAppAccount(), appContent);
        EXPECT_NE(chatId, -1);
        sleep(1);

        RtsMessageData* inviteRequest = callEventHandlerTo->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        EXPECT_FALSE(inviteRequest == NULL);
        if (inviteRequest != NULL) {
            EXPECT_EQ(chatId, inviteRequest->getChatId());
            EXPECT_EQ(from->getAppAccount(), inviteRequest->getFromAccount());
            EXPECT_EQ(from->getResource(), inviteRequest->getFromResource());
            EXPECT_EQ(appContent, inviteRequest->getAppContent());
        }

        RtsMessageData* createResponse = callEventHandlerFrom->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        EXPECT_FALSE(createResponse == NULL);
        if (createResponse != NULL) {
            EXPECT_EQ(chatId, createResponse->getChatId());
            EXPECT_EQ(true, createResponse->isAccepted());
            EXPECT_EQ(callEventHandlerFrom->LAUNCH_OK, createResponse->getErrmsg());
        }

        return chatId;
    }

    void closeCall(long chatId, User* from, TestRTSCallEventHandler* callEventHandlerFrom, TestRTSCallEventHandler* callEventHandlerTo) {
        from->closeCall(chatId);
        sleep(1);

        RtsMessageData* byeRequest = callEventHandlerTo->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(byeRequest == NULL);
        ASSERT_EQ(chatId, byeRequest->getChatId());
        ASSERT_EQ("", byeRequest->getErrmsg());

        RtsMessageData* byeResponse = callEventHandlerFrom->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(byeResponse == NULL);
        ASSERT_EQ(chatId, byeResponse->getChatId());
        ASSERT_EQ("CLOSED_INITIATIVELY", byeResponse->getErrmsg());
    }

    void sendDatas(long chatId, User* from, TestRTSCallEventHandler* callEventHandlerTo, RtsChannelType channel_type) {
        vector<string> sendDatas;
        for (int size = 0; size <= 500; size += 50) {
            string sendData = Utils::generateRandomString(size * 1024);
            ASSERT_TRUE(from->sendRtsData(chatId, sendData, AUDIO, channel_type));
            sendDatas.push_back(sendData);
            sleep(size / 200);
        }

        for (int i = 0; i < sendDatas.size(); i++) {
            XMDLoggerWrapper::instance()->info("current i is %d", i);
            RtsMessageData* recvData = callEventHandlerTo->pollData(WAIT_TIME_FOR_MESSAGE);
            ASSERT_FALSE(recvData == NULL);
            ASSERT_EQ(chatId, recvData->getChatId());
            ASSERT_EQ(AUDIO, recvData->getDataType());
            ASSERT_EQ(channel_type, recvData->getChannelType());
            const string& data = recvData->getRecvData();
            int index = data.size() / (50 * 1024);
            XMDLoggerWrapper::instance()->info("recv data index is %d", index);
            ASSERT_EQ(sendDatas.at(index), data);
        }

        string sendData0 = Utils::generateRandomString(RTS_MAX_PAYLOAD_SIZE + 1);
        ASSERT_FALSE(from->sendRtsData(chatId, sendData0, AUDIO, channel_type));
    }

    void sendDataToEachOther(long chatId, User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallEventHandler* callEventHandlerTo, RtsChannelType channel_type) {
        string sendData1 = "hello to";
        string sendData2 = "hello from";

        ASSERT_TRUE(from->sendRtsData(chatId, sendData1, AUDIO, channel_type));
        ASSERT_TRUE(to->sendRtsData(chatId, sendData2, VIDEO, channel_type));

        RtsMessageData* recvData = callEventHandlerTo->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(recvData == NULL);
        ASSERT_EQ(chatId, recvData->getChatId());
        ASSERT_EQ(AUDIO, recvData->getDataType());
        ASSERT_EQ(channel_type, recvData->getChannelType());
        ASSERT_EQ(sendData1, recvData->getRecvData());
        recvData = callEventHandlerTo->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(recvData == NULL);

        recvData = callEventHandlerFrom->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(recvData == NULL);
        ASSERT_EQ(chatId, recvData->getChatId());
        ASSERT_EQ(VIDEO, recvData->getDataType());
        ASSERT_EQ(channel_type, recvData->getChannelType());
        ASSERT_EQ(sendData2, recvData->getRecvData());
        recvData = callEventHandlerFrom->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(recvData == NULL);
    }

    void sendDataAfterClose(User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallEventHandler* callEventHandlerTo, const string& appContent) {
        logIn(from, callEventHandlerFrom);
        logIn(to, callEventHandlerTo);

        string sendData = Utils::generateRandomString(50);

        long chatId = createCall(from, callEventHandlerFrom, to, callEventHandlerTo, appContent);
        from->closeCall(chatId);
        ASSERT_FALSE(from->sendRtsData(chatId, sendData, AUDIO));
        RtsMessageData* recvData = callEventHandlerTo->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(recvData == NULL);

        RtsMessageData* byeRequest = callEventHandlerTo->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(byeRequest == NULL);
        ASSERT_EQ(chatId, byeRequest->getChatId());
        ASSERT_EQ("", byeRequest->getErrmsg());

        RtsMessageData* byeResponse = callEventHandlerFrom->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(byeResponse == NULL);
        ASSERT_EQ(chatId, byeResponse->getChatId());
        ASSERT_EQ("CLOSED_INITIATIVELY", byeResponse->getErrmsg());

        chatId = createCall(from, callEventHandlerFrom, to, callEventHandlerTo, appContent);
        to->closeCall(chatId);
        ASSERT_TRUE(from->sendRtsData(chatId, sendData, VIDEO));
        recvData = callEventHandlerTo->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(recvData == NULL);

        byeRequest = callEventHandlerFrom->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(byeRequest == NULL);
        ASSERT_EQ(chatId, byeRequest->getChatId());
        ASSERT_EQ("", byeRequest->getErrmsg());

        byeResponse = callEventHandlerTo->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(byeResponse == NULL);
        ASSERT_EQ(chatId, byeResponse->getChatId());
        ASSERT_EQ("CLOSED_INITIATIVELY", byeResponse->getErrmsg());

        ASSERT_FALSE(from->sendRtsData(chatId, sendData, AUDIO));
        recvData = callEventHandlerTo->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(recvData == NULL);
    }

    void testOneBrokenNetWorkRecoverBeforeConnTimeout(User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallEventHandler* callEventHandlerTo, const string& appContent) {
        logIn(from, callEventHandlerFrom);
        logIn(to, callEventHandlerTo);

        string sendData = Utils::generateRandomString(30);

        long chatId = createCall(from, callEventHandlerFrom, to, callEventHandlerTo, appContent);
        to->setTestPacketLoss(100);

        ASSERT_TRUE(to->sendRtsData(chatId, sendData, AUDIO));
        RtsMessageData* recvData = callEventHandlerFrom->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(recvData == NULL);
        ASSERT_TRUE(from->sendRtsData(chatId, sendData, AUDIO));
        recvData = callEventHandlerTo->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(recvData == NULL);

        to->setTestPacketLoss(0);

        ASSERT_TRUE(to->sendRtsData(chatId, sendData, AUDIO));
        recvData = callEventHandlerFrom->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(recvData == NULL);
        ASSERT_EQ(chatId, recvData->getChatId());
        ASSERT_EQ(AUDIO, recvData->getDataType());
        ASSERT_EQ(RELAY, recvData->getChannelType());
        ASSERT_EQ(sendData, recvData->getRecvData());

        ASSERT_TRUE(from->sendRtsData(chatId, sendData, AUDIO));
        recvData = callEventHandlerTo->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(recvData == NULL);
        ASSERT_EQ(chatId, recvData->getChatId());
        ASSERT_EQ(AUDIO, recvData->getDataType());
        ASSERT_EQ(RELAY, recvData->getChannelType());
        ASSERT_EQ(sendData, recvData->getRecvData());

        closeCall(chatId, from, callEventHandlerFrom, callEventHandlerTo);
    }

/*    void testOneBrokenNetWorkRecoverAfterConnTimeoutBeforeServerTimeout(User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallEventHandler* callEventHandlerTo, const string& appContent) {
        logIn(from, callEventHandlerFrom);
        logIn(to, callEventHandlerTo);

        string sendData = Utils::generateRandomString(30);

        long chatId = createCall(from, callEventHandlerFrom, to, callEventHandlerTo, appContent);
        to->setTestNetworkBlocked(true);

        ASSERT_TRUE(to->sendRtsData(chatId, sendData, AUDIO));
        RtsMessageData* recvData = callEventHandlerFrom->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(recvData == NULL);
        ASSERT_TRUE(from->sendRtsData(chatId, sendData, AUDIO));
        recvData = callEventHandlerTo->pollData(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(recvData == NULL);

        sleep(UDP_CONN_TIMEOUT);
        to->setTestNetworkBlocked(false);
        sleep(5);

        RtsMessageData* byeInfo = callEventHandlerFrom->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(byeInfo == NULL);

        byeInfo = callEventHandlerTo->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(byeInfo == NULL);
        ASSERT_EQ(chatId, byeInfo->getChatId());
        ASSERT_EQ("ALL_DATA_TRANSPORTS_CLOSED", byeInfo->getErrmsg());


    }
*/

    void createCallTimeout(User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallTimeoutResponseEventHandler* callEventHandlerTo) {
        logIn(from, callEventHandlerFrom);
        logIn(to, NULL);
        callEventHandlerTo->clear();

        long chatId = from->dialCall(to->getAppAccount());
        ASSERT_NE(chatId, -1);
        sleep(1);

        RtsMessageData* inviteRequest = callEventHandlerTo->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId, inviteRequest->getChatId());
        ASSERT_EQ(from->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(from->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("", inviteRequest->getAppContent());

        RtsMessageData* createResponse = callEventHandlerFrom->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(createResponse == NULL);

        createResponse = callEventHandlerFrom->pollCreateResponse(RTS_CALL_TIMEOUT);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId, createResponse->getChatId());
        ASSERT_EQ(false, createResponse->isAccepted());
        ASSERT_EQ(DIAL_CALL_TIMEOUT, createResponse->getErrmsg());

        RtsMessageData* byeResponse_from = callEventHandlerFrom->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(byeResponse_from == NULL);

        RtsMessageData* byeResponse_to = callEventHandlerTo->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(byeResponse_to == NULL);
        ASSERT_EQ(chatId, byeResponse_to->getChatId());
    }

    void testP2PSendOneRtsSignalToOffline(User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallEventHandler* callEventHandlerTo, string appContent) {
        logIn(from, callEventHandlerFrom);

        long chatId = from->dialCall(to->getAppAccount(), appContent);
        ASSERT_NE(chatId, -1);
        sleep(1);
        RtsMessageData* inviteRequest = callEventHandlerTo->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(inviteRequest == NULL);

        RtsMessageData* createResponse = callEventHandlerFrom->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(createResponse == NULL);

        createResponse = callEventHandlerFrom->pollCreateResponse(RTS_CALL_TIMEOUT);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId, createResponse->getChatId());
        ASSERT_EQ(false, createResponse->isAccepted());
        ASSERT_EQ(DIAL_CALL_TIMEOUT, createResponse->getErrmsg());
    }

    void testP2PSendOneRtsSignalFromOffline(User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallEventHandler* callEventHandlerTo, string appContent) {
        logIn(to, callEventHandlerTo);

        long chatId = from->dialCall(to->getAppAccount(), appContent);
        ASSERT_EQ(chatId, -1);
    }

    void testCreateCallRefuse(User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallEventHandler* callEventHandlerTo, string appContent) {
        logIn(from, callEventHandlerFrom);
        logIn(to, callEventHandlerTo);

        long chatId = from->dialCall(to->getAppAccount(), appContent);
        ASSERT_NE(chatId, -1);
        sleep(1);

        RtsMessageData* inviteRequest = callEventHandlerTo->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId, inviteRequest->getChatId());
        ASSERT_EQ(from->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(from->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ(appContent, inviteRequest->getAppContent());

        RtsMessageData* createResponse = callEventHandlerFrom->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId, createResponse->getChatId());
        ASSERT_EQ(false, createResponse->isAccepted());
        ASSERT_EQ(callEventHandlerFrom->LAUNCH_ERR_ILLEGALAPPCONTENT, createResponse->getErrmsg());
    }

    void testCreateCallWithSelf(User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallEventHandler* callEventHandlerTo) {
        logIn(from, callEventHandlerFrom);
        logIn(to, callEventHandlerTo);

        long chatId1 = from->dialCall(from->getAppAccount(), "", from->getResource());
        ASSERT_NE(chatId1, -1);
        sleep(1);

        RtsMessageData* inviteRequest = callEventHandlerFrom->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(inviteRequest == NULL);

        RtsMessageData* createResponse = callEventHandlerFrom->pollCreateResponse(RTS_CALL_TIMEOUT);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId1, createResponse->getChatId());
        ASSERT_EQ(false, createResponse->isAccepted());
        ASSERT_EQ(DIAL_CALL_TIMEOUT, createResponse->getErrmsg());

        long chatId2 = from->dialCall(from->getAppAccount(), "", to->getResource());
        ASSERT_NE(chatId2, -1);
        sleep(1);

        inviteRequest = callEventHandlerTo->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId2, inviteRequest->getChatId());
        ASSERT_EQ(from->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(from->getResource(), inviteRequest->getFromResource());

        createResponse = callEventHandlerFrom->pollCreateResponse(RTS_CALL_TIMEOUT);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId2, createResponse->getChatId());
        ASSERT_EQ(true, createResponse->isAccepted());
        ASSERT_EQ(callEventHandlerFrom->LAUNCH_OK, createResponse->getErrmsg());

        closeCall(chatId2, from, callEventHandlerFrom, callEventHandlerTo);
    }

    void testCreateCallFromMulResources(User* from_r1, TestRTSCallEventHandler* callEventHandlerFrom1, User* from_r2, TestRTSCallEventHandler* callEventHandlerFrom2, User* to, TestRTSCallEventHandler* callEventHandlerTo) {
        logIn(from_r1, callEventHandlerFrom1);
        logIn(from_r2, callEventHandlerFrom2);
        logIn(to, callEventHandlerTo);

        long chatId = from_r1->dialCall(to->getAppAccount(), "ll123456");
        ASSERT_NE(chatId, -1);
        sleep(1);

        RtsMessageData* inviteRequest = callEventHandlerTo->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId, inviteRequest->getChatId());
        ASSERT_EQ(from_r1->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(from_r1->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("ll123456", inviteRequest->getAppContent());

        RtsMessageData* createResponse = callEventHandlerFrom2->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(createResponse == NULL);

        createResponse = callEventHandlerFrom1->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId, createResponse->getChatId());
        ASSERT_EQ(true, createResponse->isAccepted());
        ASSERT_EQ(callEventHandlerFrom1->LAUNCH_OK, createResponse->getErrmsg());

        closeCall(chatId, from_r1, callEventHandlerFrom1, callEventHandlerTo);
    }

    void testCreateCallWithMulResources(User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to_r1, TestRTSCallEventHandler* callEventHandlerTo1, User* to_r2, TestRTSCallDelayResponseEventHandler* callEventHandlerTo2) {
        logIn(from, callEventHandlerFrom);
        logIn(to_r1, callEventHandlerTo1);
        logIn(to_r2, NULL);
        callEventHandlerTo2->clear();
        to_r2->registerRTSCallEventHandler(callEventHandlerTo2);

        long chatId = from->dialCall(to_r1->getAppAccount(), "ll123456", to_r1->getResource());
        ASSERT_NE(chatId, -1);
        sleep(1);

        RtsMessageData* inviteRequest = callEventHandlerTo2->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(inviteRequest == NULL);

        inviteRequest = callEventHandlerTo1->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId, inviteRequest->getChatId());
        ASSERT_EQ(from->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(from->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("ll123456", inviteRequest->getAppContent());

        RtsMessageData* createResponse = callEventHandlerFrom->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId, createResponse->getChatId());
        ASSERT_EQ(true, createResponse->isAccepted());
        ASSERT_EQ(callEventHandlerFrom->LAUNCH_OK, createResponse->getErrmsg());

        closeCall(chatId, from, callEventHandlerFrom, callEventHandlerTo1);

        //call with no given resource, success with the resource answer first
        chatId = from->dialCall(to_r1->getAppAccount(), "ll123456");
        ASSERT_NE(chatId, -1);
        sleep(1);

        inviteRequest = callEventHandlerTo1->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId, inviteRequest->getChatId());
        ASSERT_EQ(from->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(from->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("ll123456", inviteRequest->getAppContent());

        inviteRequest = callEventHandlerTo2->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId, inviteRequest->getChatId());
        ASSERT_EQ(from->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(from->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("ll123456", inviteRequest->getAppContent());

        createResponse = callEventHandlerFrom->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId, createResponse->getChatId());
        ASSERT_EQ(true, createResponse->isAccepted());
        ASSERT_EQ(callEventHandlerFrom->LAUNCH_OK, createResponse->getErrmsg());
        createResponse = callEventHandlerFrom->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(createResponse == NULL);

        RtsMessageData* byeRequest = callEventHandlerTo1->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(byeRequest == NULL);
        byeRequest = callEventHandlerTo2->pollBye(WAIT_TIME_FOR_MESSAGE * 4);
        ASSERT_FALSE(byeRequest == NULL);
        ASSERT_EQ(chatId, byeRequest->getChatId());
        ASSERT_EQ("already answered", byeRequest->getErrmsg());

        closeCall(chatId, from, callEventHandlerFrom, callEventHandlerTo1);

        //call with no given resource, resource2 answer after call close
        chatId = from->dialCall(to_r1->getAppAccount(), "ll123456");
        ASSERT_NE(chatId, -1);
        sleep(1);

        inviteRequest = callEventHandlerTo1->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId, inviteRequest->getChatId());
        ASSERT_EQ(from->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(from->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("ll123456", inviteRequest->getAppContent());

        inviteRequest = callEventHandlerTo2->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId, inviteRequest->getChatId());
        ASSERT_EQ(from->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(from->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("ll123456", inviteRequest->getAppContent());

        createResponse = callEventHandlerFrom->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId, createResponse->getChatId());
        ASSERT_EQ(true, createResponse->isAccepted());
        ASSERT_EQ(callEventHandlerFrom->LAUNCH_OK, createResponse->getErrmsg());
        createResponse = callEventHandlerFrom->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(createResponse == NULL);

        closeCall(chatId, from, callEventHandlerFrom, callEventHandlerTo1);

        byeRequest = callEventHandlerTo2->pollBye(WAIT_TIME_FOR_MESSAGE * 4);
        ASSERT_FALSE(byeRequest == NULL);
        ASSERT_EQ(chatId, byeRequest->getChatId());
        ASSERT_EQ("illegal request", byeRequest->getErrmsg());
    }

    //同一个用户不能接通超过maxcallnum的电话
    void testCreateCallWhenCalling(User* user1_r1, TestRTSCallEventHandler* callEventHandler1_r1, User* user1_r2, TestRTSCallEventHandler* callEventHandler1_r2, User* user2_r1, TestRTSCallEventHandler* callEventHandler2_r1, User* user2_r2, TestRTSCallEventHandler* callEventHandler2_r2) {
        logIn(user1_r1, callEventHandler1_r1);
        logIn(user1_r2, callEventHandler1_r2);
        logIn(user2_r1, callEventHandler2_r1);
        logIn(user2_r2, callEventHandler2_r2);

        long chatId1 = user1_r1->dialCall(user2_r1->getAppAccount(), "ll123456", user2_r1->getResource());
        ASSERT_NE(chatId1, -1);
        sleep(1);

        RtsMessageData* inviteRequest = callEventHandler2_r1->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId1, inviteRequest->getChatId());
        ASSERT_EQ(user1_r1->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(user1_r1->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("ll123456", inviteRequest->getAppContent());

        RtsMessageData* createResponse = callEventHandler1_r1->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId1, createResponse->getChatId());
        ASSERT_EQ(true, createResponse->isAccepted());
        ASSERT_EQ(callEventHandler1_r1->LAUNCH_OK, createResponse->getErrmsg());

        long chatId2 = user1_r2->dialCall(user2_r1->getAppAccount(), "ll123456", user2_r1->getResource());
        ASSERT_NE(chatId2, -1);
        sleep(1);

        inviteRequest = callEventHandler2_r1->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(inviteRequest == NULL);

        createResponse = callEventHandler1_r2->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId2, createResponse->getChatId());
        ASSERT_EQ(false, createResponse->isAccepted());
        ASSERT_EQ("USER_BUSY", createResponse->getErrmsg());

        long chatId3 = user1_r2->dialCall(user2_r1->getAppAccount(), "ll123456", user2_r2->getResource());
        ASSERT_NE(chatId3, -1);
        sleep(1);

        inviteRequest = callEventHandler2_r2->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId3, inviteRequest->getChatId());
        ASSERT_EQ(user1_r2->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(user1_r2->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("ll123456", inviteRequest->getAppContent());

        createResponse = callEventHandler1_r2->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId3, createResponse->getChatId());
        ASSERT_EQ(true, createResponse->isAccepted());
        ASSERT_EQ(callEventHandler1_r2->LAUNCH_OK, createResponse->getErrmsg());

        closeCall(chatId1, user1_r1, callEventHandler1_r1, callEventHandler2_r1);
        closeCall(chatId3, user1_r2, callEventHandler1_r2, callEventHandler2_r2);
    }

    void testRecvCallWhenCalling(User* user1_r1, TestRTSCallEventHandler* callEventHandler1_r1, User* user1_r2, TestRTSCallEventHandler* callEventHandler1_r2, User* user2_r1, TestRTSCallEventHandler* callEventHandler2_r1, User* user2_r2, TestRTSCallEventHandler* callEventHandler2_r2) {
        logIn(user1_r1, callEventHandler1_r1);
        logIn(user1_r2, callEventHandler1_r2);
        logIn(user2_r1, callEventHandler2_r1);
        logIn(user2_r2, callEventHandler2_r2);

        long chatId1 = user1_r1->dialCall(user2_r1->getAppAccount(), "ll123456", user2_r1->getResource());
        ASSERT_NE(chatId1, -1);
        sleep(1);

        RtsMessageData* inviteRequest = callEventHandler2_r1->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId1, inviteRequest->getChatId());
        ASSERT_EQ(user1_r1->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(user1_r1->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("ll123456", inviteRequest->getAppContent());

        RtsMessageData* createResponse = callEventHandler1_r1->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId1, createResponse->getChatId());
        ASSERT_EQ(true, createResponse->isAccepted());
        ASSERT_EQ(callEventHandler1_r1->LAUNCH_OK, createResponse->getErrmsg());

        long chatId2 = user2_r2->dialCall(user1_r1->getAppAccount());
        ASSERT_NE(chatId2, -1);
        sleep(1);

        inviteRequest = callEventHandler1_r1->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(inviteRequest == NULL);

        inviteRequest = callEventHandler1_r2->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(chatId2, inviteRequest->getChatId());
        ASSERT_EQ(user2_r2->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(user2_r2->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("", inviteRequest->getAppContent());

        createResponse = callEventHandler2_r2->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId2, createResponse->getChatId());
        if ("USER_BUSY" == createResponse->getErrmsg()) {
            ASSERT_EQ(false, createResponse->isAccepted());
        } else if (callEventHandler2_r2->LAUNCH_OK == createResponse->getErrmsg()) {
            ASSERT_EQ(true, createResponse->isAccepted());
            closeCall(chatId2, user2_r2, callEventHandler2_r2, callEventHandler1_r2);
        }
        closeCall(chatId1, user1_r1, callEventHandler1_r1, callEventHandler2_r1);
    }

    void testCreateCalls(User* user1, TestRTSCallEventHandler* callEventHandler1, User* user2, TestRTSCallEventHandler* callEventHandler2, User* user3, TestRTSCallEventHandler* callEventHandler3) {
        logIn(user1, callEventHandler1);
        logIn(user2, callEventHandler2);
        logIn(user3, callEventHandler3);

        //user1 call user2 and user3
        long chatId1 = user1->dialCall(user2->getAppAccount(), "ll123456");
        long chatId2 = user1->dialCall(user3->getAppAccount());
        sleep(1);
        ASSERT_NE(chatId1, -1);
        ASSERT_EQ(chatId2, -1);

        RtsMessageData* inviteRequest = callEventHandler2->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(inviteRequest == NULL);
        ASSERT_EQ(inviteRequest->getChatId(), chatId1);
        ASSERT_EQ(user1->getAppAccount(), inviteRequest->getFromAccount());
        ASSERT_EQ(user1->getResource(), inviteRequest->getFromResource());
        ASSERT_EQ("ll123456", inviteRequest->getAppContent());

        inviteRequest = callEventHandler3->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(inviteRequest == NULL);

        RtsMessageData* createResponse = callEventHandler1->pollCreateResponse(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId1, createResponse->getChatId());
        ASSERT_EQ(true, createResponse->isAccepted());
        ASSERT_EQ(callEventHandler1->LAUNCH_OK, createResponse->getErrmsg());

        closeCall(chatId1, user1, callEventHandler1, callEventHandler2);
    }

    void testCreateCallWithOffline(User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallEventHandler* callEventHandlerTo) {
        logIn(from, callEventHandlerFrom);
        logIn(to, callEventHandlerTo);
        ASSERT_TRUE(to->logout());
        sleep(1);
        long chatId = from->dialCall(to->getAppAccount(), "ll123456");
        ASSERT_NE(-1, chatId);

        sleep(1);

        logIn(to, NULL);
        RtsMessageData* inviteRequest = callEventHandlerTo->pollInviteRequest(WAIT_TIME_FOR_MESSAGE);
        ASSERT_TRUE(inviteRequest == NULL);

        RtsMessageData* createResponse = callEventHandlerFrom->pollCreateResponse(RTS_CALL_TIMEOUT);
        ASSERT_FALSE(createResponse == NULL);
        ASSERT_EQ(chatId, createResponse->getChatId());
        ASSERT_EQ(false, createResponse->isAccepted());
        ASSERT_EQ(DIAL_CALL_TIMEOUT, createResponse->getErrmsg());
    }

    void testCloseCall(long chatId, User* from, TestRTSCallEventHandler* callEventHandlerFrom, TestRTSCallEventHandler* callEventHandlerTo) {
        string byeReason = "i don't wanna talk";

        from->closeCall(chatId, byeReason);
        sleep(1);
        RtsMessageData* byeRequest = callEventHandlerTo->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(byeRequest == NULL);
        ASSERT_EQ(chatId, byeRequest->getChatId());
        ASSERT_EQ(byeReason, byeRequest->getErrmsg());

        RtsMessageData* byeResponse = callEventHandlerFrom->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(byeResponse == NULL);
        ASSERT_EQ(chatId, byeResponse->getChatId());
        ASSERT_EQ("CLOSED_INITIATIVELY", byeResponse->getErrmsg());
    }

    void closeCallSameTime(long chatId, User* from, TestRTSCallEventHandler* callEventHandlerFrom, User* to, TestRTSCallEventHandler* callEventHandlerTo) {
        string byeReasonFrom = "close by from";
        string byeReasonTo = "close by to";

        from->closeCall(chatId, byeReasonFrom);
        to->closeCall(chatId, byeReasonTo);

        sleep(1);

        RtsMessageData* byeRequest = callEventHandlerTo->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(byeRequest == NULL);
        ASSERT_EQ(chatId, byeRequest->getChatId());
        ASSERT_EQ("CLOSED_INITIATIVELY", byeRequest->getErrmsg());

        byeRequest = callEventHandlerFrom->pollBye(WAIT_TIME_FOR_MESSAGE);
        ASSERT_FALSE(byeRequest == NULL);
        ASSERT_EQ(chatId, byeRequest->getChatId());
        ASSERT_EQ("CLOSED_INITIATIVELY", byeRequest->getErrmsg());
    }
protected:
    User* rtsUser1_r1;
    User* rtsUser1_r2;
    User* rtsUser2_r1;
    User* rtsUser2_r2;
    User* rtsUser3;

    TestMessageHandler* msgHandler1_r1;
    TestMessageHandler* msgHandler1_r2;
    TestMessageHandler* msgHandler2_r1;
    TestMessageHandler* msgHandler2_r2;
    TestMessageHandler* msgHandler3;

    TestRTSCallEventHandler* callEventHandler1_r1;
    TestRTSCallEventHandler* callEventHandler1_r2;
    TestRTSCallEventHandler* callEventHandler2_r1;
    TestRTSCallEventHandler* callEventHandler2_r2;
    TestRTSCallEventHandler* callEventHandler3;
    TestRTSCallDelayResponseEventHandler* callEventHandlerDelayResponse;
    TestRTSCallTimeoutResponseEventHandler* callEventHandlerTimeoutResponse;
};

TEST_F(RtsTest, testP2PSendOneRtsSignalToOffline) {
    testP2PSendOneRtsSignalToOffline();
}

TEST_F(RtsTest, testP2PSendOneRtsSignalFromOffline) {
    testP2PSendOneRtsSignalFromOffline();
}

TEST_F(RtsTest, testCreateCallRefuse) {
    testCreateCallRefuse();
}

TEST_F(RtsTest, testCreateCall) {
    testCreateCall();
}

TEST_F(RtsTest, testCreateCallTimeout) {
    testCreateCallTimeout();
}

TEST_F(RtsTest, testCreateCallWithSelf) {
    testCreateCallWithSelf();
}

TEST_F(RtsTest, testCreateCallFromMulResources) {
    testCreateCallFromMulResources();
}

TEST_F(RtsTest, testCreateCallWithMulResources) {
    testCreateCallWithMulResources();
}

TEST_F(RtsTest, testCreateCallWhenCalling) {
    testCreateCallWhenCalling();
}

TEST_F(RtsTest, testRecvCallWhenCalling) {
    testRecvCallWhenCalling();
}

TEST_F(RtsTest, testCreateCalls) {
    testCreateCalls();
}

TEST_F(RtsTest, testCreateCallWithOffline) {
    testCreateCallWithOffline();
}

TEST_F(RtsTest, testCloseCall) {
    testCloseCall();
}

TEST_F(RtsTest, testCloseCallSameTime) {
    testCloseCallSameTime();
}

TEST_F(RtsTest, testSendDatas) {
    testSendDatas();
}

TEST_F(RtsTest, testSendDataToEachOther) {
    testSendDataToEachOther();
}

TEST_F(RtsTest, testSendDataAfterClose) {
    testSendDataAfterClose();
}

TEST_F(RtsTest, testOneBrokenNetWorkRecoverBeforeConnTimeout) {
    testOneBrokenNetWorkRecoverBeforeConnTimeout();
}

/*
TEST_F(RtsTest, testOneBrokenNetWorkRecoverAfterConnTimeoutBeforeServerTimeout) {
    testOneBrokenNetWorkRecoverAfterConnTimeoutBeforeServerTimeout();
}
*/

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
