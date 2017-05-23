package com.snailstdio.software.xsdk.feedback;

import java.util.Date;
import java.util.Properties;

import javax.mail.Address;
import javax.mail.Authenticator;
import javax.mail.BodyPart;
import javax.mail.Message;
import javax.mail.MessagingException;
import javax.mail.Multipart;
import javax.mail.PasswordAuthentication;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeBodyPart;
import javax.mail.internet.MimeMessage;
import javax.mail.internet.MimeMultipart;

import android.content.Context;

public class MailSender {
    // 发送邮件的服务器的IP和端口
    private String mailServerHost;
    private String mailServerPort = "25";

    // 邮件发送者的地址
    private String fromAddress;
    // 邮件接收者的地址
    private String toAddress;
    // 登陆邮件发送服务器的用户名和密码
    private String userName;
    private String password;
    // 是否需要身份验证
    private boolean validate = true;
    // 邮件主题
    private String subject;
    // 邮件的文本内容
    private String content;
    // 邮件附件的文件名
    private String[] attachFileNames;

    /**
     * 获得邮件会话属性
     */
    public Properties getProperties() {
        Properties p = new Properties();
        p.put("mail.smtp.host", this.mailServerHost);
        p.put("mail.smtp.port", this.mailServerPort);
        p.put("mail.smtp.auth", validate ? "true" : "false");
        return p;
    }

    public String getMailServerHost() {
        return mailServerHost;
    }

    public void setMailServerHost(String mailServerHost) {
        this.mailServerHost = mailServerHost;
    }

    public String getMailServerPort() {
        return mailServerPort;
    }

    public void setMailServerPort(String mailServerPort) {
        this.mailServerPort = mailServerPort;
    }

    public void setValidate(boolean validate) {
        this.validate = validate;
    }

    public String[] getAttachFileNames() {
        return attachFileNames;
    }

    public void setAttachFileNames(String[] fileNames) {
        this.attachFileNames = fileNames;
    }

    public void setFromAddress(String fromAddress) {
        this.fromAddress = fromAddress;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public void setToAddress(String toAddress) {
        this.toAddress = toAddress;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public void setSubject(String subject) {
        this.subject = subject;
    }

    public void setContent(String textContent) {
        this.content = textContent;
    }

    /**
     * 以文本格式发送邮件
     * 
     * @param mailInfo
     *            待发送的邮件的信息
     */
    public boolean sendTextMail() {
        // 判断是否需要身份认证
        MyAuthenticator authenticator = null;
        Properties pro = this.getProperties();
        if (validate) {
            // 如果需要身份认证，则创建一个密码验证器
            authenticator = new MyAuthenticator(userName, password);
        }
        // 根据邮件会话属性和密码验证器构造一个发送邮件的session
        Session sendMailSession = Session
                .getDefaultInstance(pro, authenticator);
        try {
            // 根据session创建一个邮件消息
            Message mailMessage = new MimeMessage(sendMailSession);
            // 创建邮件发送者地址
            Address from = new InternetAddress(fromAddress);
            // 设置邮件消息的发送者
            mailMessage.setFrom(from);
            // 创建邮件的接收者地址，并设置到邮件消息中
            Address to = new InternetAddress(toAddress);
            mailMessage.setRecipient(Message.RecipientType.TO, to);
            // 设置邮件消息的主题
            mailMessage.setSubject(subject);
            // 设置邮件消息发送的时间
            mailMessage.setSentDate(new Date());
            // 设置邮件消息的主要内容
            String mailContent = content;
            mailMessage.setText(mailContent);
            // 发送邮件
            Transport.send(mailMessage);
            return true;
        } catch (MessagingException ex) {
            ex.printStackTrace();
        }
        return false;
    }

    /**
     * 以HTML格式发送邮件
     * 
     * @param mailInfo
     *            待发送的邮件信息
     */
    public boolean sendHtmlMail() {
        // 判断是否需要身份认证
        MyAuthenticator authenticator = null;
        Properties pro = this.getProperties();
        // 如果需要身份认证，则创建一个密码验证器
        if (validate) {
            authenticator = new MyAuthenticator(userName, password);
        }
        // 根据邮件会话属性和密码验证器构造一个发送邮件的session
        Session sendMailSession = Session
                .getDefaultInstance(pro, authenticator);
        try {
            // 根据session创建一个邮件消息
            Message mailMessage = new MimeMessage(sendMailSession);
            // 创建邮件发送者地址
            Address from = new InternetAddress(fromAddress);
            // 设置邮件消息的发送者
            mailMessage.setFrom(from);
            // 创建邮件的接收者地址，并设置到邮件消息中
            Address to = new InternetAddress(toAddress);
            // Message.RecipientType.TO属性表示接收者的类型为TO
            mailMessage.setRecipient(Message.RecipientType.TO, to);
            // 设置邮件消息的主题
            mailMessage.setSubject(subject);
            // 设置邮件消息发送的时间
            mailMessage.setSentDate(new Date());
            // MiniMultipart类是一个容器类，包含MimeBodyPart类型的对象
            Multipart mainPart = new MimeMultipart();
            // 创建一个包含HTML内容的MimeBodyPart
            BodyPart html = new MimeBodyPart();
            // 设置HTML内容
            html.setContent(content, "text/html; charset=utf-8");
            mainPart.addBodyPart(html);
            // 将MiniMultipart对象设置为邮件内容
            mailMessage.setContent(mainPart);
            // 发送邮件
            Transport.send(mailMessage);
            return true;
        } catch (MessagingException ex) {
            ex.printStackTrace();
        }
        return false;
    }

    public class MyAuthenticator extends Authenticator {
        String userName = null;
        String password = null;

        public MyAuthenticator() {
        }

        public MyAuthenticator(String username, String password) {
            this.userName = username;
            this.password = password;
        }

        protected PasswordAuthentication getPasswordAuthentication() {
            return new PasswordAuthentication(userName, password);
        }
    }

    public static String submitFeedback(Context context, String content,
            String user) {
        try {
            MailSender mailInfo = new MailSender();
            mailInfo.setMailServerHost("smtp.163.com");
            mailInfo.setMailServerPort("25");
            mailInfo.setValidate(true);
            mailInfo.setUserName(MailData.user_mail_id); // 你的邮箱地址
            mailInfo.setPassword(MailData.user_mail_password);// 您的邮箱密码
            mailInfo.setFromAddress(MailData.user_mail_id);
            mailInfo.setToAddress(MailData.my_mail_id);
            int appNameId = context.getResources().getIdentifier("app_name",
                    "string", context.getPackageName());
            mailInfo.setSubject(context.getString(appNameId)
                    + context.getString(R.string.feedback_from) + user);
            mailInfo.setContent(DeviceInfo.getAllInfo(context)
                    + context.getString(R.string.feedback_content) + content);

            if (mailInfo.sendTextMail()) {
                return "COMPLETE";
            } else {
                return "ERROR";
            }

        } catch (Exception e) {
            return "ERROR";
        }
    }
}
