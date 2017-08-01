﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Win32;
using System.Diagnostics;
using System.Threading;
using System.IO;

namespace DefaultColor
{
    public static class Utility
    {
        public static object RegistryKeyRead(RegistryHive hKey, string SubKey, string Name, object defaultvalue)
        {
            if (Environment.Is64BitProcess)
                return RegistryKey.OpenBaseKey(hKey, RegistryView.Registry64).OpenSubKey(SubKey).GetValue(Name, defaultvalue);
            else
                return RegistryKey.OpenBaseKey(hKey, RegistryView.Registry32).OpenSubKey(SubKey).GetValue(Name, defaultvalue);

        }
        public static void RegistryKeyWrite(RegistryHive hKey, string SubKey, string Name, object setvalue, RegistryValueKind valuekind)
        {
            if (Environment.Is64BitProcess)
                RegistryKey.OpenBaseKey(hKey, RegistryView.Registry64).CreateSubKey(SubKey).SetValue(Name, setvalue, valuekind);
            else
                RegistryKey.OpenBaseKey(hKey, RegistryView.Registry32).CreateSubKey(SubKey).SetValue(Name, setvalue, valuekind);


        }
        public static void CheckProcessExistAndCreate(string processeName, string path)
        {
            bool exsit = false;
            try
            {
                foreach (var k in Process.GetProcessesByName(processeName))
                    exsit = true;
            }
            catch { }

            if (exsit == false)
            {
                try
                {
                    ProcessStartInfo ps;
                    ps = new ProcessStartInfo();
                    ps.FileName = processeName;
                    //ps.WorkingDirectory = new FileInfo(System.Reflection.Assembly.GetExecutingAssembly().Location).DirectoryName;
                    ps.WorkingDirectory = path;
                    ps.UseShellExecute = true;
                    ps.CreateNoWindow = false;
                    ps.RedirectStandardInput = false;
                    ps.RedirectStandardOutput = false;
                    Process.Start(ps);
                    Thread.Sleep(1000); //wait process initialize
                }
                catch
                {
                    Console.WriteLine("CheckProcessExistAndCreate failed");
                }

            }
        }

        public static void Log(string logMessage)
        {
#if DEBUG
            using (StreamWriter w = File.AppendText("MyColor2.log"))
            {
                w.WriteLine("{0}", logMessage);
            }
#endif
        }
    }
}
