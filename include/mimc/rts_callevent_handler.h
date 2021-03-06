#ifndef MIMC_CPP_SDK_RTS_CALLEVENTHANDLER_H
#define MIMC_CPP_SDK_RTS_CALLEVENTHANDLER_H

#include <string>
#include <mimc/launchedresponse.h>
#include <mimc/constant.h>

class RTSCallEventHandler {
public:
	/**
	 * 新会话接入回调
	 **/
	virtual LaunchedResponse onLaunched(long chatId, const std::string fromAccount, const std::string appContent, const std::string fromResource) = 0;
	/**
	 * 会话被接通回调
	 **/
	virtual void onAnswered(long chatId, bool accepted, const std::string errmsg) = 0;

	/**
	 * 会话被关闭回调
	 **/
	virtual void onClosed(long chatId, const std::string errmsg) = 0;

	/**
	 * 接收到数据的回调
	 **/
	virtual void handleData(long chatId, const std::string data, RtsDataType dataType, RtsChannelType channelType) = 0;

	/**
     * 发送数据成功的回调
     */
	virtual void handleSendDataSucc(long chatId, int groupId, const std::string ctx) = 0;

	/**
     * 发送数据失败的回调
     */
	virtual void handleSendDataFail(long chatId, int groupId, const std::string ctx) = 0;

	virtual ~RTSCallEventHandler() {}
};
#endif