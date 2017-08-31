/*
 * CLog.h
 *
 *  Created on: 2015年4月22日
 *      Author: chuanjiang.zh@qq.com
 */

#ifndef CLOG_H_
#define CLOG_H_

#include <stdio.h>
#include <stdarg.h>


/**
 * 写日志函数
 * @param level 日志级别
 * @param data  日志文本
 * @param length 文本长度
 * @param flags 日志输出标志 @see CLog::FormatFlag
 */
typedef void (*WriteLog)(int level, const char* data, int length, int flags);


/**
 * 简单的日志输出类
 * 用法一（代码配置）
 *  CLog::startup("");
 *  CLog::setLogger(CLog::kCout);
 *  CLog::setLogger(CLog::kFile); /// 使用默认文件输出参数
 *  CLog::debug("%s go go go!\n", CLog::getTime());
 *  /// ...
 *  CLog::cleanup(); /// 程序退出前调用
 *
 *
 * 用法二（文件配置）
 *  CLog::startup("applog.ini");
 *  CLog::debug("%s go go go!\n", CLog::getTime());
 *  /// ...
 *  CLog::cleanup(); /// 程序退出前调用
 *
 */
class CLog
{
public:
    CLog();
    virtual ~CLog();

    /// 日志级别
    enum Level
    {
        kNone =  0,
        kDebug = 1,
        kInfo,
        kWarning,
        kError,
        kFatal
    };

    /// 日志输出类型
    enum LoggerType
    {
        COUT = 0,  /// 控制台输出
        SYSLOG,    /// 系统日志
        FILE,      /// 循环文件输出
        DEBUGWINDOW,   /// windows系统下的调试输出，即 OutputDebugString

        EXTERNAL,      /// 外部输出

        MAXLOGGER
    };

    enum FormatFlag
	{
    	DATETIME = 0x01,
		TIME 	 = 0x02,
	};

    /**
     * 初始化
     * @param configPath 日志配置文件路径,可以为NULL或者空字符串“”
     * @return 错误码，0 表示成功
     */
    static int startup(const char* configPath);

    /**
     * 清理
     */
    static void cleanup();

    /**
     * 写日志
     * @param level 日志级别
     * @param format 日志格式
     */
    static void log(int level, const char* format, ...);

    /**
     * 写日志
     * @param level 日志级别
     * @param format 日志格式
     * @param args 可变参数
     */
    static void logv(int level, const char* format, va_list args);

    /**
     * 写日志
     * @param level 日志级别
     * @param data  日志内容
     * @param length 日志长度
     */
    static void writeLog(int level, const char* data, int length);


    /**
     * 写调试级别的日志
     * @param format 日志格式
     */
    static void debug(const char* format, ...);

    /**
     * 写信息级别的日志
     * @param fmt 日志格式
     */
    static void info(const char* format, ...);

    /**
     * 写警告级别的日志
     * @param fmt 日志格式
     */
    static void warning(const char* format, ...);

    /**
     * 写错误级别的日志
     * @param fmt 日志格式
     */
    static void error(const char* format, ...);

    /**
     * 写严重错误级别的日志
     * @param fmt 日志格式
     */
    static void fatal(const char* format, ...);

    /**
     * 启用指定类型的日志输出
     * @param type 输出类型
     * @param flags 输出格式标志 @see FormatFlag
     * @return 错误码， 0 表示成功
     */
    static int setLogger(LoggerType type, Level level = kNone, int flags = DATETIME);

    /**
     * 设置文件输出的参数.
     * 启用文件输出时，不调用该API，这些参数就取默认值
     * @param filename  日志文件路径名称,默认为：app.log
     * @param fileSize  日志文件大小,单位为MB,默认值为：10M
     * @param rollCount 日志文件个数（循环回滚个数）,默认值为: 2
     * @return 错误码， 0 表示成功
     */
    static int setFileParam(const char* filename, int fileSize, int rollCount);

    /**
     * 启用外部日志输出
     * @param logger
     */
    static void setExtLogger(WriteLog logger, Level level = kNone);

    /**
     * 重置日志输出
     */
    static void resetLogger();

    /**
     * 重新加载配置文件
     * @param configPath
     * @return 错误码， 0 表示成功
     */
    static int reload(const char* configPath);

    /**
     * 获取当前系统日期时间.
     * 格式：年-月-日 时:分:秒.毫秒， 比如： 2015-01-15 17:06:06.999
     * @return 时间字符串
     */
    static const char* getDateTime();

    /**
     * 获取当前系统时间，不包括日期.
     * 格式：时:分:秒.毫秒，比如： 17:06:06.999
     * @return 时间字符串
     */
    static const char* getShortTime();


    /**
     * 获取当前系统时间
     * @param ms 毫秒部分
     * @return 单位为秒的时间
     */
    static int getCurTime(int* ms);


    /**
     * 获取日志级别的名称
     * @param level 日志级别
     * @return 名称
     */
    static const char* getName(int level);

    /**
     * 日志输出接口
     */
    class Writer
    {
    public:
        virtual ~Writer() {}

        /**
         * 输出日志
         * @param level 日志级别
         * @param msg   日志内容
         * @param length 日志内容长度
         * @param flags 输出格式标志 @see FormatFlag
         */
        virtual void write(int level, const char* msg, int length, int flags) =0;
    };


    /**
     * 启用外部日志输出
     * @param logger 必须是new创建的
     */
    static void setExtLogger(Writer* logger);


};


#endif /* CLOG_H_ */
