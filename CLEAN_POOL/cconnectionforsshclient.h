#ifndef CCONNECTIONFORSSHCLIENT_H
#define CCONNECTIONFORSSHCLIENT_H

/* Func:以用户密码的形式连接ssh服务器  发送命令到shell执行  需加\n
 * Note:定时检查连接状态的逻辑是  连接成功关闭定时器检查
 *      连接断开  开启定时器检查并尝试重连  直至连接成功
 *      即关闭定时器检查
 * Use:绑定两个信号
 *      检测状态:sigConnectStateChanged(bState,strIp,nPort);
 *      接收信息:sigDataArrived(QString strMsg,QString strIp, int nPort);
 *     绑定一个槽
 *      发送信息:void slotSend(QString strMsg);
 */
#include <sshconnection.h>
#include <sshremoteprocess.h>
#include <sftpchannel.h>
#include <QTimer>
#include <QHostAddress>
#include <QThread>


#define RECONNET_SPAN_TIME (1000*10)  //连接状态心跳


class  CConnectionForSshClient : public QObject
{
    Q_OBJECT
public:
    explicit CConnectionForSshClient(QString strIp, int nPort = 22,QString strPwd = "17909",QString strUser = "root");

    void init();
    void unInit();

    ~CConnectionForSshClient();
private:
    QThread *m_pThread = nullptr;
    bool m_bConnected = false;
    bool m_bIsFirstConnect = true;
    bool m_bSendAble = false;

    QTimer *m_pTimer;

    QString m_strIp = "";
    int m_nPort = -1;
    QString m_strUser;
    QString m_strPwd;
    QString m_strIpPort;

    QSsh::SshConnectionParameters m_argParameters;
    QSsh::SshConnection *m_pSshSocket = nullptr;
    QSharedPointer<QSsh::SshRemoteProcess> m_shell;
signals:
    void sigInitForClild();
    void sigConnectStateChanged(bool bState,QString strIp,int nPort);
    void sigDataArrived(QString strMsg,QString strIp, int nPort);
private:
    int send(QString strMessage);
    QString getIpPort(){return m_strIp + ":" + QString::number(m_nPort);}
public slots:
    void slotResetConnection(QString strIpPort);
    void slotSend(QString strIpPort,QString strMessage);
    void slotSend(QString strMsg);
    void slotSendByQByteArray(QString strIpPort,QByteArray arrMsg);
    void slotDisconnected();
    void slotDataReceived();
private slots:
    void slotInitForClild();
    void slotCreateConnection();
    void slotConnected();

    void slotThreadFinished();

    void slotSshConnectError(QSsh::SshError sshError);
    void slotShellStart();
    void slotShellError();
};

#endif // CCONNECTIONFORSSHCLIENT_H

