package com.snailstdio.software.xsdk.feedback;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.text.NumberFormat;

import android.app.ActivityManager;
import android.app.ActivityManager.MemoryInfo;
import android.content.Context;
import android.os.Build;
import android.os.Environment;
import android.os.StatFs;
import android.util.Log;

import com.snailstdio.software.xsdk.utils.ApplicationUtils;
import com.snailstdio.software.xsdk.utils.ScreenInfo;

public class DeviceInfo {

    public static String get_phone_model() {
        return Build.MODEL;
    }

    public static String get_system_edition() {
        return Build.VERSION.RELEASE;
    }

    public static String get_system_edition_number() {
        return Build.DISPLAY;
    }

    public static String getCpuName() {
        try {
            FileReader fr = new FileReader("/proc/cpuinfo");
            BufferedReader br = new BufferedReader(fr);
            String text = br.readLine();
            String[] array = text.split(":\\s+", 2);
            for (int i = 0; i < array.length; i++) {
            }
            return array[1];
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    public static String getMaxCpuFreq() {
        String result = "";
        ProcessBuilder cmd;
        try {
            String[] args = { "/system/bin/cat",
                    "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq" };
            cmd = new ProcessBuilder(args);
            Process process = cmd.start();
            InputStream in = process.getInputStream();
            byte[] re = new byte[24];
            while (in.read(re) != -1) {
                result = result + new String(re);
            }
            in.close();
        } catch (IOException ex) {
            ex.printStackTrace();
            result = "N/A";
        }

        double result_double = 0;
        try {
            result_double = Double.valueOf(result.trim());
        } catch (Exception e) {
            e.printStackTrace();
        }

        NumberFormat f = NumberFormat.getInstance(); // 创建一个格式化类f
        f.setMaximumFractionDigits(2); // 设置小数位的格式
        String str_result = f.format(result_double / 1000); // 格式化数据a,将a格式化为f

        str_result = remove(str_result, ',');
        str_result += "MHz";
        return str_result;
    }

    public static String remove(String d, char c) {
        String s1, s2;

        int id = d.indexOf(c);
        while (id != -1) {
            s1 = d.substring(0, id);
            s2 = d.substring(id + 1, d.length());
            d = s1 + s2;
            id = d.indexOf(c);
        }

        return d;
    }

    public static String get_total_rom_memory() {
        File path = Environment.getDataDirectory();
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSize();
        long totalBlocks = stat.getBlockCount();
        return (totalBlocks * blockSize / 1024 / 1024) + "MB";
    }

    public static String get_available_rom_memory() {
        long romInfo;
        // Total rom memory

        // Available rom memory
        File path = Environment.getDataDirectory();
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSize();
        long availableBlocks = stat.getAvailableBlocks();
        romInfo = blockSize * availableBlocks;
        // getVersion();
        return (romInfo / 1024 / 1024) + "MB";
    }

    public static String[] getSDCardMemory() {
        String[] sdCardInfo = new String[2];
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            File sdcardDir = Environment.getExternalStorageDirectory();
            StatFs sf = new StatFs(sdcardDir.getPath());
            long bSize = sf.getBlockSize();
            long bCount = sf.getBlockCount();
            long availBlocks = sf.getAvailableBlocks();

            sdCardInfo[0] = (bSize * bCount / 1024 / 1024) + "MB";// 总大小
            sdCardInfo[1] = (bSize * availBlocks / 1024 / 1024) + "MB";// 可用大小
        }
        return sdCardInfo;
    }

    public static String get_rest_Memory(Context context) {
        long all = getTotalMemory();
        long use = all - getAvailMemory(context);

        return String.valueOf(use) + "MB" + "("
                + String.valueOf((int) ((double) use / (double) all * 100))
                + "%)";
    }

    public static String get_total_Memory() {
        return String.valueOf(getTotalMemory()) + "MB";
    }

    public static long getAvailMemory(Context context) {
        // 获取android当前可用内存大小
        ActivityManager am = (ActivityManager) context
                .getSystemService(Context.ACTIVITY_SERVICE);
        MemoryInfo mi = new MemoryInfo();
        am.getMemoryInfo(mi);
        // mi.availMem; 当前系统的可用内存

        // return Formatter.formatFileSize(context, mi.availMem);// 将获取的内存大小规格化
        return mi.availMem / (1024 * 1024);
    }

    public static long getTotalMemory() {
        String str1 = "/proc/meminfo";// 系统内存信息文件
        String str2;
        String[] arrayOfString;
        long initial_memory = 0;

        try {
            FileReader localFileReader = new FileReader(str1);
            BufferedReader localBufferedReader = new BufferedReader(
                    localFileReader, 8192);
            str2 = localBufferedReader.readLine();// 读取meminfo第一行，系统总内存大小

            arrayOfString = str2.split("\\s+");
            for (String num : arrayOfString) {
                Log.i(str2, num + "\t");
            }

            initial_memory = Integer.valueOf(arrayOfString[1]).intValue() * 1024;// 获得系统总内存，单位是KB，乘以1024转换为Byte
            localBufferedReader.close();

        } catch (IOException e) {
        }
        // return Formatter.formatFileSize(context, initial_memory);//
        // Byte转换为KB或者MB，内存大小规格化
        return initial_memory / (1024 * 1024);
    }

    public static String getCPUABI() {
        return android.os.Build.CPU_ABI;
    }

    public static String getAllInfo(Context context) {
        String SDCardMemory[] = DeviceInfo.getSDCardMemory();
        String str = "设备型号: " + DeviceInfo.get_phone_model() + "\n"
                + "操作系统版本: " + DeviceInfo.get_system_edition() + "\n"
                + "操作系统版本号: " + DeviceInfo.get_system_edition_number() + "\n"
                + "CPU型号: " + DeviceInfo.getCpuName() + "\n" + "CPU频率: "
                + DeviceInfo.getMaxCpuFreq() + "\n" + "CPU架构: "
                + DeviceInfo.getCPUABI() + "\n" + "内部存储器容量: "
                + DeviceInfo.get_total_rom_memory() + "MB\n" + "内部存储器剩余: "
                + DeviceInfo.get_available_rom_memory() + "MB\n" + "SD卡容量: "
                + SDCardMemory[0] + "\n" + "SD卡剩余: " + SDCardMemory[1] + "\n"
                + "全部内存: " + DeviceInfo.get_total_Memory() + "\n" + "已用内存: "
                + DeviceInfo.get_rest_Memory(context) + "\n" + "屏幕分辨率: "
                + ScreenInfo.getInstance().getSize() + "\n" + "当前版本: "
                + ApplicationUtils.getVersionName(context) + "\n\n";

        return str;

    }
}