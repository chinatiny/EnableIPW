using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Win32;
using System.Management;
using SimpleWifi;

namespace IPW
{
    class Program
    {
        static Wifi wifi;
        static void EnableInternet(bool enable)
        {
            var wmiQuery = new SelectQuery("SELECT * FROM Win32_NetworkAdapter WHERE NetConnectionId != NULL");
            var searchProcedure = new ManagementObjectSearcher(wmiQuery);
            foreach (ManagementObject item in searchProcedure.Get())
            {
                if (((string)item["NetConnectionId"]) == "ローカル エリア接続")
                {
                    item.InvokeMethod(enable ? "Enable" : "Disable", null);
                }
            }
        }

        static void EnableProxy(bool enable)
        {
            var registry = Registry.CurrentUser.OpenSubKey("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", true);

            ////disable proxy
            //registry.SetValue("ProxyServer", 1);

            //remove tik
            registry.SetValue("ProxyEnable", enable ? 1 : 0);

            //Proxy Status
            int proxyStatus = (int)registry.GetValue("ProxyEnable");
            Console.WriteLine(proxyStatus == 0 ? "disable" : "enable");
        }

        static void EnableWifi(bool enable)
        {
            wifi = new Wifi();
            var accessPoints = wifi.GetAccessPoints();
            var selectedAP = accessPoints.Single(ap => ap.Name == "NzXj AIBPXFJ");
            if (enable)
            {
                var authRequest = new AuthRequest(selectedAP);
                authRequest.Password = "greatminds";
                selectedAP.Connect(authRequest);
            }
            else
            {
                wifi.Disconnect();
                selectedAP.DeleteProfile();
            }
        }

        // Note: Run as Administrator
        static void Main(string[] args)
        {
            if (args.Length < 1) return;
            if (args[0] == "/13")
            {
                EnableInternet(false);
                EnableProxy(false);
                EnableWifi(true);
            }
            else
            {
                EnableWifi(false);
                EnableInternet(true);
                EnableProxy(true);
            }
        }
    }
}
