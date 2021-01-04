/****************************************
*   Semantic data.hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.19
*****************************************/
#pragma once
#include <string>
#include <functional>
#include "cJSON.h"
#include "LogInterface.hpp"
#define TOTAL_URL_LIST 10
namespace AiBox
{
    typedef enum DirectiveType{
        Directive_Type_SCHEDULE=500,
        Directive_Type_URL=501,
        Directive_Type_MEDIA=502,
        Directive_Type_SMARTHOME=503,
        Directive_Type_TVMIC=504,
        Directive_Type_PLAYCONTROL=505,
        Directive_Type_BOXCONTROL=506,
        Directive_Type_URL_LIST=507,
        Directive_Type_JHK_MEDIA_LIST=508,
        Directive_Type_VOICE_MEMORY=509,
        Directive_Type_WAKEUP=510,
        Directive_Type_FART=511,
        Directive_Type_QQMUSIC=512,
        Directive_Type_SKILL=513,
        Directive_Type_SCENE=514
    }directive_type_t;
	enum TVMIC{
        TVMIC_CLOSE = 0,
        TVMIC_OPEN,
        TVMIC_EXEC,
        TVMIC_TVPUSH,
    };
    typedef struct MsgSchedule{
		std::string date_time;	//用户的定时时间
		std::string suggest_time; //建议提醒时间或者闹钟响起时间
		std::string content;	 //定时任务内容
		std::string intent;	   //创建闹钟为CREATE，取消闹钟为CANCEL
		std::string type;		   //"clock" or "reminder"
		std::string repeat;	   //"clock" or "reminder"
		std::string session_id;
		std::string scene;  //新闻、天气
	} msg_schedule_t;
    typedef struct MsgControl
	{
		std::string session_id;
		int param;		  //action的参数//无参数时为0xffff //时间以second为unit
		std::string action; //vol，volUp，volDown,pause，play , fastforward(unit：second)，rewind
	} msg_control_t;
    /*
	    used in SHM_ID_D2_TVMIC
	*/
	typedef struct MsgTvpartner
	{
		int action;		   //0:close, 1: open; 2:execute; 3,tvPush
		std::string content; //湖南卫视，中国好声音
		std::string session_id;
	} msg_tvpartner_t;
	typedef struct MsgSmartHome{
		std::string content;
		std::string session_id;
	}msg_common_t;
    typedef struct MsgUrl
	{
		std::string source; //url list size ////20190628  30
		std::string url;//最多支持10个url 列表////20190628  208
		std::string mediaId; //媒资ID
		std::string title; //标题
		int offsetInMilliseconds;  //2020/04/07 jyl
	} msg_url_t;
    typedef struct MsgUrlList
	{
		int url_num; //url list size
		msg_url_t url_item[TOTAL_URL_LIST];
		std::string tts_content;
		char isloop; ////1,单曲循环，2，随机播放，3，顺序播放, 4,循环播放, 88,未设置播放模式, 'n',仅仅本次播放内容为顺序播放，但播放模式的设置不改变
		std::string sort; //2020/04/07 jyl
	} msg_url_list_t;
	typedef struct MsgQQMusic
	{
		std::string url;	 //最多支持10个url 列表
		std::string song;	 //url list size
		std::string singer;   //url list size
		std::string song_id;  //url list size
		std::string album_id; //url list size
	} msg_qqmusic_t;
	typedef struct MsgQQMusicList
	{
		int url_num; //url list size
		msg_qqmusic_t url_item[TOTAL_URL_LIST];
		std::string tts_content;
		char isloop; //1,单曲循环，2，随机播放，3，顺序播放, 4,循环播放, 88,未设置播放模式, 'n',仅仅本次播放内容为顺序播放，但播放模式的设置不改变
	} msg_qqmusic_list_t;
	typedef struct MsgVoiceMemory
	{
		int action; //0--设置, 1--查询， 2--删除
		std::string content;
		std::string session_id;
	} msg_voice_memory_t;
	typedef struct MsgMedia
	{
		std::string session_id;   //用于上传歌曲名称时，整机将该session id回传给语音模块
		std::string song;		   //歌曲
		std::string artist;	        //歌手
		std::string tags;		   //标签
		std::string album;		   //专辑
		std::string ins_type;	 //收藏、播放收藏
		std::string tts_content; //当搜到对应的歌曲时播放
		char isloop;//1,单曲循环，2，随机播放，3，顺序播放, 4,循环播放, 88,未设置播放模式, 'n',仅仅本次播放内容为顺序播放，但播放模式的设置不改变
	} msg_media_t;
	typedef struct MsgJHKMedia
	{
		std::string vendor; //内容提供商id
		std::string src_id; //源内容id
		std::string obj_id; //聚好看媒资id
		int obj_child_type;
        std::string parent_media_id;
		std::string parent_src_id;
		int content_type;
		std::string isContinuedPlay;
	} msg_jhk_media_t;
	typedef struct MsgJHKMediaList
	{
		int jhkmedia_num; //url list size
		msg_jhk_media_t jhkmedia_item[TOTAL_URL_LIST];
		int index;		  //第几集
		std::string title;   //节目名称
		int time_seconds; //第几秒
		std::string tts_content;
		char isloop; //1,单曲循环，2，随机播放，3，顺序播放, 4,循环播放, 88,未设置播放模式, 'n',仅仅本次播放内容为顺序播放，但播放模式的设置不改变
		std::string session_id;
	} msg_jhkmedia_list_t;
    /*Directive base data*/
    template<typename T>
    class DirectiveDataBase
    {
        public:
			//used to vehicle the Overridable directive
            static DirectiveDataBase<T>* getSemanticDataSingletonPtr()
			{
    			if(_singleton_ptr==nullptr){
        			_singleton_ptr=new DirectiveDataBase<T>();
    			}
    			return _singleton_ptr;
			}
			void* operator new(std::size_t size)throw(std::bad_alloc)
			{
				return sysMemMalloc(size);
			}
			void operator delete(void *ptr)throw()
			{
				if(ptr==nullptr) return;
				sysMemFree(ptr);
				ptr=nullptr;
			}
			//used to vehicle the Do not allow cover directive 
			static  DirectiveDataBase<T>* creatSemanticDataVehicle()
			{
				return new DirectiveDataBase<T>();
			}
			static void getSemanticDataByptr(void* ptr,T& data)
			{
				auto directive_ptr=reinterpret_cast<DirectiveDataBase<T>*>(ptr);
				if(directive_ptr==nullptr){
					return;
				}
				//get the directive data
				data=directive_ptr->_data;
				//free the heap memory
				delete directive_ptr;
				//reset the ptr 
				ptr=nullptr;
			}
            void setData(const T& msg_data)
			{
    			_data=msg_data;
			}
        private:
            DirectiveDataBase<T>(){}
			~DirectiveDataBase<T>(){}
			static DirectiveDataBase<T>* _singleton_ptr;
            T _data;
    };
	template<typename T>
	DirectiveDataBase<T>* DirectiveDataBase<T>::_singleton_ptr=nullptr;
    
	typedef DirectiveDataBase<msg_schedule_t>       MsgScheduleClass;
    typedef DirectiveDataBase<msg_tvpartner_t>      MsgTvpartnerClass;
    typedef DirectiveDataBase<msg_url_list_t>       MsgUrlListClass;
    typedef DirectiveDataBase<msg_voice_memory_t>   MsgVoiceMemoryClass; 
    typedef DirectiveDataBase<msg_qqmusic_list_t>   MsgQQMusicListClass;
    typedef DirectiveDataBase<msg_media_t>          MsgMediaClass;
    typedef DirectiveDataBase<msg_jhkmedia_list_t>  MsgJHKMediaListClass;
	typedef DirectiveDataBase<msg_common_t> 		MsgCommonClass;
	typedef DirectiveDataBase<msg_control_t> 		MsgControlClass;
  
	typedef struct DirectiveQueue{
        directive_type_t _directive_type;
		void* _directive_ptr;
    }directive_queue_t;
};