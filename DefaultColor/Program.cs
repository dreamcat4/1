using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace DefaultColor
{
    class Program
    {
        private static uint DFKeyboardCS(uint[] data, uint len)
        {
            uint i;
            uint sum;

            sum = 0;

            for (i = 0; i < len; i++)
                sum += data[i];

            return (uint)(0xFF - sum);
        }

        static void Main(string[] args)
        {
            int type = NativeMethod.GRGBDLL_GetMEKBTYPE();
            if (type == Type_RGBKY_DLL.ME_KB_FAIL)
            {
                return;
            }
            else if (type == Type_RGBKY_DLL.FULL_ME_KB)
            {
                DLLBuffer m_windows_DLLBuffer = new DLLBuffer();
                m_windows_DLLBuffer.singleRGB = new byte[4];
                m_windows_DLLBuffer.userRGB = new NativeKBRGB[16];
                m_windows_DLLBuffer.KBRGB = new NativeKBRGB[126];
                m_windows_DLLBuffer.bkeyCount = 126; // reserved

                m_windows_DLLBuffer.Mode = 0; // Hans's spec,  0= windows, 1 = Welcome
                m_windows_DLLBuffer.Effect = 0x05;  //Rainbow mode
                m_windows_DLLBuffer.Brightnesslevel = 0x16; //level 2
                m_windows_DLLBuffer.Tempolevel = 0x05;
                m_windows_DLLBuffer.Direction = 0x00;
                m_windows_DLLBuffer.NVsaving = true;

                m_windows_DLLBuffer.userRGB[1].ID = 1; m_windows_DLLBuffer.userRGB[1].R = 0xFF; m_windows_DLLBuffer.userRGB[1].G = 0x00; m_windows_DLLBuffer.userRGB[1].B = 0x00;
                m_windows_DLLBuffer.userRGB[2].ID = 1; m_windows_DLLBuffer.userRGB[2].R = 0xFF; m_windows_DLLBuffer.userRGB[2].G = 0x7D; m_windows_DLLBuffer.userRGB[2].B = 0x00;
                m_windows_DLLBuffer.userRGB[3].ID = 1; m_windows_DLLBuffer.userRGB[3].R = 0xFF; m_windows_DLLBuffer.userRGB[3].G = 0xFF; m_windows_DLLBuffer.userRGB[3].B = 0x00;
                m_windows_DLLBuffer.userRGB[4].ID = 1; m_windows_DLLBuffer.userRGB[4].R = 0x00; m_windows_DLLBuffer.userRGB[4].G = 0xFF; m_windows_DLLBuffer.userRGB[4].B = 0x00;
                m_windows_DLLBuffer.userRGB[5].ID = 1; m_windows_DLLBuffer.userRGB[5].R = 0x00; m_windows_DLLBuffer.userRGB[5].G = 0x00; m_windows_DLLBuffer.userRGB[5].B = 0xFF;
                m_windows_DLLBuffer.userRGB[6].ID = 1; m_windows_DLLBuffer.userRGB[6].R = 0x00; m_windows_DLLBuffer.userRGB[6].G = 0xFF; m_windows_DLLBuffer.userRGB[6].B = 0xFF;
                m_windows_DLLBuffer.userRGB[7].ID = 1; m_windows_DLLBuffer.userRGB[7].R = 0xFF; m_windows_DLLBuffer.userRGB[7].G = 0x00; m_windows_DLLBuffer.userRGB[7].B = 0xFF;

                //m_DLLBuffer.user_sec = TranslateDarfonSeconds();
                NativeMethod.GRGBDLL_CaptureCMDBuffer(ref m_windows_DLLBuffer);


                uint Checksum = 0;
                //Set Default RainDrop momde....
                uint[] data = new uint[7] { 0x08, 0x03, 0x0A, 0x05, 0x16, 0x08, 0x00 };
                Checksum = DFKeyboardCS(data, 7);
                string[] av = new string[] { "OemServiceWinApp.exe", "ledkb", "/setdata", "0x08", "0x03", "0x0A","0x05", "0x16","0x08", "0x00",
                    Checksum.ToString() };
                IntPtr buffer = Marshal.AllocHGlobal(256);
                NativeMethod.OemSvcHook(11, av, buffer, System.Runtime.InteropServices.Marshal.SizeOf(buffer));
                Marshal.FreeHGlobal(buffer);

                NativeKBRGB[] welColr = new NativeKBRGB[16];
                welColr[0].ID = 0; welColr[0].R = 0x00; welColr[0].G = 0x00; welColr[0].B = 0x00;
                welColr[1].ID = 1; welColr[1].R = 0xFF; welColr[1].G = 0x00; welColr[1].B = 0x00;
                welColr[2].ID = 1; welColr[2].R = 0xFF; welColr[2].G = 0x7D; welColr[2].B = 0x00;
                welColr[3].ID = 1; welColr[3].R = 0xFF; welColr[3].G = 0xFF; welColr[3].B = 0x00;
                welColr[4].ID = 1; welColr[4].R = 0x00; welColr[4].G = 0xFF; welColr[4].B = 0x00;
                welColr[5].ID = 1; welColr[5].R = 0x00; welColr[5].G = 0x00; welColr[5].B = 0xFF;
                welColr[6].ID = 1; welColr[6].R = 0x00; welColr[6].G = 0xFF; welColr[6].B = 0xFF;
                welColr[7].ID = 1; welColr[7].R = 0xFF; welColr[7].G = 0x00; welColr[7].B = 0xFF;

                for (int i = 9; i < 16; i++)
                {
                    welColr[i].ID = 0; welColr[i].R = 0; welColr[i].G = 0; welColr[i].B = 0;
                }
                NativeMethod.GRGBDLL_iSetWelColor_14H(welColr);
            }
            else if(type == Type_RGBKY_DLL.FOURZONE_ME_KB)
            {
                FOURZONE_DLLBUFFER m_windows_FOURZONE_DLLBuffer = new FOURZONE_DLLBUFFER();
                m_windows_FOURZONE_DLLBuffer.userRGB = new NativeKBRGB[16];
                m_windows_FOURZONE_DLLBuffer._TSLedType_08H.bEffectType = 0x05;  //Rainbow
                m_windows_FOURZONE_DLLBuffer._TSLedType_08H.blight = 0x16; //level 2
                m_windows_FOURZONE_DLLBuffer._TSLedType_08H.bSpeed = 0x05; // 
                m_windows_FOURZONE_DLLBuffer._TSLedType_08H.bdirection = 0x01;
                m_windows_FOURZONE_DLLBuffer._TSLedType_08H.bsavingMode = true;

                m_windows_FOURZONE_DLLBuffer._TSLedType_08H.bColor = 0x08;  // 08:circular color, 09: color index 1
                m_windows_FOURZONE_DLLBuffer.userRGB[1].ID = 1; m_windows_FOURZONE_DLLBuffer.userRGB[1].R = 0xFF; m_windows_FOURZONE_DLLBuffer.userRGB[1].G = 0x00; m_windows_FOURZONE_DLLBuffer.userRGB[1].B = 0x00;
                m_windows_FOURZONE_DLLBuffer.userRGB[2].ID = 1; m_windows_FOURZONE_DLLBuffer.userRGB[2].R = 0xFF; m_windows_FOURZONE_DLLBuffer.userRGB[2].G = 0xFF; m_windows_FOURZONE_DLLBuffer.userRGB[2].B = 0x00;
                m_windows_FOURZONE_DLLBuffer.userRGB[3].ID = 1; m_windows_FOURZONE_DLLBuffer.userRGB[3].R = 0x00; m_windows_FOURZONE_DLLBuffer.userRGB[3].G = 0x00; m_windows_FOURZONE_DLLBuffer.userRGB[3].B = 0xFF;
                m_windows_FOURZONE_DLLBuffer.userRGB[4].ID = 1; m_windows_FOURZONE_DLLBuffer.userRGB[4].R = 0xFF; m_windows_FOURZONE_DLLBuffer.userRGB[4].G = 0x00; m_windows_FOURZONE_DLLBuffer.userRGB[4].B = 0xFF;

                NativeMethod.GRGBDLL_Capture4ZONEBuffer(ref m_windows_FOURZONE_DLLBuffer);

                uint Checksum = 0;
                //Set Default Wave momde....
                uint[] data = new uint[7] { 0x08, 0x03, 0x03, 0x05, 0x16, 0x08, 0x00 };
                Checksum = DFKeyboardCS(data, 7);
                string[] av = new string[] { "OemServiceWinApp.exe", "ledkb", "/setdata", "0x08", "0x03", "0x03","0x05", "0x16","0x08", "0x00",
                    Checksum.ToString() };
                IntPtr buffer = Marshal.AllocHGlobal(256);
                NativeMethod.OemSvcHook(11, av, buffer, System.Runtime.InteropServices.Marshal.SizeOf(buffer));
                Marshal.FreeHGlobal(buffer);

                NativeKBRGB[] welColr = new NativeKBRGB[16];
                welColr[0].ID = 0; welColr[0].R = 0x00; welColr[0].G = 0x00; welColr[0].B = 0x00;
                //Red Yellow Blue Purple
                welColr[1].ID = 1; welColr[1].R = 0xFF; welColr[1].G = 0x00; welColr[1].B = 0x00;
                welColr[2].ID = 1; welColr[2].R = 0xFF; welColr[2].G = 0xFF; welColr[2].B = 0x00;
                welColr[3].ID = 1; welColr[3].R = 0x00; welColr[3].G = 0x00; welColr[3].B = 0xFF;
                welColr[4].ID = 1; welColr[4].R = 0xFF; welColr[4].G = 0x00; welColr[4].B = 0xFF;

                welColr[5].ID = 0; welColr[0].R = 0x00; welColr[0].G = 0x00; welColr[0].B = 0x00;
                welColr[6].ID = 0; welColr[0].R = 0x00; welColr[0].G = 0x00; welColr[0].B = 0x00;
                welColr[7].ID = 0; welColr[0].R = 0x00; welColr[0].G = 0x00; welColr[0].B = 0x00;

                for (int i = 9; i < 16; i++)
                {
                    welColr[i].ID = 0; welColr[i].R = 0; welColr[i].G = 0; welColr[i].B = 0;
                }
                NativeMethod.GRGBDLL_iSetWelColor_14H(welColr);

            }
            else
            {

            }

        }
    }
}
