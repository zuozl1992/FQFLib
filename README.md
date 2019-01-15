# FQFLib
## FQFLib V2.0 [fqflib.zuozl.com](http://fqflib.zuozl.com).
* 基于FFmpeg的rtsp/rtmp网络流媒体播放库
	* 项目文档：
		* [fqflib.zuozl.com/doc](http://fqflib.zuozl.com/index.php/document/)；
		* 文档制作中，稍后即可访问。
	* 目录介绍：
		* src->Qt版本源码，直接添加pri文件到工程即可使用，请在pri文件配置本地FFmpeg目录；
		* sample->项目示例。
	* 使用说明：
		* 对应不同平台需要实现音视频对应的驱动，继承项目中的FQFAudioDevice与FQFVideoDevice类完成即可；
		* 将对应的对象指针通过FQFDemuxThread构造函数即可；
		* FQFDemuxThread为顶层操作类，实例化该类即可完成对应功能；
		* sample中有qt音频播放驱动及qtquick视频（图片）传递驱动示例。
		* 使用ffmpeg 4.1完成，运行时需要具有ffmpeg对应的DLL文件