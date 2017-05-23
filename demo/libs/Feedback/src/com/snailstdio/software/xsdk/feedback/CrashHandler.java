package com.snailstdio.software.xsdk.feedback;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.Writer;
import java.lang.Thread.UncaughtExceptionHandler;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;
import android.view.WindowManager;

import com.snailstdio.software.xsdk.logger.Logger;
import com.snailstdio.software.xsdk.utils.Cache;

public class CrashHandler implements UncaughtExceptionHandler {
    public static final String TAG = "CrashHandler";

    private static final String KEY_CRASH_TRACE = "crashTrace";

    // 系统默认的UncaughtException处理类
    private Thread.UncaughtExceptionHandler mDefaultHandler;
    // CrashHandler实例
    private static CrashHandler INSTANCE = new CrashHandler();
    // 程序的Context对象
    private Context mContext;

    private Class<?> mMainActivity;

    /** 保证只有一个CrashHandler实例 */
    private CrashHandler() {
    }

    /** 获取CrashHandler实例 ,单例模式 */
    public static CrashHandler getInstance() {
        return INSTANCE;
    }

    /**
     * 初始化
     * 
     * @param context
     */
    public void init(Context context, Class<?> mainActivity) {
        if (mContext == null) {
            mContext = context;
            mMainActivity = mainActivity;
            // 获取系统默认的UncaughtException处理器
            mDefaultHandler = Thread.getDefaultUncaughtExceptionHandler();
            // 设置该CrashHandler为程序的默认处理器
            Thread.setDefaultUncaughtExceptionHandler(this);

            final String crashTrace = Cache.read_String(KEY_CRASH_TRACE, null);
            Logger.d("crashTrace : " + (crashTrace != null));
            if (!TextUtils.isEmpty(crashTrace)) {
                Cache.write_String(KEY_CRASH_TRACE, null);
                new Thread() {
                    @Override
                    public void run() {
                        String submitFeedback = MailSender.submitFeedback(
                                mContext, "Crash trace:\n" + crashTrace, null);
                        Logger.d("submitFeedback : " + submitFeedback);
                    }
                }.start();
            }

        }
    }

    /**
     * 当UncaughtException发生时会转入该函数来处理
     */
    @Override
    public void uncaughtException(final Thread thread, final Throwable ex) {
        Log.e("uncaughtException", ex.toString());

        new Thread() {
            @Override
            public void run() {
                Looper.prepare();

                AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
                builder.setTitle(R.string.prompt);
                builder.setMessage(R.string.feedback_handle_crash);
                builder.setCancelable(false);
                builder.setNegativeButton("OK", new OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.cancel();
                        if (!handleException(ex) && mDefaultHandler != null) {
                            // 如果用户没有处理则让系统默认的异常处理器来处理
                            mDefaultHandler.uncaughtException(thread, ex);
                        } else {
                            Intent intent = new Intent(mContext, mMainActivity);
                            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                            mContext.startActivity(intent);
                            android.os.Process.killProcess(android.os.Process
                                    .myPid());
                        }
                    }
                });
                Dialog dialog = builder.create();
                dialog.getWindow().setType(
                        WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
                dialog.show();

                Looper.loop();
            }
        }.start();

    }

    /**
     * 自定义错误处理,收集错误信息 发送错误报告等操作均在此完成.
     * 
     * @param ex
     * @return true:如果处理了该异常信息;否则返回false.
     */
    private boolean handleException(Throwable ex) {
        Log.d("handleException", ex.toString());
        if (ex == null) {
            return false;
        }
        // 保存日志文件
        return saveCrashInfo2File(ex);
    }

    /**
     * 保存错误信息到文件中
     * 
     * @param ex
     * @return 返回文件名称,便于将文件传送到服务器
     */
    private boolean saveCrashInfo2File(Throwable ex) {

        Writer writer = new StringWriter();
        PrintWriter printWriter = new PrintWriter(writer);
        ex.printStackTrace(printWriter);
        Throwable cause = ex.getCause();
        while (cause != null) {
            cause.printStackTrace(printWriter);
            cause = cause.getCause();
        }
        printWriter.close();
        String result = writer.toString();
        Log.e("Crash", result);

        Cache.write_String(KEY_CRASH_TRACE, result);
        return true;
    }

}