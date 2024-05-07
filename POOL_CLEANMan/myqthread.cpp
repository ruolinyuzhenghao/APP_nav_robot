#include "myqthread.h"

myQthread::myQthread()
{

}
void myQthread::setThreadName(QString name)
{
    threadName = name;
}

//实现run函数
void myQthread::run()
{
    qDebug()<<"MyThread::run:"<<QThread::currentThreadId();

    qDebug() << threadName << "I'am A thread";
    Assessment();
    get_cnahu(ph,wendu);
    emit updateSignal(ph,wendu);
    sleep(1);
}

void myQthread::get_cnahu(double a,double b)
{
    qDebug()<<"zi--->"<<a;
    qDebug()<<"zi--->"<<b;
}
void myQthread::get_shuizhicanshu(double a,double b)
{
    ph=a;
    wendu=b;
    qDebug()<<"zi canshu--->"<<ph;
    qDebug()<<"zi canshu--->"<<wendu;
}
/*******************************
 函数功能：调用api水质评估
*******************************/
inline size_t onWriteData(void * buffer, size_t size, size_t nmemb, void * userp)
{
    std::string * str = dynamic_cast<std::string *>((std::string *)userp);
    str->append((char *)buffer, size * nmemb);
    return nmemb;
}
void myQthread::Assessment()
{
    std::string result;
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/table_infer/water_detect?access_token=24.2b9d7c269cbdfe55354251041cc34503.2592000.1692451891.282335-36435223");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        //c++格式属实写吐了，官方没有表格数据的例程，在那儿狂试格式
        //最后还是粘贴验证模型时的json的格式，会报错，将“col0”-->\"col0\"
        const char * data="{\"data\":[{\"col0\":9.45,\"col1\":18.46,\"col2\":34.61,\"col3\":0.02,\"col4\":1.97,\"col5\":7.89}]}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, onWriteData);
        res = curl_easy_perform(curl);
        qDebug()<<QString::fromStdString(result);
    }
    curl_easy_cleanup(curl);

}
