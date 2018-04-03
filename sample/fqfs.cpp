#include "fqfs.h"
#include <iostream>
using namespace std;

fqfs::fqfs(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.s1->setReadFrameTimeout(10);
	ui.s1->openStream("rtsp:admin:briup2017@192.168.1.110");
	ui.s2->openStream("rtsp:admin:briup2017@192.168.1.120");
	ui.s3->openStream("rtsp:admin:briup2017@192.168.1.130");
	ui.s4->openStream("rtsp:admin:briup2017@192.168.1.140");
}
